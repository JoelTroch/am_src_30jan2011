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

enum Glock18_e 
{
	GLOCK18_IDLE1 = 0,
	GLOCK18_IDLE2,
	GLOCK18_IDLE3,

	GLOCK18_FIRE_BURST_1,
	GLOCK18_FIRE_BURST_2,
	GLOCK18_FIRE,
	GLOCK18_FIRE_VACIO,

	GLOCK18_RELOAD,
	GLOCK18_DEPLOY,
	GLOCK18_HOLSTER,

	GLOCK18_ANIM_FIX,

	GLOCK18_DRAW_2,
	GLOCK18_RELOAD_2
};

LINK_ENTITY_TO_CLASS( weapon_glock18, CGlock18 );

void CGlock18::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_glock18"); // hack to allow for old names
	Precache( );
	m_iId = WEAPON_GLOCK18;

	if (!FStringNull (v_model) )
	SET_MODEL( ENT(pev), STRING(w_model) );
	else
	SET_MODEL(ENT(pev), "models/weapons/glock18/w_glock18.mdl");

	if (pev->armorvalue)
	{ 
		if ((pev->armorvalue > GLOCK18_DEFAULT_GIVE) && (pev->armorvalue < 0))
		pev->armorvalue = GLOCK18_DEFAULT_GIVE;

		m_iDefaultAmmo = pev->armorvalue;
	}
	else
	m_iDefaultAmmo = GLOCK18_DEFAULT_GIVE;

	m_fDefaultAnim = FIREMODE_SEMI;

	FallInit();// get ready to fall down.
}

void CGlock18::KeyValue( KeyValueData *pkvd )//this sets for custom fields of weapon_generic. G-Cont.
{
	if (FStrEq(pkvd->szKeyName, "m_iszModel"))
	{
		char string[64];

		sprintf(string, "models/weapons/glock18/v_%s.mdl", pkvd->szValue);
		v_model = ALLOC_STRING(string);

		sprintf(string, "models/weapons/glock18/p_%s.mdl", pkvd->szValue);
		p_model = ALLOC_STRING(string);

		sprintf(string, "models/weapons/glock18/w_%s.mdl", pkvd->szValue);
		w_model = ALLOC_STRING(string);

		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

void CGlock18::Precache( void )
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
		PRECACHE_MODEL("models/weapons/glock18/v_glock18.mdl");
		PRECACHE_MODEL("models/weapons/glock18/w_glock18.mdl");
#ifdef CLIENT_DLL
		if ( bIsMultiplayer() )
#else
		if ( g_pGameRules->IsMultiplayer() )
#endif
		PRECACHE_MODEL("models/weapons/glock18/p_glock18.mdl");
	}

	m_iShell = PRECACHE_MODEL ("models/weapons/shell_9mm.mdl");// brass shell

	PRECACHE_SOUND ("weapons/glock18/glock18_fire-1.wav");// H to the K
	PRECACHE_SOUND ("weapons/glock18/glock18_fire-2.wav");// H to the K

	m_usFireGlock18 = PRECACHE_EVENT( 1, "scripts/events/glock18.sc" );
	m_usFireGlock18_B = PRECACHE_EVENT( 1, "scripts/events/glock18_burst.sc" );

//	m_usSpriteTracer = PRECACHE_EVENT( 1, "scripts/events/sprite_tracer.sc" );
}

int CGlock18::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "9mm";
	p->iMaxAmmo1 = _9MM_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
/*
	if (m_pPlayer->m_bGlock40shots== TRUE )
	p->iMaxClip = 40;
	else
	p->iMaxClip = 20;
*/
	p->iMaxClip = GLOCK18_MAX_CLIP;
	p->iSlot = 1;
	p->iPosition = 3;
    p->iFlags    = ITEM_FLAG_SELECTONEMPTY;
	p->iId = m_iId = WEAPON_GLOCK18;
	p->iWeight = GLOCK18_WEIGHT;
	p->weaponName = "Glock 18";
	return 1;
}

int CGlock18::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#Pickup_Glock18"); //digamos al cliente

		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}

