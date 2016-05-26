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

// There is a few errors whit the names of m16, m4a1 and their classes. I made this a long time ago.
// I should change it to avoid gramatical errors

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"

enum M4A2_e
{
	M4A2_IDLE1 = 0,
	M4A2_IDLE2,
	M4A2_FIRE1,
	M4A2_FIRE2,
	M4A2_FIREBURST1,
	M4A2_FIREBURST2,
	M4A2_RELOAD1,
	M4A2_RELOAD2,
	M4A2_RELOAD_VACIO,
	M4A2_RELOAD_NMC1,
	M4A2_RELOAD_NMC2,
	M4A2_RELOAD_NMC_VACIO,
	M4A2_DEPLOY_FIRST,
	M4A2_DEPLOY,
	M4A2_HOLSTER,

	M4A2_GL_ON,

	M4A2_GL_FIRE1,
	M4A2_GL_RELOAD1,
	M4A2_GL_RELOAD_NMC1,
	M4A2_GL_IDLE,

	M4A2_GL_OFF,

	M4A2_GL_HOLSTER,

	SHOTGUN_NEWANIMSBYME,
	M4A2_GL_NEWFIRE
};

//LINK_ENTITY_TO_CLASS( weapon_m16, CM4A2 );

//=========================================================
//=========================================================
int CM4A2::SecondaryAmmoIndex( void )
{
	return m_iSecondaryAmmoType;
}

void CM4A2::Spawn( )
{
//	pev->classname = MAKE_STRING("weapon_m16"); // hack to allow for old names
	Precache( );

	if (!FStringNull (v_model) )
	SET_MODEL( ENT(pev), STRING(w_model) );
	else
	SET_MODEL(ENT(pev), "models/weapons/m16/w_m16.mdl");

	m_iId = WEAPON_M4A2;

	m_iDefaultAmmo = M16_DEFAULT_AMMO;
	m_fDefaultAnim = MP5_DEFAULT_ANIM; //to define on startup if the silencer added or not

//	m_bM16SilAdd = FALSE;     // Have we been initialised

	FallInit();// get ready to fall down.

	m_fattack = -1.0;
	m_fattacklast = -1.0;
	bCanShoot = false;
	iShootMode = 0;
}

void CM4A2::KeyValue( KeyValueData *pkvd )//this sets for custom fields of weapon_generic. G-Cont.
{
	if (FStrEq(pkvd->szKeyName, "m_iszModel"))
	{
		char string[64];

		sprintf(string, "models/weapons/m16/v_%s.mdl", pkvd->szValue);
		v_model = ALLOC_STRING(string);

		sprintf(string, "models/weapons/m16/p_%s.mdl", pkvd->szValue);
		p_model = ALLOC_STRING(string);

		sprintf(string, "models/weapons/m16/w_%s.mdl", pkvd->szValue);
		w_model = ALLOC_STRING(string);

		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

void CM4A2::Precache( void )
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
		PRECACHE_MODEL("models/weapons/m16/v_m16.mdl");
		PRECACHE_MODEL("models/weapons/m16/w_m16.mdl");
#ifdef CLIENT_DLL
		if ( bIsMultiplayer() )
#else
		if ( g_pGameRules->IsMultiplayer() )
#endif
		PRECACHE_MODEL("models/weapons/m16/p_m16.mdl");
	}

//	PRECACHE_MODEL("models/weapons/m16/p_m16_2.mdl");

	m_iShell = PRECACHE_MODEL ("models/weapons/shell_556.mdl");// brass shellTE_MODEL

	PRECACHE_SOUND ("weapons/m16/m16_fire-1.wav");// H to the K
	PRECACHE_SOUND ("weapons/m16/m16_fire-2.wav");// H to the K

	m_usFireM4A2 = PRECACHE_EVENT( 1, "scripts/events/m4a2.sc" );
	m_usFireM4A2Gl = PRECACHE_EVENT( 1, "scripts/events/m16gl.sc" );
}

int CM4A2::GetItemInfo(ItemInfo *p)
{
    p->pszName   = STRING(pev->classname);
    p->pszAmmo1  = "556";              // The type of ammo it uses
    p->iMaxAmmo1 = _556_MAX_AMMO;            // Max ammo the player can carry
    p->pszAmmo2  = "ARgrenades";             
    p->iMaxAmmo2 = M203_GRENADE_MAX_CARRY;;
    p->iMaxClip  = M16_DEFAULT_AMMO;        // The clip size
    p->iSlot     = 3;						 // The number in the HUD
    p->iPosition = 3;						  // despues de famas
    p->iFlags    = ITEM_FLAG_SELECTONEMPTY;
    p->iId       = m_iId = WEAPON_M4A2;      // The weapon id
    p->iWeight   = M4A2_WEIGHT; 
	p->weaponName = "M15-Gl Launcher";

	return 1;
}

int CM4A2::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
				ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#Pickup_M15"); //digamos al cliente

		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}

