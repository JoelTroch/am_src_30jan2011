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
// NEW WEAPON FILE
//===================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"

#include "shake.h" //This is required for the screen fade

#define pda_DEFAULT_GIVE		1
#define pda_MAX_CARRY			5
#define pda_WEIGHT				1 //ddd!

//DELAYS
// Fire Anim: 5.35
// Reload Anim: 3.05

enum pda_e
{
	PDA_IDLE1 = 0,
	PDA_DEPLOY,

	PDA_HACK_START,
	PDA_HACK_LOOP,
	PDA_HACK_END
};

LINK_ENTITY_TO_CLASS( weapon_pda, CPDA );

//=========================================================
//=========================================================
void CPDA::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_pda"); // hack to allow for old names
	Precache( );

	if (!FStringNull (v_model) )
	SET_MODEL( ENT(pev), STRING(w_model) );
	else
	SET_MODEL(ENT(pev), "models/null.mdl");

	m_iId = WEAPON_PDA;

	m_iDefaultAmmo = pda_DEFAULT_GIVE; // 

	FallInit();// get ready to fall down.
}

void CPDA::Precache( void )
{
	if (!FStringNull (v_model) )
	{
		PRECACHE_MODEL( (char *)STRING(v_model) );
	//	PRECACHE_MODEL( (char *)STRING(p_model) );
	//	PRECACHE_MODEL( (char *)STRING(w_model) );
	}
	else
	{
		PRECACHE_MODEL("models/weapons/pda/v_pda.mdl");
	//	PRECACHE_MODEL("models/weapons/oicw/w_oicw.mdl");
	//	PRECACHE_MODEL("models/weapons/oicw/p_oicw.mdl");
	}

	PRECACHE_SOUND ("weapons/pda/pda_open.wav");
	PRECACHE_SOUND ("weapons/pda/pda_close.wav");
	PRECACHE_SOUND ("weapons/pda/pda_type.wav");
}

void CPDA::KeyValue( KeyValueData *pkvd )//this sets for custom fields of weapon_generic. G-Cont.
{
	if (FStrEq(pkvd->szKeyName, "m_iszModel"))
	{
		char string[64];

		sprintf(string, "models/weapons/pda/v_%s.mdl", pkvd->szValue);
		v_model = ALLOC_STRING(string);
/*
		sprintf(string, "models/weapons/oicw/p_%s.mdl", pkvd->szValue);
		p_model = ALLOC_STRING(string);

		sprintf(string, "models/weapons/oicw/w_%s.mdl", pkvd->szValue);
		w_model = ALLOC_STRING(string);
*/
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

int CPDA::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 0;
	p->iPosition = 3;
    p->iFlags    = ITEM_FLAG_SELECTONEMPTY;
	p->iId = m_iId = WEAPON_PDA;
	p->iWeight = 10;
	p->weaponName = "PDA";

	return 1;
}

int CPDA::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#Pickup_pda"); //digamos al cliente

		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();

		return TRUE;
	}
	return FALSE;
}

BOOL CPDA::Deploy( )
{
	m_pPlayer->m_fCrosshairOff = TRUE;
//	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/weapon_deploy.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/pda/pda_open.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));

	m_fInAttack = 0;
		
	m_pPlayer->bRadarIsOn = TRUE;

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;

	if (!FStringNull (v_model) )
	{
		m_pPlayer->pev->viewmodel = v_model;
		//m_pPlayer->pev->weaponmodel = p_model;
	
		SendWeaponAnim( PDA_DEPLOY, 1, 0 );	
		return TRUE;
	}
	else
	return DefaultDeploy( "models/weapons/pda/v_pda.mdl", "models/null.mdl", PDA_DEPLOY, "mp5a4" );
}

void CPDA::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->m_fCrosshairOff = FALSE;
	m_fInReload = FALSE;// cancel any reload in progress.
		
	m_pPlayer->bRadarIsOn = FALSE;

	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", RANDOM_FLOAT(0.8, 0.9), ATTN_NORM);

	m_fInAttack = 0;

	pev->nextthink = -1;

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.5; //0.5

	SendWeaponAnim( PDA_HACK_END );
}

#ifndef CLIENT_DLL
extern int gmsgOS;
#endif

void CPDA::ChargeOk()
{				
//	SERVER_COMMAND( "home\n" );
//TODO
	//check map name

	#ifndef CLIENT_DLL
		char *map_name = (char *)STRING(gpGlobals->mapname);
		
		MESSAGE_BEGIN( MSG_ONE, gmsgOS, NULL, m_pPlayer->pev );
			WRITE_STRING(map_name);
			WRITE_BYTE(1);
		MESSAGE_END();
	#endif
		
	SendWeaponAnim( PDA_IDLE1 );

	return;
} 

