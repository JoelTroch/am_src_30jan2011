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

//====================
// NEW HL 2WEAPON FILE
//====================
#if _CODE_REBIRTH

// This weapon it's going to be used on C17's derivation. Please don't kill me Valve. There is not any
// leaked code!

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

enum hl2shotgun_e {
	SHOTGUN_IDLE = 0,
	SHOTGUN_FIRE,
	SHOTGUN_FIRE2,
	SHOTGUN_RELOAD,
	SHOTGUN_PUMP,
	SHOTGUN_START_RELOAD,
	SHOTGUN_DRAW,
	SHOTGUN_HOLSTER,
	SHOTGUN_IDLE4,
	SHOTGUN_IDLE_DEEP
};

LINK_ENTITY_TO_CLASS( weapon_hl2_shotgun, CHL2Shotgun );

void CHL2Shotgun::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_hl2_shotgun"); // hack to allow for old names
	Precache( );
	m_iId = WEAPON_HL2_SHOTGUN;
	SET_MODEL(ENT(pev), "models/hl2/weapons/w_shotgun.mdl");

	m_iDefaultAmmo = SHOTGUN_DEFAULT_GIVE;

	FallInit();// get ready to fall
}


void CHL2Shotgun::Precache( void )
{
	PRECACHE_MODEL("models/hl2/weapons/w_shotgun.mdl");
	PRECACHE_MODEL("models/hl2/weapons/p_shotgun.mdl");
	PRECACHE_MODEL("models/hl2/weapons/v_shotgun.mdl");

//	m_iShell = PRECACHE_MODEL ("models/weapons/shell_buckshot.mdl");// shotgun shell

	PRECACHE_SOUND("items/9mmclip1.wav");              

	PRECACHE_SOUND ("weapons/spas12/spas_hl2_fire1.wav");//shotgun
	PRECACHE_SOUND ("weapons/spas12/spas_hl2_fire2.wav");//shotgun

	PRECACHE_SOUND ("weapons/spas12/spas_hl2_reload1.wav");
	PRECACHE_SOUND ("weapons/spas12/spas_hl2_reload2.wav");
	
	PRECACHE_SOUND ("weapons/spas12/spas12-pump.wav");	// cock gun

	m_usSingleFire = PRECACHE_EVENT( 1, "scripts/events/hl2shotgun1.sc" );
	m_usRealDoubleFire = PRECACHE_EVENT( 1, "scripts/events/hl2shotgun2.sc" );

	m_usDoubleFire = PRECACHE_EVENT( 1, "scripts/events/shotgun2.sc" );//this is used for spas shell

}

int CHL2Shotgun::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#Pickup_Shotgun"); //digamos al cliente

//		m_pPlayer->SetSuitUpdate("!HEV_SHOTGUN", FALSE, SUIT_NEXT_IN_5SEC);
		m_pPlayer->SetSuitUpdate("!HEV_PISTOL", FALSE, SUIT_NEXT_IN_5SEC);

		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}


int CHL2Shotgun::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "buckshot";
	p->iMaxAmmo1 = BUCKSHOT_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = SHOTGUN_MAX_CLIP;
	p->iSlot = 4;
	p->iPosition = 2;
    p->iFlags    = ITEM_FLAG_SELECTONEMPTY;
	p->iId = m_iId = WEAPON_HL2_SHOTGUN;
	p->iWeight = SHOTGUN_WEIGHT;
	p->weaponName = "SPAS-12";

	return 1;
}



BOOL CHL2Shotgun::Deploy( )
{
	m_pPlayer->b_UsingHL2Weapon = TRUE;//so we will use the HEV's flashlight

	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/weapon_deploy.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
	return DefaultDeploy( "models/hl2/weapons/v_shotgun.mdl", "models/hl2/weapons/p_shotgun.mdl", SHOTGUN_DRAW, "spas12" );
}

void CHL2Shotgun::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->b_UsingHL2Weapon = FALSE;//so we will use normal flashlight

	m_fInReload = FALSE;// cancel any reload in progress.
}
void CHL2Shotgun::SecondaryAttack()
{	
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3 )
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15;
		return;
	}

	if (m_iClip == 1)
	{
		PrimaryAttack();
		return;
	}

	if (m_iClip == 0)
	{
		PlayEmptySound( );
		Reload( );
		return;
	}

	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_iClip -= 2;

	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	Vector vecDir;
		
	vecDir = m_pPlayer->FireBulletsPlayer( 12, vecSrc, vecAiming, VECTOR_CONE_10DEGREES, 2048, BULLET_PLAYER_BUCKSHOT, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );


	PLAYBACK_EVENT_FULL( 0, m_pPlayer->edict(), m_usRealDoubleFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, pev->body, 0, 0, 0 );

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);


	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.5;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.5;

	if (m_iClip != 0) m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 6.0;
	else	        m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.9;
	m_iChargeLevel = 0;

	m_pPlayer->pev->punchangle.x -= 5;
}


