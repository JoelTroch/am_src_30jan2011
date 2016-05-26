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
//  cdll_int.c
//
// this implementation handles the linking of the engine to the DLL
//

#include "hud.h"
#include "cl_util.h"
#include "netadr.h"
#include "vgui_schememanager.h"
#include "mp3.h" //AJH - Killars MP3player

#include "blur.h"

extern "C"
{
#include "pm_shared.h"
}

#include <string.h>
#include "hud_servers.h"
#include "vgui_int.h"
#include "interface.h"

#include "exception.h" // buz
#include "log.h"

#define DLLEXPORT __declspec( dllexport )


cl_enginefunc_t gEngfuncs;
CHud gHUD;
TeamFortressViewport *gViewPort = NULL;
CMP3 gMP3; //AJH - Killars MP3player


#include <windows.h>
#include <gl\gl.h>

void InitInput (void);
void EV_HookEvents( void );
void IN_Commands( void );

/*
========================== 
    Initialize

Called when the DLL is first loaded.
==========================
*/

// SCREEN GLOW FragBait0
extern void InitScreenGlow(); // FragBait0 - Glow Effect
extern void RenderScreenGlow(); // FragBait0 - Glow Effect


extern "C" 
{
int		DLLEXPORT Initialize( cl_enginefunc_t *pEnginefuncs, int iVersion );
int		DLLEXPORT HUD_VidInit( void );
void	DLLEXPORT HUD_Init( void );
int		DLLEXPORT HUD_Redraw( float flTime, int intermission );
int		DLLEXPORT HUD_UpdateClientData( client_data_t *cdata, float flTime );
void	DLLEXPORT HUD_Reset ( void );
void	DLLEXPORT HUD_PlayerMove( struct playermove_s *ppmove, int server );
void	DLLEXPORT HUD_PlayerMoveInit( struct playermove_s *ppmove );
char	DLLEXPORT HUD_PlayerMoveTexture( char *name );
int		DLLEXPORT HUD_ConnectionlessPacket( const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size );
int		DLLEXPORT HUD_GetHullBounds( int hullnumber, float *mins, float *maxs );
void	DLLEXPORT HUD_Frame( double time );
void	DLLEXPORT HUD_VoiceStatus(int entindex, qboolean bTalking);
void	DLLEXPORT HUD_DirectorMessage( int iSize, void *pbuf );
}











void ENGINE_SetContext( void )
{/*
	HGLRC OGLContext = wglGetCurrentContext();
	
	if( OGLContext)
	{
		MessageBox(NULL, "VIDEO: Half-life's renderer will be changed for a custom one.\n", "ERROR", MB_OK);

		HDC hDC = wglGetCurrentDC();
		wglMakeCurrent(hDC, NULL);
		wglDeleteContext(OGLContext);

		PIXELFORMATDESCRIPTOR pfd = 
		{
			sizeof(PIXELFORMATDESCRIPTOR),	// size of this pfd
			1,								// version number
			PFD_DRAW_TO_WINDOW |			// support window
			PFD_SUPPORT_OPENGL |			// support OpenGL
			PFD_DOUBLEBUFFER,				// double buffered
			PFD_TYPE_RGBA,					// RGBA type
			32,								// 24-bit color depth // <---- PBM 32
			0, 255, 0, 0, 0, 0,				// color bits ignored
			0,								// no alpha buffer
			255,								// shift bit ignored
			0,								// no accumulation buffer
			0, 0, 0, 0, 					// accum bits ignored
			24,								// 32-bit z-buffer	
			4,								// no stencil buffer  // <---- PBM STENCIL 4 BITS
			0,								// no auxiliary buffer
			PFD_MAIN_PLANE,					// main layer
			0,								// reserved
			0, 255, 0							// layer masks ignored
		};
		int  pixelformat;
		PIXELFORMATDESCRIPTOR *ppfd = &pfd;

		DEVMODE dm;
		ZeroMemory(&dm, sizeof(dm));
		dm.dmSize = sizeof(dm);
		dm.dmBitsPerPel = 32;
		dm.dmPelsWidth = GetSystemMetrics(SM_CXSCREEN);
		dm.dmPelsHeight = GetSystemMetrics(SM_CYSCREEN);
		dm.dmFields = DM_BITSPERPEL;

		// Full screen?
		if( GetWindowLong(WindowFromDC(hDC), GWL_EXSTYLE) & WS_EX_TOPMOST )
			dm.dmFields |= DM_PELSWIDTH | DM_PELSHEIGHT;
		
		ChangeDisplaySettings(&dm,0);

		pixelformat = ChoosePixelFormat(hDC, ppfd);
		SetPixelFormat(hDC, pixelformat, ppfd);
		OGLContext = wglCreateContext(hDC);
		wglMakeCurrent(hDC, OGLContext);

		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER,0);		
	}*/
}
/*
================================
HUD_GetHullBounds

  Engine calls this to enumerate player collision hulls, for prediction.  Return 0 if the hullnumber doesn't exist.
================================
*/
int DLLEXPORT HUD_GetHullBounds( int hullnumber, float *mins, float *maxs )
{
	int iret = 0;

	switch ( hullnumber )
	{
	case 0:				// Normal player
		mins = Vector(-16, -16, -36);
		maxs = Vector(16, 16, 36);
		iret = 1;
		break;
	case 1:				// Crouched player
		mins = Vector(-16, -16, -18 );
		maxs = Vector(16, 16, 18 );
		iret = 1;
		break;
	case 2:				// Point based hull
		mins = Vector( 0, 0, 0 );
		maxs = Vector( 0, 0, 0 );
		iret = 1;
		break;
	}

	return iret;
}

