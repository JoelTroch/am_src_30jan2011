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

#include "decals.h"

#define	CROWBAR_BODYHIT_VOLUME 64
#define	CROWBAR_WALLHIT_VOLUME 256

LINK_ENTITY_TO_CLASS( weapon_fists, CFists );



enum crowbar_e 
{
	fists_DEPLOY = 0,
	fists_SLASH1,
	fists_SLASH2,
	fists_SLASH3,
	fists_COVER_IN,
	fists_COVERING,
	fists_COVER_OUT,
	fists_IDLE,
};


void CFists::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_fists"); // hack to allow for old names
	Precache( );
	m_iId = WEAPON_FISTS;

	if (!FStringNull (v_model) )
	SET_MODEL( ENT(pev), STRING(w_model) );
	else
	SET_MODEL(ENT(pev), "models/null.mdl");
	
	m_iClip = -1;

/*
	CBaseEntity *pEntity = Create( "weapon_flash", pev->origin + Vector (0,0,10), pev->angles );		
	pEntity->pev->velocity = Vector (RANDOM_FLOAT(-100,100), RANDOM_FLOAT(-100,100), RANDOM_FLOAT(200,300));
	pEntity->pev->avelocity = Vector ( 0, RANDOM_FLOAT( 200, 400 ), 0 );
*/
	FallInit();// get ready to fall down.
}

void CFists::KeyValue( KeyValueData *pkvd )//this sets for custom fields of weapon_generic. G-Cont.
{
	if (FStrEq(pkvd->szKeyName, "m_iszModel"))
	{
		char string[64];

		sprintf(string, "models/weapons/fists/v_%s.mdl", pkvd->szValue);
		v_model = ALLOC_STRING(string);

		sprintf(string, "models/weapons/fists/p_%s.mdl", pkvd->szValue);
		p_model = ALLOC_STRING(string);

		sprintf(string, "models/weapons/fists/w_%s.mdl", pkvd->szValue);
		w_model = ALLOC_STRING(string);

		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

void CFists::Precache( void )
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
		PRECACHE_MODEL("models/weapons/fists/v_fists.mdl");
		PRECACHE_MODEL("models/null.mdl");
#ifdef CLIENT_DLL
		if ( bIsMultiplayer() )
#else
		if ( g_pGameRules->IsMultiplayer() )
#endif
		PRECACHE_MODEL("models/null.mdl");
	}

	PRECACHE_SOUND("weapons/fists/miss1.wav");
	PRECACHE_SOUND("weapons/fists/miss2.wav");
	PRECACHE_SOUND("weapons/fists/hit1.wav");
	PRECACHE_SOUND("weapons/fists/hit2.wav");
}

int CFists::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 0;
	p->iPosition = 5;
	p->iId = WEAPON_FISTS;
	p->iWeight = CROWBAR_WEIGHT;
	p->weaponName = "Fists";

	return 1;
}

int CFists::AddToPlayer( CBasePlayer *pPlayer )
{
    if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
    {
				ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "#Pickup_fists"); //digamos al cliente

          MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
          WRITE_BYTE( m_iId );
          MESSAGE_END();
          return TRUE;
    }
    return FALSE;
}


BOOL CFists::Deploy( )
{
	pev->skin = 0;
#ifndef CLIENT_DLL
	MESSAGE_BEGIN( MSG_ONE, gmsgSetSkin, NULL, m_pPlayer->pev );
		WRITE_BYTE( pev->skin ); //weaponmodel skin.
	MESSAGE_END();
#endif	
	m_pPlayer->m_fCrosshairOff = TRUE;

	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/fists_deploy.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));

	if (!FStringNull (v_model) )
	{
		m_pPlayer->pev->viewmodel = v_model;
//		m_pPlayer->pev->weaponmodel = p_model;
	
		SendWeaponAnim( fists_DEPLOY, 1, 0 );	
		return TRUE;
	}
	else
	return DefaultDeploy( "models/weapons/fists/v_fists.mdl", "models/null.mdl", fists_DEPLOY, "psfists" ); //"crowbar"
}

void CFists::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->m_fCrosshairOff = FALSE;

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.5; ///UTIL_WeaponTimeBase() + 0.5;
}


void FindHullIntersection_Fists( const Vector &vecSrc, TraceResult &tr, float *mins, float *maxs, edict_t *pEntity )
{
	int			i, j, k;
	float		distance;
	float		*minmaxs[2] = {mins, maxs};
	TraceResult tmpTrace;
	Vector		vecHullEnd = tr.vecEndPos;
	Vector		vecEnd;

	distance = 1e6f;

	vecHullEnd = vecSrc + ((vecHullEnd - vecSrc)*2);
	UTIL_TraceLine( vecSrc, vecHullEnd, dont_ignore_monsters, pEntity, &tmpTrace );
	if ( tmpTrace.flFraction < 1.0 )
	{
		tr = tmpTrace;
		return;
	}

	for ( i = 0; i < 2; i++ )
	{
		for ( j = 0; j < 2; j++ )
		{
			for ( k = 0; k < 2; k++ )
			{
				vecEnd.x = vecHullEnd.x + minmaxs[i][0];
				vecEnd.y = vecHullEnd.y + minmaxs[j][1];
				vecEnd.z = vecHullEnd.z + minmaxs[k][2];

				UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, pEntity, &tmpTrace );
				if ( tmpTrace.flFraction < 1.0 )
				{
					float thisDistance = (tmpTrace.vecEndPos - vecSrc).Length();
					if ( thisDistance < distance )
					{
						tr = tmpTrace;
						distance = thisDistance;
					}
				}
			}
		}
	}
}


