//========= Copyright � 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "hud.h"
#include "cl_util.h"

// Triangle rendering apis are in gEngfuncs.pTriAPI

#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"
#include "triangleapi.h"

#include "rain.h" 
#include "com_model.h"
#include "studio_util.h"

//***********************
#include <stdio.h>
#include <windows.h>
#include <gl/gl.h>

#include "pys_rope.h"
#include "particle_header.h"

#include "com_weapons.h"

#include "pm_defs.h"
#include "pmtrace.h"

// buz start

#include "windows.h"
//#include "gl/gl.h"
//#include "gl/glext.h"

#include "r_studioint.h"
extern engine_studio_api_t IEngineStudio;

#include "com_model.h"
#include "studio.h"

#include "StudioModelRenderer.h"
#include "GameStudioModelRenderer.h"


#include "triangleapi.h"
#include "pm_defs.h"
#include "event_api.h"
#include "pmtrace.h"

#include "gl_bored.h"

#include "r_video.h" 

extern CGameStudioModelRenderer g_StudioRenderer;

#define SURF_PLANEBACK		2
#define SURF_DRAWSKY		4
#define SURF_DRAWSPRITE		8
#define SURF_DRAWTURB		0x10
#define SURF_DRAWTILED		0x20
#define SURF_DRAWBACKGROUND	0x40
#define SURF_UNDERWATER		0x80
#define SURF_DONTWARP		0x100
#define BACKFACE_EPSILON	0.01

void VectorAngles( const float *forward, float *angles );

#define DLLEXPORT __declspec( dllexport )

extern "C"
{
	void DLLEXPORT HUD_DrawNormalTriangles( void );
	void DLLEXPORT HUD_DrawTransparentTriangles( void );
};

extern float g_iFogColor[4]; 
extern float g_iStartDist; 
extern float g_iEndDist; 
extern int g_iWaterLevel;
extern vec3_t FogColor;

extern engine_studio_api_t IEngineStudio;

extern int g_iWaterLevel;
extern vec3_t v_origin;

int UseTexture(HSPRITE &hsprSpr, char * str)
{
	if (hsprSpr == 0)
	{
		char sz[256];
		sprintf( sz, str );
		hsprSpr = SPR_Load( sz );
	}

	return gEngfuncs.pTriAPI->SpriteTexture( (struct model_s *)gEngfuncs.GetSpritePointer( hsprSpr ), 0 );
}


/*
====================
buz:
Orthogonal polygons
====================
*/

// helper functions

void OrthoQuad(int x1, int y1, int x2, int y2)
{
	gEngfuncs.pTriAPI->Begin(TRI_QUADS); //start our quad

		gEngfuncs.pTriAPI->TexCoord2f(0.01, 0.01);
		gEngfuncs.pTriAPI->Vertex3f(x1, y1, 0);

		gEngfuncs.pTriAPI->TexCoord2f(0.01, 0.99);
		gEngfuncs.pTriAPI->Vertex3f(x1, y2, 0);

		gEngfuncs.pTriAPI->TexCoord2f(0.99, 0.99);
		gEngfuncs.pTriAPI->Vertex3f(x2, y2, 0);

		gEngfuncs.pTriAPI->TexCoord2f(0.99f, 0.01);
		gEngfuncs.pTriAPI->Vertex3f(x2, y1, 0);

	gEngfuncs.pTriAPI->End(); //end our list of vertexes
}
//-----------------------------------------------------

void SetPoint( float x, float y, float z, float (*matrix)[4])
{
	vec3_t point, result;
	point[0] = x;
	point[1] = y;
	point[2] = z;

	VectorTransform(point, matrix, result);

	gEngfuncs.pTriAPI->Vertex3f(result[0], result[1], result[2]);
}

//LRC - code for CShinySurface, declared in hud.h
//CShinySurface::CShinySurface( float fScale, float fAlpha, float fMinX, float fMaxX, float fMinY, float fMaxY, float fZ, char *szSprite)
CShinySurface::CShinySurface( float fScale, float fAlpha, float vAbsMax_x, float vAbsMax_y, float vAbsMax_z, float vAbsMin_x, float vAbsMin_y, float vAbsMin_z, char *szSprite)
{
	m_fScale = fScale; 
	m_fAlpha = fAlpha;

	Max_x = vAbsMax_x; 
	Max_y = vAbsMax_y;
	Max_z = vAbsMax_z; 

	Min_x = vAbsMin_x;
	Min_y = vAbsMin_y;
	Min_z = vAbsMin_z;
/*
	//for some reason, the ABS are expanded... by 2... lets fix this
	Max_x -= 1.99; 
	Max_y -= 1.99;
	Max_z -= 1.99; 

	Min_x += 1.99;
	Min_y += 1.99;	
	Min_z += 1.99;
*/
	gEngfuncs.Con_Printf("minx %f, maxx %f, miny %f, maxy %f,minz %f, maxz %f\n", Min_x, Max_x, Min_y, Max_y, Min_z, Max_z);

	m_hsprSprite = 0;
	sprintf( m_szSprite, szSprite );
	m_pNext = NULL;
}

