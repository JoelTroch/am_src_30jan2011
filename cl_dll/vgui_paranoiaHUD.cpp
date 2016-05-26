// ====================================
// Paranoia vgui hud
// written by BUzer.
// ====================================

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "const.h"
#include "entity_types.h"
#include "cdll_int.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_hud.h"
#include "vgui_paranoiatext.h" // Wargon
#include "..\game_shared\vgui_loadtga.h"
#include<VGUI_TextImage.h>

#include "ammohistory.h"


#include "triangleapi.h"
#include "pm_defs.h"
#include "event_api.h"
#include "pmtrace.h"

using namespace std;

float g_fLeftAlpha;
float g_fRightAlpha;
float g_fMenuAlpha = 255.0;//invisble at start
float g_fXhairAlpha = 255.0;//invisble at start

extern int gRadarIsOn;//hudradar.cpp
extern int g_iRoundtime;//timer.cpp
extern bool g_NVGOn;//nvg.cpp
extern BOOL bKeypadVisible;

//#if LOL

#define WEAPON_PAINKILLER	24 // should match server's index

#define HEALTH_RIGHT_OFFSET	(XRES(10))
#define HEALTH_DOWN_OFFSET	(YRES(10))
#define HEALTH_MIN_SPACE	(YRES(3)) // minimum space between bars
#define HEALTH_FLASH_TIME	0.3

#define HEALTH_FADE_TIME	5
#define HEALTH_ZERO_ALPHA	150
#define HEALTH_ALPHA		70

// Wargon: Èêîíêà þçà.
#define USAGE_FADE_TIME 1
#define USAGE_ALPHA 70

#define MAX_WEAPONS_ICONIZED	16//a pretty stupid descriptive name

struct sWeapon
{
	char szWeaponName[32];
	char szAmmoIcon[32];
};

sWeapon PlayerWeapons[MAX_WEAPONS_ICONIZED] =
{ 
	{ "weapon_9mmAR", "ammo_9mm" },
	{ "weapon_9mmhandgun", "ammo_9mm" },
	{ "weapon_ber92f", "ammo_9mm" },
	{ "weapon_deagle", "ammo_9mm" },
	{ "weapon_glock18", "ammo_9mm" },
	{ "weapon_p90", "ammo_9mm" },
	
	{ "weapon_AK47", "ammo_762mm" },
	{ "weapon_famas", "ammo_762mm" },
	{ "weapon_m4a1", "ammo_762mm" },
	{ "weapon_m16", "ammo_762mm" },
	{ "weapon_M249", "ammo_762mm" },
	{ "weapon_sniper", "ammo_762mm" },

	{ "weapon_shotgun", "ammo_buckshot" },
	{ "weapon_usas", "ammo_buckshot" },
	{ "weapon_shotgun_non_lethal", "ammo_buckshot" },

	{ "weapon_handgrenade", "ammo_nade" },
};

//The following icons are stored on VGUI/HUD/ 's folder
const char* weaponNames[NUM_WEAPON_ICONS] = 
{
	"ammo_def",//default icon
	"ammo_9mm",//9mm
	"ammo_762mm",
	"ammo_buckshot",
	"ammo_nade",
	"base_right",//the ammo stuff
	"base_left",//life status, probabily
	"armor",//small icon for armor
	"radar",
	"logo",
	"nvg",
	"nvg_overlay",
	"scr_overlay",
	"menu_base",
	"menu_avatar_hero",
	"menu_locked",
	"sound0",
	"sound1",
	"sound2",
	"sound3",
	"sound4",
	"crosshair1",
	"gas_mask",
	"head_bag",
	"zoom",
	"binoculars",
	"noise",
	"crosshair_zoom"
};

// Wargon: Èêîíêà þçà.
int CanUseStatus;
int Red;
int __MsgFunc_CanUse( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	CanUseStatus = READ_BYTE();
	Red = READ_BYTE();

	return 1;
}

void CanUseInit( void )
{
	HOOK_MESSAGE( CanUse );
}

BitmapTGA* LoadResolutionImage (const char *imgname)
{
	BitmapTGA *pBitmap;
	static int resArray[] =
	{
		320, 400, 512, 640, 800,
		1024, 1152, 1280, 1600
	};

	// try to load image directly
	pBitmap = vgui_LoadTGA(imgname);
	if (!pBitmap)
	{
		//resolution based image. Should contain %d substring
		int resArrayIndex = 0;
		int i = 0;
		while ((resArray[i] <= ScreenWidth) && (i < 9))
		{
			resArrayIndex = i;
			i++;
		}

		while(pBitmap == NULL && resArrayIndex >= 0)
		{
			char imgName[64];
			sprintf(imgName, imgname, resArray[resArrayIndex]);
			pBitmap = vgui_LoadTGA(imgName);
			resArrayIndex--;
		}
	}

	return pBitmap;
}


int ShouldDrawHUD()
{
	if (!gHUD.m_pCvarDraw->value)
		return FALSE;

	if ((gHUD.m_iHideHUDDisplay & HIDEHUD_HEALTH) || gEngfuncs.IsSpectateOnly() )
		return FALSE;

	if (gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)))
		return TRUE;

	return FALSE;
}


// simple class that owns pointer to a bitmap and draws it
class ImageHolder : public Panel
{
public:
	ImageHolder(const char *imgname, Panel *parent) : Panel(0, 0, 10, 10)
	{
		m_pBitmap = LoadResolutionImage(imgname);
		setParent(parent);
		setPaintBackgroundEnabled(false);
		setVisible(true);
		if (m_pBitmap) m_pBitmap->setPos(0, 0);
	}

	~ImageHolder() {delete m_pBitmap;}
	BitmapTGA *GetBitmap() {return m_pBitmap;}

protected:
	virtual void paint()
	{
		if (ShouldDrawHUD())
		{
			if (m_pBitmap)
				m_pBitmap->doPaint(this);
		}
	}

	BitmapTGA *m_pBitmap;
};


class ShadowLabel : public Label
{
public:
	ShadowLabel(const char* text,int x,int y) : Label(text, x, y) {}

protected:
	virtual void paint()
	{
		int mr, mg, mb, ma;
		int ix, iy;
		getFgColor(mr, mg, mb, ma);
		_textImage->getPos(ix, iy);
		_textImage->setPos(ix+1, iy+1);
		_textImage->setColor( Color(0, 0, 0, ma) );
		_textImage->doPaint(this);
		_textImage->setPos(ix, iy);
		_textImage->setColor( Color(mr, mg, mb, ma) );
		_textImage->doPaint(this);
	}
};


void Hud2Init()
{
//	gEngfuncs.pfnHookUserMsg("RadioIcon", MsgShowRadioIcon);
}

void CHud2::Initialize()
{
	health = -1;
	armor = -1;
	slowmotion = -1;
	level = -1;
//	m_fMedkitUpdateTime = 0;
	m_fUsageUpdateTime = 0; // Wargon: Èêîíêà þçà.
	iLevelNotified = CVAR_GET_FLOAT("xp_points") / 100;//same as current level
}

