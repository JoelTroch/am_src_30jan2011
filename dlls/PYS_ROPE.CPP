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

//****************
//RYOKEEN's CODE
//****************/

// Ryo, you know I love you, but I cannot make this work. I need to keep looking
// in the whole code, but I haven't time.

//TO DO: Need I to add the CPointEntity::Activate() and UTIL_DesiredAction(). I'm afraid of crashes. 
//Anyway I can't make it work in any SoHL version.


#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"
#include "pys_rope.h"

extern int gmsgAddRope;

class CPys_Rope : public CPointEntity 
{
public:

	void Spawn( void );
	void Precache( void );
	void EXPORT Think( void );
	float m_flNextChatTime;
};

LINK_ENTITY_TO_CLASS( pys_rope, CPys_Rope );

void CPys_Rope::Spawn( void )
{
	ALERT(at_console, "CPys_Rope Spawned!\n");

	pev->solid			= SOLID_NOT;
	pev->movetype		= MOVETYPE_NONE;
	pev->rendermode		= kRenderTransAdd;

	Precache();

	UTIL_SetOrigin(pev, pev->origin);
	SET_MODEL(edict(), "sprites/rope.spr");//null

	SetThink ( Think );
	pev->nextthink = gpGlobals->time + 1;
}

void CPys_Rope::Precache( void )
{
	PRECACHE_MODEL("sprites/null.spr");
	PRECACHE_MODEL("sprites/rope.spr");
}

void CPys_Rope::Think( void )
{
	ALERT(at_console, "CPys_Rope Think!\n");

	CBaseEntity *pTarget = NULL;
	pTarget = UTIL_FindEntityByTargetname( pTarget, STRING(pev->target) );

	if ( pTarget != NULL )
	{
	//	if ( m_flNextChatTime > gpGlobals->time )
	//	{
			MESSAGE_BEGIN( MSG_ALL, gmsgAddRope,NULL);

			WRITE_BYTE(entindex());
			//WRITE_BYTE( pTarget->entindex() );

			WRITE_STRING(STRING(pev->message) );

			MESSAGE_END();
						
	//		m_flNextChatTime = gpGlobals->time + 20;
	//	}
	}

	SetNextThink( 5 );
}
