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

#if !defined( OEM_BUILD ) && !defined( HLDEMO_BUILD )

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "monsters.h"
#include "player.h"
#include "gamerules.h"
#include "shake.h" //This is required for the screen fade

enum python_e 
{/*
	PYTHON_IDLE1 = 0,
	PYTHON_FIRE1,
	PYTHON_FIRE2,
	PYTHON_RELOAD_HAS_5,
	PYTHON_RELOAD_HAS_4,
	PYTHON_RELOAD_HAS_3,
	PYTHON_RELOAD_HAS_2,
	PYTHON_RELOAD_HAS_1,
	PYTHON_RELOAD_HAS_0,
	PYTHON_RELOAD_INSERT_1,
	PYTHON_RELOAD_INSERT_2,
	PYTHON_RELOAD_INSERT_3,
	PYTHON_RELOAD_INSERT_4,
	PYTHON_RELOAD_INSERT_5,
	PYTHON_RELOAD_INSERT_6,
	PYTHON_RELOAD_END,
	PYTHON_DEPLOY,
	PYTHON_HOLSTER*/
	
	PYTHON_IDLE1 = 0,
	PYTHON_FIRE1,
	PYTHON_FIRE2,
	PYTHON_FIRE_EMPTY,
	PYTHON_RELOAD,
	PYTHON_DEPLOY
};

LINK_ENTITY_TO_CLASS( weapon_python, CPython );
LINK_ENTITY_TO_CLASS( weapon_357, CPython );

int CPython::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "357";
	p->iMaxAmmo1 = _357_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = PYTHON_MAX_CLIP;
    p->iFlags    = ITEM_FLAG_SELECTONEMPTY;
	p->iSlot = 1;
	p->iPosition = 4;
	p->iId = m_iId = WEAPON_PYTHON;
	p->iWeight = PYTHON_WEIGHT;
	p->weaponName = "Colt Anaconda";

	return 1;
}

int CPython::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#Pickup_357"); //digamos al cliente

		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}

void CPython::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_357"); // hack to allow for old names
	Precache( );
	m_iId = WEAPON_PYTHON;
	
	if (!FStringNull (v_model) )
	SET_MODEL( ENT(pev), STRING(w_model) );
	else
	SET_MODEL(ENT(pev), "models/weapons/357/w_357.mdl");

	m_iDefaultAmmo = PYTHON_DEFAULT_GIVE;

	FallInit();// get ready to fall down.
}

void CPython::KeyValue( KeyValueData *pkvd )//this sets for custom fields of weapon_generic. G-Cont.
{
	if (FStrEq(pkvd->szKeyName, "m_iszModel"))
	{
		char string[64];

		sprintf(string, "models/weapons/357/v_%s.mdl", pkvd->szValue);
		v_model = ALLOC_STRING(string);

		sprintf(string, "models/weapons/357/p_%s.mdl", pkvd->szValue);
		p_model = ALLOC_STRING(string);

		sprintf(string, "models/weapons/357/w_%s.mdl", pkvd->szValue);
		w_model = ALLOC_STRING(string);

		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

void CPython::Precache( void )
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
		PRECACHE_MODEL("models/weapons/357/v_357.mdl");
		PRECACHE_MODEL("models/weapons/357/w_357.mdl");
#ifdef CLIENT_DLL
		if ( bIsMultiplayer() )
#else
		if ( g_pGameRules->IsMultiplayer() )
#endif
		PRECACHE_MODEL("models/weapons/357/p_357.mdl");
	}

	PRECACHE_MODEL("models/weapons/357/v_357_scope.mdl");

	PRECACHE_MODEL("models/w_357ammobox.mdl");
	PRECACHE_SOUND("items/9mmclip1.wav");              

	PRECACHE_SOUND ("weapons/357_reload1.wav");
	PRECACHE_SOUND ("weapons/357_cock1.wav");

	PRECACHE_SOUND ("weapons/357/357_reload-1.wav");
	PRECACHE_SOUND ("weapons/357/357_reload-2.wav");

	PRECACHE_SOUND ("weapons/357/357_FIRE-1.wav");
	PRECACHE_SOUND ("weapons/357/357_FIRE-2.wav");

	m_usFirePython = PRECACHE_EVENT( 1, "scripts/events/python.sc" );
}

BOOL CPython::Deploy( )
{
#ifdef CLIENT_DLL
	if ( bIsMultiplayer() )
#else
	if ( g_pGameRules->IsMultiplayer() )
#endif
	{
		// enable laser sight geometry.
		pev->body = 1;
	}
	else
	{
		pev->body = 0;
	}

	if (!FStringNull (v_model) )
	{
		m_pPlayer->pev->viewmodel = v_model;
//		m_pPlayer->pev->weaponmodel = p_model;
	
		SendWeaponAnim( PYTHON_DEPLOY, 1, 0 );	
		return TRUE;
	}
	else
	return DefaultDeploy( "models/weapons/357/v_357.mdl", "models/weapons/357/p_357.mdl", PYTHON_DEPLOY, "de", UseDecrement(), pev->body );
}


