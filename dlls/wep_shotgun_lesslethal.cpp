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
#include "gamerules.h"

// special deathmatch shotgun spreads
#define VECTOR_CONE_DM_SHOTGUN	Vector( 0.08716, 0.04362, 0.00  )// 10 degrees by 5 degrees
#define VECTOR_CONE_DM_DOUBLESHOTGUN Vector( 0.17365, 0.04362, 0.00 ) // 20 degrees by 5 degrees

enum shotgun_e 
{
	SHOTGUN_IDLE1 = 0,
	SHOTGUN_IDLE2,

	SHOTGUN_RELOAD_START,
	SHOTGUN_RELOAD_INS_1,
	SHOTGUN_RELOAD_END_LAST,
	
	SHOTGUN_DEPLOY,

	SHOTGUN_FIRE1,
	SHOTGUN_FIRE2,

	SHOTGUN_PUMP1,
	SHOTGUN_PUMP2,

	SHOTGUN_DEPLOYFIRST,

	SHOTGUN_HOLSTER,
	SHOTGUN_NEWFIRE1,
	SHOTGUN_NEWFIRE2,
};

LINK_ENTITY_TO_CLASS( weapon_shotgun_non_lethal, CShotgunLessLethal );

void CShotgunLessLethal::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_shotgun_non_lethal"); // hack to allow for old names
	Precache( );
	m_iId = WEAPON_SHOTGUN_LESSLETHAL;
	
	if (!FStringNull (v_model) )
	SET_MODEL( ENT(pev), STRING(w_model) );
	else
	SET_MODEL(ENT(pev), "models/weapons/lesslethal/w_spas12.mdl");

	m_iDefaultAmmo = SHOTGUN_DEFAULT_GIVE;

	FallInit();// get ready to fall
}


void CShotgunLessLethal::Precache( void )
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
		PRECACHE_MODEL("models/weapons/lesslethal/v_spas12.mdl");
		PRECACHE_MODEL("models/weapons/lesslethal/w_spas12.mdl");
#ifdef CLIENT_DLL
		if ( bIsMultiplayer() )
#else
		if ( g_pGameRules->IsMultiplayer() )
#endif
			PRECACHE_MODEL("models/weapons/lesslethal/p_spas12.mdl");
	}

//	m_iShell = PRECACHE_MODEL ("models/weapons/shell_buckshot.mdl");// shotgun shell

	PRECACHE_SOUND("items/9mmclip1.wav");              

	PRECACHE_SOUND ("weapons/lesslethal/lesslethal-fire.wav");//shotgun

	PRECACHE_SOUND ("weapons/shotgun/shotgun_reload-1.wav");	// shotgun reload
	PRECACHE_SOUND ("weapons/shotgun/shotgun_reload-2.wav");	// shotgun reload
	
	PRECACHE_SOUND ("weapons/357_cock1.wav"); // gun empty sound

	PRECACHE_SOUND ("weapons/spas12/spas12-pump.wav");	// cock gun


	m_usSingleFire = PRECACHE_EVENT( 1, "scripts/events/shotgun_lesslethal.sc" );
	
//	m_usSpasShell = PRECACHE_EVENT( 1, "events/spas_shell.sc" );
//	m_usDoubleFire = PRECACHE_EVENT( 1, "scripts/events/shotgun2.sc" );
}

void CShotgunLessLethal::KeyValue( KeyValueData *pkvd )//this sets for custom fields of weapon_generic. G-Cont.
{
	if (FStrEq(pkvd->szKeyName, "m_iszModel"))
	{
		char string[64];

		sprintf(string, "models/weapons/lesslethal/v_%s.mdl", pkvd->szValue);
		v_model = ALLOC_STRING(string);

		sprintf(string, "models/weapons/lesslethal/p_%s.mdl", pkvd->szValue);
		p_model = ALLOC_STRING(string);

		sprintf(string, "models/weapons/lesslethal/w_%s.mdl", pkvd->szValue);
		w_model = ALLOC_STRING(string);

		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

int CShotgunLessLethal::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
				ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#Pickup_Shotgun_lesslethal"); //digamos al cliente

		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}


int CShotgunLessLethal::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "buckshot";//change me
	p->iMaxAmmo1 = BUCKSHOT_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = SHOTGUN_MAX_CLIP;
	p->iSlot = 4; //2
	p->iPosition = 2;
    p->iFlags    = ITEM_FLAG_SELECTONEMPTY;
	p->iId = m_iId = WEAPON_SHOTGUN_LESSLETHAL;
	p->iWeight = SHOTGUN_WEIGHT;
	p->weaponName = "Less Lethal Shotgun";

	return 1;
}



BOOL CShotgunLessLethal::Deploy( )
{
//	m_pPlayer->m_fCanUseFlashlight = TRUE;
	if( bDeployed )
	{
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/weapon_deploy.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));

		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;

		if (!FStringNull (v_model) )
		{
			m_pPlayer->pev->viewmodel = v_model;
	//		m_pPlayer->pev->weaponmodel = p_model;
		
			SendWeaponAnim( SHOTGUN_DEPLOY, 1, 0 );	
			return TRUE;
		}
		else
		return DefaultDeploy( "models/weapons/lesslethal/v_spas12.mdl", "models/weapons/lesslethal/p_spas12.mdl", SHOTGUN_DEPLOY, "benelli" );
	}
	else
	{
		bDeployed = true;

		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/weapon_deploy.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));

		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 2.0;

		if (!FStringNull (v_model) )
		{
			m_pPlayer->pev->viewmodel = v_model;
		
			SendWeaponAnim( SHOTGUN_DEPLOYFIRST, 1, 0 );	
			return TRUE;
		}
		else
		return DefaultDeploy( "models/weapons/lesslethal/v_spas12.mdl", "models/weapons/lesslethal/p_spas12.mdl", SHOTGUN_DEPLOYFIRST, "benelli" );
	}
}

