//********************************************************************
// Newton Game dynamics 
// copyright 2000
// By Julio Jerez
// VC: 6.0
// simple 4d matrix class
//********************************************************************

#ifndef MATRIX_H
#define MATRIX_H

// ------------------------------------------------------------------------------------
// UNITS:
// ------------------------------------------------------------------------------------
// NOTE:  Coordinates are in HL units.  1 unit == 1 inch.  X is east (forward), Y is north (left), Z is up (up)
// QAngle are pitch (around y), Yaw (around Z), Roll (around X)
// AngularImpulse are exponetial maps (an axis in HL units scaled by a "twist" angle in degrees)
//		They can be transformed like normals/covectors and added linearly
// mass is kg, volume is in^3, acceleration is in/s^2, velocity is in/s

// density is kg/m^3 (water ~= 998)
// preferably, these would be in kg/in^3, but the range of those numbers makes them not very human readable
// having water be about 1000 is really convenient for data entry.
// Since volume is in in^3 and density is in kg/m^3: 
//	density = (mass / volume) * CUBIC_METERS_PER_CUBIC_INCH
// Force is applied using impulses (kg*in/s)
// Torque is applied using impulses (kg*degrees/s)
// ------------------------------------------------------------------------------------

#define METERS_PER_INCH		(0.0254f)
#define CUBIC_METERS_PER_CUBIC_INCH	(METERS_PER_INCH*METERS_PER_INCH*METERS_PER_INCH)
// 2.2 lbs / kg
#define POUNDS_PER_KG		(2.2f)
#define KG_PER_POUND		(1.0f/POUNDS_PER_KG)

// convert from pounds to kg
#define lbs2kg(x)			((x)*KG_PER_POUND)
#define kg2lbs(x)			((x)*POUNDS_PER_KG)

// Convert our units (inches) to IVP units (meters)
#define HL2NWT_FACTOR	METERS_PER_INCH
#define INCHES(x)		(float)(x * (1.0f/HL2NWT_FACTOR))
#define METERS(x)		(double)(x * HL2NWT_FACTOR)
#define INSQR_PER_METERSQR (1.f / (METERS_PER_INCH*METERS_PER_INCH))

#include "vector.h"
#include "newton.h"

//matrix uses this
#ifndef M_PI
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

#define M_PI_F ((float)(M_PI)) // Shouldn't collide with anything.
#define RAD2DEG( x )  ( (float)(x) * (float)(180.f / M_PI_F) )
#define DEG2RAD( x )  ( (float)(x) * (float)(M_PI_F / 180.f) )

inline void SinCos (float angle, float *sine, float *cosine) 
{
	__asm {
	push	ecx
	fld	dword ptr angle
	fsincos
	mov	ecx, dword ptr[cosine]
	fstp      dword ptr [ecx]
	mov 	ecx, dword ptr[sine]
	fstp	dword ptr [ecx]
	pop	ecx
    	}
}

class VMatrix
{
public:
	VMatrix ();
	VMatrix( const float *matrix );

	Vector4D& operator[] (int i) { return (&m_front)[i]; }
	VMatrix (const Vector4D &front, const Vector4D &up, const Vector4D &left, const Vector4D &pos);
	void LoadIdentity( void );

	void GetAngles( Vector &angles );
	void GetOrigin( Vector &origin );
	void GetTransform( Vector &origin, Vector &angles );

	void SetAngles( const Vector &angles );
	void SetOrigin( const Vector &origin );
	void SetTransform( const Vector &origin, const Vector &angles );

	Vector4D RotateVector (const Vector4D &v) const;
	Vector4D UnrotateVector (const Vector4D &v) const;
	Vector4D TransformVector (const Vector4D &v) const;
	Vector4D UntransformVector (const Vector4D &v) const;

	Vector4D m_front;
	Vector4D m_up;
	Vector4D m_left;
	Vector4D m_pos;
};

inline VMatrix::VMatrix ()
{
	//constructor
	//LoadIdentity();
}

inline VMatrix::VMatrix ( const Vector4D &front, const Vector4D &up, const Vector4D &left, const Vector4D &pos) :m_front(front), m_up(up), m_left(left), m_pos(pos)
{
}

inline VMatrix::VMatrix ( const float *matrix )
{
	m_front[0] = *matrix++; 	
	m_front[1] = *matrix++;
	m_front[2] = *matrix++;
	m_front[3] = *matrix++;

	m_left[0] = *matrix++; 	
	m_left[1] = *matrix++;
	m_left[2] = *matrix++;
	m_left[3] = *matrix++;

	m_up[0] = *matrix++; 	
	m_up[1] = *matrix++;
	m_up[2] = *matrix++;
	m_up[3] =	*matrix++;

	m_pos[0] = *matrix++; 	
	m_pos[1] = *matrix++;
	m_pos[2] = *matrix++;
	m_pos[3] = *matrix++;
}

inline void VMatrix::LoadIdentity( void )
{
	m_front[0] = 1.0f; m_front[1] = 0.0f; m_front[2] = 0.0f; m_front[3] = 0.0f; 
	m_left[0]  = 0.0f; m_left[1]  = 1.0f; m_left[2]  = 0.0f; m_left[3]  = 0.0f; 
	m_up[0]    = 0.0f; m_up[1]    = 0.0f; m_up[2]    = 1.0f; m_up[3]    = 0.0f; 
	m_pos[0]	 = 0.0f; m_pos[1]	= 0.0f; m_pos[2]   = 0.0f; m_pos[3]   = 1.0f; 
}