CHud2::CHud2() : Panel(0, 0, XRES(640), YRES(480))
{
	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hTextScheme = pSchemes->getSchemeHandle( "HUD_Main" );
	
	Font *pFont = pSchemes->getFont( hTextScheme );

	hTextScheme = pSchemes->getSchemeHandle( "HUD_Main" );
	m_pFontDigits = pSchemes->getFont( hTextScheme );

	hTextScheme = pSchemes->getSchemeHandle( "Default Text" );
	m_pFontSystem = pSchemes->getFont( hTextScheme );

	hTextScheme = pSchemes->getSchemeHandle( "HUD_BC" );
	m_pFontHUD_BC = pSchemes->getFont( hTextScheme );

//	hTextScheme = pSchemes->getSchemeHandle( "HUD_Bars" );
//	m_pFontHUDBars = pSchemes->getFont( hTextScheme );

 	/*
		Font *pNormalFont = pSchemes->getFont( hTextScheme );

		Panel* panel = new Panel(XRES(120), YRES(180), XRES(400), YRES(120));
		panel->setParent(this);
		panel->setBgColor(0, 0, 0, 100);
		panel->setBorder(new LineBorder);

		int butX, butY;
		Button* button = new Button("     OK     ", 0, 0);
		button->setParent(panel);
		button->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );

		button->getSize(butX, butY);
		butX = (panel->getWide() - butX) / 2;
		butY = panel->getTall() - butY - YRES(10);
		button->setPos( butX, butY );




		Button* button2 = new Button("Toogle Silencer", 0, 0);
		button2->setParent(panel);

		button2->addActionSignal( new CMenuHandler_StringCommand("silencer") );
			button2->setImage(vgui_LoadTGA("gfx/vgui/arrowup.tga"));

		button2->setPos(CVAR_GET_FLOAT( "vector_x" ), CVAR_GET_FLOAT( "vector_y" ) );
*/

	//
	// load health bars
	//
	//health
	m_pBitmapHealthFull = new ImageHolder("gfx/vgui/%d_health_full.tga", this);
	m_pBitmapHealthEmpty = new ImageHolder("gfx/vgui/%d_health_empty.tga", this);
	m_pBitmapHealthFlash = new ImageHolder("gfx/vgui/%d_health_flash.tga", this);

	if (m_pBitmapHealthFull->GetBitmap())
		m_pBitmapHealthFull->GetBitmap()->getSize(m_iHealthBarWidth, m_iHealthBarHeight);
	else
		m_iHealthBarWidth = m_iHealthBarHeight = 0;
	
	//slowmotion
	m_pBitmapSlowMotionFull = new ImageHolder("gfx/vgui/%d_slowmotion_full.tga", this);
	m_pBitmapSlowMotionEmpty = new ImageHolder("gfx/vgui/%d_slowmotion_empty.tga", this);
	m_pBitmapSlowMotionFlash = new ImageHolder("gfx/vgui/%d_slowmotion_flash.tga", this);

	if (m_pBitmapSlowMotionFull->GetBitmap())
		m_pBitmapSlowMotionFull->GetBitmap()->getSize(m_iSlowMotionBarWidth, m_iSlowMotionBarHeight);
	else
		m_iSlowMotionBarWidth = m_iSlowMotionBarHeight = 0;

	m_pBitmapLevelFull = new ImageHolder("gfx/vgui/%d_level_full.tga", this);
	m_pBitmapLevelEmpty = new ImageHolder("gfx/vgui/%d_level_empty.tga", this);

	if (m_pBitmapLevelFull->GetBitmap())
		m_pBitmapLevelFull->GetBitmap()->getSize(m_iLevelBarWidth, m_iLevelBarHeight);
	else
		m_iLevelBarWidth = m_iLevelBarHeight = 0;

	m_iHealthBarXpos = 0;
	m_iHealthBarYpos = 0;

 	m_iSlowMotionBarXpos = 0;
	m_iSlowMotionBarYpos = 0;

 	m_iLevelBarXpos = 100;
	m_iLevelBarYpos = 100;
		
	m_pBitmapHealthFull->GetBitmap()->setColor(Color(HUD_COLOR, 255));
	m_pBitmapHealthEmpty->GetBitmap()->setColor(Color(HUD_COLOR, 255));

	m_pBitmapSlowMotionFull->GetBitmap()->setColor(Color(HUD_COLOR, 255));
	m_pBitmapSlowMotionEmpty->GetBitmap()->setColor(Color(HUD_COLOR, 255));

	m_pBitmapLevelFull->GetBitmap()->setColor(Color(HUD_COLOR, 255));
	m_pBitmapLevelEmpty->GetBitmap()->setColor(Color(HUD_COLOR, 255));

	//
	// load ammo icons
	//
	for (int i = 0; i < NUM_WEAPON_ICONS; i++)
	{
		char path[256] = "gfx/vgui/HUD/%d_";
		strcat(path, weaponNames[i]);
		strcat(path, ".tga");
	//	CONPRINT("Loading ammo icon [%s]\n", path);
		m_pWeaponIconsArray[i] = LoadResolutionImage(path);
		if (!m_pWeaponIconsArray[i])
			CONPRINT("Failed to load ammo icon [%s]\n", path);
	}

	//
	// Wargon: Èêîíêà þçà.
	//
	m_pUsageIcon = new CImageLabel("usage", ScreenWidth / 2 - 12, ScreenHeight / 2 + 100);

	if (!m_pUsageIcon->m_pTGA)
	{
		delete m_pUsageIcon;
		m_pUsageIcon = NULL;
		gEngfuncs.Con_Printf("Usage icon can't be loaded\n");
	}
	else
	{
		int bw, bt;
		m_pUsageIcon->setParent(this);
		m_pUsageIcon->setPaintBackgroundEnabled(false);
		m_pUsageIcon->setVisible(false);
		m_pUsageIcon->m_pTGA->getSize(bw, bt);
		m_pUsageIcon->setSize(bw, bt);
		m_fUsageUpdateTime = 0;
	}
		/*
	//45x72

	pBuddyIcons[0] = new CImageLabel("buddy", 0 + HEALTH_RIGHT_OFFSET, (ScreenHeight / 2) );
	
	if (pBuddyIcons[0]->m_pTGA)//be sure there's a image... if now it will crash getting wide on the next line
	{
		pBuddyIcons[1] = new CImageLabel("buddy", pBuddyIcons[0]->getImageWide(), (ScreenHeight / 2) );//use width from the original image
		pBuddyIcons[2] = new CImageLabel("buddy", pBuddyIcons[0]->getImageWide() * 2, (ScreenHeight / 2) );
		pBuddyIcons[3] = new CImageLabel("buddy", pBuddyIcons[0]->getImageWide() * 3, (ScreenHeight / 2) );
	}

	if (!pBuddyIcons[0]->m_pTGA)//theres no image... delete all
	{
		for( int i = -1; i < MAX_ICONS; i++)
		{
			delete pBuddyIcons[i];
			pBuddyIcons[i] = NULL;
		}
		gEngfuncs.Con_Printf("m_pBuddyIcon can't be loaded\n");
	}
	else
	{
		int bw, bt;
			
		for( int i = -1; i < MAX_ICONS; i++)
		{
			pBuddyIcons[i]->setParent(this);
			pBuddyIcons[i]->setPaintBackgroundEnabled(false);
			pBuddyIcons[i]->setVisible(false);
			pBuddyIcons[i]->m_pTGA->getSize(bw, bt);
			pBuddyIcons[i]->setSize(bw, bt);
		}
	}
*/
	//create the different blood images and hide them
	for( int j = 0; j <MAX_BLOOD_TYPES; j++ )
	{
		char szImageName[32];
		sprintf( szImageName, "blood_%i", j +1);//j+1 = starts from 1

		Bloods[j].m_pBloodImage[j] = new CImageLabel( szImageName, 0, 0 );
		CONPRINT("Adding pointers\n" );
		if (!Bloods[j].m_pBloodImage[j]->m_pTGA)
		{
			delete Bloods[j].m_pBloodImage[j];
			Bloods[j].m_pBloodImage[j] = NULL;
			gEngfuncs.Con_Printf("Bloods TGA file can't be loaded\n");
		}
		else
		{
			int blood_icon_w,  blood_icon_h;
			Bloods[j].m_pBloodImage[j]->setParent(this);
			Bloods[j].m_pBloodImage[j]->setPaintBackgroundEnabled(false);
		//	Bloods[j].m_pBloodImage[j]->setVisible(false);
			Bloods[j].m_pBloodImage[j]->m_pTGA->getSize(blood_icon_w, blood_icon_h);
			Bloods[j].m_pBloodImage[j]->setSize(blood_icon_w, blood_icon_h);
			CONPRINT("Creating images\n" );

			Bloods[j].fBloodAlpha = 0.0;
		}
	}
	
	//create the different raindrops images and hide them
	for( int p = 0; p <MAX_RAINDROP_TYPES; p++ )
	{
		char szImageName[32];
		sprintf( szImageName, "%i", p +1);//p+1 = starts from 1

		Raindrops[p].m_pRainDropImage[p] = new CImageLabel( szImageName, 0, 0 );
		CONPRINT("Adding pointers\n" );
		if (!Raindrops[p].m_pRainDropImage[p]->m_pTGA)
		{
			delete Raindrops[p].m_pRainDropImage[p];
			Raindrops[p].m_pRainDropImage[p] = NULL;
			gEngfuncs.Con_Printf("raindrop TGA file can't be loaded\n");
		}
		else
		{
			int rain_icon_w,  rain_icon_h;
			Raindrops[p].m_pRainDropImage[p]->setParent(this);
			Raindrops[p].m_pRainDropImage[p]->setPaintBackgroundEnabled(false);
		//	Raindrops[p].m_pRainDropImage[p]->setVisible(false);
			Raindrops[p].m_pRainDropImage[p]->m_pTGA->getSize(rain_icon_w, rain_icon_h);
			Raindrops[p].m_pRainDropImage[p]->setSize(rain_icon_w, rain_icon_h);
			CONPRINT("Creating images\n" );

			Raindrops[p].fRainDropAlpha = 0.0;
		}
	}

	// ======== Séléction des armes =========================
	// ======================================================
	m_pHudWeaponsSelPanel = new CWeaponsSelPanel ( 4, 44, 202, 32 + MAX_WEAPON_POSITIONS * 45 );
	m_pHudWeaponsSelPanel->setParent( this );
	m_pHudWeaponsSelPanel->setBounds( 4, 44, 202, 32 + MAX_WEAPON_POSITIONS * 45 );
	m_pHudWeaponsSelPanel->setVisible( false );
	m_pHudWeaponsSelPanel->setFgColor( 0, 0, 0, 255 );
	m_pHudWeaponsSelPanel->setBgColor( 0, 0, 0, 255 );
	m_pHudWeaponsSelPanel->Initialize();
}

CHud2::~CHud2()
{
	for (int i = 0; i < NUM_WEAPON_ICONS; i++)
	{
		delete m_pWeaponIconsArray[i];
	}
	
//	gEngfuncs.Cvar_SetValue( "score_time", gEngfuncs.GetClientTime() );
}

void CHud2::paintBackground()
{
//	Panel::paintBackground();
}

void CHud2::ResetOverlays()
{
	CONPRINT("ResetOverlays()\n" );

	for( int i = 0; i < MAX_RAINDROP_TYPES; i++ )
	{
		if( Raindrops[i].m_pRainDropImage )
		{
			Raindrops[i].fRainDropAlpha = 255;
			Raindrops[i].m_pRainDropImage[i]->m_pTGA->setColor(Color(HUD_COLOR, 255));
		//	Raindrops[i].m_pRainDropImage[i]->setVisible(false);
		}
	}
			
	for( int p = 0; p < MAX_BLOOD_TYPES; p++ )		
	{

		if( Bloods[p].m_pBloodImage )
		{
			Bloods[p].fBloodAlpha = 255;
			Bloods[p].m_pBloodImage[p]->m_pTGA->setColor(Color(HUD_COLOR, 255));
		//		Bloods[p].m_pBloodImage[p]->setVisible(false);
		}
	}
}

