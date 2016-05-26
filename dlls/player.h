/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#ifndef PLAYER_H
#define PLAYER_H


#include "pm_materials.h"


#define PLAYER_FATAL_FALL_SPEED		1024// approx 60 feet
#define PLAYER_MAX_SAFE_FALL_SPEED	580// approx 20 feet
#define DAMAGE_FOR_FALL_SPEED		(float) 100 / ( PLAYER_FATAL_FALL_SPEED - PLAYER_MAX_SAFE_FALL_SPEED )// damage per unit per second.
#define PLAYER_MIN_BOUNCE_SPEED		200
#define PLAYER_FALL_PUNCH_THRESHHOLD (float)350 // won't punch player's screen/make scrape noise unless player falling at least this fast.
#define	PLAYER_SEARCH_RADIUS		(float)64//sys add

//
// Player PHYSICS FLAGS bits
//
#define		PFLAG_ONLADDER		( 1<<0 )
#define		PFLAG_ONSWING		( 1<<0 )
#define		PFLAG_ONTRAIN		( 1<<1 )
#define		PFLAG_ONBARNACLE	( 1<<2 )
#define		PFLAG_DUCKING		( 1<<3 )		// In the process of ducking, but totally squatted yet
#define		PFLAG_USING			( 1<<4 )		// Using a continuous entity
#define		PFLAG_OBSERVER		( 1<<5 )		// player is locked in stationary cam mode. Spectators can move, observers can't.

//
// generic player
//
//-----------------------------------------------------
//This is Half-Life player entity
//-----------------------------------------------------
#define CSUITPLAYLIST	4		// max of 4 suit sentences queued up at any time

#define SUIT_GROUP			TRUE
#define	SUIT_SENTENCE		FALSE

#define	SUIT_REPEAT_OK		0
#define SUIT_NEXT_IN_3SEC	3
#define SUIT_NEXT_IN_5SEC	5
#define SUIT_NEXT_IN_30SEC	30
#define SUIT_NEXT_IN_1MIN	60
#define SUIT_NEXT_IN_5MIN	300
#define SUIT_NEXT_IN_10MIN	600
#define SUIT_NEXT_IN_30MIN	1800
#define SUIT_NEXT_IN_1HOUR	3600

#define CSUITNOREPEAT		32

#define	SOUND_FLASHLIGHT_ON		"items/flashlight1.wav"
#define	SOUND_FLASHLIGHT_OFF	"items/flashlight1.wav"

#define TEAM_NAME_LENGTH	16

class CGrass;
class CParticleEmitter;

typedef enum
{
	PLAYER_IDLE,
	PLAYER_WALK,
	PLAYER_JUMP,
	PLAYER_SUPERJUMP,
	PLAYER_DIE,
	PLAYER_ATTACK1,
	PLAYER_RELOAD,
	PLAYER_DRAW,
} PLAYER_ANIM;

#define MAX_ID_RANGE 2048
#define SBAR_STRING_SIZE 128
#define MAX_PAGES 10

enum sbar_data
{
	SBAR_ID_TARGETNAME = 1,
	SBAR_ID_TARGETHEALTH,
	SBAR_ID_TARGETARMOR,
	SBAR_END,
};

#define CHAT_INTERVAL 1.0f

#define STEALTH 0
#define DYNAMIC 1

// buz: flags for goal panel
#define TP_FL_ENABLE	1
#define TP_FL_TITLE		2
#define TP_FL_IMAGE		4
#define TP_FL_POPUP		8

#define PLAYER_MAX_NORMAL_VELOCITY		280

class CBasePlayer : public CBaseMonster
{
public:
	int iLevelNotified;

	// buz: current goal description
	string_t	m_strCurrentGoalName;
	string_t	m_strCurrentGoalImageName;
	string_t	m_strCurrentGoalTitleName;
	int			m_iGoalNeedsUpdate;

	void	PlayerSetGoalDesc( string_t strindex, string_t title, string_t imgname );

//	int iFollowers;

	BOOL bBlurredByEntity;
//	int m_iFirstSlot;// Primary Weapons, only one per slot

	//SYS: dynamic music related - start
	void PlayDynamicMusic();
	void FmodFadeIn();
	void FmodFadeOut();
	int iStamina;
	
