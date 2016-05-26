class CFuncKeypad : public CBaseToggle
{
public:

	void Spawn			( void );

	virtual void KeyValue	( KeyValueData *pkvd );
	virtual void Use		( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	void EXPORT BThink	( void );

	virtual int  ObjectCaps	( void )
	{ 
		return (CBaseToggle :: ObjectCaps() | FCAP_IMPULSE_USE/* FCAP_CONTINUOUS_USE*/) & ~FCAP_ACROSS_TRANSITION;
	}

	BOOL		m_bNeedsUpdate;

	string_t	s_code;
	string_t	s_frame_label;
	string_t	s_my_target;
	string_t	s_my_failed_target;
	BOOL		b_hide_pass;

	void FireTarget			( void );
	void WrongCode			( void );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );

	static	TYPEDESCRIPTION m_SaveData[];
};
