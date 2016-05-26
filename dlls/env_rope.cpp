#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "customentity.h"
#include "effects.h"
#include "weapons.h"
#include "decals.h"
#include "func_break.h"
#include "shake.h"

#define	B1(t)		(t*t)
#define	B2(t)		(2*t*(1-t))
#define	B3(t)		((1-t)*(1-t))

struct pointer_ent
{
	CBeam *pBeam;
};

#define START 0
#define MID_POINT 1
#define END 2
#define MAX_SEGMENTS 64

#define SF_ROPE_RANDOM_SEG			1
#define SF_ROPE_RANDOM_LENGHT		2
#define SF_ROPE_RANDOM_THICK		4

#define SF_ROPE_STARTON 1
#define SF_ROPE_NO_PVS_CHECK 2

class CEnvRope : public CPointEntity
{
public:
	void Spawn( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	void EXPORT	RopeThink( void );
	void EXPORT	RopeTurnOn( void );
	void EXPORT	RopeThinkContinous( void );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	BOOL	m_fRopeActive;
	BOOL	m_fRopeNoUpdate;
	BOOL	m_fRopeDeactivated;
};

LINK_ENTITY_TO_CLASS( env_rope, CEnvRope );

TYPEDESCRIPTION	CEnvRope::m_SaveData[] = 
{
	DEFINE_FIELD( CEnvRope, m_fRopeActive, FIELD_BOOLEAN ),
	DEFINE_FIELD( CEnvRope, m_fRopeDeactivated, FIELD_BOOLEAN ),
};

IMPLEMENT_SAVERESTORE( CEnvRope, CPointEntity );


void CEnvRope::Spawn( void )
{
	//	PRECACHE_MODEL("sprites/rope.spr");//CLIENT SIDE IS USING THIS

	pev->solid = SOLID_NOT;// always solid_not 
	SET_MODEL( ENT(pev), STRING(pev->model) );
	pev->effects |= EF_NODRAW;

	if (FStringNull(pev->targetname))
		pev->spawnflags |= 1;

	pev->solid = SOLID_NOT;	// Remove model & collisions

//	if ( !g_pGameRules->IsMultiplayer() )
//	{
		pev->nextthink = gpGlobals->time + 1.0;
		SetThink( RopeThink );
//	}
}
void CEnvRope::RopeThink( void )
{
	if ( !(pev->spawnflags & SF_ROPE_STARTON) )
	{
		m_fRopeActive = FALSE; 
		m_fRopeDeactivated = TRUE;
	}
	else
	{
		m_fRopeActive = TRUE; 
		m_fRopeDeactivated = FALSE;
		SetThink( RopeTurnOn );
		pev->nextthink = gpGlobals->time + 0.001;
	}
};
void CEnvRope::RopeTurnOn( void )
{
	if ( m_fRopeActive )
	{
		CBaseEntity *pTarget = UTIL_FindEntityByTargetname( NULL, STRING( pev->target ) );

		extern int gmsgAddRope;

		if(pTarget != NULL)
		{
			MESSAGE_BEGIN( MSG_ALL, gmsgAddRope,NULL);

				WRITE_COORD( pev->origin.x );	// X
				WRITE_COORD( pev->origin.y );	// Y
				WRITE_COORD( pev->origin.z );	// Z

				WRITE_COORD( pTarget->pev->origin.x );	// X
				WRITE_COORD( pTarget->pev->origin.y );	// Y
				WRITE_COORD( pTarget->pev->origin.z );	// Z

				WRITE_STRING( STRING(pev->message) );

			MESSAGE_END();
		}
	}

	m_fRopeNoUpdate = TRUE;

	SetThink( RopeThinkContinous );
	pev->nextthink = gpGlobals->time + 0.5;
}
void CEnvRope::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( m_fRopeDeactivated )
	{
		m_fRopeActive = TRUE; 
		m_fRopeDeactivated = FALSE;
		SetThink( RopeTurnOn );
		pev->nextthink = gpGlobals->time + 0.1;
		return;
	}
	else
	{
		m_fRopeActive = FALSE; 
		m_fRopeDeactivated = TRUE;
		SetThink( RopeTurnOn );
		pev->nextthink = gpGlobals->time + 0.001;
		return;
	}
};
void CEnvRope::RopeThinkContinous( void )
{
	if ( !m_fRopeNoUpdate && m_fRopeActive && !m_fRopeDeactivated )
	{
		SetThink( RopeTurnOn );
		pev->nextthink = gpGlobals->time + 0.001;
	}
	else
	{
		SetThink( RopeThinkContinous );
		pev->nextthink = gpGlobals->time + 0.1;
	}
}

