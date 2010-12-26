#include "cinputtextbuffer.hpp"
#include "cengine.hpp"

namespace EE { namespace Window {

cInputTextBuffer::cInputTextBuffer( const bool& active, const bool& supportNewLine, const bool& supportFreeEditing, const Uint32& maxLenght ) :
	mFlags(0),
	mCallback(0),
	mPromptPos(0),
	mMaxLenght(0xFFFFFFFF)
{
	Active( active );

	SupportFreeEditing( supportFreeEditing );

	SupportNewLine( supportNewLine );

	AutoPrompt( true );

	SupportCopyPaste( true );

	mMaxLenght = maxLenght;
}

cInputTextBuffer::cInputTextBuffer() :
	mFlags(0),
	mCallback(0),
	mPromptPos(0),
	mMaxLenght(0xFFFFFFFF)
{
	Active( true );

	SupportFreeEditing( true );

	SupportNewLine( false );

	AutoPrompt( true );

	SupportCopyPaste( true );
}

cInputTextBuffer::~cInputTextBuffer() {
	if ( 0 != mCallback && NULL != cInput::ExistsSingleton() )
		cInput::instance()->PopCallback( mCallback );

	mText.clear();
}

void cInputTextBuffer::Start() {
	mCallback = cInput::instance()->PushCallback( cb::Make1( this, &cInputTextBuffer::Update ) );
}

void cInputTextBuffer::Update( EE_Event* Event ) {
	if ( Active() ) {
		ChangedSinceLastUpdate( false );
		Int32 c = convertKeyCharacter( Event );

		if ( SupportFreeEditing() ) {
			switch ( Event->type ) {
				case SDL_KEYDOWN:
					if ( cInput::instance()->ShiftPressed() || cInput::instance()->ControlPressed() ) {
						if ( !AllowOnlyNumbers() &&
							(	( ( Event->key.keysym.mod & KMOD_SHIFT ) && Event->key.keysym.sym == SDLK_INSERT ) ||
								( ( Event->key.keysym.mod & KMOD_CTRL ) && Event->key.keysym.sym == SDLK_v ) )
							)
						{
							std::wstring txt = cEngine::instance()->GetClipboardTextWStr();

							if ( !SupportNewLine() ) {
								Uint32 pos = txt.find_first_of( L'\n' );

								if ( pos != std::string::npos )
									txt = txt.substr( 0, pos );
							}

							if ( txt.size() ) {
								ChangedSinceLastUpdate( true );

								if ( mText.size() + txt.size() < mMaxLenght ) {
									if ( AutoPrompt() ) {
										mText += txt;
										mPromptPos = (eeInt)mText.size();
									} else {
										mText.insert( mPromptPos, txt );
										mPromptPos += txt.size();
									}

									AutoPrompt( false );
								}
							}
						}
					}

					if ( ( c == KEY_BACKSPACE || c == KEY_DELETE ) ) {
						if ( mText.size() ) {
							ChangedSinceLastUpdate( true );

							if ( mPromptPos < (eeInt)mText.size() ) {
								if ( c == KEY_BACKSPACE ) {
									if ( mPromptPos > 0 ) {
										mText.erase(mPromptPos-1,1);
										mPromptPos--;
									}
								} else
									mText.erase(mPromptPos,1);
							} else if ( c == KEY_BACKSPACE ) {
								mText.resize( mText.size() - 1 );
								AutoPrompt( true );
							}
						}
					} else if ( ( c == KEY_RETURN || c == KEY_KP_ENTER ) ) {
						if ( SupportNewLine() && CanAdd() ) {
							InsertChar( mText, mPromptPos, L'\n' );

							mPromptPos++;

							ChangedSinceLastUpdate( true );
						}

						if ( mEnterCall.IsSet() )
							mEnterCall();

 					} else if ( c == KEY_LEFT ) {
						if ( ( mPromptPos - 1 ) >= 0 ) {
							mPromptPos--;
							AutoPrompt( false );
						}
					} else if ( c == KEY_RIGHT ) {
						if ( ( mPromptPos + 1 ) < (eeInt)mText.size() ) {
							mPromptPos++;
							AutoPrompt(false);
						} else if ( ( mPromptPos + 1 ) == (eeInt)mText.size() ) {
							AutoPrompt( true );
						}
					} else if ( c == KEY_UP ) {
						MovePromptRowUp( false );
					} else if ( c == KEY_DOWN ) {
						MovePromptRowDown( true );
					} else if ( c == KEY_PAGEUP ) {
						MovePromptRowUp( true );
					} else if ( c == KEY_PAGEDOWN ) {
						MovePromptRowDown( false );
					} else if ( CanAdd() && isCharacter(c) ) {
						bool Ignored = false;

						if ( AllowOnlyNumbers() && !isNumber( c, AllowDotsInNumbers() ) ) {
							Ignored = true;
						}

						if ( mIgnoredChars.size() ) {
							for ( eeUint i = 0; i < mIgnoredChars.size(); i++ ) {
								if ( mIgnoredChars[i] == (Uint32)c )
									Ignored = true;
							}
						}

						if ( !Ignored ) {
							ChangedSinceLastUpdate( true );

							if ( AutoPrompt() ) {
								mText += c;
								mPromptPos = (eeInt)mText.size();
							} else {
								InsertChar( mText, mPromptPos, c );
								mPromptPos++;
							}
						}
					}

					break;
				case SDL_KEYUP:
					if ( SupportNewLine() ) {
						if ( Event->key.keysym.sym == SDLK_END ) {
							for ( Uint32 i = mPromptPos; i < mText.size(); i++ )  {
								if ( mText[i] == L'\n' ) {
									mPromptPos = i;
									AutoPrompt( false );
									break;
								}

								if ( i == ( mText.size() - 1 ) ) {
									mPromptPos = mText.size();
									AutoPrompt( false );
								}
							}
						}

						if ( Event->key.keysym.sym == SDLK_HOME ) {
							if ( 0 != mPromptPos ) {
								for ( Int32 i = (Int32)mPromptPos - 1; i >= 0; i-- )  {
									if ( i >= 0 ) {
										if ( mText[i] == L'\n' ) {
											mPromptPos = i + 1;
											AutoPrompt( false );
											break;
										}

										if ( i == 0 ) {
											mPromptPos = 0;
											AutoPrompt( false );
										}
									}
								}
							}
						}
					} else {
						if ( Event->key.keysym.sym == SDLK_END ) {
							AutoPrompt( true );
						}

						if ( Event->key.keysym.sym == SDLK_HOME ) {
							mPromptPos = 0;
							AutoPrompt(false);
						}
					}
					break;
			}
		} else {
			if ( Event->type == SDL_KEYDOWN ) {
				ChangedSinceLastUpdate( true );

				if ( c == KEY_BACKSPACE && mText.size() > 0 ) {
					mText.resize( mText.size() - 1 );
				} else if ( ( c == KEY_RETURN || c == KEY_KP_ENTER ) && !cInput::instance()->MetaPressed() && !cInput::instance()->AltPressed() && !cInput::instance()->ControlPressed() ) {
					if ( SupportNewLine() && CanAdd() )
						mText += L'\n';

					if ( mEnterCall.IsSet() )
						mEnterCall();
				} else if ( CanAdd() && isCharacter(c) && !cInput::instance()->MetaPressed() && !cInput::instance()->AltPressed() && !cInput::instance()->ControlPressed() ) {
					if ( !( AllowOnlyNumbers() && !isNumber( c, AllowDotsInNumbers() ) ) ) {
						mText += c;
					}
				}
			}
		}
	}
}

void cInputTextBuffer::MovePromptRowDown( const bool& breakit ) {
	if ( SupportFreeEditing() && SupportNewLine() ) {
		Uint32 dNLPos	= 0;
		GetCurPosLinePos( dNLPos );
		Uint32 dCharsTo = mPromptPos - dNLPos;

		Uint32 dLastLinePos		= 0;
		Uint32 dCharLineCount	= 0;

		for ( Uint32 i = mPromptPos; i < mText.size(); i++ )  {
			if ( mText[i] == L'\n' ) {
				if ( breakit ) {
					if ( 0 == dLastLinePos ) {
						dLastLinePos = i + 1;

						dCharLineCount = 0;
					} else {
						break;
					}
				} else {
					dLastLinePos = i + 1;

					dCharLineCount = 0;
				}
			} else {
				dCharLineCount++;
			}
		}

		if ( 0 != dLastLinePos ) {
			if ( dCharLineCount < dCharsTo ) {
				mPromptPos = dLastLinePos + dCharLineCount;
			} else {
				mPromptPos = dLastLinePos + dCharsTo;
			}

			AutoPrompt( false );
		}
	}
}

void cInputTextBuffer::MovePromptRowUp( const bool& breakit ) {
	if ( SupportFreeEditing() && SupportNewLine() ) {
		Uint32 uNLPos	= 0;
		Uint32 uLineNum	= GetCurPosLinePos( uNLPos );
		Uint32 uCharsTo = mPromptPos - uNLPos;

		if ( uLineNum >= 1 ) {
			Uint32 uLastLinePos		= 0;
			Uint32 uCharLineCount	= 0;
			uNLPos					= ( uNLPos - 1 );

			for ( Uint32 i = 0; i < uNLPos; i++ )  {
				if ( mText[i] == L'\n' ) {
					if ( !breakit ) {
						uLastLinePos = i + 1;

						uCharLineCount = 0;
					} else {
						break;
					}
				} else {
					uCharLineCount++;
				}
			}

			if ( uCharLineCount < uCharsTo ) {
				mPromptPos = uLastLinePos + uCharLineCount;
			} else {
				mPromptPos = uLastLinePos + uCharsTo;
			}

			AutoPrompt( false );
		}
	}
}

void cInputTextBuffer::Clear() {
	mText.clear();
	AutoPrompt( true );
}

void cInputTextBuffer::SetReturnCallback( EnterCallback EC ) {
	mEnterCall = EC;
}

void cInputTextBuffer::Buffer( const std::wstring& str ) {
	if ( mText != str ) {
		mText = str;
		ChangedSinceLastUpdate( true );
	}
}

eeInt cInputTextBuffer::CurPos() const {
	return mPromptPos;
}

void cInputTextBuffer::CurPos( const Uint32& pos ) {
	if ( SupportFreeEditing() && pos < mText.size() ) {
		mPromptPos = pos;
		AutoPrompt( false );
	}
}

Uint32 cInputTextBuffer::GetCurPosLinePos( Uint32& LastNewLinePos ) {
	if ( SupportFreeEditing() ) {
		Uint32 nl = 0;
		LastNewLinePos = 0;
		for ( eeInt i = 0; i < mPromptPos; i++ )  {
			if ( mText[i] == L'\n' ) {
				nl++;
				LastNewLinePos = i + 1;
			}
		}
		return nl;
	}
	return 0;
}

void cInputTextBuffer::PushIgnoredChar( const Uint32& ch ) {
	mIgnoredChars.push_back( ch );
}

bool cInputTextBuffer::CanAdd() {
	return mText.size() < mMaxLenght;
}

void cInputTextBuffer::MaxLenght( const Uint32& Max ) {
	mMaxLenght = Max;

	if ( mText.size() > mMaxLenght )
		mText.resize( mMaxLenght );
}

const Uint32& cInputTextBuffer::MaxLenght() const {
	return mMaxLenght;
}

std::wstring cInputTextBuffer::Buffer() const {
	return mText;
}

bool cInputTextBuffer::ChangedSinceLastUpdate() {
	return 0 != ( mFlags & ( 1 << INPUT_TB_CHANGE_SINCE_LAST_UPDATE ) );
}

void cInputTextBuffer::ChangedSinceLastUpdate( const bool& Changed ) {
	Write32BitKey( &mFlags, INPUT_TB_CHANGE_SINCE_LAST_UPDATE, Changed == true );
}

void cInputTextBuffer::AutoPrompt( const bool& set ) {
	Write32BitKey( &mFlags, INPUT_TB_PROMPT_AUTO_POS, set == true );

	if ( set ) {
		mPromptPos		= (eeInt)mText.size();
	}
}

bool cInputTextBuffer::AutoPrompt() {
	return 0 != ( mFlags & ( 1 << INPUT_TB_PROMPT_AUTO_POS ) );
}

bool cInputTextBuffer::Active() const {
	return 0 != ( mFlags & ( 1 << INPUT_TB_ACTIVE ) );;
}

void cInputTextBuffer::Active( const bool& Active ) {
	Write32BitKey( &mFlags, INPUT_TB_ACTIVE, Active == true );
}

bool cInputTextBuffer::SupportNewLine() {
	return 0 != ( mFlags & ( 1 << INPUT_TB_SUPPORT_NEW_LINE ) );
}

void cInputTextBuffer::SupportNewLine( const bool& SupportNewLine ) {
	Write32BitKey( &mFlags, INPUT_TB_SUPPORT_NEW_LINE, SupportNewLine == true );
}

void cInputTextBuffer::AllowOnlyNumbers( const bool& onlynums, const bool& allowdots ) {
	Write32BitKey( &mFlags, INPUT_TB_ALLOW_ONLY_NUMBERS, onlynums == true );
	Write32BitKey( &mFlags, INPUT_TB_ALLOW_DOT_IN_NUMBERS, allowdots == true );
}

bool cInputTextBuffer::AllowOnlyNumbers() {
	return 0 != ( mFlags & ( 1 << INPUT_TB_ALLOW_ONLY_NUMBERS ) );
}

bool cInputTextBuffer::AllowDotsInNumbers() {
	return 0 != ( mFlags & ( 1 << INPUT_TB_ALLOW_DOT_IN_NUMBERS ) );
}

bool cInputTextBuffer::SupportFreeEditing() const {
	return 0 != ( mFlags & ( 1 << INPUT_TB_FREE_EDITING ) );
}

void cInputTextBuffer::SupportFreeEditing( const bool& Support ) {
	Write32BitKey( &mFlags, INPUT_TB_FREE_EDITING, Support == true );
}

void cInputTextBuffer::SupportCopyPaste( const bool& support ) {
	Write32BitKey( &mFlags, INPUT_TB_SUPPORT_COPY_PASTE, support == true );
}

bool cInputTextBuffer::SupportCopyPaste() {
	return 0 != ( mFlags & ( 1 << INPUT_TB_SUPPORT_COPY_PASTE ) );
}

void cInputTextBuffer::CursorToEnd() {
	mPromptPos = mText.size();
}

}}