	char szCommand[32];

	int iCounter;

	bool bActualMode;
	bool bPlaying;
	bool bCheckedWater;

	bool bConditionsCleared;

	bool bShouldFadeIn;
	bool bShouldFadeOut;

	float fFadeInTime;
	float fFadeOutTime;
	float fMP3Volume;
	//SYS: dynamic music related - end

	void ResetOpenGL( void );

	float m_flTimerTime;
	float m_fGrayscale;
	float m_flTimerStartTime;
	BOOL m_fTimerActivePlayer;
	BOOL m_fTimerJustStarted;
	string_t m_TimerTargetPlayer;
	CBaseEntity *pTimerEntityPlayer;

	void InitCountdown( float flCountdownTime, BOOL m_fTimerActive, string_t m_TimerTarget, CBaseEntity *pTimerEntity );

	void ViewPunch( float p, float y, float r ); // buz - нагло спер из хл2

    /*
	float f_NextTipTime;
	float i_TipCounter;
*/
	BOOL	m_bSpawnWhitAI;
	int	g_iNextEntToSpawn;
//	bool OldGlauncher;
	float				m_flRadarNextSoundTime; // the time after which the player can next use the suicide command

	//HL: TOwn car's Code
	//Thanks to protector

	// I think the following two lines are not necessary
	float	fNextEntUpdate;
	int		savecountents;
	float	flCarUseTime;
	float	fl_CarViewOffset;
	int		iCarFixAngle;
	float	flCarFixUseTime;
	int		saved_flags;
	float	flCarHornTime;
	float	flCarDuckTime;

	CBaseEntity *pCar;
	int iSprite;

	//HL:TOWN
	void PlayerMoveForward( void );
	void PlayerMoveBackward( void );
	void PlayerMoveLeft( void );
	void PlayerMoveRight( void );

	void PlayerCarJump( void );
	void PlayerCarDuck( void );
	void PlayerCarHorn( void );
	void PlayerCarUse( void );

	void PlayerCarAttack( void );
	void PlayerCarAttack2( void );
	//HL:TOWN

	//HL: TOwn car's Code
	BOOL				m_fRespawned;				// True when fog update msg needs to be sent
	bool				m_bSpawnPS;
	bool				m_bSpawnGrass;
	float				m_flLastPSSpawn;
	float				m_flLastGrassSpawn;
	CGrass				*pLastGrassSpawned;
	CParticleEmitter	*pLastPSSpawned;
	//Ben

	BOOL b_FlashlightIsOn;
	BOOL m_bPlayerNearSmoke;
	
	int f_NextKickTime;

	BOOL m_bSilAdd;//ber
	BOOL m_bMp5SilAdd;//mp5
	BOOL m_bM16SilAdd;//m16
	BOOL m_bUZISilAdd;//usi

	BOOL m_fParachute; //the variable which checks if we have the parachute
	BOOL m_fParaOpen; //the variable which cheks if the parachute is opened
	BOOL m_fNvg; //booleano, si tienes NVG
	BOOL m_fNvgOn; //si esta activado o no.
	BOOL m_fIMode; //si esta en modo I = Infrared

	int m_iNextInfraredUpdate;

	void NVGToggle(BOOL activate);
	
	void NvgUpdate();

	BOOL m_fCrosshairOff; //si es true, no crosshair, si es flaso, si crooshair
	BOOL m_fCanUseFlashlight;

float posiciondemanos_abajo;
float posiciondemanos_adentro;
float posiciondemanos_medio;
float posiciondezoom;

	int iWeaponsPicked;

	BOOL bRadarIsOn; //
	BOOL	m_InDREAM;//don't send fades and such

	BOOL	b_UsingHL2Weapon;//for use HEV
	BOOL	bCanCheckDoor;

	float	m_flPlayerNextBloodTime;

	float	m_flNextBreathTime;
	float	m_flNextTumbleTime;
	float	m_flNextOrderTime;
	
	float m_flNextTDetailUpdateTime;
	float m_flNextBitsSoundsUpdateTime;
	float m_flNextPhysicsValueUpdateTime;
	float f_NextOffTime;
	float m_flNextFlashbangUpdateTime;

