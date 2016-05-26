//=======================================================================
//			Copyright (C) Shambler Team 2005
//		         	  physic.cpp - phyiscs wrapper
//			       for newton engine			    
//=======================================================================
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "saverestore.h"
#include "physics.h"

extern playermove_t *pmove = NULL;

extern "C" long _ftol( double ); //defined by VC6 C libs
extern "C" long _ftol2( double dblSource ) { return _ftol( dblSource ); }

// memory allocation for Newton
void* PhysicsAlloc (int sizeInBytes) { return malloc (sizeInBytes); }
void  PhysicsFree (void *ptr, int sizeInBytes) { free (ptr); }

void PhysicsApplyForceAndTorque (const NewtonBody* body);


class CBasePhysics : public CBaseEntity
{
public:
	void Spawn( void );
	void Precache( void );
	void KeyValue( KeyValueData *pkvd );
	void SetupPhysic( void );
	void UpdateEntity( float timeStep );
	static CBasePhysics *Instance( edict_t *pent )
	{
		if ( !pent )pent = ENT(0);
		CBasePhysics *pEnt = (CBasePhysics *)GET_PRIVATE(pent);
		return pEnt;
	}
	virtual BOOL IsPhysics( void ) { return pev->flags & FL_PHYSICS ? TRUE : FALSE; }
	virtual void SetPhysic( int set = 0 )
	{
		if(set) pev->flags |= FL_PHYSICS;
		else pev->flags &= ~FL_PHYSICS;
	}
	float m_flMass;
private:
	//physics use this
	NewtonBody* boxBody;
	NewtonCollision* collision;
	VMatrix matrix;
	Vector omega;
	Vector force;
};
LINK_ENTITY_TO_CLASS( phys_model, CBasePhysics );
LINK_ENTITY_TO_CLASS( phys_brush, CBasePhysics );

//==============================================================================
//			newton engine init operations
//==============================================================================
void InitPhysicEngine ( void )	//called from client.cpp->ServerActivate( void )
{
	//FIXME : Move to CalcWorldCollision
	m_prevTime = 0;//reset timer
}

void ClosePhysicEngine ( void )	//called from client.cpp->ServerDeactivate( void )
{
	//close physics engine
	if(bPhysInit == PHYS_INIT)
	{
		bPhysInit = PHYS_NOINIT;
		NewtonDestroy (nWorld);
	}
}

//==============================================================================
//			newton engine calc world geometry
//==============================================================================

void levelCallback (const NewtonBody* bodyWithTreeCollision, const NewtonBody* body, const dFloat* vertex, int vertexstrideInBytes, int indexCount, const int* indexArray)
{
}

void CalcWorldCollision ( void )
{
    	float	*v;
    	glpoly_t	*p;
	Vector vert[32];
	NewtonCollision* collision;
	
	//create world
	nWorld = NewtonCreate (PhysicsAlloc, PhysicsFree);
	NewtonSetSolverModel( nWorld, 0 );
	NewtonSetFrictionModel( nWorld, 0 );
		
	// create the collsion tree geometry
	collision = NewtonCreateTreeCollision(nWorld, levelCallback);
	NewtonTreeCollisionBeginBuild(collision);

	if (pmove && pmove->numphysent > 0)
	{
		model_t *world = (model_t*)pmove->physents[0].model;
		if (world)
		{
			for (int i = 0; i < world->numsurfaces; i++) 
                              {
				p = world->surfaces[i].polys;
				v = p->verts[0];
                                        
                                        ALERT(at_console, "NumVerts %d Poly Num %d\n", p->numverts, i );
                                       	for (int d = 0 ; d < p->numverts ; d++, v+= VERTEXSIZE)
                                       	{
                                       		vert[d] = v;
                                       		//ALERT(at_console, "vertex %.f %.f %.f\n", vert[d].x, vert[d].y, vert[d].z);
                                       	}
				// add this face to the collsion tree
				NewtonTreeCollisionAddFace(collision, p->numverts, &vert[0].x, sizeof(Vector), 1);
			}
		}
	}

	// finalize the collition tree build
	NewtonTreeCollisionEndBuild(collision, 1);

	// create the level ridif body
	BSPWorld = NewtonCreateBody(nWorld, collision);

	// release the collision tree (this way the application does not have to do book keeping of Newton objects
	NewtonReleaseCollision (nWorld, collision);

	// set the global position of this body
	NewtonBodySetMatrix (BSPWorld, &matrix_world[0][0]); 

	Vector boxP0, boxP1; 
	// get the position of the aabb of this geometry
	NewtonCollisionCalculateAABB (collision, &matrix_world[0][0], &boxP0.x, &boxP1.x); 

	// add some extra padding the world size
	boxP0.x -= 10.0f;
	boxP0.y -= 10.0f;
	boxP0.z -= 10.0f;
	boxP1.x += 10.0f;
	boxP1.y += 10.0f;
	boxP1.z += 10.0f;
	NewtonSetWorldSize (nWorld, &boxP0.x, &boxP1.x); 
}

