#include <eepp/audio/cmusic.hpp>
#include <eepp/audio/csoundfile.hpp>
#include <eepp/system/cpackmanager.hpp>

namespace EE { namespace Audio {

cMusic::cMusic( std::size_t BufferSize ) :
	mFile ( NULL ),
	mDuration( 0.f ),
	mSamples( BufferSize )
{
}

cMusic::~cMusic() {
	Stop();
	eeSAFE_DELETE( mFile );
}

bool cMusic::OpenFromPack( cPack* Pack, const std::string& FilePackPath ) {
	if ( Pack->IsOpen() && Pack->ExtractFileToMemory( FilePackPath, mData ) )
		return OpenFromMemory( reinterpret_cast<const char*> ( mData.Data ), mData.DataSize );

	return false;
}

bool cMusic::OpenFromFile( const std::string& Filename ) {
	if ( !FileSystem::FileExists( Filename ) ) {
		if ( cPackManager::instance()->FallbackToPacks() ) {
			std::string tPath( Filename );

			cPack * tPack = cPackManager::instance()->Exists( tPath );

			if ( NULL != tPack ) {
				return OpenFromPack( tPack, tPath );
			}
		}

		return false;
	}

	// Create the sound file implementation, and open it in read mode
	Stop();
	eeSAFE_DELETE( mFile );

	mFile = cSoundFile::CreateRead( Filename );

	if ( NULL == mFile ) {
		eePRINTL( "Failed to open %s for reading", Filename.c_str() );
		return false;
	}

	// Compute the duration
	mDuration = static_cast<float>( mFile->GetSamplesCount() ) / mFile->GetSampleRate() / mFile->GetChannelCount();

	// Initialize the stream
	Initialize( mFile->GetChannelCount(), mFile->GetSampleRate() );

	eePRINTL( "Music file %s loaded.", Filename.c_str() );

	return true;
}

bool cMusic::OpenFromMemory( const char * Data, std::size_t SizeInBytes ) {
	Stop();
	eeSAFE_DELETE( mFile );

	// Create the sound file implementation, and open it in read mode
	mFile = cSoundFile::CreateRead( Data, SizeInBytes );

	if ( NULL == mFile ) {
		eePRINTL( "Failed to open music from memory for reading" );
		return false;
	}

	mDuration = static_cast<float>( mFile->GetSamplesCount() ) / mFile->GetSampleRate(); // Compute the duration

	Initialize( mFile->GetChannelCount(), mFile->GetSampleRate() ); // Initialize the stream

	eePRINTL( "Music file loaded from memory." );

	return true;
}

bool cMusic::OnStart() {
	return NULL != mFile && mFile->Restart();
}

bool cMusic::OnGetData( cSoundStream::Chunk& Data ) {
	if ( NULL != mFile ) {
		// Fill the chunk parameters
		Data.Samples   = &mSamples[0];
		Data.SamplesCount = mFile->Read( &mSamples[0], mSamples.size() );

		// Check if we have reached the end of the audio file
		return Data.SamplesCount == mSamples.size();
	}

	return false;
}

cTime cMusic::GetDuration() const {
	return Seconds( mDuration );
}

void cMusic::OnSeek( cTime timeOffset ) {
	if ( NULL != mFile ) {
		mFile->Seek( timeOffset );
	}
}

}}