// buz: I dunno exactly, what solve() function does. I just wanna to find
// some function, who being called each frame, to put panels position setting in there.
// I've tried paintBackground, Chud::Redraw, CHud::Think, and some others, but
// health and armor bars sizes are jumping during interpolation and
// panels motion looks jerky (especially in steam version).
// Putting this in solve() seems to fix first problem.
void CHud2::solve()
{
	float curtime = gEngfuncs.GetClientTime();

	// Wargon: Èêîíêà þçà.
	if (m_pUsageIcon)
	{
		m_fUsageUpdateTime = curtime;
		if (CanUseStatus && gHUD.m_pCvarDraw->value && !(gViewPort && gViewPort->m_pParanoiaText && gViewPort->m_pParanoiaText->isVisible()))
		{
			m_pUsageIcon->setVisible(true);
			if (m_fUsageUpdateTime > curtime)
				m_fUsageUpdateTime = curtime;
			float frac = curtime - m_fUsageUpdateTime;
			int alpha = USAGE_ALPHA;
			if (frac < USAGE_FADE_TIME)
			{
				frac = frac / USAGE_FADE_TIME;
				alpha = (int)(frac * USAGE_ALPHA);
			}
					
			if (Red)
			m_pUsageIcon->m_pTGA->setColor(Color(255,0,0, alpha));
			else
			m_pUsageIcon->m_pTGA->setColor(Color(HUD_COLOR, alpha));

			gEngfuncs.Cvar_SetValue( "icon_show", 1 );
		}
		else
		{
			m_pUsageIcon->setVisible(false);
			gEngfuncs.Cvar_SetValue( "icon_show", 0 );
		}
			
		if (bKeypadVisible == TRUE)  
		m_pUsageIcon->setVisible(false);
	}

	//draw all the blod icons and make them visible
	if ( (gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT))) && ShouldDrawHUD() ) // Wargon: Èíôîðìàöèÿ î ïàòðîíàõ ðèñóåòñÿ òîëüêî åñëè hud_draw = 1.
	for( int j = 0; j <MAX_BLOOD_TYPES; j++ )
	{
		if (Bloods[j].m_pBloodImage[j])
		{
			//commented because there were no updates when no suit!
		//	if ( gHUD.m_pCvarDraw->value && !(gViewPort && gViewPort->m_pParanoiaText && gViewPort->m_pParanoiaText->isVisible()))
			{
				int pos_x, pos_y;
				Bloods[j].m_pBloodImage[j]->getPos( pos_x, pos_y );

				Bloods[j].m_pBloodImage[j]->m_pTGA->setColor(Color(HUD_COLOR, 222));
						
				Bloods[j].m_pBloodImage[j]->setVisible(true);
		
				if( (pos_x == 0) || (pos_y == 0) )//initial position? don't draw then!
				Bloods[j].m_pBloodImage[j]->setVisible(false);
			//	else
			//	CONPRINT("Drawing Bloods images\n" );
			}
/*			else
			{
				Bloods[j].m_pBloodImage[j]->setVisible(false);
			}*/
		}
	}

	//draw all the rain icons and make them visible
	if ( (gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT))) && ShouldDrawHUD() ) // Wargon: Èíôîðìàöèÿ î ïàòðîíàõ ðèñóåòñÿ òîëüêî åñëè hud_draw = 1.
	for( int p = 0; p <MAX_RAINDROP_TYPES; p++ )
	{
		if (Raindrops[p].m_pRainDropImage[p])
		{
			//commented because there were no updates when no suit!
		//	if ( gHUD.m_pCvarDraw->value && !(gViewPort && gViewPort->m_pParanoiaText && gViewPort->m_pParanoiaText->isVisible()))
			{
				int pos_x, pos_y;
				Raindrops[p].m_pRainDropImage[p]->getPos( pos_x, pos_y );
			
				Raindrops[p].m_pRainDropImage[p]->m_pTGA->setColor(Color(HUD_COLOR, 222));

				Raindrops[p].m_pRainDropImage[p]->setVisible(true);
						
				if( (pos_x == 0) || (pos_y == 0) )//initial position? don't draw then!
				Raindrops[p].m_pRainDropImage[p]->setVisible(false);
			//	else
			//	CONPRINT("Drawing raindrops images\n" );
			}
		/*	else
			Raindrops[p].m_pRainDropImage[p]->setVisible(false);*/
		}
	}


	//change rain's alpha
		for( int i = 0; i < MAX_RAINDROP_TYPES; i++ )//TODO: cvar-ize MAX_RAINDROPS
		{
int time = i;
			//TODO: cvar-ize the following value

		//	cl_entity_t *player = gEngfuncs.GetLocalPlayer();
		//	if ( !player )
		//		return;

		//	if( player->angles[0] >= 3.0 )// facing down
		//	time = i;

			//increase the alpha to hide each image
			if( Raindrops[i].fRainDropAlpha < 255.0 )
			{
				Raindrops[i].fRainDropAlpha += (gHUD.m_flTimeDelta * time );
//				CONPRINT("increasing alpha\n" );
			}
	
			

	
//			if ( CVAR_GET_FLOAT("show_rain") == 1 )
			{
			
					if( Raindrops[i].fRainDropAlpha >= 255.0 )//is this image hidden?
					{
						int raindrop_icon_w,  raindrop_icon_h;
						Raindrops[i].m_pRainDropImage[i]->m_pTGA->getSize(raindrop_icon_w, raindrop_icon_h);
							
					//	gEngfuncs.Con_Printf( "time: %i, angles %f\n", time, player->angles[0] );

						//hide, set new pos and make it visible
						Raindrops[i].m_pRainDropImage[i]->setVisible(false);
 						Raindrops[i].m_pRainDropImage[i]->setPos(gEngfuncs.pfnRandomLong( 1, ScreenWidth - raindrop_icon_w ), gEngfuncs.pfnRandomLong( 1, ScreenHeight - raindrop_icon_h ));

						if ( CVAR_GET_FLOAT("show_rain") == 0 )
						Raindrops[i].fRainDropAlpha = 255.0;//0.0 ?
							else
						Raindrops[i].fRainDropAlpha = 222.0;//0.0 ?

					//	CONPRINT("changing position and resetting alpha\n" );
					}
				
			}
								
			if( Raindrops[i].fRainDropAlpha <= 0.0 )
			Raindrops[i].fRainDropAlpha = 222.0;//0.0 ?

			//set alpha to each image
			Raindrops[i].m_pRainDropImage[i]->m_pTGA->setColor(Color(255,255,255, int(Raindrops[i].fRainDropAlpha)));
		}


	//
	// update health and armor bars
	// (damn, it's so messy...)
	int healthdiv;
	
	// health bar
	if (m_pBitmapHealthEmpty->GetBitmap() && m_pBitmapHealthFull->GetBitmap())
	{
		m_pBitmapHealthFlash->setVisible(false);
		m_pBitmapHealthEmpty->GetBitmap()->setColor(Color(HUD_COLOR, 0));
		m_pBitmapHealthFull->GetBitmap()->setColor(Color(HUD_COLOR, 0));

		if (curtime >= m_fHealthUpdateTime + HEALTH_FLASH_TIME)
		{
			healthdiv = (int)((float)health/100 * m_iHealthBarWidth);
			float frac = curtime - m_fHealthUpdateTime - HEALTH_FLASH_TIME;
			int targetalpha = HEALTH_ALPHA;
			if (health == 0) targetalpha = HEALTH_ZERO_ALPHA;
			int alpha = targetalpha;
			if (frac < HEALTH_FADE_TIME)
			{
				frac = frac / HEALTH_FADE_TIME;
				alpha = (int)(frac * targetalpha);
			}
			m_pBitmapHealthEmpty->GetBitmap()->setColor(Color(HUD_COLOR, alpha));
			m_pBitmapHealthFull->GetBitmap()->setColor(Color(HUD_COLOR, alpha));
		}
		else
		{
			float frac = (curtime - m_fHealthUpdateTime) / HEALTH_FLASH_TIME;
			if ( ((health < oldhealth) && m_pBitmapHealthFlash->GetBitmap()) )
			{
				m_pBitmapHealthFlash->setVisible(true);
				m_pBitmapHealthFlash->GetBitmap()->setColor(Color(HUD_COLOR, 255*frac));
			}
			frac = 1 - frac;
			frac *= frac;
			healthdiv = health - (int)((float)(health - oldhealth)*frac);
			healthdiv = (int)((float)healthdiv/100 * m_iHealthBarWidth);
		}
		
		m_pBitmapHealthFull->setBounds(m_iHealthBarXpos-XRES(12), m_iHealthBarYpos, healthdiv, m_iHealthBarHeight);
		m_pBitmapHealthFlash->setBounds(m_iHealthBarXpos-XRES(12), m_iHealthBarYpos, healthdiv, m_iHealthBarHeight);

		m_pBitmapHealthEmpty->setBounds(m_iHealthBarXpos-XRES(12) + healthdiv, m_iHealthBarYpos,
			m_iHealthBarWidth - healthdiv, m_iHealthBarHeight);
		m_pBitmapHealthEmpty->GetBitmap()->setPos( -healthdiv, 0 );
	}

	// slowmotion bar
	if (m_pBitmapSlowMotionEmpty->GetBitmap() && m_pBitmapSlowMotionFull->GetBitmap())
	{
		m_pBitmapSlowMotionFlash->setVisible(false);
		m_pBitmapSlowMotionEmpty->GetBitmap()->setColor(Color(HUD_COLOR, 0));
		m_pBitmapSlowMotionFull->GetBitmap()->setColor(Color(HUD_COLOR, 0));

		if (curtime >= m_fSlowMotionUpdateTime + HEALTH_FLASH_TIME)
		{
			healthdiv = (int)((float)slowmotion/100 * m_iSlowMotionBarWidth);
			float frac = curtime - m_fSlowMotionUpdateTime - HEALTH_FLASH_TIME;
			int targetalpha = HEALTH_ALPHA;
			if (slowmotion == 0) targetalpha = HEALTH_ZERO_ALPHA;
			int alpha = targetalpha;
			if (frac < HEALTH_FADE_TIME)
			{
				frac = frac / HEALTH_FADE_TIME;
				alpha = (int)(frac * targetalpha);
			}
			m_pBitmapSlowMotionEmpty->GetBitmap()->setColor(Color(HUD_COLOR, alpha));
			m_pBitmapSlowMotionFull->GetBitmap()->setColor(Color(HUD_COLOR, alpha));
		}
		else
		{
			float frac = (curtime - m_fSlowMotionUpdateTime) / HEALTH_FLASH_TIME;
			if ((slowmotion < oldslowmotion) && m_pBitmapSlowMotionFlash->GetBitmap()) // we had take damage, make red flash
			{
				m_pBitmapSlowMotionFlash->setVisible(true);
				m_pBitmapSlowMotionFlash->GetBitmap()->setColor(Color(HUD_COLOR, 255*frac));
			}
			frac = 1 - frac;
			frac *= frac;
			healthdiv = slowmotion - (int)((float)(slowmotion - oldslowmotion)*frac);
			healthdiv = (int)((float)healthdiv/100 * m_iSlowMotionBarWidth);
		}

		int healthdiv2 = healthdiv;
 		healthdiv2 -= m_iSlowMotionBarWidth;
		healthdiv2 = healthdiv2 *-1;

		m_pBitmapSlowMotionFull->setBounds(m_iSlowMotionBarXpos+XRES(12) + healthdiv2, m_iSlowMotionBarYpos, m_iSlowMotionBarWidth - healthdiv2, m_iSlowMotionBarHeight);
		m_pBitmapSlowMotionFull->GetBitmap()->setPos( -healthdiv2, 0 );

		m_pBitmapSlowMotionFlash->setBounds(m_iSlowMotionBarXpos+XRES(12) + healthdiv2, m_iSlowMotionBarYpos, m_iSlowMotionBarWidth - healthdiv2, m_iSlowMotionBarHeight);
		m_pBitmapSlowMotionFlash->GetBitmap()->setPos( -healthdiv2, 0 );

		m_pBitmapSlowMotionEmpty->setBounds(m_iSlowMotionBarXpos+XRES(12), m_iSlowMotionBarYpos, m_iSlowMotionBarWidth - healthdiv, m_iSlowMotionBarHeight);
	}

	// health bar
	if (m_pBitmapLevelEmpty->GetBitmap() && m_pBitmapLevelFull->GetBitmap())
	{
		m_pBitmapLevelFull->GetBitmap()->setColor(Color(HUD_COLOR, 0));
		m_pBitmapLevelEmpty->GetBitmap()->setColor(Color(HUD_COLOR, 0));

		if (curtime >= m_fLevelUpdateTime + HEALTH_FLASH_TIME)
		{
			healthdiv = (int)((float)level/100 * m_iLevelBarWidth);
			float frac = curtime - m_fLevelUpdateTime - HEALTH_FLASH_TIME;
			int targetalpha = HEALTH_ALPHA;
			if (level == 0) targetalpha = HEALTH_ZERO_ALPHA;
			int alpha = targetalpha;
			if (frac < HEALTH_FADE_TIME)
			{
				frac = frac / HEALTH_FADE_TIME;
				alpha = (int)(frac * targetalpha);
			}
			m_pBitmapLevelEmpty->GetBitmap()->setColor(Color(HUD_COLOR, alpha));
			m_pBitmapLevelFull->GetBitmap()->setColor(Color(HUD_COLOR, alpha));
		}
		else
		{
			float frac = (curtime - m_fLevelUpdateTime) / HEALTH_FLASH_TIME;

/*			if ( ((level < oldhealth) && m_pBitmapLevelFlash->GetBitmap()) )
			{
				m_pBitmapLevelFlash->setVisible(true);
				m_pBitmapLevelFlash->GetBitmap()->setColor(Color(HUD_COLOR, 255*frac));
			}*/

			frac = 1 - frac;
			frac *= frac;
			healthdiv = level - (int)((float)(level - oldlevel)*frac);
			healthdiv = (int)((float)healthdiv/100 * m_iLevelBarWidth);
		}
		
		m_pBitmapLevelFull->setBounds(m_iLevelBarXpos, m_iLevelBarYpos, healthdiv, m_iLevelBarHeight);

		m_pBitmapLevelEmpty->setBounds(m_iLevelBarXpos + healthdiv, m_iLevelBarYpos, m_iLevelBarWidth - healthdiv, m_iLevelBarHeight);
		m_pBitmapLevelEmpty->GetBitmap()->setPos( -healthdiv, 0 );
	}

	Panel::solve();
}