CShinySurface::~CShinySurface()
{
	if (m_pNext)
		delete m_pNext;
}

void CShinySurface::DrawAll(const vec3_t &org)
{
	gEngfuncs.pTriAPI->RenderMode( kRenderTransAdd ); //kRenderTransTexture );
	gEngfuncs.pTriAPI->CullFace( TRI_NONE );

	for(CShinySurface *pCurrent = this; pCurrent; pCurrent = pCurrent->m_pNext)
	{
		pCurrent->Draw(org);
	}

	gEngfuncs.pTriAPI->RenderMode( kRenderNormal );
}

void CShinySurface::Draw(const vec3_t &org)
{
	// add 5 to the view height, so that we don't get an ugly repeating texture as it approaches 0.
//	float fHeight = org.z - Max_z + 5;

	// only visible from above
//	if (fHeight < 5) return;

	// fade out if we're really close to the surface, so they don't see an ugly repeating texture
//	if (fHeight < 15)
//		gEngfuncs.pTriAPI->Color4f( 1.0, 1.0, 1.0, (fHeight - 5)*0.1*m_fAlpha );
//	else
		gEngfuncs.pTriAPI->Color4f( 1.0, 1.0, 1.0, m_fAlpha );

	// check whether the texture is valid
	if (!UseTexture(m_hsprSprite, m_szSprite)) return;

//	gEngfuncs.Con_Printf("minx %f, maxx %f, miny %f, maxy %f,minz %f, maxz %f\n", Min_x, Max_x, Min_y, Max_y, Min_z, Max_z);

//	float fFactor = 1/(m_fScale*fHeight);
	float fFactor = 0.1/m_fScale;


//	gEngfuncs.Con_Printf("m_fScale %f \n", m_fScale);


	float fMaxTX = (org.x - Max_x)*fFactor;
	float fMaxTY = (org.y - Max_y)*fFactor;
	float fMaxTZ = (org.z - Max_z)*fFactor;

	float fMinTX = (org.x - Min_x)*fFactor;
	float fMinTY = (org.y - Min_y)*fFactor;
	float fMinTZ = (org.z - Min_z)*fFactor;


//top
 	gEngfuncs.pTriAPI->Begin( TRI_QUADS );
		gEngfuncs.pTriAPI->TexCoord2f(	fMinTX,		fMinTY					);
		gEngfuncs.pTriAPI->Vertex3f  (	Min_x,	Min_y,	Max_z +0.01	); // add 0.02 to avoid z-buffer problems
		gEngfuncs.pTriAPI->TexCoord2f(	fMinTX,		fMaxTY					);
		gEngfuncs.pTriAPI->Vertex3f  (	Min_x,	Max_y,	Max_z	+0.01	);
		gEngfuncs.pTriAPI->TexCoord2f(	fMaxTX,		fMaxTY					);
		gEngfuncs.pTriAPI->Vertex3f  (	Max_x,	Max_y,	Max_z	+0.01	);
		gEngfuncs.pTriAPI->TexCoord2f(	fMaxTX,		fMinTY					);
		gEngfuncs.pTriAPI->Vertex3f  (	Max_x,	Min_y,	Max_z	+0.01	);
	gEngfuncs.pTriAPI->End();
	
//bottom
	gEngfuncs.pTriAPI->Begin( TRI_QUADS );
		gEngfuncs.pTriAPI->TexCoord2f(	fMinTX,		fMinTY				);
		gEngfuncs.pTriAPI->Vertex3f  (	Min_x,	Min_y,	Min_z 	-0.01		); // add 0.02 to avoid z-buffer problems
		gEngfuncs.pTriAPI->TexCoord2f(	fMinTX,		fMaxTY					);
		gEngfuncs.pTriAPI->Vertex3f  (	Min_x,	Max_y,	Min_z	-0.01	);
		gEngfuncs.pTriAPI->TexCoord2f(	fMaxTX,		fMaxTY					);
		gEngfuncs.pTriAPI->Vertex3f  (	Max_x,	Max_y,	Min_z	-0.01	);
		gEngfuncs.pTriAPI->TexCoord2f(	fMaxTX,		fMinTY					);
		gEngfuncs.pTriAPI->Vertex3f  (	Max_x,	Min_y,	Min_z	-0.01	);
	gEngfuncs.pTriAPI->End();


 	gEngfuncs.pTriAPI->Begin( TRI_QUADS );//right
		gEngfuncs.pTriAPI->TexCoord2f(	fMinTX,		fMinTY					);
		gEngfuncs.pTriAPI->Vertex3f  (	Min_x,	Max_y+0.01	,	Max_z  );//Min_y
		gEngfuncs.pTriAPI->TexCoord2f(	fMinTX,		fMaxTY					);

		gEngfuncs.pTriAPI->Vertex3f  (	Min_x,	Max_y+0.01	,	Min_z 	);
		gEngfuncs.pTriAPI->TexCoord2f(	fMaxTX,		fMaxTY						);
		gEngfuncs.pTriAPI->Vertex3f  (	Max_x,	Max_y+0.01	,	Min_z );

		gEngfuncs.pTriAPI->TexCoord2f(	fMaxTX,		fMinTY						);
		gEngfuncs.pTriAPI->Vertex3f  (	Max_x,	Max_y+0.01	,	Max_z );//Min_y
	gEngfuncs.pTriAPI->End();
	
 	gEngfuncs.pTriAPI->Begin( TRI_QUADS );//left
		gEngfuncs.pTriAPI->TexCoord2f(	fMinTX,		fMinTY						);
		gEngfuncs.pTriAPI->Vertex3f  (	Min_x,	Min_y-0.01,	Min_z); // add 0.02 to avoid z-buffer problems
		gEngfuncs.pTriAPI->TexCoord2f(	fMinTX,		fMaxTY						);
		gEngfuncs.pTriAPI->Vertex3f  (	Min_x,	Min_y-0.01,	Max_z	);
		gEngfuncs.pTriAPI->TexCoord2f(	fMaxTX,		fMaxTY						);
		gEngfuncs.pTriAPI->Vertex3f  (	Max_x,	Min_y-0.01,	Max_z 	);
		gEngfuncs.pTriAPI->TexCoord2f(	fMaxTX,		fMinTY						);
		gEngfuncs.pTriAPI->Vertex3f  (	Max_x,	Min_y-0.01,	Min_z);
	gEngfuncs.pTriAPI->End();




 	gEngfuncs.pTriAPI->Begin( TRI_QUADS );//front
		gEngfuncs.pTriAPI->TexCoord2f(	fMinTX,		fMinTY						);
		gEngfuncs.pTriAPI->Vertex3f  (	Min_x-0.01,	Min_y,	Max_z 	); // add 0.02 to avoid z-buffer problems
		gEngfuncs.pTriAPI->TexCoord2f(	fMinTX,		fMaxTY						);
		gEngfuncs.pTriAPI->Vertex3f  (	Min_x-0.01,	Max_y,	Max_z 	);
		gEngfuncs.pTriAPI->TexCoord2f(	fMaxTX,		fMaxTY						);

		gEngfuncs.pTriAPI->Vertex3f  (	Min_x-0.01,	Max_y,	Min_z	);
		gEngfuncs.pTriAPI->TexCoord2f(	fMaxTX,		fMinTY						);
		gEngfuncs.pTriAPI->Vertex3f  (	Min_x-0.01,	Min_y,	Min_z	);
	gEngfuncs.pTriAPI->End();

	gEngfuncs.pTriAPI->Begin( TRI_QUADS );//front
		gEngfuncs.pTriAPI->TexCoord2f(	fMinTX,		fMinTY						);
		gEngfuncs.pTriAPI->Vertex3f  (	Max_x+0.01,	Min_y,	Min_z	); // add 0.02 to avoid z-buffer problems
		gEngfuncs.pTriAPI->TexCoord2f(	fMinTX,		fMaxTY						);
		gEngfuncs.pTriAPI->Vertex3f  (	Max_x+0.01,	Max_y,	Min_z	);
		gEngfuncs.pTriAPI->TexCoord2f(	fMaxTX,		fMaxTY						);

		gEngfuncs.pTriAPI->Vertex3f  (	Max_x+0.01,	Max_y,	Max_z);
		gEngfuncs.pTriAPI->TexCoord2f(	fMaxTX,		fMinTY						);
		gEngfuncs.pTriAPI->Vertex3f  (	Max_x+0.01,	Min_y,	Max_z	);
	gEngfuncs.pTriAPI->End();
}

