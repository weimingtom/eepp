#ifndef EE_WINDOWCJOYSTICKMANAGER_HPP
#define EE_WINDOWCJOYSTICKMANAGER_HPP

#include <eepp/window/base.hpp>
#include <eepp/window/cjoystick.hpp>

namespace EE { namespace Window {

/** @brief A Joystick Manager class */
class EE_API cJoystickManager {
	public:
		cJoystickManager();

		virtual ~cJoystickManager();

		/** @return The number of joysticks attached to the system */
		virtual Uint32 Count();

		/** Update the states of all joysticks */
		virtual void 	Update() = 0;

		/** @return The joystick instante of the joystick index */
		cJoystick * 	GetJoystick( const Uint32& index );

		/** Rescan all joysticks to look for new joystick connected.
		* This could be slow on some backends, and unnecessary on others.
		* Is slow in SDL. SFML and SDL2 shouldn't need this.
		*/
		virtual void	Rescan();

		/** Close all the joysticks */
		virtual void 	Close();

		/** Open all the joysticks */
		virtual void 	Open();
	protected:
		friend class cJoystick;
		
		bool			mInit;

		cJoystick * 	mJoysticks[ MAX_JOYSTICKS ];

		Uint32			mCount;

		virtual void 	Create( const Uint32& index ) = 0;
};

}}

#endif
