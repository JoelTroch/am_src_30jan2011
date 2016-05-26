#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "saverestore.h"
#include "func_break.h"
#include "explode.h"
#include "soundent.h"

#include "effects.h"//for sprite effects 
#include "particle_defs.h"

#include "game.h"
#include "nodes.h"
#include "weapons.h"
#include "monsters.h"
//#include "effects.cpp"
extern int gmsgParticles;

extern DLL_GLOBAL Vector		g_vecAttackDir;

#define	NUM_SHARDS 6 // this many shards spawned when breakable objects break;


//foxtrop: change body when health go down
//#define PROP_BREAK_PERFECT	20
#define PROP_BREAK_MEDIUM	10
#define PROP_BREAK_BAD		5

#define PROP_STATEGROUP		1

#define PROP_BODY_PERFECT	0
#define PROP_BODY_MEDIUM	1
#define PROP_BODY_BAD		2
//this variables are for Arrangment but can work on spirit or hlsdk 2.3
#define SF_DONTUSELOD			8
#define SF_USELOD_HIGH			16
#define SF_USELOD_LOW			32
#define SF_DONTCHECK_FOV		64
//#define SF_CREATELIGHT			128
#define SF_ONSIGHT_STATECHECK	0x00004 //AJH
#define SF_USE_REALLOD			256
#define SF_NO_PHYS			512

class CProp_break : public CBaseAnimating
{
public:
	// basic functions
	void Spawn( void );
	void Precache( void );
	void KeyValue( KeyValueData* pkvd);
	void EXPORT BreakTouch( CBaseEntity *pOther );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void DamageSound( void );

	void Think( void );
	BOOL VisionCheck2( void );
	BOOL CanSee2(CBaseEntity *pLooker, CBaseEntity *pSeen);

	BOOL m_CeIsRope;
	BOOL m_bDoRealLOD;

	// breakables use an overridden takedamage
	virtual int TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType );
	// To spark when hit
	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType );

	BOOL m_IsFlamable;

	BOOL IsBreakable( void );
	BOOL SparkWhenHit( void );
	BOOL bNoModel;
//	int	 DamageDecal( int bitsDamageType );
//	int iContents = UTIL_PointContents ( pev->origin );//moved up

	void EXPORT		Die( void );
	virtual int		ObjectCaps( void ) { return (CBaseEntity :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION); }
	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );

	inline BOOL		Explodable( void ) { return ExplosionMagnitude() > 0; }
	inline int		ExplosionMagnitude( void ) { return pev->impulse; }
	inline void		ExplosionSetMagnitude( int magnitude ) { pev->impulse = magnitude; }

	static void MaterialSoundPrecache( Materials precacheMaterial );
	static void MaterialSoundRandom( edict_t *pEdict, Materials soundMaterial, float volume );
	static const char **MaterialSoundList( Materials precacheMaterial, int &soundCount );

	static const char *pSoundsWood[];
	static const char *pSoundsFlesh[];
	static const char *pSoundsGlass[];
	static const char *pSoundsMetal[];
	static const char *pSoundsConcrete[];
	static const char *pSpawnObjects[];

	static	TYPEDESCRIPTION m_SaveData[];

	Materials	m_Material;
	Explosions	m_Explosion;
	int			m_idShard;
	float		m_angle;
	int			m_iszGibModel;
	int			m_iszSpawnObject;

	
	void	Touch ( CBaseEntity *pOther );
};

//#endif	// FUNC_BREAK_H

// =================== FUNC_Breakable ==============================================

// Just add more items to the bottom of this array and they will automagically be supported
// This is done instead of just a classname in the FGD so we can control which entities can
// be spawned, and still remain fairly flexible
const char *CProp_break::pSpawnObjects[] =
{
	NULL,				// 0
	"item_battery",		// 1
	"item_healthkit",	// 2
	"weapon_9mmhandgun",// 3
	"ammo_9mmclip",		// 4
	"weapon_9mmAR",		// 5
	"ammo_9mmAR",		// 6
	"ammo_ARgrenades",	// 7
	"weapon_shotgun",	// 8
	"ammo_buckshot",	// 9
	"weapon_crossbow",	// 10
	"ammo_crossbow",	// 11
	"weapon_357",		// 12
	"ammo_357",			// 13
	"weapon_rpg",		// 14
	"ammo_rpgclip",		// 15
	"ammo_gaussclip",	// 16
	"weapon_handgrenade",// 17
	"weapon_tripmine",	// 18
	"weapon_satchel",	// 19
	"weapon_snark",		// 20
	"weapon_hornetgun",	// 21
	//Begin Here
	//"xp_point_brown",	// 22
	//"xp_point_plate",	// 23
	//"xp_point_gold",	// 24
};

void CProp_break::KeyValue( KeyValueData* pkvd )
{
	// UNDONE_WC: explicitly ignoring these fields, but they shouldn't be in the map file!
	if (FStrEq(pkvd->szKeyName, "explosion"))
	{
		if (!stricmp(pkvd->szValue, "directed"))
		{
			m_IsFlamable = 1;
			m_Explosion = expDirected;
		}
		else if (!stricmp(pkvd->szValue, "random"))
			m_Explosion = expRandom;
		else
			m_Explosion = expRandom;

		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "material"))
	{
		int i = atoi( pkvd->szValue);

		// 0:glass, 1:metal, 2:flesh, 3:wood

		if ((i < 0) || (i >= matLastMaterial))
			m_Material = matWood;
		else
			m_Material = (Materials)i;

		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "deadmodel"))
	{
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "shards"))
	{
//			m_iShards = atof(pkvd->szValue);
			pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "gibmodel") )
	{
		m_iszGibModel = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "spawnobject") )
	{
		int object = atoi( pkvd->szValue );
		if ( object > 0 && object < ARRAYSIZE(pSpawnObjects) )
			m_iszSpawnObject = MAKE_STRING( pSpawnObjects[object] );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "explodemagnitude") )
	{
		ExplosionSetMagnitude( atoi( pkvd->szValue ) );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "lip") )
		pkvd->fHandled = TRUE;
	else
		CBaseDelay::KeyValue( pkvd );
}


//
// func_breakable - bmodel that breaks into pieces after taking damage
//
LINK_ENTITY_TO_CLASS( prop_break, CProp_break );
LINK_ENTITY_TO_CLASS( prop_brekeable, CProp_break );//oops, spelling issues
LINK_ENTITY_TO_CLASS( env_model_breakable, CProp_break );

TYPEDESCRIPTION CProp_break::m_SaveData[] =
{
	DEFINE_FIELD( CProp_break, m_Material, FIELD_INTEGER ),
	DEFINE_FIELD( CProp_break, m_Explosion, FIELD_INTEGER ),

// Don't need to save/restore these because we precache after restore
//	DEFINE_FIELD( CBreakable, m_idShard, FIELD_INTEGER ),

	DEFINE_FIELD( CBreakable, m_angle, FIELD_FLOAT ),
	DEFINE_FIELD( CBreakable, m_iszGibModel, FIELD_STRING ),
	DEFINE_FIELD( CBreakable, m_iszSpawnObject, FIELD_STRING ),

	// Explosion magnitude is stored in pev->impulse
};