/*
class CEnvRope : public CPointEntity
{
public:
	void Spawn( void );
	void Precache( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	void KeyValue( KeyValueData *pkvd );
	void EXPORT Think( void );
	void EXPORT ResumeThink( void );
	void DoRope();

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	int m_iSegments;
	int m_iRopeLenght;
	int m_iThickness;
};

LINK_ENTITY_TO_CLASS( env_rope, CEnvRope );

TYPEDESCRIPTION CEnvRope::m_SaveData[] =
{
	DEFINE_FIELD( CEnvRope, m_iSegments, FIELD_INTEGER ),
	DEFINE_FIELD( CEnvRope, m_iRopeLenght, FIELD_INTEGER ),
	DEFINE_FIELD( CEnvRope, m_iThickness, FIELD_INTEGER ),
};

IMPLEMENT_SAVERESTORE( CEnvRope, CPointEntity );

void CEnvRope::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "segments"))
	{
		m_iSegments = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "lenght"))
	{
		m_iRopeLenght = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "thickness"))
	{
		m_iThickness = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
	{
		CEnvRope::KeyValue( pkvd );
	}
}

void CEnvRope::Spawn( void )
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;

	Precache();
	SET_MODEL(ENT(pev),"sprites/null.spr"); // should be visible to send to client
	UTIL_SetSize(pev, g_vecZero, g_vecZero);

	//why think? on what?

	//THERE MUST BE A DELAY, OTHER WISE THE ROPE WON'T FIND THE TARGET!
	SetThink( Think );
	pev->nextthink = gpGlobals->time + 0.5;
}


void CEnvRope::Think( void )
{	
	DoRope();

//	pev->nextthink = gpGlobals->time + 0.5;
//	pev->nextthink = -1;
	DontThink();
}

void CEnvRope::Precache( void )
{
	PRECACHE_MODEL("sprites/null.spr");
	PRECACHE_MODEL("sprites/rope.spr");//CLIENT SIDE IS USING THIS
	PRECACHE_MODEL( (char*)STRING(pev->model) );

	if( bCreateAfter )
	{
		SetThink( ResumeThink );
		pev->nextthink = gpGlobals->time + 2;
	}
}

void CEnvRope::DoRope()
{	
	CBaseEntity *pTarget = UTIL_FindEntityByTargetname( NULL, STRING( pev->target ) );

	extern int gmsgAddRope;

	if(pTarget != NULL)
	{
		MESSAGE_BEGIN( MSG_ALL, gmsgAddRope,NULL);

			WRITE_COORD( pev->origin.x );	// X
			WRITE_COORD( pev->origin.y );	// Y
			WRITE_COORD( pev->origin.z );	// Z

			WRITE_COORD( pTarget->pev->origin.x );	// X
			WRITE_COORD( pTarget->pev->origin.y );	// Y
			WRITE_COORD( pTarget->pev->origin.z );	// Z

		//	WRITE_STRING( (char*)STRING(pev->model) );

		//	WRITE_SHORT( m_iSegments );
		//	WRITE_SHORT( m_iRopeLenght );
		//	WRITE_SHORT( m_iThickness );

		MESSAGE_END();
	}
}

//not used
void CEnvRope::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if (useType == USE_ON)
		pev->effects &= ~EF_NODRAW;
	else if (useType == USE_OFF)
		pev->effects |= EF_NODRAW;
	else if (useType == USE_TOGGLE)
	{
		if (pev->effects & EF_NODRAW)
			pev->effects &= ~EF_NODRAW;
		else
			pev->effects |= EF_NODRAW;
	}
};*/