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

//#define SILENCED_GUN_VOLUME		64
//#define	SMALEST_GUN_FLASH		64

#define	P90_CLIP		50
//int m_iBrassShell;

enum UZI_e
{
	UZI_SIL_IDLE1 = 0,
	UZI_SIL_IDLE2,
	UZI_SIL_FIRE1,
	UZI_SIL_FIRE2,
	UZI_SIL_DEPLOY,
	UZI_SIL_HOLSTER,
	UZI_SIL_RELOAD,
	UZI_SIL_RELOAD_VACIO,
/*
	UZI_SIL_ON,
	UZI_SIL_OFF,

	UZI_IDLE1,
	UZI_IDLE2,
	UZI_FIRE1,
	UZI_FIRE2,
	UZI_DEPLOY,
	UZI_HOLSTER,
	UZI_RELOAD,
	UZI_RELOAD_VACIO,*/
};

LINK_ENTITY_TO_CLASS( weapon_uzi, CUZI );
LINK_ENTITY_TO_CLASS( weapon_p90, CUZI );

//=========================================================
//=========================================================
void CUZI::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_p90"); // hack to allow for old names
	Precache( );

	if (!FStringNull (v_model) )
	SET_MODEL( ENT(pev), STRING(w_model) );
	else
	SET_MODEL(ENT(pev), "models/weapons/p90/w_p90.mdl");

	m_iId = WEAPON_UZI;

	if (pev->armorvalue)
	{ 
		if ((pev->armorvalue > P90_CLIP) && (pev->armorvalue < 0))
		pev->armorvalue = P90_CLIP;

		m_iDefaultAmmo = pev->armorvalue;
	}
	else
	m_iDefaultAmmo = P90_CLIP; // 250 ammo

//	m_bUZISilAdd = FALSE;     // Have we been initialised
	FallInit();// get ready to fall down.
}


void CUZI::Precache( void )
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
		PRECACHE_MODEL("models/weapons/p90/v_p90.mdl");
		PRECACHE_MODEL("models/weapons/p90/w_p90.mdl");
#ifdef CLIENT_DLL
		if ( bIsMultiplayer() )
#else
		if ( g_pGameRules->IsMultiplayer() )
#endif

		PRECACHE_MODEL("models/weapons/p90/p_p90.mdl");
	}

	m_iShell = PRECACHE_MODEL ("models/weapons/shell_9mm.mdl");// brass shellTE_MODEL

	PRECACHE_SOUND ("weapons/p90/p90_fire-1.wav");// H to the K
//	PRECACHE_SOUND ("weapons/UZI/UZI_fire-2.wav");// H to the K

//	PRECACHE_SOUND ("weapons/UZI/UZI_fireSIL-1.wav");// H to the K
//	PRECACHE_SOUND ("weapons/UZI/UZI_fireSIL-2.wav");// H to the K

	PRECACHE_SOUND ("weapons/pistol_reload.wav");
//	m_iBrassShell = PRECACHE_MODEL ("models/shell.mdl");// brass shell

	m_usFireUZI = PRECACHE_EVENT( 1, "scripts/events/UZI.sc" );
	m_usFireUZI_SIL = PRECACHE_EVENT( 1, "scripts/events/UZI_sil.sc" );
}

void CUZI::KeyValue( KeyValueData *pkvd )//this sets for custom fields of weapon_generic. G-Cont.
{
	if (FStrEq(pkvd->szKeyName, "m_iszModel"))
	{
		char string[64];

		sprintf(string, "models/weapons/p90/v_%s.mdl", pkvd->szValue);
		v_model = ALLOC_STRING(string);

		sprintf(string, "models/weapons/p90/p_%s.mdl", pkvd->szValue);
		p_model = ALLOC_STRING(string);

		sprintf(string, "models/weapons/p90/w_%s.mdl", pkvd->szValue);
		w_model = ALLOC_STRING(string);

		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

int CUZI::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
//	p->pszAmmo1 = "9mm";
	p->pszAmmo1 = "ammo_57";
	p->iMaxAmmo1 = _9MM_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1; //M203_GRENADE_MAX_CARRY;
	p->iMaxClip = P90_CLIP; //CLIP
	p->iSlot = 2;
	p->iPosition = 1;
    p->iFlags    = ITEM_FLAG_SELECTONEMPTY;
	p->iId = m_iId = WEAPON_UZI;
	p->iWeight = UZI_WEIGHT;
	p->weaponName = "IMI UZI";

	return 1;
}

int CUZI::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
				ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#Pickup_Uzi"); //digamos al cliente

		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();

		return TRUE;
	}
	return FALSE;
}