IMPLEMENT_SAVERESTORE( CProp_break, CBaseAnimating );

void CProp_break::Spawn( void )
{
    Precache( );    

	if ( FBitSet( pev->spawnflags, SF_BREAK_TRIGGER_ONLY ) )
		pev->takedamage	= DAMAGE_NO;
	else
	 	pev->takedamage	= DAMAGE_YES;

	Precache( );
	if (pev->model)
	SET_MODEL( ENT(pev), STRING(pev->model) );
	else
	SET_MODEL(ENT(pev), "models/props/wood_crate01.mdl");
	if (pev->health == 0) //LRC
		pev->health			= 30;
	SetBodygroup( PROP_STATEGROUP, PROP_BODY_PERFECT);


//	pev->solid = SOLID_SLIDEBOX;
//	pev->movetype		=  MOVETYPE_PUSHSTEP;

	if ( FStringNull( pev->speed ))
		pev->speed = 0.1;
	
	if ( FStringNull( pev->health ))
		pev->health = 999999;//Fix para mapas con anteriores entidades func_phy

	if ( FStringNull( pev->gravity ))
		pev->gravity = 1;//Fix para mapas con anteriores entidades func_phy

	if ( FStringNull( pev->friction ))
		pev->friction = 0.8;//Fix para mapas con anteriores entidades func_phy

	pev->movetype	= MOVETYPE_BOUNCE;//MOVETYPE_PUSHSTEP;
	pev->solid		= SOLID_BBOX;




    m_angle			= pev->angles.y;
	pev->angles.y	= 0;
	SetModelCollisionBox();

	// HACK:  matGlass can receive decals, we need the client to know about this
	//  so use class to store the material flag
	/*
	if ( m_Material == matGlass )
	{
		pev->playerclass = 1;
		//FIXME: all piece of glass mat has a additive render mode as default.
		/*
		pev->rendermode = kRenderNormal;			
		pGib->pev->rendermode = pev->rendermode;
		pGib->pev->renderamt = pev->renderamt;
		pGib->pev->rendercolor = pev->rendercolor;
		pGib->pev->renderfx = pev->renderfx;
		*/
	//}
//  	m_bloodColor	= DONT_BLEED;
	pev->origin.z += 1;
	DROP_TO_FLOOR ( ENT(pev) );
//	SET_MODEL(ENT(pev), STRING(pev->model) );//set size and link into world.

	SetTouch( BreakTouch );
	if ( FBitSet( pev->spawnflags, SF_BREAK_TRIGGER_ONLY ) )		// Only break on trigger
		SetTouch( NULL );
/*
	// Flag unbreakable glass as "worldbrush" so it will block ALL tracelines
	if ( !IsBreakable() && pev->rendermode != kRenderNormal )
		pev->flags |= FL_WORLDBRUSH;
		*/
}
void CProp_break::Think( void )
{
	//LODfoxxy
//		int iTemp;

	StudioFrameAdvance ( ); // set m_fSequenceFinished if necessary
/*
	if (m_fSequenceFinished && !m_fSequenceLoops)
	{
		if (pev->spawnflags & SF_ENVMODEL_OFF)
			iTemp = m_iAction_Off;
		else
			iTemp = m_iAction_On;

		switch (iTemp)
		{
		case 2: // change state
			if (pev->spawnflags & SF_ENVMODEL_OFF)
				pev->spawnflags &= ~SF_ENVMODEL_OFF;
			else
				pev->spawnflags |= SF_ENVMODEL_OFF;
			SetSequence();
			break;
		default: //remain frozen
			return;
		}
	}
	*/	
	if ( CVAR_GET_FLOAT( "cl_drawprops" ) == 0 )
	{
		pev->effects |= EF_NODRAW;//hide
	}
	else
	{
		if (!(pev->spawnflags & SF_DONTCHECK_FOV))//check fov
		{
			if (VisionCheck2())//can we see the model?
			{
				pev->effects &= ~EF_NODRAW;//ok, draw it
			}
			else
			{
				pev->effects |= EF_NODRAW;//hide it
			}
		}

		//MAKE FAKE Level Of Detail
		//if the model its too far away... let's hide it				
		CBaseEntity *pPlayer = NULL;
		pPlayer = UTIL_FindEntityByClassname( NULL, "player" );

		if (pPlayer)
		{
			float flDist = (pPlayer->Center() - pev->origin).Length();

				if (!(pev->spawnflags & SF_DONTUSELOD))//si no tiene NO usar lod = Usa LOD
				{
					if (pev->spawnflags & SF_USELOD_HIGH)//demasiado LOD
					{
						if ( flDist >= 300)
						{
							pev->effects |= EF_NODRAW;//hide
							pev->renderamt = 0;
						}

						else if (flDist >= 295)
								pev->renderamt = 10;
						else if (flDist >= 290)
								pev->renderamt = 20;
						else if (flDist >= 285)
								pev->renderamt = 30;
						else if (flDist >= 280)
								pev->renderamt = 40;
						else if (flDist >= 275)
								pev->renderamt = 50;
						else if (flDist >= 270)
								pev->renderamt = 60;
						else if (flDist >= 265)
								pev->renderamt = 70;
						else if (flDist >= 260)
								pev->renderamt = 80;
						else if (flDist >= 255)
								pev->renderamt = 90;
						else if (flDist >= 250)
								pev->renderamt = 100;
						else if (flDist >= 245)
								pev->renderamt = 110;
						else if (flDist >= 240)
								pev->renderamt = 120;
						else if (flDist >= 235)
								pev->renderamt = 130;
						else if (flDist >= 230)
								pev->renderamt = 140;
						else if (flDist >= 225)
								pev->renderamt = 150;
						else if (flDist >= 220)
								pev->renderamt = 160;
						else if (flDist >= 215)
								pev->renderamt = 170;
						else if (flDist >= 210)
								pev->renderamt = 180;
						else if (flDist >= 200)
								pev->renderamt = 190;
						else if (flDist >= 195)
								pev->renderamt = 200;
						else if (flDist >= 190)
								pev->renderamt = 210;
						else if (flDist >= 185)
								pev->renderamt = 220;
						else if (flDist >= 180)
								pev->renderamt = 230;
						else if (flDist >= 175)
								pev->renderamt = 240;
						else
								pev->renderamt = 255;
					}
					else if (pev->spawnflags & SF_USELOD_LOW)//poco LOD
					{
						if ( flDist >= 900)
						{
							pev->effects |= EF_NODRAW;//hide
							pev->renderamt = 0;
						}

						else if (flDist >= 890)
								pev->renderamt = 10;
						else if (flDist >= 880)
								pev->renderamt = 20;
						else if (flDist >= 870)
								pev->renderamt = 30;
						else if (flDist >= 860)
								pev->renderamt = 40;
						else if (flDist >= 850)
								pev->renderamt = 50;
						else if (flDist >= 840)
								pev->renderamt = 60;
						else if (flDist >= 830)
								pev->renderamt = 70;
						else if (flDist >= 820)
								pev->renderamt = 80;
						else if (flDist >= 810)
								pev->renderamt = 90;
						else if (flDist >= 800)
								pev->renderamt = 100;
						else if (flDist >= 790)
								pev->renderamt = 110;
						else if (flDist >= 780)
								pev->renderamt = 120;
						else if (flDist >= 770)
								pev->renderamt = 130;
						else if (flDist >= 760)
								pev->renderamt = 140;
						else if (flDist >= 750)
								pev->renderamt = 150;
						else if (flDist >= 740)
								pev->renderamt = 160;
						else if (flDist >= 730)
								pev->renderamt = 170;
						else if (flDist >= 720)
								pev->renderamt = 180;
						else if (flDist >= 710)
								pev->renderamt = 190;
						else if (flDist >= 700)
								pev->renderamt = 200;
						else if (flDist >= 690)
								pev->renderamt = 210;
						else if (flDist >= 680)
								pev->renderamt = 220;
						else if (flDist >= 670)
								pev->renderamt = 230;
						else if (flDist >= 660)
								pev->renderamt = 240;
						else
								pev->renderamt = 255;
					}
					else//normal LOD
					{
						if ( flDist >= 500)//900 it's too larger... using 500
						{
							pev->effects |= EF_NODRAW;//hide
							pev->renderamt = 0;
						}

						else if (flDist >= 490)
								pev->renderamt = 10;
						else if (flDist >= 480)
								pev->renderamt = 20;
						else if (flDist >= 470)
								pev->renderamt = 30;
						else if (flDist >= 460)
								pev->renderamt = 40;
						else if (flDist >= 450)
								pev->renderamt = 50;
						else if (flDist >= 440)
								pev->renderamt = 60;
						else if (flDist >= 430)
								pev->renderamt = 70;
						else if (flDist >= 420)
								pev->renderamt = 80;
						else if (flDist >= 410)
								pev->renderamt = 90;
						else if (flDist >= 400)
								pev->renderamt = 100;
						else if (flDist >= 390)
								pev->renderamt = 110;
						else if (flDist >= 380)
								pev->renderamt = 120;
						else if (flDist >= 370)
								pev->renderamt = 130;
						else if (flDist >= 360)
								pev->renderamt = 140;
						else if (flDist >= 350)
								pev->renderamt = 150;
						else if (flDist >= 340)
								pev->renderamt = 160;
						else if (flDist >= 330)
								pev->renderamt = 170;
						else if (flDist >= 320)
								pev->renderamt = 180;
						else if (flDist >= 310)
								pev->renderamt = 190;
						else if (flDist >= 300)
								pev->renderamt = 200;
						else if (flDist >= 290)
								pev->renderamt = 210;
						else if (flDist >= 280)
								pev->renderamt = 220;
						else if (flDist >= 270)
								pev->renderamt = 230;
						else if (flDist >= 260)
								pev->renderamt = 240;
						else
								pev->renderamt = 255;
					}
				}//eo dont use lod check

				if(m_bDoRealLOD)
				{						
					if (flDist >= 1000)
					{
						pev->body = 2;
					}
					else if (flDist >= 500)
					{
						pev->body = 1;
					}
					else
					{
						pev->body = 0;
					}
					
					if (VisionCheck2())//can I see the model?
					{
						if (flDist >= 2000)//soooo far
						pev->effects |= EF_NODRAW;
						else
						pev->effects &= ~EF_NODRAW;
					}						
					else
					{
						pev->effects |= EF_NODRAW;//hide it
					}
				}
		//	}//eo is player //not necesary since we're looking for the palyer
		}//eo while
	}//eo cvar check
	

	CBaseEntity *pEntityPlayer = NULL;

	pev->nextthink = 0.5;//0.1
	//LODfoxxy
		//foxtrop: body damage models
	if (pev->health > PROP_BREAK_MEDIUM)
	{
		SetBodygroup( PROP_STATEGROUP, PROP_BODY_PERFECT);
	}
	else if (pev->health <= PROP_BREAK_MEDIUM)
	{
		SetBodygroup( PROP_STATEGROUP, PROP_BODY_MEDIUM);
			if (m_IsFlamable = 1)
	{
//			if (iContents != CONTENTS_WATER)
//		{
//			WRITE_SHORT( g_sModelIndexFireball );
//		}
			if ( CVAR_GET_FLOAT( "cl_expdetail" ) == 2 )//create fireball
			{
				// create explosion particle system
				if ( CVAR_GET_FLOAT("r_particles" ) != 0 )			
				{
					MESSAGE_BEGIN(MSG_ALL, gmsgParticles);
						WRITE_SHORT(0);
						WRITE_BYTE(0);
						WRITE_COORD( pev->origin.x );
						WRITE_COORD( pev->origin.y );
						WRITE_COORD( pev->origin.z );
						WRITE_COORD( 0 );
						WRITE_COORD( 0 );
						WRITE_COORD( 0 );
						WRITE_SHORT(iDefaultExplosion);
					MESSAGE_END();
				}
			}
			}

	}
	else if (pev->health <= PROP_BREAK_BAD)
	{
		SetBodygroup( PROP_STATEGROUP, PROP_BODY_BAD);
	}
	
	//end
}

