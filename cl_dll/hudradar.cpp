#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"
#include <string.h>
#include <stdio.h>
#include "parsemsg.h"
#include "event_api.h"
#include "vgui_TeamFortressViewport.h"

#include "triangleapi.h"

int gRadarIsOn;

DECLARE_MESSAGE( m_HudRadar, HudRadar);

int CHudRadar::Init ( void )
{
	// add our new menu to the list of all the other HUD elements, so we can 
	// receive our new message, and get updated, etc.

	gHUD.AddHudElem ( this );

	HOOK_MESSAGE ( HudRadar ); //lets HL know that our class wants the HudRadar message.

	InitHUDData (); // init our variables 

return 1;
}

void CHudRadar::InitHUDData ( void )
{
// Here, we are just initializing all the member variables. 
// Set HUD_ACTIVE to display the radar at startup (if you wanted it switchable, you could change that)
UnpackRGB ( Red, Green, Blue, RGB_YELLOWISH );
m_iNumTargets = 0;
m_iFlags |= HUD_ACTIVE;

}

void CHudRadar::Reset ( void )
{
// Do nothing for now
}

int CHudRadar::VidInit ( void )
{
//	m_hRadarSpr = 0;

	return 1;
}

int CHudRadar::MsgFunc_HudRadar ( const char *pszName, int iSize, void *pbuf )
{

// Read message
BEGIN_READ ( pbuf, iSize );
//test if on
cl_bRadarIsOn = READ_BYTE();

// Set number of targets
m_iNumTargets = READ_BYTE();

// Read+store each x, y coordinate for each target
for (int i=0;i < m_iNumTargets; i++ )
{
	m_iPosx[i] = READ_BYTE();
	m_iPosy[i] = READ_BYTE();
}
return 1;
}

int CHudRadar::Draw ( float flTime )
{
	gRadarIsOn = cl_bRadarIsOn;

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
	return 1;

	if(cl_bRadarIsOn == 1)//si esta encendido hacer todo esto
	{
		// Load radar background sprite
	/*	if (!m_hRadarSpr)
		{
			m_hRadarSpr = LoadSprite("sprites/radar1.spr"); // quick and nasty way of displaying a sprite - you should precache instead
		}*/

		// Draw an "x" at the position on screen for each entity. 
		// Of course you'll want to change this to a sprite display in your own code.
		for( int i = 0;i < m_iNumTargets;i++)
		{
			int radar_bounds = 128;

			int screen_coord_x = ScreenWidth /2 + radar_bounds /2 - radar_bounds;//?????????
			int screen_coord_y = ScreenHeight /2 + radar_bounds /2 /*- radar_bounds*/; 

		//	int screen_coord = screen_coord_x + screen_coord_y;

			// scale screen by 1/2 for display
		//	gHUD.DrawHudString ( screen_coord_x-(m_iPosx[i]/2), screen_coord_y-(m_iPosy[i]/2), screen_coord, "'", Red, Green, Blue );

			//-10 is for adjust char's position
			gHUD.DrawHudString ( screen_coord_x+(m_iPosx[i]/2), screen_coord_y-(m_iPosy[i]/2) -10, ScreenWidth, "'", Red, Green, Blue );
		}
	}

return 1;
}

/*
DECLARE_MESSAGE( m_HudRadar, HudRadar);

int CHudRadar::Init ( void )
{
#if COMPILE_HOE
	return 0;
#endif

#if _CODE_REBIRTH
	return 0;
#else
	// add our new menu to the list of all the other HUD elements, so we can 
	// receive our new message, and get updated, etc.

	gHUD.AddHudElem ( this );

	HOOK_MESSAGE ( HudRadar ); //lets HL know that our class wants the HudRadar message.

	InitHUDData (); // init our variables 

	return 1;
#endif
}

void CHudRadar::InitHUDData ( void )
{
	// Here, we are just initializing all the member variables. 
	// Set HUD_ACTIVE to display the radar at startup (if you wanted it switchable, you could change that)
	UnpackRGB ( Red, Green, Blue, RGB_YELLOWISH );
	m_iNumTargets = 0;

//	m_iFlags |= HUD_ACTIVE;

	cl_bRadarIsOn = 0;
	m_iFlags = 0;
}

void CHudRadar::Reset ( void )
{
// Do nothing for now
}

int CHudRadar::VidInit ( void )
{
	m_hRadarSpr = 0;

	return 1;
}

int CHudRadar::MsgFunc_HudRadar ( const char *pszName, int iSize, void *pbuf )
{
	// Read message
	BEGIN_READ ( pbuf, iSize );

	//test if on
	cl_bRadarIsOn = READ_BYTE();

	// Set number of targets
	m_iNumTargets = READ_BYTE();
	// Read+store each x, y coordinate for each target
	for (int i=0;i < m_iNumTargets; i++ )
	{
		m_iPosx[i] = READ_BYTE();
		m_iPosy[i] = READ_BYTE();
	}

	if(cl_bRadarIsOn == 1)//si esta encendido hacer todo esto
	{
		m_iFlags |= HUD_ACTIVE;
	}
	else
	{
		m_iFlags &= ~HUD_ACTIVE;
	}

	return 1;
}

int CHudRadar::Draw ( float flTime )
{	
	float fl1 = CVAR_GET_FLOAT( "value_1" );
	float fl2 = CVAR_GET_FLOAT( "value_2" );
	float fl3 = CVAR_GET_FLOAT( "value_3" );
		
	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
	return 1;

	if(cl_bRadarIsOn == 1)//si esta encendido hacer todo esto
	{
		// Load radar background sprite
		if (!m_hRadarSpr)
		{
			m_hRadarSpr = LoadSprite("sprites/radar1.spr"); // quick and nasty way of displaying a sprite - you should precache instead
		}

		// Draw an "x" at the position on screen for each entity. 
		// Of course you'll want to change this to a sprite display in your own code.
		for( int i = 0;i < m_iNumTargets;i++)
		{
			//well, something is displayed on the radar... make sound!
			//if (CVAR_GET_FLOAT("cl_playhudsounds") == 1)					
			//PlaySound("player/radar_warning.wav", 0.9);

//			int xpos = ScreenWidth / CVAR_GET_FLOAT( "value_1" ); //One forth across the screen. Useful in the case of various screen resolutions.
//			int ypos = ScreenHeight / CVAR_GET_FLOAT( "value_2" ); //Very middle of the screen.

			// scale screen by 1/2 for display
//			gHUD.DrawHudString ( xpos, ypos, ScreenWidth, "o", 255, 0, 0 );

////			gHUD.DrawHudString ( m_iPosx[i]/fl1, fl3-(m_iPosy[i]/fl2), fl3, "o", 255, 0, 0 );

//			gHUD.DrawHudString ( fl1-m_iPosx[i]/2, fl2-(m_iPosy[i]/2), fl3, "o", 255, 0, 0 );

			gHUD.DrawHudString ( m_iPosx[i]/2, 128-(m_iPosy[i]/2), 128, ".", 255, 0, 0 );
		}

		// Draw Radar background
		// You can use DrawHoles to draw a sprite with transparent parts, or DrawAdditive, or Draw...
		// Maybe i'll stick this stuff in a separate tutorial
		SPR_Set( m_hRadarSpr, 155, 155, 155 );
		SPR_DrawAdditive( 0, 0, 13, NULL);
//		SPR_DrawAdditive( 0, 0, 10, NULL);
	}

return 1;
}
*/