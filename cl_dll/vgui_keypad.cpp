#include "hud.h"
#include "cl_util.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_keypad.h"
#include "VGUI_LineBorder.h"
#include "VGUI_TextImage.h"
#include "../engine/keydefs.h"
#include "triangleapi.h"

#include "r_studioint.h"
#include "com_model.h"

#define SPACING 5

CKeypad::CKeypad() : Panel(0, 0, ScreenWidth, ScreenHeight)
{	
	setVisible(true);

//	setPaintBackgroundEnabled(true);
	setBgColor(0, 0, 0, 200);

	gViewPort->UpdateCursorState();

	iHidePassword=0;
//	iShowCables=0;

	return;
}

void CKeypad::Init()
{
	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hTextScheme = pSchemes->getSchemeHandle( "LCDDot" );
	Font *pLCDDot = pSchemes->getFont( hTextScheme );

	iPressedNumbers = 0;
	fKeyPadAlpha = ALPHA_INVISIBLE;
//	iShowCables = 1;

	if(iShowCables==1)
	iCodeNumbers = 3;
	else
	iCodeNumbers = 4;

	int tga_size_x, tga_size_y;

	if(iShowCables==1)
	{
	mbutton_cable_red	= new CMyKeypadButton("",  0,  0, 0, ALPHA_VISIBLE);
	mbutton_cable_blue	= new CMyKeypadButton("",  0,  0, 0, ALPHA_VISIBLE);
	mbutton_cable_green	= new CMyKeypadButton("",  0,  0, 0, ALPHA_VISIBLE);

	if( mbutton_cable_red )
	{
		mbutton_cable_red->setParent(this);
		
		mbutton_cable_red->m_pTGA = vgui_LoadTGA("gfx/vgui/cables_on.tga");
		mbutton_cable_red->setImage(mbutton_cable_red->m_pTGA);

 		mbutton_cable_red->m_pTGA->getSize( tga_size_x, tga_size_y );
		mbutton_cable_red->setSize( tga_size_x, tga_size_y );

		mbutton_cable_red->addActionSignal(new CKeypadHandler_Command(this,0) );

		mbutton_cable_red->setPos( 0, 0 );
	}

	mbutton_cable_blue->setParent(this);	
	mbutton_cable_blue->m_pTGA = vgui_LoadTGA("gfx/vgui/cables_on.tga");
	mbutton_cable_blue->setImage(mbutton_cable_blue->m_pTGA);
	mbutton_cable_blue->m_pTGA->getSize( tga_size_x, tga_size_y );
	mbutton_cable_blue->setSize( tga_size_x, tga_size_y );
	mbutton_cable_blue->setPos( 0, 60 );
	mbutton_cable_blue->addActionSignal(new CKeypadHandler_Command(this,1) );

	mbutton_cable_green->setParent(this);
	mbutton_cable_green->m_pTGA = vgui_LoadTGA("gfx/vgui/cables_on.tga");
	mbutton_cable_green->m_pTGA->getSize( tga_size_x, tga_size_y );
	mbutton_cable_green->setSize( tga_size_x, tga_size_y );
	mbutton_cable_green->setImage(mbutton_cable_green->m_pTGA);
	mbutton_cable_green->setPos( 0, 120 );
	mbutton_cable_green->addActionSignal(new CKeypadHandler_Command(this,2) );

		int total_items_height = tga_size_y*3;

		int items_y = ScreenHeight / 2 - total_items_height / 2;
		int items_x = ScreenWidth / 2 - tga_size_x / 2;

		mbutton_cable_red->setPos( items_x, items_y );
		mbutton_cable_blue->setPos( items_x, items_y + tga_size_y );
		mbutton_cable_green->setPos( items_x, items_y + tga_size_y *2);
	}


	if(iShowCables==1)
	return;

	my_wallpaper	= new CImageLabel( "", 0, ALPHA_INVISIBLE );
	
	if(my_wallpaper)
	{
		my_wallpaper->setParent(this);

		my_wallpaper->m_pTGA = vgui_LoadTGA("gfx/vgui/keypad_base.tga");

		if ( my_wallpaper->m_pTGA )
		{
			my_wallpaper->setImage(my_wallpaper->m_pTGA);
	
			my_wallpaper->setPaintBackgroundEnabled(false);//TRUE FOR DEBUG PURPOSE

 			my_wallpaper->m_pTGA->getSize( tga_size_x, tga_size_y );
			my_wallpaper->setSize( tga_size_x, tga_size_y );

			my_wallpaper->setPos( ScreenWidth/2 - tga_size_x/2, ScreenHeight/2 - tga_size_y/2 );
				
		//	my_wallpaper->m_pTGA->setColor( Color(255,255,255, 11) );
		}
	}


//CUSTOM BUTTONS & LABELS - INITIALIZATION

	mbutton_close	= new CMyKeypadButton("",  0,  0, 0, ALPHA_VISIBLE);
	mbutton_clear	= new CMyKeypadButton("",  0,  0, 0, ALPHA_VISIBLE);

	label				= new Label("", 0, 0, 0, ALPHA_VISIBLE);
	label_keypad_title	= new Label("", 0, 0, 0, ALPHA_VISIBLE);

//MAIN PANEL
	panel			= new Panel(0, 0, 0, ALPHA_VISIBLE);
//	panel			= new DragNDropPanel(0,0,0,0);
	panel_screen	= new Panel(0, 0, 0, ALPHA_VISIBLE);

	panel->setParent(this);
//	panel->setBorder(new LineBorder(1, Color(255, 255, 255, ALPHA_VISIBLE)));

	panel_screen->setParent(panel);
//	panel_screen->setBorder(new LineBorder(1, Color(255, 255, 255, ALPHA_VISIBLE)));

//	int tga_size_x, tga_size_y;


//	mbutton_cable_red_off	= new CMyKeypadButton("",  0,  0, 0, ALPHA_VISIBLE);



//LABELS

	if( label )
	{
		label->setParent(panel_screen);
		label->setFont(pLCDDot);
		label->setPaintBackgroundEnabled(false);//TRUE FOR DEBUG PURPOSE
		label->setContentFitted(true);//to fit the button to the current characters size

		label->setFgColor(255, 255, 255, ALPHA_VISIBLE);//this is useless because will be overidden
		
		label->setText( " " );

		label->setVisible(true);//to prevent show the button at the corner on the very first frame
	}

	if( label_keypad_title )
	{
		label_keypad_title->setParent(panel_screen);
		label_keypad_title->setFont(pLCDDot);
		label_keypad_title->setPaintBackgroundEnabled(false);
		label_keypad_title->setContentFitted(true);

		label_keypad_title->setFgColor(255, 255, 255, ALPHA_VISIBLE);
		
		label_keypad_title->setText( CHudTextMessage::BufferedLocaliseTextString( szFrameLabel ) );//must contain # !

		label_keypad_title->setVisible(true);
	}



//init buttons
	char szImageName[64];

	for( int i = 0; i < 9; i++)
	{
		MenuButtons[i] = new CMyKeypadButton("",  0,  0, 0, ALPHA_VISIBLE);
		sprintf( szImageName, "gfx/vgui/keypad_%i.tga", i+1 );

		if( MenuButtons[i] )
		{
			MenuButtons[i]->setParent(panel);

			MenuButtons[i]->m_pTGA = vgui_LoadTGA(szImageName);
			MenuButtons[i]->setImage(MenuButtons[i]->m_pTGA);

 			MenuButtons[i]->m_pTGA->getSize( tga_size_x, tga_size_y );
			MenuButtons[i]->setSize( tga_size_x, tga_size_y );

			MenuButtons[i]->addActionSignal( new CKeypadHandler_Command(this,i) );
		}
	}

//create the panel
	int ibuttons_w, ibuttons_h;
	int ipanel_width, ipanel_height;
	int ipos_x, ipos_y;

	ibuttons_w = tga_size_x *3 + SPACING *2;
	ibuttons_h = tga_size_y *4 + SPACING *3;

	ipanel_width = ibuttons_w + tga_size_y;
	ipanel_height = ibuttons_h + tga_size_y * 2 + SPACING;

	panel->setSize( ipanel_width, ipanel_height);//size of the buttons, more or less
	panel->setPos( ScreenWidth/2 - ipanel_width /2, ScreenHeight/2 - ipanel_height/2);//centered

	ipos_x = (ipanel_width /2) - ibuttons_w/2;
	ipos_y = ipanel_height - tga_size_y /2 - ibuttons_w - tga_size_y - SPACING;

//resize screen panel
	int ipanel_screen_width, ipanel_screen_height;

	ipanel_screen_width = ibuttons_w;
	ipanel_screen_height = tga_size_y + SPACING;

	panel_screen->setSize( ipanel_screen_width, ipanel_screen_height);
	panel_screen->setPos(  ipos_x, ipos_y - ipanel_screen_height - SPACING *2);

//place the buttons
	MenuButtons[0]->setPos( ipos_x, ipos_y);
	MenuButtons[1]->setPos( ipos_x + tga_size_x + SPACING, ipos_y);
	MenuButtons[2]->setPos( ipos_x + tga_size_x *2 + SPACING *2, ipos_y);

	MenuButtons[3]->setPos( ipos_x, ipos_y + tga_size_y + SPACING);
	MenuButtons[4]->setPos( ipos_x + tga_size_x + SPACING, ipos_y + tga_size_y + SPACING);
	MenuButtons[5]->setPos( ipos_x + tga_size_x *2 + SPACING *2, ipos_y + tga_size_y + SPACING);

	MenuButtons[6]->setPos( ipos_x, ipos_y + tga_size_y *2+ SPACING *2 );
	MenuButtons[7]->setPos( ipos_x + tga_size_x + SPACING, ipos_y + tga_size_y *2 + SPACING *2 );
	MenuButtons[8]->setPos( ipos_x + tga_size_x *2 + SPACING *2, ipos_y + tga_size_y *2+ SPACING *2 );


	int itext_width, itext_height;

//place the labels
	label_keypad_title->getSize( itext_width, itext_height );
	label_keypad_title->setPos( ipanel_screen_width /2 - itext_width /2, 0);//keypad title

	label->setPos( 0, itext_height );//numbers entered



//CLOSE WINDOW BUTTON

	if( mbutton_close )
	{
		mbutton_close->setParent(panel);
		
		mbutton_close->m_pTGA = vgui_LoadTGA("gfx/vgui/keypad_close.tga");
		mbutton_close->setImage(mbutton_close->m_pTGA);

 		mbutton_close->m_pTGA->getSize( tga_size_x, tga_size_y );
		mbutton_close->setSize( tga_size_x, tga_size_y );

		mbutton_close->addActionSignal(new CKeypadHandler_Command(this,KEYPAD_CLOSE_ALL) );

		mbutton_close->setPos( ipanel_width/2 - tga_size_x/2 + SPACING + tga_size_x, ipanel_height - tga_size_y - tga_size_y/2 );
	}

//clear window button

	if( mbutton_clear )
	{
		mbutton_clear->setParent(panel);
		
		mbutton_clear->m_pTGA = vgui_LoadTGA("gfx/vgui/keypad_clear.tga");
		mbutton_clear->setImage(mbutton_close->m_pTGA);

 		mbutton_clear->m_pTGA->getSize( tga_size_x, tga_size_y );
		mbutton_clear->setSize( tga_size_x, tga_size_y );

		mbutton_clear->addActionSignal(new CKeypadHandler_Command(this,KEYPAD_CLEAR) );

		mbutton_clear->setPos( ipanel_width/2 - tga_size_x/2, ipanel_height - tga_size_y - tga_size_y/2 );
	}
}