const char *CProp_break::pSoundsWood[] = 
{
	"debris/wood1.wav",
	"debris/wood2.wav",
	"debris/wood3.wav",
};

const char *CProp_break::pSoundsFlesh[] = 
{
	"debris/flesh1.wav",
	"debris/flesh2.wav",
	"debris/flesh3.wav",
	"debris/flesh5.wav",
	"debris/flesh6.wav",
	"debris/flesh7.wav",
};

const char *CProp_break::pSoundsMetal[] = 
{
	"debris/metal1.wav",
	"debris/metal2.wav",
	"debris/metal3.wav",
};

const char *CProp_break::pSoundsConcrete[] = 
{
	"debris/concrete1.wav",
	"debris/concrete2.wav",
	"debris/concrete3.wav",
};


const char *CProp_break::pSoundsGlass[] = 
{
	"debris/glass1.wav",
	"debris/glass2.wav",
	"debris/glass3.wav",
};

const char **CProp_break::MaterialSoundList( Materials precacheMaterial, int &soundCount )
{
	const char	**pSoundList = NULL;

    switch ( precacheMaterial ) 
	{
	case matWood:
		pSoundList = pSoundsWood;
		soundCount = ARRAYSIZE(pSoundsWood);
		break;
	case matFlesh:
		pSoundList = pSoundsFlesh;
		soundCount = ARRAYSIZE(pSoundsFlesh);
		break;
	case matComputer:
	case matUnbreakableGlass:
	case matGlass:
		pSoundList = pSoundsGlass;
		soundCount = ARRAYSIZE(pSoundsGlass);
		break;

	case matMetal:
		pSoundList = pSoundsMetal;
		soundCount = ARRAYSIZE(pSoundsMetal);
		break;

	case matCinderBlock:
	case matRocks:
		pSoundList = pSoundsConcrete;
		soundCount = ARRAYSIZE(pSoundsConcrete);
		break;
	
	
	case matCeilingTile:
	case matNone:
	default:
		soundCount = 0;
		break;
	}

	return pSoundList;
}

