#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"schedule.h"
#include	"squadmonster.h"
#include	"cbasehuman.h"
#include	"cbasehumanfollower.h"
#include	"defaultai.h"
#include	"scripted.h"
#include	"soundent.h"
#include	"animation.h"
#include	"weapons.h"
#include	"decals.h"



#define NUM_HEADS 1

//=====================
// Animation Events
//=====================

class CHumanDead : public CBaseHumanFollower
{
public:
	void Spawn( );
	void Precache();
	int Classify( void ) { return CLASS_PLAYER_ALLY; };

	void GibMonster();
};

LINK_ENTITY_TO_CLASS( monster_cine_dead, CHumanDead );

//=========================================================
// Precache
//=========================================================

void CHumanDead :: Precache()
{
	if (pev->model)
	PRECACHE_MODEL( (char*)STRING(pev->model) );
	else
	PRECACHE_MODEL("models/player/dead.mdl");
	
	if ( pev->frags == HUMAN_WEAPON_NONE )	// If no weapons precache all possible weapon sounds because we could pick up any one
	{
		m_iBrassShell = PRECACHE_MODEL ("models/shell.mdl");// brass shell
		m_iShotgunShell = PRECACHE_MODEL ("models/shotgunshell.mdl"); //shotgun shell
	}
	
	m_szFriends[0] = "monster_cine_dead";
	m_szFriends[1] = "monster_AGMEMBER_medic";
	m_szFriends[2] = "monster_barney";
	m_szFriends[3] = "player";
	m_szFriends[4] = "monster_peasant";
	m_nNumFriendTypes = 5;

	strcpy( m_szSpeechLabel, "MF_");

	CBaseHumanFollower::Precache();
}

//=========================================================
// Spawn
//=========================================================

void CHumanDead::Spawn()
{
    Precache( );

//	SET_MODEL(ENT(pev), "models/player/dead.mdl");

	pev->health			= 100;
	m_flFieldOfView		= VIEW_FIELD_WIDE;	// NOTE: we need a wide field of view so npc will notice player and say hello

	//just in case
	bTerrorist			= FALSE;

	CBaseHumanFollower::Spawn();
	SetBodygroup( 2, 1 );

	CBaseHumanFollower::Killed( pev, GIB_NEVER );
}

//=========================================================
// GibMonster
//=========================================================

void CHumanDead :: GibMonster( void )
{
	CBaseHumanFollower::GibMonster();
}



















//=====================
// Animation Events
//=====================

class CHumanDeadCrouch : public CBaseHumanFollower
{
public:
	void Spawn( );
	void Precache();
	int Classify( void ) { return CLASS_PLAYER_ALLY; };

	void GibMonster();
};

LINK_ENTITY_TO_CLASS( monster_cine_dead_crouch, CHumanDeadCrouch );

//=========================================================
// Precache
//=========================================================

void CHumanDeadCrouch :: Precache()
{
	if (pev->model)
	PRECACHE_MODEL( (char*)STRING(pev->model) );
	else
	PRECACHE_MODEL("models/player/dead.mdl");
	
	if ( pev->frags == HUMAN_WEAPON_NONE )	// If no weapons precache all possible weapon sounds because we could pick up any one
	{
		m_iBrassShell = PRECACHE_MODEL ("models/shell.mdl");// brass shell
		m_iShotgunShell = PRECACHE_MODEL ("models/shotgunshell.mdl"); //shotgun shell
	}
	
	m_szFriends[0] = "monster_cine_dead";
	m_szFriends[1] = "monster_AGMEMBER_medic";
	m_szFriends[2] = "monster_barney";
	m_szFriends[3] = "player";
	m_szFriends[4] = "monster_peasant";
	m_nNumFriendTypes = 5;

	strcpy( m_szSpeechLabel, "MF_");

	CBaseHumanFollower::Precache();
}

//=========================================================
// Spawn
//=========================================================

void CHumanDeadCrouch::Spawn()
{
    Precache( );

	SET_MODEL(ENT(pev), "models/player/dead.mdl");

	pev->health			= 100;
	m_flFieldOfView		= VIEW_FIELD_WIDE;	// NOTE: we need a wide field of view so npc will notice player and say hello

	//just in case
	bTerrorist			= FALSE;
//	m_fCrouching		= TRUE;//this doesn't help

	CBaseHumanFollower::Spawn();
	SetBodygroup( 2, 1 );

	CBaseHumanFollower::Killed( pev, GIB_NEVER );
}

//=========================================================
// GibMonster
//=========================================================

void CHumanDeadCrouch :: GibMonster( void )
{
	CBaseHumanFollower::GibMonster();
}