BOOL CUZI::Deploy( )
{
//	m_fDefaultAnim = 1;

	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/weapon_deploy.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
    
/*	if(m_fDefaultAnim == 1)
	{
		if (!FStringNull (v_model) )
		{
			m_pPlayer->pev->viewmodel = v_model;
//			m_pPlayer->pev->weaponmodel = p_model;
		
			SendWeaponAnim( UZI_SIL_DEPLOY, 1, 0 );	
			return TRUE;
		}
		else
		return DefaultDeploy("models/weapons/p90/v_p90.mdl", "models/weapons/p90/p_p90.mdl", UZI_SIL_DEPLOY, "p90", 3.50f );
	}
    else
	{
		if (!FStringNull (v_model) )
		{
			m_pPlayer->pev->viewmodel = v_model;
//			m_pPlayer->pev->weaponmodel = p_model;
		
			SendWeaponAnim( UZI_DEPLOY, 1, 0 );	
			return TRUE;
		}
		else
		return DefaultDeploy("models/weapons/p90/v_p90.mdl", "models/weapons/p90/p_p90.mdl", UZI_DEPLOY, "p90", 3.50f );
//	}

	 m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
*/
	if (!FStringNull (v_model) )
	{
		m_pPlayer->pev->viewmodel = v_model;
//		m_pPlayer->pev->weaponmodel = p_model;
	
		SendWeaponAnim( UZI_SIL_DEPLOY, 1, 0 );	
		return TRUE;
	}
	else
	return DefaultDeploy("models/weapons/p90/v_p90.mdl", "models/weapons/p90/p_p90.mdl", UZI_SIL_DEPLOY, "uzi" );
	
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}

void CUZI::Holster( int skiplocal /* = 0 */ )
{
	m_fInReload = FALSE;// cancel any reload in progress.
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", RANDOM_FLOAT(0.8, 0.9), ATTN_NORM);
	m_pPlayer->m_bIron = FALSE;

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.5; //0.5
	if (m_iClip)
		SendWeaponAnim( UZI_SIL_HOLSTER );
	else
		SendWeaponAnim( UZI_SIL_HOLSTER );
}
void CUZI::SecondaryAttack( void )
{
		if ( m_pPlayer->m_bIron )
	m_pPlayer->m_bIron = FALSE;
	else
	m_pPlayer->m_bIron = TRUE;
	
	m_flNextSecondaryAttack = m_flNextSecondaryAttack + 0.5;

	return; // return y no ejecutar más codigo.
/*
	// Acá comprobamos que el jugador tiene o no silenciador
	// y si no lo tiene, no lo ponemos...
	if (m_pPlayer->m_bUZISilAdd == FALSE )
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#No_Silencer"); //digamos al cliente
		
		m_flNextSecondaryAttack = m_flNextSecondaryAttack + 2.0;//delays
		return; // return y no ejecutar más codigo.
	}
	else
	{

    m_flTimeWeaponIdle = gpGlobals->time + 5.50f; //5.50f;
    
    if(m_fDefaultAnim == 1){
    m_fDefaultAnim = 0; //Detach silencer
    SendWeaponAnim(UZI_SIL_ON);
    }
    else
    {
    m_fDefaultAnim = 1; //Add silencer
    SendWeaponAnim(UZI_SIL_OFF);
    }
    
    m_flNextSecondaryAttack = m_flNextSecondaryAttack + 3.15;//3.0
    if (m_flNextSecondaryAttack < gpGlobals->time)
    
    m_flNextPrimaryAttack = m_flNextPrimaryAttack + 3.15;//3.0 //8
    if (m_flNextPrimaryAttack < gpGlobals->time)
    return;
	}//cierre del sil*/
}