BOOL CGlock18::Deploy( )
{
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/weapon_deploy.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
	if (RANDOM_LONG(0,1))
	{
		if (!FStringNull (v_model) )
		{
			m_pPlayer->pev->viewmodel = v_model;
//			m_pPlayer->pev->weaponmodel = p_model;
		
			SendWeaponAnim( GLOCK18_DEPLOY, 1, 0 );	
			return TRUE;
		}
		else
		return DefaultDeploy( "models/weapons/glock18/v_glock18.mdl", "models/weapons/glock18/p_glock18.mdl", GLOCK18_DEPLOY, "glock", /*UseDecrement() ? 1 : 0*/ 0 );
	}
	else
	{
		if (!FStringNull (v_model) )
		{
			m_pPlayer->pev->viewmodel = v_model;
//			m_pPlayer->pev->weaponmodel = p_model;
		
			SendWeaponAnim( GLOCK18_DRAW_2, 1, 0 );	
			return TRUE;
		}
		else
		return DefaultDeploy( "models/weapons/glock18/v_glock18.mdl", "models/weapons/glock18/p_glock18.mdl", GLOCK18_DRAW_2, "glock", /*UseDecrement() ? 1 : 0*/ 0 );
	}

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.5;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.5;
}
/*
void CGlock18::PrimaryAttack( void )
{
#ifdef CLIENT_DLL
    if ( m_fDefaultAnim == 0 )
#else
    if ( m_fDefaultAnim == NULL )
#endif
    {
        m_fDefaultAnim = 0;//No Silencer
 //       if (!(m_pPlayer->m_afButtonPressed & IN_ATTACK)) //only one shoot by mousclick
  //      return;
        FireSemi();
        return;
    }

    if ( m_fDefaultAnim != NULL )//Silencer added
    {
        m_fDefaultAnim = 1;
		FireBurst();
		return;
    }
}
*/
void CGlock18::PrimaryAttack( void )
{
	FireSemi();
}

void CGlock18::SecondaryAttack()
{
	if ( m_pPlayer->m_bIron )
	m_pPlayer->m_bIron = FALSE;
	else
	m_pPlayer->m_bIron = TRUE;
	
	m_flNextSecondaryAttack = m_flNextSecondaryAttack + 0.5;

	/*
	if (!(m_pPlayer->m_afButtonPressed & IN_ATTACK2 || IN_ATTACK)) 
	return; // SP: Fix to allow multichange pressed

	if(m_fDefaultAnim == 1)
	{
    m_fDefaultAnim = 0; //Detach silencer
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/glock18/fire-1.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
	ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#Semi_Mode");
//	Deploy();	
//    SendWeaponAnim(BER_SILENCER_OFF);
    }
    else
    {
    m_fDefaultAnim = 1; //Add silencer
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/glock18/fire-2.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
	ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#Burst_Mode");
//	Deploy();
//    SendWeaponAnim(BER_SILENCER_ON);
    }

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.7;    
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.7;
	return; // return y no ejecutar más codigo.*/
}
void CGlock18::FireSemi()
{
    if ( m_fDefaultAnim == FIREMODE_SEMI )
    {
		if (!(m_pPlayer->m_afButtonPressed & IN_ATTACK)) 
		return;
    }

    // don't fire if empty
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

    // Weapon sound
    m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
    m_pPlayer->m_iWeaponFlash  = NORMAL_GUN_FLASH;

    // one less round in the clip
    m_iClip--;

    // add a muzzle flash
//    m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;
    
    // player "shoot" animation
    m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
	Vector vecDir;

// ### COD RECOIL START ###
		if ( m_pPlayer->pev->flags & FL_ONGROUND ) 
		{	
			if ( m_pPlayer->pev->flags & FL_DUCKING ) 
			{	//  si esta en el suelo y agachado
				vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_2DEGREES, 8192, BULLET_PLAYER_GLOCK18, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
			}
			else // si no es porq esta parado
			{																//perfe 3d
				vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_5DEGREES, 8192, BULLET_PLAYER_GLOCK18, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
			}
		}
		else // si no esta en el suelo es porque esta en el aire
		{
			if ( m_pPlayer->pev->flags & FL_DUCKING ) 
			{// y si esta agachado en el aire gana un punto
				vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_15DEGREES, 2048, BULLET_PLAYER_GLOCK18, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
			}						// 14 puntos igual
			else
			{ // y si no esta agachado 15 puntos (still on air)
				vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_20DEGREES, 2048, BULLET_PLAYER_GLOCK18, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
			}
		}
// ### COD RECOIL END ###
	
  int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usFireGlock18, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );

    // Add a delay before the player can fire the next shot
//    m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + FAMAS_FIRE_DELAY;    
    m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.08;

	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.50; // tata
	
	m_flTimeWeaponIdle    = UTIL_WeaponTimeBase() + 
                            UTIL_SharedRandomFloat(m_pPlayer->random_seed, 
                                        FAMAS_FIRE_DELAY + 1, FAMAS_FIRE_DELAY + 2);
}
//This is the meat of the weapon.

