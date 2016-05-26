//
// Implementation of the CBaseHumanFollowerMedic class
//
//=========================================================

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"schedule.h"
#include	"squadmonster.h"

#include	"CBasehuman.h"
#include	"CBasehumanfollower.h"
#include	"CBasehumanfollowermedic.h"

#include	"defaultai.h"
#include	"scripted.h"
#include	"soundent.h"
#include	"animation.h"
#include	"weapons.h"


void CBaseHumanFollowerMedic :: Spawn( void )
{
	CBaseHumanFollowerMedic::Spawn();
}


void CBaseHumanFollowerMedic :: Precache( void )
{
	CBaseHumanFollowerMedic::Precache();
}


//=========================================================
// FollowerUse - heal player if he is injured
//=========================================================

void CBaseHumanFollowerMedic :: Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	CBaseHumanFollowerMedic::Use( pActivator, pCaller, useType, value );
}


//=========================================================
// SetActivity - totally overrides basemonster SetActivities
// because of the necessity to pick a crouch animation etc
//=========================================================

void CBaseHumanFollowerMedic :: SetActivity ( Activity NewActivity )
{
	int	iSequence = ACTIVITY_NOT_AVAILABLE;
	void *pmodel = GET_MODEL_PTR( ENT(pev) );
	char seq[40];

	switch ( NewActivity)
	{
		case ACT_SPECIAL_ATTACK2:
			strcpy( seq, "heal" );
			break;

		default:
			CBaseHumanFollowerMedic::SetActivity( NewActivity );
			return;
			break;
	}

	if (iSequence == ACTIVITY_NOT_AVAILABLE)
	{
		char seq2[40];

		if ( m_fCrouching )
		{
			strcpy( seq2, "crouch_" );
			strcat( seq2, seq );
			strcpy( seq, seq2 );
		}

		iSequence = LookupSequence( seq );
	}

	m_Activity = NewActivity; // Go ahead and set this so it doesn't keep trying when the anim is not present

	// Set to the desired anim, or default anim if the desired is not present
	if ( iSequence > ACTIVITY_NOT_AVAILABLE )
	{
		if ( pev->sequence != iSequence || !m_fSequenceLoops )
		{
			pev->frame = 0;
		}

		pev->sequence		= iSequence;	// Set to the reset anim (if it's there)
		ResetSequenceInfo( );
		SetYawSpeed();
	}
	else
	{
		// Not available try to get default anim
		ALERT ( at_console, "%s has no sequence for act:%d\n", STRING(pev->classname), NewActivity );
		pev->sequence		= 0;	// Set to the reset anim (if it's there)
	}
}


//=========================================================
// HandleAnimEvent - catches the monster-specific messages
// that occur when tagged animation frames are played.
//
// Returns number of events handled, 0 if none.
//=========================================================

void CBaseHumanFollowerMedic :: HandleAnimEvent( MonsterEvent_t *pEvent )
{
	switch( pEvent->event )
	{
	case HUMAN_FOLLOWER_MEDIC_AE_SYRINGE_HELMET:
		SetBodygroup( GetSyringeGroupNum(), HUMAN_FOLLOWER_MEDIC_SYRINGE_HELMET );
		break;

	case HUMAN_FOLLOWER_MEDIC_AE_SYRINGE_HAND:
		SetBodygroup( GetSyringeGroupNum(), HUMAN_FOLLOWER_MEDIC_SYRINGE_HAND );
		break;

	case HUMAN_FOLLOWER_MEDIC_AE_HEAL:
		Heal();
		break;

	default:
		CBaseHumanFollowerMedic::HandleAnimEvent( pEvent );
		break;
	}
}


//=========================================================
// Heal target, provided he is close enough
//=========================================================

void CBaseHumanFollowerMedic::Heal( void )
{
	if ( m_hTargetEnt == NULL ) return;

	Vector target = m_hTargetEnt->pev->origin - pev->origin;
	if ( target.Length() > 100 )
		return;

	m_hTargetEnt->TakeHealth( gSkillData.hgruntHealth/2, DMG_GENERIC );///veeeeeery hardcoded metod
}


//=========================================================
// StartTask
//=========================================================