void CProp_break::MaterialSoundPrecache( Materials precacheMaterial )
{
	const char	**pSoundList;
	int			i, soundCount = 0;

	pSoundList = MaterialSoundList( precacheMaterial, soundCount );

	for ( i = 0; i < soundCount; i++ )
	{
		PRECACHE_SOUND( (char *)pSoundList[i] );
	}
}

void CProp_break::MaterialSoundRandom( edict_t *pEdict, Materials soundMaterial, float volume )
{
	const char	**pSoundList;
	int			soundCount = 0;

	pSoundList = MaterialSoundList( soundMaterial, soundCount );

	if ( soundCount )
		EMIT_SOUND( pEdict, CHAN_BODY, pSoundList[ RANDOM_LONG(0,soundCount-1) ], volume, 1.0 );
}


void CProp_break::Precache( void )
{
	if (pev->model)
	PRECACHE_MODEL( (char*)STRING(pev->model) );
	else
	PRECACHE_MODEL("models/props/wood_crate01.mdl");


//	PRECACHE_MODEL("models/WoodCrate_Gibs.mdl");

	const char *pGibName;

    switch (m_Material) 
	{
	case matWood:
		pGibName = "models/woodgibs.mdl";
		
		PRECACHE_SOUND("debris/bustcrate1.wav");
		PRECACHE_SOUND("debris/bustcrate2.wav");
		break;
	case matFlesh:
		pGibName = "models/fleshgibs.mdl";
		
		PRECACHE_SOUND("debris/bustflesh1.wav");
		PRECACHE_SOUND("debris/bustflesh2.wav");
		break;
	case matComputer:
		PRECACHE_SOUND("buttons/spark5.wav");
		PRECACHE_SOUND("buttons/spark6.wav");
		pGibName = "models/computergibs.mdl";
		
		PRECACHE_SOUND("debris/bustmetal1.wav");
		PRECACHE_SOUND("debris/bustmetal2.wav");
		break;

	case matUnbreakableGlass:
	case matGlass:
		pGibName = "models/glassgibs.mdl";
	//	pGibName = "models/glassSmall.mdl";
		
		PRECACHE_SOUND("debris/bustglass1.wav");
		PRECACHE_SOUND("debris/bustglass2.wav");
		break;
	case matMetal:
		pGibName = "models/metalplategibs.mdl";
		
		PRECACHE_SOUND("debris/bustmetal1.wav");
		PRECACHE_SOUND("debris/bustmetal2.wav");
		break;
	case matCinderBlock:
		pGibName = "models/cindergibs.mdl";
		
		PRECACHE_SOUND("debris/bustconcrete1.wav");
		PRECACHE_SOUND("debris/bustconcrete2.wav");
		break;
	case matRocks:
		pGibName = "models/rockgibs.mdl";
		
		PRECACHE_SOUND("debris/bustconcrete1.wav");
		PRECACHE_SOUND("debris/bustconcrete2.wav");
		break;
	case matCeilingTile:
		pGibName = "models/ceilinggibs.mdl";
		
		PRECACHE_SOUND ("debris/bustceiling.wav");  
		break;
	}
	MaterialSoundPrecache( m_Material );
	if ( m_iszGibModel )
		pGibName = STRING(m_iszGibModel);

/*	int     length;
	byte    *pMemFile;

	//uncomment if you want to try this out
//		strcat ( result, "asd");

	pMemFile = LOAD_FILE_FOR_ME(pGibName, &length);
	
	//if the file isn't present load a default one
	if ( !pMemFile )
	{
		ALERT ( at_console, "Can't find '%s', using NULL model\n", pGibName );
	
	//	PRECACHE_MODEL("models/props/error.mdl");
		m_idShard = PRECACHE_MODEL("models/props/error.mdl");
		bNoModel = TRUE;
		return;
	}
	else*/
	m_idShard = PRECACHE_MODEL( (char *)pGibName );

	// Precache the spawn item's data
	if ( m_iszSpawnObject )
		UTIL_PrecacheOther( (char *)STRING( m_iszSpawnObject ) );
}

// play shard sound when func_breakable takes damage.
// the more damage, the louder the shard sound.


void CProp_break::DamageSound( void )
{
	int pitch;
	float fvol;
	char *rgpsz[6];
	int i;
	int material = m_Material;

//	if (RANDOM_LONG(0,1))
//		return;

	if (RANDOM_LONG(0,2))
		pitch = PITCH_NORM;
	else
		pitch = 95 + RANDOM_LONG(0,34);

	fvol = RANDOM_FLOAT(0.75, 1.0);

	if (material == matComputer && RANDOM_LONG(0,1))
		material = matMetal;

	switch (material)
	{
	case matComputer:
	case matGlass:
	case matUnbreakableGlass:
		rgpsz[0] = "debris/glass1.wav";
		rgpsz[1] = "debris/glass2.wav";
		rgpsz[2] = "debris/glass3.wav";
		i = 3;
		break;

	case matWood:
		rgpsz[0] = "debris/wood1.wav";
		rgpsz[1] = "debris/wood2.wav";
		rgpsz[2] = "debris/wood3.wav";
		i = 3;
		break;

	case matMetal:
		rgpsz[0] = "debris/metal1.wav";
		rgpsz[1] = "debris/metal3.wav";
		rgpsz[2] = "debris/metal2.wav";
		i = 2;
		break;

	case matFlesh:
		rgpsz[0] = "debris/flesh1.wav";
		rgpsz[1] = "debris/flesh2.wav";
		rgpsz[2] = "debris/flesh3.wav";
		rgpsz[3] = "debris/flesh5.wav";
		rgpsz[4] = "debris/flesh6.wav";
		rgpsz[5] = "debris/flesh7.wav";
		i = 6;
		break;

	case matRocks:
	case matCinderBlock:
		rgpsz[0] = "debris/concrete1.wav";
		rgpsz[1] = "debris/concrete2.wav";
		rgpsz[2] = "debris/concrete3.wav";
		i = 3;
		break;

	case matCeilingTile:
		// UNDONE: no ceiling tile shard sound yet
		i = 0;
		break;
	}

	if (i)
		EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, rgpsz[RANDOM_LONG(0,i-1)], fvol, ATTN_NORM, 0, pitch);
}

void CProp_break::BreakTouch( CBaseEntity *pOther )
{
	float flDamage;
	entvars_t*	pevToucher = pOther->pev;
	
	// only players can break these right now
	if ( !pOther->IsPlayer() || !IsBreakable() )
	{
        return;
	}

	if ( FBitSet ( pev->spawnflags, SF_BREAK_TOUCH ) )
	{// can be broken when run into 
		flDamage = pevToucher->velocity.Length() * 0.01;

		if (flDamage >= pev->health)
		{
			SetTouch( NULL );
			TakeDamage(pevToucher, pevToucher, flDamage, DMG_CRUSH);

			// do a little damage to player if we broke glass or computer
			pOther->TakeDamage( pev, pev, flDamage/4, DMG_SLASH );
		}
	}

	if ( FBitSet ( pev->spawnflags, SF_BREAK_PRESSURE ) && pevToucher->absmin.z >= pev->maxs.z - 2 )
	{// can be broken when stood upon
		
		// play creaking sound here.
		DamageSound();

		SetThink ( Die );
		SetTouch( NULL );
		
		if ( m_flDelay == 0 )
		{// !!!BUGBUG - why doesn't zero delay work?
			m_flDelay = 0.1;
		}

		pev->nextthink = pev->ltime + m_flDelay;

	}

}