	int		m_fFireMode;
	int		fCurrentPage;
	BOOL b_PlayerCanHideInShadows; //si es true, el personaje puede esconderse de las sombras

	void CineToggle(BOOL activate);
	BOOL m_fCineOn; //si esta activado o no.

	virtual void ShowMenu(CBasePlayer *pPlayer, int bitsValidSlots, int nDisplayTime, BOOL fNeedMore, char *pszText);
	
	void EXPORT RestoreSpeedVol( void );
	
	void EXPORT SlowMotion( void );
	void EXPORT SlowMotionOff( void );

	void Nvg( void ); //we need to define this function before we can call/write it

	void ParaGlide( void ); //we need to define this function before we can call/write it
//	void Jetpack( void ); //we need to define this function before we can call/write it

	int					random_seed;

	int					m_iPlayerSound;// the index of the sound list slot reserved for this player
	int					m_iTargetVolume;// ideal sound volume. 
	int					m_iWeaponVolume;// how loud the player's weapon is right now.
	int					m_iExtraSoundTypes;// additional classification for this weapon's sound
	int					m_iWeaponFlash;// brightness of the weapon flash
	float				m_flStopExtraSoundTime;
	
	float				m_flFlashLightTime;	// Time until next battery draw/Recharge
	int					m_iFlashBattery;		// Flashlight Battery Draw

	int					m_afButtonLast;
	int					m_afButtonPressed;
	int					m_afButtonReleased;
	
	edict_t			   *m_pentSndLast;			// last sound entity to modify player room type
	float				m_flSndRoomtype;		// last roomtype set by sound entity
	float				m_flSndRange;			// dist from player to sound entity

	float				m_flFallVelocity;

	int					m_nmenudisp;

	int					m_rgItems[MAX_ITEMS];
	int					m_fKnownItem;		// True when a new item needs to be added
	int					m_fNewAmmo;			// True when a new item has been added

	unsigned int		m_afPhysicsFlags;	// physics flags - set when 'normal' physics should be revisited or overriden
	float				m_fNextSuicideTime; // the time after which the player can next use the suicide command


// these are time-sensitive things that we keep track of
	float				m_flTimeStepSound;	// when the last stepping sound was made
	float				m_flTimeWeaponIdle; // when to play another weapon idle animation.
	float				m_flSwimTime;		// how long player has been underwater
	float				m_flDuckTime;		// how long we've been ducking
	float				m_flWallJumpTime;	// how long until next walljump

	float				m_flSuitUpdate;					// when to play next suit update
	int					m_rgSuitPlayList[CSUITPLAYLIST];// next sentencenum to play for suit update
	int					m_iSuitPlayNext;				// next sentence slot for queue storage;
	int					m_rgiSuitNoRepeat[CSUITNOREPEAT];		// suit sentence no repeat list
	float				m_rgflSuitNoRepeatTime[CSUITNOREPEAT];	// how long to wait before allowing repeat
	int					m_lastDamageAmount;		// Last damage taken
	float				m_tbdPrev;				// Time-based damage timer

	float				m_flgeigerRange;		// range to nearest radiation source
	float				m_flgeigerDelay;		// delay per update of range msg to client
	int					m_igeigerRangePrev;
	int					m_iStepLeft;			// alternate left/right foot stepping sound
	char				m_szTextureName[CBTEXTURENAMEMAX];	// current texture name we're standing on
	char				m_chTextureType;		// current texture type

	int					m_idrowndmg;			// track drowning damage taken
	int					m_idrownrestored;		// track drowning damage restored

	int					m_bitsHUDDamage;		// Damage bits for the current fame. These get sent to 
												// the hude via the DAMAGE message
//	BOOL				m_fStealth; //sys
	BOOL				m_bIron; //sys
	BOOL				m_fInitHUD;				// True when deferred HUD restart msg needs to be sent
	BOOL				m_fGameHUDInitialized;
	int					m_iTrain;				// Train control position
	BOOL				m_fWeapon;				// Set this to FALSE to force a reset of the current weapon HUD info

	EHANDLE				m_pTank;				// the tank which the player is currently controlling,  NULL if no tank
	float				m_fDeadTime;			// the time at which the player died  (used in PlayerDeathThink())
	BOOL				m_bPoitingTerrorist;

	int modelindexsave;//SP
	string_t modelsave;

