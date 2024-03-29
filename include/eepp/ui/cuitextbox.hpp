#ifndef EE_UICUITEXTBOX_H
#define EE_UICUITEXTBOX_H

#include <eepp/ui/cuicomplexcontrol.hpp>

namespace EE { namespace UI {

class EE_API cUITextBox : public cUIComplexControl {
	public:
		class CreateParams : public cUIComplexControl::CreateParams {
			public:
				inline CreateParams() :
					cUIComplexControl::CreateParams(),
					Font( NULL ),
					FontColor( 0, 0, 0, 255 ),
					FontShadowColor( 255, 255, 255, 150 ),
					FontSelectionBackColor( 150, 150, 150, 150 )
				{
					cUITheme * Theme = cUIThemeManager::instance()->DefaultTheme();

					if ( NULL != Theme ) {
						Font			= Theme->Font();
						FontColor		= Theme->FontColor();
						FontShadowColor	= Theme->FontShadowColor();
					}

					if ( NULL == Font )
						Font = cUIThemeManager::instance()->DefaultFont();
				}

				inline ~CreateParams() {}

				cFont * 	Font;
				eeColorA 	FontColor;
				eeColorA 	FontShadowColor;
				eeColorA	FontSelectionBackColor;
		};

		cUITextBox( const cUITextBox::CreateParams& Params );

		virtual ~cUITextBox();

		virtual Uint32 Type() const;

		virtual bool IsType( const Uint32& type ) const;

		virtual void Draw();

		virtual void Alpha( const eeFloat& alpha );

		cFont * Font() const;

		void Font( cFont * font );

		virtual const String& Text();

		virtual void Text( const String& text );

		const eeColorA& Color() const;

		void Color( const eeColorA& color );

		const eeColorA& ShadowColor() const;

		void ShadowColor( const eeColorA& color );

		const eeColorA& SelectionBackColor() const;

		void SelectionBackColor( const eeColorA& color );

		virtual void OnTextChanged();

		virtual void OnFontChanged();

		virtual void Padding( const eeRecti& padding );

		const eeRecti& Padding() const;

		virtual void SetTheme( cUITheme * Theme );

		cTextCache * GetTextCache();

		eeFloat GetTextWidth();

		eeFloat GetTextHeight();

		const eeInt& GetNumLines() const;

		const eeVector2f& AlignOffset() const;

		virtual void ShrinkText( const Uint32& MaxWidth );

		bool IsTextSelectionEnabled() const;
	protected:
		cTextCache *	mTextCache;
		String			mString;
		eeColorA 		mFontColor;
		eeColorA 		mFontShadowColor;
		eeColorA		mFontSelectionBackColor;
		eeVector2f 		mAlignOffset;
		eeRecti			mPadding;
		Int32			mSelCurInit;
		Int32			mSelCurEnd;

		virtual void DrawSelection();

		virtual void OnSizeChange();

		virtual void AutoShrink();

		virtual void AutoSize();

		virtual void AutoAlign();

		virtual Uint32 OnFocusLoss();

		virtual Uint32 OnMouseDoubleClick( const eeVector2i& Pos, const Uint32 Flags );

		virtual Uint32 OnMouseClick( const eeVector2i& Pos, const Uint32 Flags );

		virtual Uint32 OnMouseDown( const eeVector2i& Pos, const Uint32 Flags );

		virtual void SelCurInit( const Int32& init );

		virtual void SelCurEnd( const Int32& end );

		virtual Int32 SelCurInit();

		virtual Int32 SelCurEnd();

};

}}

#endif
