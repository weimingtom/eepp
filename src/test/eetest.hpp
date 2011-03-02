#ifndef EE_EETEST_HPP
#define EE_EETEST_HPP

#include "../ee.h"

class cUITest : public cUIControlAnim {
	public:
		cUITest( cUIControlAnim::CreateParams& Params ) : cUIControlAnim( Params ) 	{ mOldColor = mBackground->Colors(); }

		virtual Uint32 OnMouseEnter( const eeVector2i& Pos, const Uint32 Flags )	{
			if ( 4 == mOldColor.size() ) {
				mBackground->Colors( eeColorA( mOldColor[0].R(), mOldColor[0].G(), mOldColor[0].B(), 200 ),
									eeColorA( mOldColor[1].R(), mOldColor[1].G(), mOldColor[1].B(), 200 ),
									eeColorA( mOldColor[2].R(), mOldColor[2].G(), mOldColor[2].B(), 200 ),
									eeColorA( mOldColor[3].R(), mOldColor[3].G(), mOldColor[3].B(), 200 )
								);
			} else {
				mBackground->Color( eeColorA( mOldColor[0].R(), mOldColor[0].G(), mOldColor[0].B(), 200 ) );
			}

			return 1;
		}

		virtual Uint32 OnMouseExit( const eeVector2i& Pos, const Uint32 Flags )	{
			if ( 4 == mOldColor.size() ) {
				mBackground->Colors( mOldColor[0], mOldColor[1], mOldColor[2], mOldColor[3] );
			} else {
				mBackground->Color( mOldColor[0] );
			}

			return 1;
		}

		virtual Uint32 OnMouseUp( const eeVector2i& Pos, const Uint32 Flags ) {
			cUIDragable::OnMouseUp( Pos, Flags );

			if ( cEngine::instance()->GetCurrentWindow()->GetInput()->MouseWheelUp() )
				Scale( Scale() + 0.1f );
			else if ( cEngine::instance()->GetCurrentWindow()->GetInput()->MouseWheelDown() )
				Scale( Scale() - 0.1f );

			return 1;
		}

		virtual Uint32 OnFocus() {
			ToFront();

			return 1;
		}

		const std::vector<eeColorA>& OldColor() { return mOldColor; }
	protected:
		std::vector<eeColorA> mOldColor;
};

enum CollisionTypes {
	BALL_TYPE,
	BLOCKING_SENSOR_TYPE,
	CATCH_SENSOR_TYPE,
};

typedef struct Emitter {
	int queue;
	int blocked;
	cVect position;
} Emitter;

class cEETest : private cThread {
	public:
		typedef cb::Callback0<void> SceneCb;

		void Init();
		void End();
		void Process();
		void Render();
		void Input();
		void ParticlesCallback(cParticle* P, cParticleSystem* Me);

		void ParticlesThread();
		void Particles();
		void LoadTextures();
		void CmdSetPartsNum ( const std::vector < std::wstring >& params );

		std::vector<cParticleSystem, eeAllocator< cParticleSystem > > PS;

		cTimeElapsed cElapsed;
		eeFloat PSElapsed;
	private:
		cEngine * EE;
		cWindow * mWindow;
		cTextureFactory* TF;
		cLog* Log;
		cInput* KM;
		cInputTextBuffer InBuf;

		bool run, side, aside;
		eeFloat ang, scale, alpha, Ang;
		eeFloat et;
		Int32 x, y;
		Uint32 lasttick;

		std::vector<Uint32> TN;
		std::vector<cTexture *> TNP;

		std::vector<Graphics::cShape*> Tiles;

		eeVector2i Mouse;
		eeVector2f Mousef;

		cSprite SP;
		cSprite CL1, CL2;
		cFont * FF;
		cFont * FF2;
		cFont * TTF;
		cFont * TTFB;

		cPrimitives PR;
		bool iL1, iL2;
		eeFloat HWidth, HHeight;

