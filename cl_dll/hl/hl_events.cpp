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
#include "../hud.h"
#include "../cl_util.h"
#include "event_api.h"

extern "C"
{
// HLDM
void EV_FireGlock1( struct event_args_s *args  );
void EV_FireGlock2( struct event_args_s *args  );
void EV_FireMP51( struct event_args_s *args  );
void EV_FireMP52( struct event_args_s *args  );
void EV_FirePython( struct event_args_s *args  );
void EV_FireGauss( struct event_args_s *args  );
void EV_SpinGauss( struct event_args_s *args  );
void EV_FireRpg( struct event_args_s *args );
void EV_HornetGunFire( struct event_args_s *args );
void EV_TripmineFire( struct event_args_s *args );
void EV_SnarkFire( struct event_args_s *args );

void EV_FireM161( struct event_args_s *args  );
void EV_FireM162( struct event_args_s *args  );
void EV_FireAK74( struct event_args_s *args  );
void EV_FireAK74_STAB( struct event_args_s *args  );
void EV_FireDeagle( struct event_args_s *args  );
void EV_FireDeagle2( struct event_args_s *args  );
void EV_FireSniper( struct event_args_s *args  );
void EV_FireFamas( struct event_args_s *args  );
void EV_FireFamas2( struct event_args_s *args  );
//void EV_Hand( struct event_args_s *args  );
void EV_FireM249( struct event_args_s *args  );
void EV_FireUZI( struct event_args_s *args  );
void EV_FireUZI_SIL( struct event_args_s *args  );
void EV_Fire92f( struct event_args_s *args  );
void EV_Fire92fSil( struct event_args_s *args  );
void EV_FireGlock18( struct event_args_s *args  );
void EV_FireGlock18_B( struct event_args_s *args  );
void EV_FireUsas( struct event_args_s *args  );
void EV_FireP90( struct event_args_s *args  );

void EV_Knife( struct event_args_s *args );
void EV_Knife_stab( struct event_args_s *args );

//void EV_FAKnife( struct event_args_s *args );
//void EV_FAKnife_stab( struct event_args_s *args );
void EV_FireSvd( struct event_args_s *args );
//|SP|
void EV_FireCrossbow( struct event_args_s *args );
void EV_FireCrossbow2( struct event_args_s *args );
void EV_FirePistol( struct event_args_s *args  ); //ohne Silencer
											
void EV_FireEliteRight( struct event_args_s *args  ); //ohne Silencer
void EV_FireEliteLeft( struct event_args_s *args  );    //mit Silencer
void EV_FireEliteBoth( struct event_args_s *args  );    //mit Silencer

void EV_FireLx( struct event_args_s *args  );
void EV_FireM4A2( struct event_args_s *args  );	    //ohne Silencer
void EV_FireM4A2Gl( struct event_args_s *args  );    //mit Silencer

void EV_FireOICW( struct event_args_s *args  ); //ohne Silencer
void EV_FireOICWgl( struct event_args_s *args  );    //mit Silencer

void EV_Smoke( struct event_args_s *args  );
void EV_TrainPitchAdjust( struct event_args_s *args );

void EV_SpriteTracer( struct event_args_s *args  );
void EV_SpasShell( struct event_args_s *args  );
void EV_FireIRGUN( struct event_args_s *args  );

void EV_FireHL2Crowbar( struct event_args_s *args  );

void EV_FireShotGunSingle( struct event_args_s *args  );
void EV_FireShotGunDouble( struct event_args_s *args  );

void EV_FireHL2ShotGunSingle( struct event_args_s *args  );
void EV_FireHL2ShotGunDouble( struct event_args_s *args  );

void EV_FireShotGunLessLethal( struct event_args_s *args  );

}