//
//-----------------------------------------------------
//

//#define TEST_IT
#if defined( TEST_IT )

/*
=================
Draw_Triangles

Example routine.  Draws a sprite offset from the player origin.
=================
*/
void Draw_Triangles( void )
{
	cl_entity_t *player;
	vec3_t org;

	// Load it up with some bogus data
	player = gEngfuncs.GetLocalPlayer();
	if ( !player )
		return;

	org = player->origin;

	org.x += 50;
	org.y += 50;

	if (gHUD.m_hsprCursor == 0)
	{
		char sz[256];
		sprintf( sz, "sprites/cursor.spr" );
		gHUD.m_hsprCursor = SPR_Load( sz );
	}

	if ( !gEngfuncs.pTriAPI->SpriteTexture( (struct model_s *)gEngfuncs.GetSpritePointer( gHUD.m_hsprCursor ), 0 ))
	{
		return;
	}
	
	// Create a triangle, sigh
	gEngfuncs.pTriAPI->RenderMode( kRenderNormal );
	gEngfuncs.pTriAPI->CullFace( TRI_NONE );
	gEngfuncs.pTriAPI->Begin( TRI_QUADS );
	// Overload p->color with index into tracer palette, p->packedColor with brightness
	gEngfuncs.pTriAPI->Color4f( 1.0, 1.0, 1.0, 1.0 );
	// UNDONE: This gouraud shading causes tracers to disappear on some cards (permedia2)
	gEngfuncs.pTriAPI->Brightness( 1 );
	gEngfuncs.pTriAPI->TexCoord2f( 0, 0 );
	gEngfuncs.pTriAPI->Vertex3f( org.x, org.y, org.z );

	gEngfuncs.pTriAPI->Brightness( 1 );
	gEngfuncs.pTriAPI->TexCoord2f( 0, 1 );
	gEngfuncs.pTriAPI->Vertex3f( org.x, org.y + 50, org.z );

	gEngfuncs.pTriAPI->Brightness( 1 );
	gEngfuncs.pTriAPI->TexCoord2f( 1, 1 );
	gEngfuncs.pTriAPI->Vertex3f( org.x + 50, org.y + 50, org.z );

	gEngfuncs.pTriAPI->Brightness( 1 );
	gEngfuncs.pTriAPI->TexCoord2f( 1, 0 );
	gEngfuncs.pTriAPI->Vertex3f( org.x + 50, org.y, org.z );

	gEngfuncs.pTriAPI->End();
	gEngfuncs.pTriAPI->RenderMode( kRenderNormal );
}
#endif

