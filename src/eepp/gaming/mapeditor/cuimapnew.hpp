#ifndef EE_GAMINGCUIMAPNEW_HPP
#define EE_GAMINGCUIMAPNEW_HPP

#include <eepp/gaming/base.hpp>
#include <eepp/ui/cuiwindow.hpp>
#include <eepp/gaming/mapeditor/cuimap.hpp>

using namespace EE::UI;

namespace EE { namespace Gaming { namespace MapEditor {

class EE_API cUIMapNew {
	public:
		cUIMapNew( cUIMap * Map, cb::Callback0<void> NewMapCb = cb::Callback0<void>(), bool ResizeMap = false );

		virtual ~cUIMapNew();
	protected:
		cUITheme *			mTheme;
		cUIWindow *			mUIWindow;
		cUIMap *			mUIMap;
		cUISpinBox *		mUIMapWidth;
		cUISpinBox *		mUIMapHeight;
		cUISpinBox *		mUIMapTWidth;
		cUISpinBox *		mUIMapTHeight;
		cUISpinBox *		mUIMapMaxLayers;
		cUICheckBox *		mUILightsEnabled;
		cUICheckBox *		mUILightsByVertex;
		cUICheckBox *		mUIClampBorders;
		cUICheckBox *		mUIClipArea;
		cUIComplexControl *	mUIBaseColor;
		cUISlider *			mUIRedSlider;
		cUISlider *			mUIGreenSlider;
		cUISlider *			mUIBlueSlider;
		cUITextBox *		mUIRedTxt;
		cUITextBox *		mUIGreenTxt;
		cUITextBox *		mUIBlueTxt;

		cb::Callback0<void> mNewMapCb;

		eeSize				mNewSize;
		bool				mResizeMap;

		void WindowClose( const cUIEvent * Event );

		void CancelClick( const cUIEvent * Event );

		void OKClick( const cUIEvent * Event );

		void OnRedChange( const cUIEvent * Event );

		void OnGreenChange( const cUIEvent * Event );

		void OnBlueChange( const cUIEvent * Event );
};

}}}

#endif
