#ifndef EE_GRAPHICSCTEXTCACHE_H
#define EE_GRAPHICSCTEXTCACHE_H

#include <eepp/graphics/base.hpp>
#include <eepp/graphics/fonthelper.hpp>

namespace EE { namespace Graphics {

class cFont;

/** @brief Cached text for a fast font rendering. */
class EE_API cTextCache {
	public:
		cTextCache( cFont * font, const String& text = "", eeColorA FontColor = eeColorA(255,255,255,255), eeColorA FontShadowColor = eeColorA(0,0,0,255) );

		cTextCache();

		~cTextCache();

		void Create( cFont * font, const String& text = "", eeColorA FontColor = eeColorA(255,255,255,255), eeColorA FontShadowColor = eeColorA(0,0,0,255) );

		cFont * Font() const;

		void Font( cFont * font );

		String& Text();

		void Text( const String& text );

		eeFloat GetTextWidth();

		eeFloat GetTextHeight();

		const std::vector<eeFloat>& LinesWidth();

		std::vector<eeVertexCoords>& VertextCoords();

		std::vector<eeColorA>& Colors();

		void Draw( const eeFloat& X, const eeFloat& Y, const Uint32& Flags = 0, const eeFloat& Scale = 1.0f, const eeFloat& Angle = 0, EE_BLEND_MODE Effect = ALPHA_NORMAL );

		void Cache();

		/** @return The Font Color */
		const eeColorA& Color() const;

		/** Set the color of the string rendered */
		void Color(const eeColorA& Color);

		/** @return The Shadow Font Color */
		const eeColorA& ShadowColor() const;

		/** Set the shadow color of the string rendered */
		void ShadowColor(const eeColorA& Color);

		const eeInt& GetNumLines() const;
	protected:
		friend class cFont;

		String						mText;
		cFont * 					mFont;
		std::vector<eeFloat> 		mLinesWidth;
		eeFloat 					mCachedWidth;
		eeInt 						mNumLines;

		eeColorA					mFontColor;
		eeColorA					mFontShadowColor;

		std::vector<eeVertexCoords>	mRenderCoords;
		std::vector<eeColorA>		mColors;

		Uint32						mFlags;
		Uint32						mVertexNumCached;

		bool						mCachedCoords;

		void UpdateCoords();

		const bool& CachedCoords() const;

		void CachedCoords( const bool& cached );
		
		const eeUint& CachedVerts() const;

		void CachedVerts( const eeUint& num );
};

}}

#endif