/*
void BlackFog ( void )
{
    static float fColorBlack[3] = {0,0,0};
    bool bFog = g_iStartDist > 0 && g_iEndDist > 0;
    if (bFog)
        gEngfuncs.pTriAPI->Fog ( fColorBlack, g_iStartDist, g_iEndDist, bFog );
    else
        gEngfuncs.pTriAPI->Fog ( g_iFogColor, g_iStartDist, g_iEndDist, bFog );
}
*/
void BlackFog ( bool GL = true )
{
	static float fColorBlack[3] = {0,0,0};
	bool bFog = g_iStartDist > 0 && g_iEndDist > 0;

	if (bFog)
	{
		if ( !GL /*|| gHUD.SteamFog->value >= 1*/ )
		{
			gEngfuncs.pTriAPI->Fog ( fColorBlack, g_iStartDist, g_iEndDist, bFog );
		}
		else
		{
			glEnable(GL_FOG);
			glFogi (GL_FOG_MODE, GL_LINEAR);
			glFogfv (GL_FOG_COLOR, fColorBlack);
			glFogf (GL_FOG_DENSITY, 1.0f);
			glHint (GL_FOG_HINT, GL_NICEST);
			glFogf (GL_FOG_START, g_iStartDist);
			glFogf (GL_FOG_END, g_iEndDist);
		}
	}
	else
	{
		if ( !GL /*|| gHUD.SteamFog->value >= 1*/ )
		{
			gEngfuncs.pTriAPI->Fog ( fColorBlack, g_iStartDist, g_iEndDist, bFog );
		}
		else
		{
			glDisable( GL_FOG );
		}
	}
}

