/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
//  hud_msg.cpp
//

#include "mp3.h" //AJH - Killar MP3

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "r_efx.h"
#include "rain.h"

//#include <gl/gl.h>
//#include <gl/glext.h>

#include "util.h"

#include "tri_rope.h"

#include "vgui_TeamFortressViewport.h"//m_pHud2
#include "vgui_hud.h"//CHud2
#include "gl_bored.h"
vec3_t FogColor;
float g_iFogColor[3];
float g_iStartDist;
float g_iEndDist;

extern cvar_t *cl_pulso;



#include "r_studioint.h"
extern engine_studio_api_t IEngineStudio;

#include "com_model.h"
#include "studio.h"

#include "StudioModelRenderer.h"
#include "GameStudioModelRenderer.h"
extern CGameStudioModelRenderer g_StudioRenderer;

#define MAX_CLIENTS 32

//extern BEAM *pBeam;
//extern BEAM *pBeam2;
extern rain_properties Rain;

/// USER-DEFINED SERVER MESSAGE HANDLERS
bool bIsMultiplayerGame ( void )
{
	return gEngfuncs.GetMaxClients() == 1 ? 0 : 1;
}

int CHud :: MsgFunc_ResetHUD(const char *pszName, int iSize, void *pbuf )
{
	ASSERT( iSize == 0 );

	// clear all hud data
	HUDLIST *pList = m_pHudList;

	while ( pList )
	{
		if ( pList->p )
			pList->p->Reset();
		pList = pList->pNext;
	}

	// reset sensitivity
	m_flMouseSensitivity = 0;

	// reset concussion effect
	m_iConcussionEffect = 0;

    g_iStartDist = 0.0; 
    g_iEndDist = 0.0;

	//ryo
//	gRopeRender.StartRenderer();

	return 1;
}

void CAM_ToFirstPerson(void);

void CHud :: MsgFunc_ViewMode( const char *pszName, int iSize, void *pbuf )
{
	CAM_ToFirstPerson();
}

//extern bool g_blReplaceTexture;

void CHud :: MsgFunc_InitHUD( const char *pszName, int iSize, void *pbuf )
{
	m_iSkyMode = SKY_OFF; //LRC

	// prepare all hud data
	HUDLIST *pList = m_pHudList;

	while (pList)
	{
		if ( pList->p )
			pList->p->InitHUDData();
		pList = pList->pNext;
	}
		

	//reset ropes
	gRopeRender.ResetRopes();

	//reset overlays
	if( gViewPort && gViewPort->m_pHud2 )
	gViewPort->m_pHud2->ResetOverlays();

//	g_blReplaceTexture = true;
	//Shader Water

	//Remove any shaders already loaded
	gTexture.KillTextures();
	g_CGShader.RemoveAll();

	//Setup shader info
	g_CGShader.SetShaderEnv();

	//Load the shader data
	g_Effects.LoadData();

	gHUD.m_iRoundtime = -1;

//|SP|
	//Probably not a good place to put this.
//	pBeam = pBeam2 = NULL;
}

int CHud :: MsgFunc_SetFog( const char *pszName, int iSize, void *pbuf )
{
    BEGIN_READ( pbuf, iSize );
    FogColor.x = TransformColor ( READ_SHORT() );
    FogColor.y = TransformColor ( READ_SHORT() );
    FogColor.z = TransformColor ( READ_SHORT() );
    g_iStartDist = READ_SHORT();
    g_iEndDist = READ_SHORT();

    return 1;
}

int CHud :: MsgFunc_GameMode(const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	m_Teamplay = READ_BYTE();

	return 1;
}


