#include "cshader.hpp"

namespace EE { namespace Graphics {

cShader::cShader( const Uint32& Type ) {
	Init( Type );
}

cShader::cShader( const Uint32& Type, const std::string& Filename ) {
    Init( Type );

	std::fstream fs;
	fs.open( Filename.c_str(), std::ios::in | std::ios::binary );

	if ( !fs.is_open() ) {
		cLog::instance()->Write( std::string( "Couldn't open shader object: " ) + Filename );
	}

	fs.seekg ( 0, ios::end );
	Int32 Length = fs.tellg();
	fs.seekg ( 0, ios::beg );
	std::vector<char> Buffer( Length + 1, 0 );
	fs.read( reinterpret_cast<char*> ( &Buffer[0] ), Length );
	fs.close();

    SetSource( Buffer );

    Compile();
}

cShader::~cShader() {
	glDeleteShader( GetId() );
}

void cShader::Init( const Uint32& Type ) {
	mType = Type;
	mValid = false;
	mCompiled = false;
	mGLId = glCreateShader( mType );
}

void cShader::SetSource( const std::string& Source ) {
	std::vector<char> _dst( Source.size(), 0 );
	std::string _src( Source.size(), 0 );
	_src = Source;

	memcpy( reinterpret_cast<char*>( &_dst[0] ), reinterpret_cast<char*>( &_src[0] ), Source.size() );

	SetSource( _src );
}

void cShader::SetSource( const std::vector<char>& Source ) {
	if ( IsCompiled() ) {
		cLog::instance()->Write( "Can't set source for compiled shaders" );
		return;
	}

	const char* src = &Source[0];

	glShaderSource( GetId(), 1, &src, NULL );
}

bool cShader::Compile() {
	if ( IsCompiled() ) {
		cLog::instance()->Write( " Can't compile a shader twice" );
		return false;
	}

	glCompileShader( GetId() );
	mCompiled = true;

	int Compiled;
	glGetShaderiv( GetId(), GL_COMPILE_STATUS, &Compiled );
	mValid = 0 != Compiled;

	GLsizei logsize, logarraysize;
	glGetShaderiv( GetId(), GL_INFO_LOG_LENGTH, &logarraysize );

	mCompileLog.resize( logarraysize - 1 );

	glGetShaderInfoLog( GetId(), logarraysize, &logsize, reinterpret_cast<GLchar*>( &mCompileLog[0] ) );

	if ( !mValid ) {
		cLog::instance()->Write( "Couldn't compile shader. Log follows:" );
		cLog::instance()->Write( mCompileLog );
	} else {
		cLog::instance()->Write( "Shader Loaded Succesfully" );
	}

	return mValid;
}

cVertexShader::cVertexShader() : cShader( GL_VERTEX_SHADER ) {}
cVertexShader::cVertexShader( const std::string& Filename ) : cShader( GL_VERTEX_SHADER, Filename ) {}
cFragmentShader::cFragmentShader() : cShader(GL_FRAGMENT_SHADER) {}
cFragmentShader::cFragmentShader( const std::string& Filename ) : cShader( GL_FRAGMENT_SHADER, Filename ) {}

}}
