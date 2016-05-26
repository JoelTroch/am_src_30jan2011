#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"
#include <string.h>
#include <stdio.h>
#include "parsemsg.h"
#include "event_api.h"
#include "vgui_TeamFortressViewport.h"

int CHudNoise::Init(void)
{
	m_iFlags |= HUD_ACTIVE;
//	m_iFlags = 0;
	gHUD.AddHudElem(this);

	return 1;
}


int CHudNoise::VidInit(void)
{
	m_hFlicker = LoadSprite("sprites/noise.spr");
//	m_hFlicker = LoadSprite("sprites/nvg.spr");

	return 1;
}

int CHudNoise::Draw(float fTime)
{
//	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
//	return 1;

/*	int x, y, w, h;
	int frame;

	SPR_Set(m_hFlicker, 11, 11, 11 );

	// play at 15fps
	frame = (int)(fTime * 15) % SPR_Frames(m_hFlicker);

	w = SPR_Width(m_hFlicker,0);
	h = SPR_Height(m_hFlicker,0);

	for(y = -(rand() % h); y < ScreenHeight; y += h) 
	{
		for(x = -(rand() % w); x < ScreenWidth; x += w) 
		{
			SPR_DrawAdditive( frame, x, y, NULL );
		}
	}
*/
	return 1;
}

void CHudNoise::Reset(void)
{
}