void RenderFog ( void ) 
{
	float g_iFogColor[4] = { FogColor.x, FogColor.y, FogColor.z, 1.0 };
	bool bFog = g_iStartDist > 0 && g_iEndDist > 0;
	if ( bFog )
	{
		if ( IEngineStudio.IsHardware() == 2 )
		{
			gEngfuncs.pTriAPI->Fog ( g_iFogColor, g_iStartDist, g_iEndDist, bFog );
		}
		else if( IEngineStudio.IsHardware() == 1 )
		{
			glEnable(GL_FOG);
			glFogi (GL_FOG_MODE, GL_LINEAR);
			glFogfv (GL_FOG_COLOR, g_iFogColor);
			glFogf (GL_FOG_DENSITY, 1.0f);
			glHint (GL_FOG_HINT, GL_NICEST);
			glFogf (GL_FOG_START, g_iStartDist);
			glFogf (GL_FOG_END, g_iEndDist);
		}
	}
	else
	{
		if ( IEngineStudio.IsHardware() == 2 )
		{
			gEngfuncs.pTriAPI->Fog ( g_iFogColor, g_iStartDist, g_iEndDist, bFog );
		}
		else if( IEngineStudio.IsHardware() == 1 )
		{
			glDisable(GL_FOG);
		}
	}
}
/*
void RenderFog ( void ) 
{
    float g_iFogColor[4] = { FogColor.x, FogColor.y, FogColor.z, 1.0 };
    bool bFog = g_iStartDist > 0 && g_iEndDist > 0;

    if ( bFog )
    {
        if ( IEngineStudio.IsHardware() == 2 )
        {
            gEngfuncs.pTriAPI->Fog ( g_iFogColor, g_iStartDist, g_iEndDist, bFog );
        }
        else if ( IEngineStudio.IsHardware() == 1 )
        {
            glEnable(GL_FOG);
            glFogi (GL_FOG_MODE, GL_LINEAR);
            glFogfv (GL_FOG_COLOR, g_iFogColor);
            glFogf (GL_FOG_DENSITY, 1.0f);
            glHint (GL_FOG_HINT, GL_DONT_CARE);
            glFogf (GL_FOG_START, g_iStartDist);
            glFogf (GL_FOG_END, g_iEndDist);
        }
    }
}
*/
/*
=================================
DrawRain

draw raindrips and snowflakes
=================================
*/
extern cl_drip FirstChainDrip;
extern rain_properties Rain;

