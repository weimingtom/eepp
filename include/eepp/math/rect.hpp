#ifndef EE_MATHCRECT_H
#define EE_MATHCRECT_H

#include <eepp/math/vector2.hpp>
#include <eepp/math/size.hpp>

namespace EE { namespace Math {

template <typename T>
class tRECT {
	public:
		T Left, Right, Top, Bottom;

		tRECT( T left, T top, T right, T bottom );

		tRECT( const Vector2<T>& Pos, const tSize<T>& Size );

		tRECT();

		tRECT<T> Copy();

		bool Intersect( const tRECT<T>& Rect );

		bool Contains( const tRECT<T>& Rect );

		bool Contains( const Vector2<T>& Vect );

		void Merge( const tRECT<T>& Rect );

		void Expand( const Vector2<T>& Vect );

		T Area();

		T MergedArea( const tRECT<T>& Rect );

		bool IntersectsSegment( const Vector2<T>& a, const Vector2<T>& b );

		/** Determine if a RECT and a Circle are intersecting
		* @param pos Circle position
		* @param radius Circle Radius
		* @return True if are intersecting
		*/
		bool IntersectCircle( Vector2<T> pos, const T& radius );

		/** Determine if a RECT ( representing a circle ) is intersecting another RECT ( also representing a circle ) */
		bool IntersectCircles( const tRECT<T>& b );

		Vector2<T> ClampVector( const Vector2<T>& Vect );

		Vector2<T> WrapVector( const Vector2<T>& Vect );

		Vector2<T> Pos();

		Vector2<T> Center();

		tSize<T> Size();

		void Scale( T scale, const Vector2<T>& center );

		void Scale( T scale );

		void Scale( Vector2<T> scale, const Vector2<T>& center );

