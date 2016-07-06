/*
----o0o=================================================================o0o----
* Copyright (c) 2006, Ian Parberry
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the University of North Texas nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
----o0o=================================================================o0o----
*/

/// \file StateMenu.cpp
/// \brief Code for the StateMenu class.

/// Edited by the SAGE team, 2005
/// Last updated June 13, 2006

#include "Common/Renderer.h"
#include "StateMenu.h"
#include "Input/Input.h"
#include "WindowsWrapper/WindowsWrapper.h"
#include "game.h"
#include "DirectoryManager\DirectoryManager.h"

StateMenu::StateMenu():
m_textureHandle(-1)
{


}

void StateMenu::initiate()
{
	gDirectoryManager.setDirectory(eDirectorySounds);
	
  // load the texture
  m_textureHandle = gRenderer.cacheTextureDX("menu.png");
	m_blankTexture = gRenderer.cacheTextureDX("blankwhite.png");
	m_cubeTextureL = gRenderer.cacheTextureDX("cubeL.png");
	m_cubeTextureR = gRenderer.cacheTextureDX("cubeR.png");
	joySleep = timeGetTime();

	optionSelect = 0;

	for(int i=0; i<4; i++){
		laneColor[i].alpha = 255;
		laneColor[i].red = laneColor[i].green
		= laneColor[i].blue = 255;
		laneHue[i] = 0;
	}
}

void StateMenu::enterState()
{
  // make sure wireframe is off
  gRenderer.setWireframe(false);
	gGame.m_statePlaying.m_xsound->startVoiceAudio(8);
	gGame.m_statePlaying.m_xsound->submitSound(8);
	gGame.m_statePlaying.m_xsound->changeVolume(8, 0.4f);
	gGame.m_statePlaying.m_xsound->startVoiceAudio(9);
	gGame.m_statePlaying.m_xsound->changeVolume(9, 0.25f);
	gGame.m_statePlaying.m_xsound->startVoiceAudio(10);
	gGame.m_statePlaying.m_xsound->changeVolume(10, 0.25f);
	
}

void StateMenu::process(float dt)
{
  if (gInput.keyJustDown(DIK_ESCAPE, true))
    gWindowsWrapper.quit();

	/*
  if (gInput.keyJustDown(DIK_R, true))
	{
		gGame.m_song = 1;
		gGame.setBPM(175);
		gGame.m_statePlaying.resetGame();
    gGame.changeState(eGameStatePlaying);
	}

  if (gInput.keyJustDown(DIK_S, true))
  {
		gGame.m_song = 0;
		gGame.setBPM(120);
    gGame.m_statePlaying.resetGame();
    gGame.changeState(eGameStatePlaying);

  }
	*/

	if (gInput.keyJustDown(DIK_W, true))
	{
		gGame.m_statePlaying.m_xsound->stopVoiceAudio(9);
		gGame.m_statePlaying.m_xsound->startVoiceAudio(9);
		gGame.m_statePlaying.m_xsound->changeVolume(9, 0.25f);
		gGame.m_statePlaying.m_xsound->submitSound(9);
		optionSelect = (optionSelect + 2) % 3;
	}

  if (gInput.keyJustDown(DIK_S, true))
  {
		gGame.m_statePlaying.m_xsound->stopVoiceAudio(9);
		gGame.m_statePlaying.m_xsound->startVoiceAudio(9);
		gGame.m_statePlaying.m_xsound->changeVolume(9, 0.25f);
		gGame.m_statePlaying.m_xsound->submitSound(9);
		optionSelect = (optionSelect + 1) % 3;
  }


	if (gInput.keyJustDown(DIK_RETURN, true))
	{
		gGame.m_statePlaying.m_xsound->submitSound(10);
		if(optionSelect == 0){
			gGame.m_song = 0;
			gGame.setBPM(120);
			gGame.m_statePlaying.resetGame();
			gGame.changeState(eGameStatePlaying);
			gGame.m_statePlaying.m_xsound->stopVoiceAudio(8);
		}
		else if(optionSelect == 1){
			gGame.m_song = 1;
			gGame.setBPM(175);
			gGame.m_statePlaying.resetGame();
			gGame.changeState(eGameStatePlaying);
			gGame.m_statePlaying.m_xsound->stopVoiceAudio(8);
		}
		else if(optionSelect == 2){
			gGame.m_statePlaying.m_xsound->stopVoiceAudio(8);
			gGame.m_statePlaying.m_xsound->stopVoiceAudio(9);
			gGame.m_statePlaying.m_xsound->stopVoiceAudio(10);
			gWindowsWrapper.quit();
		}
	}


	if (gInput.joyEnabled())
  {
		int curTime = timeGetTime();
		if(curTime > joySleep + 250){
			if(gInput.joyPadPositionY() < 0.0f){
				gGame.m_statePlaying.m_xsound->stopVoiceAudio(9);
				gGame.m_statePlaying.m_xsound->startVoiceAudio(9);
				gGame.m_statePlaying.m_xsound->changeVolume(9, 0.25f);
				gGame.m_statePlaying.m_xsound->submitSound(9);
				optionSelect = (optionSelect + 2) % 3;
				joySleep = curTime;
			}
			else if(gInput.joyPadPositionY() > 0.0f){
				gGame.m_statePlaying.m_xsound->stopVoiceAudio(9);
				gGame.m_statePlaying.m_xsound->startVoiceAudio(9);
				gGame.m_statePlaying.m_xsound->changeVolume(9, 0.25f);
				gGame.m_statePlaying.m_xsound->submitSound(9);
				optionSelect = (optionSelect + 1) % 3;
				joySleep = curTime;
			}
		}
		if(gInput.joyButtonJustDown(2)){
			gGame.m_statePlaying.m_xsound->submitSound(10);
			if(optionSelect == 0){
				gGame.m_song = 0;
				gGame.setBPM(120);
				gGame.m_statePlaying.resetGame();
				gGame.changeState(eGameStatePlaying);
				gGame.m_statePlaying.m_xsound->stopVoiceAudio(8);
			}
			else if(optionSelect == 1){
				gGame.m_song = 1;
				gGame.setBPM(175);
				gGame.m_statePlaying.resetGame();
				gGame.changeState(eGameStatePlaying);
				gGame.m_statePlaying.m_xsound->stopVoiceAudio(8);
			}
			else if(optionSelect == 2){
				gGame.m_statePlaying.m_xsound->stopVoiceAudio(8);
				gGame.m_statePlaying.m_xsound->stopVoiceAudio(9);
				gGame.m_statePlaying.m_xsound->stopVoiceAudio(10);
				gWindowsWrapper.quit();
			}
		}
	}




	updateColors();
  

}