void DrawRain( void )
{
	if (FirstChainDrip.p_Next == NULL)
		return; // no drips to draw

	if (gHUD.ProcessWeather->value == 0.0)
		return; // no drips to draw

	HSPRITE hsprTexture;
	const model_s *pTexture;
	float visibleHeight = Rain.globalHeight - SNOWFADEDIST;

	if (Rain.weatherMode == 0)
		hsprTexture = LoadSprite( "sprites/hi_rain.spr" ); // load rain sprite
	else if (Rain.weatherMode == 2)
		hsprTexture = LoadSprite( "sprites/fx_dust.spr" ); // load dust sprite
	else
		hsprTexture = LoadSprite( "sprites/snowflake.spr" ); // load snow sprite

	// usual triapi stuff
	pTexture = gEngfuncs.GetSpritePointer( hsprTexture );
	gEngfuncs.pTriAPI->SpriteTexture( (struct model_s *)pTexture, 0 );
	gEngfuncs.pTriAPI->RenderMode( kRenderTransAdd );
	gEngfuncs.pTriAPI->CullFace( TRI_NONE );
	
	// go through drips list
	cl_drip* Drip = FirstChainDrip.p_Next;
	cl_entity_t *player = gEngfuncs.GetLocalPlayer();

	if ( Rain.weatherMode == 0 ) // draw rain
	{
		while (Drip != NULL)
		{
			cl_drip* nextdDrip = Drip->p_Next;
					
			Vector2D toPlayer; 
			toPlayer.x = player->origin[0] - Drip->origin[0];
			toPlayer.y = player->origin[1] - Drip->origin[1];
			toPlayer = toPlayer.Normalize();
	
			toPlayer.x *= DRIP_SPRITE_HALFWIDTH;
			toPlayer.y *= DRIP_SPRITE_HALFWIDTH;

			float shiftX = (Drip->xDelta / DRIPSPEED) * DRIP_SPRITE_HALFHEIGHT;
			float shiftY = (Drip->yDelta / DRIPSPEED) * DRIP_SPRITE_HALFHEIGHT;

		// --- draw triangle --------------------------
			gEngfuncs.pTriAPI->Color4f( 1.0, 1.0, 1.0, Drip->alpha );
			gEngfuncs.pTriAPI->Begin( TRI_TRIANGLES );

				gEngfuncs.pTriAPI->TexCoord2f( 0, 0 );
				gEngfuncs.pTriAPI->Vertex3f( Drip->origin[0]-toPlayer.y - shiftX, Drip->origin[1]+toPlayer.x - shiftY,Drip->origin[2] + DRIP_SPRITE_HALFHEIGHT );

				gEngfuncs.pTriAPI->TexCoord2f( 0.5, 1 );
				gEngfuncs.pTriAPI->Vertex3f( Drip->origin[0] + shiftX, Drip->origin[1] + shiftY, Drip->origin[2]-DRIP_SPRITE_HALFHEIGHT );

				gEngfuncs.pTriAPI->TexCoord2f( 1, 0 );
				gEngfuncs.pTriAPI->Vertex3f( Drip->origin[0]+toPlayer.y - shiftX, Drip->origin[1]-toPlayer.x - shiftY, Drip->origin[2]+DRIP_SPRITE_HALFHEIGHT);
	
			gEngfuncs.pTriAPI->End();
		// --- draw triangle end ----------------------

			Drip = nextdDrip;
		}
	}
/////////////////
// SYS: 
// Dust mode
/////////////////
	else if ( Rain.weatherMode == 2 ) // draw dust
	{ 
		vec3_t normal;
		gEngfuncs.GetViewAngles((float*)normal);
	
		float matrix[3][4];
		AngleMatrix (normal, matrix);	// calc view matrix

		while (Drip != NULL)
		{
			cl_drip* nextdDrip = Drip->p_Next;

			matrix[0][3] = Drip->origin[0]; // write origin to matrix
			matrix[1][3] = Drip->origin[1];
			matrix[2][3] = Drip->origin[2];

			// apply start fading effect
			float alpha = (Drip->origin[2] <= visibleHeight) ? Drip->alpha : ((Rain.globalHeight - Drip->origin[2]) / (float)SNOWFADEDIST) * Drip->alpha;
					
		// --- draw quad --------------------------
			gEngfuncs.pTriAPI->Color4f( 1.0, 1.0, 1.0, alpha );
			gEngfuncs.pTriAPI->Begin( TRI_QUADS );

				gEngfuncs.pTriAPI->TexCoord2f( 0, 0 );
				SetPoint(0, DUST_SPRITE_HALFSIZE ,DUST_SPRITE_HALFSIZE, matrix);

				gEngfuncs.pTriAPI->TexCoord2f( 0, 1 );
				SetPoint(0, DUST_SPRITE_HALFSIZE ,-DUST_SPRITE_HALFSIZE, matrix);

				gEngfuncs.pTriAPI->TexCoord2f( 1, 1 );
				SetPoint(0, -DUST_SPRITE_HALFSIZE ,-DUST_SPRITE_HALFSIZE, matrix);

				gEngfuncs.pTriAPI->TexCoord2f( 1, 0 );
				SetPoint(0, -DUST_SPRITE_HALFSIZE ,DUST_SPRITE_HALFSIZE, matrix);
				
			gEngfuncs.pTriAPI->End();
		// --- draw quad end ----------------------

			Drip = nextdDrip;
		}
	}
/////////////////
// SYS: 
// Dust mode
/////////////////
	else	// draw snow
	{ 
		vec3_t normal;
		gEngfuncs.GetViewAngles((float*)normal);
	
		float matrix[3][4];
		AngleMatrix (normal, matrix);	// calc view matrix

		while (Drip != NULL)
		{
			cl_drip* nextdDrip = Drip->p_Next;

			matrix[0][3] = Drip->origin[0]; // write origin to matrix
			matrix[1][3] = Drip->origin[1];
			matrix[2][3] = Drip->origin[2];

			// apply start fading effect
			float alpha = (Drip->origin[2] <= visibleHeight) ? Drip->alpha : ((Rain.globalHeight - Drip->origin[2]) / (float)SNOWFADEDIST) * Drip->alpha;
					
		// --- draw quad --------------------------
			gEngfuncs.pTriAPI->Color4f( 1.0, 1.0, 1.0, alpha );
			gEngfuncs.pTriAPI->Begin( TRI_QUADS );

				gEngfuncs.pTriAPI->TexCoord2f( 0, 0 );
				SetPoint(0, SNOW_SPRITE_HALFSIZE ,SNOW_SPRITE_HALFSIZE, matrix);

				gEngfuncs.pTriAPI->TexCoord2f( 0, 1 );
				SetPoint(0, SNOW_SPRITE_HALFSIZE ,-SNOW_SPRITE_HALFSIZE, matrix);

				gEngfuncs.pTriAPI->TexCoord2f( 1, 1 );
				SetPoint(0, -SNOW_SPRITE_HALFSIZE ,-SNOW_SPRITE_HALFSIZE, matrix);

				gEngfuncs.pTriAPI->TexCoord2f( 1, 0 );
				SetPoint(0, -SNOW_SPRITE_HALFSIZE ,SNOW_SPRITE_HALFSIZE, matrix);
				
			gEngfuncs.pTriAPI->End();
		// --- draw quad end ----------------------

			Drip = nextdDrip;
		}
	}
}