void CKeypad::paint()
{
	if (fKeyPadAlpha > 0.0)
		fKeyPadAlpha -= (gHUD.m_flTimeDelta * 500);

	if (fKeyPadAlpha < 0.0)
		fKeyPadAlpha = 0.0;
/*	}
	else
	{
		if (fKeyPadAlpha < 255.0)
			fKeyPadAlpha += (gHUD.m_flTimeDelta * 500);

		if (fKeyPadAlpha > 255.0)
			fKeyPadAlpha = 255.0;
	}
		*/		

	if( iPressedNumbers == iCodeNumbers )
	{
		if (!strcmp(szEnteredNumbers, szCode ))//match!
		{
			gEngfuncs.pfnClientCmd( "code_match" );

			if(iShowCables==0)
			PlaySound("common/keypad_granted.wav", 1);

			CloseWindow();
		}
		else
		{
			gEngfuncs.pfnClientCmd( "code_dismatch" );
			
			if(iShowCables==0)
			PlaySound("common/keypad_error.wav", 1);
			CloseWindow();
		}
	}

	
	if(iShowCables==1)
	{
		if( mbutton_cable_red->isArmed() )
			mbutton_cable_red->m_pTGA->setColor( Color(255,0,0, mbutton_cable_red->iAlpha) );
		else
			mbutton_cable_red->m_pTGA->setColor( Color(255,255,255, mbutton_cable_red->iAlpha) );

		if( mbutton_cable_blue->isArmed() )
			mbutton_cable_blue->m_pTGA->setColor( Color(0,0,255, mbutton_cable_blue->iAlpha) );
		else
			mbutton_cable_blue->m_pTGA->setColor( Color(255,255,255, mbutton_cable_blue->iAlpha) );

		if( mbutton_cable_green->isArmed() )
			mbutton_cable_green->m_pTGA->setColor( Color(0,255,0, mbutton_cable_green->iAlpha) );
		else
			mbutton_cable_green->m_pTGA->setColor( Color(255,255,255, mbutton_cable_green->iAlpha) );
		return;
	}


	for( int i = 0; i < 9; i++)
	{	
		if ( MenuButtons[i]->isSelected() )
		MenuButtons[i]->m_pTGA->setColor( Color(0,111,0, MenuButtons[i]->iAlpha) );//FIXME! this should done on the paint() function!
		else
		MenuButtons[i]->m_pTGA->setColor( Color(255,255,255, MenuButtons[i]->iAlpha) );//FIXME! this should done on the paint() function!
	}
	
	my_wallpaper->m_pTGA->setColor( Color(255,255,255, fKeyPadAlpha) );




	panel->setBgColor(0, 0, 0, ALPHA_INVISIBLE);
	panel_screen->setBgColor(0, 111, 0, ALPHA_INVISIBLE);

	panel->setBorder(new LineBorder(2, Color(255, 255, 255, ALPHA_INVISIBLE)));
	panel_screen->setBorder(new LineBorder(1, Color(255, 255, 255, ALPHA_INVISIBLE)));






/*
  if( wasMouseDoublePressed(MOUSE_LEFT) )
  {
			PlaySound("common/keypad_error.wav", 1);

  }*/

	//just changes the colour to let the player know is going to close that
	if( mbutton_close->isArmed() )
		mbutton_close->m_pTGA->setColor( Color(255,0,0, mbutton_close->iAlpha) );
	else
		mbutton_close->m_pTGA->setColor( Color(255,255,255, mbutton_close->iAlpha) );


	mbutton_clear->m_pTGA->setColor( Color(255,255,255, mbutton_clear->iAlpha) );
}

