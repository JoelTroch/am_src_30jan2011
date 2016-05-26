//
// Interface for the humanfollower class
//
////////////////////////////////////////////////////////////

#ifndef HUMANFOLLOWER_H
#define HUMANFOLLOWER_H

#include "CBasehuman.h"


//=========================================================
// monster-specific schedule types
// These are schedules that only this monster does - NOT
// schedules that this monster does slightly differently
// from other monsters - those can be merely over-ridden
// in GetScheduleofType
//=========================================================

enum
{
	SCHED_HUMAN_MOVE_AWAY = LAST_HUMAN_SCHEDULE + 1,// Try to get out of the player's way
	SCHED_HUMAN_MOVE_AWAY_FOLLOW,	// same, but follow afterward
	SCHED_HUMAN_MOVE_AWAY_FAIL,		// Turn back toward player
	SCHED_HUMAN_STOP_SHOOTING,		// Tell player to stop shooting my buddies

	LAST_HUMAN_FOLLOWER_SCHEDULE,
};


//=========================================================
// monster-specific tasks
//=========================================================

enum 
{
	TASK_HUMAN_SOUND_STOP_SHOOTING = LAST_HUMAN_TASK + 1,// Tell player to stop shooting my buddies
	TASK_HUMAN_WALK_PATH_FOR_UNITS, // Walk along a path for a certain number of units
	TASK_HUMAN_MOVE_AWAY_PATH,		// Get a path to somewhere out of the way
	TASK_HUMAN_FACE_PLAYER,			// Look at the player
	TASK_HUMAN_SOUND_OUTWAY,		// For when you are getting out of the players way

	LAST_HUMAN_FOLLOWER_TASK,
};


class CBaseHumanFollower : public CBaseHuman
{
public:
	virtual void Spawn( void );
	virtual void Precache( void );
	virtual CBaseHumanFollower *MyHumanFollowerPointer( void ) { return this; };
/*
	virtual int	ObjectCaps( void )
	{ 
		return CBaseHuman :: ObjectCaps() | FCAP_IMPULSE_USE; 
	}
*/
	// Wargon: Юзать монстра можно только если он жив. Это нужно чтобы иконка юза не показывалась на мертвых монстрах.
	virtual int	ObjectCaps( void )
	{ 
	//	if (pev->deadflag == DEAD_NO) 
	//		return CBaseHuman :: ObjectCaps() | FCAP_IMPULSE_USE | FCAP_DISTANCE_USE; 
	//	else 
			return CBaseHuman::ObjectCaps(); 
	}

	virtual void RunTask( Task_t *pTask );
	virtual void StartTask( Task_t *pTask );
	virtual Schedule_t *GetScheduleOfType ( int Type );
	virtual Schedule_t *GetSchedule ( void );
	virtual Schedule_t *GetScheduleFromSquadCommand ( void );

	virtual int TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	virtual void Killed( entvars_t *pevAttacker, int iGib );
	virtual BOOL IsFacing( entvars_t *pevTest, const Vector &reference );

	virtual void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void EXPORT FollowerTouch(	CBaseEntity *pOther );
	
	static void PlayerSquadCommand( SquadCommand Cmd );
	virtual void DeclineFollowing( void );
	virtual void SquadReceiveCommand( SquadCommand Cmd );
    virtual void PlayScriptedSentence( const char *pszSentence, float duration, float volume, float attenuation, BOOL bConcurrent, CBaseEntity *pListener );
	virtual void IdleSound();
	float m_useTime;						// Don't allow +USE until this time

	// Save functions
	virtual int	Save( CSave &save );
	virtual int	Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	CUSTOM_SCHEDULES;
};



#endif