/*
=================================
DrawFXObjects
=================================
*/
extern cl_rainfx FirstChainFX;

void DrawFXObjects( void )
{
	if (FirstChainFX.p_Next == NULL)
		return; // no objects to draw

	float curtime = gEngfuncs.GetClientTime();

	// usual triapi stuff
	HSPRITE hsprTexture;
	const model_s *pTexture;
	hsprTexture = LoadSprite( "sprites/waterring.spr" ); // load water ring sprite
	pTexture = gEngfuncs.GetSpritePointer( hsprTexture );
	gEngfuncs.pTriAPI->SpriteTexture( (struct model_s *)pTexture, 0 );
	gEngfuncs.pTriAPI->RenderMode( kRenderTransAdd );
	gEngfuncs.pTriAPI->CullFace( TRI_NONE );
	
	// go through objects list
	cl_rainfx* curFX = FirstChainFX.p_Next;
	while (curFX != NULL)
	{
		cl_rainfx* nextFX = curFX->p_Next;
					
		// fadeout
		float alpha = ((curFX->birthTime + curFX->life - curtime) / curFX->life) * curFX->alpha;
		float size = (curtime - curFX->birthTime) * MAXRINGHALFSIZE;

		// --- draw quad --------------------------
		gEngfuncs.pTriAPI->Color4f( 1.0, 1.0, 1.0, alpha );
		gEngfuncs.pTriAPI->Begin( TRI_QUADS );

			gEngfuncs.pTriAPI->TexCoord2f( 0, 0 );
			gEngfuncs.pTriAPI->Vertex3f(curFX->origin[0] - size, curFX->origin[1] - size, curFX->origin[2]);

			gEngfuncs.pTriAPI->TexCoord2f( 0, 1 );
			gEngfuncs.pTriAPI->Vertex3f(curFX->origin[0] - size, curFX->origin[1] + size, curFX->origin[2]);

			gEngfuncs.pTriAPI->TexCoord2f( 1, 1 );
			gEngfuncs.pTriAPI->Vertex3f(curFX->origin[0] + size, curFX->origin[1] + size, curFX->origin[2]);

			gEngfuncs.pTriAPI->TexCoord2f( 1, 0 );
			gEngfuncs.pTriAPI->Vertex3f(curFX->origin[0] + size, curFX->origin[1] - size, curFX->origin[2]);
	
		gEngfuncs.pTriAPI->End();
		// --- draw quad end ----------------------

		curFX = nextFX;
	}
}

/*
=================
HUD_DrawNormalTriangles

Non-transparent triangles-- add them here
=================
*/
void RendererDrawNormal(); // Engine

void DLLEXPORT HUD_DrawNormalTriangles( void )
{
	RenderFog();

	//Shader Water
	g_Effects.WaterPlane( ); //Hide old and boring boring water brushes

RendererDrawNormal(); // Engine
#if defined( TEST_IT )
	Draw_Triangles();
#endif
}
/*
=================
HUD_DrawTransparentTriangles

Render any triangles with transparent rendermode needs here
=================
*/
void RendererDrawTransparent();

