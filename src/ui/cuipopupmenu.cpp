#include "cuipopupmenu.hpp"
#include "cuimanager.hpp"

namespace EE { namespace UI {

cUIPopUpMenu::cUIPopUpMenu( cUIPopUpMenu::CreateParams Params ) :
	cUIMenu( Params )
{
	ApplyDefaultTheme();
}

cUIPopUpMenu::~cUIPopUpMenu() {
}

void cUIPopUpMenu::SetTheme( cUITheme * Theme ) {
	cUIControl::SetTheme( Theme, "popupmenu" );
	DoAfterSetTheme();
}

void cUIPopUpMenu::Show() {
	if ( !Visible() ) {
		Enabled( true );
		Visible( true );

		if ( 255.f == Alpha() )
			StartAlphaAnim( 0.f, 255.f, cUIThemeManager::instance()->ControlsFadeInTime() );
		else
			CreateFadeIn( cUIThemeManager::instance()->ControlsFadeInTime() );

		cUIManager::instance()->FocusControl( this );
	}
}

void cUIPopUpMenu::Hide() {
	if ( cUIThemeManager::instance()->DefaultEffectsEnabled() ) {
		DisableFadeOut( cUIThemeManager::instance()->ControlsFadeOutTime() );
	} else {
		Enabled( false );
		Visible( false );
	}
}

void cUIPopUpMenu::OnComplexControlFocusLoss() {
	Hide();
}

Uint32 cUIPopUpMenu::OnMessage( const cUIMessage * Msg ) {
	switch ( Msg->Msg() ) {
		case cUIMessage::MsgClick:
		{
			Hide();
			
			return 1;
		}
	}

	return cUIMenu::OnMessage( Msg );
}

}}