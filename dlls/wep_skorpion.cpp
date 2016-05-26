/***
*
*		   °°
*			 °     °°°°°
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


#define SKORPION_MODEL_1STPERSON "models/weapons/bizon/v_bizon.mdl"
#define SKORPION_MODEL_3RDPERSON "models/weapons/bizon/w_bizon.mdl"
#define SKORPION_DEFAULT_AMMO 64
#define SKORPION_MAX_AMMO _9MM_MAX_CARRY
#define SKORPION_WEIGHT 666

enum skorpion_e
{
	SKORPION_IDLE1 = 0,
	SKORPION_RELOAD,
	SKORPION_DEPLOY,
	SKORPION_FIRE1,
	SKORPION_FIRE2,
	SKORPION_FIRE3
};

LINK_ENTITY_TO_CLASS( weapon_skorpion, CSkorpion );
LINK_ENTITY_TO_CLASS( weapon_bizon, CSkorpion );


//=========================================================
//=========================================================

void CSkorpion::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_skorpion"); // hack to allow for old names
	Precache( );
	
	if (!FStringNull (v_model) )
	SET_MODEL( ENT(pev), STRING(w_model) );
	else
	SET_MODEL(ENT(pev), "models/weapons/bizon/w_bizon.mdl");
	m_iId = WEAPON_SKORPION;

	m_iDefaultAmmo = SKORPION_DEFAULT_AMMO;

	FallInit();// get ready to fall down.
}


void CSkorpion::Precache( void )
{
	if (!FStringNull (v_model) )
	{
		PRECACHE_MODEL( (char *)STRING(v_model) );
#ifdef CLIENT_DLL
		if ( bIsMultiplayer() )
#else
		if ( g_pGameRules->IsMultiplayer() )
#endif
		PRECACHE_MODEL( (char *)STRING(p_model) );
		PRECACHE_MODEL( (char *)STRING(w_model) );
	}
	else
	{
		PRECACHE_MODEL("models/weapons/bizon/v_bizon.mdl");
		PRECACHE_MODEL("models/weapons/bizon/w_bizon.mdl");
#ifdef CLIENT_DLL
		if ( bIsMultiplayer() )
#else
		if ( g_pGameRules->IsMultiplayer() )
#endif
		PRECACHE_MODEL("models/weapons/bizon/p_bizon.mdl");
	}

	PRECACHE_SOUND ("weapons/skorpion/skorpion_fire-1.wav");// H to the K
}

void CSkorpion::KeyValue( KeyValueData *pkvd )//this sets for custom fields of weapon_generic. G-Cont.
{
	if (FStrEq(pkvd->szKeyName, "m_iszModel"))
	{
		char string[64];

		sprintf(string, "models/weapons/bizon/v_%s.mdl", pkvd->szValue);
		v_model = ALLOC_STRING(string);

		sprintf(string, "models/weapons/bizon/p_%s.mdl", pkvd->szValue);
		p_model = ALLOC_STRING(string);

		sprintf(string, "models/weapons/bizon/w_%s.mdl", pkvd->szValue);
		w_model = ALLOC_STRING(string);

		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

int CSkorpion::GetItemInfo(ItemInfo *p)
{
    p->pszName   = STRING(pev->classname);
    p->pszAmmo1  = "9mm";              // The type of ammo it uses
    p->iMaxAmmo1 = SKORPION_MAX_AMMO;            // Max ammo the player can carry
    p->pszAmmo2  = NULL;                    // No secondary ammo
    p->iMaxAmmo2 = -1;
    p->iMaxClip  = SKORPION_DEFAULT_AMMO;        // The clip size
    p->iSlot     = 2;						 // The number in the HUD
    p->iPosition = 2;						  // The position in a HUD slot
    p->iFlags    = ITEM_FLAG_SELECTONEMPTY;
    p->iId       = m_iId = WEAPON_SKORPION;      // The weapon id
    p->iWeight   = SKORPION_WEIGHT; 
	p->weaponName = "Bizon";

	return 1;
}

int CSkorpion::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#Pickup_Skorpion"); //digamos al cliente

		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}

BOOL CSkorpion::Deploy( )
{	
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/weapon_deploy.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));

	//m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 3.1;
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.53;

	if (!FStringNull (v_model) )
	{
		m_pPlayer->pev->viewmodel = v_model;
//		m_pPlayer->pev->weaponmodel = p_model;
	
		SendWeaponAnim( SKORPION_DEPLOY, 1, 0 );	
		return TRUE;
	}
	else
	return DefaultDeploy( SKORPION_MODEL_1STPERSON, SKORPION_MODEL_3RDPERSON, SKORPION_DEPLOY, "minigun" ); //"mp5"
}

void CSkorpion::Holster( int skiplocal /* = 0 */ )
{
	m_fInReload = FALSE;// cancel any reload in progress.
	m_pPlayer->m_bIron = FALSE;

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
}

