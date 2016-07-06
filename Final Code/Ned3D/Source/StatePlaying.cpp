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

/// \file StatePlaying.cpp
/// \brief Code for the StatePlaying class.

/// Edited by the SAGE team, 2005
/// Last updated June 13, 2006

#include "StatePlaying.h"
#include "game.h"
#include "input/input.h"
#include <algorithm>
#include "DirectoryManager/DirectoryManager.h"
#include "Common/MathUtil.h"
#include "Common/Renderer.h"
#include "Common/Random.h"
#include "Common/RotationMatrix.h"
#include "Console/Console.h"
#include "Graphics/ModelManager.h"
#include "Input/Input.h"
#include "Particle/ParticleEngine.h"
#include "Sound/SoundManager.h"
#include "Terrain/Terrain.h"
#include "WindowsWrapper/WindowsWrapper.h"
#include "Ned3DObjectManager.h"
#include "Game.h"
#include "xsound.h"
#include "miditextparser.h"
#include "DirectoryManager\DirectoryManager.h"

extern CRandom Random;

/// Global function that can be called from the console
bool StatePlaying::consoleSetFollowCamera(ParameterList* params,std::string* errorMessage)
{
  gGame.m_currentCam = gGame.m_statePlaying.m_tetherCamera;
	return true;
}

bool StatePlaying::consoleSetCameraTarget(ParameterList* params,std::string* errorMessage)
{
  unsigned int obj = gGame.m_statePlaying.m_objects->getObjectID(params->Strings[0]);
  if(obj == 0)
  {
    *errorMessage = "Object not found.";
    return false;
  }
  if(gGame.m_statePlaying.m_tetherCamera == NULL)
  {
    *errorMessage = "Tether camera not initialized.";
    return false;
  }
  gGame.m_statePlaying.m_tetherCamera->setTargetObject(obj);
  gGame.m_statePlaying.m_tetherCamera->reset();
  return true;
}

bool StatePlaying::consoleGodMode(ParameterList* params,std::string* errorMessage)
{
  PlaneObject::takeDamage = !params->Bools[0];
  return true;
}

bool StatePlaying::consoleTime(ParameterList* params, std::string* errorMessage) {

  std::string t_sTime;
  t_sTime = gGame.getTime();
  gConsole.printLine(t_sTime);

  return true;
}

StatePlaying::StatePlaying():
terrain(NULL),
water(NULL),
m_objects(NULL),
m_tetherCamera(NULL),
m_failedInstance(SoundManager::NOINSTANCE),
totalRotationTimeCam(0.0f)
{

}