void StateMenu::renderScreen()
{
	
	float sw = (float)gRenderer.getScreenX() / 2.0f;
  float sh = (float)gRenderer.getScreenY() / 2.0f;
	RenderVertexTL backA[4];
  backA[0].p = Vector3(-sw,-sh,1.0f);
  backA[0].u = 0.0f; backA[0].v = 0.0f;
  backA[1].p = Vector3(sw,-sh,1.0f);
  backA[1].u = 1.0f; backA[1].v = 0.0f;
  backA[2].p = Vector3(sw,sh,1.0f);
  backA[2].u = 1.0f; backA[2].v = 1.0f;
  backA[3].p = Vector3(-sw,sh,1.0f);
  backA[3].u = 0.0f; backA[3].v = 1.0f;
	backA[0].argb = backA[1].argb =
		MAKE_ARGB(
			laneColor[0].alpha,
			laneColor[0].red,
			laneColor[0].green,
			laneColor[0].blue);
	backA[2].argb = backA[3].argb =
		MAKE_ARGB(
			laneColor[1].alpha,
			laneColor[1].red,
			laneColor[1].green,
			laneColor[1].blue);
	gRenderer.selectTexture(m_blankTexture);
  gRenderer.instance(Vector3(sw,sh,0.0f),EulerAngles::kEulerAnglesIdentity);
  gRenderer.renderSprite(backA);
  gRenderer.instancePop();

	

	float tw = sw;
	float th = sh;	
	sw = sw * 0.294f;
	sh = sh * 0.4f;	
  backA[0].p = Vector3(-sw,-sh,1.0f);
  backA[0].u = 0.0f; backA[0].v = 0.0f;
  backA[1].p = Vector3(sw,-sh,1.0f);
  backA[1].u = 1.0f; backA[1].v = 0.0f;
  backA[2].p = Vector3(sw,sh,1.0f);
  backA[2].u = 1.0f; backA[2].v = 1.0f;
  backA[3].p = Vector3(-sw,sh,1.0f);
  backA[3].u = 0.0f; backA[3].v = 1.0f;
	backA[0].argb = backA[1].argb =
		MAKE_ARGB(
			laneColor[3].alpha,
			laneColor[3].red,
			laneColor[3].green,
			laneColor[3].blue);
	backA[2].argb = backA[3].argb =
		MAKE_ARGB(
			laneColor[2].alpha,
			laneColor[2].red,
			laneColor[2].green,
			laneColor[2].blue);
	gRenderer.selectTexture(m_blankTexture);
  gRenderer.instance(Vector3(tw,th*1.212f,0.0f),EulerAngles::kEulerAnglesIdentity);
  gRenderer.renderSprite(backA);
  gRenderer.instancePop();

	

	gRenderer.setARGB(0XFF000000);
  gRenderer.selectTexture(m_textureHandle);
  gRenderer.renderTextureOverScreen();



	tw = tw * 2.0f;
	th = th * 2.0f;
	gRenderer.setARGB(
			MAKE_ARGB(
			laneColor[0].alpha,
			laneColor[0].red,
			laneColor[0].green,
			laneColor[0].blue));
	
	
	if(optionSelect==0){
		gRenderer.selectTexture(m_cubeTextureL);
		gRenderer.instance(Vector3(tw*0.361f,th*0.635f,0.0f),EulerAngles::kEulerAnglesIdentity);
		gRenderer.renderSprite(tw*0.03f,th*0.04f);
		gRenderer.instancePop();
		gRenderer.selectTexture(m_cubeTextureR);
		gRenderer.instance(Vector3(tw*0.65f,th*0.635f,0.0f),EulerAngles::kEulerAnglesIdentity);
		gRenderer.renderSprite(tw*0.03f,th*0.04f);
		gRenderer.instancePop();
	}
	else if(optionSelect==1){
		gRenderer.selectTexture(m_cubeTextureL);
		gRenderer.instance(Vector3(tw*0.361f,th*0.6925f,0.0f),EulerAngles::kEulerAnglesIdentity);
		gRenderer.renderSprite(tw*0.03f,th*0.04f);
		gRenderer.instancePop();
		gRenderer.selectTexture(m_cubeTextureR);
		gRenderer.instance(Vector3(tw*0.65f,th*0.6925f,0.0f),EulerAngles::kEulerAnglesIdentity);
		gRenderer.renderSprite(tw*0.03f,th*0.04f);
		gRenderer.instancePop();
	}
	else if(optionSelect==2){
		gRenderer.selectTexture(m_cubeTextureL);
		gRenderer.instance(Vector3(tw*0.391f,th*0.75f,0.0f),EulerAngles::kEulerAnglesIdentity);
		gRenderer.renderSprite(tw*0.03f,th*0.04f);
		gRenderer.instancePop();
		gRenderer.selectTexture(m_cubeTextureR);
		gRenderer.instance(Vector3(tw*0.62f,th*0.75f,0.0f),EulerAngles::kEulerAnglesIdentity);
		gRenderer.renderSprite(tw*0.03f,th*0.04f);
		gRenderer.instancePop();
	}

}




