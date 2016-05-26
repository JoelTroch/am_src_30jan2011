//=========================================================
//=========================================================
//
// CBaseHumanRepel class functions
//
//=========================================================
//=========================================================

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"schedule.h"
#include	"squadmonster.h"
#include	"cbasehuman.h"
#include	"defaultai.h"
#include	"scripted.h"
#include	"soundent.h"
#include	"animation.h"
#include	"weapons.h"
#include	"effects.h"
#include	"customentity.h"

#define SF_MONSTERMAKER_MONSTERCLIP	8

//=========================================================
// Spawn
//=========================================================

void CBaseHumanRepel::Spawn( void )
{
	Precache( );
	pev->solid = SOLID_NOT;

	SetUse( RepelUse );
}

void CBaseHumanRepel::Precache( void )
{
	UTIL_PrecacheOther( EntityName() );
	m_iSpriteTexture = PRECACHE_MODEL( "sprites/rope.spr" );
}

void CBaseHumanRepel::RepelUse ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	TraceResult tr;
	UTIL_TraceLine( pev->origin, pev->origin + Vector( 0, 0, -4096.0), dont_ignore_monsters, ENT(pev), &tr);
	/*
	if ( tr.pHit && Instance( tr.pHit )->pev->solid != SOLID_BSP) 
		return NULL;
	*/

	edict_t	*pent;
	CBaseEntity *pEntity;

	pent = CREATE_NAMED_ENTITY( MAKE_STRING( EntityName() ));
	if ( FNullEnt( pent ) )
	{
		ALERT ( at_console, "NULL Ent in HumanRepel Create!\n" );
		return;
	}




	
	pEntity = Instance( pent );
	pEntity->pev->owner = NULL;

	//
	pEntity->pev->origin = pev->origin;
	pEntity->pev->angles = pev->angles;
	pEntity->pev->spawnflags = pev->spawnflags;
	pEntity->pev->weapons = pev->weapons;
	pEntity->pev->scale = pev->scale;
	pEntity->pev->health = pev->health;
	pEntity->pev->skin = pev->skin;
	pEntity->pev->frags = pev->frags;//HL2 model compatibility (not used anymore)
	pEntity->pev->dmg = pev->dmg;//for monster breakpoint
	pEntity->pev->message = pev->message;//when a surrender terrorist fires a trigger
	pEntity->pev->target = pev->noise;

	//LRC - custom monster behaviour
//	CBaseEntity *pEntity = CBaseEntity::Instance( pEntity );
	CBaseMonster *pMonst = NULL;
	if (pEntity && (pMonst = pEntity->MyMonsterPointer()) != NULL)
	{
		pMonst->m_iClass = this->m_iClass;
		pMonst->m_iPlayerReact = this->m_iPlayerReact;
		pMonst->m_iTriggerCondition = this->m_iTriggerCondition;	//AJH
		pMonst->m_iszTriggerTarget = this->m_iszTriggerTarget;		//AJH	
	}
	//

	/*
	pEntity->pev->origin = pev->origin;
	pEntity->pev->angles = pev->angles;
	pEntity->pev->spawnflags = pev->spawnflags;
	pEntity->pev->body = pev->body;
	pEntity->pev->weapons = pev->weapons;
	pEntity->pev->netname = pev->netname;
	*/




	DispatchSpawn( pEntity->edict() );

	pEntity->pev->movetype = MOVETYPE_FLY;
	pEntity->pev->velocity = Vector( 0, 0, RANDOM_FLOAT( -196, -128 ) );

	CBaseMonster *pHuman = pEntity->MyMonsterPointer( );
	pHuman->SetActivity( ACT_GLIDE );
	pHuman->m_vecLastPosition = tr.vecEndPos;

	CBeam *pBeam = CBeam::BeamCreate( "sprites/rope.spr", 10 );
	pBeam->PointEntInit( pev->origin/* + Vector(0,0,112)*/, pEntity->entindex() );
	pBeam->SetFlags( BEAM_FSOLID );
	pBeam->SetColor( 255, 255, 255 );
	pBeam->SetThink( SUB_Remove );
	pBeam->pev->nextthink = gpGlobals->time + -4096.0 * tr.flFraction / pEntity->pev->velocity.z + 0.5;

	UTIL_Remove( this );
}