void StatePlaying::initiate()
{
  float farClippingPlane = 2000.0f;

	m_nSkyTexture = gRenderer.cacheTextureDX("space.png");
	m_ncompleteBGTex = gRenderer.cacheTextureDX("gridshadowtexture.png");

	m_sfontHandle = gRenderer.addFont("Arial", 60,90,true);

	showDebugStuff = false;

	planezpos = 0.0f;
	planexpos = 0.0f;
	planeypos = 0.0f;
	currentLaneFlag = 0;

	planeOrigSpawnVal = Vector3(0,0,0);

  // Set far clipping plane
  gRenderer.setNearFarClippingPlanes(1.0f,farClippingPlane);    

  m_objects = new Ned3DObjectManager();	
	m_objects->setNumberOfDeadFrames(2);
	m_tetherCamera = new TetherCamera(m_objects);
	
	//-------- New XSound Stuff --------
	// Create XSound object
	m_xsound = new XSound();
	m_mtp[0] = new MidiTextParser();
	m_mtp[1] = new MidiTextParser();
	m_mtp[2] = new MidiTextParser();
	m_mtp[3] = new MidiTextParser();
	// Load Sound Directory
	gDirectoryManager.setDirectory(eDirectorySounds);
	// Load Sounds onto 8 mixer channels
	//song 0
	m_xsound->addSourceVoice("bdrums.wav");
	m_xsound->addSourceVoice("bperc.wav");
	m_xsound->addSourceVoice("bbass.wav");
	m_xsound->addSourceVoice("bsynth.wav");
	//song 1
	m_xsound->addSourceVoice("sofreshdrums.wav");
	m_xsound->addSourceVoice("sofreshfx.wav");
	m_xsound->addSourceVoice("sofreshbass.wav");
	m_xsound->addSourceVoice("sofreshlead.wav");
	//menu
	m_xsound->addSourceVoice("goadnb.wav");
	m_xsound->addSourceVoice("bleep.wav");
	m_xsound->addSourceVoice("select.wav");
	
	
	
	//-------- End XSound Stuff --------

  
  // Load models
  m_objects->setModelManager(gModelManager);
  gModelManager.importXml("models.xml");

  // Loads game objects like the crows, plane, and silo
	resetGame();
  
  // set fog
  gRenderer.setFogEnable(true);
  gRenderer.setFogDistance(farClippingPlane - 1000.0f,farClippingPlane);

  //gRenderer.setFogColor(MAKE_ARGB(0,60,180,254));
	//make sky black
	gRenderer.setFogColor(MAKE_ARGB(0,0,0,0));

  // set lights
  gRenderer.setAmbientLightColor(MAKE_ARGB(255,100,100,100));
  gRenderer.setDirectionalLightColor(0XFFFFFFFF);
  Vector3 dir = Vector3(5.0f,-5.0f, 6.0f);
  dir.normalize();
  gRenderer.setDirectionalLightVector(dir);
  	
  // Create water now that we know what camera to use
  float fov = degToRad(gGame.m_currentCam->fov);
  water = new Water(fov, farClippingPlane, "water.xml");
  
  m_objects->spawnWater(water);

  // add console commands  
  gConsole.addFunction("camerafollow","",consoleSetFollowCamera);
  gConsole.addFunction("cameratarget","s",consoleSetCameraTarget);
  gConsole.addFunction("godmode","b",consoleGodMode);


  // NEW
  gConsole.addFunction("timer","b",consoleTime);

  // New - Rotation
  m_fRotationTimeRemain = 0.0f;
  m_fNextCameraBank = 0.0f;

  // New - GridManager
  m_GridManager = new GridManager(Vector3(0,360,0), m_objects->getPlaneObject()->getOrientation());

  // New - Score
  m_nLargeText = gRenderer.addFont("Arial",15,15,true);
  m_nHugeText = gRenderer.addFont("Arial",30,30,true);

  int textY = gRenderer.getScreenY()/2;
  int leftPost = gRenderer.getScreenX()/4;
  leftPost *= 3;
  m_rectScore = IRectangle(leftPost,textY,gRenderer.getScreenX()-1,textY+20);
  m_rectMultiplier = m_rectScore;
  m_rectMultiplier.top = m_rectScore.bottom + 10;
  m_rectMultiplier.bottom = m_rectMultiplier.top+20;

	//need to reinitialize gridmanager stuff
}

void StatePlaying::shutdown()
{
  delete m_tetherCamera; m_tetherCamera = NULL;
  delete water; water = NULL;
	delete terrain; terrain = NULL;
  delete m_objects; m_objects = NULL;
	//-------- New XSound Cleanup Stuff --------
	m_xsound->shutDownXSound();
	delete m_xsound; m_xsound = NULL;
	for(int i=0; i<4; i++){
		delete m_mtp[i]; m_mtp[i] = NULL;
	}
	delete m_GridManager;
	//-------- End XSound Cleanup Stuff --------
}

void StatePlaying::exitState()
{
	m_xsound->stopVoiceAudio(0);
	m_xsound->stopVoiceAudio(1);
	m_xsound->stopVoiceAudio(2);
	m_xsound->stopVoiceAudio(3);
	m_xsound->stopVoiceAudio(4);
	m_xsound->stopVoiceAudio(5);
	m_xsound->stopVoiceAudio(6);
	m_xsound->stopVoiceAudio(7);
	m_xsound->stopVoiceAudio(8);
	m_xsound->stopVoiceAudio(9);
	m_xsound->stopVoiceAudio(10);
}

