//========= Copyright © 2004-2008, Raven City Team, All rights reserved. ============//
//																					 //
// Purpose:																			 //
//																					 //
// $NoKeywords: $																	 //
//===================================================================================//

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "customentity.h"
//#include "particle.h"
#include "weapons.h"
#include "decals.h"
#include "func_break.h"
#include "shake.h"
#include "entity_state.h"
#include "gamerules.h"
#include "player.h"
#include "func_grass.h"

//===============================================
// Entity light - Finally it works, we can freely use
// it under the limits, as only a certain number of
// elights are allowed to affect a model at a time.
//===============================================
extern int gmsgGrassParticles;

LINK_ENTITY_TO_CLASS( func_grass, CFuncGrass );

TYPEDESCRIPTION	CFuncGrass::m_SaveData[] = 
{
	DEFINE_FIELD( CFuncGrass, m_iIDNumber, FIELD_INTEGER ),
	DEFINE_FIELD( CFuncGrass, m_fGrassActive, FIELD_BOOLEAN ),
	DEFINE_FIELD( CFuncGrass, m_fGrassDeactivatedByPVS, FIELD_BOOLEAN ),
	DEFINE_FIELD( CFuncGrass, m_fGrassDeactivated, FIELD_BOOLEAN ),
};

IMPLEMENT_SAVERESTORE( CFuncGrass, CPointEntity );

int CFuncGrass::ms_iNextFreeKey = 1;

void CFuncGrass::Spawn( void )
{
	pev->solid = SOLID_NOT;// always solid_not 
	SET_MODEL( ENT(pev), STRING(pev->model) );
	pev->effects |= EF_NODRAW;

	m_iIDNumber = ms_iNextFreeKey;
	ms_iNextFreeKey++;

	if (FStringNull(pev->targetname))
		pev->spawnflags |= 1;

	pev->solid = SOLID_NOT;	// Remove model & collisions
	if ( !g_pGameRules->IsMultiplayer() )
	{
		pev->nextthink = gpGlobals->time + 1.0;
		SetThink( GrassThink );
	}
}
void CFuncGrass::GrassThink( void )
{
	if ( !(pev->spawnflags & SF_GRASS_STARTON) )
	{
		m_fGrassActive = FALSE; 
		m_fGrassDeactivated = TRUE;
	}
	else
	{
		m_fGrassActive = TRUE; 
		m_fGrassDeactivated = FALSE;
		SetThink( GrassTurnOn );
		pev->nextthink = gpGlobals->time + 0.001;
	}
};
void CFuncGrass::GrassTurnOn( void )
{
	if ( m_fGrassActive )
	{
		MESSAGE_BEGIN(MSG_ALL, gmsgGrassParticles);
			WRITE_SHORT(m_iIDNumber);
			WRITE_BYTE(0);
			WRITE_COORD(pev->absmax.x);
			WRITE_COORD(pev->absmax.y);
			WRITE_COORD(pev->absmax.z);
			WRITE_COORD(pev->absmin.x);
			WRITE_COORD(pev->absmin.y);
			WRITE_COORD(pev->absmin.z);
			WRITE_STRING(STRING(pev->message));
		MESSAGE_END();
	//	ALERT( at_console, "Activating Grass!!!\n" );
	}
	else
	{
		MESSAGE_BEGIN(MSG_ALL, gmsgGrassParticles);
			WRITE_SHORT(m_iIDNumber);
			WRITE_BYTE(1);
		MESSAGE_END();
	}
	m_fGrassNoUpdate = TRUE;

	SetThink( GrassThinkContinous );
	pev->nextthink = gpGlobals->time + 0.5;
}
void CFuncGrass::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( m_fGrassDeactivated )
	{
		m_fGrassActive = TRUE; 
		m_fGrassDeactivated = FALSE;
		SetThink( GrassTurnOn );
		pev->nextthink = gpGlobals->time + 0.1;
		return;
	}
	else
	{
		m_fGrassActive = FALSE; 
		m_fGrassDeactivated = TRUE;
		SetThink( GrassTurnOn );
		pev->nextthink = gpGlobals->time + 0.001;
		return;
	}
};
void CFuncGrass::GrassThinkContinous( void )
{
	if ( !m_fGrassNoUpdate && m_fGrassActive && !m_fGrassDeactivated )
	{
		SetThink( GrassTurnOn );
		pev->nextthink = gpGlobals->time + 0.001;
	//	ALERT( at_console, "Activating Grass!!!\n" );
	}
	else
	{
		SetThink( GrassThinkContinous );
		pev->nextthink = gpGlobals->time + 0.1;
	//	ALERT( at_console, "Thinking!!!\n" );
	}
}