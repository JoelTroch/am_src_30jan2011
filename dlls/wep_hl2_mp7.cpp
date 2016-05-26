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
#include "soundent.h"
#include "gamerules.h"


enum hl2mp5_e
{
	MP5_LONGIDLE = 0,
	MP5_IDLE1,
	MP5_LAUNCH,
	MP5_RELOAD,
	MP5_DEPLOY,
	MP5_FIRE1,
	MP5_FIRE2,
	MP5_FIRE3,
};


LINK_ENTITY_TO_CLASS( weapon_hl2_mp7, CP90 );
//=========================================================
//=========================================================
void CP90::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_hl2_mp7"); // hack to allow for old names
	Precache( );
	SET_MODEL(ENT(pev), "models/hl2/weapons/w_mp7.mdl");
	m_iId = WEAPON_HL2_MP7;

	m_iDefaultAmmo = P90_DEFAULT_GIVE; // 

	FallInit();// get ready to fall down.
}


void CP90::Precache( void )
{
	PRECACHE_MODEL("models/hl2/weapons/v_mp7.mdl");
	PRECACHE_MODEL("models/hl2/weapons/p_mp7.mdl");
	PRECACHE_MODEL("models/hl2/weapons/w_mp7.mdl");

	m_iShell = PRECACHE_MODEL ("models/weapons/shell_9mm.mdl");// brass shellTE_MODEL

	PRECACHE_SOUND ("weapons/mp7/mp7_fire-1.wav");// H to the K
	PRECACHE_SOUND ("weapons/mp7/mp7_fire-2.wav");// H to the K

//	PRECACHE_SOUND ("weapons/pistol_reload.wav");

	m_usP90 = PRECACHE_EVENT( 1, "scripts/events/ump.sc" );
}

int CP90::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "P90";
	p->iMaxAmmo1 = 120;//P90_MAX_CARRY
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1; //M203_GRENADE_MAX_CARRY;
	p->iMaxClip = 30; //P90_DEFAULT_GIVE//CLIP
	p->iSlot = 2;
	p->iPosition = 1;
    p->iFlags    = ITEM_FLAG_SELECTONEMPTY;
	p->iId = m_iId = WEAPON_HL2_MP7;
	p->iWeight = P90_WEIGHT;
	p->weaponName = "MP-7";


	return 1;
}

int CP90::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#Pickup_MP7"); //digamos al cliente

//		m_pPlayer->SetSuitUpdate("!HEV_ASSAULT", FALSE, SUIT_NEXT_IN_5SEC);
		m_pPlayer->SetSuitUpdate("!HEV_PISTOL", FALSE, SUIT_NEXT_IN_5SEC);

		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();

		return TRUE;
	}
	return FALSE;
}

BOOL CP90::Deploy( )
{
	m_pPlayer->b_UsingHL2Weapon = TRUE;//so we will use the HEV's flashlight

	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/weapon_deploy.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
	return DefaultDeploy("models/hl2/weapons/v_mp7.mdl", "models/hl2/weapons/p_mp7.mdl", MP5_DEPLOY, "mp5a4" );
}

void CP90::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->b_UsingHL2Weapon = FALSE;//so we will use normal flashlight

	m_fInReload = FALSE;// cancel any reload in progress.
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", RANDOM_FLOAT(0.8, 0.9), ATTN_NORM);

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.5; //0.5
	/*
	if (m_iClip)
		SendWeaponAnim( P90_HOLSTER ); //KELLY:? P90_HOLSTER
	else
		SendWeaponAnim( P90_HOLSTER );
		*/
}

