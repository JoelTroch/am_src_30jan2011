//=======================================================================
//			Copyright (C) Shambler Team 2005
//		         	  physic.cpp - phyiscs header
//			       for newton engine			    
//=======================================================================

#ifndef PHYSICS_H
#define PHYSICS_H

#include "com_model.h"
#include "pm_defs.h"
#include "matrix.h"

#define GFORCE -9.8f		//gravity

static NewtonWorld* nWorld;
static NewtonBody* BSPWorld; 
static VMatrix matrix_world;

const dFloat TICKS2SEC = 1.0e-3f;
void* PhysicsAlloc (int sizeInBytes);
void  PhysicsFree (void *ptr, int sizeInBytes);


//global physics state
#define PHYS_NOINIT	0 
#define PHYS_SET	1
#define PHYS_INIT	2


static unsigned m_prevTime;
static int bPhysInit = 0;

//external functions
void ClosePhysicEngine ( void );
void InitPhysicEngine ( void );
void UpdatePhysic (void );

#endif //PHYSICS_H