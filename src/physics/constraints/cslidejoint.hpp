#ifndef EE_PHYSICS_CSLIDEJOINT_HPP
#define EE_PHYSICS_CSLIDEJOINT_HPP

#include "cconstraint.hpp"

namespace EE { namespace Physics {

class cSlideJoint : public cConstraint {
	public:
		cSlideJoint( cBody * a, cBody *b, cVect anchr1, cVect anchr2, cpFloat min, cpFloat max );

		cVect Anchr1();

		void Anchr1( const cVect& anchr1 );

		cVect Anchr2();

		void Anchr2( const cVect& anchr2 );

		cpFloat Min();

		void Min( const cpFloat& min );

		cpFloat Max();

		void Max( const cpFloat& max );

		virtual void Draw();
};

}}

#endif
