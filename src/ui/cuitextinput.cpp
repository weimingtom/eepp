#include "cuitextinput.hpp"
#include "cuimanager.hpp"

namespace EE { namespace UI {

cUITextInput::cUITextInput( const cUITextInput::CreateParams& Params ) :
	cUITextBox( Params ),
	mCursorPos(0)
{
	mTextBuffer.Start();
	mTextBuffer.Active( false );
	mTextBuffer.SupportNewLine( Params.SupportNewLine );
	mTextBuffer.SupportFreeEditing( Params.SupportFreeEditing );
	mTextBuffer.MaxLenght( Params.MaxLenght );
	mTextBuffer.SetReturnCallback( boost::bind( &cUITextInput::OnPressEnter, this ) );
}

cUITextInput::~cUITextInput() {
}

void cUITextInput::Update() {
	cUITextBox::Update();
	
	if ( mTextBuffer.ChangedSinceLastUpdate() ) {
		eeVector2f offSet = mAlignOffset;
		
		Text( mTextBuffer.Buffer() );
		
		mAlignOffset = offSet;
		
		ResetWaitCursor();
		
		AlignFix();
		
		return;
	}
	
	if ( mCursorPos != mTextBuffer.CurPos() ) {
		AlignFix();
		mCursorPos = mTextBuffer.CurPos();
	}
}

void cUITextInput::Draw() {	
	cUITextBox::Draw();
	
	if ( Visible() && mTextBuffer.Active() && mTextBuffer.SupportFreeEditing() ) {
		mWaitCursorTime += cUIManager::instance()->Elapsed();
		
		if ( mShowingWait ) {
			bool disableSmooth = mShowingWait && cEngine::instance()->GetVideoInfo()->LineSmooth;
			
			if ( disableSmooth )
				cEngine::instance()->SetLineSmooth( false );
			
			cPrimitives P;
			P.SetColor( mFontColor );
			
			eeVector2i Pos = mPos;
			ControlToScreen( Pos );
			
			eeFloat CurPosX = Pos.x + mAlignOffset.x + mCurPos.x + 1;
			eeFloat CurPosY = Pos.y + mAlignOffset.y + mCurPos.y;
			
			if ( CurPosX > (eeFloat)Pos.x + (eeFloat)mSize.x )
				CurPosX = (eeFloat)Pos.x + (eeFloat)mSize.x;
			
			P.DrawLine( CurPosX, CurPosY, CurPosX, CurPosY + mTextCache.Font()->GetFontHeight(), 1.f );
			
			if ( disableSmooth )
				cEngine::instance()->SetLineSmooth( true );
		}
		
		if ( mWaitCursorTime >= 500.f ) {
			mShowingWait = !mShowingWait;
			mWaitCursorTime = 0.f;
		}
	}
}

Uint32 cUITextInput::OnFocus() {
	mTextBuffer.Active( true );
	ResetWaitCursor();
	return 1;
}

Uint32 cUITextInput::OnFocusLoss() {
	mTextBuffer.Active( false );
	return 1;
}

Uint32 cUITextInput::OnPressEnter() {
	return 0;
}

void cUITextInput::PushIgnoredChar( const Uint32& ch ) {
	mTextBuffer.PushIgnoredChar( ch );
}

void cUITextInput::ResetWaitCursor() {
	mShowingWait = true;
	mWaitCursorTime = 0.f;
}

void cUITextInput::AlignFix() {
	if ( !( FontHAlignGet( Flags() ) == UI_HALIGN_LEFT && !mTextBuffer.SupportNewLine() ) )
		return;
	
	Uint32 NLPos = 0;
	Uint32 LineNum = mTextBuffer.GetCurPosLinePos( NLPos );
	
	mTextCache.Font()->SetText( mTextBuffer.Buffer().substr( NLPos, mTextBuffer.CurPos() - NLPos ) );
	
	eeFloat tW = mTextCache.Font()->GetTextWidth();
	eeFloat tX = mAlignOffset.x + tW;
	
	mCurPos.x = tW;
	mCurPos.y = (eeFloat)LineNum * (eeFloat)mTextCache.GetTextHeight();
	
	if ( tX < 0.f )
		mAlignOffset.x = -( mAlignOffset.x + ( tW - mAlignOffset.x ) );
	else if ( tX > mSize.Width() )
		mAlignOffset.x = mSize.Width() - ( mAlignOffset.x + ( tW - mAlignOffset.x ) );
	
}

}}