void CKeypad::ActionSignal(int cmd)
{
	if( cmd < 9 )
	{
		char number[8];

		sprintf( number, "%i", cmd+1 );//we need the number as a string
		
		if(iShowCables==1)
		{
			//change image and change also message of the button so that, it won't be able to be pressed again
			if( cmd == 0 )
			{
				if(mbutton_cable_red->bActive)
				{
					mbutton_cable_red->m_pTGA = vgui_LoadTGA("gfx/vgui/cables_off.tga");
					mbutton_cable_red->setImage(mbutton_cable_red->m_pTGA);
					mbutton_cable_red->bActive = false;
				}
				else
				return;
			}
			else if( cmd == 1 )
			{
				if(mbutton_cable_blue->bActive)
				{
					mbutton_cable_blue->m_pTGA = vgui_LoadTGA("gfx/vgui/cables_off.tga");
					mbutton_cable_blue->setImage(mbutton_cable_blue->m_pTGA);
				//	mbutton_cable_blue->addActionSignal(new CKeypadHandler_Command(this,KEYPAD_CMD_NONE) );
					mbutton_cable_blue->bActive = false;
				}
				else
				return;
			}
			else if( cmd == 2 )
			{
				if(mbutton_cable_green->bActive)
				{
					mbutton_cable_green->m_pTGA = vgui_LoadTGA("gfx/vgui/cables_off.tga");
					mbutton_cable_green->setImage(mbutton_cable_green->m_pTGA);
				//	mbutton_cable_green->addActionSignal(new CKeypadHandler_Command(this,KEYPAD_CMD_NONE) );
					mbutton_cable_green->bActive = false;
				}
				else
				return;
			}
			
			PlaySound("common/cable_cut.wav", 1);
		}
		else
		PlaySound("common/keypad_click.wav", 1);

		iPressedNumbers++;//useful to close the window when 4 numbers are pressed

		//add the new number, otherwise, display the first one
		if(iPressedNumbers >1)
		strcat (szEnteredNumbers,number);
		else
		strcpy (szEnteredNumbers,number);
			
		if(iShowCables==1)
		return;

		label->setVisible(true);

		if( iHidePassword == 1 )
		{
			//so fucking lazy, blame me
			if(iPressedNumbers == 1)
			label->setText( "*" );
			if(iPressedNumbers == 2)
			label->setText( "**" );
			if(iPressedNumbers == 3)
			label->setText( "***" );
			if(iPressedNumbers == 4)
			label->setText( "****" );
		}
		else
		label->setText( szEnteredNumbers );
	}
	else
	{
		switch (cmd)
		{
			case KEYPAD_CMD_NONE :			break;

			case KEYPAD_CLEAR:
				iPressedNumbers = 0;

				PlaySound("common/keypad_click.wav", 1);

				label->setVisible(false);

			break;

			
			case KEYPAD_CLOSE_ALL :
			{
				gEngfuncs.pfnClientCmd( "code_dismatch" );

				PlaySound("common/keypad_error.wav", 1);
				CloseWindow();
			}
			break;

			default : 	gEngfuncs.Con_DPrintf("Unknown ActionSingal %i.\n",cmd); 
				break;
		}
	}
}

// return 0 to hook key
// return 1 to allow key
int CKeypad::KeyInput(int down, int keynum, const char *pszCurrentBinding)
{
	if (!down)
		return 1; // dont disable releasing of the keys

	switch (keynum)
	{
	// close window
	case K_ENTER:
	case K_ESCAPE:
	{
		PlaySound("common/keypad_error.wav", 1);

		PlaySound("common/keypad_click.wav", 1);
		CloseWindow();
		return 0;
	}

	case 96:
	case 126:
/*	case K_F1:
	case K_F2:
	case K_F3:
	case K_F4:
	case K_F5:
	case K_F6:
	case K_F7:
	case K_F8:
	case K_F9:
	case K_F10:
	case K_F11:
	case K_F12:*/
	case K_MOUSE1:
	case K_MOUSE2:
	case K_MOUSE3:
/*	case K_MOUSE4:
	case K_MOUSE5:*/
		return 1;
	}

	return 0;
}

void CKeypad::CloseWindow()
{
	setVisible(false);
	gViewPort->UpdateCursorState();
}

CKeypad :: ~CKeypad()
{
}