void CPython::Holster( int skiplocal /* = 0 */ )
{
	if (m_fInZoom == TRUE) 
	{
		m_pPlayer->b_EstaEnZoom = FALSE;

		#ifndef CLIENT_DLL
				if (!FStringNull (v_model) )
				m_pPlayer->pev->viewmodel = v_model;
				else
				m_pPlayer->pev->viewmodel = MAKE_STRING("models/weapons/357/v_357.mdl");
		#else
				if (!FStringNull (v_model) )
				m_pPlayer->pev->viewmodel = v_model;
				else
				LoadVModel ( "models/weapons/357/v_357.mdl", m_pPlayer );
		#endif
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0; // 0 means reset to default fov
		m_fInZoom = FALSE;
	}

	m_fInReload = FALSE;// cancel any reload in progress.

//	if ( m_fInZoom )
//	{
//		SecondaryAttack();
//	}

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
	SendWeaponAnim( PYTHON_IDLE1 );//PYTHON_HOLSTER
}

void CPython::SecondaryAttack()
{
	if (m_fInZoom == FALSE) //false
	{
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 45;
		m_fInZoom = TRUE;//**ON**
		m_pPlayer->b_EstaEnZoom = TRUE;

		#ifndef CLIENT_DLL
			m_pPlayer->pev->viewmodel = MAKE_STRING("models/weapons/357/v_357_scope.mdl");
		#else
			LoadVModel ( "models/weapons/357/v_357_scope.mdl", m_pPlayer ); 
		#endif								

		UTIL_ScreenFade( m_pPlayer, Vector(0,0,0), 1, 0.1, 255, FFADE_IN); //fade to red 
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/zoom.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));   
	}
	else
	{
		m_pPlayer->b_EstaEnZoom = FALSE;

		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0;
		m_fInZoom = FALSE;//**OFF**

		#ifndef CLIENT_DLL
				if (!FStringNull (v_model) )
				m_pPlayer->pev->viewmodel = v_model;
				else
				m_pPlayer->pev->viewmodel = MAKE_STRING("models/weapons/357/v_357.mdl");//model default
		#else
				if (!FStringNull (v_model) )
				m_pPlayer->pev->viewmodel = v_model;
				else
				LoadVModel ( "models/weapons/357/v_357.mdl", m_pPlayer );
		#endif

		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/zoom.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));   
	}

//	pev->nextthink = UTIL_WeaponTimeBase() + 0.1;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5; 
}

