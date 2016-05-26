/***
*
*		   ��
*			 �    �������
*			� �   �  �  � 
*		   �   �  �  �  �
*		  �     � �  �  �
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


/**

  CHANGES ON THIS FILE:
  
NONE

***/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"

class CAirtank : public CGrenade
{
	void Spawn( void );
	void Precache( void );
	void EXPORT TankThink( void );
	void EXPORT TankTouch( CBaseEntity *pOther );
	int	 BloodColor( void ) { return DONT_BLEED; };
	void Killed( entvars_t *pevAttacker, int iGib );

	virtual int		Save( CSave &save ); 
	virtual int		Restore( CRestore &restore );
	
	static	TYPEDESCRIPTION m_SaveData[];

	int	 m_state;
};


LINK_ENTITY_TO_CLASS( item_airtank, CAirtank );
TYPEDESCRIPTION	CAirtank::m_SaveData[] = 
{
	DEFINE_FIELD( CAirtank, m_state, FIELD_INTEGER ),
};

IMPLEMENT_SAVERESTORE( CAirtank, CGrenade );


void CAirtank :: Spawn( void )
{
	Precache( );
	// motor
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/w_oxygen.mdl");
	UTIL_SetSize(pev, Vector( -16, -16, 0), Vector(16, 16, 36));
	UTIL_SetOrigin( pev, pev->origin );

	SetTouch( TankTouch );
	SetThink( TankThink );

	pev->flags |= FL_MONSTER;
	pev->takedamage		= DAMAGE_YES;
	pev->health			= 20;
	pev->dmg			= 50;
	m_state				= 1;
}

void CAirtank::Precache( void )
{
	PRECACHE_MODEL("models/w_oxygen.mdl");
	PRECACHE_SOUND("doors/aliendoor3.wav");
}


void CAirtank :: Killed( entvars_t *pevAttacker, int iGib )
{
	pev->owner = ENT( pevAttacker );

	// UNDONE: this should make a big bubble cloud, not an explosion

	Explode( pev->origin, Vector( 0, 0, -1 ) );
}


void CAirtank::TankThink( void )
{
	// Fire trigger
	m_state = 1;
	SUB_UseTargets( this, USE_TOGGLE, 0 );
}


void CAirtank::TankTouch( CBaseEntity *pOther )
{
	if ( !pOther->IsPlayer() )
		return;

	if (!m_state)
	{
		// "no oxygen" sound
		EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_swim2.wav", 1.0, ATTN_NORM );
		return;
	}
		
	// give player 12 more seconds of air
	pOther->pev->air_finished = gpGlobals->time + 12;

	// suit recharge sound
	EMIT_SOUND( ENT(pev), CHAN_VOICE, "doors/aliendoor3.wav", 1.0, ATTN_NORM );

	// recharge airtank in 30 seconds
	pev->nextthink = gpGlobals->time + 30;
	m_state = 0;
	SUB_UseTargets( this, USE_TOGGLE, 1 );
}