	BOOL			m_fNoPlayerSound;	// a debugging feature. Player makes no sound if this is true. 
	BOOL			m_fLongJump; // does this player have the longjump module?
//sys
	BOOL			m_fHurted; // does this player have the longjump module?
	void Stealth( void ); 

	float       m_tSneaking;
	int			m_iUpdateTime;		// stores the number of frame ticks before sending HUD update messages
	int			m_iClientHealth;	// the health currently known by the client.  If this changes, send a new
	int			m_iClientArmor;	// the Armor currently known by the client.  If this changes, send a new
	int			m_iClientSlowMotion;	// the Armor currently known by the client.  If this changes, send a new
	int			m_iHideHUD;		// the players hud weapon info is to be hidden
	int			m_iClientHideHUD;
	int			m_iFOV;			// field of view
	int			m_iClientFOV;	// client's known FOV
	// usable player items 
	CBasePlayerItem	*m_rgpPlayerItems[MAX_ITEM_TYPES];
	CBasePlayerItem *m_pActiveItem;
	CBasePlayerItem *m_pClientActiveItem;  // client version of the active item
	CBasePlayerItem *m_pLastItem;
	// shared ammo slots
	int	m_rgAmmo[MAX_AMMO_SLOTS];
	int	m_rgAmmoLast[MAX_AMMO_SLOTS];
 Vector v_LastAngles;
	Vector				m_vecAutoAim;
	BOOL				m_fOnTarget;
	int					m_iDeaths;
	float				m_iRespawnFrames;	// used in PlayerDeathThink() to make sure players can always respawn

	int m_lastx, m_lasty;  // These are the previous update's crosshair angles, DON"T SAVE/RESTORE

	int m_nCustomSprayFrames;// Custom clan logo frames for this player
	float	m_flNextDecalTime;// next time this player can spray a decal

	char m_szTeamName[TEAM_NAME_LENGTH];

	virtual void Spawn( void );
	void Pain( void );

//	virtual void Think( void );
	virtual void Jump( void );
	virtual void Duck( void );
	virtual void PreThink( void );
	virtual void PostThink( void );
	virtual Vector GetGunPosition( void );
	virtual int TakeHealth( float flHealth, int bitsDamageType );
	virtual void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	virtual int TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	virtual void	Killed( entvars_t *pevAttacker, int iGib );
	virtual Vector BodyTarget( const Vector &posSrc ) { return Center( ) + pev->view_ofs * RANDOM_FLOAT( 0.5, 1.1 ); };		// position to shoot at
	virtual void StartSneaking( void ) { m_tSneaking = gpGlobals->time - 1; }
	virtual void StopSneaking( void ) { m_tSneaking = gpGlobals->time + 30; }
	virtual BOOL IsSneaking( void ) { return m_tSneaking <= gpGlobals->time; }
	virtual BOOL IsAlive( void ) { return (pev->deadflag == DEAD_NO) && pev->health > 0; }
	virtual BOOL ShouldFadeOnDeath( void ) { return FALSE; }
	virtual	BOOL IsPlayer( void ) { return TRUE; }			// Spectators should return FALSE for this, they aren't "players" as far as game logic is concerned

	virtual BOOL IsNetClient( void ) { return TRUE; }		// Bots should return FALSE for this, they can't receive NET messages
															// Spectators should return TRUE for this
	virtual const char *TeamID( void );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	void RenewItems(void);
	void PackDeadPlayerItems( void );
	void RemoveAllItems( BOOL removeSuit );
	BOOL SwitchWeapon( CBasePlayerItem *pWeapon );

	void RemoveItems( int iWeaponMask, int i9mm, int i357, int iBuck, int iBolt, int iARGren, int iRock, int iEgon, int iSatchel, int iSnark, int iTrip, int iGren, int iHornet );
	void RemoveAmmo( const char* szName, int iAmount );
	

	// JOHN:  sends custom messages if player HUD data has changed  (eg health, ammo)
	virtual void UpdateClientData( void );
	
	static	TYPEDESCRIPTION m_playerSaveData[];