/*
================================
HUD_ConnectionlessPacket

 Return 1 if the packet is valid.  Set response_buffer_size if you want to send a response packet.  Incoming, it holds the max
  size of the response_buffer, so you must zero it out if you choose not to respond.
================================
*/
int	DLLEXPORT HUD_ConnectionlessPacket( const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size )
{
	// Parse stuff from args
	int max_buffer_size = *response_buffer_size;

	// Zero it out since we aren't going to respond.
	// If we wanted to response, we'd write data into response_buffer
	*response_buffer_size = 0;

	// Since we don't listen for anything here, just respond that it's a bogus message
	// If we didn't reject the message, we'd return 1 for success instead.
	return 0;
}

void DLLEXPORT HUD_PlayerMoveInit( struct playermove_s *ppmove )
{
	PM_Init( ppmove );
}

char DLLEXPORT HUD_PlayerMoveTexture( char *name )
{
	return PM_FindTextureType( name );
}

void DLLEXPORT HUD_PlayerMove( struct playermove_s *ppmove, int server )
{
	PM_Move( ppmove, server );
}

int DLLEXPORT Initialize( cl_enginefunc_t *pEnginefuncs, int iVersion )
{
	gEngfuncs = *pEnginefuncs;

	if (iVersion != CLDLL_INTERFACE_VERSION)
		return 0;

	memcpy(&gEngfuncs, pEnginefuncs, sizeof(cl_enginefunc_t));

	EV_HookEvents();

//	if ( CVAR_GET_FLOAT( "r_stencil_buffer" ) != 0 )
//	ENGINE_SetContext();
		
	return 1;
}


/*
==========================
	HUD_VidInit

Called when the game initializes
and whenever the vid_mode is changed
so the HUD can reinitialize itself.
==========================
*/

int DLLEXPORT HUD_VidInit( void )
{
	gHUD.VidInit();

	VGui_Startup();

	if (CVAR_GET_FLOAT("r_glow") != 0)	 //check the cvar for the glow is on.//AJH Modified to include glow mode (1&2)
	InitScreenGlow(); // glow effect --FragBait0

//	if (CVAR_GET_FLOAT("r_blur") != 0)	 //check the cvar for the glow is on.//AJH Modified to include glow mode (1&2)

	gBlur.InitScreen();

	return 1;
}

/*
==========================
	HUD_Init

Called whenever the client connects
to a server.  Reinitializes all 
the hud variables.
==========================
*/

void DLLEXPORT HUD_Init( void )
{
	InitInput();
	gHUD.Init();
	Scheme_Init();
	
	SetupLogging (); // buz

	// buz: enable CrashRpt library
	LoadCrashRpt();
}


/*
==========================
	HUD_Redraw

called every screen frame to
redraw the HUD.
===========================
*/

int DLLEXPORT HUD_Redraw( float time, int intermission )
{
	if (CVAR_GET_FLOAT("r_glow") != 0)	 //check the cvar for the glow is on.//AJH Modified to include glow mode (1&2)
	RenderScreenGlow(); // glow effect --FragBait0

//	if (CVAR_GET_FLOAT("r_blur") != 0)	 //check the cvar for the glow is on.//AJH Modified to include glow mode (1&2)

	gHUD.Redraw( time, intermission );

//	if (CVAR_GET_FLOAT("r_blur") != 0)	 //check the cvar for the glow is on.//AJH Modified to include glow mode (1&2)
	gBlur.DrawBlur();

	return 1;
}


/*
==========================
	HUD_UpdateClientData

called every time shared client
dll/engine data gets changed,
and gives the cdll a chance
to modify the data.

returns 1 if anything has been changed, 0 otherwise.
==========================
*/

int DLLEXPORT HUD_UpdateClientData(client_data_t *pcldata, float flTime )
{
	IN_Commands();

	return gHUD.UpdateClientData(pcldata, flTime );
}

/*
==========================
	HUD_Reset

Called at start and end of demos to restore to "non"HUD state.
==========================
*/

void DLLEXPORT HUD_Reset( void )
{
	gHUD.VidInit();
}

/*
==========================
HUD_Frame

Called by engine every frame that client .dll is loaded
==========================
*/

void DLLEXPORT HUD_Frame( double time )
{
	ServersThink( time );

	GetClientVoiceMgr()->Frame(time);
}


/*
==========================
HUD_VoiceStatus

Called when a player starts or stops talking.
==========================
*/

void DLLEXPORT HUD_VoiceStatus(int entindex, qboolean bTalking)
{
	GetClientVoiceMgr()->UpdateSpeakerStatus(entindex, bTalking);
}

/*
==========================
HUD_DirectorEvent

Called when a director event message was received
==========================
*/

void DLLEXPORT HUD_DirectorMessage( int iSize, void *pbuf )
{
	 gHUD.m_Spectator.DirectorMessage( iSize, pbuf );
}


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{/*
	if(fdwReason == DLL_PROCESS_ATTACH)
	{
	//	ENGINE_SetContext();
	}*/
	return TRUE;
}