void CShotgunLessLethal::Holster( int skiplocal /* = 0 */ )
{
	m_fInReload = FALSE;// cancel any reload in progress.
		m_pPlayer->m_bIron = FALSE;

}

void CShotgunLessLethal::PrimaryAttack()
{
	if (!(m_pPlayer->m_afButtonPressed & IN_ATTACK))
    return;

//	if( m_fInSpecialReload != 0 )
 //   return;

		// don't fire underwater
		if (m_pPlayer->pev->waterlevel == 3)
		{
			PlayEmptySound( );
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15;
			return;
		}

		if (m_iClip <= 0)
		{
			Reload( );
			if (m_iClip == 0)
				PlayEmptySound( );
			return;
		}

		m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

		m_iClip--;

		int flags;
	#if defined( CLIENT_WEAPONS )
		flags = FEV_NOTHOST;
	#else
		flags = 0;
	#endif


//		m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

		// player "shoot" animation
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

		Vector vecSrc	 = m_pPlayer->GetGunPosition( );
		Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

		Vector vecDir;

	#ifdef CLIENT_DLL
		if ( bIsMultiplayer() )
	#else
		if ( g_pGameRules->IsMultiplayer() )
	#endif
		{
			vecDir = m_pPlayer->FireBulletsPlayer( 4, vecSrc, vecAiming, VECTOR_CONE_DM_SHOTGUN, 2048, BULLET_PLAYER_RUBBER, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
		}
		else
		{
			vecDir = m_pPlayer->FireBulletsPlayer( 8, vecSrc, vecAiming, VECTOR_CONE_5DEGREES, 2048, BULLET_PLAYER_RUBBER, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );					
		}

		//m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * 8 * 15; //*8 * 5

		PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usSingleFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );
		
		if (m_iClip != 0)
			m_flPumpTime = gpGlobals->time + 0.52;//call the pump!!!

		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.15;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.15;
		if (m_iClip != 0)
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 5;
		else
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.75;

		m_fInSpecialReload = 0;
//	}//si need pump
}

void CShotgunLessLethal::SecondaryAttack()
{	
 	if ( m_pPlayer->m_bIron )
	m_pPlayer->m_bIron = FALSE;
	else
	m_pPlayer->m_bIron = TRUE;
	
	m_flNextSecondaryAttack = m_flNextSecondaryAttack + 0.5;

	return; // return y no ejecutar más codigo.
}

void CShotgunLessLethal::Reload( void )
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == SHOTGUN_MAX_CLIP)
		return;

	// don't reload until recoil is done
	if (m_flNextPrimaryAttack > UTIL_WeaponTimeBase())
		return;

	// check to see if we're ready to reload
	if (m_fInSpecialReload == 0)
	{
		SendWeaponAnim( SHOTGUN_RELOAD_START );
		m_fInSpecialReload = 1;
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.6;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.6;
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
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/shotgun/shotgun_reload-1.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0,0x1f));
		else
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/shotgun/shotgun_reload-1.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0,0x1f));

		SendWeaponAnim( SHOTGUN_RELOAD_INS_1 );
		m_pPlayer->SetAnimation( PLAYER_RELOAD ); 

		m_flNextReload = UTIL_WeaponTimeBase() + 0.4;//this?? 0.8
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.4;
	}
	else
	{
		// Add them to the clip
		m_iClip += 1;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 1;
		m_fInSpecialReload = 1;
	}
}


void CShotgunLessLethal::WeaponIdle( void )
{
	ResetEmptySound( );

	m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( m_flPumpTime && m_flPumpTime < gpGlobals->time )
	{	
		int flags;
		#if defined( CLIENT_WEAPONS )
			flags = FEV_NOTHOST;
		#else
			flags = 0;
		#endif

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
			if (m_iClip != 8 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
			{
				Reload( );
			}
			else
			{
				// reload debounce has timed out
				SendWeaponAnim( SHOTGUN_RELOAD_END_LAST );
				
				// play cocking sound
				//EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/spas12/spas12-pump.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,0x1f));
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
				iAnim = SHOTGUN_IDLE1;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (60.0/12.0);// * RANDOM_LONG(2, 5);
			}
			else if (flRand <= 0.95)
			{
				iAnim = SHOTGUN_IDLE2;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (20.0/9.0);
			}
			else
			{
				iAnim = SHOTGUN_IDLE1;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (20.0/9.0);
			}
			SendWeaponAnim( iAnim );
		}
	}
}


/*
class CShotgunLessLethalAmmo : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/weapons/shotgun/w_shotbox.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/weapons/shotgun/w_shotbox.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		if (pOther->GiveAmmo( AMMO_BUCKSHOTBOX_GIVE, "buckshot", BUCKSHOT_MAX_CARRY ) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS( ammo_buckshot, CShotgunLessLethalAmmo );

*/