	// Player is moved across the transition by other means
	virtual int		ObjectCaps( void ) { return CBaseMonster :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	virtual void	Precache( void );
	BOOL			IsOnLadder( void );
	BOOL			FlashlightIsOn( void );
	void			FlashlightTurnOn( void );
	void			FlashlightTurnOff( void );
	
	void UpdatePlayerSound ( void );
	void DeathSound ( void );

	int Classify ( void );
	void SetAnimation( PLAYER_ANIM playerAnim );
	void SetWeaponAnimType( const char *szExtention );
	char m_szAnimExtention[32];

	// custom player functions
	virtual void ImpulseCommands( void );
	void CheatImpulseCommands( int iImpulse );

	void StartDeathCam( void );
	void StartObserver( Vector vecPosition, Vector vecViewAngle );

	void AddPoints( int score, BOOL bAllowNegativeScore );
	void AddPointsToTeam( int score, BOOL bAllowNegativeScore );
	BOOL AddPlayerItem( CBasePlayerItem *pItem );
	BOOL RemovePlayerItem( CBasePlayerItem *pItem );
	void DropPlayerItem ( char *pszItemName );
	BOOL HasPlayerItem( CBasePlayerItem *pCheckItem );
	BOOL HasNamedPlayerItem( const char *pszItemName );
	BOOL HasWeapons( void );// do I have ANY weapons?
	void SelectPrevItem( int iItem );
	void SelectNextItem( int iItem );
	void SelectLastItem(void);
	void SelectItem(const char *pstr);
	void ItemPreFrame( void );
	void ItemPostFrame( void );
	void GiveNamedItem( const char *szName );
	void EnableControl(BOOL fControl);

	int  GiveAmmo( int iAmount, char *szName, int iMax );
	void SendAmmoUpdate(void);

	void WaterMove( void );
	void EXPORT PlayerDeathThink( void );
	void PlayerUse( void );

	void CheckSuitUpdate();
	void SetSuitUpdate(char *name, int fgroup, int iNoRepeat);
	void UpdateGeigerCounter( void );
	void CheckTimeBasedDamage( void );

	BOOL FBecomeProne ( void );
	void BarnacleVictimBitten ( entvars_t *pevBarnacle );
	void BarnacleVictimReleased ( void );
	static int GetAmmoIndex(const char *psz);
	int AmmoInventory( int iAmmoIndex );
	int Illumination( void );

	void ResetAutoaim( void );
	Vector GetAutoaimVector( float flDelta  );
	Vector AutoaimDeflection( Vector &vecSrc, float flDist, float flDelta  );

	void ForceClientDllUpdate( void );  // Forces all client .dll specific data to be resent to client.

	void DeathMessage( entvars_t *pevKiller );

	void SetCustomDecalFrames( int nFrames );
	int GetCustomDecalFrames( void );

	void CBasePlayer::TabulateAmmo( void );

	float m_flStartCharge;
	float m_flAmmoStartCharge;
	float m_flPlayAftershock;
	float m_flNextAmmoBurn;// while charging, when to absorb another unit of player's ammo?
	
	//Player ID
	void InitStatusBar( void );
	void UpdateStatusBar( void );
	int m_izSBarState[ SBAR_END ];
	float m_flNextSBarUpdateTime;
	float m_flStatusBarDisappearDelay;
	char m_SbarString0[ SBAR_STRING_SIZE ];
	char m_SbarString1[ SBAR_STRING_SIZE ];
	
	float m_flNextChatTime;

		
	int	Rain_dripsPerSecond;
	float	Rain_windX, Rain_windY;
	float	Rain_randX, Rain_randY;

	int	Rain_ideal_dripsPerSecond;
	float	Rain_ideal_windX, Rain_ideal_windY;
	float	Rain_ideal_randX, Rain_ideal_randY;

	float	Rain_endFade; // 0 means off
	float	Rain_nextFadeUpdate;

	int	Rain_needsUpdate;
};

#define AUTOAIM_2DEGREES  0.0348994967025
#define AUTOAIM_5DEGREES  0.08715574274766
#define AUTOAIM_8DEGREES  0.1391731009601
#define AUTOAIM_10DEGREES 0.1736481776669

extern int	gmsgParticle; // LRC
extern int	gmsgHudText;
extern int	gmsgSetBody;
extern int	gmsgSetSkin;
extern BOOL gInitHUD;

#endif // PLAYER_H