//
// Smash the our breakable object
//

// Break when triggered
void CProp_break::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( IsBreakable() )
	{
		pev->angles.y = m_angle;
		UTIL_MakeVectors(pev->angles);
		g_vecAttackDir = gpGlobals->v_forward;

		Die();
	}
}


void CProp_break::TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType )
{


	// random spark if this is a 'computer' object
	if (RANDOM_LONG(0,1) )
	{
		switch( m_Material )
		{
			case matComputer:
			{
				UTIL_Sparks( ptr->vecEndPos );

				float flVolume = RANDOM_FLOAT ( 0.7 , 1.0 );//random volume range
				switch ( RANDOM_LONG(0,1) )
				{
					case 0: EMIT_SOUND(ENT(pev), CHAN_VOICE, "buttons/spark5.wav", flVolume, ATTN_NORM);	break;
					case 1: EMIT_SOUND(ENT(pev), CHAN_VOICE, "buttons/spark6.wav", flVolume, ATTN_NORM);	break;
				}
			}
			break;
			
			case matUnbreakableGlass:
				UTIL_Ricochet( ptr->vecEndPos, RANDOM_FLOAT(0.5,1.5) );
			break;
		}
	}

	CBaseDelay::TraceAttack( pevAttacker, flDamage, vecDir, ptr, bitsDamageType );
}

float AngleBetweenVectors2( const vec3_t v1, const vec3_t v2 )
{
	float angle;
	float l1 = v1.Length();
	float l2 = v2.Length();

	if ( !l1 || !l2 )
		return 0.0f;

	angle = acos( DotProduct( v1, v2 ) / (l1*l2) );
	angle = ( angle  * 180.0f ) / M_PI;

	return angle;
}

void NormalizeAngles2( float *angles )
{
	int i;
	// Normalize angles
	for ( i = 0; i < 3; i++ )
	{
		if ( angles[i] > 180.0 )
		{
			angles[i] -= 360.0;
		}
		else if ( angles[i] < -180.0 )
		{
			angles[i] += 360.0;
		}
	}
}

//=========================================================
// Special takedamage for func_breakable. Allows us to make
// exceptions that are breakable-specific
// bitsDamageType indicates the type of damage sustained ie: DMG_CRUSH
//=========================================================
int CProp_break :: TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType )
{
/*
  if (pev->spawnflags & SF_NO_PHYS )
  {
		//no phys
  }
  else
  {
	//	if (!(pev->spawnflags & SF_SOLID_BSP))
		pev->movetype	= MOVETYPE_BOUNCE;//This should work?

		Vector			vecDir, r, anorm, rforward, rup, rright;
	//	float a;
		float force = flDamage * 10;
		TraceResult trace = UTIL_GetGlobalTrace( );
		//UTIL_MakeVectors( pev->angles );

		// grab the vector of the incoming attack. ( pretend that the inflictor is a little lower than it really is, so the body will tend to fly upward a bit).
		vecDir = r = Vector( 0, 0, 0 );
		if (!FNullEnt( pevInflictor ))
		{			
			if ( FClassnameIs( pevInflictor, "projectile" ) )
				pevInflictor = VARS( pevInflictor->owner );

			CBaseEntity *pInflictor = CBaseEntity :: Instance( pevInflictor );
			if (pInflictor)
			{	
				vecDir = g_vecAttackDir = ( trace.vecEndPos - pInflictor->Center() ).Normalize();
				r = ( trace.vecEndPos - Center() ).Normalize();
			}
		}

		anorm = UTIL_VecToAngles( r );
		anorm.x = -anorm.x;
		UTIL_MakeVectorsPrivate( anorm, rforward, rright, rup );

		float flVolume = RANDOM_FLOAT ( 0.7 , 1.0 );//random volume range
		BOOL	b_CanMakeParticles;

		if ( CVAR_GET_FLOAT( "r_paintball" ) == 0 )
		if ( CVAR_GET_FLOAT( "cl_wallpuff" ) >= 1 )
		{			
				if (pev->frags == 1) //metal
				{								
					UTIL_Ricochet( trace.vecEndPos, 0.5 );

					switch ( RANDOM_LONG(0,2) )
					{
						case 0: UTIL_EmitAmbientSound(ENT(0), trace.vecEndPos, "weapons/bulletimpact/metal1.wav", flVolume, ATTN_NORM, 0, 100); break;
						case 1: UTIL_EmitAmbientSound(ENT(0), trace.vecEndPos, "weapons/bulletimpact/metal2.wav", flVolume, ATTN_NORM, 0, 100); break;
						case 2: UTIL_EmitAmbientSound(ENT(0), trace.vecEndPos, "weapons/bulletimpact/metal3.wav", flVolume, ATTN_NORM, 0, 100); break;
					}

					if (RANDOM_LONG( 0, 99 ) < 40)
					UTIL_WhiteSparks( trace.vecEndPos, trace.vecPlaneNormal, 0, 5, 500, 500 );//chispas

					UTIL_WhiteSparks( trace.vecEndPos, trace.vecPlaneNormal, 9, 5, 5, 100 );//puntos
					UTIL_WhiteSparks( trace.vecEndPos, trace.vecPlaneNormal, 0, 5, 500, 20 );//chispas
														
					b_CanMakeParticles = FALSE;
				}
				if (pev->frags == 5) //wood
				{
					switch ( RANDOM_LONG(0,2) )
					{
						case 0: UTIL_EmitAmbientSound(ENT(0), trace.vecEndPos, "weapons/bulletimpact/wood1.wav", flVolume, ATTN_NORM, 0, 100); break;
						case 1: UTIL_EmitAmbientSound(ENT(0), trace.vecEndPos, "weapons/bulletimpact/wood2.wav", flVolume, ATTN_NORM, 0, 100); break;
						case 2: UTIL_EmitAmbientSound(ENT(0), trace.vecEndPos, "weapons/bulletimpact/wood3.wav", flVolume, ATTN_NORM, 0, 100); break;
					}
				}
				else
				{
					switch ( RANDOM_LONG(0,2) )
					{
						case 0: UTIL_EmitAmbientSound(ENT(0), trace.vecEndPos, "weapons/bulletimpact/concrete1.wav", flVolume, ATTN_NORM, 0, 100); break;
						case 1: UTIL_EmitAmbientSound(ENT(0), trace.vecEndPos, "weapons/bulletimpact/concrete2.wav", flVolume, ATTN_NORM, 0, 100); break;
						case 2: UTIL_EmitAmbientSound(ENT(0), trace.vecEndPos, "weapons/bulletimpact/concrete3.wav", flVolume, ATTN_NORM, 0, 100); break;
					}
				}	
				
				if (b_CanMakeParticles)
				{
					if ( CVAR_GET_FLOAT("r_particles" ) != 0 )			
					{
						MESSAGE_BEGIN(MSG_ALL, gmsgParticles);
							WRITE_SHORT(0);
							WRITE_BYTE(0);
							WRITE_COORD( trace.vecEndPos.x );
							WRITE_COORD( trace.vecEndPos.y );
							WRITE_COORD( trace.vecEndPos.z );
							WRITE_COORD( 0 );
							WRITE_COORD( 0 );
							WRITE_COORD( 0 );
							if (pev->frags == 5) //wood
							WRITE_SHORT(iDefaultHitWood1);
							else
							WRITE_SHORT(iDefaultWallSmoke);
						MESSAGE_END();
					}
				}
			}//eo cvar check


			if ( ( bitsDamageType & DMG_BULLET)|| ( bitsDamageType & DMG_CLUB) )
				force *= 0.5;

		pev->flags &= ~FL_ONGROUND;
		pev->origin.z += 1;

		pev->avelocity.x = cos( AngleBetweenVectors2( vecDir, rup ) ) * 100;
		pev->avelocity.y = cos( AngleBetweenVectors2( vecDir, -rright ) ) * 200;

		ALERT( at_console, "X : %3.1f %3.1f° Y: %3.1f %3.1f°\n", pev->avelocity.x, AngleBetweenVectors2( vecDir, rup ), pev->avelocity.y, AngleBetweenVectors2( vecDir, -rright ) );


		pev->velocity = pev->velocity + gpGlobals->v_up * force * RANDOM_FLOAT( 0, 0.5 ) + vecDir * force * RANDOM_FLOAT( 0.5, 1.0 );
	}*/






	Vector	vecTemp;

	// if Attacker == Inflictor, the attack was a melee or other instant-hit attack.
	// (that is, no actual entity projectile was involved in the attack so use the shooter's origin). 
	if ( pevAttacker == pevInflictor )	
	{
		vecTemp = pevInflictor->origin - ( pev->absmin + ( pev->size * 0.5 ) );
				
		// if a client hit the breakable with a crowbar, and breakable is crowbar-sensitive, break it now.
			if ( FBitSet ( pevAttacker->flags, FL_CLIENT ) &&
					 FBitSet ( pev->spawnflags, SF_BREAK_CROWBAR ) && (bitsDamageType & DMG_CLUB))
				flDamage = pev->health;
	}
	else
	// an actual missile was involved.
	{
		vecTemp = pevInflictor->origin - ( pev->absmin + ( pev->size * 0.5 ) );
	}
	
	if (!IsBreakable())
		return 0;

	// si el breakeable tiene el flag de 'solo semtex' y es dañado por uno, romperlo
	//fix: el atacante es el cliente o el semtex mismo? sacarlo por las dudas
	if ( FBitSet ( pev->spawnflags, SF_BREAK_BYSEMTEX ))
	{
		ALERT( at_console, "SF_BREAK_BYSEMTEX spawnflag detected in -func_break-\n" );

		if ((bitsDamageType & DMG_CRUSH))
		{
			ALERT( at_console, "-func_break- killed by semtex!\n" );
			flDamage = pev->health;	
		}
		else
		{
			ALERT( at_console, "-func_break- can't be damaged!\n" );
			flDamage = 0;	
		}
	}
	else
	{
	//	ALERT( at_console, "-func_break- is not damaged by semtex!\n" );
//		ALERT( at_console, "-func_break- is damaged!\n" );
	}

	// Breakables take double damage from the crowbar
	if ( bitsDamageType & DMG_CLUB )
		flDamage *= 2;

	// Boxes / glass / etc. don't take much poison damage, just the impact of the dart - consider that 10%
	if ( bitsDamageType & DMG_POISON )
		flDamage *= 0.1;

// this global is still used for glass and other non-monster killables, along with decals.
	g_vecAttackDir = vecTemp.Normalize();
		
// do the damage
	pev->health -= flDamage;

	if (pev->health <= 0)
	{
		//movido a Die() func
	
		Killed( pevAttacker, GIB_NORMAL );
/*
		CSprite *pSprite = CSprite::SpriteCreate( "sprites/exp_end.spr", pev->origin, TRUE );
		pSprite->AnimateAndDie( RANDOM_FLOAT( 8.0, 12.0 ) );//framerate
		pSprite->SetTransparency( kRenderTransAlpha, 50, 50, 50, 255, kRenderFxNone );
		pSprite->SetScale( 5.0 );
		pSprite->Expand( RANDOM_FLOAT( 10, 15 ), RANDOM_FLOAT( 18.0, 26.0 )  );//expand
		pSprite->pev->frame = 0;
*/
	//	SetThink ( Die );		
	//	pev->nextthink = gpGlobals->time + pev->speed;

	//	pev->takedamage = DAMAGE_NO;
	//sys test
		Die();
		return 0;
	}

	// Make a shard noise each time func breakable is hit.
	// Don't play shard noise if cbreakable actually died.

	DamageSound();

	return 1;
}