BOOL CM4A2::Deploy( )
{
//	CVAR_SET_FLOAT( "in_reload", 1 );

	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/weapon_deploy.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
    
/*	if(m_fDefaultAnim == 1)
	{
		if (!FStringNull (v_model) )
		{
			m_pPlayer->pev->viewmodel = v_model;
//			m_pPlayer->pev->weaponmodel = p_model;
		
			SendWeaponAnim( M4A2_DEPLOY_FIRST, 1, 0 );	
			return TRUE;
		}
		else
		return DefaultDeploy("models/weapons/m16/v_m16.mdl", "models/weapons/m16/p_m16_2.mdl", M4A2_DEPLOY_FIRST, "sniper", 2.50f );
	}
    else
	{*/
		if (!FStringNull (v_model) )
		{
			m_pPlayer->pev->viewmodel = v_model;
//			m_pPlayer->pev->weaponmodel = p_model;
		
			SendWeaponAnim( M4A2_DEPLOY, 1, 0 );	
			return TRUE;
		}
		else
		return DefaultDeploy("models/weapons/m16/v_m16.mdl", "models/weapons/m16/p_m16.mdl", M4A2_DEPLOY, "famas", 2.50f );
//	}

	 m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
//	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.3; //delay before fire

//	return DefaultDeploy("models/weapons/MP5/v_MP5.mdl", "models/weapons/MP5/p_MP5.mdl", A2_DEPLOY, "mp5a4" );
}

void CM4A2::Holster( int skiplocal /* = 0 */ )
{
    m_fInReload = FALSE;// cancel any reload in progress.
	m_pPlayer->m_bIron = FALSE;

 //  m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
    m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}

void CM4A2::SecondaryAttack( void )
{
    m_flTimeWeaponIdle = gpGlobals->time + 5.50f; //5.50f;
    
    if(m_fDefaultAnim == 1)
	{
    m_fDefaultAnim = 0; //Detach silencer
    SendWeaponAnim(M4A2_GL_OFF);
    }
    else
    {
    m_fDefaultAnim = 1; //Add silencer
    SendWeaponAnim(M4A2_GL_ON);
    }
    
    m_flNextSecondaryAttack = m_flNextSecondaryAttack + 3.10;//3.0
    if (m_flNextSecondaryAttack < gpGlobals->time)
    
    m_flNextPrimaryAttack = m_flNextPrimaryAttack + 2.20;//3.0 //8
    if (m_flNextPrimaryAttack < gpGlobals->time)
    return;
//	}//cierre del sil
}

void CM4A2::Attack( void )
{
#ifdef CLIENT_DLL
    if ( m_fDefaultAnim == MP5_DEFAULT_ANIM )
#else
    if ( m_fDefaultAnim == NULL )
#endif
    {
        m_fDefaultAnim = 0;//No Silencer 
   //     if (!(m_pPlayer->m_afButtonPressed & IN_ATTACK)) //only one shoot by mousclick
    //    return;
// ### COD RECOIL START ###
		if ( m_pPlayer->pev->flags & FL_ONGROUND ) 
		{	
			if ( m_pPlayer->pev->flags & FL_DUCKING ) 
			{	//  si esta en el suelo y agachado
				M4A2Fire( 0.03, 0.1, TRUE, FALSE );
			}
			else // si no es porq esta parado
			{																//perfe 3d
				M4A2Fire( 0.08, 0.1, TRUE, FALSE );
			}
		}
		else // si no esta en el suelo es porque esta en el aire
		{
			if ( m_pPlayer->pev->flags & FL_DUCKING ) 
			{// y si esta agachado en el aire gana un punto
				M4A2Fire( 0.4, 0.1, TRUE, FALSE );
			}						// 14 puntos igual
			else
			{ // y si no esta agachado 15 puntos (still on air)
				M4A2Fire( 0.4, 0.1, TRUE, FALSE );
			}
		}
// ### COD RECOIL END ###
        //CM4A2Fire( 0.01, 0.10, TRUE, FALSE );
       return;
    }

    if ( m_fDefaultAnim != NULL )//Silencer added
    {
        m_fDefaultAnim = 1;

			M4A2FireGl();
		//	NeedReloadGl = TRUE;
		//	ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#PressFireToReloadGL"); //digamos al cliente
		//}
    }
        return;
}

void CM4A2::PrimaryAttack( void )
{
	if (!(m_pPlayer->m_afButtonPressed & IN_ATTACK)) //only one shoot by mousclick
	return;

	if( iShootMode == 0 )
	{
		Attack();
/*
if ( m_pPlayer->m_afButtonPressed & (  IN_ATTACK  ) )
{

// clear attack/use commands from player
m_pPlayer->m_afButtonPressed = 0;
m_pPlayer->pev->button = 0;
m_pPlayer->m_afButtonReleased = IN_ATTACK;
}*/

		iShootMode = 1;

		ALERT ( at_console, "iShootMode:%i\n",iShootMode );

		m_fattack = gpGlobals->time + 0.05;		
	}

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
}

