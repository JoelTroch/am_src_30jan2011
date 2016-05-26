#include "hud.h"
#include "cl_util.h"
#include "vgui_TeamFortressViewport.h"

#define BASE_WIDTH		500
#define BASE_HEIGHT		500

CFirstMenu :: CFirstMenu(int iTrans, int iRemoveMe, int x, int y, int wide, int tall) : CMenuPanel(iTrans, iRemoveMe, x,y,wide,tall)
{
    CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
    SchemeHandle_t hTitleScheme = pSchemes->getSchemeHandle( "Title Font" );
 
    int r, g, b, a;
 
    m_pPanel = new CTransparentPanel( 200, XRES(80), YRES(80), XRES(BASE_WIDTH), YRES(BASE_HEIGHT));
    m_pPanel->setParent( this );
    m_pPanel->setBorder( new LineBorder( Color(255 * 0.7,170 * 0.7,0,0) ) );
 
    m_pCancelButton = new CommandButton( gHUD.m_TextMessage.BufferedLocaliseTextString( "Cancel" ),
                                         XRES(50), YRES(279), XRES(378), YRES(42));
    m_pCancelButton->setContentAlignment(vgui::Label::a_center);
    m_pCancelButton->setParent( m_pPanel );
    m_pCancelButton->addActionSignal( new CMenuHandler_TextWindow(HIDE_TEXTWINDOW) );
 
    m_pTitle = new Label( "", XRES(1), YRES(1), XRES(478), YRES(58) );
    m_pTitle->setParent( m_pPanel );
    m_pTitle->setFont( pSchemes->getFont(hTitleScheme) );
    pSchemes->getFgColor( hTitleScheme, r, g, b, a );
    m_pTitle->setFgColor( r, g, b, a );
    pSchemes->getBgColor( hTitleScheme, r, g, b, a );
    m_pTitle->setBgColor( r, g, b, a );
    m_pTitle->setContentAlignment( vgui::Label::a_center );
    m_pTitle->setText( "Your First Menu" );
 
    // Start - VGUI Tutorial 2
    m_pSpeak = new CommandButton( "Say something", XRES(1), YRES(61), XRES(478), YRES(58));
    m_pSpeak->setContentAlignment( vgui::Label::a_center );
    m_pSpeak->setParent( m_pPanel );
    m_pSpeak->addActionSignal( new CMenuHandler_StringCommand( "test_say" ) );
    // End - VGUI Tutorial 2


	  // Start - VGUI Tutorial 3
	/*
    m_pEditbox = new TextEntry( "Testing!", XRES(200), YRES(230), XRES(230), 17);
    m_pEditbox->setVisible( true );
    m_pEditbox->setParent( m_pPanel );
*/
    m_pMyPicture = new CImageLabel( "cross", XRES(BASE_WIDTH/2), YRES(BASE_HEIGHT/2) );
	m_pMyPicture->setParent( m_pPanel );
    m_pMyPicture->setVisible( true );
    // End - VGUI Tutorial 3 
}