		void Scale( Vector2<T> scale );
};

template <typename T>
bool operator ==(const tRECT<T>& R1, const tRECT<T>& R2) {
	return ( R1.Left == R2.Left ) && ( R1.Right == R2.Right ) && ( R1.Top == R2.Top ) && ( R1.Bottom == R2.Bottom );
}

template <typename T>
bool operator !=(const tRECT<T>& R1, const tRECT<T>& R2) {
	return ( R1.Left != R2.Left ) || ( R1.Right != R2.Right ) || ( R1.Top != R2.Top ) || ( R1.Bottom != R2.Bottom );
}

template <typename T>
tRECT<T>::tRECT(T left, T top, T right, T bottom) {
	Left = left; Right = right; Top = top; Bottom = bottom;
}

template <typename T>
tRECT<T> tRECT<T>::Copy() {
	return tRECT<T>( Left, Top, Right, Bottom );
}

template <typename T>
tRECT<T>::tRECT( const Vector2<T>& Pos, const tSize<T>& Size ) {
	Left = Pos.x;
	Top = Pos.y;
	Right = Left + Size.Width();
	Bottom = Top + Size.Height();
}

template <typename T>
tRECT<T>::tRECT() : Left(0), Right(0), Top(0), Bottom(0) {}

template <typename T>
bool tRECT<T>::Contains( const tRECT<T>& Rect ) {
	return ( Left <= Rect.Left && Right >= Rect.Right && Top <= Rect.Top && Bottom >= Rect.Bottom );
}

template <typename T>
bool tRECT<T>::Intersect( const tRECT<T>& Rect ) {
	return !( Left > Rect.Right || Right < Rect.Left || Top > Rect.Bottom || Bottom < Rect.Top );
}

template <typename T>
bool tRECT<T>::Contains( const Vector2<T>& Vect ) {
	return ( Left <= Vect.x && Right >= Vect.x && Top <= Vect.y && Bottom >= Vect.y );
}

template <typename T>
Vector2<T> tRECT<T>::Pos() {
	return Vector2<T>( Left, Top );
}

template <typename T>
Vector2<T> tRECT<T>::Center() {
	return Vector2<T>( Left + ( ( Right - Left ) * 0.5 ), Top + ( ( Bottom - Top ) * 0.5 ) );
}

template <typename T>
tSize<T> tRECT<T>::Size() {
	return tSize<T>( Right - Left, Bottom - Top );
}

template <typename T>
void tRECT<T>::Merge( const tRECT<T>& Rect ) {
	Left	= eemin( Left	, Rect.Left		);
	Bottom	= eemin( Bottom	, Rect.Bottom	);
	Right	= eemax( Right	, Rect.Right	);
	Top		= eemax( Top	, Rect.Top		);
}

template <typename T>
void tRECT<T>::Expand( const Vector2<T>& Vect ) {
	Left	= eemin( Left	, Vect.x	);
	Bottom	= eemin( Bottom	, Vect.y	);
	Right	= eemax( Right	, Vect.x	);
	Top		= eemax( Top	, Vect.y	);
}

template <typename T>
T tRECT<T>::Area() {
	return ( Right - Left ) * ( Bottom - Top );
}

template <typename T>
T tRECT<T>::MergedArea( const tRECT<T>& Rect ) {
	return ( eemax( Right, Rect.Right ) - eemin( Left, Rect.Left ) ) * ( eemin( Bottom, Rect.Bottom ) - eemax( Top, Rect.Top ) );
}

template <typename T>
bool tRECT<T>::IntersectsSegment( const Vector2<T>& a, const Vector2<T>& b ) {
	tRECT<T> seg_bb = tRECT<T>( eemin( a.x, b.x ), eemin( a.y, b.y ), eemax( a.x, b.x ), eemax( a.y, b.y ) );

	if( Intersects( seg_bb ) ){
		Vector2<T> axis( b.y - a.y, a.x - b.x );
		Vector2<T> offset( ( a.x + b.x - Right - Left ), ( a.y + b.y - Bottom - Top ) );
		Vector2<T> extents( Right - Left, Bottom - Top );

		return ( eeabs( axis.Dot( offset ) ) < eeabs( axis.x * extents.x ) + eeabs( axis.y * extents.y ) );
	}

	return false;
}

template <typename T>
bool tRECT<T>::IntersectCircle( Vector2<T> pos, const T& radius ) {
	Vector2<T> tPos( pos );

	if (tPos.x < Left)		tPos.x = Left;
	if (tPos.x > Right)		tPos.x = Right;
	if (tPos.y < Top)		tPos.y = Top;
	if (tPos.y > Bottom)	tPos.y = Bottom;

	if ( pos.Distance( tPos ) < radius )
		return true;

	return false;
}

template <typename T>
bool tRECT<T>::IntersectCircles( const tRECT<T>& b ) {
	eeFloat ra = (eeFloat)(Right - Left) * 0.5f;
	eeFloat rb = (eeFloat)(b.Right - b.Left) * 0.5f;
	eeFloat dist = ra + rb;
	eeFloat dx = (b.Left + rb) - (Left + ra);
	eeFloat dy = (b.Top + rb) - (Top + ra);
	eeFloat res = (dx * dx) + (dy * dy);

	if ( res <= (dist * dist))
		return true;
	return false;
}

template <typename T>
Vector2<T> tRECT<T>::ClampVector( const Vector2<T>& Vect ) {
	T x = eemin( eemax( Left	, Vect.x ), Right	);
	T y = eemin( eemax( Top		, Vect.y ), Bottom	);

	return Vector2<T>( x, y );
}

template <typename T>
Vector2<T> tRECT<T>::WrapVector( const Vector2<T>& Vect ) {
	T ix	= eeabs( Right - Left );
	T modx	= eemod( Vect.x - Left, ix );
	T x		= ( modx > 0 ) ? modx : modx + ix;

	T iy	= eeabs( Top - Top );
	T mody	= eemod( Vect.y - Top, iy );
	T y		= ( mody > 0 ) ? mody : mody + iy;

	return Vector2<T>( x + Left, y + Top );
}

template <typename T>
void tRECT<T>::Scale( Vector2<T> scale, const Vector2<T>& center ) {
	if ( scale != 1.0f ) {
		Left			= center.x	+	(	Left	-	center.x	)	*	scale.x;
		Top				= center.y	+	(	Top		-	center.y	)	*	scale.y;
		Right			= center.x	+	(	Right	-	center.x	)	*	scale.x;
		Bottom			= center.y	+	(	Bottom	-	center.y	)	*	scale.y;
	}
}

template <typename T>
void tRECT<T>::Scale( T scale, const Vector2<T>& center ) {
	Scale( eeVector2f( scale, scale ), center );
}

template <typename T>
void tRECT<T>::Scale( T scale ) {
	Scale( scale, Center() );
}

template <typename T>
void tRECT<T>::Scale( Vector2<T> scale ) {
	Scale( scale, Center() );
}

typedef tRECT<eeUint>		eeRectu;
typedef tRECT<eeFloat>		eeRectf;
typedef tRECT<eeFloat>		eeAABB; // Axis-Aligned Bounding Box
typedef tRECT<eeInt>		eeRecti;
typedef tRECT<Int32>		eeRect;

}}

#endif