void CBaseHumanFollowerMedic :: StartTask( Task_t *pTask )
{

	switch ( pTask->iTask )
	{
	case TASK_HUMAN_FOLLOWER_MEDIC_SOUND_HEAL:
		{
			if ( (pTask->flData == 0 || RANDOM_FLOAT(0,1) <= pTask->flData) &&
				 FOkToShout() )
			{
				PlayLabelledSentence( "HEAL" );
			}
			if ( m_hTargetEnt != NULL && m_hTargetEnt->IsAlive() && m_hTargetEnt->pev->deadflag != DEAD_DYING ) 
			{																	// this is redundant but I'm paranoid
				CBaseHuman * pTarget = m_hTargetEnt->MyHumanPointer();
				if (pTarget  && pTarget->SafeToChangeSchedule() && !pTarget->HasConditions( bits_COND_SEE_ENEMY ) ) 
				{
					pTarget->ChangeSchedule( pTarget->GetScheduleOfType( SCHED_HUMAN_WAIT_HEAL ) );
				}
			}
			TaskComplete();
		}
		break;

	case TASK_HUMAN_FOLLOWER_MEDIC_CHECK_TARGET:
		{
			// Make sure target is not dead

			if ( m_hTargetEnt!=NULL && m_hTargetEnt->pev->deadflag == DEAD_NO )
			{
				// If Target has an enemy, it's more useful to shoot the enemy than to heal target

				CBaseHuman * pHuman = m_hTargetEnt->MyHumanPointer();
				if ( pHuman && pHuman->m_hEnemy != NULL && pHuman->HasConditions( bits_COND_SEE_ENEMY ) )
				{
					m_hEnemy = pHuman->m_hEnemy;
					SetConditions( bits_COND_NEW_ENEMY );
					TaskFail();
				}
				else
				{
					TaskComplete();
				}
			}
			else
			{
				TaskFail();
			}
		}
		break;

	case TASK_PLAY_SEQUENCE_FACE_TARGET:
		ResetSequenceInfo();
		pev->frame = 0;

	default:
		{
			CBaseHumanFollowerMedic::StartTask( pTask );
		}
		break;
	}
}


//=========================================================
// GetSchedule - Decides which type of schedule best suits
// the monster's current state and conditions. Then calls
// monster's member function to get a pointer to a schedule
// of the proper type.
//=========================================================

Schedule_t *CBaseHumanFollowerMedic :: GetSchedule ( void )
{
	// Flying? If PRONE, barnacle has me. IF not, it's assumed I am repelling. 

	if ( pev->movetype == MOVETYPE_FLY && m_MonsterState != MONSTERSTATE_PRONE )
	{
		return CBaseHuman::GetSchedule();	// Repelling code is handled in human class
	}

	// Humans place HIGH priority on running away from danger sounds.

	if ( HasConditions(bits_COND_HEAR_SOUND) )
	{
		CSound *pSound;
		pSound = PBestSound();
		if ( pSound)
		{
			if (pSound->m_iType & bits_SOUND_DANGER)
			{
				return GetScheduleOfType( SCHED_TAKE_COVER_FROM_BEST_SOUND );
			}
		}
	}
	
	switch( m_MonsterState )
	{
	case MONSTERSTATE_COMBAT:
		{
			if ( HasConditions(bits_COND_NEW_ENEMY) && InSquad() && !IsLeader() && !SquadAnyIdle() )
			{
				MySquadLeader()->m_fEnemyEluded = FALSE;
				return GetScheduleOfType ( SCHED_TAKE_COVER_FROM_ENEMY );
			}
			
			// This over-rides the default human code for occluded enemies because it establishes a line of fire
			// whereas medics are more evasive and care more about healing the wounded than attacking

			if ( HasConditions( bits_COND_ENEMY_OCCLUDED ) )
			{
				if ( IsFollowingHuman() )
				{
					if ( !m_hTargetEnt->IsAlive() || m_hTargetEnt->pev->health > 2*m_hTargetEnt->pev->max_health / 3 )
					{
						m_hTargetEnt->MyHumanPointer()->StopFollowing( TRUE ); // safe because we know it's a human from above
						StopFollowing( TRUE );
						return GetSchedule();
					}
					else
					{
						return GetScheduleOfType( SCHED_HUMAN_FOLLOWER_MEDIC_CHASE );
					}
				}
				else if ( IsFollowingPlayer() )
				{
					if ( m_hTargetEnt->IsAlive() && m_hTargetEnt->pev->health <= 2*m_hTargetEnt->pev->max_health / 3 )
					{
						return GetScheduleOfType( SCHED_HUMAN_FOLLOWER_MEDIC_CHASE );
					}
				}
				else if ( SquadGetWounded() )
				{
					return GetScheduleOfType( SCHED_HUMAN_FOLLOWER_MEDIC_CHASE );
				}
				else if ( HasConditions( bits_COND_CAN_RANGE_ATTACK2 ) && OccupySlot( bits_SLOTS_HUMAN_GRENADE ) )
				{
					return GetScheduleOfType( SCHED_RANGE_ATTACK2 );
				}
				else
				{
					return GetScheduleOfType( SCHED_STANDOFF );
				}
			}
		}
		break;

	case MONSTERSTATE_ALERT:
	case MONSTERSTATE_IDLE:
		{
			// If has no target

			if ( !IsFollowing() )
			{
				// If nearest friendly guy is injured, go over and heal him

				CBaseEntity *pFriend = FindNearestFriend( !HasConditions(bits_COND_PROVOKED) );

				if ( pFriend && pFriend->pev->deadflag == DEAD_NO && pFriend->pev->health <= (2*pFriend->pev->max_health) / 3 )
				{
					StartFollowing( pFriend );
					return GetScheduleOfType( SCHED_HUMAN_FOLLOWER_MEDIC_CHASE );
				}

				// Search for guy with biggest wounds in squad

				if ( SquadGetWounded() )
				{
					return GetScheduleOfType( SCHED_HUMAN_FOLLOWER_MEDIC_CHASE );
				}
			}
			else if ( IsFollowingHuman() )
			{
				if ( !m_hTargetEnt->IsAlive() || m_hTargetEnt->pev->health > 2*m_hTargetEnt->pev->max_health / 3 )
				{
					m_hTargetEnt->MyHumanPointer()->StopFollowing( TRUE ); // safe because we know it's a human from above
					StopFollowing( TRUE );
					return GetSchedule();
				}
				else
				{
					return GetScheduleOfType( SCHED_HUMAN_FOLLOWER_MEDIC_CHASE );
				}
			}
			else if ( IsFollowingPlayer() )
			{
				if ( m_hTargetEnt->IsAlive() && m_hTargetEnt->pev->health <= 2*m_hTargetEnt->pev->max_health / 3 )
				{
					return GetScheduleOfType( SCHED_HUMAN_FOLLOWER_MEDIC_CHASE );
				}
			}
		}
		break;
	}

	return CBaseHumanFollowerMedic::GetSchedule();
}