void StatePlaying::enterState()
{
  // start up the windmill sound
  //gSoundManager.play(m_windmillSound, m_windmillSoundInstance, true);
	gGame.m_tTimer.startTimer();
  gGame.m_tTimer.startCountdown();
  m_bSoundStart = false;

	m_GridManager->initVars();

	//if song 0
	if(gGame.m_song == 0){
		// Start consuming audio on the 4 channels
		
		m_xsound->startVoiceAudio(0);
		m_xsound->startVoiceAudio(1);
		m_xsound->startVoiceAudio(2);
		m_xsound->startVoiceAudio(3);
    // Sound gets submitted to buffer after countdown ends

		gDirectoryManager.setDirectory(eDirectorySounds);
		
		m_mtp[0]->clearVals();
		m_mtp[1]->clearVals();
		m_mtp[2]->clearVals();
		m_mtp[3]->clearVals();

		m_mtp[0]->parse("bdrums.txt",0);
		m_mtp[1]->parse("bperc.txt",1);
		m_mtp[2]->parse("bbass.txt",2);
		m_mtp[3]->parse("bsynth.txt",3);
		
	}
	else if(gGame.m_song == 1){
		// Start consuming audio on the 4 channels
		m_xsound->startVoiceAudio(4);
		m_xsound->startVoiceAudio(5);
		m_xsound->startVoiceAudio(6);
		m_xsound->startVoiceAudio(7);
    // Sound gets submitted to buffer after countdown ends

		srand(timeGetTime());

		gDirectoryManager.setDirectory(eDirectorySounds);

		m_mtp[0]->clearVals();
		m_mtp[1]->clearVals();
		m_mtp[2]->clearVals();
		m_mtp[3]->clearVals();
		
		m_mtp[0]->parse("sofreshdrums.txt",0);
		m_mtp[1]->parse("sofreshfx.txt",1);
		m_mtp[2]->parse("sofreshbass.txt",2);
		m_mtp[3]->parse("sofreshlead.txt",3);
	}
	for(int i=1;i<4;i++){
		adjustXSoundVolumeByActiveLane(i,false);
	}
}

void StatePlaying::process(float dt)
{ 
  PlaneObject *planeObject = m_objects->getPlaneObject();

	planezpos = planeObject->getPosition().z;
	planexpos = planeObject->getPosition().x;
	planeypos = planeObject->getPosition().y;
	currentLaneFlag = planeObject->getPlaneOrientFlag();

	planeOrigSpawnVal = planeObject->getOrigSpawn();

  // this should never happen but if it does leave
  if (planeObject == NULL) 
    return; 
    
  gConsole.process();
  
  // call process and move on all objects in the object manager

  // Only allow plane to move if countdown is false
  if(!gGame.m_tTimer.isCountdown()) {
    if(!m_bSoundStart) {
      m_bSoundStart = true;

      // Start the appropriate tracks
      if(gGame.m_song == 0) {
		    m_xsound->submitSound(0);
				m_xsound->submitSound(1);
				m_xsound->submitSound(2);
				m_xsound->submitSound(3);
      }
      else if(gGame.m_song == 1) {
		    m_xsound->submitSound(4);
				m_xsound->submitSound(5);
				m_xsound->submitSound(6);
				m_xsound->submitSound(7);
      }
    }
    m_objects->update(dt); 
  }
    
  // process escape key and space bar
  processInput();

  // update location of camera
  processCamera(dt);
    
  // allow water to process per frame movements
  water->process(dt);
 
  // render reflection
  if (Water::m_bReflection)
  {
    // render water reflection    
    Plane plane( 0, 1, 0, -water->getWaterHeight());
    //get water texture ready  
    water->m_reflection.beginReflectedScene(plane);
    renderScene(true);
    water->m_reflection.endReflectedScene();    
  }

}

void StatePlaying::adjustXSoundVolumeByActiveLane(int lane, bool active){
	if(gGame.m_song == 0){
		if(active)
			m_xsound->changeVolume(lane, 1.0f);
		else
			m_xsound->changeVolume(lane, 0.0f);
	}
	else if(gGame.m_song == 1){
		if(active)
			m_xsound->changeVolume(lane + 4, 1.0f);
		else
			m_xsound->changeVolume(lane + 4, 0.0f);
	}
}