void CPDA::PrimaryAttack()
{
	// don't fire underwater
	if ( m_pPlayer->pev->waterlevel == 3 )
	{
		PlayEmptySound( );
		
		m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		return;
	}

	SendWeaponAnim( PDA_HACK_START );	
	ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#AccesingOS"); //digamos al cliente
//	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/heal/heal_fire-1.wav", 1, ATTN_NORM);
	SetThink( ChargeOk );
	pev->nextthink = gpGlobals->time + 0.5;

/*	if ( m_fInAttack == 0 )
	{
		SendWeaponAnim( PDA_HACK_START );
		m_fInAttack = 1;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.50;
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.50;

	}*/
/*	else if (m_fInAttack == 1)
	{
		if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
		{
			SendWeaponAnim( PDA_HACK_LOOP );
			//m_fInAttack = 2;
				
*/
/*
		CBaseEntity *pObject = NULL;
		CBaseEntity *pClosest = NULL;
		Vector		vecLOS;
		float flMaxDot = VIEW_FIELD_NARROW;
		float flDot;

		UTIL_MakeVectors ( pev->v_angle );// so we know which way we are facing
		
		while ((pObject = UTIL_FindEntityInSphere( pObject, pev->origin, PLAYER_SEARCH_RADIUS )) != NULL)
		{
			if (pObject->ObjectCaps() & (FCAP_IMPULSE_USE | FCAP_CONTINUOUS_USE | FCAP_ONOFF_USE))
			{
				// !!!PERFORMANCE- should this check be done on a per case basis AFTER we've determined that
				// this object is actually usable? This dot is being done for every object within PLAYER_SEARCH_RADIUS
				// when player hits the use key. How many objects can be in that area, anyway? (sjb)
				vecLOS = (VecBModelOrigin( pObject->pev ) - (pev->origin + pev->view_ofs));
				
				// This essentially moves the origin of the target to the corner nearest the player to test to see 
				// if it's "hull" is in the view cone
				vecLOS = UTIL_ClampVectorToBox( vecLOS, pObject->pev->size * 0.5 );
				
				flDot = DotProduct (vecLOS , gpGlobals->v_forward);
				if (flDot > flMaxDot )
				{// only if the item is in front of the user
					pClosest = pObject;
					flMaxDot = flDot;
	//				ALERT( at_console, "%s : %f\n", STRING( pObject->pev->classname ), flDot );
				}
	//			ALERT( at_console, "%s : %f\n", STRING( pObject->pev->classname ), flDot );
			}
		}
		pObject = pClosest;

		// Found an object
		if (pObject )
		{			
			//!!!UNDONE: traceline here to prevent USEing buttons through walls			
			int caps = pObject->ObjectCaps();

	//		if ( m_pPlayer->m_afButtonPressed & IN_USE )
				EMIT_SOUND( ENT(pev), CHAN_ITEM, "common/wpn_select.wav", 0.4, ATTN_NORM);

	//		if ( ( (pev->button & IN_USE) && (caps & FCAP_CONTINUOUS_USE) ) ||
	//			 ( (m_pPlayer->m_afButtonPressed & IN_USE) && (caps & (FCAP_IMPULSE_USE|FCAP_ONOFF_USE)) ) )
	//		{
	//			if ( caps & FCAP_CONTINUOUS_USE )
	//				m_pPlayer->m_afPhysicsFlags |= PFLAG_USING;

				pObject->Use( this, this, USE_SET, 1 );
	//		}
			// UNDONE: Send different USE codes for ON/OFF.  Cache last ONOFF_USE object to send 'off' if you turn away
	//		else if ( (m_pPlayer->m_afButtonReleased & IN_USE) && (pObject->ObjectCaps() & FCAP_ONOFF_USE) )	// BUGBUG This is an "off" use
	//		{
	//			pObject->Use( this, this, USE_SET, 0 );
	//		}
		}
		else
		{
		//	if ( m_pPlayer->m_afButtonPressed & IN_USE )
				EMIT_SOUND( ENT(pev), CHAN_ITEM, "common/wpn_denyselect.wav", 0.4, ATTN_NORM);
		}

*/











/*












			//CLIENT_COMMAND(m_pPlayer->edict(), "+use\n");
			
			ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "Working...\nPress Mouse2 to stop hacking"); //digamos al cliente

			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/pda/pda_type.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));

			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.16;
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 2.16;
		}
	}
	else
	{
	//	SendWeaponAnim( PDA_HACK_START );

	//	SendWeaponAnim( PDA_HACK_END );
	}*/
	
//	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.16;
//	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 2.16;
}

void CPDA::FuncThink()
{
	SERVER_COMMAND( "lastinv\n" );

	pev->nextthink = -1;
}

void CPDA::SecondaryAttack()
{
	return;

	SendWeaponAnim( PDA_HACK_END );
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/pda/pda_close.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		
	SetThink( FuncThink );
	pev->nextthink = gpGlobals->time + 1;
}

void CPDA::WeaponIdle( void )
{	
	if ( m_pPlayer->pev->waterlevel == 3 )
	{
		PlayEmptySound( );
		
		m_flNextSecondaryAttack = m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
			
		SetThink( FuncThink );
		pev->nextthink = gpGlobals->time + 1;

		return;
	}
//	if (m_fInAttack == 2 )
//	CLIENT_COMMAND(m_pPlayer->edict(), "+use\n");

	/*else if (m_fInAttack == 2 )
	CLIENT_COMMAND(m_pPlayer->edict(), "+use\n");
	else
	CLIENT_COMMAND(m_pPlayer->edict(), "+use\n");*/

//	ALERT( at_console, "m_fInAttack: %f\n", m_fInAttack );
/*
	CLIENT_COMMAND(m_pPlayer->edict(), "-use\n");
*/
	m_flTimeWeaponIdle = gpGlobals->time + 0.01;
}

class CPDAAmmoClip : public CBasePlayerAmmo
{
};