int CHud :: MsgFunc_Damage(const char *pszName, int iSize, void *pbuf )
{
	int		armor, blood;
	Vector	from;
	int		i;
	float	count;
	
	BEGIN_READ( pbuf, iSize );
	armor = READ_BYTE();
	blood = READ_BYTE();

	for (i=0 ; i<3 ; i++)
		from[i] = READ_COORD();

	count = (blood * 0.5) + (armor * 0.5);

	if (count < 10)
		count = 10;

	// TODO: kick viewangles,  show damage visually

	return 1;
}
int CHud :: MsgFunc_Clcommand( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	const char *tmpcmd2 = READ_STRING();
	int sayteamno		= READ_BYTE();

	char tmpcmd3[512];

	if (sayteamno == 0)
		sprintf(tmpcmd3, "say_team %s", CHudTextMessage::BufferedLocaliseTextString( tmpcmd2 ));
	else
		sprintf(tmpcmd3, "%s", tmpcmd2);

	ClientCmd(tmpcmd3);

	return 1;
}
int CHud :: MsgFunc_Concuss( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	m_iConcussionEffect = READ_BYTE();
/*
	if (m_iConcussionEffect)
	{
		this->m_StatusIcons.EnableIcon("dmg_concuss",255,160,0);
	}
	else
	{
		this->m_StatusIcons.DisableIcon("dmg_concuss");
	}
*/
	return 1;
}
int CHud :: MsgFunc_RainData( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
		Rain.dripsPerSecond =	READ_SHORT();
		Rain.distFromPlayer =	READ_COORD();
		Rain.windX =			READ_COORD();
		Rain.windY =			READ_COORD();
		Rain.randX =			READ_COORD();
		Rain.randY =			READ_COORD();
		Rain.weatherMode =		READ_SHORT();
		Rain.globalHeight =		READ_COORD();
	return 1;
}
int CHud :: MsgFunc_PlayMP3( const char *pszName, int iSize, void *pbuf ) //AJH -Killar MP3
{
	BEGIN_READ( pbuf, iSize );

	gMP3.PlayMP3( READ_STRING(), -1 );

	return 1;
}
void CHud :: MsgFunc_SetBody( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	gHUD.m_iBody = READ_BYTE();
	cl_entity_s *view = gEngfuncs.GetViewModel();
	view->curstate.body = gHUD.m_iBody;
	//gEngfuncs.pfnWeaponAnim( 2, gHUD.m_iBody );//UNDONE: custom frame for view model don't working
}

void CHud :: MsgFunc_SetSkin( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	gHUD.m_iSkin = READ_BYTE();
	cl_entity_s *view = gEngfuncs.GetViewModel();
	view->curstate.skin = gHUD.m_iSkin;
}

//sky sys
void CHud :: MsgFunc_AddShine( const char *pszName, int iSize, void *pbuf )
{
//	CONPRINT("MSG:AddShine");
	BEGIN_READ( pbuf, iSize );

	float fScale = READ_BYTE();
	float fAlpha = READ_BYTE()/255.0;

/*	float fMinX = READ_COORD();
	float fMaxX = READ_COORD();
	float fMinY = READ_COORD();
	float fMaxY = READ_COORD();
*/

	// read the rest of the msg in
	float vAbsMax_x = READ_COORD();
	float vAbsMax_y = READ_COORD();
	float vAbsMax_z = READ_COORD();
	float vAbsMin_x = READ_COORD();
	float vAbsMin_y = READ_COORD();
	float vAbsMin_z = READ_COORD();

//	float fZ = READ_COORD();

	char *szSprite = READ_STRING();

	gEngfuncs.Con_Printf("fScale %f \n", fScale);

//	CShinySurface *pSurface = new CShinySurface(fScale, fAlpha, fMinX, fMaxX, fMinY, fMaxY, fZ, szSprite);
	CShinySurface *pSurface = new CShinySurface(fScale, fAlpha, vAbsMax_x, vAbsMax_y, vAbsMax_z, vAbsMin_x, vAbsMin_y, vAbsMin_z, szSprite);

	pSurface->m_pNext = m_pShinySurface;
	m_pShinySurface = pSurface;
}

void CHud :: MsgFunc_SetSky( const char *pszName, int iSize, void *pbuf )
{
//	CONPRINT("MSG:SetSky");
	BEGIN_READ( pbuf, iSize );

	m_iSkyMode = READ_BYTE();
	m_vecSkyPos.x = READ_COORD();
	m_vecSkyPos.y = READ_COORD();
	m_vecSkyPos.z = READ_COORD();
//	m_iSkyScale = READ_BYTE();
}

extern void DrawQuad(int width, int height, int ofsX = 0, int ofsY = 0);

int CHud :: MsgFunc_AddELight( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	dlight_t *dl = gEngfuncs.pEfxAPI->CL_AllocElight( READ_SHORT() );

	int bELightActive = READ_BYTE();
	if (!bELightActive)
	{
		dl->die = gEngfuncs.GetClientTime();
	}
	else
	{
		dl->die = gEngfuncs.GetClientTime() + 1E6;

		dl->origin[0] = READ_COORD();
		dl->origin[1] = READ_COORD();
		dl->origin[2] = READ_COORD();
		dl->radius = READ_COORD();
		dl->color.r = READ_BYTE();
		dl->color.g = READ_BYTE();
		dl->color.b = READ_BYTE();
	}
	return 1;
}

void CHud :: MsgFunc_AddRope( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	vec3_t start_source;
	vec3_t end_source;

	start_source.x = READ_COORD();
	start_source.y = READ_COORD();
	start_source.z = READ_COORD();

	end_source.x = READ_COORD();
	end_source.y = READ_COORD();
	end_source.z = READ_COORD();

	char *datafile = READ_STRING();

	gRopeRender.CreateRope( datafile, start_source, end_source );
}