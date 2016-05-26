//========= Copyright © 2004-2008, Raven City Team, All rights reserved. ============//
//																					 //
// Purpose:																			 //
//																					 //
// $NoKeywords: $																	 //
//===================================================================================//

#include "particle_defs.h"

#define SF_GRASS_STARTON 1
#define SF_GRASS_NO_PVS_CHECK 2
class CFuncGrass : public CPointEntity
{
	unsigned int iID;
public:
	void Spawn( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void EXPORT	GrassThink( void );
	void EXPORT	GrassTurnOn( void );
	void EXPORT	GrassThinkContinous( void );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	static int	ms_iNextFreeKey;

	int		m_iIDNumber;
	BOOL	m_fGrassActive;
	BOOL	m_fGrassNoUpdate;
	BOOL	m_fGrassDeactivatedByPVS;
	BOOL	m_fGrassDeactivated;
};