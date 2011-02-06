#ifndef EE_GRAPHICS_CRENDERERGL3_HPP
#define EE_GRAPHICS_CRENDERERGL3_HPP

#include "cgl.hpp"

#ifdef EE_GL3_ENABLED

// Xlib Madness
#ifdef True
#undef True
#endif

#ifdef False
#undef False
#endif

#include "../../helper/glm/gtx/matrix_projection.hpp"
#include "../../helper/glm/gtx/transform.hpp"
#include "../../helper/glm/gtx/transform2.hpp"

#define EE_MAX_PLANES 6

namespace EE { namespace Graphics {

class cRendererGL3 : public cGL {
	public:
		cRendererGL3();

		~cRendererGL3();

		EEGL_version Version();

		void Init();

		void PointSize( GLfloat size );

		GLfloat PointSize();

		void PushMatrix();

		void PopMatrix();

		void LoadIdentity();

		void Disable ( GLenum cap );

		void Enable( GLenum cap );

		void Translatef( GLfloat x, GLfloat y, GLfloat z );

		void Rotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z );

		void Scalef( GLfloat x, GLfloat y, GLfloat z );

		void MatrixMode (GLenum mode);

		void Ortho ( GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar );

		void LookAt( GLfloat eyeX, GLfloat eyeY, GLfloat eyeZ, GLfloat centerX, GLfloat centerY, GLfloat centerZ, GLfloat upX, GLfloat upY, GLfloat upZ );

		void Perspective ( GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar );

		void EnableClientState( GLenum array );

		void DisableClientState( GLenum array );

		void VertexPointer ( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer, GLuint allocate );

		void ColorPointer ( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer, GLuint allocate );

		void TexCoordPointer ( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer, GLuint allocate );

		void ClientActiveTexture( GLenum texture );

		GLuint BaseShaderId();

		void SetShader( cShaderProgram * Shader );

		void SetShader( const EEGL_SHADERS& Shader );

		GLint GetStateIndex( const Uint32& State );

		void Clip2DPlaneEnable( const Int32& x, const Int32& y, const Int32& Width, const Int32& Height );

		void Clip2DPlaneDisable();

		void MultMatrixf ( const GLfloat *m );

		void ClipPlane( GLenum plane, const GLdouble *equation );

		void TexEnvi( GLenum target, GLenum pname, GLint param );

		void LoadMatrixf( const GLfloat *m );

		void Frustum( GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near_val, GLfloat far_val );

		void GetCurrentMatrix( GLenum mode, GLfloat * m );

		glm::mat4 toGLMmat4( const GLfloat * m );

		void fromGLMmat4( glm::mat4 from, GLfloat * to );

		GLenum GetCurrentMatrixMode();
	protected:
		std::stack<glm::mat4>	mProjectionMatrix;		// cpu-side
		GLint					mProjectionMatrix_id;	// cpu-side hook to shader uniform
		std::stack<glm::mat4>	mModelViewMatrix;		// cpu-side
		GLint					mModelViewMatrix_id;	// cpu-side hook to shader uniform
		GLenum					mCurrentMode;
		std::stack<glm::mat4>*	mCurMatrix;
		cShaderProgram *		mShaders[ EEGL_SHADERS_COUNT ];
		cShaderProgram *		mCurShader;
		GLuint					mVAO;
		GLuint					mVBO[ EEGL_ARRAY_STATES_COUNT ];
		GLint					mStates[ EEGL_ARRAY_STATES_COUNT ];
		GLint					mPlanes[ EE_MAX_PLANES ];
		GLint					mPlanesStates[ EE_MAX_PLANES ];
		cShaderProgram *		mShaderPrev;
		Int32					mTexActive;
		GLint					mTexActiveLoc;
		GLint					mPointSpriteLoc;
		GLfloat					mPointSize;

		void UpdateMatrix();

		void PlaneStateCheck( bool tryEnable );
};

}}

#endif

#endif