//=========================================================
// Killed - called when he's killed
//=========================================================

void CBaseHumanFollowerMedic::Killed( entvars_t *pevAttacker, int iGib )
{
/*	if ( m_nHeadNum == GetNumHeads() ) 
		SetBodygroup( GetSyringeGroupNum(), HUMAN_FOLLOWER_MEDIC_SYRINGE_NONE );*/

	CBaseHumanFollowerMedic::Killed( pevAttacker, iGib );
}


//=========================================================
// Get out needle and heal target
//=========================================================

Task_t	tlHumanFollowerMedicHeal[] =
{
	{ TASK_HUMAN_FOLLOWER_MEDIC_CHECK_TARGET,	(float)0			},	// Check if he's still alive, no point healing a dead man
	{ TASK_HUMAN_FOLLOWER_MEDIC_SOUND_HEAL,		(float)0			},  // Tell target to hold still, this won't hurt a bit
	{ TASK_PLAY_SEQUENCE_FACE_TARGET,	(float)ACT_SPECIAL_ATTACK2	},	// Whip out the needle
	{ TASK_SET_ACTIVITY,						(float)ACT_IDLE		},
};

Schedule_t	slHumanFollowerMedicHeal[] =
{
	{
		tlHumanFollowerMedicHeal,
		ARRAYSIZE ( tlHumanFollowerMedicHeal ),
		0,	// Don't interrupt or he'll end up running around with a needle all the time
		0,
		"Human Follower Medic Heal"
	},
};


//=========================================================
// Chase target
//=========================================================

Task_t	tlHumanFollowerMedicChase[] =
{
	{ TASK_MOVE_TO_TARGET_RANGE,				(float)64			},	// Move within 64 of target ent
	{ TASK_FACE_IDEAL,							(float)0			},
	{ TASK_SET_SCHEDULE,	(float)SCHED_HUMAN_FOLLOWER_MEDIC_HEAL	},
};

Schedule_t	slHumanFollowerMedicChase[] =
{
	{
		tlHumanFollowerMedicChase,
		ARRAYSIZE ( tlHumanFollowerMedicChase ),
		bits_COND_LIGHT_DAMAGE	|
		bits_COND_HEAVY_DAMAGE	|
		bits_COND_SEE_ENEMY		|
		bits_COND_HEAR_SOUND	|
		bits_COND_PROVOKED,
		
		bits_SOUND_DANGER,
		"Human Follower Medic Chase"
	},
};


DEFINE_CUSTOM_SCHEDULES( CBaseHumanFollowerMedic )
{
	slHumanFollowerMedicHeal,
	slHumanFollowerMedicChase,
};

IMPLEMENT_CUSTOM_SCHEDULES( CBaseHumanFollowerMedic, CBaseHumanFollowerMedic );


//=========================================================
// AI Schedules Specific to this monster
//=========================================================

Schedule_t* CBaseHumanFollowerMedic :: GetScheduleOfType ( int Type )
{
	switch( Type )
	{
		case SCHED_HUMAN_FOLLOWER_MEDIC_HEAL:
		{
			return &slHumanFollowerMedicHeal[ 0 ];
		}
		break;

		case SCHED_HUMAN_FOLLOWER_MEDIC_CHASE:
		{
			return &slHumanFollowerMedicChase[ 0 ];
		}
		break;
	}

	return CBaseHumanFollowerMedic::GetScheduleOfType( Type );
}