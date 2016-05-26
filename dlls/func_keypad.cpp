#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "saverestore.h"
#include "doors.h"

#include "func_keypad.h"

#define SF_USE_SECRETCODE		1

extern int gmsgKeyPad;

TYPEDESCRIPTION CFuncKeypad::m_SaveData[] =
{
	DEFINE_FIELD( CFuncKeypad, m_bNeedsUpdate, FIELD_BOOLEAN),
	DEFINE_FIELD( CFuncKeypad, s_code, FIELD_STRING),
	DEFINE_FIELD( CFuncKeypad, s_frame_label, FIELD_STRING),
	DEFINE_FIELD( CFuncKeypad, s_my_target, FIELD_STRING),
	DEFINE_FIELD( CFuncKeypad, s_my_failed_target, FIELD_STRING),
	DEFINE_FIELD( CFuncKeypad, b_hide_pass, FIELD_BOOLEAN)
};

IMPLEMENT_SAVERESTORE( CFuncKeypad, CBaseEntity );

void CFuncKeypad::Spawn( void )
{
	pev->movetype	= MOVETYPE_PUSHSTEP;
	pev->solid		= SOLID_BBOX;

	UTIL_SetOrigin(pev, pev->origin);
	SET_MODEL(ENT(pev), STRING(pev->model));

	m_bNeedsUpdate		= FALSE;

	pev->classname = MAKE_STRING("func_keypad"); // hack to allow for old names
}

LINK_ENTITY_TO_CLASS( func_keypad, CFuncKeypad );
LINK_ENTITY_TO_CLASS( func_defuse_bomb, CFuncKeypad );

void CFuncKeypad::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "code"))
	{
		s_code = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "label"))
	{
		s_frame_label = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "my_target"))
	{
		s_my_target = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "my_target_f"))
	{
		s_my_failed_target = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "hide_pass"))
	{
		b_hide_pass = BOOL(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseToggle::KeyValue( pkvd );
}

void CFuncKeypad::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !pActivator->IsPlayer() )
		return;

	if ( !pActivator->IsAlive() )
		return;

	if ( FClassnameIs( pev, "func_defuse_bomb" ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgKeyPad, NULL, pActivator->edict() );
			WRITE_STRING(STRING(s_code));//password
			WRITE_STRING(STRING(s_frame_label));//what's displayed on LCD screen
			WRITE_LONG( 0 );//hide password option
			WRITE_LONG( 1 );//use cables
		MESSAGE_END();
	}
	else
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgKeyPad, NULL, pActivator->edict() );
			if ( pev->spawnflags & SF_USE_SECRETCODE )
			WRITE_STRING( STRING(DBGetFinalCode()) );//use final secretpassword
			else
			WRITE_STRING(STRING(s_code));//password

			WRITE_STRING(STRING(s_frame_label));//what's displayed on LCD screen
			WRITE_LONG( b_hide_pass ? 1 : 0 );//hide password option
			WRITE_LONG( -1 );//dont use cables
		MESSAGE_END();
	}

	m_bNeedsUpdate = TRUE;
}

/*
this function is callled when:
1-the user introduced the correct code and vgui sent a command (code_match)
2-the command (code_match) checks if there is a keypad that needs that code (see client.cpp)
*/
void CFuncKeypad::FireTarget( void )
{
	if ( !FStringNull( s_my_target ) )
		FireTargets( STRING( s_my_target ), CBaseEntity::Instance( pev->owner ), this, USE_TOGGLE, 0 );

	//pev->frame stores button current texture (on/off)
	if( pev->frame == 0 )
	pev->frame = 1;//on
	else
	pev->frame = 0;//off

	m_bNeedsUpdate = FALSE;
}


/*
this function is callled when:
1-the user introduced the wrong code and vgui sent a command (code_dismatch)
2-the command (code_dismatch) checks if there is a keypad that needs that code (see client.cpp)

you can also here do another stuff, for IE, triggering another entity
*/
void CFuncKeypad::WrongCode( void )
{
	if ( !FStringNull( s_my_failed_target ) )
	FireTargets( STRING( s_my_failed_target ), CBaseEntity::Instance( pev->owner ), this, USE_TOGGLE, 0 );

	m_bNeedsUpdate = FALSE;
}

void CFuncKeypad::BThink( void )
{
	Nextthink( 0 );
}