void CHud2::paint()
{
	//
	// draw ammo counters
	//

	if ( (gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT))) && ShouldDrawHUD() ) // Wargon: Èíôîðìàöèÿ î ïàòðîíàõ ðèñóåòñÿ òîëüêî åñëè hud_draw = 1.
	{
		WEAPON *pw = gHUD.m_Ammo.m_pWeapon; // shorthand

		cl_entity_t *player = gEngfuncs.GetLocalPlayer();
		if ( !player )
			return;

		if( player->angles[0] >= 7.6 || CVAR_GET_FLOAT( "cl_fadehud" ) == 0 )//show HUD when facing down
		{
			g_fLeftAlpha = 0.0;
			g_fRightAlpha = 0.0;
		}

		
		//alpha test
		if (g_fLeftAlpha < 255.0)
			g_fLeftAlpha += (gHUD.m_flTimeDelta * 20);

		if (g_fRightAlpha < 255.0)
			g_fRightAlpha += (gHUD.m_flTimeDelta * 20);

		if ( CVAR_GET_FLOAT( "cl_menu" ) != 0 )
		{
			g_fLeftAlpha = 255.0;
			g_fRightAlpha = 255.0;
		}
			


	
		//fade the health and the ammo bar
		m_pBitmapHealthFull->GetBitmap()->setColor(Color(HUD_COLOR, int(g_fLeftAlpha)));
		m_pBitmapHealthEmpty->GetBitmap()->setColor(Color(HUD_COLOR, int(g_fLeftAlpha)));

		m_pBitmapSlowMotionFull->GetBitmap()->setColor(Color(HUD_COLOR, int(g_fRightAlpha)));
		m_pBitmapSlowMotionEmpty->GetBitmap()->setColor(Color(HUD_COLOR, int(g_fRightAlpha)));
				
		//fade the menu
		m_pBitmapLevelFull->GetBitmap()->setColor(Color(HUD_COLOR, int(g_fMenuAlpha)));
		m_pBitmapLevelEmpty->GetBitmap()->setColor(Color(HUD_COLOR, int(g_fMenuAlpha)));

		//don't draw the images if positions havne't been set
		if(	m_iHealthBarXpos == 0 )
		{
			m_pBitmapHealthFull->GetBitmap()->setColor(Color(HUD_COLOR, 255.0));
			m_pBitmapHealthEmpty->GetBitmap()->setColor(Color(HUD_COLOR, 255.0));
		}
		
		if(	m_iSlowMotionBarXpos == 0 )
		{
			m_pBitmapSlowMotionFull->GetBitmap()->setColor(Color(HUD_COLOR, 255.0));
			m_pBitmapSlowMotionEmpty->GetBitmap()->setColor(Color(HUD_COLOR, 255.0));
		}

			
 


		//change rain's alpha
		for( int i = 0; i < MAX_RAINDROP_TYPES; i++ )//TODO: cvar-ize MAX_RAINDROPS
		{
int time = i;
			//TODO: cvar-ize the following value

		//	cl_entity_t *player = gEngfuncs.GetLocalPlayer();
		//	if ( !player )
		//		return;

		//	if( player->angles[0] >= 3.0 )// facing down
		//	time = i;

			//increase the alpha to hide each image
			if( Raindrops[i].fRainDropAlpha < 255.0 )
			{
				Raindrops[i].fRainDropAlpha += (gHUD.m_flTimeDelta * time );
//				CONPRINT("increasing alpha\n" );
			}
	
			

	
			if ( CVAR_GET_FLOAT("show_rain") == 1 )
			{
			
					if( Raindrops[i].fRainDropAlpha >= 255.0 )//is this image hidden?
					{
						int raindrop_icon_w,  raindrop_icon_h;
						Raindrops[i].m_pRainDropImage[i]->m_pTGA->getSize(raindrop_icon_w, raindrop_icon_h);
							
					//	gEngfuncs.Con_Printf( "time: %i, angles %f\n", time, player->angles[0] );

						//hide, set new pos and make it visible
						Raindrops[i].m_pRainDropImage[i]->setVisible(false);
 						Raindrops[i].m_pRainDropImage[i]->setPos(gEngfuncs.pfnRandomLong( 1, ScreenWidth - raindrop_icon_w ), gEngfuncs.pfnRandomLong( 1, ScreenHeight - raindrop_icon_h ));

						Raindrops[i].fRainDropAlpha = 222.0;//0.0 ?
							
					//	CONPRINT("changing position and resetting alpha\n" );
					}
				
			}
								
			if( Raindrops[i].fRainDropAlpha <= 0.0 )
			Raindrops[i].fRainDropAlpha = 222.0;//0.0 ?

			//set alpha to each image
			Raindrops[i].m_pRainDropImage[i]->m_pTGA->setColor(Color(255,255,255, int(Raindrops[i].fRainDropAlpha)));
		}
				

		for( int p = 0; p < MAX_BLOOD_TYPES; p++ )//TODO: cvar-ize MAX_BLOOD_TYPES
		{
			//TODO: cvar-ize the following value
			int time = p * 10;//20

			if ( time <= 0 )
			time = 20;//CHECK ME

			//increase the alpha to hide each image
			if( Bloods[p].fBloodAlpha < 255.0 )
			{
				Bloods[p].fBloodAlpha += (gHUD.m_flTimeDelta * time );
				gEngfuncs.Cvar_SetValue( "hurt", 0 );
			}

			//set alpha to each image
			Bloods[p].m_pBloodImage[p]->m_pTGA->setColor(Color(255,255,255, int(Bloods[p].fBloodAlpha)));
		}
	
		if ( CVAR_GET_FLOAT("hurt") == 1 )
		{
			for( int i = 0; i < MAX_BLOOD_TYPES; i++ )
			{
				int blood_icon_w,  blood_icon_h;
				Bloods[i].m_pBloodImage[i]->m_pTGA->getSize(blood_icon_w, blood_icon_h);
				
				//hide, set new pos and make it visible
				Bloods[i].m_pBloodImage[i]->setVisible(false);

 				Bloods[i].m_pBloodImage[i]->setPos(gEngfuncs.pfnRandomLong( 1, ScreenWidth - blood_icon_w ), gEngfuncs.pfnRandomLong( 1, ScreenHeight - blood_icon_h ));

				Bloods[i].fBloodAlpha = 0.0;//0.0 ?
			}
			gEngfuncs.Cvar_SetValue( "hurt", 0 );
		}
		
//////////////////
		//TODO: check for overlays and resolutions
		//binoculars, gas_mask, nvg, zoom, head_bag
		if ( CVAR_GET_FLOAT("cl_screenoverlays") == 1 )
		{
			if ( CVAR_GET_FLOAT("show_mask") == 1 )
			{
				BitmapTGA* pImg_GasMask = FindAmmoImageForWeapon("gas_mask");

				if (pImg_GasMask)
				{
					pImg_GasMask->setColor(Color(HUD_COLOR, 0 ));
					pImg_GasMask->setPos( 0, 0 );
					pImg_GasMask->doPaint(this);
				}
			}

			if ( CVAR_GET_FLOAT("show_head_bag") == 1 )
			{
				BitmapTGA* pImg_HeadBag = FindAmmoImageForWeapon("head_bag");

				if (pImg_HeadBag)
				{
					pImg_HeadBag->setColor(Color(HUD_COLOR, 0 ));
					pImg_HeadBag->setPos( 0, 0 );
					pImg_HeadBag->doPaint(this);
				}
			}

				
			BitmapTGA* pImg_IronCross = FindAmmoImageForWeapon("zoom");
			
			if (pImg_IronCross)
			{
				pImg_IronCross->setColor(Color(HUD_COLOR, int(g_fXhairAlpha) ));
				pImg_IronCross->setPos( 0, 0);
				pImg_IronCross->doPaint(this);
			}

			if( g_NVGOn )
			{
			/*	BitmapTGA* pImg_NVG_OV = FindAmmoImageForWeapon("nvg_overlay");

				if (pImg_NVG_OV)//if base_left
				{
				//	int image_w, image_h;

					#define NVG_COLOR 28,122,255

				//	pImg_NVG_OV->getSize(image_w, image_h);
					pImg_NVG_OV->setColor(Color(NVG_COLOR, 255 ));//do not show
					pImg_NVG_OV->setPos( 0, 0 );
					pImg_NVG_OV->doPaint(this);
				}*/
						
				BitmapTGA* pImg_NVG = FindAmmoImageForWeapon("nvg");

				if (pImg_NVG)//if base_left
				{
				//	int image_w, image_h;
				//	pImg_NVG->getSize(image_w, image_h);
					pImg_NVG->setColor(Color(HUD_COLOR, 0 ));
					pImg_NVG->setPos( 0, 0);
					pImg_NVG->doPaint(this);
				}
			}

		}











		//this shouldn't be used!
		BitmapTGA* pImg_MenuBase = FindAmmoImageForWeapon("menu_base");

		if (pImg_MenuBase)
		{
			pImg_MenuBase->setColor(Color(HUD_COLOR, int(g_fMenuAlpha) ));
			pImg_MenuBase->setPos( 0, 0 );
			pImg_MenuBase->doPaint(this);
		}

		BitmapTGA* pImg_MenuAvatar = FindAmmoImageForWeapon("menu_avatar_hero");

		if (pImg_MenuAvatar)
		{
			pImg_MenuAvatar->setColor(Color(HUD_COLOR, int(g_fMenuAlpha) ));
			pImg_MenuAvatar->setPos( 0, 0 );
			pImg_MenuAvatar->doPaint(this);
		}

		int iCurrentLevel = CVAR_GET_FLOAT("xp_points") / 100;
		int i_filled_level = CVAR_GET_FLOAT("xp_points") - (iCurrentLevel * 100);
/*
		if( iLevelNotified == iCurrentLevel )
		{
			//do nothing
		}
		else
		{
		//	gHUD.m_TextMessage.MsgFunc_TextMsg( NULL, strlen( szLevelText ) + 1, szLevelText );
			allowed_to_print_msg = TRUE;
			iTextLife = 1000;

			PlaySound("common/level_up.wav", 1);

			iLevelNotified = iCurrentLevel;
		}

		if( allowed_to_print_msg )
		{
			iTextLife -= (gHUD.m_flTimeDelta * 60);

			int itext_x = 20, itext_y = 400;
			int itext_width, itext_height;

			char szLevelText[64];		
			char *level_string = CHudTextMessage::BufferedLocaliseTextString( "#LEVEL_UP" );

			drawSetTextFont(m_pFontSystem);//set font for new numbers

			sprintf(szLevelText, "'%s' %s %i", CVAR_GET_STRING("name"), level_string, iCurrentLevel);//upgraded to level

			m_pFontSystem->getTextSize(szLevelText, itext_width, itext_height);//get size
  
			drawSetTextPos(itext_x, itext_y);//posisionate it
			drawSetTextColor(HUD_COLOR, 0.0); // alpha TEST
			drawPrintText(szLevelText, strlen(szLevelText));//draw it

			if( iTextLife <= 0 )
			allowed_to_print_msg = FALSE;
		}
*/
		if ( CVAR_GET_FLOAT( "cl_menu" ) != 0 )
		{
			if (g_fMenuAlpha > 10.0)
				g_fMenuAlpha -= (gHUD.m_flTimeDelta * 1500);

			if (g_fMenuAlpha < 10.0)
				g_fMenuAlpha = 10.0;

			UpdateLevel( i_filled_level );

			BitmapTGA* pImg_MenuLocked = FindAmmoImageForWeapon("menu_locked");

			if (pImg_MenuLocked)
			{
				pImg_MenuLocked->setColor(Color(HUD_COLOR, int(g_fMenuAlpha) ));
				pImg_MenuLocked->setPos( 0, 500 );
				pImg_MenuLocked->doPaint(this);
			}

			BitmapTGA* pImg_MenuLocked2 = FindAmmoImageForWeapon("menu_locked");

			if (pImg_MenuLocked2)
			{
				pImg_MenuLocked2->setColor(Color(HUD_COLOR, int(g_fMenuAlpha) ));
				pImg_MenuLocked2->setPos( 160, 500 );
				pImg_MenuLocked2->doPaint(this);
			}

		//draw TEXT
			int itext_x, itext_y;

			itext_x = 100, itext_y = 60;

			char szLevelText[64];		
//			int itext_width, itext_height;

			char *player_name = CHudTextMessage::BufferedLocaliseTextString( "#PLAYER_HUD" );

			drawSetTextFont(m_pFontSystem);//set font for new numbers
			drawSetTextColor(HUD_COLOR, int(g_fMenuAlpha)); // alpha TEST

			sprintf(szLevelText, "'%s' as %s", CVAR_GET_STRING("name"), player_name);
		
			drawSetTextPos(itext_x, itext_y);//posisionate it
			drawPrintText(szLevelText, strlen(szLevelText));//draw it


			itext_x = 100, itext_y = 120;
					
			sprintf(szLevelText, "Level : %i", iCurrentLevel);
		
			drawSetTextPos(itext_x, itext_y);//posisionate it
			drawPrintText(szLevelText, strlen(szLevelText));//draw it

			itext_x = 100, itext_y = 140;
					
			sprintf(szLevelText, "Experience : %i/100", i_filled_level);
		
			drawSetTextPos(itext_x, itext_y);//posisionate it
			drawPrintText(szLevelText, strlen(szLevelText));//draw it


			itext_x = 100, itext_y = 180;
		
			sprintf(szLevelText, "Total Kills : %i", int(CVAR_GET_FLOAT("score_killed")) );
		
			drawSetTextPos(itext_x, itext_y);//posisionate it
			drawPrintText(szLevelText, strlen(szLevelText));//draw it
						
			itext_x = 100, itext_y = 200;

			sprintf(szLevelText, "         Headshots : %i", int(CVAR_GET_FLOAT("score_head")) );
		
			drawSetTextPos(itext_x, itext_y);//posisionate it
			drawPrintText(szLevelText, strlen(szLevelText));//draw it

			itext_x = 100, itext_y = 220;

			sprintf(szLevelText, "         Knifed : %i", int(CVAR_GET_FLOAT("score_knifed")) );
		
			drawSetTextPos(itext_x, itext_y);//posisionate it
			drawPrintText(szLevelText, strlen(szLevelText));//draw it

			itext_x = 100, itext_y = 240;

			sprintf(szLevelText, "Unlocked Pages : %i", int(CVAR_GET_FLOAT("score_pages")) );
		
			drawSetTextPos(itext_x, itext_y);//posisionate it
			drawPrintText(szLevelText, strlen(szLevelText));//draw it
		}
		else
		{
			if (g_fMenuAlpha < 255.0)
				g_fMenuAlpha += (gHUD.m_flTimeDelta * 500);

			if (g_fMenuAlpha > 255.0)
				g_fMenuAlpha = 255.0;
		}
				

//HUD elements

		for( int z = -1; z<5; z++ )
		{
			BitmapTGA* pImg_SoundLevel[5];
			char szImagename[32];

			sprintf ( szImagename, "sound%i", z );
			pImg_SoundLevel[z] = FindAmmoImageForWeapon(szImagename);
				
			if (pImg_SoundLevel[z])
			{
				int image_w, image_h;
				pImg_SoundLevel[z]->getSize(image_w, image_h);

				if( z == 0 && (CVAR_GET_FLOAT("sound_level") < 200) )
				pImg_SoundLevel[z]->setColor(Color(255,255,255, int(g_fRightAlpha) ));
				else if( z == 1 && (CVAR_GET_FLOAT("sound_level") >= 200) )
				pImg_SoundLevel[z]->setColor(Color(255,255,255, int(g_fRightAlpha) ));
				else if( z == 2 && (CVAR_GET_FLOAT("sound_level") >= 500) )
				pImg_SoundLevel[z]->setColor(Color(255,255,255, int(g_fRightAlpha) ));
				else if( z == 3 && (CVAR_GET_FLOAT("sound_level") >= 1000) )
				pImg_SoundLevel[z]->setColor(Color(255,255,255, int(g_fRightAlpha) ));
				else if( z == 4 && (CVAR_GET_FLOAT("sound_level") >= 1800) )
				pImg_SoundLevel[z]->setColor(Color(255,255,255, int(g_fRightAlpha) ));
				else
				{
					pImg_SoundLevel[z]->setColor(Color(255,255,255, 255 ));//hidden
				}
				pImg_SoundLevel[z]->setPos( ScreenWidth/2 - image_w/2, ScreenHeight - image_h);
				pImg_SoundLevel[z]->doPaint(this);
			}
		}

		if( gRadarIsOn )
		{
			BitmapTGA* pImg_Radar = FindAmmoImageForWeapon("radar");

			if (pImg_Radar)//if base_left
			{
				int image_left_w, image_left_h;
				pImg_Radar->getSize(image_left_w, image_left_h);
				pImg_Radar->setColor(Color(HUD_COLOR, 100 ));
				pImg_Radar->setPos( ScreenWidth/2 - image_left_w/2, ScreenHeight/2 - image_left_h/2);
				pImg_Radar->doPaint(this);
			}
		}




		BitmapTGA* pImg_IronCrosshairZoom = FindAmmoImageForWeapon("crosshair_zoom");
		
		if (pImg_IronCrosshairZoom)
		{
			pImg_IronCrosshairZoom->setColor(Color(HUD_COLOR, int(g_fXhairAlpha) ));

			int image_left_w, image_left_h;
			pImg_IronCrosshairZoom->getSize(image_left_w, image_left_h);

			pImg_IronCrosshairZoom->setPos( ScreenWidth/2 - image_left_w/2, ScreenHeight/2 - image_left_h/2);
			pImg_IronCrosshairZoom->doPaint(this);
		}

		if ( CVAR_GET_FLOAT( "hud_newcross") == 2 )
		{
			if (g_fXhairAlpha > 0.0)
				g_fXhairAlpha -= (gHUD.m_flTimeDelta * 1500);

			if (g_fXhairAlpha < 0.0)
				g_fXhairAlpha = 0.0;

		//	pImg_IronCross->setColor(Color(HUD_COLOR, int(g_fXhairAlpha) ));
		//	pImg_IronCross->setPos( 0, 0);
		//	pImg_IronCross->doPaint(this);
		}
		else
		{
			if (g_fXhairAlpha < 255.0)
				g_fXhairAlpha += (gHUD.m_flTimeDelta * 1500);

			if (g_fXhairAlpha > 255.0)
				g_fXhairAlpha = 255.0;
		}	


		BitmapTGA* pImg_Binoculars = FindAmmoImageForWeapon("binoculars");
	
		if ( CVAR_GET_FLOAT("show_binoculars") == 1 )
		if (pImg_Binoculars)
		{
			pImg_Binoculars->setColor(Color(HUD_COLOR, 0 ));
			pImg_Binoculars->setPos( 0, 0);
			pImg_Binoculars->doPaint(this);
		}

/*		BitmapTGA* pImg_Noise = FindAmmoImageForWeapon("noise");
	
		if (pImg_Noise)
		{
			pImg_Noise->setColor(Color(HUD_COLOR, 248 ));
			pImg_Noise->setPos( 0, 0);
			pImg_Noise->doPaint(this);
		}*/
		

/*
		BitmapTGA* pImg_XHair1 = FindAmmoImageForWeapon("crosshair1");

		if ( CVAR_GET_FLOAT( "in_reload" ) == 0 )
		if ( CVAR_GET_FLOAT( "crosshair" ) == 1 )
		if ( CVAR_GET_FLOAT( "cl_aim_mode" ) == 0 )
		if ( CVAR_GET_FLOAT( "hud_newcross" ) != 0 )
		if (pImg_XHair1)
		{
			int image_left_w, image_left_h;
			pImg_XHair1->getSize(image_left_w, image_left_h);


//			int iAlpha = (int)CVAR_GET_FLOAT("sound_level") * 255 / 2000;//2000 is the maximun sound level value
			int iAlpha = 0;
					
			if ( CVAR_GET_FLOAT( "hud_newcross_size" ) == 0 )//no transparent cross
			iAlpha = 255;
				
			if ( CVAR_GET_FLOAT( "hud_newcross_size" ) == 1 )//small | mayor
			iAlpha = 0;
			
			if ( CVAR_GET_FLOAT( "hud_newcross_size" ) == 2 )//medium
			iAlpha = 50;
		
			if ( CVAR_GET_FLOAT( "hud_newcross_size" ) == 3 )//large
			iAlpha = 100;
			
			if ( CVAR_GET_FLOAT( "hud_newcross_size" ) == 4 )//extra large
			iAlpha = 150;
		
			if ( CVAR_GET_FLOAT( "hud_newcross_size" ) >= 5 )//extra extra large
			iAlpha = 200;

			pImg_XHair1->setColor(Color(HUD_COLOR, iAlpha ));

			pImg_XHair1->setPos( ScreenWidth/2 - image_left_w/2, ScreenHeight/2 - image_left_h/2);
			pImg_XHair1->doPaint(this);
		}
*/


/*		if ( CVAR_GET_FLOAT( "cl_menu" ) == 0 )
		{


			BitmapTGA* pImg_Logo = FindAmmoImageForWeapon("logo");

			if (pImg_Logo)//if base_left
			{
				int image_w, image_h;
				pImg_Logo->getSize(image_w, image_h);
				pImg_Logo->setColor(Color(HUD_COLOR, 200 ));
				pImg_Logo->setPos( 0, 0);
				pImg_Logo->doPaint(this);
			}
		}*/
				
		if ( CVAR_GET_FLOAT( "cl_hud_mode" ) == 0 )
		{
			int x = ScreenWidth - HEALTH_RIGHT_OFFSET;
			int y = ScreenHeight - HEALTH_DOWN_OFFSET;
			
			BitmapTGA* pImg_BaseLeft = FindAmmoImageForWeapon("base_left");

			if (pImg_BaseLeft)//if base_left
			{
				int image_left_w, image_left_h;
				pImg_BaseLeft->getSize(image_left_w, image_left_h);
				pImg_BaseLeft->setColor(Color(HUD_COLOR, int(g_fLeftAlpha)));
				pImg_BaseLeft->setPos( HEALTH_RIGHT_OFFSET, y - image_left_h );
				pImg_BaseLeft->doPaint(this);
					
				m_iHealthBarXpos = HEALTH_RIGHT_OFFSET + image_left_w - m_iHealthBarWidth;
				m_iHealthBarYpos = y - (image_left_h/2) - (m_iHealthBarHeight /2);

				BitmapTGA* pImg_Armor = FindAmmoImageForWeapon("armor");

				if (pImg_Armor)//if base_left
				{
					int icon_armor_w, icon_armor_h, icon_armor_pos_x, icon_armor_pos_y;
					int r,g,b;

					pImg_Armor->getSize(icon_armor_w, icon_armor_h);

					if( armor  >=40 )
					{
						r= 255;
						g= 255;
						b= 255;
					}
					if( armor < 40 )
					{
						r=255;
						g= 0;
						b= 0;
					}
					if( armor <= 0 )
					{
						r=0;
						g= 0;
						b= 0;
					}

					pImg_Armor->setColor(Color(r,g,b, int(g_fLeftAlpha)));

					icon_armor_pos_x = HEALTH_RIGHT_OFFSET + (HEALTH_RIGHT_OFFSET/2)+XRES(5);
					icon_armor_pos_y = y - (image_left_h/2) - (icon_armor_h/2);

					pImg_Armor->setPos( icon_armor_pos_x, icon_armor_pos_y );
					pImg_Armor->doPaint(this);
				}
			}


			if (pw)
			{
				// Do we have secondary ammo?
				// NOT USED
				// NOT USED
				if ((pw->iAmmo2Type > 0) && (gWR.CountAmmo(pw->iAmmo2Type) > 0))
				{
					// Draw the secondary ammo Icon
					char buf[256];
					sprintf(buf, "%s_sec", pw->szName);
					BitmapTGA* pImg = FindAmmoImageForWeapon(buf);
					if (pImg)
					{
						int iw, ih;
						pImg->getSize(iw, ih);
						x -= iw;
						pImg->setColor(Color(HUD_COLOR, 0)); // TEST
						pImg->setPos(x, y - ih);
						pImg->doPaint(this);
						x -= XRES(6); // make some space between icon and text
						y -= 10;
					}

					// draw ammo count string
					int tw, th;				
					sprintf(buf, "%d", gWR.CountAmmo(pw->iAmmo2Type));

					m_pFontDigits->getTextSize(buf, tw, th);
					drawSetTextFont(m_pFontDigits);
					drawSetTextColor(HUD_COLOR_PRESSED, 0); // TEST
					drawSetTextPos(x - tw, y - th);
					drawPrintText(buf, strlen(buf));
					x = ScreenWidth - HEALTH_RIGHT_OFFSET - XRES(100);
				}
				// NOT USED
				// NOT USED

				if (pw->iAmmoType > 0)
				{
					y = ScreenHeight - HEALTH_DOWN_OFFSET;

					// Draw the ammo Icon
					BitmapTGA* pImg_BaseRight = FindAmmoImageForWeapon("base_right");// pw->szName);
					BitmapTGA* pImgIconArmor = FindAmmoImageForWeapon("base_left");
					BitmapTGA* pImgIconAmmo = FindAmmoImageForWeapon( "ammo_def" );//mmmm... we don't have an ammo icon for this weapon!

					for( int i = 0; i <MAX_WEAPONS_ICONIZED; i++ )
					{
						if(	strcmp(PlayerWeapons[i].szWeaponName, pw->szName) == 0 )//the current weapon match with some of the iconized weapons
						{
							pImgIconAmmo = FindAmmoImageForWeapon( PlayerWeapons[i].szAmmoIcon );
									
						//	CONPRINT("pw->szName: %s - PlayerWeapons[].szWeaponName[%s] - szAmmoIcon: %s\n", pw->szName, PlayerWeapons[i].szWeaponName, PlayerWeapons[i].szAmmoIcon );
						}
					}


					if (pImg_BaseRight)//if base_right
					{
						float fPer = float(pw->iClip) * 100.0 / CVAR_GET_FLOAT("clip_size");	
//						float fPer = float(pw->iClip) * 100.0 / gWR.CountAmmo(pw->iAmmoType);	

						UpdateSlowMotion( int(fPer) );

						//show base right
						int image_w, image_h;
						int icon_w, icon_h;

						pImg_BaseRight->getSize(image_w, image_h);
						pImg_BaseRight->setColor(Color(HUD_COLOR, int(g_fRightAlpha))); // TEST
						pImg_BaseRight->setPos(x- image_w, y - image_h);
						pImg_BaseRight->doPaint(this);

						//show icon for ammo
						if ( pImgIconAmmo )
						{
							pImgIconAmmo->getSize(icon_w, icon_h);
							pImgIconAmmo->setColor(Color(HUD_COLOR, int(g_fRightAlpha)));
							pImgIconAmmo->setPos( x-XRES(5) - (HEALTH_RIGHT_OFFSET /2) - icon_w, y - (image_h /2) - (icon_h /2));//center that bitch
							pImgIconAmmo->doPaint(this);
						}

						m_iSlowMotionBarXpos = x - image_w;
						m_iSlowMotionBarYpos = y - (image_h /2) - (m_iSlowMotionBarHeight /2);


					}	
					if (pw->iClip < 0) // has no clip?
					{
						m_pBitmapSlowMotionFull->GetBitmap()->setColor(Color(HUD_COLOR, 255));
						m_pBitmapSlowMotionFlash->GetBitmap()->setColor(Color(HUD_COLOR, 255));
						m_pBitmapSlowMotionEmpty->GetBitmap()->setColor(Color(HUD_COLOR, 255));
					}
				}
				else//KNIFE - PDA - ETC
				{
					m_pBitmapSlowMotionFull->GetBitmap()->setColor(Color(HUD_COLOR, 255));
					m_pBitmapSlowMotionFlash->GetBitmap()->setColor(Color(HUD_COLOR, 255));
					m_pBitmapSlowMotionEmpty->GetBitmap()->setColor(Color(HUD_COLOR, 255));
				}
			}

	//NEW NUMBERS
			drawSetTextFont(m_pFontSystem);//set font for new numbers

	//draw XP
			 x = ScreenWidth - HEALTH_RIGHT_OFFSET;
			 y = 0;
				
			int health_x = ScreenWidth - HEALTH_RIGHT_OFFSET;
			int health_y = 0;

			char szHealth[128];
			
			//Draw health number
			int itext_width, itext_height;
			drawSetTextColor(HUD_COLOR_PRESSED, int(g_fLeftAlpha)); //HUD_COLOR change colour

			sprintf(szHealth, "%i", health);
			m_pFontSystem->getTextSize(szHealth, itext_width, itext_height);

			health_x = m_iHealthBarXpos/* + (m_iHealthBarWidth )*/ + XRES(5);
			health_y = m_iHealthBarYpos + (m_iHealthBarHeight /2) - (itext_height /2);//center these numbers following image's size

			drawSetTextPos( health_x, health_y);//posisionate it
			drawPrintText(szHealth, strlen(szHealth));//draw it

			//draw ammo count
			if (pw)
			{
				if (pw->iAmmoType > 0)
				{
					int tw, th;
					char buf[256];					
					drawSetTextColor(HUD_COLOR_PRESSED, int(g_fRightAlpha)); //HUD_COLOR alpha TEST

					sprintf(buf, "%d", gWR.CountAmmo(pw->iAmmoType));
					m_pFontSystem->getTextSize(buf, tw, th);
					
					x = m_iSlowMotionBarXpos - tw - XRES(5);
					y = m_iSlowMotionBarYpos + m_iSlowMotionBarHeight/2 - th /2;

					drawSetTextPos(x, y); 
					drawPrintText(buf, strlen(buf));
				}
			}
		}//EOF MENU TYPE
		else
		{
			m_pBitmapHealthFull->GetBitmap()->setColor(Color(HUD_COLOR, 255));
			m_pBitmapHealthFlash->GetBitmap()->setColor(Color(HUD_COLOR, 255));
			m_pBitmapHealthEmpty->GetBitmap()->setColor(Color(HUD_COLOR, 255));

			m_pBitmapSlowMotionFull->GetBitmap()->setColor(Color(HUD_COLOR, 255));
			m_pBitmapSlowMotionFlash->GetBitmap()->setColor(Color(HUD_COLOR, 255));
			m_pBitmapSlowMotionEmpty->GetBitmap()->setColor(Color(HUD_COLOR, 255));

//NEW NUMBERS
			drawSetTextFont(m_pFontHUD_BC);
			drawSetTextColor(250, 250, 250, 0); // TEST
				
			int health_x = HEALTH_RIGHT_OFFSET;
			int health_y = ScreenHeight - HEALTH_DOWN_OFFSET;

			char szLifeStats[128];
			
			//Draw LIFE ARMOR
			int itext_width, itext_height;
			drawSetTextColor(HUD_COLOR, 0);		
			
			sprintf(szLifeStats, "%i | %i", health, armor);

			m_pFontDigits->getTextSize(szLifeStats, itext_width, itext_height);

			drawSetTextPos( health_x, health_y - itext_height );//posisionate it
			drawPrintText(szLifeStats, strlen(szLifeStats));//draw it
/*
			sprintf(szLifeStats, "%i", health );

			m_pFontDigits->getTextSize("100", itext_width, itext_height);

			drawSetTextPos( health_x, health_y - itext_height );//posisionate it
			drawPrintText(szLifeStats, strlen(szLifeStats));//draw it

			health_x += itext_width;

			sprintf(szLifeStats, "|" );
			m_pFontDigits->getTextSize(szLifeStats, itext_width, itext_height);

			health_x += itext_width;

			drawSetTextPos( health_x, health_y - itext_height );//posisionate it
			drawPrintText(szLifeStats, strlen(szLifeStats));//draw it

			sprintf(szLifeStats, "%i", armor );
			health_x += XRES(12);
			m_pFontDigits->getTextSize(szLifeStats, itext_width, itext_height);
			drawSetTextPos( health_x, health_y - itext_height );//posisionate it
			drawPrintText(szLifeStats, strlen(szLifeStats));//draw it
*/
			//draw ammo count
			if (pw)
			{
				int x = ScreenWidth - HEALTH_RIGHT_OFFSET;
				int y = ScreenHeight - HEALTH_DOWN_OFFSET;

				// Do we have secondary ammo?
				if ((pw->iAmmo2Type > 0) && (gWR.CountAmmo(pw->iAmmo2Type) > 0))
				{
					// Draw the secondary ammo Icon
					
					char buf[256];/*
					sprintf(buf, "%s_sec", pw->szName);
					BitmapTGA* pImg = FindAmmoImageForWeapon(buf);
					if (pImg)
					{
						int iw, ih;
						pImg->getSize(iw, ih);
						x -= iw;
						pImg->setColor(Color(255, 255, 255, 0)); // TEST
						pImg->setPos(x, y - ih);
						pImg->doPaint(this);
						x -= XRES(6); // make some space between icon and text
						y -= 10;
					}*/

					// draw ammo count string
					int tw, th;				
					sprintf(buf, "%d", gWR.CountAmmo(pw->iAmmo2Type));

					m_pFontDigits->getTextSize(buf, tw, th);

					drawSetTextFont(m_pFontHUD_BC);
					drawSetTextColor(HUD_COLOR, 0);
					drawSetTextPos(x - tw, y - th);

					drawPrintText(buf, strlen(buf));
					x = ScreenWidth - HEALTH_RIGHT_OFFSET - XRES(100);
				}

				if (pw->iAmmoType > 0)
				{
					y = ScreenHeight - HEALTH_DOWN_OFFSET;

					// Draw the ammo Icon
					/*
					BitmapTGA* pImg = FindAmmoImageForWeapon(pw->szName);
					if (pImg)
					{
						int iw, ih;
						pImg->getSize(iw, ih);
						x -= iw;
						pImg->setColor(Color(255, 255, 255, 0)); // TEST
						pImg->setPos(x, y - ih);
						pImg->doPaint(this);
						x -= XRES(6); // make some space between icon and text
						y -= 10;
					}*/

					// draw ammo count string
					int tw, th;
					char buf[256];					
					drawSetTextFont(m_pFontHUD_BC);
					drawSetTextColor(250, 250, 250, 0); // TEST

					sprintf(buf, "%d", gWR.CountAmmo(pw->iAmmoType));
					m_pFontDigits->getTextSize(buf, tw, th);
					x -= tw; y -= th;
					drawSetTextPos(x, y); drawPrintText(buf, strlen(buf));

					if (pw->iClip >= 0) // has clip?
					{
						x -= YRES(12);
						drawSetTextPos(x, y); drawPrintText("/", 1);

						sprintf(buf, "%d", pw->iClip);
						m_pFontDigits->getTextSize(buf, tw, th);
						x = x - tw - YRES(5);
						drawSetTextPos(x, y); drawPrintText(buf, strlen(buf));
					}
				}
			}
		}

		//draw clock
		int stat_text_x = 0 + HEALTH_RIGHT_OFFSET;
		int stat_text_y = ScreenHeight / 2;

		char szTextStatusBuffer[64];		
		int istat_width, istat_height;

		if(g_iRoundtime > 0)
		{
			if( g_iRoundtime%60 < 10 )
			sprintf(szTextStatusBuffer, "%i:0%i", g_iRoundtime/60, g_iRoundtime%60);
			else
			sprintf(szTextStatusBuffer, "%i:%i", g_iRoundtime/60, g_iRoundtime%60);
		}
		else if ( g_iRoundtime <= g_iRoundtime *25.0/100.0 )
		{
			sprintf(szTextStatusBuffer, "0:00");
		}
		else if ( g_iRoundtime == -1)
		{
			sprintf(szTextStatusBuffer, "ERROR");
		}

		m_pFontDigits->getTextSize(szTextStatusBuffer, istat_width, istat_height);//get size
  
		stat_text_y -= istat_height/2;

		drawSetTextPos(stat_text_x, stat_text_y);//posisionate it
				
		drawSetTextColor(HUD_COLOR, 100); // alpha TEST

		if ( g_iRoundtime > g_iRoundtime *25.0/100.0 )
		drawPrintText(szTextStatusBuffer, strlen(szTextStatusBuffer));//draw it
	}

	m_pHudWeaponsSelPanel->setVisible( true );

	if (gHUD.m_iHideHUDDisplay & (HIDEHUD_ALL))
		m_pHudWeaponsSelPanel->setVisible( false );
	
	if ( gHUD.m_fPlayerDead || (gHUD.m_iHideHUDDisplay & (HIDEHUD_WEAPONS | HIDEHUD_ALL)) )
		m_pHudWeaponsSelPanel->setVisible( false );
	
	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
		m_pHudWeaponsSelPanel->setVisible( false );

	if (CVAR_GET_FLOAT("hud_draw") == 0)	
		m_pHudWeaponsSelPanel->setVisible( false );

	if ( m_pHudWeaponsSelPanel->isVisible() )
		m_pHudWeaponsSelPanel->Update();

	Panel::paint();
}