void CFists::PrimaryAttack()
{
	Swing( 0 );
	
	m_flNextPrimaryAttack = 0.46;
	m_flNextSecondaryAttack = 0.46;
}

void CFists::Smack( )
{
}


void CFists::SwingAgain( void )
{
	Swing( 0 );
}

int CFists::Swing( int fFirst )
{
	int fDidHit = FALSE;

	TraceResult tr;

	UTIL_MakeVectors (m_pPlayer->pev->v_angle);
	Vector vecSrc	= m_pPlayer->GetGunPosition( );
	Vector vecEnd	= vecSrc + gpGlobals->v_forward * 32;

	UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, ENT( m_pPlayer->pev ), &tr );

#ifndef CLIENT_DLL
	if ( tr.flFraction >= 1.0 )
	{
		UTIL_TraceHull( vecSrc, vecEnd, dont_ignore_monsters, head_hull, ENT( m_pPlayer->pev ), &tr );
		if ( tr.flFraction < 1.0 )
		{
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			CBaseEntity *pHit = CBaseEntity::Instance( tr.pHit );
			if ( !pHit || pHit->IsBSPModel() )
				FindHullIntersection_Fists( vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, m_pPlayer->edict() );
			vecEnd = tr.vecEndPos;	// This is the point on the actual surface (the hull could have hit space)
		}
	}
#endif

		int iAnim;
		switch ( RANDOM_LONG( 0, 2 ) )
		{
		case 0:	
			iAnim = fists_SLASH2;
			break;
		
		case 1:
			iAnim = fists_SLASH1;
			break;
				
		case 2:
			iAnim = fists_SLASH3;
			break;
		}

		SendWeaponAnim( iAnim );
			
		m_pPlayer->ViewPunch(0, RANDOM_LONG(-10,10), RANDOM_LONG(-2,2));//abajo-arriba -derecha-izquierda? -circular izquierda-derecha


	if ( tr.flFraction >= 1.0 )
	{
		// play miss sound
		switch( RANDOM_LONG(0,1) )
		{
		case 0:
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/fists/miss1.wav", 0.3, ATTN_NORM); break;
		case 1:
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/fists/miss2.wav", 0.3, ATTN_NORM); break;
		}
	}
	else
	{
		// player "shoot" animation
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		
		m_pPlayer->ViewPunch(RANDOM_LONG(-5,10), RANDOM_LONG(-3,3), RANDOM_LONG(-3,3));//abajo-arriba -derecha-izquierda? -circular izquierda-derecha

#ifndef CLIENT_DLL

		// hit
		fDidHit = TRUE;
		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

		ClearMultiDamage( );

		// subsequent swings do half
		pEntity->TraceAttack(m_pPlayer->pev, 20 /*/ 2*/, gpGlobals->v_forward, &tr, DMG_CLUB | DMG_NEVERGIB ); //DMG_CLUB );
			
		ApplyMultiDamage( m_pPlayer->pev, m_pPlayer->pev );

		// play thwack, smack, or dong sound
		float flVol = 1.0;
		int fHitWorld = TRUE;

		// play thwack or smack sound
		switch( RANDOM_LONG(0,1) )
		{
		case 0:
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/fists/hit1.wav", 0.3, ATTN_NORM); break;
		case 1:
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/fists/hit2.wav", 0.3, ATTN_NORM); break;
		}

		if (pEntity)
		{
			if ( pEntity->Classify() != CLASS_NONE && pEntity->Classify() != CLASS_MACHINE )
			{
				if ( !pEntity->IsAlive() )
				{
					m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.4; //LRC: corrected half-life bug
					m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.4;
					return TRUE;
				}
				else
					  flVol = 0.1;

				//
				if ( pEntity->BloodColor() == BLOOD_COLOR_RED )
				{
				pev->skin = 1;
				CVAR_SET_FLOAT( "hurt", 1 );
				}
				else
				pev->skin = 0;//2
#ifndef CLIENT_DLL
				MESSAGE_BEGIN( MSG_ONE, gmsgSetSkin, NULL, m_pPlayer->pev );
					WRITE_BYTE( pev->skin ); //weaponmodel skin.
				MESSAGE_END();
#endif	
				fHitWorld = FALSE;
			}
		}

		// play texture hit sound
		// UNDONE: Calculate the correct point of intersection when we hit with the hull instead of the line



		m_pPlayer->m_iWeaponVolume = flVol * CROWBAR_WALLHIT_VOLUME;
#endif
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.4; //UTIL_WeaponTimeBase() + 0.25;
		
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
	//	SetThink( Smack );
	//	pev->nextthink = UTIL_WeaponTimeBase() + 1.2; //0.2;

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2;//??
	}
	return fDidHit;
}



void CFists::SecondaryAttack()
{
	if (! Swing2( 1 ))
	{
		SetThink( SwingAgain2 );
		pev->nextthink = gpGlobals->time + 0.1; //0.1
	}
	 m_flNextPrimaryAttack = 1;
	 m_flNextSecondaryAttack = 1;
}

void CFists::Smack2( )
{
//	DecalGunshot( &m_trHit, BULLET_PLAYER_CROWBAR );
}


void CFists::SwingAgain2( void )
{
	Swing2( 0 );
}


int CFists::Swing2( int fFirst )
{
	

	return 0;
}



void CFists::WeaponIdle( void )
{
	ResetEmptySound( );

	m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:	
		iAnim = fists_IDLE;
		break;
	
	default:
	case 1:
		iAnim = fists_IDLE;
		break;
	}

	SendWeaponAnim( iAnim );

	m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 20, 25 ); // how long till we do this again. 10 15
}
