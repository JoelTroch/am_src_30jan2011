#include "ref_params.h"
#include "com_model.h"

#define	SURF_PLANEBACK		2
#define	SURF_DRAWSKY		4
#define SURF_DRAWSPRITE		8
#define SURF_DRAWTURB		0x10
#define SURF_DRAWTILED		0x20
#define SURF_DRAWBACKGROUND	0x40
#define SURF_UNDERWATER		0x80
#define SURF_DONTWARP		0x100
#define BACKFACE_EPSILON	0.01

// 0-2 are axial planes
#define	PLANE_X			0
#define	PLANE_Y			1
#define	PLANE_Z			2

#define SURF_BUMPREADY		(1<<9)
#define SURF_SPECULAR		(1<<10)

mleaf_t		*Mod_PointInLeaf (vec3_t p, model_t *model);