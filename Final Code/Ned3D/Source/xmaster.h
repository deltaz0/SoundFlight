/*
Sam Johnson
Manages the XAudio2 engine and mastering voice.
There should only be one of this object.
*/

#ifndef XMASTER_H
#define XMASTER_H

#include <XAudio2.h>

class XMaster
{
public:
	XMaster();

	//the engine
	IXAudio2* g_engine;
	//the mastering voice, for our purposes we only use 1
	IXAudio2MasteringVoice* g_master;

	//create the xaudio2 engine
	int initEngine();
	//create the xaudio2 mastering voice
	int initMasterVoice();
	//release the xaudio2 engine
	int releaseEngine();
	
};

#endif