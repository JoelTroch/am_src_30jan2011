//========= Copyright © 2004-2008, Raven City Team, All rights reserved. ============//
//																					 //
// Purpose:																			 //
//																					 //
// $NoKeywords: $																	 //
//===================================================================================//

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"

//===============================================
// Lens Flare
//===============================================
extern int gmsgLensFlare;

class CLensFlare : public CPointEntity
{
public:
	void Spawn( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
};
LINK_ENTITY_TO_CLASS( env_lensflare, CLensFlare );

void CLensFlare::Spawn( void )
{
	pev->solid = SOLID_NOT;	// Remove model & collision
	pev->effects |= EF_NODRAW;
}
void CLensFlare::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	MESSAGE_BEGIN( MSG_ALL, gmsgLensFlare, pev->origin );
	WRITE_ANGLE( pev->angles.x );
	WRITE_ANGLE( pev->angles.y );
	MESSAGE_END();
}