void CP90::PrimaryAttack()
{	
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = 0.15;
		return;
	}
	if (m_iClip <= 0)
	{

PlayEmptySound();
m_flNextPrimaryAttack = 0.15;

		/*
		if (!m_fFireOnEmpty) //si no seguis apretando:
		Reload( );
		else // y si seguis con el dedo en el mouse...
		{
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_cock1.wav", 0.8, ATTN_NORM);
			m_flNextPrimaryAttack = 0.15;
		}
		*/
		return;
	}

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_iClip--;

	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
	Vector vecDir;

	// ### COD RECOIL START ###
			if ( m_pPlayer->pev->velocity.Length2D() <= 220 )
			{
				if ( m_pPlayer->pev->flags & FL_ONGROUND ) 
				{	
					if ( m_pPlayer->pev->flags & FL_DUCKING ) 
					{	//  si esta en el suelo y agachado
						vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 8192, BULLET_PLAYER_M16, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
					}
					else // si no es porq esta parado
					{																//perfe 3d
						vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_3DEGREES, 8192, BULLET_PLAYER_M16, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
					}
				}
				else // si no esta en el suelo es porque esta en el aire
				{
					if ( m_pPlayer->pev->flags & FL_DUCKING ) 
					{// y si esta agachado en el aire gana un punto
						vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_10DEGREES, 2048, BULLET_PLAYER_M16, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
					}						// 15 puntos igual
					else
					{ // y si no esta agachado 15 puntos (still on air)
						vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_10DEGREES, 2048, BULLET_PLAYER_M16, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
					}
				}
			}
			else
			{
				if ( m_pPlayer->pev->flags & FL_ONGROUND ) 
				{																//perfe 3d
					vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_8DEGREES, 2048, BULLET_PLAYER_AK74, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
				}
				else // si no esta en el suelo es porque esta en el aire
				{
					vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_20DEGREES, 2048, BULLET_PLAYER_AK74, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
				}
			}
	// ### COD RECOIL END ###

  int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usP90, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		//m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.08; // 0.1; delay

	if ( m_flNextPrimaryAttack < UTIL_WeaponTimeBase() )
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.08; // 0.1; delay

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 20, 30 );

  if(m_pPlayer->pev->flags & FL_DUCKING)
  {
    switch (RANDOM_LONG(0,1))
    {
 	 case 0: m_pPlayer->pev->punchangle.y -= 0.2; break;
 	 case 1: m_pPlayer->pev->punchangle.y += 0.2; break;
    }
    switch (RANDOM_LONG(0,1))
    {
 	 case 0: m_pPlayer->pev->punchangle.x -= 0.2; break;
 	 case 1: m_pPlayer->pev->punchangle.x += 0.2; break;
    }
  }
  else if (m_pPlayer->pev->velocity.Length() > .01)
  {
    switch (RANDOM_LONG(0,1))
    {
 	 case 0: m_pPlayer->pev->punchangle.y -= 0.5; break;
 	 case 1: m_pPlayer->pev->punchangle.y += 0.5; break;
    }
    switch (RANDOM_LONG(0,1))
    {
 	 case 0: m_pPlayer->pev->punchangle.x -= 0.5; break;
 	 case 1: m_pPlayer->pev->punchangle.x += 0.5; break;
    }
  }
else
  {
    switch (RANDOM_LONG(0,1))
    {
 	 case 0: m_pPlayer->pev->punchangle.y -= .3; break;
 	 case 1: m_pPlayer->pev->punchangle.y += .3; break;
    }
    switch (RANDOM_LONG(0,1))
    {
 	 case 0: m_pPlayer->pev->punchangle.x -= .3; break;
 	 case 1: m_pPlayer->pev->punchangle.x += .3; break;
    }
  }
// HEV suit - indicate warning ammo condition 
if (m_iClip == 4)
m_pPlayer->SetSuitUpdate("!HEV_AMMO_WAR0", FALSE, 0); 

}

void CP90::Reload( void )
{
	int iResult;

	if (m_iClip == 0)
		iResult = DefaultReload( 30, MP5_RELOAD, 2.4 );
	else
		iResult = DefaultReload( 30, MP5_RELOAD, 2.4 );

	if (iResult)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
		m_pPlayer->SetAnimation( PLAYER_RELOAD ); 
	}
	//m_pPlayer->SetAnimation( PLAYER_RELOAD );
}

void CP90::WeaponIdle( void )
{
	
	ResetEmptySound( );

	m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:	
		iAnim = MP5_IDLE1;	
		break;
	
	default:
	case 1:
		iAnim = MP5_IDLE1;
		break;
	}

	SendWeaponAnim( iAnim );

	m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 20, 30 ); // how long till we do this again.

}

class CP90AmmoClip : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/weapons/mp7/w_mp7clip.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/weapons/mp7/w_mp7clip.mdl");
//		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		int bResult = (pOther->GiveAmmo( 30, "P90", P90_MAX_CARRY) != -1);
		if (bResult)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
		}
		return bResult;
	}
};
LINK_ENTITY_TO_CLASS( ammo_mp7clip, CP90AmmoClip );
#endif