BitmapTGA* CHud2::FindAmmoImageForWeapon(const char *wpn)
{
	for (int i = 0; i < NUM_WEAPON_ICONS; i++)
	{
		if (!strcmp(wpn, weaponNames[i]))
			return m_pWeaponIconsArray[i];
	}
	return NULL;
}


void CHud2::UpdateHealth( int newhealth )
{
	if (newhealth == health)
		return;

	if (health == -1) // first update, dont do effects
	{
		health = newhealth;
		m_fHealthUpdateTime = -666;
		return;
	}

	oldhealth = health;
	health = newhealth;
	m_fHealthUpdateTime = gEngfuncs.GetClientTime();
	g_fLeftAlpha = 0.0;//fully solid
}

void CHud2::UpdateArmor( int newarmor )
{
	if (newarmor == armor)
		return;

	if (armor == -1) // first update, dont do effects
	{
		armor = newarmor;
		m_fArmorUpdateTime = -666;
		return;
	}

	oldarmor = armor;
	armor = newarmor;
	m_fArmorUpdateTime = gEngfuncs.GetClientTime();
	g_fLeftAlpha = 0.0;//fully solid
}
void CHud2::UpdateSlowMotion( int newslowmotion )
{
	if (newslowmotion == slowmotion)
		return;

	if (slowmotion == -1) // first update, dont do effects
	{
		slowmotion = newslowmotion;
		m_fSlowMotionUpdateTime = -666;
		return;
	}

	oldslowmotion = slowmotion;
	slowmotion = newslowmotion;
	m_fSlowMotionUpdateTime = gEngfuncs.GetClientTime();
}
void CHud2::UpdateLevel( int newlevel )
{
	if (newlevel == level)
		return;

	if (level == -1) // first update, dont do effects
	{
		level = newlevel;
		m_fLevelUpdateTime = -666;
		return;
	}

	oldlevel = level;
	level = newlevel;
	m_fLevelUpdateTime = gEngfuncs.GetClientTime();
}


