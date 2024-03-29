#ifndef EE_WINDOWCJOYSTICKMANAGERSFML_HPP
#define EE_WINDOWCJOYSTICKMANAGERSFML_HPP

#ifdef EE_BACKEND_SFML_ACTIVE

#include <eepp/window/cjoystickmanager.hpp>

namespace EE { namespace Window { namespace Backend { namespace SFML {

class EE_API cJoystickManagerSFML : public cJoystickManager {
	public:
		cJoystickManagerSFML();
		
		virtual ~cJoystickManagerSFML();
		
		void Update();

		void Close();

		void Open();
	protected:
		void Create( const Uint32& index );
};

}}}}

#endif

#endif
