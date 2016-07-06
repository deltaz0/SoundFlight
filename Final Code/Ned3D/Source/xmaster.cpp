//see xmaster.h

#include <XAudio2.h>
#include "xmaster.h"

XMaster::XMaster()
{
}

int XMaster::initEngine()
{
	if( FAILED( XAudio2Create( &g_engine )))
	{
		return -1;
	}
	return 0;
}

int XMaster::initMasterVoice()
{
	if( FAILED( g_engine->CreateMasteringVoice( &g_master )))
	{
		return -2;
	}
	return 0;
}


int XMaster::releaseEngine()
{
	g_engine->Release();
	return 0;
}