// ======== Séléction des armes =========================
// ======================================================

CWeaponsSelPanel :: CWeaponsSelPanel( int x, int y, int wide, int tall ) : Panel( x, y, wide, tall )
{
}

CWeaponsSelPanel :: ~CWeaponsSelPanel( void )
{
}

void	CWeaponsSelPanel :: Initialize( void )
{
	char	tganame[ 32 ];

	int		i, k, l;

	for( i = 0; i < MAX_WEAPON_POSITIONS; i++ )
	{
		m_pSlotPreWpLabel	[ i ]	=	new CImageLabel( "hud_slot_button_0", 0, 32 + i * 45 );

		m_pSlotPreWpLabel[ i ]->setFgColor	( 0, 0, 0, 255 );
		m_pSlotPreWpLabel[ i ]->setBgColor	( 0, 0, 0, 255 );

		m_pSlotPreWpLabel[ i ]->setParent	( this );

		m_pSlotPreWpLabel[ i ]->setContentAlignment		( vgui::Label::a_west );

		m_pSlotPreWpLabel[ i ]->setBounds	( 0, 32 + i * 45, 32, 45);
		m_pSlotPreWpLabel[ i ]->setVisible	( false );
	}

	for( k = 0; k < MAX_WEAPON_SLOTS; k++ )
	{
		sprintf( tganame, "hud_slot_button_%i", k + 1 );
		m_pSlotNumberLabel[ k ]	= new CImageLabel( tganame, 0, 0 );

		// Pour la transparence
		m_pSlotNumberLabel[ k ]->setFgColor	( 0, 0, 0, 255 );
		m_pSlotNumberLabel[ k ]->setBgColor	( 0, 0, 0, 255 );

		// No comment
		m_pSlotNumberLabel[ k ]->setParent	( this );
		m_pSlotNumberLabel[ k ]->setContentAlignment	( vgui::Label::a_west );

		// On est obligé de faire un setbounds sinon les images ne s'affichent pas
		// au bon endroit suivant les résolution, bizarre, mais bon c HL, fo pas trop lui
		// en demander il a déja du mal à afficher 100 polys qi en plus en lui demande
		// d'afficher les images au bon endroit il va saturer le pauvre
		m_pSlotNumberLabel[ k ]->setBounds	( 0, 0, 32, 32 );
		m_pSlotNumberLabel[ k ]->setVisible	( false );

		for( l = 0; l < MAX_WEAPON_POSITIONS; l++ )
		{
	//		sprintf( tganame, "hud_slot_%i_%i", k, l );
			sprintf( tganame, "hud_slot_%i_%i", k+1, l+1 );
			m_pSlotWeaponLabel	[ k ][ l ]	=	new CImageLabel( tganame, 32, 32 + l * 45 );

			m_pSlotWeaponLabel	[ k ][ l ]->setFgColor	( 0, 0, 0, 255 );
			m_pSlotWeaponLabel	[ k ][ l ]->setBgColor	( 0, 0, 0, 255 );

			m_pSlotWeaponLabel	[ k ][ l ]->setParent	( this );

			m_pSlotWeaponLabel	[ k ][ l ]->setContentAlignment		( vgui::Label::a_west );

			m_pSlotWeaponLabel	[ k ][ l ]->setBounds	( 32, 32 + l * 45, 170, 45);
			m_pSlotWeaponLabel	[ k ][ l ]->setVisible	( false );
		}
	}

	for( k = 0; k < MAX_WEAPON_SLOTS; k++ )
	{
		for( l = 0; l < MAX_WEAPON_POSITIONS; l++ )
		{
//			sprintf( tganame, "hud_slot_sel_%i_%i", k, l );
			sprintf( tganame, "hud_slot_sel_%i_%i", k+1, l+1 );//usage slot NUMBER aka 1
			m_pSlotSelWeaponLabel	[ k ][ l ]	=	new CImageLabel( tganame, 32, 32 + l * 45 );

			m_pSlotSelWeaponLabel	[ k ][ l ]->setFgColor	( 0, 0, 0, 255 );
			m_pSlotSelWeaponLabel	[ k ][ l ]->setBgColor	( 0, 0, 0, 255 );

			m_pSlotSelWeaponLabel	[ k ][ l ]->setParent	( this );

			m_pSlotSelWeaponLabel	[ k ][ l ]->setContentAlignment		( vgui::Label::a_west );

			m_pSlotSelWeaponLabel	[ k ][ l ]->setBounds	( 32, 32 + l * 45, 170, 45);
			m_pSlotSelWeaponLabel	[ k ][ l ]->setVisible	( false );
		}
	}
}