void CPython::PrimaryAttack()
{
	if (!(m_pPlayer->m_afButtonPressed & IN_ATTACK))
    return;

	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	if (m_iClip <= 0)
	{
		if (!m_fFireOnEmpty)
			Reload( );
		else
		{
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_cock1.wav", 0.8, ATTN_NORM);
			m_flNextPrimaryAttack = 0.15;
		}

		return;
	}

	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	m_iClip--;

//	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );

	Vector vecDir;
	if (m_fInZoom == TRUE)//si tiene zoom
	{
		vecDir = m_pPlayer->FireBulletsThroughWalls( 1, vecSrc, vecAiming, VECTOR_CONE_PERFECT, 8192, BULLET_PLAYER_357, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
//		ALERT(at_console, "VECTOR_CONE_PERFECT\n");
	}
	else// FALSE
	{
		vecDir = m_pPlayer->FireBulletsThroughWalls( 1, vecSrc, vecAiming, VECTOR_CONE_5DEGREES, 8192, BULLET_PLAYER_357, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
//		ALERT(at_console, "VECTOR_CONE_5DEGREES\n");
	}

	m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * 8 * 15; //*8 * 5

	//FireBulletsPlayer
    int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usFirePython, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );

	m_flNextPrimaryAttack = 0.75;
	m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}

void CPython::Reload( void )
{
	if (m_fInZoom == TRUE) 
	{
		m_pPlayer->b_EstaEnZoom = FALSE;

		#ifndef CLIENT_DLL
				m_pPlayer->pev->viewmodel = MAKE_STRING("models/weapons/357/v_357.mdl");
		#else
				LoadVModel ( "models/weapons/357/v_357.mdl", m_pPlayer );
		#endif
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0; // 0 means reset to default fov
		m_fInZoom = FALSE;
	}

	int iResult;

	if (m_iClip == 0)
		iResult = DefaultReload( 7, PYTHON_RELOAD, 2.53 );
	else
		iResult = DefaultReload( 7, PYTHON_RELOAD, 2.2 );

	if (iResult)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
		m_pPlayer->SetAnimation( PLAYER_RELOAD ); 
			
		Vector vecSrc	 = m_pPlayer->GetGunPosition( );
		CBaseEntity *pEntity = Create( "item_clip_pistol", vecSrc, pev->angles );		
	}

/*
//	SHOTGUN_MAX_CLIP
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == PYTHON_MAX_CLIP)
		return;

	// don't reload until recoil is done
	if (m_flNextPrimaryAttack > UTIL_WeaponTimeBase())
		return;

	// check to see if we're ready to reload
	if (m_fInSpecialReload == 0)
	{
		//SendWeaponAnim( PYTHON_RELOAD_HAS_0 );
		
		 if (m_iClip == 0)
        SendWeaponAnim( PYTHON_RELOAD_HAS_0 );
		 else if (m_iClip == 1)
			SendWeaponAnim( PYTHON_RELOAD_HAS_1 );
		 else if (m_iClip == 2)
			SendWeaponAnim( PYTHON_RELOAD_HAS_2 );
		 else if (m_iClip == 3)
			SendWeaponAnim( PYTHON_RELOAD_HAS_3 );
		 else if (m_iClip == 4)
			SendWeaponAnim( PYTHON_RELOAD_HAS_4 );
		 else if (m_iClip == 5)
			SendWeaponAnim( PYTHON_RELOAD_HAS_5 );

		m_fInSpecialReload = 1;
		//0.6
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 2.6;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.6;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
		return;
	}
	else if (m_fInSpecialReload == 1)
	{
		if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
			return;
		// was waiting for gun to move to side
		m_fInSpecialReload = 2;

		if (RANDOM_LONG(0,1))
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/357/357_reload-1.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0,0x1f));
		else
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/357/357_reload-2.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0,0x1f));

	//	SendWeaponAnim( PYTHON_RELOAD_INSERT_6 );
		
		if (m_iClip == 0)
        SendWeaponAnim( PYTHON_RELOAD_INSERT_1 );
		 else if (m_iClip == 1)
			SendWeaponAnim( PYTHON_RELOAD_INSERT_2 );
		 else if (m_iClip == 2)
			SendWeaponAnim( PYTHON_RELOAD_INSERT_3 );
		 else if (m_iClip == 3)
			SendWeaponAnim( PYTHON_RELOAD_INSERT_4 );
		 else if (m_iClip == 4)
			SendWeaponAnim( PYTHON_RELOAD_INSERT_5 );
		 else if (m_iClip == 5)
			SendWeaponAnim( PYTHON_RELOAD_INSERT_6 );

		m_pPlayer->SetAnimation( PLAYER_RELOAD ); 

		m_flNextReload = UTIL_WeaponTimeBase() + 0.5;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
	}
	else
	{
		// Add them to the clip
		m_iClip += 1;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 1;
		m_fInSpecialReload = 1;
	}*/
}


void CPython::WeaponIdle( void )
{
	ResetEmptySound( );

	m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:	
		iAnim = PYTHON_IDLE1;	
		break;
	
	default:
	case 1:
		iAnim = PYTHON_IDLE1;
		break;
	}

	SendWeaponAnim( iAnim );

	m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 ); // how long till we do this again.

/*	ResetEmptySound( );

	m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( m_flPumpTime && m_flPumpTime < gpGlobals->time )
	{
		// play pumping sound
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/shotgun/scock1.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,0x1f));
		m_flPumpTime = 0;
	}

	if (m_flTimeWeaponIdle <  UTIL_WeaponTimeBase() )
	{
		if (m_iClip == 0 && m_fInSpecialReload == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
		{
			Reload( );
		}
		else if (m_fInSpecialReload != 0)
		{
		//	if (m_iClip != 8 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
		
			if (m_iClip != 6 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
			{
				Reload( );
			}
			else
			{
				// reload debounce has timed out
				SendWeaponAnim( PYTHON_RELOAD_END );
				
				// play cocking sound
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/shotgun/scock1.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,0x1f));
				m_fInSpecialReload = 0;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5;
			}
		}
		else
		{
			int iAnim;
			float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0, 1 );
			if (flRand <= 0.8)
			{
				iAnim = PYTHON_IDLE1;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (60.0/12.0);// * RANDOM_LONG(2, 5);
			}
			else if (flRand <= 0.95)
			{
				iAnim = PYTHON_IDLE1;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (20.0/9.0);
			}
			else
			{
				iAnim = PYTHON_IDLE1;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (20.0/9.0);
			}
			SendWeaponAnim( iAnim );
		}
	}*/
}




class CPythonAmmo : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_357ammobox.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_357ammobox.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		if (pOther->GiveAmmo( AMMO_357BOX_GIVE, "357", _357_MAX_CARRY ) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS( ammo_357, CPythonAmmo );


#endif