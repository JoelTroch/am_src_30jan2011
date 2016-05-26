// NOT USED
// NOT USED
// NOT USED
// NOT USED
// NOT USED
// NOT USED
// NOT USED
// NOT USED
// NOT USED
// NOT USED
// NOT USED
// NOT USED
// NOT USED
// NOT USED
// NOT USED
// NOT USED
// NOT USED
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

#define EPIPEN_DEFAULT_GIVE		1
#define EPIPEN_MAX_CARRY			5
#define EPIPEN_WEIGHT				1 //ddd!

//DELAYS
// Fire Anim: 5.35
// Reload Anim: 3.05

enum EPIPEN_e
{
	EPIPEN_IDLE1 = 0,
	EPIPEN_DEPLOY,
	EPIPEN_DEPLOY2,

	EPIPEN_FIRE1
};

//LINK_ENTITY_TO_CLASS( weapon_epipen, CEpipen );

//=========================================================
//=========================================================
void CEpipen::Spawn( )
{
//	pev->classname = MAKE_STRING("weapon_epipen"); // hack to allow for old names
	Precache( );
	SET_MODEL(ENT(pev), "models/weapons/epipen/w_epipen.mdl");
	m_iId = WEAPON_EPIPEN;

	m_iDefaultAmmo = EPIPEN_DEFAULT_GIVE; // 

	FallInit();// get ready to fall down.
}


void CEpipen::Precache( void )
{
	PRECACHE_MODEL("models/weapons/epipen/v_epipen.mdl");
	PRECACHE_MODEL("models/weapons/epipen/w_epipen.mdl");

	PRECACHE_SOUND ("weapons/epipen/epipen_fire-1.wav");// H to the K
}

int CEpipen::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "adrenaline";
	p->iMaxAmmo1 = EPIPEN_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = 1;
	p->iSlot = 0;
	p->iPosition = 4;
//	p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;
    p->iFlags    = ITEM_FLAG_SELECTONEMPTY;
	p->iId = m_iId = WEAPON_EPIPEN;
	p->iWeight = EPIPEN_WEIGHT;
	p->weaponName = "Adrenaline Injector";


	return 1;
}

int CEpipen::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#Pickup_Epipen"); //digamos al cliente

		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();

		return TRUE;
	}
	return FALSE;
}

BOOL CEpipen::Deploy( )
{
	m_pPlayer->m_fCrosshairOff = TRUE;
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/weapon_deploy.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));

//	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
	return DefaultDeploy("models/weapons/epipen/v_epipen.mdl", "models/null.mdl", EPIPEN_DEPLOY, "mp5a4" );
}

void CEpipen::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->m_fCrosshairOff = FALSE;
 

	//usando batteria o curandose, si hace holster, detener la curacion
	pev->nextthink = 0.0;

	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", RANDOM_FLOAT(0.8, 0.9), ATTN_NORM);

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.5; //0.5
}

void CEpipen::PrimaryAttack()
{
 
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );


		SendWeaponAnim( EPIPEN_FIRE1 );	
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#EPIPEN_Fire"); //digamos al cliente
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/epipen/epipen_fire-1.wav", 1, ATTN_NORM);

		SetThink( ChargeOk );
		pev->nextthink = gpGlobals->time + 2.59;
			
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 4.66; // delay 2.30

		if ( m_flNextPrimaryAttack < UTIL_WeaponTimeBase() )
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 4.66; // delay
 
}

void CEpipen::SecondaryAttack()
{
	//UNDONE: Adrenaline someone?

	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5; 
}

void CEpipen::ChargeOk()
{					
#ifndef CLIENT_DLL

	m_pPlayer->SlowMotion();

#endif

	m_iClip--;// charge succesfull!

	if( m_iClip < 0 )
		SERVER_COMMAND( "lastinv\n" );

	return;
} 

void CEpipen::Reload( void )
{
	int iResult;

	if (m_iClip == 0)
		iResult = DefaultReload( 1, EPIPEN_DEPLOY, 1.05 );
	else
		iResult = DefaultReload( 1, EPIPEN_DEPLOY, 1.05 );

	if (iResult)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
	}
}

void CEpipen::WeaponIdle( void )
{
//	if (m_iClip == 0)
//	SERVER_COMMAND( "lastinv\n" );

/*
	if ( m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] )
	{
		SendWeaponAnim( EPIPEN_DEPLOY );
	}
	else
	{
		RetireWeapon();
		return;
	}*/
}

class CEpipenAmmoClip : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/weapons/epipen/w_epipen.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/weapons/epipen/w_epipen.mdl");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		int bResult = (pOther->GiveAmmo( 1, "adrenaline", EPIPEN_MAX_CARRY) != -1);
		if (bResult)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
		}
		return bResult;
	}
};
LINK_ENTITY_TO_CLASS( ammo_epipenclip, CEpipenAmmoClip );