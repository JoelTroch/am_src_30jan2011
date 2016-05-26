class CFuncOS : public CBaseToggle
{
public:

	void Spawn			( void );

	virtual void KeyValue	( KeyValueData *pkvd );
	virtual void Use		( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	void EXPORT BThink	( void );

	virtual int  ObjectCaps	( void )
	{ 
		return (CBaseToggle :: ObjectCaps() | FCAP_IMPULSE_USE) & ~FCAP_ACROSS_TRANSITION;
	}

	BOOL		m_bNeedsUpdate;
	BOOL		m_bLogged;

	string_t	m_s_pc_name;

	string_t	m_szUsername;
	string_t	m_szPassword;

	void FireTarget			( void );
	void Exit			( void );
	void Logged			( void );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );

	static	TYPEDESCRIPTION m_SaveData[];
};
