
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "saverestore.h"
#include "doors.h"

#include "weapons.h"//sprites
#include "player.h"//pl search radius


#include "func_os.h"


extern int gmsgOS;

TYPEDESCRIPTION CFuncOS::m_SaveData[] =
{
	DEFINE_FIELD( CFuncOS, m_bNeedsUpdate, FIELD_BOOLEAN),
	DEFINE_FIELD( CFuncOS, m_bLogged, FIELD_BOOLEAN),

	DEFINE_FIELD( CFuncOS, m_s_pc_name, FIELD_STRING),

	DEFINE_FIELD( CFuncOS, m_szUsername, FIELD_STRING),
	DEFINE_FIELD( CFuncOS, m_szPassword, FIELD_STRING),
};

IMPLEMENT_SAVERESTORE( CFuncOS, CBaseEntity );

void CFuncOS::Spawn( void )
{	
	pev->movetype	= MOVETYPE_PUSH;
	pev->solid		= SOLID_BSP;

	SET_MODEL(ENT(pev), STRING(pev->model));

	m_bNeedsUpdate		= FALSE;
	m_bLogged			= FALSE;
}

LINK_ENTITY_TO_CLASS( func_os, CFuncOS );

void CFuncOS::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "pc"))
	{
		m_s_pc_name = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "user"))
	{
		m_szUsername = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "pass"))
	{
		m_szPassword = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseToggle::KeyValue( pkvd );
}

void CFuncOS::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !pActivator->IsPlayer() )
		return;

	if ( !pActivator->IsAlive() )
		return;

	if ( FStringNull(m_szUsername) )//user didn't type a password, machine can be used
	m_bLogged = TRUE;

	MESSAGE_BEGIN( MSG_ONE, gmsgOS, NULL, pActivator->edict() );
		WRITE_STRING(STRING(m_s_pc_name));
		WRITE_BYTE( m_bLogged );
		WRITE_STRING(STRING(m_szUsername));
		WRITE_STRING(STRING(m_szPassword));
		WRITE_LONG( DBGetFinalCode() );//client side uses this on OS thing.
	MESSAGE_END();

	m_bNeedsUpdate = TRUE;
}


void CFuncOS::FireTarget( void )
{
//	if ( !FStringNull( s_my_target ) )
//		FireTargets( STRING( s_my_target ), CBaseEntity::Instance( pev->owner ), this, USE_TOGGLE, 0 );

	m_bNeedsUpdate = FALSE;
}


void CFuncOS::Exit()
{
	m_bNeedsUpdate = FALSE;
}

void CFuncOS::Logged()
{
	m_bLogged = TRUE;
	m_bNeedsUpdate = FALSE;
}

void CFuncOS::BThink( void )
{
	Nextthink( 0 );
}
