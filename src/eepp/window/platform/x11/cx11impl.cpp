#include <eepp/declares.hpp>

#if defined( EE_X11_PLATFORM )

#include <eepp/helper/glew/glxew.h>
#include <X11/Xcursor/Xcursor.h>
#include <X11/cursorfont.h>
#include <climits>
#undef Window
#undef Display
#undef Cursor
#define XAtom(str) XInternAtom( mDisplay, str, False )

#include <eepp/window/platform/x11/cx11impl.hpp>
#include <eepp/window/platform/x11/ccursorx11.hpp>

using namespace EE::Window::Cursor;

namespace EE { namespace Window { namespace Platform {

cX11Impl::cX11Impl( cWindow * window, eeWindowHandle display, X11Window xwindow, X11Window mainwindow, LockFunc lock, UnlockFunc unlock ) :
	cPlatformImpl( window ),
	mDisplay( display ),
	mX11Window( xwindow ),
	mMainWindow( mainwindow ),
	mLock( lock ),
	mUnlock( unlock ),
	mCursorCurrent( 0 ),
	mCursorInvisible( None ),
	mCursorSystemLast( None ),
	mCursorHidden( false )
{
}

cX11Impl::~cX11Impl() {
	if ( None != mCursorInvisible )
		XFreeCursor( mDisplay, mCursorInvisible );

	if ( None != mCursorSystemLast )
		XFreeCursor( mDisplay, mCursorSystemLast );
}

void cX11Impl::MinimizeWindow() {
	Lock();

	XIconifyWindow( mDisplay, mX11Window, 0 );

	XFlush( mDisplay );

	Unlock();
}

void cX11Impl::MaximizeWindow() {
	// coded by Rafał Maj, idea from Måns Rullgård http://tinyurl.com/68mvk3
	Lock();

	XEvent xev;
	Atom wm_state =  XAtom( "_NET_WM_STATE" );
	Atom maximizeV = XAtom( "_NET_WM_STATE_MAXIMIZED_VERT" );
	Atom maximizeH = XAtom( "_NET_WM_STATE_MAXIMIZED_HORZ" );

	memset( &xev, 0, sizeof(xev) );
	xev.type = ClientMessage;
	xev.xclient.window = mX11Window;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = 1;
	xev.xclient.data.l[1] = maximizeV;
	xev.xclient.data.l[2] = maximizeH;
	xev.xclient.data.l[3] = 0;
	XSendEvent( mDisplay, DefaultRootWindow(mDisplay), 0, SubstructureNotifyMask|SubstructureRedirectMask, &xev);

	XFlush(mDisplay);

	Unlock();
}

bool cX11Impl::IsWindowMaximized() {
	Lock();

	//bool minimized = false;
	bool maximizedhorz = false;
	bool maximizedvert = false;
	Atom type;
	int format;
	unsigned long numitems, bytesafter;
	unsigned char * properties = 0;

	XGetWindowProperty( mDisplay,
						mX11Window,
						XAtom("_NET_WM_STATE"),
						0,
						LONG_MAX,
						false,
						AnyPropertyType,
						&type,
						&format,
						&numitems,
						&bytesafter, &properties
	);

	if( properties && ( format == 32 ) ) {
		for(unsigned int i = 0; i < numitems; ++i) {
			const Atom prop = (reinterpret_cast<ulong *>(properties))[i];

			if (prop == XAtom("_NET_WM_STATE_MAXIMIZED_HORZ"))
				maximizedhorz = true;

			if (prop == XAtom("_NET_WM_STATE_MAXIMIZED_VERT"))
				maximizedvert = true;

			/*if (prop == XAtom("_NET_WM_STATE_HIDDEN"))
				minimized = true;*/
		}
	}

	XFree(properties);

	XFlush(mDisplay);

	Unlock();

	if( maximizedhorz && maximizedvert ) {
		return true;
	}

	return false;
}

void cX11Impl::HideWindow() {
	Lock();

	XUnmapWindow( mDisplay, mX11Window );

	Unlock();
}

void cX11Impl::RaiseWindow() {
	Lock();

	XRaiseWindow( mDisplay, mX11Window );

	Unlock();
}

void cX11Impl::ShowWindow() {
	Lock();

	XMapRaised( mDisplay, mX11Window );

	Unlock();
}

void cX11Impl::MoveWindow( int left, int top ) {
	Lock();

	XMoveWindow( mDisplay, mX11Window, left, top );

	XFlush( mDisplay );

	Unlock();
}

void cX11Impl::SetContext( eeWindowContex Context ) {
	Lock();

	glXMakeCurrent( mDisplay, mX11Window, Context );

	Unlock();
}

eeVector2i cX11Impl::Position() {
	int x, y;
	X11Window child_return;

	XTranslateCoordinates ( mDisplay, mX11Window, DefaultRootWindow( mDisplay ), 0, 0, &x, &y, &child_return );

	return eeVector2i( x, y );
}

void cX11Impl::ShowMouseCursor() {
	if ( !mCursorHidden )
	  return;

	Lock();

	XDefineCursor( mDisplay, mMainWindow, mCursorCurrent );

	mCursorHidden = false;

	Unlock();
}

void cX11Impl::HideMouseCursor() {
	if ( mCursorHidden )
		return;

	Lock();

	if ( mCursorInvisible == None ) {
		unsigned long gcmask;
		XGCValues gcvalues;

		Pixmap pixmap = XCreatePixmap( mDisplay, mMainWindow, 1, 1, 1 );

		GC temp_gc;
		XColor color;

		gcmask = GCFunction | GCForeground | GCBackground;
		gcvalues.function = GXcopy;
		gcvalues.foreground = 0;
		gcvalues.background = 0;
		temp_gc = XCreateGC( mDisplay, pixmap, gcmask, &gcvalues);
		XDrawPoint( mDisplay, pixmap, temp_gc, 0, 0 );
		XFreeGC( mDisplay, temp_gc );
		color.pixel = 0;
		color.red = color.green = color.blue = 0;
		color.flags = DoRed | DoGreen | DoBlue;

		mCursorInvisible = XCreatePixmapCursor( mDisplay, pixmap, pixmap, &color, &color, 0, 0 );

		XFreePixmap( mDisplay, pixmap );
	}

	XDefineCursor( mDisplay, mX11Window, mCursorInvisible );

	mCursorHidden = true;

	Unlock();
}

cCursor * cX11Impl::CreateMouseCursor( cTexture * tex, const eeVector2i& hotspot, const std::string& name ) {
	return eeNew( cCursorX11, ( tex, hotspot, name, mWindow ) );
}

cCursor * cX11Impl::CreateMouseCursor( cImage * img, const eeVector2i& hotspot, const std::string& name ) {
	return eeNew( cCursorX11, ( img, hotspot, name, mWindow ) );
}

cCursor * cX11Impl::CreateMouseCursor( const std::string& path, const eeVector2i& hotspot, const std::string& name ) {
	return eeNew( cCursorX11, ( path, hotspot, name, mWindow ) );
}

void cX11Impl::SetMouseCursor( cCursor * cursor ) {
	mCursorCurrent = reinterpret_cast<cCursorX11*>( cursor )->GetCursor();

	if ( !mCursorHidden ) {
		Lock();

		XDefineCursor( mDisplay, mMainWindow,  mCursorCurrent );

		Unlock();
	}
}

void cX11Impl::RestoreCursor() {
	if ( !mCursorHidden ) {
		Lock();

		XDefineCursor( mDisplay, mMainWindow,  mCursorCurrent );

		Unlock();
	} else {
		HideMouseCursor();
	}
}

void cX11Impl::SetSystemMouseCursor( Cursor::EE_SYSTEM_CURSOR syscursor ) {
	unsigned int cursor_shape;

	switch ( syscursor ) {
		case SYS_CURSOR_ARROW:		cursor_shape = XC_arrow; break;
		case SYS_CURSOR_WAIT:		cursor_shape = XC_watch; break;
		case SYS_CURSOR_WAITARROW:	cursor_shape = XC_watch; break;
		case SYS_CURSOR_IBEAM:		cursor_shape = XC_xterm; break;
		case SYS_CURSOR_SIZEALL:	cursor_shape = XC_fleur; break;
		case SYS_CURSOR_SIZENWSE:	cursor_shape = XC_fleur; break;
		case SYS_CURSOR_SIZENESW:	cursor_shape = XC_fleur; break;
		case SYS_CURSOR_SIZEWE:		cursor_shape = XC_sb_h_double_arrow; break;
		case SYS_CURSOR_SIZENS:		cursor_shape = XC_sb_v_double_arrow; break;
		case SYS_CURSOR_CROSSHAIR:	cursor_shape = XC_tcross; break;
		case SYS_CURSOR_HAND:		cursor_shape = XC_hand2; break;
		case SYS_CURSOR_NO:			cursor_shape = XC_pirate; break;
		default:					return;
	}

	if ( None != mCursorCurrent ) {
		XFreeCursor( mDisplay, mCursorSystemLast );
	}

	Lock();

	mCursorCurrent		= XCreateFontCursor( mDisplay, cursor_shape );
	mCursorSystemLast	= mCursorCurrent;

	if ( !mCursorHidden ) {
		XDefineCursor( mDisplay, mMainWindow, mCursorCurrent );
	}

	Unlock();
}

eeWindowHandle cX11Impl::GetDisplay() const {
	return mDisplay;
}

void cX11Impl::Lock() {
	if ( NULL != mLock )
		mLock();
}

void cX11Impl::Unlock() {
	if ( NULL != mUnlock )
		mUnlock();
}

}}}

#endif
