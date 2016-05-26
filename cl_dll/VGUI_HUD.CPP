/****
*
*  Over Ground (2002-2003) - Mod pour Half-Life - SDK
*
*  Code source de Tigerknee (tigerknee@voila.fr)
*  Plus d'infos sur le site internet du mod :
*  http://og.portailmods.com
*  
****/

#include "hud.h"
#include "cl_util.h"
#include "vgui_teamfortressViewport.h"
#include "ammohistory.h"

// ======== Fonctions ===================================
// ======================================================

void TeamFortressViewport :: CreateVguiHud( void )
{
	// ======== Health / Armor / Items ======================
	// ======================================================
	m_pHudMainPanel = new CHealthPanel ( 4, ScreenHeight - 104, 416, 100 );
	m_pHudMainPanel->setParent( this );
	m_pHudMainPanel->setBounds( 4, ScreenHeight - 104, 416, 100 );
	m_pHudMainPanel->setVisible( false );
	m_pHudMainPanel->setFgColor( 0, 0, 0, 255 );
	m_pHudMainPanel->setBgColor( 0, 0, 0, 255 );
	m_pHudMainPanel->Initialize();
/*	
	// ======== Séléction des armes =========================
	// ======================================================
	m_pHudWeaponsSelPanel = new CWeaponsSelPanel ( 4, 44, 202, 32 + MAX_WEAPON_POSITIONS * 45 );
	m_pHudWeaponsSelPanel->setParent( this );
	m_pHudWeaponsSelPanel->setBounds( 4, 44, 202, 32 + MAX_WEAPON_POSITIONS * 45 );
	m_pHudWeaponsSelPanel->setVisible( false );
	m_pHudWeaponsSelPanel->setFgColor( 0, 0, 0, 255 );
	m_pHudWeaponsSelPanel->setBgColor( 0, 0, 0, 255 );
	m_pHudWeaponsSelPanel->Initialize();*/
}

// ======== Health / Armor / Items ======================
// ======================================================

CHealthPanel :: CHealthPanel( int x, int y, int wide, int tall ) : Panel( x, y, wide, tall )
{
}

CHealthPanel :: ~CHealthPanel( void )
{
}

void	CHealthPanel :: Initialize( void )
{
	CSchemeManager *pSchemes	= gViewPort->GetSchemeManager();
	SchemeHandle_t hSmallScheme = pSchemes->getSchemeHandle( "Scoreboard Small Text" );

	// On créer le label principal
	m_pHealthLabel		= new CImageLabel( "hud_main", 0, 0 );

	// Les barres d'energie et d'armure
	m_pBarHealthLabel	= new CImageLabel( "hud_bar_health", 55, 32 );
	m_pBarArmorLabel	= new CImageLabel( "hud_bar_armor", 55, 56 );

	// Le label pour afficher le timer
	m_pTimerLabel	= new Label( "--:--", 45, 8, 67, 11 );

	m_pTimerLabel->setFont	( pSchemes->getFont( hSmallScheme ) );

	// Pour la transparence
	m_pHealthLabel->setFgColor		( 0, 0, 0, 255 );
	m_pHealthLabel->setBgColor		( 0, 0, 0, 255 );
	m_pBarHealthLabel->setFgColor	( 0, 0, 0, 255 );
	m_pBarHealthLabel->setBgColor	( 0, 0, 0, 255 );
	m_pBarArmorLabel->setFgColor	( 0, 0, 0, 255 );
	m_pBarArmorLabel->setBgColor	( 0, 0, 0, 255 );
	m_pTimerLabel->setFgColor		( 255, 255, 255, 0 );
	m_pTimerLabel->setBgColor		( 0, 0, 0, 255 );

	// No comment
	m_pHealthLabel->setParent		( this );
	m_pBarHealthLabel->setParent	( m_pHealthLabel );
	m_pBarArmorLabel->setParent		( m_pHealthLabel );
	m_pTimerLabel->setParent		( m_pHealthLabel );

	// On aligne à gauche sinon il risque d'y avoir des problèmes
	// lors du redimenssionement du panel avec l'image
	m_pHealthLabel->setContentAlignment		( vgui::Label::a_west );
	m_pBarHealthLabel->setContentAlignment	( vgui::Label::a_west );
	m_pBarArmorLabel->setContentAlignment	( vgui::Label::a_west );
	m_pTimerLabel->setContentAlignment		( vgui::Label::a_center );

	// On est obligé de faire un setbounds sinon les images ne s'affichent pas
	// au bon endroit suivant les résolution, bizarre, mais bon c HL, fo pas trop lui
	// en demander il a déja du mal à afficher 100 polys qi en plus en lui demande
	// d'afficher les images au bon endroit il va saturer le pauvre
	m_pHealthLabel->setBounds		( 0, 0, 416, 100 );
	m_pBarHealthLabel->setBounds	( 55, 32, 64, 17 );
	m_pBarArmorLabel->setBounds		( 55, 56, 64, 17 );
	m_pTimerLabel->setBounds		( 45, 8, 67, 11 );

	// Les tailles par défaut
	m_pBarHealthLabel->setSize		( 0, 17 );
	m_pBarArmorLabel->setSize		( 0, 17 );
}
	
void	CHealthPanel :: Update( void )
{
	int a_newsize_x;
	int h_newsize_x;

	if ( gHUD.m_Health.m_iHealth > 0 )
	{
		// On calcule les tailles des barres
		a_newsize_x	= 64 * gHUD.m_Health.m_iArmor / 100;
		h_newsize_x	= 64 * gHUD.m_Health.m_iHealth / 100;
	}
	else
	{
		a_newsize_x = 0;
		h_newsize_x = 0;
	}

	// On change la taille des barres
	m_pBarArmorLabel->setSize	( a_newsize_x, 17 );
	m_pBarHealthLabel->setSize	( h_newsize_x, 17 );
}


void	CHealthPanel :: Update_Timer( void )
{
	char	current_timer[ 16 ];//8
//	int		timer_min;
//	int		timer_sec;
	int		iMessage;

	if ( (gHUD.m_iHideHUDDisplay & ( HIDEHUD_WEAPONS | HIDEHUD_ALL )) )
	{
		// On n'affiche pas le timer
		m_pTimerLabel->setVisible( false );
	}
	else
	{
		iMessage = CVAR_GET_FLOAT("xp_points");
		sprintf( current_timer, "Xp: %02d", iMessage );

		// On change le temps
		m_pTimerLabel->setText( current_timer );

		// On affiche le timer
		m_pTimerLabel->setVisible( true );
	}
}

/*
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
			sprintf( tganame, "hud_slot_%i_%i", k, l );
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
			sprintf( tganame, "hud_slot_sel_%i_%i", k, l );
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
			if ( !p || !p->iId || !gWR.HasAmmo( p ) )
			{
				m_pSlotSelWeaponLabel[ iActiveSlot ][ iPos ]->setVisible( false );
				m_pSlotWeaponLabel[ iActiveSlot ][ iPos ]->setVisible( false );

				continue;
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
}*/