void CProp_break::Die( void )
{	
//	Killed( pevAttacker, GIB_NORMAL );

	//esto es lo mismo q la funcion Killed()

	//movido a TakeDamage() func, para decir que ya no puede recibir mas daño
//	pev->takedamage = DAMAGE_NO;

//	pev->deadflag = DEAD_DEAD;
//	UTIL_Remove( this );

	Vector vecSpot;// shard origin
	Vector vecVelocity;// shard velocity
	CBaseEntity *pEntity = NULL;
	char cFlag = 0;
	int pitch;
	float fvol;
	
	pitch = 95 + RANDOM_LONG(0,29);

	if (pitch > 97 && pitch < 103)
		pitch = 100;

	// The more negative pev->health, the louder
	// the sound should be.

	fvol = RANDOM_FLOAT(0.85, 1.0) + (abs(pev->health) / 100.0);

	if (fvol > 1.0)
		fvol = 1.0;

	int iAmount = pev->size.x + pev->size.y + pev->size.z;
	int n;

	switch (m_Material)
	{
	case matGlass:
		switch ( RANDOM_LONG(0,1) )
		{
		case 0:	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "debris/bustglass1.wav", fvol, ATTN_NORM, 0, pitch);	
			break;
		case 1:	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "debris/bustglass2.wav", fvol, ATTN_NORM, 0, pitch);	
			break;
		}
		cFlag = BREAK_GLASS;

		//fill it up
		for ( n = 0; n <= (iAmount *0.05); n++ )
		{
			ALERT(at_console, "iAmount = %i\n", iAmount );

			MESSAGE_BEGIN(MSG_ALL, gmsgParticles);
				WRITE_SHORT(0);
				WRITE_BYTE(0);
				WRITE_COORD( Center().x + RANDOM_FLOAT( -pev->size.x /2, pev->size.x /2) );
				WRITE_COORD( Center().y + RANDOM_FLOAT( -pev->size.y /2, pev->size.y /2) );
				WRITE_COORD( Center().z + RANDOM_FLOAT( -pev->size.z /2, pev->size.z /2) );
				WRITE_COORD( 0 );
				WRITE_COORD( 0 );
				WRITE_COORD( 0 );
				WRITE_SHORT(iDefaultGlass);
			MESSAGE_END();
		}
		break;

	case matWood:
		switch ( RANDOM_LONG(0,1) )
		{
		case 0:	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "debris/bustcrate1.wav", fvol, ATTN_NORM, 0, pitch);	
			break;
		case 1:	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "debris/bustcrate2.wav", fvol, ATTN_NORM, 0, pitch);	
			break;
		}
		cFlag = BREAK_WOOD;
		break;

	case matComputer:
	case matMetal:
		switch ( RANDOM_LONG(0,1) )
		{
		case 0:	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "debris/bustmetal1.wav", fvol, ATTN_NORM, 0, pitch);	
			break;
		case 1:	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "debris/bustmetal2.wav", fvol, ATTN_NORM, 0, pitch);	
			break;
		}
		cFlag = BREAK_METAL;
		break;

	case matFlesh:
		switch ( RANDOM_LONG(0,1) )
		{
		case 0:	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "debris/bustflesh1.wav", fvol, ATTN_NORM, 0, pitch);	
			break;
		case 1:	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "debris/bustflesh2.wav", fvol, ATTN_NORM, 0, pitch);	
			break;
		}
		cFlag = BREAK_FLESH;
		break;

	case matRocks:
	case matCinderBlock:
		switch ( RANDOM_LONG(0,1) )
		{
		case 0:	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "debris/bustconcrete1.wav", fvol, ATTN_NORM, 0, pitch);	
			break;
		case 1:	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "debris/bustconcrete2.wav", fvol, ATTN_NORM, 0, pitch);	
			break;
		}
		cFlag = BREAK_CONCRETE;
		break;

	case matCeilingTile:
		EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "debris/bustceiling.wav", fvol, ATTN_NORM, 0, pitch);
		break;
	}
    
		
	if (m_Explosion == expDirected)
		vecVelocity = g_vecAttackDir * 200;
	else
	{
		vecVelocity.x = 0;
		vecVelocity.y = 0;
		vecVelocity.z = 0;
	}

	vecSpot = pev->origin + (pev->mins + pev->maxs) * 0.5;
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSpot );
		WRITE_BYTE( TE_BREAKMODEL);

		// position
		WRITE_COORD( vecSpot.x );
		WRITE_COORD( vecSpot.y );
		WRITE_COORD( vecSpot.z );

		// size
		WRITE_COORD( pev->size.x);
		WRITE_COORD( pev->size.y);
		WRITE_COORD( pev->size.z);

		// velocity
		WRITE_COORD( vecVelocity.x ); 
		WRITE_COORD( vecVelocity.y );
		WRITE_COORD( vecVelocity.z );

		// randomization
		WRITE_BYTE( 10 ); 

		// Model
		WRITE_SHORT( m_idShard );	//model id#

		// # of shards
		WRITE_BYTE( 0 );	// let client decide

		// duration
		WRITE_BYTE( 25 );// 2.5 seconds

		// flags
		WRITE_BYTE( cFlag );
	MESSAGE_END();

	float size = pev->size.x;
	if ( size < pev->size.y )
		size = pev->size.y;
	if ( size < pev->size.z )
		size = pev->size.z;

	// !!! HACK  This should work!
	// Build a box above the entity that looks like an 8 pixel high sheet
	Vector mins = pev->absmin;
	Vector maxs = pev->absmax;
	mins.z = pev->absmax.z;
	maxs.z += 8;

	// BUGBUG -- can only find 256 entities on a breakable -- should be enough
	CBaseEntity *pList[256];
	int count = UTIL_EntitiesInBox( pList, 256, mins, maxs, FL_ONGROUND );
	if ( count )
	{
		for ( int i = 0; i < count; i++ )
		{
			ClearBits( pList[i]->pev->flags, FL_ONGROUND );
			pList[i]->pev->groundentity = NULL;
		}
	}

	// Don't fire something that could fire myself
	pev->targetname = 0;
	pev->solid = SOLID_NOT;

	// Fire targets on break
	SUB_UseTargets( NULL, USE_TOGGLE, 0 );

	SetThink( SUB_Remove );
	pev->nextthink = pev->ltime + 0.1;

	if ( m_iszSpawnObject )
		CBaseEntity::Create( (char *)STRING(m_iszSpawnObject), VecBModelOrigin(pev), pev->angles, edict() );