void StatePlaying::renderScreen() {

  // render the entire scene
  renderScene();

  PlaneObject* plane = m_objects->getPlaneObject();

  if (plane != NULL)
  {
    //render plane reticles    
		
		//render grid stuff here
    m_GridManager->renderEntireGrid(plane->getPosition().z);
    
    if (plane->isPlaneAlive() == false)
    {
      int textY = gRenderer.getScreenY()/2;
      IRectangle rect = IRectangle(0,textY,gRenderer.getScreenX()-1, textY + 30);
      gRenderer.drawText("Press \"Space Bar\" to Respawn",&rect, eTextAlignModeCenter, false);
    }
  }

  // NEW - Update Time
  gGame.m_tTimer.updateTimer();

  // render FPS and console ontop of everything
	if(showDebugStuff==true){
		gGame.GameBase::renderConsoleAndFPS();


		gRenderer.drawText(gGame.getTimeC(), 10, 60);

	
		string colorbuf = "RGBH(0): "
			+ to_string(gGame.m_statePlaying.m_GridManager->getLaneColorRed(0)) + " "
			+ to_string(gGame.m_statePlaying.m_GridManager->getLaneColorGreen(0)) + " "
			+ to_string(gGame.m_statePlaying.m_GridManager->getLaneColorBlue(0)) + " "
			+ to_string(gGame.m_statePlaying.m_GridManager->getLaneHue(0)) + " ";

		string colorbuf1 = "RGBH(1): "
			+ to_string(gGame.m_statePlaying.m_GridManager->getLaneColorRed(1)) + " "
			+ to_string(gGame.m_statePlaying.m_GridManager->getLaneColorGreen(1)) + " "
			+ to_string(gGame.m_statePlaying.m_GridManager->getLaneColorBlue(1)) + " "
			+ to_string(gGame.m_statePlaying.m_GridManager->getLaneHue(1)) + " ";

		string colorbuf2 = "RGBH(2): "
			+ to_string(gGame.m_statePlaying.m_GridManager->getLaneColorRed(2)) + " "
			+ to_string(gGame.m_statePlaying.m_GridManager->getLaneColorGreen(2)) + " "
			+ to_string(gGame.m_statePlaying.m_GridManager->getLaneColorBlue(2)) + " "
			+ to_string(gGame.m_statePlaying.m_GridManager->getLaneHue(2)) + " ";

		string colorbuf3 = "RGBH(3): "
			+ to_string(gGame.m_statePlaying.m_GridManager->getLaneColorRed(3)) + " "
			+ to_string(gGame.m_statePlaying.m_GridManager->getLaneColorGreen(3)) + " "
			+ to_string(gGame.m_statePlaying.m_GridManager->getLaneColorBlue(3)) + " "
			+ to_string(gGame.m_statePlaying.m_GridManager->getLaneHue(3)) + " ";

		gRenderer.drawText((char*)colorbuf.c_str(), 10, 80);
		gRenderer.drawText((char*)colorbuf1.c_str(), 10, 90);
		gRenderer.drawText((char*)colorbuf2.c_str(), 10, 100);
		gRenderer.drawText((char*)colorbuf3.c_str(), 10, 110);

		gRenderer.drawText((char*)(to_string(
			gGame.m_statePlaying.m_GridManager->prevNode[0])).c_str(), 10, 120);
		gRenderer.drawText((char*)(to_string(
			gGame.m_statePlaying.m_GridManager->prevNode[1])).c_str(), 10, 130);
		gRenderer.drawText((char*)(to_string(
			gGame.m_statePlaying.m_GridManager->prevNode[2])).c_str(), 10, 140);
		gRenderer.drawText((char*)(to_string(
			gGame.m_statePlaying.m_GridManager->prevNode[3])).c_str(), 10, 150);

		gRenderer.drawText((char*)("Score: " + to_string(
			gGame.m_statePlaying.m_GridManager->score)).c_str(), 10, 180);
		gRenderer.drawText((char*)("Multiplier: " + to_string(
			gGame.m_statePlaying.m_GridManager->scoreMultiplier)).c_str(), 10, 190);

		gRenderer.drawText((char*)("Joy X: " + to_string(
			m_objects->getPlaneObject()->joyX)).c_str(), 10, 210);
		gRenderer.drawText((char*)("Joy Y: " + to_string(
			m_objects->getPlaneObject()->joyY)).c_str(), 10, 220);
	}

  gRenderer.drawText((char*)("Score: " + to_string(gGame.m_statePlaying.m_GridManager->score)).c_str(),
    &m_rectScore,eTextAlignModeCenter,false,m_nLargeText);

  gRenderer.drawText((char*)("Multiplier: " + to_string(gGame.m_statePlaying.m_GridManager->scoreMultiplier)).c_str(),
    &m_rectMultiplier,eTextAlignModeCenter,false,m_nLargeText);

  if(gGame.m_tTimer.isCountdown()) {
    IRectangle countdownRect = IRectangle(0,gRenderer.getScreenY()/2,gRenderer.getScreenX() - 1, gRenderer.getScreenY() - 1);
    gRenderer.drawText((char*)to_string(gGame.m_tTimer.getCountdownRemain()).c_str(), &countdownRect,eTextAlignModeCenter, false, m_nHugeText);
  }
  
	float nta = getNodePos(m_mtp[0]->getNumberOfNotes() - 1,0);
	float ntb = getNodePos(m_mtp[1]->getNumberOfNotes() - 1,1);
	float ntc = getNodePos(m_mtp[2]->getNumberOfNotes() - 1,2);
	float ntd = getNodePos(m_mtp[3]->getNumberOfNotes() - 1,3);

	float ntmax = max(max(nta,ntb),max(ntc,ntd));

	if((planezpos - 240.0f) > (ntmax * 60.0f)){

		float sw = (float)gRenderer.getScreenX();
		float sh = (float)gRenderer.getScreenY();
		Vector3 ntcamPos = gGame.m_currentCam->cameraPos;
		EulerAngles ntcamOrient = gGame.m_currentCam->cameraOrient;
		gRenderer.setZBufferEnable(false);
		gRenderer.setARGB( MAKE_ARGB(
			255,
			(int)((float)m_GridManager->getLaneColorRed(0) * 0.4f + 30.0f),
			(int)((float)m_GridManager->getLaneColorGreen(0) * 0.4f + 30.0f),
			(int)((float)m_GridManager->getLaneColorBlue(0) * 0.4f + 30.0f)));
		gRenderer.selectTexture(m_ncompleteBGTex);
		gRenderer.instance(ntcamPos + Vector3(sw/2.0f,-sh/4.0f,0.0f),EulerAngles::kEulerAnglesIdentity);
		gRenderer.renderSprite(sw * 2.0f,sh);
		gRenderer.instancePop();
		gRenderer.setZBufferEnable(true);

		gRenderer.setARGB(0XFFFFFFFF);
		gRenderer.drawText("Level Complete! Nice Job!", (int)(sw*0.5f) - 200, (int)(sh*0.1f), 1);
		gRenderer.drawText((char*)("Final Score: " + to_string(
			gGame.m_statePlaying.m_GridManager->score)).c_str(), (int)(sw*0.5f) - 120, (int)(sh*0.2f), 1);
		gRenderer.drawText("(Press Escape or Start to Return to Main Menu)", (int)(sw*0.5f) - 220, (int)(sh*0.3f), 0);
	}

}

