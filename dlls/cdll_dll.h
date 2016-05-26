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
//  cdll_dll.h

// this file is included by both the game-dll and the client-dll,

#ifndef CDLL_DLL_H
#define CDLL_DLL_H

#define MAX_WEAPONS		82		// ???

#define MAX_WEAPON_SLOTS		8	// hud item selection slots //10
#define MAX_ITEM_TYPES			9	// hud item selection slots

#define MAX_ITEMS				9	// hard coded item types

#define	HIDEHUD_WEAPONS		( 1<<0 )
#define	HIDEHUD_FLASHLIGHT	( 1<<1 )
#define	HIDEHUD_ALL			( 1<<2 )
#define 	HIDEHUD_HEALTH		( 1<<3 )

#define	MAX_AMMO_TYPES	82		// ???
#define MAX_AMMO_SLOTS  82		// not really slots

#define HUD_PRINTNOTIFY		1
#define HUD_PRINTCONSOLE	2
#define HUD_PRINTTALK		3
#define HUD_PRINTCENTER		4


#define WEAPON_SUIT			31

//Might as well share this here
// Hintbox help messages
const int HINTBOX_JUMP = 1;
const int HINTBOX_CROUCH = 2;
const int HINTBOX_RELOAD = 3;
const int HINTBOX_FLAGCAP = 4;
const int HINTBOX_FLAGOVERLOAD = 5;
const int HINTBOX_HINTBOX = 6;
const int HINTBOX_LOWSTAMINA = 7;
const int HINTBOX_TEAMSCORE = 8;
const int HINTBOX_PERSONALSCORE = 9;
const int HINTBOX_MUTE = 10;
const int HINTBOX_LOWAMMO = 11;	
const int HINTBOX_MELEE = 12;
const int HINTBOX_MELEE2 = 13;
const int HINTBOX_CAMPING = 14;
const int HINTBOX_CROUCH2 = 15;
const int HINTBOX_INACCURATE = 16;
const int HINTBOX_DISPLAYMODE = 17;
const int HINTBOX_PROGSPAWNING = 18;

// Hintbox class stats
const int HINTBOX_CLASS_LIGHTA = 21;
const int HINTBOX_CLASS_MEDA = 22;
const int HINTBOX_CLASS_HEAVYA = 23;
const int HINTBOX_CLASS_LIGHTB = 24;
const int HINTBOX_CLASS_MEDB = 25;
const int HINTBOX_CLASS_HEAVYB = 26;

#endif