/*
	CBaseEntity *pEntidad = NULL;

//	while ((pEntidad = UTIL_FindEntityByClassname(pEntidad, "env_model")) != NULL) 
		   
	while ((pEntidad = UTIL_FindEntityInSphere( pEntidad, pev->origin, 48 )) != NULL)//512
	{	
	//	float flDist = (pEntidad->Center() - pev->origin).Length();

		if ( FClassnameIs(pEntidad->pev, "env_model"))
		{
			ALERT ( at_notice, "env_model\n");

//			if (flDist <= 128)
//			{
				ALERT ( at_notice, "flDist <= 128\n");

				//if (pEntidad->pev->impacttime != 0.0)
			//{
					ALERT ( at_notice, "TakeDamage()\n");
				//	UTIL_Remove ( pEntidad );
							
					UTIL_Remove( pEntidad );
			//	}
//			}
//			else
//				ALERT ( at_notice, "flDist ERROR\n");
		}
	}
*/
	if ( Explodable() )
	{
		ExplosionCreate( Center(), pev->angles, edict(), ExplosionMagnitude(), TRUE );
	}
}



BOOL CProp_break :: IsBreakable( void ) 
{ 
	return m_Material != matUnbreakableGlass;
}

/*
int	CProp_break :: DamageDecal( int bitsDamageType )
{
	if ( m_Material == matGlass  )
		return DECAL_GLASSBREAK1 + RANDOM_LONG(0,2);

	if ( m_Material == matUnbreakableGlass )
		return DECAL_BPROOF1;

	return CBaseEntity::DamageDecal( bitsDamageType );
}
*/
BOOL CProp_break :: VisionCheck2( void )
{											
	CBaseEntity *pLooker;
	if (pev->netname)
	{
		pLooker = UTIL_FindEntityByTargetname(NULL, STRING(pev->netname));
		while (pLooker!=NULL)
		{
			if (!(pev->spawnflags & SF_ONSIGHT_STATECHECK) || (pev->spawnflags & SF_ONSIGHT_STATECHECK && pLooker->GetState()!=STATE_OFF)){

				CBaseEntity *pSeen;
				if (pev->message)
				{
					pSeen = UTIL_FindEntityByTargetname(NULL, STRING(pev->message));
					if (!pSeen)
					{
						// must be a classname.
						pSeen = UTIL_FindEntityByClassname(pSeen, STRING(pev->message));

						while (pSeen != NULL)
						{
							if (CanSee2(pLooker, pSeen))
								return TRUE;
							pSeen = UTIL_FindEntityByClassname(pSeen, STRING(pev->message));
						}
					}
					else
					{
						while (pSeen != NULL){
							if (CanSee2(pLooker, pSeen))
								return TRUE;
							pSeen = UTIL_FindEntityByTargetname(pSeen, STRING(pev->message));
						}
					}
				}
				else{
					if (CanSee2(pLooker, this))
						return TRUE;
				}
			}
			pLooker = UTIL_FindEntityByTargetname(pLooker, STRING(pev->netname));
		}
		return FALSE;
	}
	else
	{
		pLooker = UTIL_FindEntityByClassname(NULL, "player");
		if (!pLooker)
		{
			return FALSE;
		}
		CBaseEntity *pSeen;
		if (pev->message)
			pSeen = UTIL_FindEntityByTargetname(NULL, STRING(pev->message));
		else
			return CanSee2(pLooker, this);

		if (!pSeen){
		// must be a classname.
			pSeen = UTIL_FindEntityByClassname(pSeen, STRING(pev->message));
			while (pSeen != NULL){
				if (CanSee2(pLooker, pSeen))
					return TRUE;
				pSeen = UTIL_FindEntityByClassname(pSeen, STRING(pev->message));
			}
			return FALSE;
		}
		else{
			while (pSeen != NULL){
				if (CanSee2(pLooker, pSeen))
					return TRUE;
				pSeen = UTIL_FindEntityByTargetname(pSeen, STRING(pev->message));
			}
			return FALSE;
		}
	}
}
BOOL CProp_break :: CanSee2(CBaseEntity *pLooker, CBaseEntity *pSeen)
{
	if (pev->frags && (pLooker->pev->origin - pSeen->pev->origin).Length() > pev->frags)
		return FALSE;

	if (pev->max_health < 360)
	{
		Vector2D	vec2LOS;
		float	flDot;
		float flComp = pev->health;
		UTIL_MakeVectors ( pLooker->pev->angles );
		vec2LOS = ( pSeen->pev->origin - pLooker->pev->origin ).Make2D();
		vec2LOS = vec2LOS.Normalize();
		flDot = DotProduct (vec2LOS , gpGlobals->v_forward.Make2D() );
		if ( pev->max_health == -1 )
		{
			CBaseMonster *pMonst = pLooker->MyMonsterPointer();
			if (pMonst)
				flComp = pMonst->m_flFieldOfView;
			else
				return FALSE; // not a monster, can't use M-M-M-MonsterVision
		}
		if (flDot <= flComp)
			return FALSE;
	}

	//si no tiene NO LINE OF SIGHT, entonces tiene

	/*
	TraceResult tr;
	UTIL_TraceLine( pLooker->EyePosition(), pSeen->pev->origin, ignore_monsters, dont_ignore_glass, pLooker->edict(), &tr );
	
	if (tr.flFraction < 1.0 && tr.pHit != pSeen->edict())
		return FALSE;
	*/

	return TRUE;
}