void CUZI::PrimaryAttack( void )
{
	if ( m_fDefaultAnim == FIREMODE_SEMI )
    {
		if (!(m_pPlayer->m_afButtonPressed & IN_ATTACK)) 
		return;
    }

	UZIFire( 0.05, 0.06, TRUE, FALSE );
/*
#ifdef CLIENT_DLL
    if ( m_fDefaultAnim == MP5_DEFAULT_ANIM )
#else
    if ( m_fDefaultAnim == NULL )
#endif
    {
        m_fDefaultAnim = 0;//No Silencer 
		if ( m_pPlayer->pev->flags & FL_ONGROUND ) 
		{	
			if ( m_pPlayer->pev->flags & FL_DUCKING ) 
			{	//  si esta en el suelo y agachado
				UZIFire( 0.01, 0.06, TRUE, FALSE );//0.09
			}
			else // si no es porq esta parado
			{																//perfe 3d
				UZIFire( 0.05, 0.06, TRUE, FALSE );
			}
		}
		else // si no esta en el suelo es porque esta en el aire
		{
			if ( m_pPlayer->pev->flags & FL_DUCKING ) 
			{// y si esta agachado en el aire gana un punto
				UZIFire( 0.4, 0.06, TRUE, FALSE );
			}						// 14 puntos igual
			else
			{ // y si no esta agachado 15 puntos (still on air)
				UZIFire( 0.4, 0.06, TRUE, FALSE );
			}
		}
// ### COD RECOIL END ###
       return;
    }

    if ( m_fDefaultAnim != NULL )//Silencer added
    {
        m_fDefaultAnim = 1;
// ### COD RECOIL START ###
		if ( m_pPlayer->pev->flags & FL_ONGROUND ) 
		{	
			if ( m_pPlayer->pev->flags & FL_DUCKING ) 
			{	//  si esta en el suelo y agachado
				UZIFireSilenced( 0.001, 0.06, TRUE, TRUE );
			}
			else // si no es porq esta parado
			{																//perfe 3d
				UZIFireSilenced( 0.02, 0.06, TRUE, TRUE );
			}
		}
		else // si no esta en el suelo es porque esta en el aire
		{
			if ( m_pPlayer->pev->flags & FL_DUCKING ) 
			{// y si esta agachado en el aire gana un punto
				UZIFireSilenced( 0.2, 0.06, TRUE, TRUE );
			}						// 14 puntos igual
			else
			{ // y si no esta agachado 15 puntos (still on air)
				UZIFireSilenced( 0.2, 0.06, TRUE, TRUE );
			}
		}
// ### COD RECOIL END ###

    }	
//test*/
	return;
}
// this function only gets called if silenced
void CUZI::UZIFireSilenced( float flSpread , float flCycleTime, BOOL fUseAutoAim, BOOL Silenced )
{
    if (m_pPlayer->pev->waterlevel == 3)
    {
        PlayEmptySound( );
        m_flNextPrimaryAttack = 0.15;
        return;
    }

    m_fDefaultAnim = Silenced;//Nur zu sicherheit

    if (m_iClip == 1)
    {
        SendWeaponAnim( UZI_SIL_FIRE2 );
        m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2;
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

    m_iClip--;

//    m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

    int flags;

#if defined( CLIENT_WEAPONS )
    flags = FEV_NOTHOST;
#else
    flags = 0;
#endif
    m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

        m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
        m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
    
    Vector vecSrc     = m_pPlayer->GetGunPosition( );
    Vector vecAiming;
    
    if ( fUseAutoAim )
    {
        vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );
    }
    else
    {
        vecAiming = gpGlobals->v_forward;
    }

    Vector vecDir;																							//BULLET_PLAYER_45
    vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, Vector( flSpread, flSpread, flSpread ), 5096, BULLET_PLAYER_UZI, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );

    PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usFireUZI_SIL, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, ( m_iClip == 0 ) ? 0:0, 0 );
    
    m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

 	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}