/*
======================
Game_HookEvents

Associate script file name with callback functions.  Callback's must be extern "C" so
 the engine doesn't get confused about name mangling stuff.  Note that the format is
 always the same.  Of course, a clever mod team could actually embed parameters, behavior
 into the actual .sc files and create a .sc file parser and hook their functionality through
 that.. i.e., a scripting system.

That was what we were going to do, but we ran out of time...oh well.
======================
*/
void Game_HookEvents( void )
{
	gEngfuncs.pfnHookEvent( "scripts/events/glock1.sc",					EV_FireGlock1 );
	gEngfuncs.pfnHookEvent( "scripts/events/glock2.sc",					EV_FireGlock2 );

	gEngfuncs.pfnHookEvent( "scripts/events/shotgun1.sc",				EV_FireShotGunSingle );
	gEngfuncs.pfnHookEvent( "scripts/events/shotgun2.sc",				EV_FireShotGunDouble );
	gEngfuncs.pfnHookEvent( "scripts/events/shotgun_lesslethal.sc",		EV_FireShotGunLessLethal );

	gEngfuncs.pfnHookEvent( "scripts/events/mp51.sc",					EV_FireMP51 );
	gEngfuncs.pfnHookEvent( "scripts/events/mp52.sc",					EV_FireMP52 );
	gEngfuncs.pfnHookEvent( "scripts/events/python.sc",					EV_FirePython );
	gEngfuncs.pfnHookEvent( "scripts/events/gauss.sc",					EV_FireGauss );
	gEngfuncs.pfnHookEvent( "scripts/events/gaussspin.sc",				EV_SpinGauss );
	gEngfuncs.pfnHookEvent( "scripts/events/train.sc",					EV_TrainPitchAdjust );

	gEngfuncs.pfnHookEvent( "scripts/events/rpg.sc",					EV_FireRpg );

	gEngfuncs.pfnHookEvent( "scripts/events/firehornet.sc",				EV_HornetGunFire );
	gEngfuncs.pfnHookEvent( "scripts/events/tripfire.sc",				EV_TripmineFire );
	gEngfuncs.pfnHookEvent( "scripts/events/snarkfire.sc",				EV_SnarkFire );

	gEngfuncs.pfnHookEvent( "scripts/events/m161.sc",				    EV_FireM161 );
	gEngfuncs.pfnHookEvent( "scripts/events/m162.sc",				    EV_FireM162 );
	gEngfuncs.pfnHookEvent( "scripts/events/AK47.sc",				    EV_FireAK74 );
	gEngfuncs.pfnHookEvent( "scripts/events/AK47_STAB.sc",				EV_FireAK74_STAB );
	gEngfuncs.pfnHookEvent( "scripts/events/Deagle.sc",				    EV_FireDeagle );
	gEngfuncs.pfnHookEvent( "scripts/events/Deagle2.sc",				EV_FireDeagle2 );
	gEngfuncs.pfnHookEvent( "scripts/events/Sniper.sc",					EV_FireSniper );
	gEngfuncs.pfnHookEvent( "scripts/events/famas.sc",					EV_FireFamas );
	gEngfuncs.pfnHookEvent( "scripts/events/famas_burst.sc",			EV_FireFamas2 );
	gEngfuncs.pfnHookEvent( "scripts/events/M249.sc",					EV_FireM249 );
	gEngfuncs.pfnHookEvent( "scripts/events/UZI.sc",					EV_FireUZI );
	gEngfuncs.pfnHookEvent( "scripts/events/UZI_sil.sc",				EV_FireUZI_SIL );
	gEngfuncs.pfnHookEvent( "scripts/events/92f.sc",					EV_Fire92f );
	gEngfuncs.pfnHookEvent( "scripts/events/92f_sil.sc",				EV_Fire92fSil );
	gEngfuncs.pfnHookEvent( "scripts/events/glock18.sc",				EV_FireGlock18 );
	gEngfuncs.pfnHookEvent( "scripts/events/glock18_burst.sc",			EV_FireGlock18_B );
	gEngfuncs.pfnHookEvent( "scripts/events/usas.sc",					EV_FireUsas );
	gEngfuncs.pfnHookEvent( "scripts/events/ump.sc",					EV_FireP90 );	
	gEngfuncs.pfnHookEvent( "scripts/events/knife.sc",					EV_Knife );
	gEngfuncs.pfnHookEvent( "scripts/events/knife_stab.sc",				EV_Knife_stab );

	gEngfuncs.pfnHookEvent( "scripts/events/m82.sc",					EV_FireSvd );

	gEngfuncs.pfnHookEvent( "scripts/events/lx38.sc",					EV_FireLx );

	gEngfuncs.pfnHookEvent( "scripts/events/pistol.sc",					EV_FirePistol );

	gEngfuncs.pfnHookEvent( "scripts/events/m4a2.sc",					EV_FireM4A2 );
	gEngfuncs.pfnHookEvent( "scripts/events/m16gl.sc",					EV_FireM4A2Gl );

	gEngfuncs.pfnHookEvent( "scripts/events/oicw.sc",					EV_FireOICW );
	gEngfuncs.pfnHookEvent( "scripts/events/oicw_gl.sc",				EV_FireOICWgl );
			
	gEngfuncs.pfnHookEvent( "scripts/events/sprite_tracer.sc",			EV_SpriteTracer );
	gEngfuncs.pfnHookEvent( "scripts/events/sniper_shell.sc",			EV_SpasShell );

	gEngfuncs.pfnHookEvent( "scripts/events/irgun.sc",					EV_FireIRGUN );
	
	gEngfuncs.pfnHookEvent( "scripts/events/hl2crowbar.sc",				EV_FireHL2Crowbar );
	gEngfuncs.pfnHookEvent( "scripts/events/hl2shotgun1.sc",			EV_FireHL2ShotGunSingle );
	gEngfuncs.pfnHookEvent( "scripts/events/hl2shotgun2.sc",			EV_FireHL2ShotGunDouble );
}
