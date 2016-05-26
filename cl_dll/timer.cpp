//========= Copyright © 2004-2008, Raven City Team,Copyright Ryokeen All rights reserved. ============//
//																									  //
// Purpose:																							  //
//																									  //
// $NoKeywords: $																					  //
//====================================================================================================//

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <stdio.h>

int g_iRoundtime;

DECLARE_MESSAGE(m_Timer, Timer)
int CHudTimer::Init(void)
{
	HOOK_MESSAGE( Timer ); 
	gHUD.AddHudElem(this); 
	return 1; 
};

int CHudTimer::VidInit(void)
{
	g_iRoundtime = -1;
 	return 1;
};

int CHudTimer::MsgFunc_Timer( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	gHUD.m_iRoundtime = READ_LONG();
//	gHUD.m_iRoundtimeCritical = READ_LONG();

	m_iFlags |= HUD_ACTIVE;
 	return 1;
}

int CHudTimer::Draw( float flTime )
{	
	if(gHUD.m_iRoundtime > 0)
	{
	//	gHUD.DrawHudNumber( 80, ScreenHeight-80, DHN_2DIGITS | DHN_PREZERO, gHUD.m_iRoundtime/60, 255, 255, 155);
//		gHUD.DrawHudString( 124, ScreenHeight-78, ScreenWidth, ":",255, 255, 255); 
	//	gHUD.DrawHudNumber( 134, ScreenHeight-80, DHN_2DIGITS | DHN_PREZERO, gHUD.m_iRoundtime%60, 255, 255, 155);

		g_iRoundtime = gHUD.m_iRoundtime;
	}
	else if ( gHUD.m_iRoundtime <= gHUD.m_iRoundtime *25.0/100.0 )
	{
	//	gHUD.DrawHudNumber( 80, ScreenHeight-80,DHN_2DIGITS | DHN_PREZERO, 0, 255, 0, 0);
	//	gHUD.DrawHudString( 124, ScreenHeight-78, ScreenWidth, ":", 255, 0, 0); 
	//	gHUD.DrawHudNumber( 134, ScreenHeight-80,DHN_2DIGITS | DHN_PREZERO, 0, 255, 0, 0);
			
		g_iRoundtime = gHUD.m_iRoundtime;
	}
	else if ( gHUD.m_iRoundtime == -1)
	{
		return 0;
	}	
	/*
	if(gHUD.m_iRoundtime > 0)
	{
		gHUD.DrawHudNumber( 80, ScreenHeight-80, DHN_2DIGITS | DHN_PREZERO, gHUD.m_iRoundtime/60, 255, 255, 255);
		gHUD.DrawHudString(124, ScreenHeight-78, ScreenWidth, ":",255, 255, 255); 
		gHUD.DrawHudNumber(134, ScreenHeight-80, DHN_2DIGITS | DHN_PREZERO, gHUD.m_iRoundtime%60, 255, 255, 255);
	}
	else if ( gHUD.m_iRoundtime <= gHUD.m_iRoundtime *20.0/100 )
	{
		gHUD.DrawHudNumber( 80, ScreenHeight-80,DHN_2DIGITS | DHN_PREZERO, 0, 255, 0, 0);
		gHUD.DrawHudString(124, ScreenHeight-78, ScreenWidth, ":", 255, 0, 0); 
		gHUD.DrawHudNumber(134, ScreenHeight-80,DHN_2DIGITS | DHN_PREZERO, 0, 255, 0, 0);
	}
	else if ( gHUD.m_iRoundtime == -1)
	{
		return 0;
	}*/
	return 1;
}