void CHL2Shotgun::PrimaryAttack()
{
	if (!(m_pPlayer->m_afButtonPressed & IN_ATTACK))
    return;

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


		m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

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
			vecDir = m_pPlayer->FireBulletsPlayer( 4, vecSrc, vecAiming, VECTOR_CONE_DM_SHOTGUN, 2048, BULLET_PLAYER_BUCKSHOT, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
		}
		else
		{
	// ### COD RECOIL START ###
				if ( m_pPlayer->pev->flags & FL_ONGROUND ) 
				{	
					if ( m_pPlayer->pev->flags & FL_DUCKING ) 
					{	//  si esta en el suelo y agachado
						vecDir = m_pPlayer->FireBulletsPlayer( 6, vecSrc, vecAiming, VECTOR_CONE_8DEGREES, 2048, BULLET_PLAYER_BUCKSHOT, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
					}
					else // si no es porq esta parado
					{																//perfe 3d
						vecDir = m_pPlayer->FireBulletsPlayer( 6, vecSrc, vecAiming, VECTOR_CONE_4DEGREES, 2048, BULLET_PLAYER_BUCKSHOT, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
					}
				}
				else // si no esta en el suelo es porque esta en el aire
				{
					if ( m_pPlayer->pev->flags & FL_DUCKING ) 
					{// y si esta agachado en el aire gana un punto
						vecDir = m_pPlayer->FireBulletsPlayer( 6, vecSrc, vecAiming, VECTOR_CONE_15DEGREES, 2048, BULLET_PLAYER_BUCKSHOT, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
					}						// 15 puntos igual
					else
					{ // y si no esta agachado 15 puntos (still on air)
						vecDir = m_pPlayer->FireBulletsPlayer( 6, vecSrc, vecAiming, VECTOR_CONE_15DEGREES, 2048, BULLET_PLAYER_BUCKSHOT, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
					}
				}
	// ### COD RECOIL END ###

		//	vecDir = m_pPlayer->FireBulletsPlayer( 6, vecSrc, vecAiming, VECTOR_CONE_10DEGREES, 2048, BULLET_PLAYER_BUCKSHOT, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
		}

		m_pPlayer->pev->velocity = m_pPlayer->pev->velocity - gpGlobals->v_forward * 8 * 15; //*8 * 5

		PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usSingleFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );
			
	//	SetThink( SpasShell );		
	//	pev->nextthink = gpGlobals->time + 0.7;
//the player fired the gun!!
	//	NeedPump = TRUE;

		if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
			// HEV suit - indicate out of ammo condition
			m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

		if (m_iClip != 0)
			m_flPumpTime = gpGlobals->time + 0.7;//call the pump!!!

		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.7;//1.3
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.7;
		if (m_iClip != 0)
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 5;
		else
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.75;

		m_fInSpecialReload = 0;

// HEV suit - indicate warning ammo condition 
if (m_iClip == 4)
m_pPlayer->SetSuitUpdate("!HEV_AMMO_WAR0", FALSE, 0); 
}

void CHL2Shotgun::SpasShell(void)
{/*
	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

#ifndef CLIENT_DLL
	PLAYBACK_EVENT( flags, m_pPlayer->edict(), m_usDoubleFire );
	ALERT (at_console, "calling m_usDoubleFire\n");
#endif
	//return;
	pev->nextthink = gpGlobals->time + 0.1;
	*/
}
/*
void CHL2Shotgun::SecondaryAttack( void )
{
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15;
		return;
	}

	if (m_iClip <= 1)
	{
		Reload( );
		PlayEmptySound( );
		return;
	}

	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_iClip -= 2;


	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

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
		// tuned for deathmatch
		vecDir = m_pPlayer->FireBulletsPlayer( 8, vecSrc, vecAiming, VECTOR_CONE_DM_DOUBLESHOTGUN, 2048, BULLET_PLAYER_BUCKSHOT, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
	}
	else
	{
		// untouched default single player
		vecDir = m_pPlayer->FireBulletsPlayer( 12, vecSrc, vecAiming, VECTOR_CONE_10DEGREES, 2048, BULLET_PLAYER_BUCKSHOT, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
	}
		
	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usDoubleFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	if (m_iClip != 0)
		m_flPumpTime = gpGlobals->time + 0.95;

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.5;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.5;
	if (m_iClip != 0)
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 6.0;
	else
		m_flTimeWeaponIdle = 1.5;

	m_fInSpecialReload = 0;

}
*/

void CHL2Shotgun::Reload( void )
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == SHOTGUN_MAX_CLIP)
		return;

	// don't reload until recoil is done
	if (m_flNextPrimaryAttack > UTIL_WeaponTimeBase())
		return;

	// check to see if we're ready to reload
	if (m_fInSpecialReload == 0)
	{
		SendWeaponAnim( SHOTGUN_START_RELOAD );
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
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/spas12/spas_hl2_reload1.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0,0x1f));
		else
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/spas12/spas_hl2_reload2.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0,0x1f));

		SendWeaponAnim( SHOTGUN_RELOAD );
		m_pPlayer->SetAnimation( PLAYER_RELOAD ); 

		m_flNextReload = UTIL_WeaponTimeBase() + 0.3;//this??
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.3;
	}
	else
	{
		// Add them to the clip
		m_iClip += 1;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 1;
		m_fInSpecialReload = 1;
	}
}


void CHL2Shotgun::WeaponIdle( void )
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

		PLAYBACK_EVENT( flags, m_pPlayer->edict(), m_usDoubleFire );
		ALERT (at_console, "calling m_usDoubleFire\n");

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
				SendWeaponAnim( SHOTGUN_PUMP );
				
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
				iAnim = SHOTGUN_IDLE;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (60.0/12.0);// * RANDOM_LONG(2, 5);
			}
			else if (flRand <= 0.95)
			{
				iAnim = SHOTGUN_IDLE;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (20.0/9.0);
			}
			else
			{
				iAnim = SHOTGUN_IDLE;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (20.0/9.0);
			}
			SendWeaponAnim( iAnim );
		}
	}
}
#endif