void CGlock18::Reload( void )
{
	int iResult;

	if (RANDOM_LONG(0,1))
	{
		iResult = DefaultReload( 20, GLOCK18_RELOAD, 2.25 );
	}
	else
	{
		iResult = DefaultReload( 20, GLOCK18_RELOAD_2, 2.25 );
	}

	if (iResult)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
		m_pPlayer->SetAnimation( PLAYER_RELOAD ); 

/*
		Vector vecSrc	 = m_pPlayer->GetGunPosition( );
		CBaseEntity *pEntity = Create( "item_clip_pistol", vecSrc, pev->angles );	*/	
	}
}

void CGlock18::WeaponIdle( void )
{
	ResetEmptySound( );

	m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:	
		iAnim = GLOCK18_IDLE1;	
		break;
	
	default:
	case 1:
		iAnim = GLOCK18_IDLE2;
		break;
	}

	SendWeaponAnim( iAnim );

	m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 ); // how long till we do this again.
}

void CGlock18::FireBurst(void)
{	
    // don't fire if empty
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

    // Weapon sound
    m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
    m_pPlayer->m_iWeaponFlash  = NORMAL_GUN_FLASH;

    // one less round in the clip
	int iDisparos;

	if (m_iClip >= 3)
		iDisparos = 3;
	if (m_iClip == 2)
		iDisparos = 2;
	if (m_iClip == 1)
		iDisparos = 1;

	m_iClip -= iDisparos;
//  m_iClip--;
//	m_iClip--;
//	m_iClip--;

    // add a muzzle flash
//    m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;
    
    // player "shoot" animation
    m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
	Vector vecDir;
/*
#ifdef CLIENT_DLL
	if ( !bIsMultiplayer() )
#else
	if ( !g_pGameRules->IsMultiplayer() )
#endif
	{
//	vecDir = m_pPlayer->FireBulletsPlayer( 3, vecSrc, vecAiming, VECTOR_CONE_10DEGREES, 8192, BULLET_PLAYER_9MM, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
	vecDir = m_pPlayer->FireBulletsPlayer( iDisparos, vecSrc, vecAiming, VECTOR_CONE_10DEGREES, 8192, BULLET_PLAYER_9MM, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );	
	}
	else
	{	// single player spread					//3
	vecDir = m_pPlayer->FireBulletsPlayer( iDisparos, vecSrc, vecAiming, VECTOR_CONE_10DEGREES, 8192, BULLET_PLAYER_9MM, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );	
	}
*/
	// ### COD RECOIL START ###
		if ( m_pPlayer->pev->flags & FL_ONGROUND ) 
		{	
			if ( m_pPlayer->pev->flags & FL_DUCKING ) 
				vecDir = m_pPlayer->FireBulletsPlayer( iDisparos, vecSrc, vecAiming, VECTOR_CONE_8DEGREES, 8192, BULLET_PLAYER_GLOCK18, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
			else // si no es porq esta parado
				vecDir = m_pPlayer->FireBulletsPlayer( iDisparos, vecSrc, vecAiming, VECTOR_CONE_10DEGREES, 8192, BULLET_PLAYER_GLOCK18, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
		}
		else // si no esta en el suelo es porque esta en el aire
		{
			if ( m_pPlayer->pev->flags & FL_DUCKING ) 
				vecDir = m_pPlayer->FireBulletsPlayer( iDisparos, vecSrc, vecAiming, VECTOR_CONE_15DEGREES, 2048, BULLET_PLAYER_GLOCK18, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
			else
				vecDir = m_pPlayer->FireBulletsPlayer( iDisparos, vecSrc, vecAiming, VECTOR_CONE_20DEGREES, 2048, BULLET_PLAYER_GLOCK18, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
		}
// ### COD RECOIL END ###

  int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif
	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usFireGlock18_B, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );

    // Add a delay before the player can fire the next shot
    m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.6;    
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.6;	
	
	m_flTimeWeaponIdle    = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, FAMAS_FIRE_DELAY + 1, FAMAS_FIRE_DELAY + 2);
}

class CGlock18Ammo : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/weapons/Glock18/w_Glock18clip.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/weapons/Glock18/w_Glock18clip.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		if (pOther->GiveAmmo( GLOCK18_DEFAULT_GIVE, "9mm", _9MM_MAX_CARRY ) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS( ammo_glock18clip, CGlock18Ammo );