void CSkorpion::PrimaryAttack()
{
/*	ALERT( at_console, "iFiremode %i\n", iFiremode);
	if(	iFiremode == FIREMODE_SAFE )
	{
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_cock1.wav", 0.8, ATTN_NORM);
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	if(	iFiremode == FIREMODE_SEMI )
	{
	//	SendWeaponAnim( SKORPION_FIRE1 );

		if (!(m_pPlayer->m_afButtonPressed & IN_ATTACK))
		return;
	}
	*/
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	if (m_iClip <= 0) 
	{ 
		if (m_fFireOnEmpty) 
		{ 
			//PlayEmptySound();//for some reason, doesn't work
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_cock1.wav", 0.8, ATTN_NORM);
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2; 
		} 
		return; 
	} 

	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	// one less round in the clip
	int iShots = 1;

/*	if(	iFiremode == FIREMODE_BURST )
	{
		if (m_iClip >= 3)
			iShots = 3;
		if (m_iClip == 2)
			iShots = 2;

		if (m_iClip == 1)
			iShots = 1;

		m_iClip -= iShots;
	}
	else*/
	m_iClip--;


	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );		


	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

/*	if(	iFiremode == FIREMODE_BURST )
	Vector vecDir = m_pPlayer->FireBulletsThroughWalls( iShots, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 8192, BULLET_PLAYER_9MM, 2, 0, m_pPlayer->pev, m_pPlayer->random_seed );
	else*/
	Vector vecDir = m_pPlayer->FireBulletsThroughWalls( 1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 8192, BULLET_PLAYER_9MM, 2, 0, m_pPlayer->pev, m_pPlayer->random_seed );


	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/skorpion/skorpion_fire-1.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));

	SendWeaponAnim( SKORPION_FIRE1 );

	float iDelay = 0.05;
	
/*	if(	iFiremode == FIREMODE_BURST )
	iDelay = 0.5;*/
	
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + iDelay;

	if ( m_flNextPrimaryAttack < UTIL_WeaponTimeBase() )
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + iDelay;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}
void CSkorpion::SecondaryAttack()
{	
 	if ( m_pPlayer->m_bIron )
	m_pPlayer->m_bIron = FALSE;
	else
	m_pPlayer->m_bIron = TRUE;
	
	m_flNextSecondaryAttack = m_flNextSecondaryAttack + 0.5;

	return; // return y no ejecutar más codigo.
}
void CSkorpion::Reload( void )
{
	int iResult;

	iResult = DefaultReload( 64, SKORPION_RELOAD, 3.28 );

	if (iResult)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
		m_pPlayer->SetAnimation( PLAYER_RELOAD ); 
	}
}


void CSkorpion::WeaponIdle( void )
{
	ResetEmptySound( );

	m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	SendWeaponAnim( SKORPION_IDLE1 );

	m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 ); // how long till we do this again.
}
//NOT PRECACHED ATM
/*
class CSkorpionAmmoClip : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/weapons/skorpion/w_skorpionclip.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/weapons/skorpion/w_skorpionclip.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		int bResult = (pOther->GiveAmmo( M249_DEFAULT_AMMO, "ammo_skorpion", M249_MAX_AMMO) != -1);
		if (bResult)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
		}
		return bResult;
	}
};
LINK_ENTITY_TO_CLASS( ammo_skorpion, CSkorpionAmmoClip );*/