void	CWeaponsSelPanel :: Update( void )
{
	int		iActiveSlot;
	int		weappos	= 0;

	// Si le menu est pas ouvert alors rien du tout
	if ( !gpActiveSel || !gHUD.m_Ammo.m_pWeapon )
	{
		setVisible( false );

		return;
	}

	// Quel slot est actif ???
	if ( gpActiveSel == ( WEAPON * )1 )
		iActiveSlot = -1;
	else 
		iActiveSlot = gpActiveSel->iSlot;
	
	// On en a un actif
	if ( iActiveSlot > 0 )
	{
		// On regarde si y'a des armes dedans
		if ( !gWR.GetFirstPos( iActiveSlot ) )
		{
			gpActiveSel = (WEAPON *)1;
			iActiveSlot = -1;
		}
	}

	// On chope l'arme en première posistion
	WEAPON *p = gWR.GetFirstPos( iActiveSlot );

	// Y'en a une
	if ( p )
	{
		for( int k = 0; k < MAX_WEAPON_SLOTS; k++ )
		{
			// Numéro du slot
			if (  k == iActiveSlot )
				m_pSlotNumberLabel[k]->setVisible( true );
			else
			{
				m_pSlotNumberLabel[k]->setVisible( false );

				for( int l = 0; l < MAX_WEAPON_POSITIONS; l++ )
				{
					m_pSlotWeaponLabel	[ k ][ l ]->setVisible		( false );
					m_pSlotSelWeaponLabel	[ k ][ l ]->setVisible	( false );
				}
			}
		}

		// Les armes maintenant
		for ( int iPos = 0; iPos < MAX_WEAPON_POSITIONS; iPos++ )
		{
			p = gWR.GetWeaponSlot( iActiveSlot, iPos );

			// Si elle existe pas ou qu'elle a pas de munitions, on la passe
			if( CVAR_GET_FLOAT( "cl_hud_select_on_empty" ) == 0 )
			{
				if ( !p || !p->iId /*|| !gWR.HasAmmo( p )*/ )
				{
					m_pSlotSelWeaponLabel[ iActiveSlot ][ iPos ]->setVisible( false );
					m_pSlotWeaponLabel[ iActiveSlot ][ iPos ]->setVisible( false );

					continue;
				}
			}
			else
			{
				if ( !p || !p->iId /*|| !gWR.HasAmmo( p )*/ )
				{
					m_pSlotSelWeaponLabel[ iActiveSlot ][ iPos ]->setVisible( false );
					m_pSlotWeaponLabel[ iActiveSlot ][ iPos ]->setVisible( false );

					continue;
				}
			}

			// On affiche le dessin devant l'arme
			m_pSlotPreWpLabel[weappos]->setVisible( true );

			// On change la position des armes
			m_pSlotWeaponLabel[ p->iSlot ][ p->iSlotPos ]->setPos		( 32, 32 + weappos * 45 );
			m_pSlotSelWeaponLabel[ p->iSlot ][ p->iSlotPos ]->setPos	( 32, 32 + weappos * 45 );
			m_pSlotWeaponLabel[ p->iSlot ][ p->iSlotPos ]->setBounds	( 32, 32 + weappos * 45, 170, 45 );
			m_pSlotSelWeaponLabel[ p->iSlot ][ p->iSlotPos ]->setBounds	( 32, 32 + weappos * 45, 170, 45 );

			// On regarde quel image de l'arme on affiche
			
			if ( gpActiveSel == p )
			{
				m_pSlotSelWeaponLabel[ p->iSlot ][ p->iSlotPos ]->setVisible( true );
				m_pSlotWeaponLabel[ p->iSlot ][ p->iSlotPos ]->setVisible( false );
			}
			else
			{		
				m_pSlotWeaponLabel[ p->iSlot ][ p->iSlotPos ]->setVisible( true );
				m_pSlotSelWeaponLabel[ p->iSlot ][ p->iSlotPos ]->setVisible( false );
			}
								
			if( CVAR_GET_FLOAT( "cl_hud_select_on_empty" ) == 0 )
			if ( !gWR.HasAmmo(gWR.rgSlots[p->iSlot][p->iSlotPos]) || (gWR.rgSlots[p->iSlot][p->iSlotPos]->hNoAmmo != 0))
			{
				m_pSlotWeaponLabel[ p->iSlot ][ p->iSlotPos ]->m_pTGA->setColor(Color(255,0,0, 111));	
				m_pSlotSelWeaponLabel[ p->iSlot ][ p->iSlotPos ]->m_pTGA->setColor(Color(255,0,0, 111));
			}
			else//there's ammo, or switch-able or... whatever, give them a good damn white colour 'cause are selectable.
			{
				if(m_pSlotWeaponLabel[ p->iSlot ][ p->iSlotPos ]->m_pTGA)
				{
					m_pSlotWeaponLabel[ p->iSlot ][ p->iSlotPos ]->m_pTGA->setColor(Color( 255, 255, 255, 0 ));	
					m_pSlotSelWeaponLabel[ p->iSlot ][ p->iSlotPos ]->m_pTGA->setColor(Color( 255, 255, 255, 0 ));
				}
			}

			weappos++;
		}

		// On enlève les dessins à gauche qui servent à rien
		for( int i = weappos; i < MAX_WEAPON_POSITIONS; i++ )
			m_pSlotPreWpLabel[i]->setVisible	( false );
	}
	else
	{
		// Y'a pas d'arme donc on dessinne rien du tout
		for( int j = 0; j < MAX_WEAPON_POSITIONS; j++ )
			m_pSlotPreWpLabel[j]->setVisible	( false );

		for( int k = 0; k < MAX_WEAPON_SLOTS; k++ )
		{
			m_pSlotNumberLabel[k]->setVisible( false );

			for( int l = 0; l < MAX_WEAPON_POSITIONS; l++ )
			{
				m_pSlotWeaponLabel	[ k ][ l ]->setVisible		( false );
				m_pSlotSelWeaponLabel	[ k ][ l ]->setVisible	( false );
			}
		}
	}
}
//#endif