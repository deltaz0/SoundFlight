/*
Sam Johnson
Integrates master/source voices for Xaudio2 and provides
an interface for interacting with them.
Works like a mixer with a certain number of channels
Create the XSound object and use its functions to make
sounds happen.
Example:
	XSound* myxsound = new XSound;
	int mychannel = myxsound->addSourceVoice("somefile.wav");
	myxsound->startVoiceAudio(mychannel);
	myxsound->submitSound(mychannel);
*/

#ifndef XSOUND_H
#define XSOUND_H

#include <xaudio2.h>
#include "xmaster.h"
#include "xsource.h"

class XSound
{
public:
	XSound();
	
	//Initializes the master voice and engine
	int initXSound();
	//Stops all audio and deletes all voices, frees mem
	int shutDownXSound();

	//Adds a source voice to the next available channel
	//ARGS:
	//	filename - path to audio file to load
	//RETURN:
	//	int - the channel number the voice was loaded to
	int addSourceVoice(const char* filename);
	//Voice on channel starts consuming audio
	int startVoiceAudio(int channel);
	//Tell voice on channel to play a sound
	//Will queue the sound if already playing something
	int submitSound(int channel);
	//Flushes all buffered sounds and stops consuming audio
	int stopVoiceAudio(int channel);
	//Returns number of active channels
	int getNumActiveChannels();
	//Change Volume of voice immediately
	//ARGS:
	//	channel - which channel to adjust volume
	//	volume - volume multiplier
	//		2.0 = 2x loud, 1.0 = no change, 0.0 = silent
	int changeVolume(int channel, float volume);

};

#endif