void StatePlaying::renderScene(bool asReflection)
{
	
	RenderVertexL skyVerts[4];

	skyVerts[0].u = 0.0f; skyVerts[0].v = 0.0f;
  skyVerts[1].u = 1.0f; skyVerts[1].v = 0.0f;
  skyVerts[2].u = 1.0f; skyVerts[2].v = 1.0f;
  skyVerts[3].u = 0.0f; skyVerts[3].v = 1.0f;

	Vector3 camPos = gGame.m_currentCam->cameraPos;

	float levelCompletion = 0.0f;
	int lastNode = getNodeTotal(0) - 1;

	levelCompletion = 
		(getplanezpos() - planeOrigSpawnVal.z)
		/ ((getNodePos(lastNode,0) * 60.0f) - planeOrigSpawnVal.z);

	clamp(levelCompletion,0.0f,1.0f);

	skyVerts[0].p = Vector3( camPos.x - 740.0f, camPos.y + 740.0f + (levelCompletion * 0.0f), 1000.0f + camPos.z);
	skyVerts[1].p = Vector3( camPos.x + 740.0f, camPos.y + 740.0f + (levelCompletion * 0.0f), 1000.0f + camPos.z);
	skyVerts[2].p = Vector3( camPos.x + 740.0f, camPos.y - 740.0f + (levelCompletion * 0.0f), 1000.0f + camPos.z);
	skyVerts[3].p = Vector3( camPos.x - 740.0f, camPos.y - 740.0f + (levelCompletion * 0.0f), 1000.0f + camPos.z);

	for(int sv=0;sv<4;sv++){
		skyVerts[sv].argb = MAKE_ARGB(255,255,255,255);
	}

	gRenderer.selectTexture(m_nSkyTexture);
	gRenderer.setZBufferEnable(false);
	gRenderer.renderQuad(skyVerts);
	gRenderer.setZBufferEnable(true);
	
  
//terrain->render(); // render the terrain   
   
  // render water

  if (asReflection == false)      
    water->render(gGame.m_currentCam->cameraPos, gGame.m_currentCam->cameraOrient.heading);
    
   //render particles
  gParticle.render(!asReflection);   

	m_objects->render();
}

