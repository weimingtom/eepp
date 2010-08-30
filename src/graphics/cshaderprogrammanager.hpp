#ifndef EE_GRAPHICSSHADERPROGRAMANAGER_HPP
#define EE_GRAPHICSSHADERPROGRAMANAGER_HPP

#include "base.hpp"
#include "cshaderprogram.hpp"

namespace EE { namespace Graphics {

class EE_API cShaderProgramManager : public tResourceManager<cShaderProgram>, public tSingleton<cShaderProgramManager> {
	friend class tSingleton<cShaderProgramManager>;
	public:
		cShaderProgramManager();

		virtual ~cShaderProgramManager();

		void Reload();
	protected:
};

}}

#endif
