/*
Sam Johnson
Initializes and manages an XAudio2 source voice.
Loads a single wav file.
*/

#ifndef XSOURCE_H
#define XSOURCE_H

#include <xaudio2.h>
#include "wave.h"
#include "xmaster.h"

class XSource
{
public:
	XSource();
	XSource(const char* filename, XMaster* ptrToMaster);

	//the source voice
	IXAudio2SourceVoice* g_source;
	//buffer to hold format for wav
	Wave buffer;
	//structure that stores useful data about our voice
	XAUDIO2_VOICE_STATE tmpVoiceState;

	//Loads a wav file for the voice
	//ARGS:
	//	filename - the name of the file to load
	int loadWavFile( const char* filename );
	//Has the master initialize the voice
	//This should only be called once
	//ARGS:
	//	ptrToMaster - ptr to the one master voice
	int initVoiceToMaster( XMaster* ptrToMaster);
	//Tells the voice to start consuming audio
	//It will immediately play or queue buffers sent to it
	//If any buffers were queued before, they will start playing now
	int startPlaying();
	//Submits a buffer to the queue
	//If nothing is playing, will immediately play the sound
	//If currently playing something, it will add the sound to
	//the end of the queue of other queued buffers
	int submitBuffer();
	//Tells the voice to stop consuming audio
	//and flushes all current and queued buffers
	int stopPlaying();
	//Change volume
	int changeVol(float volume);

};

#endif