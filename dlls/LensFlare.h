
class CLensFlare : public CBaseEntity {

public:

void Spawn( void );
void Precache( void );
void KeyValue( KeyValueData* );

void EXPORT Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

vec3_t angles;
};