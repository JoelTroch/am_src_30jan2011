/***
*
*		   °°
*			 °    °°°°°°°
*			° °   °  °  °
*		   °   °  °  °  °
*		  °     ° °  °  °
*	   HALF-LIFE: ARRANGEMENT
*
*	AR Software (c) 2004-2007. ArrangeMent, S2P Physics, Spirit Of Half-Life and their
*	logos are the property of their respective owners.
*
*	You should have received a copy of the Developers Readme File
*   along with Arrange Mode's Source Code for Half-Life. If you
*	are going to copy, modify, translate or distribute this file
*	you should agree whit the terms of Developers Readme File.
*
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*
*	This product includes information related to Half-Life 2 licensed from Valve 
*	(c) 2004. 
*
*	All Rights Reserved.
*
*	Modifications by SysOp (sysop_axis@hotmail).
*
***/

//===================
//| Open Graphics Library | Wrapper
// Purpose: Loads and process all OGL effects on game
// TO DO: blur, img holds, change colors, brigthness, contrasts, and so on.
// For now it does nothing
//===================


//From tri.cpp...
#include "hud.h"
#include "cl_util.h"

// Triangle rendering apis are in gEngfuncs.pTriAPI

#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"
#include "triangleapi.h"
//End tri.cpp

#include <windows.h>
//#include <gl/gl.h>
//#include <gl/glext.h>
#include <cg/cg.h>
#include <cg/cgGL.h>
#include "r_studioint.h"

#define DLLEXPORT __declspec( dllexport )
#define GL_TEXTURE_RECTANGLENV 0x84F5

extern engine_studio_api_t IEngineStudio;

PFNGLACTIVETEXTUREARBPROC glActiveTextureARB2 = NULL;
PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB2 = NULL;

extern CGcontext g_cgContext;
extern CGprofile g_cgVertProfile;
extern CGprofile g_cgFragProfile;

extern CGprogram g_cgVP_GlowBlur;
extern CGprogram g_cgFP_GlowBlur;

extern CGparameter g_cgpVP0_ModelViewMatrix;
extern CGparameter g_cgpVP1_ModelViewMatrix;
extern CGparameter g_cgpVP1_XOffset;
extern CGparameter g_cgpVP1_YOffset;
extern CGparameter g_cgpVP2_ModelViewMatrix;

extern unsigned int g_uiSceneTex;
extern unsigned int g_uiBlurTex;

//================================
// B&W Shader
//================================
unsigned int fp_screen_shader;

static char fp_screen_source[] = 
"!!ARBfp1.0\n"
"OPTION ARB_precision_hint_fastest;\n"
"PARAM c0 = {0.32000000, 0.59000000, 0.090000000, 0};\n"
"TEMP R0;\n"
"TXP R0, fragment.texcoord[0], texture[0], 2D;\n"
"DP3 result.color, c0, R0;\n"
"MOV result.color.w, fragment.color.w;\n"
"END";

void InitScreenBlur(void);

void RenderScreenBlur(void);

bool LoadProgram(CGprogram* pDest, CGprofile profile, const char* szFile);

void DrawAQuad(int width, int height, int ofsX = 0, int ofsY = 0)
{
     glBegin(GL_QUADS);

     glTexCoord2f(ofsX,ofsY);
     glVertex3f(0, 1, -1);
     glTexCoord2f(ofsX,height+ofsY);
     glVertex3f(0, 0, -1);
     glTexCoord2f(width+ofsX,height+ofsY);
     glVertex3f(1, 0, -1);
     glTexCoord2f(width+ofsX,ofsY);
     glVertex3f(1, 1, -1);

     glEnd();
}

void RenderScreenBlur()
{
 	if (IEngineStudio.IsHardware() != 1)
		return;

	if (CVAR_GET_FLOAT("r_glow") == 0)
		return;

	if (CVAR_GET_FLOAT("r_glow") == 1){
		RenderScreenBlur();
	}
}

void InitScreenBlur()
{
	if (IEngineStudio.IsHardware() != 1)
		return;

	if (CVAR_GET_FLOAT("r_glow") == 0)
		return;

	if (CVAR_GET_FLOAT("r_glow") == 1){
		InitScreenBlur();
	}
}

inline bool LoadProgram(CGprogram* pDest, CGprofile profile, const char* szFile)
{
     const char* szGameDir = gEngfuncs.pfnGetGameDirectory();
     char file[512];
     sprintf(file, "%s/%s", szGameDir, szFile);

     *pDest = cgCreateProgramFromFile(g_cgContext, CG_SOURCE, file, profile, "main", 0);
     if (!(*pDest)) {
          MessageBox(NULL, cgGetErrorString(cgGetError()), NULL, NULL);
          return false;
     }

     cgGLLoadProgram(*pDest);

     return false;
}

void BlurScene(unsigned int uiSrcTex, unsigned int uiTargetTex, int srcTexWidth, int srcTexHeight, int destTexWidth, int destTexHeight, float xofs, float yofs)
{
	cgGLBindProgram(g_cgVP_GlowBlur);
	cgGLBindProgram(g_cgFP_GlowBlur);

	glActiveTextureARB2(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_RECTANGLE_NV);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, uiSrcTex);

	glActiveTextureARB2(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_RECTANGLE_NV);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, uiSrcTex);

	glActiveTextureARB2(GL_TEXTURE2_ARB);
	glEnable(GL_TEXTURE_RECTANGLE_NV);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, uiSrcTex);

	glActiveTextureARB2(GL_TEXTURE3_ARB);
	glEnable(GL_TEXTURE_RECTANGLE_NV);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, uiSrcTex);

	cgGLSetParameter1f(g_cgpVP1_XOffset, xofs);
	cgGLSetParameter1f(g_cgpVP1_YOffset, yofs);

	glViewport(0, 0, destTexWidth, destTexHeight);

	DrawAQuad(srcTexWidth, srcTexHeight);

	glBindTexture(GL_TEXTURE_RECTANGLE_NV, uiTargetTex);
	glCopyTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGB, 0, 0, destTexWidth, destTexHeight, 0);
}
