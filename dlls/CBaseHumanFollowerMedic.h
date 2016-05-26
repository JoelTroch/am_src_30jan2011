//
// Interface for the human medic class
//
////////////////////////////////////////////////////////////

#ifndef HUMANMEDIC_H
#define HUMANMEDIC_H

#include "CBaseHuman.h"

//=========================================================
// monster-specific schedule types
// These are schedules that only this monster does - NOT
// schedules that this monster does slightly differently
// from other monsters - those can be merely over-ridden
// in GetScheduleofType
//=========================================================

enum
{
	SCHED_HUMAN_FOLLOWER_MEDIC_HEAL = LAST_HUMAN_FOLLOWER_SCHEDULE + 1,
	SCHED_HUMAN_FOLLOWER_MEDIC_CHASE,

	LAST_HUMAN_FOLLOWER_MEDIC_SCHEDULE,
};


//=========================================================
// monster-specific tasks
//=========================================================

enum 
{
	TASK_HUMAN_FOLLOWER_MEDIC_SOUND_HEAL = LAST_HUMAN_FOLLOWER_TASK + 1,
	TASK_HUMAN_FOLLOWER_MEDIC_CHECK_TARGET,

	LAST_HUMAN_FOLLOWER_MEDIC_TASK,
};


//=====================
// Animation Events
//=====================

enum
{
	HUMAN_FOLLOWER_MEDIC_AE_SYRINGE_HAND = LAST_HUMAN_ANIM_EVENT + 1, // 11
	HUMAN_FOLLOWER_MEDIC_AE_SYRINGE_HELMET, // 12
	HUMAN_FOLLOWER_MEDIC_AE_HEAL, // 13
};


//=====================
// BodyGroups
//=====================

enum 
{
	HUMAN_FOLLOWER_MEDIC_SYRINGE_HELMET = 0,
	HUMAN_FOLLOWER_MEDIC_SYRINGE_HAND,
	HUMAN_FOLLOWER_MEDIC_SYRINGE_NONE,
};


class CBaseHumanFollowerMedic : public CBaseHumanFollower
{
public:
	virtual void Spawn( );
	virtual void Precache();

	Schedule_t *GetScheduleOfType ( int Type );
	Schedule_t *GetSchedule ( void );
	void StartTask( Task_t *pTask );
	void SetActivity ( Activity NewActivity );
	void HandleAnimEvent( MonsterEvent_t *pEvent );
	void Killed( entvars_t *pevAttacker, int iGib );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	BOOL SafeToChangeSchedule() { if ( m_Activity == ACT_SPECIAL_ATTACK1 ) return FALSE; return CBaseHumanFollower::SafeToChangeSchedule(); };
	
	virtual int GetSyringeGroupNum( ) { return 0; };
	virtual BOOL IsMedic() { return TRUE; };

	void Heal( void );

	CUSTOM_SCHEDULES;
};


#endif // HUMANMEDIC_H