class CException;
void DrawGlows(); // buz
bool g_blReplaceTexture = true;
void DLLEXPORT HUD_DrawTransparentTriangles( void )
{
	RendererDrawTransparent(); // Engine

//	DrawGlows();
	if( CVAR_GET_FLOAT("cl_draw_ropes") )
	gRopeRender.DrawRopes( 1 );

	glDisable(GL_FOG);


	try 
	{
		pParticleManager->UpdateSystems();
	} 
	catch( CException *e ) 
	{
		e;
		e = NULL;
		gEngfuncs.Con_Printf("There was a serious error within the particle engine. Particles will return on map change\n");
		delete pParticleManager;
		pParticleManager = NULL;
	}

	glDisable(GL_FOG);
	BlackFog();

	//22/03/03 LRC: shiny surfaces
	if (gHUD.m_pShinySurface)
		gHUD.m_pShinySurface->DrawAll(v_origin);
	
	if (gHUD.ProcessWeather->value != 0)
	{
		ProcessFXObjects();
		ProcessRain();
		
		DrawRain();
		DrawFXObjects();
	}

	//Shader Water
	if( gEngfuncs.pfnGetCvarFloat( "r_water" ) != 0) //If enabled, render the new water
	{
		g_Effects.PreRender();
		g_Effects.Render();
		g_Effects.PostRender();
	}
		
	glDisable(GL_FOG);
	BlackFog( false );
	
DrawGlows();


						
	if( g_blReplaceTexture )
	{
		model_t *world = gEngfuncs.GetEntityByIndex(0)->model;

		texture_t** tex = (texture_t**)world->textures;

		for (int i = 0; i < world->numtextures; i++)
		{
			LoadAviForTexture( tex[i] );
		}

		g_blReplaceTexture = false;
	}

	RefreshBspAvis();

}

/*
=================
HUD_DrawOrthoTriangles
Orthogonal Triangles -- (relative to resolution,
smackdab on the screen) add them here
=================
*/
void HUD_DrawOrthoTriangles( void )
{
//	gHUD.RenderGame();
	if (CVAR_GET_FLOAT("showtime") == 1)			
	gEngfuncs.Con_Printf( "Time elapsed: %f\n", gEngfuncs.GetClientTime());

//	gEngfuncs.Cvar_SetValue( "score_time", CVAR_GET_FLOAT("score_time")+gEngfuncs.GetClientTime() );
/*
	float fColorModifier = CVAR_GET_FLOAT("gl_color");
	float fEnable = CVAR_GET_FLOAT("gl_light");
	float fAlpha = CVAR_GET_FLOAT("gl_alpha");

	if (fEnable)
	{
		glDisable(GL_TEXTURE_2D);
		glBlendFunc( GL_DST_COLOR, GL_SRC_COLOR );
		glEnable( GL_BLEND );
		glDepthMask(GL_FALSE);

		glBegin( GL_QUADS );

		glColor4f(fColorModifier,fColorModifier,fColorModifier,fAlpha);

		glVertex2f(0.0,0.0);
		glVertex2f(ScreenWidth,0.0);
		glVertex2f(ScreenWidth,ScreenHeight);
		glVertex2f(0.0,ScreenHeight);

		glEnd();

		glDepthMask(GL_TRUE);
		glEnable(GL_TEXTURE_2D);
	}*/
}

// buz: use triapi to draw sprites on screen
void DrawSpriteAsPoly( HSPRITE hspr, wrect_t *rect, wrect_t *screenpos, int mode, float r, float g, float b, float a)
{
	if (!hspr) return;

	const struct model_s *sprmodel = gEngfuncs.GetSpritePointer(hspr);
	gEngfuncs.pTriAPI->RenderMode(mode);
	gEngfuncs.pTriAPI->SpriteTexture( (struct model_s *) sprmodel, 0);
	gEngfuncs.pTriAPI->CullFace( TRI_NONE ); //no culling
	gEngfuncs.pTriAPI->Color4f( r, g, b, a );

	float x = rect->left / (float)SPR_Width(hspr, 0) + 0.01;
	float x2 = rect->right / (float)SPR_Width(hspr, 0) - 0.01;
	float y = rect->top / (float)SPR_Height(hspr, 0) + 0.01;
	float y2 = rect->bottom / (float)SPR_Height(hspr, 0) - 0.01;

	gEngfuncs.pTriAPI->Begin(TRI_QUADS); //start our quad
		gEngfuncs.pTriAPI->TexCoord2f(x, y);
		gEngfuncs.pTriAPI->Vertex3f(screenpos->left, screenpos->top, 0);

		gEngfuncs.pTriAPI->TexCoord2f(x, y2);
		gEngfuncs.pTriAPI->Vertex3f(screenpos->left, screenpos->bottom, 0);

		gEngfuncs.pTriAPI->TexCoord2f(x2, y2);
		gEngfuncs.pTriAPI->Vertex3f(screenpos->right, screenpos->bottom, 0);

		gEngfuncs.pTriAPI->TexCoord2f(x2, y);
		gEngfuncs.pTriAPI->Vertex3f(screenpos->right, screenpos->top, 0);
	gEngfuncs.pTriAPI->End();
}