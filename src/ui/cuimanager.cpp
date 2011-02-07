#include "cuimanager.hpp"
#include "../window/cengine.hpp"

namespace EE { namespace UI {

cUIManager::cUIManager() :
	mKM( NULL ),
	mInit( false ),
	mControl( NULL ),
	mFocusControl( NULL ),
	mOverControl( NULL ),
	mDownControl( NULL ),
	mFirstPress( false ),
	mCbId(-1)
{
	mKM = cInput::instance();
}

cUIManager::~cUIManager() {
	Shutdown();
}

void cUIManager::Init() {
	if ( mInit )
		Shutdown();

	mInit			= true;
	mControl		= eeNew( cUIControlAnim, ( cUIControl::CreateParams( NULL, eeVector2i( 0, 0 ), eeSize( cEngine::instance()->GetWidth(), cEngine::instance()->GetHeight() ) ) ) );
	mControl->Visible( true );
	mControl->Enabled( true );

	mFocusControl	= mControl;
	mOverControl	= mControl;

	mCbId = cInput::instance()->PushCallback( cb::Make1( this, &cUIManager::InputCallback ) );
}

void cUIManager::Shutdown() {
	if ( mInit ) {
		if ( -1 != mCbId )
			cInput::instance()->PopCallback( mCbId );

		mOverControl = NULL;
		mFocusControl = NULL;

		eeSAFE_DELETE( mControl );

		mInit = false;
	}

	cUIThemeManager::DestroySingleton();
}

void cUIManager::InputCallback( EE_Event * Event ) {
	switch( Event->type ) {
		case SDL_KEYUP:
			SendKeyUp( Event->key.keysym.sym, Event->key.keysym.unicode, Event->key.keysym.mod );
			break;
		case SDL_KEYDOWN:
			SendKeyDown( Event->key.keysym.sym, Event->key.keysym.unicode, Event->key.keysym.mod );
			break;
	}
}

void cUIManager::ResizeControl() {
	mControl->Size( cEngine::instance()->GetWidth(), cEngine::instance()->GetHeight() );
	SendMsg( mControl, cUIMessage::MsgWindowResize );
}

void cUIManager::SendKeyUp( const Uint32& KeyCode, const Uint16& Char, const Uint32& Mod ) {
	cUIEventKey	KeyEvent	= cUIEventKey( mFocusControl, cUIEvent::EventKeyUp, KeyCode, Char, Mod );
	cUIControl * CtrlLoop	= mFocusControl;

	while( NULL != CtrlLoop ) {
		if ( CtrlLoop->Enabled() && CtrlLoop->OnKeyUp( KeyEvent ) )
			break;

		CtrlLoop = CtrlLoop->Parent();
	}
}

void cUIManager::SendKeyDown( const Uint32& KeyCode, const Uint16& Char, const Uint32& Mod ) {
	cUIEventKey	KeyEvent	= cUIEventKey( mFocusControl, cUIEvent::EventKeyDown, KeyCode, Char, Mod );
	cUIControl * CtrlLoop	= mFocusControl;

	while( NULL != CtrlLoop ) {
		if ( CtrlLoop->Enabled() && CtrlLoop->OnKeyDown( KeyEvent ) )
			break;

		CtrlLoop = CtrlLoop->Parent();
	}
}

cUIControl * cUIManager::FocusControl() const {
	return mFocusControl;
}

void cUIManager::FocusControl( cUIControl * Ctrl ) {
	if ( NULL != Ctrl && Ctrl != mFocusControl ) {
		cUIControl * CtrlFocusLoss = mFocusControl;

		mFocusControl = Ctrl;

		CtrlFocusLoss->OnFocusLoss();
		SendMsg( CtrlFocusLoss, cUIMessage::MsgFocusLoss );

		mFocusControl->OnFocus();
		SendMsg( mFocusControl, cUIMessage::MsgFocus );
	}
}

cUIControl * cUIManager::OverControl() const {
	return mOverControl;
}

void cUIManager::OverControl( cUIControl * Ctrl ) {
	mOverControl = Ctrl;
}

void cUIManager::SendMsg( cUIControl * Ctrl, const Uint32& Msg, const Uint32& Flags ) {
	cUIMessage tMsg( Ctrl, Msg, Flags );

	Ctrl->MessagePost( &tMsg );
}

void cUIManager::Update() {
	mElapsed = cEngine::instance()->Elapsed();

	mControl->Update();

	if ( mKM->PressTrigger() ) {
		if ( NULL != mOverControl ) {
			if ( mOverControl != mFocusControl )
				FocusControl( mOverControl );

			mOverControl->OnMouseDown( mKM->GetMousePos(), mKM->PressTrigger() );
			SendMsg( mOverControl, cUIMessage::MsgMouseDown, mKM->PressTrigger() );
		}

		if ( !mFirstPress ) {
			mDownControl = mOverControl;

			mFirstPress = true;
		}
	}

	if ( mKM->ReleaseTrigger() ) {
		if ( NULL != mFocusControl ) {
			mFocusControl->OnMouseUp( mKM->GetMousePos(), mKM->ReleaseTrigger() );
			SendMsg( mFocusControl, cUIMessage::MsgMouseUp, mKM->ClickTrigger() );

			if ( mDownControl == mOverControl && mKM->ClickTrigger() ) {
				SendMsg( mFocusControl, cUIMessage::MsgClick, mKM->ClickTrigger() );
				mFocusControl->OnMouseClick( mKM->GetMousePos(), mKM->ClickTrigger() );

				if ( mKM->DoubleClickTrigger() ) {
					SendMsg( mFocusControl, cUIMessage::MsgDoubleClick, mKM->DoubleClickTrigger() );
					mFocusControl->OnMouseDoubleClick( mKM->GetMousePos(), mKM->DoubleClickTrigger() );
				}
			}
		}

		mFirstPress = false;
	}

	cUIControl * pOver = mControl->OverFind( mKM->GetMousePosf() );

	if ( pOver != mOverControl ) {
		if ( NULL != mOverControl ) {
			SendMsg( mOverControl, cUIMessage::MsgMouseExit );
			mOverControl->OnMouseExit( mKM->GetMousePos(), 0 );
		}

		mOverControl = pOver;

		if ( NULL != mOverControl ) {
			SendMsg( mOverControl, cUIMessage::MsgMouseEnter );
			mOverControl->OnMouseEnter( mKM->GetMousePos(), 0 );
		}
	} else {
		if ( NULL != mOverControl )
			mOverControl->OnMouseMove( mKM->GetMousePos(), mKM->PressTrigger() );
	}

	mControl->CheckClose();
}

void cUIManager::Draw() {
	mControl->InternalDraw();
	cGlobalBatchRenderer::instance()->Draw();
}

cUIControlAnim * cUIManager::MainControl() const {
	return mControl;
}

const eeFloat& cUIManager::Elapsed() const {
	return mElapsed;
}

eeVector2i cUIManager::GetMousePos() {
	return mKM->GetMousePos();
}

cInput * cUIManager::GetInput() const {
	return mKM;
}

const Uint32& cUIManager::PressTrigger() const {
	return mKM->PressTrigger();
}

const Uint32& cUIManager::LastPressTrigger() const {
	return mKM->LastPressTrigger();
}

void cUIManager::ClipEnable( const Int32& x, const Int32& y, const Uint32& Width, const Uint32& Height ) {
	cEngine::instance()->ClipPlaneEnable( x, y, Width, Height );
}

void cUIManager::ClipDisable() {
	cEngine::instance()->ClipPlaneDisable();
}

}}