void CUZI::UZIFire( float flSpread , float flCycleTime, BOOL fUseAutoAim, BOOL Silenced )
{
    
    if (m_pPlayer->pev->waterlevel == 3)
    {
        PlayEmptySound( );
        m_flNextPrimaryAttack = 0.15;
        return;
    }

  //  m_fDefaultAnim = Silenced;
    
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

    m_iClip--;

//    m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;
/*
    int flags;

#if defined( CLIENT_WEAPONS )
    flags = FEV_NOTHOST;
#else
    flags = 0;
#endif
*/
    // player "shoot" animation
    m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

        m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
        m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

    Vector vecSrc     = m_pPlayer->GetGunPosition( );
    Vector vecAiming;
    
    if ( fUseAutoAim )
    {
        vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );
    }
    else
    {
        vecAiming = gpGlobals->v_forward;
    }

    Vector vecDir, vecDir1;																				// BULLET_PLAYER_45
    vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, Vector( flSpread, flSpread, flSpread ), 5096, BULLET_PLAYER_UZI, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );

//    PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usFireUZI, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, ( m_iClip == 0 ) ? 0:0, 0 );
   		
//	Vector	vecShellVelocity = gpGlobals->v_right * RANDOM_FLOAT(40,90)/4 + gpGlobals->v_up * RANDOM_FLOAT(75,200)/4 + gpGlobals->v_forward * RANDOM_FLOAT(-40, 40)/4;
//	EjectBrass ( vecSrc, vecShellVelocity, pev->angles.y, m_iBrassShell, TE_BOUNCE_SHELL); 

	if( sModifiers.iSilencer == MOD_ON )
	{
		m_pPlayer->m_iWeaponVolume = 0;
		m_pPlayer->m_iWeaponFlash = 0;

		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/m4a1/m4a1_FireSil-1.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 90 + RANDOM_LONG(0,0xF));
	}
	else
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/p90/p90_Fire-1.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 90 + RANDOM_LONG(0,0xF));

	if (RANDOM_LONG(0,1))
	SendWeaponAnim( UZI_SIL_FIRE1 );
	else
	SendWeaponAnim( UZI_SIL_FIRE1 ); 

    m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}


void CUZI::Reload( void )
{
//    if ( m_pPlayer->ammo_57 <= 0 )
  //  return;

    int iResult;

if(m_iClip<P90_CLIP)
{    
  //  if(m_fDefaultAnim != 0)
  //  {
		if (m_iClip == 0)
			iResult = DefaultReload( P90_CLIP, UZI_SIL_RELOAD_VACIO, 3.4125 );
		else
			iResult = DefaultReload( P90_CLIP, UZI_SIL_RELOAD, 3.4125 );
 //   }
 /*   else
    {
		if (m_iClip == 0)
			iResult = DefaultReload( P90_CLIP, UZI_RELOAD_VACIO, 3.4125 );
		else
			iResult = DefaultReload( P90_CLIP, UZI_RELOAD, 3.4125 );
   }*/

}
 	if (iResult)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
		m_pPlayer->SetAnimation( PLAYER_RELOAD );
			
		Vector vecSrc	 = m_pPlayer->GetGunPosition( );
		CBaseEntity *pEntity = Create( "item_clip_pistol", vecSrc, pev->angles );
	}
}


void CUZI::WeaponIdle( void )
{
	
    ResetEmptySound( );
    
    m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
    
    if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
          return;
    
    if (m_iClip != 0)    
    {
//    if(m_fDefaultAnim == 1)
       SendWeaponAnim( UZI_SIL_IDLE1 );
 //   else
//    SendWeaponAnim( UZI_IDLE1 );

    m_flTimeWeaponIdle = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
    }
	
}


class CUZIAmmoClip : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/weapons/p90/w_p90clip.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/weapons/p90/w_p90clip.mdl");
//		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
	//	int bResult = (pOther->GiveAmmo( P90_CLIP, "9mm", _9MM_MAX_CARRY) != -1);
		int bResult = (pOther->GiveAmmo( P90_CLIP, "ammo_57", _9MM_MAX_CARRY) != -1);
		if (bResult)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
		}
		return bResult;
	}
};
LINK_ENTITY_TO_CLASS( ammo_p90clip, CUZIAmmoClip );

