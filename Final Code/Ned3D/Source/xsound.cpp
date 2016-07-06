//see xsound.h

#include <xaudio2.h>
#include "xsound.h"
#include "xmaster.h"
#include "xsource.h"

//number of channels on our mixer
#define TOTAL_VOICE_CH 16

XMaster g_xmaster;
//create TOTAL_VOICE_CH source voices
//this behaves like a TOTAL_VOICE_CH channel mixer
XSource* g_xsource[TOTAL_VOICE_CH];
int numLoadedVoices;

XSound::XSound()
{
	initXSound();
}

int XSound::initXSound()
{
	numLoadedVoices = 0;

	//create the engine
	if( FAILED( g_xmaster.initEngine() ) )
	{
		return -1;
	}

	//create the mastering voice
	if( FAILED( g_xmaster.initMasterVoice() ) )
	{
		g_xmaster.releaseEngine();
		return -2;
	}

	return 0;
}

int XSound::addSourceVoice(const char* filename)
{
	if(numLoadedVoices < TOTAL_VOICE_CH){
		g_xsource[numLoadedVoices] = new XSource(filename, &g_xmaster);
		numLoadedVoices++;
		return numLoadedVoices - 1;
	}
	else
		return -1;
	return 0;
}

int XSound::startVoiceAudio(int channel)
{
	//start consuming audio in the source voice
	if(!g_xsource[channel]){
		return -1;
	}
	if(FAILED(g_xsource[channel]->startPlaying())){
		return -2;
	}
	return 0;
}

int XSound::submitSound(int channel)
{
	//submit buffer to source voice channel
	if(!g_xsource[channel]){
		return -1;
	}
	if(FAILED(g_xsource[channel]->submitBuffer())){
		return -2;
	}
	return 0;
}

int XSound::shutDownXSound()
{	
	//stop the voices and delete them
	for(int i=0;i<TOTAL_VOICE_CH;i++){
		if(g_xsource[i]){
			stopVoiceAudio(i);
			int x=0;
			delete g_xsource[i];
		}
		g_xsource[i] = NULL;
	}
	numLoadedVoices = 0;
	//release the engine
	g_xmaster.releaseEngine();
	return 0;
}

int XSound::stopVoiceAudio(int channel)
{
	if(!g_xsource[channel]){
		return -1;
	}
	//stop voice and flush its buffers
	g_xsource[channel]->stopPlaying();
	return 0;
}

int XSound::getNumActiveChannels()
{
	//returns number of active channels
	return numLoadedVoices;
}

int XSound::changeVolume(int channel, float volume)
{
	//change volume scalar for voice
	g_xsource[channel]->changeVol(volume);
		return 0;
}