void UpdatePhysic (void )		//called from client.cpp->StartFrame( void )
{
	edict_t		*pEdict = g_engfuncs.pfnPEntityOfEntIndex( 1 );
	CBasePhysics	*pEntity;

	CBaseEntity *pPhysEnt;
	pPhysEnt = UTIL_FindEntityByClassname(NULL, "phys_brush");

	if (!pPhysEnt)
	{
		ALERT(at_error, " can't find phys_brush!?\n");
		return;
	}

	if(bPhysInit == PHYS_NOINIT) 
	{
		//set physics for all phys entities
		if(!pEdict) return;
		for ( int e = 1; e < gpGlobals->maxEntities; e++, pEdict++ )
		{
			if ( pEdict->free )continue;
		
			//if ( pEdict->v.flags & FL_PHYSICS ) //we found child
			if (pPhysEnt) 
			{
				if(bPhysInit != PHYS_SET)
				{
					CalcWorldCollision();
					bPhysInit = PHYS_SET;//yes phys entity present on level
				}
				//pEntity = CBasePhysics::Instance(pEdict);
				pEntity = CBasePhysics::Instance(pPhysEnt->edict());
				pEntity->SetupPhysic();
				
			}
		}
                    
		if(bPhysInit == PHYS_SET)
		{
			MessageBox(NULL, "Newton sucessfully initialized", "", MB_OK);
			bPhysInit = PHYS_INIT;//newoton sucessfully initialized
			return;//physics inintialized ?
		}
	}

	if(bPhysInit != PHYS_INIT) 
		return; //don't execute code

	dFloat timeStep;
	unsigned miliseconds;

	if ( !pEdict ) return;//client is present ?
	
	//get current time
	miliseconds = gpGlobals->time;
	timeStep = dFloat (miliseconds - m_prevTime) * TICKS2SEC;
	m_prevTime = miliseconds;
	
	if (timeStep > 0.1f) timeStep = 0.1f;
	if (timeStep < 0.005f) timeStep = 0.005f;
	
	NewtonUpdate (nWorld, gpGlobals->time);
	ALERT(at_console, "NewtonUpdate %f\n", gpGlobals->time);

	//update all entities
	for ( int i = 1; i < gpGlobals->maxEntities; i++, pEdict++ )
	{
		if ( pEdict->free )	continue;
		
		//if ( pEdict->v.flags & FL_PHYSICS ) //we found child
		if (pPhysEnt) 
		{
			//pEntity = CBasePhysics::Instance(pEdict);
			pEntity = CBasePhysics::Instance(pPhysEnt->edict());
			pEntity->UpdateEntity( timeStep );
		}
	}
}

void SetUpdate (const NewtonBody* body, const dFloat* matrix)
{
/*	edict_t		*pEdict = g_engfuncs.pfnPEntityOfEntIndex( 1 );
	CBasePhysics	*pEntity;
	
	for ( int i = 1; i < gpGlobals->maxEntities; i++, pEdict++ )
	{
		if ( pEdict->free )	continue;
		
		if ( pEdict->v.flags & FL_PHYSICS ) //we found child
		{
			pEntity = CBasePhysics::Instance(pEdict);
			pEntity->UpdateEntity( body, matrix );
		}
	}
*/	
	ALERT(at_console, "Update Physics\n");
}

//==============================================================================
//			physic_entity
//==============================================================================
void CBasePhysics :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "mass"))
	{
		m_flMass = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "omega"))
	{
		UTIL_StringToVector( omega, pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "force"))
	{
		UTIL_StringToVector( force, pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else CBaseEntity::KeyValue( pkvd );
}

void CBasePhysics :: Spawn( void )
{
	Precache();

//	SetPhysic( TRUE );
	pev->flags |= FL_PHYSICS;

	pev->renderfx 	= kRenderFxEntInPVS;//for debug

	if (FClassnameIs(pev, "phys_brush")) 
	{/*
		pev->movetype	= MOVETYPE_PUSH;
		pev->solid	= SOLID_BSP;*/
				pev->solid	= SOLID_SLIDEBOX;
		pev->movetype	= MOVETYPE_NONE;

		SET_MODEL( ENT(pev), STRING(pev->model) );
	}
	else 
	{/*
		pev->solid	= SOLID_SLIDEBOX;
		pev->movetype	= MOVETYPE_NONE;
		UTIL_SetModel( ENT(pev), "models/cube.mdl" );
          	UTIL_AutoSetSize();*/
          }
	
	UTIL_SetOrigin(pev, pev->origin );
}

void CBasePhysics :: Precache( void )
{
//	if (!FClassnameIs(pev, "phys_brush")) UTIL_PrecacheModel( "models/cube.mdl" );
}

void CBasePhysics :: SetupPhysic( void )
{
	if(!pev->scale) pev->scale = 1;//reset scale factor
	else
	{
		m_flMass = m_flMass * pev->scale;
		pev->size = pev->size * pev->scale;
	}
    
	if (!FClassnameIs(pev, "phys_brush")) 
		return;
	ALERT(at_console, "pev->size %.f %.f %.f\n", pev->size.x, pev->size.y, pev->size.z );
	collision = NewtonCreateBox (nWorld, pev->size.x, pev->size.y, pev->size.z, NULL); 

	//create the physic body
	boxBody = NewtonCreateBody (nWorld, collision);

	//get angles and position
	matrix.LoadIdentity();
          matrix.SetTransform( pev->origin, pev->angles );

	//set velocity & angular velocity
	NewtonBodySetOmega (boxBody, omega );
          NewtonBodySetVelocity (boxBody, force );
          
	//NewtonBodySetTransformCallback (boxBody, SetUpdate);
	NewtonBodySetForceAndTorqueCallback (boxBody, PhysicsApplyForceAndTorque);

	// set the mass matrix
	NewtonBodySetMassMatrix (boxBody, 1.0f, 0, 0, 0 );
	NewtonBodySetMatrix (boxBody, &matrix[0][0]);
}

void PhysicsApplyForceAndTorque (const NewtonBody* body)
{
	//m_flMass
	dFloat mass;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;

	NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz);
	Vector force (0.0f, 0.0f, 0.0f);//kg2lbs(mass) * -9.8f);
	NewtonBodySetForce (body, &force.x);
}

void CBasePhysics :: UpdateEntity( float timeStep )
{
	//convert matrix
	VMatrix mat;
	NewtonBodyGetMatrix(boxBody, &mat[0][0]);
	mat.GetTransform(pev->origin, pev->angles);
}
