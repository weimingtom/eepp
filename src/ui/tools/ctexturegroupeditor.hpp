#ifndef EE_UITOOLSCTEXTUREGROUPEDITOR_HPP
#define EE_UITOOLSCTEXTUREGROUPEDITOR_HPP

#include "../base.hpp"
#include "ctexturegroupshapeeditor.hpp"
#include "../cuiwindow.hpp"
#include "../cuispinbox.hpp"
#include "../cuilistbox.hpp"
#include "../cuiwinmenu.hpp"
#include "../../graphics/ctexturepacker.hpp"
#include "../../graphics/ctexturegrouploader.hpp"
#include "../../graphics/cshapegroupmanager.hpp"

namespace EE { namespace UI { namespace Tools {

class EE_API cTextureGroupEditor {
	public:
		typedef cb::Callback0<void> TGEditorCloseCb;

		cTextureGroupEditor( cUIWindow * AttatchTo = NULL, const TGEditorCloseCb& callback = TGEditorCloseCb() );

		virtual ~cTextureGroupEditor();

		cUISpinBox *			SpinOffX() const { return mSpinOffX; }

		cUISpinBox *			SpinOffY() const { return mSpinOffY; }
	protected:
		cUIWindow *				mUIWindow;
		cUIControl *			mUIContainer;
		cUITheme *				mTheme;
		TGEditorCloseCb			mCloseCb;
		cTexturePacker *		mTexturePacker;
		cTextureGroupLoader *	mTextureGroupLoader;
		cShape *				mCurShape;
		cUISpinBox *			mSpinOffX;
		cUISpinBox *			mSpinOffY;
		cUISpinBox *			mSpinDestW;
		cUISpinBox *			mSpinDestH;
		cUIListBox *			mShapeList;
		cUIWinMenu *			mWinMenu;
		cTextureGroupShapeEditor * mShapeEditor;

		void WindowClose( const cUIEvent * Event );

		void CreateTGEditor();

		void CreateWinMenu();

		void FileMenuClick( const cUIEvent * Event );

		void OnTextureGroupCreate( cTexturePacker * TexPacker );

		void OpenTextureGroup( const cUIEvent * Event );

		void SaveTextureGroup( const cUIEvent * Event );

		void OnTextureGroupClose( const cUIEvent * Event );

		void OnShapeChange( const cUIEvent * Event );

		cUITextBox * CreateTxtBox( eeVector2i Pos, const String& Text );

		void UpdateControls();

		void FillShapeList();

		void OnOffXChange( const cUIEvent * Event );

		void OnOffYChange( const cUIEvent * Event );

		void OnDestWChange( const cUIEvent * Event );

		void OnDestHChange( const cUIEvent * Event );

		void OnResetDestSize( const cUIEvent * Event );

		void OnResetOffset( const cUIEvent * Event );

		void OnCenterOffset( const cUIEvent * Event );

		void OnHBOffset( const cUIEvent * Event );
};

}}}

#endif
