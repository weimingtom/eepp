#include "cuimenuitem.hpp"
#include "cuimenu.hpp"

namespace EE { namespace UI {

cUIMenuItem::cUIMenuItem( cUIPushButton::CreateParams& Params ) : 
	cUIPushButton( Params )
{
	mType |= UI_TYPE_GET( UI_TYPE_MENUITEM );
	
	ApplyDefaultTheme();
}

cUIMenuItem::~cUIMenuItem() {
}

void cUIMenuItem::SetTheme( cUITheme * Theme ) {
	cUIControl::SetTheme( Theme, "menuitem" );
	DoAfterSetTheme();
}

Uint32 cUIMenuItem::OnMouseEnter( const eeVector2i &Pos, Uint32 Flags ) {
	cUIPushButton::OnMouseEnter( Pos, Flags );

	reinterpret_cast<cUIMenu*> ( Parent() )->SetItemSelected( this );

	return 1;
}

}}
