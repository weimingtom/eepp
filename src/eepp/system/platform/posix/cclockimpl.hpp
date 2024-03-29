#ifndef EE_SYSTEMCCLOCKIMPLPOSIX_H
#define EE_SYSTEMCCLOCKIMPLPOSIX_H

#include <eepp/base.hpp>

#if defined( EE_PLATFORM_POSIX )

#ifdef EE_HAVE_CLOCK_GETTIME
	#include <time.h>
#else
	#include <sys/time.h>
#endif

namespace EE { namespace System { namespace Platform {

class cClockImpl {
	public:
		cClockImpl();

		~cClockImpl();

		void Restart();

		unsigned long GetElapsedTime();
	private:
		#ifdef EE_HAVE_CLOCK_GETTIME
		struct timespec mStart;
		#else
		struct timeval mStart;
		#endif
	};
}}}

#endif

#endif
