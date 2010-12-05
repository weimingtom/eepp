#ifndef EE_UICUIDROPDOWNLIST_HPP
#define EE_UICUIDROPDOWNLIST_HPP

#include "cuitextinput.hpp"
#include "cuilistbox.hpp"

namespace EE { namespace UI {

class EE_API cUIDropDownList : public cUITextInput {
	public:
		class CreateParams : public cUITextInput::CreateParams {
			public:
				inline CreateParams() :
					cUITextInput::CreateParams(),
					ListBox( NULL ),
					MinNumVisibleItems( 6 ),
					PopUpToMainControl( false )
				{
				}

				inline ~CreateParams() {}

				cUIListBox * 	ListBox;
				Uint32			MinNumVisibleItems;
				bool			PopUpToMainControl;
		};

		cUIDropDownList( cUIDropDownList::CreateParams& Params );

		~cUIDropDownList();

		virtual void SetTheme( cUITheme * Theme );

		cUIListBox * ListBox() const;

		virtual void Update();
	protected:
		cUIListBox *	mListBox;
		Uint32			mMinNumVisibleItems;
		bool			mPopUpToMainControl;

		void ShowListBox();

		void OnListBoxFocusLoss( const cUIEvent * Event );

		virtual void OnItemSelected( const cUIEvent * Event );

		virtual void Show();

		virtual void Hide();

		Uint32 OnMouseClick( const eeVector2i& Pos, const Uint32 Flags );
};

}}

#endif