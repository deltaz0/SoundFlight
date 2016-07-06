//see xsource.h

#include <XAudio2.h>
#include "xsource.h"
#include "xmaster.h"

//this is to prevent accidental queueing of too many buffers
const int MAX_BUFFERS_ON_VOICE = 4;

XSource::XSource()
{
}

XSource::XSource(const char* filename, XMaster* ptrToMaster)
{
	loadWavFile(filename);
	initVoiceToMaster(ptrToMaster);
}

int XSource::loadWavFile( const char* filename )
{
	if( !buffer.load(filename)){
		return -1;
	}
	return 0;
}

int XSource::initVoiceToMaster(XMaster* ptrToMaster)
{
	if( FAILED(ptrToMaster->g_engine->CreateSourceVoice( &g_source, buffer.wf() ))){
		return -2;
	}
	return 0;
}

int XSource::startPlaying()
{
	g_source->Start();
	return 0;
}

int XSource::submitBuffer()
{
	g_source->GetState(&tmpVoiceState);
	if(tmpVoiceState.BuffersQueued < MAX_BUFFERS_ON_VOICE){
		g_source->SubmitSourceBuffer( buffer.xaBuffer() );
	}
	return 0;
}

int XSource::stopPlaying()
{	
	g_source->Stop();
	g_source->FlushSourceBuffers();
	return 0;
}

int XSource::changeVol(float volume)
{
	g_source->SetVolume(volume);
	return 0;
}