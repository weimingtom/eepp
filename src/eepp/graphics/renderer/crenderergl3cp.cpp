#include <eepp/graphics/renderer/crenderergl3cp.hpp>

#ifdef EE_GL3_ENABLED

#include <eepp/graphics/renderer/rendererhelper.hpp>

namespace EE { namespace Graphics {

const char * EEGL3CP_STATES_NAME[] = {
	"dgl_Vertex",
	"dgl_Normal",
	"dgl_FrontColor"
};

const char * EEGL3CP_TEXTUREUNIT_NAMES[] = {
	"dgl_MultiTexCoord0",
	"dgl_MultiTexCoord1",
	"dgl_MultiTexCoord2",
	"dgl_MultiTexCoord3"
};

const char * EEGL3CP_PLANES_ENABLED_NAME[] = {
	"dgl_ClipEnabled[0]",
	"dgl_ClipEnabled[1]",
	"dgl_ClipEnabled[2]",
	"dgl_ClipEnabled[3]",
	"dgl_ClipEnabled[4]",
	"dgl_ClipEnabled[5]"
};

const char * EEGL3CP_PLANES_NAME[] = {
	"dgl_ClipPlane[0]",
	"dgl_ClipPlane[1]",
	"dgl_ClipPlane[2]",
	"dgl_ClipPlane[3]",
	"dgl_ClipPlane[4]",
	"dgl_ClipPlane[5]"
};

#if EE_PLATFORM != EE_PLATFORM_EMSCRIPTEN

const GLchar * EEGL3CP_SHADER_BASE_VS =
#include "shaders/basegl3cp.vert"

const GLchar * EEGL3CP_SHADER_BASE_FS =
#include "shaders/basegl3cp.frag"

#else

const GLchar * EEGL3CP_SHADER_BASE_VS =
#include "shaders/basegl3.vert"

const GLchar * EEGL3CP_SHADER_BASE_FS =
#include "shaders/basegl3.frag"

#endif

cRendererGL3CP::cRendererGL3CP() :
	mProjectionMatrix_id(0),
	mModelViewMatrix_id(0),
	mCurrentMode(0),
	mCurShader(NULL),
	mShaderPrev(NULL),
	mTexActive(1),
	mTexActiveLoc(-1),
	mPointSpriteLoc(-1),
	mPointSize(1.f),
	mCurActiveTex( 0 ),
	mCurTexCoordArray( 0 ),
	mVBOSizeAlloc( 1024 * 1024 ),
	mBiggestAlloc( 0 ),
	mLoaded( false )
{
	mQuadsSupported		= false;
	mQuadVertexs		= 6;

	mStack = eeNew( cMatrixStack, () );
	mStack->mProjectionMatrix.push	( glm::mat4( 1.0f ) ); // identity matrix
	mStack->mModelViewMatrix.push	( glm::mat4( 1.0f ) ); // identity matrix
}

cRendererGL3CP::~cRendererGL3CP() {
	for ( Uint32 i = 0; i < eeARRAY_SIZE( mVBO ); i++ ) {
		if ( 0 != mVBO[i] ) {
			glDeleteBuffersARB( 1, &mVBO[i] );
		}
	}

	DeleteVertexArrays( 1, &mVAO );

	eeSAFE_DELETE( mStack );

	#ifdef EE_DEBUG
	cLog::instance()->Write( "Biggest VBO allocation on GL3 Renderer: " + FileSystem::SizeToString( mBiggestAlloc ) );
	#endif
}

EEGL_version cRendererGL3CP::Version() {
	return GLv_3CP;
}

std::string cRendererGL3CP::VersionStr() {
	return "OpenGL 3 Core Profile";
}

void cRendererGL3CP::Init() {
	if ( !mLoaded ) {
		Uint32 i;

		cGL::Init();

		std::string vs( EEGL3CP_SHADER_BASE_VS );
		std::string fs( EEGL3CP_SHADER_BASE_FS );

		mBaseVertexShader = vs;

		for ( i = 0; i < EEGL_ARRAY_STATES_COUNT; i++ ) {
			mAttribsLoc[ i ]		= -1;
			mAttribsLocStates[ i ]	= 0;
		}

		for ( i = 0; i < eeARRAY_SIZE(mVBO); i++ )
			mVBO[i] = 0;

		for ( i = 0; i < EE_MAX_PLANES; i++ ) {
			mPlanes[i]			= -1;
			mPlanesStates[i]	= 0;
		}

		for ( i = 0; i < EE_MAX_TEXTURE_UNITS; i++ ) {
			mTextureUnits[i] = -1;
			mTextureUnitsStates[i]	= 0;
		}

		cShader::Ensure( false );

		mShaders[ EEGL3CP_SHADER_BASE ]			= cShaderProgram::New( vs.c_str(), vs.size(), fs.c_str(), fs.size() );
		mShaders[ EEGL3CP_SHADER_BASE ]->SetReloadCb( cb::Make1( this, &cRendererGL3CP::ReloadShader ) );

		cShader::Ensure( true );
	} else {
		mCurShader = NULL;

		mShaders[ EEGL3CP_SHADER_BASE ]->Reload();
	}

	GenVertexArrays( 1, &mVAO );
	BindVertexArray( mVAO );

	glGenBuffersARB( EEGL_ARRAY_STATES_COUNT+5, &mVBO[0] );

	AllocateBuffers( mVBOSizeAlloc );

	ClientActiveTexture( GL_TEXTURE0 );

	SetShader( mShaders[ EEGL3CP_SHADER_BASE ] );

	mLoaded = true;
}

GLuint cRendererGL3CP::BaseShaderId() {
	return mCurShader->Handler();
}

void cRendererGL3CP::ReloadCurrentShader() {
	ReloadShader( mCurShader );
}

void cRendererGL3CP::ReloadShader( cShaderProgram * Shader ) {
	mCurShader = NULL;

	SetShader( Shader );
}

void cRendererGL3CP::SetShader( const EEGL3CP_SHADERS& Shader ) {
	SetShader( mShaders[ Shader ] );
}

void cRendererGL3CP::SetShader( cShaderProgram * Shader ) {
	if ( NULL == Shader ) {
		Shader = mShaders[ EEGL3CP_SHADER_BASE ];
	}

	if ( mCurShader == Shader ) {
		return;
	}

	DisableClientState( GL_VERTEX_ARRAY );
	DisableClientState( GL_TEXTURE_COORD_ARRAY );
	DisableClientState( GL_COLOR_ARRAY );

	mShaderPrev				= mCurShader;
	mCurShader				= Shader;
	mProjectionMatrix_id	= mCurShader->UniformLocation( "dgl_ProjectionMatrix" );
	mModelViewMatrix_id		= mCurShader->UniformLocation( "dgl_ModelViewMatrix" );
	mTexActiveLoc			= mCurShader->UniformLocation( "dgl_TexActive" );
	mPointSpriteLoc			= mCurShader->UniformLocation( "dgl_PointSpriteActive" );
	mClippingEnabledLoc		= mCurShader->UniformLocation( "dgl_ClippingEnabled" );
	mCurActiveTex			= 0;

	Uint32 i;

	for ( i = 0; i < EEGL_ARRAY_STATES_COUNT; i++ ) {
		mAttribsLoc[ i ] = mCurShader->AttributeLocation( EEGL3CP_STATES_NAME[ i ] );
	}

	for ( i = 0; i < EE_MAX_PLANES; i++ ) {
		mPlanes[ i ] = mCurShader->UniformLocation( EEGL3CP_PLANES_NAME[ i ] );
	}

	for ( i = 0; i < EE_MAX_TEXTURE_UNITS; i++ ) {
		mTextureUnits[ i ] = mCurShader->AttributeLocation( EEGL3CP_TEXTUREUNIT_NAMES[ i ] );
	}

	glUseProgram( mCurShader->Handler() );

	if ( -1 != mAttribsLoc[ EEGL_VERTEX_ARRAY ] )
		EnableClientState( GL_VERTEX_ARRAY );

	if ( -1 != mAttribsLoc[ EEGL_COLOR_ARRAY ] )
		EnableClientState( GL_COLOR_ARRAY );

	if ( -1 != mTextureUnits[ mCurActiveTex ] )
		EnableClientState( GL_TEXTURE_COORD_ARRAY );

	GLenum CM = mCurrentMode;

	MatrixMode( GL_PROJECTION );
	UpdateMatrix();
	MatrixMode( GL_MODELVIEW );
	UpdateMatrix();
	MatrixMode( CM );

	if ( -1 != mTexActiveLoc ) {
		mCurShader->SetUniform( mTexActiveLoc, 1 );
	}

	mCurShader->SetUniform( mClippingEnabledLoc, 0 );

	for ( i = 0; i < EE_MAX_PLANES; i++ ) {
		if ( -1 != mPlanes[ i ] ) {
			mCurShader->SetUniform( EEGL3CP_PLANES_ENABLED_NAME[ i ], 0 );
		}
	}

	if ( -1 != mPointSpriteLoc ) {
		mCurShader->SetUniform( mPointSpriteLoc, 0 );
	}
}

void cRendererGL3CP::Enable( GLenum cap ) {
	switch ( cap ) {
		case GL_TEXTURE_2D:
		{
			if ( 0 == mTexActive ) {
				mTexActive = 1;

				mCurShader->SetUniform( mTexActiveLoc, mTexActive );
				mCurShader->SetUniform( mTextureUnits[ mCurActiveTex ], mCurActiveTex );
			}

			return;
		}
		case GL_CLIP_PLANE0:
		case GL_CLIP_PLANE1:
		case GL_CLIP_PLANE2:
		case GL_CLIP_PLANE3:
		case GL_CLIP_PLANE4:
		case GL_CLIP_PLANE5:
		{
			GLint plane = cap - GL_CLIP_PLANE0;

			if ( 0 == mPlanesStates[ plane ] ) {
				mPlanesStates[ plane ] = 1;

				PlaneStateCheck( true );

				mCurShader->SetUniform( EEGL3CP_PLANES_ENABLED_NAME[ plane ], 1 );
			}

			return;
		}
		case GL_POINT_SPRITE:
		{
			mCurShader->SetUniform( mPointSpriteLoc, 1 );

			return;
		}
	}

	cGL::Enable( cap );
}

void cRendererGL3CP::Disable ( GLenum cap ) {
	switch ( cap ) {
		case GL_TEXTURE_2D:
		{
			if ( 1 == mTexActive ) {
				mTexActive = 0;

				mCurShader->SetUniform( mTexActiveLoc, mTexActive );
			}
			return;
		}
		case GL_CLIP_PLANE0:
		case GL_CLIP_PLANE1:
		case GL_CLIP_PLANE2:
		case GL_CLIP_PLANE3:
		case GL_CLIP_PLANE4:
		case GL_CLIP_PLANE5:
		{
			GLint plane = cap - GL_CLIP_PLANE0;

			if ( 1 == mPlanesStates[ plane ] ) {
				mPlanesStates[ plane ] = 0;

				PlaneStateCheck( false );

				mCurShader->SetUniform( EEGL3CP_PLANES_ENABLED_NAME[ plane ], 0 );
			}

			return;
		}
		case GL_POINT_SPRITE:
		{
			mCurShader->SetUniform( mPointSpriteLoc, 0 );

			return;
		}
	}

	cGL::Disable( cap );
}

void cRendererGL3CP::EnableClientState( GLenum array ) {
	GLint state;

	if ( GL_TEXTURE_COORD_ARRAY == array ) {
		if ( -1 != ( state = mTextureUnits[ mCurActiveTex ] ) ) {
			mTextureUnitsStates[ mCurActiveTex ] = 1;

			glEnableVertexAttribArray( state );
		}
	} else {
		Int32 Pos = array - GL_VERTEX_ARRAY;

		if ( -1 != ( state = mAttribsLoc[ Pos ] ) ) {
			mAttribsLocStates[ Pos ] = 1;

			glEnableVertexAttribArray( state );
		}
	}
}

void cRendererGL3CP::DisableClientState( GLenum array ) {
	GLint state;

	if ( GL_TEXTURE_COORD_ARRAY == array ) {
		if ( -1 != ( state = mTextureUnits[ mCurActiveTex ] ) ) {
			mTextureUnitsStates[ mCurActiveTex ] = 0;

			glDisableVertexAttribArray( state );
		}
	} else {
		Int32 Pos = array - GL_VERTEX_ARRAY;

		if ( -1 != ( state = mAttribsLoc[ Pos ] ) ) {
			mAttribsLocStates[ Pos ] = 0;

			glDisableVertexAttribArray( state );
		}
	}
}

void cRendererGL3CP::VertexPointer ( GLint size, GLenum type, GLsizei stride, const GLvoid * pointer, GLuint allocate ) {
	const GLint index = mAttribsLoc[ EEGL_VERTEX_ARRAY ];

	#ifdef EE_DEBUG
	mBiggestAlloc = eemax( mBiggestAlloc, allocate );
	#endif

	if ( -1 != index ) {
		BindVertexArray( mVAO );

		if ( allocate > mVBOSizeAlloc ) {
			AllocateBuffers( allocate );
		}

		glBindBufferARB( GL_ARRAY_BUFFER, mVBO[ EEGL_VERTEX_ARRAY ]			);
		glBufferSubDataARB( GL_ARRAY_BUFFER, 0, allocate, pointer );

		if ( 0 == mAttribsLocStates[ EEGL_VERTEX_ARRAY ] ) {
			mAttribsLocStates[ EEGL_VERTEX_ARRAY ] = 1;

			glEnableVertexAttribArray( index );
		}

		if ( type == GL_UNSIGNED_BYTE ) {
			glVertexAttribPointerARB( index, size, type, GL_TRUE, stride, 0 );
		} else {
			glVertexAttribPointerARB( index, size, type, GL_FALSE, stride, 0 );
		}
	}
}

void cRendererGL3CP::ColorPointer ( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer, GLuint allocate ) {
	const GLint index = mAttribsLoc[ EEGL_COLOR_ARRAY ];

	#ifdef EE_DEBUG
	mBiggestAlloc = eemax( mBiggestAlloc, allocate );
	#endif

	if ( -1 != index ) {
		BindVertexArray( mVAO );

		if ( allocate > mVBOSizeAlloc ) {
			AllocateBuffers( allocate );
		}

		glBindBufferARB( GL_ARRAY_BUFFER, mVBO[ EEGL_COLOR_ARRAY ]				);
		glBufferSubDataARB( GL_ARRAY_BUFFER, 0, allocate, pointer );

		if ( 0 == mAttribsLocStates[ EEGL_COLOR_ARRAY ] ) {
			mAttribsLocStates[ EEGL_COLOR_ARRAY ] = 1;

			glEnableVertexAttribArray( index );
		}

		if ( type == GL_UNSIGNED_BYTE ) {
			glVertexAttribPointerARB( index, size, type, GL_TRUE, stride, 0 );
		} else {
			glVertexAttribPointerARB( index, size, type, GL_FALSE, stride, 0 );
		}
	}
}

void cRendererGL3CP::TexCoordPointer ( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer, GLuint allocate ) {
	const GLint index = mTextureUnits[ mCurActiveTex ];

	#ifdef EE_DEBUG
	mBiggestAlloc = eemax( mBiggestAlloc, allocate );
	#endif

	if ( -1 != index ) {
		BindVertexArray( mVAO );

		if ( allocate > mVBOSizeAlloc ) {
			AllocateBuffers( allocate );
		}

		glBindBufferARB( GL_ARRAY_BUFFER, mCurTexCoordArray );
		glBufferSubDataARB( GL_ARRAY_BUFFER, 0, allocate, pointer );

		if ( 0 == mTextureUnitsStates[ mCurActiveTex ] ) {
			mTextureUnitsStates[ mCurActiveTex ] = 1;

			glEnableVertexAttribArray( index );
		}

		glVertexAttribPointerARB( index, size, type, GL_FALSE, stride, 0 );
	}
}

GLint cRendererGL3CP::GetStateIndex( const Uint32& State ) {
	eeASSERT( State < EEGL_ARRAY_STATES_COUNT );

	if ( EEGL_TEXTURE_COORD_ARRAY == State )
		return mTextureUnits[ mCurActiveTex ];

	return mAttribsLoc[ State ];
}

void cRendererGL3CP::PlaneStateCheck( bool tryEnable ) {
	GLint i;

	if (  tryEnable  ) {
		for ( i = 0; i < EE_MAX_PLANES; i++ ) {
			if ( 0 != mPlanesStates[ i ] ) {
				mCurShader->SetUniform( mClippingEnabledLoc, 1 );
				return;
			}
		}
	} else {
		for ( i = 0; i < EE_MAX_PLANES; i++) {
			if ( 0 != mPlanesStates[ i ] ) {
				return;
			}
		}

		mCurShader->SetUniform( mClippingEnabledLoc, 0 );
	}
}

void cRendererGL3CP::UpdateMatrix() {
	switch ( mCurrentMode ) {
		case GL_PROJECTION:
		{
			if ( -1 != mProjectionMatrix_id ) {
				mCurShader->SetUniformMatrix( mProjectionMatrix_id, &mStack->mProjectionMatrix.top()[0][0] );
			}

			break;
		}
		case GL_MODELVIEW:
		{
			if ( -1 != mModelViewMatrix_id ) {
				mCurShader->SetUniformMatrix( mModelViewMatrix_id, &mStack->mModelViewMatrix.top()[0][0] );
			}

			break;
		}
	}
}

void cRendererGL3CP::PushMatrix() {
	mStack->mCurMatrix->push( mStack->mCurMatrix->top() );
	UpdateMatrix();
}

void cRendererGL3CP::PopMatrix() {
	mStack->mCurMatrix->pop();
	UpdateMatrix();
}

void cRendererGL3CP::LoadIdentity() {
	mStack->mCurMatrix->top() = glm::mat4(1.0);
	UpdateMatrix();
}

void cRendererGL3CP::MultMatrixf ( const GLfloat * m ) {
	mStack->mCurMatrix->top() *= toGLMmat4( m );
	UpdateMatrix();
}

void cRendererGL3CP::Translatef( GLfloat x, GLfloat y, GLfloat z ) {
	mStack->mCurMatrix->top() *= glm::translate( glm::vec3( x, y, z ) );
	UpdateMatrix();
}

void cRendererGL3CP::Rotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z ) {
	mStack->mCurMatrix->top() *= glm::rotate( angle, glm::vec3( x, y, z ) );
	UpdateMatrix();
}

void cRendererGL3CP::Scalef( GLfloat x, GLfloat y, GLfloat z ) {
	mStack->mCurMatrix->top() *= glm::scale( glm::vec3( x, y, z ) );
	UpdateMatrix();
}

void cRendererGL3CP::Ortho( GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar ) {
	mStack->mCurMatrix->top() *= glm::ortho( left, right, bottom, top , zNear, zFar );
	UpdateMatrix();
}

void cRendererGL3CP::LookAt( GLfloat eyeX, GLfloat eyeY, GLfloat eyeZ, GLfloat centerX, GLfloat centerY, GLfloat centerZ, GLfloat upX, GLfloat upY, GLfloat upZ ) {
	mStack->mCurMatrix->top() *= glm::lookAt( glm::vec3(eyeX, eyeY, eyeZ), glm::vec3(centerX, centerY, centerZ), glm::vec3(upX, upY, upZ) );
	UpdateMatrix();
}

void cRendererGL3CP::Perspective ( GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar ) {
	mStack->mCurMatrix->top() *= glm::perspective( fovy, aspect, zNear, zFar );
	UpdateMatrix();
}

void cRendererGL3CP::LoadMatrixf( const GLfloat * m ) {
	mStack->mCurMatrix->top() = toGLMmat4( m );
	UpdateMatrix();
}

void cRendererGL3CP::Frustum( GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near_val, GLfloat far_val ) {
	mStack->mCurMatrix->top() *= glm::frustum( left, right, bottom, top, near_val, far_val );
	UpdateMatrix();
}

void cRendererGL3CP::GetCurrentMatrix( GLenum mode, GLfloat * m ) {
	switch ( mode ) {
		case GL_PROJECTION:
		case GL_PROJECTION_MATRIX:
		{
			fromGLMmat4( mStack->mProjectionMatrix.top(), m );
			break;
		}
		case GL_MODELVIEW:
		case GL_MODELVIEW_MATRIX:
		{
			fromGLMmat4( mStack->mModelViewMatrix.top(), m );
			break;
		}
	}
}

GLenum cRendererGL3CP::GetCurrentMatrixMode() {
	return mCurrentMode;
}

void cRendererGL3CP::MatrixMode(GLenum mode) {
	mCurrentMode = mode;

	switch ( mCurrentMode ) {
		case GL_PROJECTION:
		case GL_PROJECTION_MATRIX:
		{
			mStack->mCurMatrix = &mStack->mProjectionMatrix;
			break;
		}
		case GL_MODELVIEW:
		case GL_MODELVIEW_MATRIX:
		{
			mStack->mCurMatrix = &mStack->mModelViewMatrix;
			break;
		}
	}
}

void cRendererGL3CP::Clip2DPlaneEnable( const Int32& x, const Int32& y, const Int32& Width, const Int32& Height ) {
	GLfloat tX = (GLfloat)x;
	GLfloat tY = (GLfloat)y;
	GLfloat tW = (GLfloat)Width;
	GLfloat tH = (GLfloat)Height;

	glm::vec4 vclip_left	( 1.0	, 0.0	, 0.0	, -tX		);
	glm::vec4 vclip_right	( -1.0	, 0.0	, 0.0	, tX + tW	);
	glm::vec4 vclip_top		( 0.0	, 1.0	, 0.0	, -tY		);
	glm::vec4 vclip_bottom	( 0.0	, -1.0	, 0.0	, tY + tH	);

	glm::mat4 invMV = glm::inverse( mStack->mModelViewMatrix.top() );

	vclip_left		= vclip_left	* invMV;
	vclip_right		= vclip_right	* invMV;
	vclip_top		= vclip_top		* invMV;
	vclip_bottom	= vclip_bottom	* invMV;

	GLi->Enable(GL_CLIP_PLANE0);
	GLi->Enable(GL_CLIP_PLANE1);
	GLi->Enable(GL_CLIP_PLANE2);
	GLi->Enable(GL_CLIP_PLANE3);

	glUniform4fv( mPlanes[0], 1, static_cast<const GLfloat*>( &vclip_left[0]	)	);
	glUniform4fv( mPlanes[1], 1, static_cast<const GLfloat*>( &vclip_right[0]	)	);
	glUniform4fv( mPlanes[2], 1, static_cast<const GLfloat*>( &vclip_top[0]		)	);
	glUniform4fv( mPlanes[3], 1, static_cast<const GLfloat*>( &vclip_bottom[0]	)	);

	if ( mPushClip ) {
		mPlanesClipped.push_back( eeRectf( x, y, Width, Height ) );
	}
}

void cRendererGL3CP::Clip2DPlaneDisable() {
	if ( ! mPlanesClipped.empty() ) { // This should always be true
		mPlanesClipped.pop_back();
	}

	if ( mPlanesClipped.empty() ) {
		GLi->Disable(GL_CLIP_PLANE0);
		GLi->Disable(GL_CLIP_PLANE1);
		GLi->Disable(GL_CLIP_PLANE2);
		GLi->Disable(GL_CLIP_PLANE3);
	} else {
		eeRectf R( mPlanesClipped.back() );
		mPushClip = false;
		Clip2DPlaneEnable( R.Left, R.Top, R.Right, R.Bottom );
		mPushClip = true;
	}
}

void cRendererGL3CP::PointSize( GLfloat size ) {
	mCurShader->SetUniform( "dgl_PointSize", size );

	mPointSize = size;
}

void cRendererGL3CP::ClipPlane( GLenum plane, const GLdouble * equation ) {
	Int32 nplane	= plane - GL_CLIP_PLANE0;
	Int32 location;

	if ( nplane < EE_MAX_PLANES ) {
		location = mPlanes[ nplane ];
	} else {
		std::string planeNum( "dgl_ClipPlane[" + String::ToStr( nplane ) + "]" );

		location = glGetUniformLocation( mCurShader->Handler(), (GLchar*)&planeNum[0] );
	}

	glm::vec4 teq( equation[0], equation[1], equation[2], equation[3] );

	teq = teq * glm::inverse( mStack->mModelViewMatrix.top() );		/// Apply the inverse of the model view matrix to the equation

	glUniform4f( location, (GLfloat)teq[0], (GLfloat)teq[1], (GLfloat)teq[2], (GLfloat)teq[3] );
}

GLfloat cRendererGL3CP::PointSize() {
	return mPointSize;
}

void cRendererGL3CP::ClientActiveTexture( GLenum texture ) {
	mCurActiveTex = texture - GL_TEXTURE0;

	if ( mCurActiveTex >= EE_MAX_TEXTURE_UNITS )
		mCurActiveTex = 0;

	switch ( mCurActiveTex )
	{
		case 0: mCurTexCoordArray = mVBO[ EEGL_TEXTURE_COORD_ARRAY ]; break;
		case 1: mCurTexCoordArray = mVBO[ EEGL_TEXTURE_COORD_ARRAY+1 ]; break;
		case 2: mCurTexCoordArray = mVBO[ EEGL_TEXTURE_COORD_ARRAY+2 ]; break;
		case 3: mCurTexCoordArray = mVBO[ EEGL_TEXTURE_COORD_ARRAY+3 ]; break;
	}
}

void cRendererGL3CP::TexEnvi( GLenum target, GLenum pname, GLint param ) {
	//! @TODO: Implement TexEnvi
}

std::string cRendererGL3CP::GetBaseVertexShader() {
	return mBaseVertexShader;
}

GLint cRendererGL3CP::Project( GLfloat objx, GLfloat objy, GLfloat objz, const GLfloat modelMatrix[16], const GLfloat projMatrix[16], const GLint viewport[4], GLfloat *winx, GLfloat *winy, GLfloat *winz ) {
	glm::vec3 tv3( glm::project( glm::vec3( objx, objy, objz ), toGLMmat4( modelMatrix ), toGLMmat4( projMatrix ), glm::vec4( viewport[0], viewport[1], viewport[2], viewport[3] ) ) );

	if ( NULL != winx )
		*winx = tv3.x;

	if ( NULL != winy )
		*winy = tv3.y;

	if ( NULL != winz )
		*winz = tv3.z;

	return GL_TRUE;
}

GLint cRendererGL3CP::UnProject( GLfloat winx, GLfloat winy, GLfloat winz, const GLfloat modelMatrix[16], const GLfloat projMatrix[16], const GLint viewport[4], GLfloat *objx, GLfloat *objy, GLfloat *objz ) {
	glm::vec3 tv3( glm::unProject( glm::vec3( winx, winy, winz ), toGLMmat4( modelMatrix ), toGLMmat4( projMatrix ), glm::vec4( viewport[0], viewport[1], viewport[2], viewport[3] ) ) );

	if ( NULL != objx )
		*objx = tv3.x;

	if ( NULL != objy )
		*objy = tv3.y;

	if ( NULL != objz )
		*objz = tv3.z;

	return GL_TRUE;
}

void cRendererGL3CP::BindGlobalVAO() {
	BindVertexArray( mVAO );
}

void cRendererGL3CP::AllocateBuffers( const Uint32& size ) {
	if ( mVBOSizeAlloc != size )
		cLog::instance()->Write( "Allocating new VBO buffers size: " + String::ToStr( size ) );

	mVBOSizeAlloc = size;

	glBindBufferARB( GL_ARRAY_BUFFER, mVBO[ EEGL_VERTEX_ARRAY ] );
	glBufferDataARB( GL_ARRAY_BUFFER, mVBOSizeAlloc, NULL, GL_STREAM_DRAW );

	glBindBufferARB( GL_ARRAY_BUFFER, mVBO[ EEGL_COLOR_ARRAY ] );
	glBufferDataARB( GL_ARRAY_BUFFER, mVBOSizeAlloc, NULL, GL_STREAM_DRAW );

	glBindBufferARB( GL_ARRAY_BUFFER, mVBO[ EEGL_TEXTURE_COORD_ARRAY ] );
	glBufferDataARB( GL_ARRAY_BUFFER, mVBOSizeAlloc, NULL, GL_STREAM_DRAW );

	glBindBufferARB( GL_ARRAY_BUFFER, mVBO[ EEGL_TEXTURE_COORD_ARRAY+1 ] );
	glBufferDataARB( GL_ARRAY_BUFFER, mVBOSizeAlloc, NULL, GL_STREAM_DRAW );

	glBindBufferARB( GL_ARRAY_BUFFER, mVBO[ EEGL_TEXTURE_COORD_ARRAY+2 ] );
	glBufferDataARB( GL_ARRAY_BUFFER, mVBOSizeAlloc, NULL, GL_STREAM_DRAW );

	glBindBufferARB( GL_ARRAY_BUFFER, mVBO[ EEGL_TEXTURE_COORD_ARRAY+3 ] );
	glBufferDataARB( GL_ARRAY_BUFFER, mVBOSizeAlloc, NULL, GL_STREAM_DRAW );
}

}}

#endif