void CProp_break :: Touch( CBaseEntity *pOther )
{
	/*
	Vector savedangles = Vector( 0, 0, 0 );
	int negate = 0;
	TraceResult tr;
	// look down directly to know the surface we're lying.
	UTIL_TraceLine( pev->origin, pev->origin - Vector(0,0,64), ignore_monsters, edict(), &tr );

	//ALERT( at_debug, "Velocity Touch : %3.2f\n", pev->velocity.x );
	pev->velocity = pev->velocity * 0.8;
//	pev->avelocity = pev->avelocity * 0.5;

	if( !(pev->flags & FL_ONGROUND) )
	{
		//pev->avelocity.x = RANDOM_FLOAT( -400, 400 );//fooz
		if (pev->frags == 0)
		{
			//no sound
		}

		if (pev->frags == 1) //metal
		{
			if( RANDOM_LONG( 0, 1 ))
				EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "fisica/metal/impact1.wav", 1, ATTN_NORM, 0, PITCH_NORM);
			else
				EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "fisica/metal/impact2.wav", 1, ATTN_NORM, 0, PITCH_NORM);
		}

		if (pev->frags == 2) //plaster
		{			
			switch (RANDOM_LONG(0,2)) 
			{
				case 0:	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "fisica/plastico/impact1.wav", 1, ATTN_NORM, 0, PITCH_NORM); break;
				case 1:	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "fisica/plastico/impact2.wav", 1, ATTN_NORM, 0, PITCH_NORM); break;
				case 2:	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "fisica/plastico/impact3.wav", 1, ATTN_NORM, 0, PITCH_NORM); break;
			}			
		}
		
		if (pev->frags == 3) //glass
		{
			if( RANDOM_LONG( 0, 1 ))
				EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "debris/glass3.wav", 1, ATTN_NORM, 0, PITCH_NORM);
			else
				EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "debris/glass1.wav", 1, ATTN_NORM, 0, PITCH_NORM);
		}

		if (pev->frags == 4) //concrete
		{
			if( RANDOM_LONG( 0, 1 ))
				EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "debris/concrete1.wav", 1, ATTN_NORM, 0, PITCH_NORM);
			else
				EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "debris/concrete3.wav", 1, ATTN_NORM, 0, PITCH_NORM);
		}

		if (pev->frags == 5) //wood
		{
			switch (RANDOM_LONG(0,1)) 
			{
				case 0:	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "fisica/madera/impact1.wav", 1, ATTN_NORM, 0, PITCH_NORM); break;
				case 1:	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "fisica/madera/impact2.wav", 1, ATTN_NORM, 0, PITCH_NORM); break;
			}
		}
	}
	else
	{	
		for( int i = 0; i<3; i++ )
		{
		if( pev->angles.x < 0 )
			negate = 1;

			if( fabs(pev->angles.x) < 45 )
				savedangles.x = 0;
			else if( fabs(pev->angles.x) >= 45 && fabs(pev->angles.x) <= 135 )
				savedangles.x = 90;
			else if( fabs(pev->angles.x) > 135 && fabs(pev->angles.x) <= 180 )
				savedangles.x = 180;
		}

		#ifndef M_PI
		#define M_PI 3.14159265358979
		#endif
		#define ang2rad (2 * M_PI / 360)

          if ( tr.flFraction < 1.0 )
          {
			Vector forward, right, angdir, angdiry;
			Vector Angles = pev->angles;

			//Fooz
			NormalizeAngles2( Angles );
		
			UTIL_MakeVectorsPrivate( Angles, forward, right, NULL );
			angdir = forward;
			Vector left = -right;
			angdiry = left;

			pev->angles.x = -UTIL_VecToAngles( angdir - DotProduct(angdir, tr.vecPlaneNormal) * tr.vecPlaneNormal).x;
			pev->angles.y = UTIL_VecToAngles( angdir - DotProduct(angdir, tr.vecPlaneNormal) * tr.vecPlaneNormal).y;

		//	pev->angles.z = UTIL_VecToAngles( angdir - DotProduct(angdir, tr.vecPlaneNormal) * tr.vecPlaneNormal).z;
		
		//	pev->angles.z = UTIL_VecToAngles( angdiry - DotProduct(angdiry, tr.vecPlaneNormal) * tr.vecPlaneNormal).x;

			pev->angles.z = UTIL_VecToAngles( angdiry - DotProduct(angdiry, tr.vecPlaneNormal) * right ).x;
					
		//	NormalizeAngles2( Angles );
          }

		#undef ang2rad
		
		  if( negate )
			pev->angles.x -= savedangles.x;
		  else
			pev->angles.x += savedangles.x;
	}


	if ( FClassnameIs( pOther->pev, "worldspawn" ) )
		return;*/

//	Move( pOther, 1 );
}