inline Vector4D VMatrix::RotateVector (const Vector4D &v) const
{
	return Vector4D (v.x * m_front.x + v.y * m_up.x + v.z * m_left.x,
		       v.x * m_front.y + v.y * m_up.y + v.z * m_left.y,
		       v.x * m_front.z + v.y * m_up.z + v.z * m_left.z, v.w);
}


inline Vector4D VMatrix::UnrotateVector (const Vector4D &v) const
{
	return Vector4D (v % m_front, v % m_up, v % m_left, v.w);
}


inline Vector4D VMatrix::TransformVector (const Vector4D &v) const
{
	return m_pos + RotateVector(v);
}

inline Vector4D VMatrix::UntransformVector (const Vector4D &v) const
{
	return UnrotateVector(v - m_pos);
}

inline void VMatrix::GetAngles( Vector &angles )
{
	float xyDist = sqrtf( m_front[0] * m_front[0] + m_front[1] * m_front[1] );

	// enough here to get angles?
	if ( xyDist > 0.001f )
	{
		angles[0] = -RAD2DEG( atan2f( -m_front[2], xyDist ) );
		angles[1] = RAD2DEG( atan2f( m_front[1], m_front[0] ) );
		angles[2] = RAD2DEG( atan2f( m_left[2], m_up[2] ) );
	}
	else	// forward is mostly Z, gimbal lock
	{
		angles[0] = -RAD2DEG( atan2f( -m_front[2], xyDist ) );
		angles[1] = RAD2DEG( atan2f( -m_left[0], m_left[1] ) );
		angles[2] = 0;
	}
}

inline void VMatrix::GetOrigin( Vector &origin )
{
	//give this in hl units
	origin[0] = INCHES( m_pos[0] );
	origin[1] = INCHES( m_pos[1] );
	origin[2] = INCHES( m_pos[2] );
}

inline void VMatrix::GetTransform( Vector &origin, Vector &angles )
{
	GetOrigin( origin );
	GetAngles( angles );
}

inline void VMatrix::SetAngles( const Vector &angles )
{
	float	sr, sp, sy, cr, cp, cy;

	SinCos( DEG2RAD( -angles[0] ), &sp, &cp );
	SinCos( DEG2RAD( angles[1] ), &sy, &cy );
	SinCos( DEG2RAD( angles[2] ), &sr, &cr );

	m_front[0] = cp*cy;
	m_front[1] = cp*sy;
	m_front[2] = -sp;

	m_left[0] = sr*sp*cy+cr*-sy;
	m_left[1] = sr*sp*sy+cr*cy;
	m_left[2] = sr*cp;

	m_up[0] = cr*sp*cy+-sr*-sy;
	m_up[1] = cr*sp*sy+-sr*cy;
	m_up[2] = cr*cp;
}

inline void VMatrix::SetOrigin( const Vector &origin )
{
	//convert hl units to meters before putting them to matrix
	m_pos[0] = METERS( origin[0] );
	m_pos[1] = METERS( origin[1] );
	m_pos[2] = METERS( origin[2] );
}

inline void VMatrix::SetTransform( const Vector &origin, const Vector &angles )
{
	if ( angles == g_vecZero )
	{
		LoadIdentity();
		SetOrigin( origin );
	}
	else
	{
		SetOrigin( origin );
		SetAngles( angles );
	}
}

inline VMatrix PitchMatrix(dFloat ang)
{
	dFloat cosAng;
	dFloat sinAng;
	sinAng = sinf (ang);
	cosAng = cosf (ang);
	return VMatrix (Vector4D (1.0f,    0.0f,    0.0f, 0.0f), 
		      Vector4D (0.0f,  cosAng,  sinAng, 0.0f),
		      Vector4D (0.0f, -sinAng,  cosAng, 0.0f), 
		      Vector4D (0.0f,    0.0f,    0.0f, 1.0f)); 

}

inline VMatrix YawMatrix(dFloat ang)
{
	dFloat cosAng;
	dFloat sinAng;
	sinAng = sinf (ang);
	cosAng = cosf (ang);
	return VMatrix (Vector4D (cosAng, 0.0f, -sinAng, 0.0f), 
		      Vector4D (0.0f,   1.0f,    0.0f, 0.0f), 
		      Vector4D (sinAng, 0.0f,  cosAng, 0.0f), 
		      Vector4D (0.0f,   0.0f,    0.0f, 1.0f)); 
}

inline VMatrix RollMatrix(dFloat ang)
{
	dFloat cosAng;
	dFloat sinAng;
	sinAng = sinf (ang);
	cosAng = cosf (ang);
	return VMatrix (Vector4D ( cosAng, sinAng, 0.0f, 0.0f), 
		      Vector4D (-sinAng, cosAng, 0.0f, 0.0f),
		      Vector4D (   0.0f,   0.0f, 1.0f, 0.0f), 
		      Vector4D (   0.0f,   0.0f, 0.0f, 1.0f)); 
}
																		 
#endif