		cMusic * Mus;
		cSoundManager SndMng;

		bool DrawBack;

		cConsole Con;
		virtual void Run();

		eeVector2f Point;

		std::string MyPath;
		bool ShowParticles;

		cIsoMap Map;

		bool Wireframe;

		void CreateTiling( const bool& Wire );
		void RandomizeHeights();
		cPerlinNoise PerlinNoise;
		bool TreeTilingCreated;

		eeFloat H;
		Int32 NH;

		Uint8 Screen;
		SceneCb Scenes[6];
		void Screen1();
		void Screen2();
		void Screen3();
		void Screen4();
		void Screen5();

		cZip PAK;
		cZip PakTest;

		std::vector<Uint8> tmpv;
		std::vector<Uint8> MySong;

		cWaypoints WP;
		Int32 PartsNum;
		Uint32 Cursor[2];
		cTexture * CursorP[2];
		std::string mInfo;

		bool MultiViewportMode;

		cBatchRenderer Batch;
		eeFloat AnimVal;
		bool AnimSide;

		cView Views[2];

		cShaderProgram * mShaderProgram;
    	eeFloat mBlurFactor;
    	bool mUseShaders;

    	Uint32 mLastFPSLimit;
    	bool mWasMinimized;

		eeInt mWidth;
		eeInt mHeight;

		std::wstring mBuda;

		cResourceLoader mResLoad;
		void OnTextureLoaded( cResourceLoader * ObjLoaded );

		void CreateUI();
		void CreateShaders();

		void LoadFonts();

		cResourceLoader mFontLoader;
		void OnFontLoaded( cResourceLoader * ObjLoaded );

		cJoystickManager * JM;
		eeFloat mAxisX;
		eeFloat mAxisY;

		cTextureGroupLoader * mTGL;
		cSprite mBlindy;
		cSprite * mBlindyPtr;

		cFrameBuffer * mFBO;
		cVertexBuffer * mVBO;

		cTimeElapsed	mFTE;

		void ItemClick( const cUIEvent * Event );
		void MainClick( const cUIEvent * Event );
		void QuitClick( const cUIEvent * Event );
		void CloseClick( const cUIEvent * Event );
		void ButtonClick( const cUIEvent * Event );
		void OnValueChange( const cUIEvent * Event );
		void CreateDecoratedWindow();
		void CreateAquaTextureAtlas();

		cUIControlAnim * C;
		cUIScrollBar * mScrollBar;
		cUITextBox * mTextBoxValue;
		cUISlider * mSlider;
		cUIProgressBar * mProgressBar;
		cUIListBox * mListBox;
		cUIPopUpMenu * Menu;
		cUIWindow * mUIWindow;

		cTextCache mEEText;
		cTextCache mFBOText;
		cTextCache mInfoText;

		cSpace * mSpace;
		cBody * mMouseBody;
		cVect mMousePoint;
		cVect mMousePoint_last;
		cConstraint * mMouseJoint;
		void PhysicsCreate();
		void PhysicsUpdate();
		void PhysicsDestroy();

		void SetScreen( Uint32 num );

		cpBool blockerBegin( cArbiter *arb, cSpace *space, void *unused );
		void blockerSeparate( cArbiter *arb, cSpace *space, void *unused );
		void postStepRemove( cSpace *space, void * tshape, void *unused );
		cpBool catcherBarBegin( cArbiter *arb, cSpace *space, void *unused );

		void Demo1Create();
		void Demo1Update();
		void Demo1Destroy();

		void Demo2Create();
		void Demo2Update();
		void Demo2Destroy();

		Emitter emitterInstance;

		void ChangeDemo( Uint32 num );

		struct physicDemo {
			SceneCb init;
			SceneCb update;
			SceneCb destroy;
		};

		std::vector<physicDemo> mDemo;
		Int32					mCurDemo;
		cSprite *				mBoxSprite;
		cSprite *				mCircleSprite;
};

#endif
 