void StateMenu::updateColors() {
	int colorDuration = 10000;
	int colorMax = 200;
	int tempHue 
		= (int)(((timeGetTime() % colorDuration)
		/ (float)(colorDuration)) * 360.0f) % 360;
	for(int i=0; i<4; i++){
		if(i==0||i==2){
			laneHue[i] = (tempHue + (90 * i)) % 360;
		}
		else{
			laneHue[i] = (tempHue + (90 * (i-1)) + 45) % 360;
		}
		float cval = 1.0f;
		float xval = 1.0f -
			abs(((float)(laneHue[i] % 120) / 60.0f) - 1.0f );
		int cival = (int)(cval * colorMax);
		int xival = (int)(xval * colorMax);
		int hueSection = laneHue[i] / 60;
		switch(hueSection){
		case 0:
			laneColor[i].red = cival;
			laneColor[i].green = xival;
			laneColor[i].blue = 0;
			break;
		case 1:
			laneColor[i].red = xival;
			laneColor[i].green = cival;
			laneColor[i].blue = 0;
			break;
		case 2:
			laneColor[i].red = 0;
			laneColor[i].green = cival;
			laneColor[i].blue = xival;
			break;
		case 3:
			laneColor[i].red = 0;
			laneColor[i].green = xival;
			laneColor[i].blue = cival;
			break;
		case 4:
			laneColor[i].red = xival;
			laneColor[i].green = 0;
			laneColor[i].blue = cival;
			break;
		case 5:
			laneColor[i].red = cival;
			laneColor[i].green = 0;
			laneColor[i].blue = xival;
			break;
		default:
			break;
		}
	}
}