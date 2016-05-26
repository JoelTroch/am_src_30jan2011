using namespace vgui;

#include "..\game_shared\vgui_loadtga.h"

#define MIN_KEYPAD_BUTTON_ALPHA	100.0//127

#define ALPHA_VISIBLE		0.0
#define ALPHA_INVISIBLE		255.0
#define BUTTON_SPEED		5


#define KEYPAD_CMD_NONE				90
#define	KEYPAD_CLOSE_ALL			91
#define KEYPAD_CLEAR				92

#define CABLE_RED					80
#define CABLE_BLUE					81
#define CABLE_GREEN					82
//==================================
// CMyButton
//==================================
class CMyKeypadButton : public CommandButton
{
public:
	BitmapTGA	*m_pTGA;
	int iAlpha;
	bool bReady;//determines whether the button is ready to click
	bool bSndSend;
	bool bActive;

	CMyKeypadButton( const char* text,int x,int y,int wide,int tall, bool bNoHighlight = false):CommandButton(text,x,y,wide,tall, false, false)
	{
		setPos( 0, 0 );//in case a stupid forgot

		setPaintBackgroundEnabled(false);//to not show solid colours
		iAlpha = ALPHA_INVISIBLE;//to prevent alpha ping-pong when first started
		bReady = false;//meh, just to be sure
		bActive = true;
		bSndSend = 0;
	}
	~CMyKeypadButton() 
	{
		if( m_pTGA )
		delete m_pTGA;
	}

	void paint()
	{
		//this plays around with TGAs. Be sure there's one of those.
		if (!m_pTGA)
			return;

		if ( bReady )
		{
			if (isArmed())
			{
				if( bSndSend == 0 )
				{
				//	PlaySound("common/keypad_over.wav", 1);

					bSndSend = 1;
				}

				if (iAlpha > ALPHA_VISIBLE)
					iAlpha -= (gHUD.m_flTimeDelta * 1000);

				if (iAlpha < ALPHA_VISIBLE)
					iAlpha = ALPHA_VISIBLE;
			}//
			else
			{
				if (iAlpha < MIN_KEYPAD_BUTTON_ALPHA)
					iAlpha += (gHUD.m_flTimeDelta * 1000);

				if (iAlpha > MIN_KEYPAD_BUTTON_ALPHA)
					iAlpha = MIN_KEYPAD_BUTTON_ALPHA;
			}
		}
		else//button is just created, fade it slooooowly
		{
			if (iAlpha > MIN_KEYPAD_BUTTON_ALPHA)
				iAlpha -= (gHUD.m_flTimeDelta * 50);

			if (iAlpha < MIN_KEYPAD_BUTTON_ALPHA)
				iAlpha = MIN_KEYPAD_BUTTON_ALPHA;

			if (iAlpha == MIN_KEYPAD_BUTTON_ALPHA)
				bReady = true;//yay!
		}

		m_pTGA->doPaint(this);		
	}

	void internalCursorExited()
	{
		setSelected(false);
		setArmed(false);//when mouse is over it, it sets stays armed forever, lets prevent that
		bSndSend = 0;
	}
};


class CKeypad : public Panel/*, public ActionSignal*/
{
public:
	CKeypad();
	~CKeypad();

	void	ActionSignal(int cmd);

	int KeyInput(int down, int keynum, const char *pszCurrentBinding);

	void paint();
	void Init();

	char szCode[32];
	char szFrameLabel[32];

	int iPressedNumbers;

	char szEnteredNumbers[32];
	float fKeyPadAlpha;
	int iHidePassword;
	int iShowCables;

	int iCodeNumbers;

private:
	void CloseWindow(void);

	CMyKeypadButton* mbutton_close;//button for close the window
	CMyKeypadButton* mbutton_clear;

	CImageLabel *	my_wallpaper;

	Label* label;//common label for all buttons
	Label* label_keypad_title;//displays .. something like... "hai, this door is locked"
	Panel* panel;
	Panel* panel_screen;

	CMyKeypadButton* MenuButtons[MAX_BUTTONS];

	CMyKeypadButton* mbutton_cable_red;
	CMyKeypadButton* mbutton_cable_blue;
	CMyKeypadButton* mbutton_cable_green;
};



class CKeypadHandler_Command : public ActionSignal
{

private:
	CKeypad * m_pFather;
	int				 m_cmd;

public:
	CKeypadHandler_Command( CKeypad * panel, int cmd )
	{
		m_pFather = panel;
		m_cmd = cmd;
	}

	virtual void actionPerformed( Panel * panel )
	{
		m_pFather->ActionSignal(m_cmd);
	}
};