// this function only gets called if silenced
void CM4A2::M4A2FireGl( void )
{
	if (!(m_pPlayer->m_afButtonPressed & IN_ATTACK)) //only one shoot by mousclick
    return;

    m_fDefaultAnim = 1;//Nur zu sicherheit

	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	if (m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] == 0)
	{
		PlayEmptySound( );
		return;
	}

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	m_pPlayer->m_iExtraSoundTypes = bits_SOUND_DANGER;
	m_pPlayer->m_flStopExtraSoundTime = UTIL_WeaponTimeBase() + 0.2;
			
	m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]--;

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

 	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );

	// we don't add in player velocity anymore.
	CGrenade::ShootContact( m_pPlayer->pev, 
							m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 16, 
							gpGlobals->v_forward * 800 );

	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT( flags, m_pPlayer->edict(), m_usFireM4A2Gl );
	
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 5;//3.1
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 5;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 5;// idle pretty soon after shooting.

 	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
	
//	SetThink( M4A2FireGlAnim );
//	pev->nextthink = gpGlobals->time + 0.1;
}
/*
void CM4A2::M4A2FireGlAnim( void )
{
	SendWeaponAnim( M4A2_GL_RELOAD_NMC1 ); //3.1
	//test
			
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 2.5;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 2.5;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.5;

	ALERT( at_console, "SendWeaponAnim( M4A2_GL_RELOAD_NMC1 ) \n" );
}
*/
void CM4A2::M4A2Fire( float flSpread , float flCycleTime, BOOL fUseAutoAim, BOOL Silenced )
{
    
    if (m_pPlayer->pev->waterlevel == 3)
    {
        PlayEmptySound( );
        m_flNextPrimaryAttack = 0.15;
        return;
    }

    m_fDefaultAnim = Silenced;
    
    if (m_iClip <= 0)
    {
        if (m_fFireOnEmpty)
        {
            PlayEmptySound();
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
    vecDir = m_pPlayer->FireBulletsThroughWalls( 1, vecSrc, vecAiming, Vector( flSpread, flSpread, flSpread ), 5096, BULLET_PLAYER_M16, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );

    PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usFireM4A2, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, ( m_iClip == 0 ) ? 0:0, 0 );
    
//  m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;
//  m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}


void CM4A2::Reload( void )
{
    if ( m_pPlayer->ammo_556 <= 0 )
    return;

    int iResult;

//if(m_iClip<=29)
//{    
    if(m_fDefaultAnim != 0)
    {/*
		if (m_iClip == 0)
		{
		iResult = DefaultReload( 30, M4A2_GL_RELOAD1, 2.70 );
		}
		else
		{
			switch (RANDOM_LONG(0,1))
			{
			case 0: iResult = DefaultReload( 30, M4A2_GL_RELOAD1, 2.45 );
				break;
			case 1: iResult = DefaultReload( 30, M4A2_GL_RELOAD_NMC1, 2.45 );
				break;
			}
		}
*/
    }
    else
    {
	//	if (m_iClip == 0)
		iResult = DefaultReload( 30, M4A2_RELOAD_NMC_VACIO, 2.70 );
	//	else
	//	{
	//		switch (RANDOM_LONG(0,1))
	//		{
	//		case 0: iResult = DefaultReload( 30, M4A2_RELOAD_NMC1, 2.40 );
	//			break;
	//		case 1: iResult = DefaultReload( 30, M4A2_RELOAD_NMC2, 2.40 );
	//			break;
	//		}
	//	}
    }

//}

 	if (iResult)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
		m_pPlayer->SetAnimation( PLAYER_RELOAD ); 
			
		Vector vecSrc	 = m_pPlayer->GetGunPosition( );
		CBaseEntity *pEntity = Create( "item_clip_rifle", vecSrc, pev->angles );		
	}
}


void CM4A2::WeaponIdle( void )
{
	if(	iShootMode == 1 )
	{
		if ( m_fattack < gpGlobals->time )
		{
			Attack();
			m_fattack = -1;
			m_fattacklast = gpGlobals->time + 0.05;
			iShootMode = 2;
		}
	}

	if(	iShootMode == 2 )
	{
		if ( m_fattacklast < gpGlobals->time )
		{
			Attack();
			m_fattacklast = -1;
			m_fattack = -1;
					
			iShootMode = 0;
		}
	}

	ALERT ( at_console, "iShootMode:%i\n",iShootMode );

	/*
    ResetEmptySound( );
    
    m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
    
    if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
          return;
    
    if (m_iClip != 0)    
    {
    if(m_fDefaultAnim == 1)
        SendWeaponAnim( M16_SIL_IDLE1 );
    else
    SendWeaponAnim( M16_IDLE1 );

    m_flTimeWeaponIdle = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
    }
	*/
}


class CM4A2AmmoClip : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/weapons/m16/w_m16clip.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/weapons/m16/w_m16clip.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		int bResult = (pOther->GiveAmmo( M16_DEFAULT_AMMO, "556", _556_MAX_AMMO) != -1);
		if (bResult)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
		}
		return bResult;
	}
};
LINK_ENTITY_TO_CLASS( ammo_m16, CM4A2AmmoClip );