void StatePlaying::resetGame()
{ 
  m_objects->clear();
  m_objects->spawnWater(water);
  
  // Create plane  
	Vector3 newPlanePos = m_objects->getOrigSpawnPlane();
  unsigned int planeID = m_objects->spawnPlane(newPlanePos);
  m_tetherCamera->setTargetObject(planeID);


  // reset camera following crow variables
  m_planeCrashed = false;

  // Set the tether camera

  // EDIT - INCREASE CAMERA DISTANCE
  m_tetherCamera->minDist = 70.0f;
  m_tetherCamera->maxDist = 70.0f;
  m_tetherCamera->fov;
  m_tetherCamera->reset();
  m_tetherCamera->process(0.0f);
  gGame.m_currentCam = m_tetherCamera;   // select tether camera as the current camera
  
  // kill all active particle effects
  gParticle.killAll();

}

void StatePlaying::processInput()
{
  PlaneObject *planeObject = m_objects->getPlaneObject();

  // Exit to menu if requested
  if (gInput.keyJustUp(DIK_ESCAPE, true)) 
  {        
    gGame.changeState(eGameStateMenu);    
    return;
  }

	if (gInput.keyJustUp(DIK_L, true)) 
  {
		if(showDebugStuff==false){
			showDebugStuff = true;
		}
		else {
			showDebugStuff = false;
		}
    return;
  }

  // If you press space bar after you die reset game
  if (gInput.keyJustUp(DIK_SPACE))
   if (planeObject->isPlaneAlive() == false)
    {
      resetGame();
      return;
    }    
}

/// \param dt Change in time
void StatePlaying::processCamera(float dt)
{
  PlaneObject *planeObject = m_objects->getPlaneObject();
  Camera * cam = gGame.m_currentCam;

  // this should never happen but if it does leave
  if (planeObject == NULL || cam == NULL) 
    return; 

  // Grab current states
  Vector3 cameraPosOld = cam->cameraPos;
  Vector3 planePosOld = planeObject->getPosition();
  
  cam->process(dt); // move camera

  // Test for camera collision with terrain
  const float CameraHeight = 2.0f;
  
  // location is now above terrain, set it
  cam->setAsCamera();

  // Set 3D sound parameters based on new camera position and velocity
  Vector3 cameraVel = (cam->cameraPos - cameraPosOld) / dt;  

  // New - Rotation
  if(TetherCamera* cam = dynamic_cast<TetherCamera*>(gGame.m_currentCam) ) {

    if(cam->cameraOrient.bank != planeObject->planeTargetOrient) {
      // A rotation needs to occur

      if(m_fRotationTimeRemain == 0.0f) {
        // Set the rotation time
        m_fRotationTimeRemain = 2.0f * gGame.m_tTimer.getBeatTime(); // Rotation takes the time of two beats
				totalRotationTimeCam = m_fRotationTimeRemain;

      }
      else if(m_fRotationTimeRemain > 0.0f) {
        m_fRotationTimeRemain -= dt;
        float t_fNewBank;
				t_fNewBank = (planeObject->planeTargetOrient - cam->cameraOrient.bank);
				t_fNewBank *= 0.09f;
				cam->setBank(cam->cameraOrient.bank + t_fNewBank);
				if(abs(cam->cameraOrient.bank - planeObject->planeTargetOrient) < 0.0245436926f){
					cam->setBank(planeObject->planeTargetOrient);
					m_fRotationTimeRemain = 0.0f;
				}
      }
      else { 
				cam->setBank(planeObject->planeTargetOrient);
        m_fRotationTimeRemain = 0.0f;
      }

    }
  }
}

/// This is used to help spawn objects.
/// \param x X Coordinate on the terrain.
/// \param y Value that is added to the Y component after it is set to the height
/// of the terrain under point (X, Z)
/// \param z Z Coordinate on the terrain.
Vector3 StatePlaying::LocationOnterrain(float x, float y, float z)
{
  return Vector3(x,terrain->getHeight(x,z)+y, z);
}

float StatePlaying::getNodePos(int index, int lane){
	int m = m_mtp[lane]->getMeasureValue(index);
	int b = m_mtp[lane]->getBeatValue(index);
	float r = m_mtp[lane]->getRatioValue(index);
	float z = ((float)(((m-1) * 4) + (b-1))) + r;
	return z;
}

float StatePlaying::getplanezpos(){
	return planezpos;
}

float StatePlaying::getplanexpos(){
	return planexpos;
}

float StatePlaying::getplaneypos(){
	return planeypos;
}

int StatePlaying::getNodeTotal(int lane){
	return m_mtp[lane]->getNumberOfNotes();
}

int StatePlaying::getCurrentLaneFlag(){
	return currentLaneFlag;
}

Vector3 StatePlaying::getPlaneOrigSpawnVal(){
	return planeOrigSpawnVal;
}