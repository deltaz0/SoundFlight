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

/// \file PlaneObject.cpp
/// \brief Code for the PlaneObject class.

/// Edited by the SAGE team, 2005
/// Last updated June 13, 2006

#include <assert.h>
#include "Common/MathUtil.h"
#include "PlaneObject.h"
#include "Input/Input.h"
#include "ObjectTypes.h"
#include "common/RotationMatrix.h"
#include "Ned3DObjectManager.h"
#include "Particle/ParticleEngine.h"
#include "Sound/SoundManager.h"
#include "game.h"
#include "directorymanager/directorymanager.h"

bool PlaneObject::takeDamage = true;

//--------new stuff--------
Vector3 planeOrigSpawn;
float gridWidth;
float planeZSpeed;
float gridZFrontOffset;
float gridZBackOffset;
float beatForwardOffset;
static const float m_Pi = 3.14159f;
//--------end new stuff--------

PlaneObject::PlaneObject(Model *m):
  GameObject(m,1),
  m_gunPosition(0, 2.2f, 3.1f),
  m_enginePosition(0,0.0f, 3.3f),
  m_propOffset(0.4f),
  m_isPlaneAlive(true),
  m_maxTurnRate(kPi * 0.25f),
  m_maxPitchRate(kPi * 0.25f),
  m_maxBankRate(kPi * 0.25f),
  m_planeState(PS_FLYING),
  m_turnState(TS_STRAIGHT),
  m_pitchState(PS_LEVEL),
  m_moveState(MS_STOP),
  m_hp(4),
  m_maxHP(4),
	joyX(0.0f),
	joyY(0.0f),
  // New for rotation
  m_planeOrientation(0.0f)
{
  assert(m);
  assert(m->getPartCount() >= 1);
	setModelOrientation(EulerAngles(0.0f, (-0.0425f) * kPi, 0.0f));
  setPosition(0,m_propOffset,0,0);
  m_fSpeed = 0.0f;
  m_maxSpeed = 5.0f;
  m_className = "Plane";
  m_type = ObjectTypes::PLANE;
  m_pitchRate = m_turnRate = 0.0f;
  m_smokeID = -1;
  m_reticleLockOnUpdated = false;
	//--------new stuff--------
	planeOrigSpawn = Vector3(0,360,0);
	gridWidth = 30.0f;
	gridZFrontOffset = 4440.0f;
	gridZBackOffset = 60.0f;

	planeOrientFlag = 0;
	planeTargetOrient = 0.0f;


	planeZSpeed = (float)(gGame.getBPM());
		
	beatForwardOffset = 60.0f;

	//--------end new stuff--------

	//--------new stuff--------
  m_planeMovement[0] = DIK_W; // UP
  m_planeMovement[1] = DIK_S; // DOWN
  m_planeMovement[2] = DIK_A; // LEFT
  m_planeMovement[3] = DIK_D; // RIGHT
	//--------end new stuff--------

  m_timeSinceFired = gRenderer.getTime();

  // load reticle texture
  m_reticleTexture = gRenderer.cacheTextureDX("reticle.png");

  // load all textures that will be used on the plane.
  // Multiple textures are used throughout game play so the plane appears to
  // "take damage"

  m_allParticles.resize(5);
  m_allParticles[0] = "smokeveryheavy";   // when hp = 0
  m_allParticles[1] = "smokeheavy";   // when hp = 0
  m_allParticles[2] = "smokemedium";  // hp = 1
  m_allParticles[3] = "smokelight";   // hp = 2
  m_allParticles[4] = "";             // hp > 2

  setTextureAndSmoke();

}

PlaneObject::~PlaneObject()
{
  // kill particle engine if one is attached
  if (m_smokeID != -1)
    gParticle.killSystem(m_smokeID);
  m_smokeID = -1;

}

void PlaneObject::process(float dt)
{
  if(!m_isPlaneAlive) return;
  // save the position
  m_oldPosition = getPosition();

  // Put any non-movement logic here

  // Unbuffered input (remove if handling input elsewhere)  
  inputStraight();
  inputLevel();
  inputStop();
 
  // if(gInput.keyDown(DIK_SPACE)) inputFire();
	 
  if(gInput.keyDown(m_planeMovement[0])) // UP
    inputClimb(1.0f);
  if(gInput.keyDown(m_planeMovement[1])) // DOWN
    inputDive(1.0f);
  if(gInput.keyDown(m_planeMovement[2])) // LEFT
    inputTurnLeft(1.0f);
  if(gInput.keyDown(m_planeMovement[3])) // RIGHT
    inputTurnRight(1.0f);

  if (gInput.keyJustUp(DIK_EQUALS))
    inputSpeedUp();
  if (gInput.keyJustUp(DIK_MINUS))
    inputSpeedDown();
  if (gInput.keyDown(DIK_RETURN))
    inputForward(1.0f);

  // NEW - Rotation
  if(gInput.keyJustDown(DIK_RIGHT)) {
		if(abs((-1.0f * m_planeOrientation) - planeTargetOrient) > (m_Pi * 1.0f))
			m_planeOrientation = (-1.0f * (planeTargetOrient - (m_Pi / 2)));
		planeTargetOrient += (m_Pi / 2);
		planeOrientFlag = (planeOrientFlag + 1);
		if(planeOrientFlag > 3)
			planeOrientFlag = 0;
  }
  else if(gInput.keyJustDown(DIK_LEFT)) {
		if(abs((-1.0f * m_planeOrientation) - planeTargetOrient) > (m_Pi * 1.0f))
			m_planeOrientation = (-1.0f * (planeTargetOrient + (m_Pi / 2)));
		planeTargetOrient -= (m_Pi / 2);
		planeOrientFlag = (planeOrientFlag - 1);
		if(planeOrientFlag < 0)
			planeOrientFlag = 3;
  }
	
	m_planeOrientation -= (0.05f * (m_planeOrientation + planeTargetOrient));
	if(abs((-1.0f * m_planeOrientation) - planeTargetOrient) < (m_Pi / 128))
		m_planeOrientation = (-1.0f * planeTargetOrient);
	if(abs((-1.0f * m_planeOrientation) - planeTargetOrient) > (m_Pi * 1.0f))
		m_planeOrientation = (-1.0f * planeTargetOrient);

	switch(planeOrientFlag){
	case 0:

    m_planeMovement[0] = DIK_W; // UP
    m_planeMovement[1] = DIK_S; // DOWN
    m_planeMovement[2] = DIK_A; // LEFT
    m_planeMovement[3] = DIK_D; // RIGHT
		break;
	case 1:

    m_planeMovement[0] = DIK_D; // UP
    m_planeMovement[1] = DIK_A; // DOWN
    m_planeMovement[2] = DIK_W; // LEFT
    m_planeMovement[3] = DIK_S; // RIGHT
		break;
	case 2:

    m_planeMovement[0] = DIK_S; // UP
    m_planeMovement[1] = DIK_W; // DOWN
    m_planeMovement[2] = DIK_D; // LEFT
    m_planeMovement[3] = DIK_A; // RIGHT
		break;
	case 3:

    m_planeMovement[0] = DIK_A; // UP
    m_planeMovement[1] = DIK_D; // DOWN
    m_planeMovement[2] = DIK_S; // LEFT
    m_planeMovement[3] = DIK_W; // RIGHT
		break;
	default:
		break;
	}

  // process all joystick input if it is disabled
  if (gInput.joyEnabled())
  {
    // set plane speed based on slider

		if(gInput.joyButtonJustDown(7)) {
		if(abs((-1.0f * m_planeOrientation) - planeTargetOrient) > (m_Pi * 1.0f))
			m_planeOrientation = (-1.0f * (planeTargetOrient - (m_Pi / 2)));
		planeTargetOrient += (m_Pi / 2);
		planeOrientFlag = (planeOrientFlag + 1);
		if(planeOrientFlag > 3)
			planeOrientFlag = 0;
		}
		else if(gInput.joyButtonJustDown(6)) {
			if(abs((-1.0f * m_planeOrientation) - planeTargetOrient) > (m_Pi * 1.0f))
				m_planeOrientation = (-1.0f * (planeTargetOrient + (m_Pi / 2)));
			planeTargetOrient -= (m_Pi / 2);
			planeOrientFlag = (planeOrientFlag - 1);
			if(planeOrientFlag < 0)
				planeOrientFlag = 3;
		}

		joyX = gInput.joyPadPositionX();
		joyY = gInput.joyPadPositionY();

		switch(planeOrientFlag){
		case 0:

			if(gInput.joyPadPositionX() < 0.0f)  
				inputTurnLeft(-gInput.joyPadPositionX() * 1.5f);  
			if(gInput.joyPadPositionX() > 0.0f)
				inputTurnRight(gInput.joyPadPositionX()* 1.5f);
			if(gInput.joyPadPositionY() > 0.0f)
				inputDive(gInput.joyPadPositionY()* 1.5f);
			if(gInput.joyPadPositionY() < 0.0f)
				inputClimb(-gInput.joyPadPositionY()* 1.5f);
			break;
		case 1:

			if(gInput.joyPadPositionY() < 0.0f)  
				inputTurnLeft(-gInput.joyPadPositionY()* 1.5f);  
			if(gInput.joyPadPositionY() > 0.0f)
				inputTurnRight(gInput.joyPadPositionY()* 1.5f);
			if(gInput.joyPadPositionX() < 0.0f)
				inputDive(-gInput.joyPadPositionX()* 1.5f);
			if(gInput.joyPadPositionX() > 0.0f)
				inputClimb(gInput.joyPadPositionX()* 1.5f);
			break;
		case 2:

			if(gInput.joyPadPositionX() > 0.0f)  
				inputTurnLeft(gInput.joyPadPositionX()* 1.5f);  
			if(gInput.joyPadPositionX() < 0.0f)
				inputTurnRight(-gInput.joyPadPositionX()* 1.5f);
			if(gInput.joyPadPositionY() < 0.0f)
				inputDive(-gInput.joyPadPositionY()* 1.5f);
			if(gInput.joyPadPositionY() > 0.0f)
				inputClimb(gInput.joyPadPositionY()* 1.5f);
			break;
		case 3:

			if(gInput.joyPadPositionY() > 0.0f)  
				inputTurnLeft(gInput.joyPadPositionY()* 1.5f);  
			if(gInput.joyPadPositionY() < 0.0f)
				inputTurnRight(-gInput.joyPadPositionY()* 1.5f);
			if(gInput.joyPadPositionX() > 0.0f)
				inputDive(gInput.joyPadPositionX()* 1.5f);
			if(gInput.joyPadPositionX() < 0.0f)
				inputClimb(-gInput.joyPadPositionX()* 1.5f);
			break;
		default:
			break;
		}


		if (gInput.joyButtonDown(9)) 
		{        
			gGame.changeState(eGameStateMenu);    
			return;
		}
  }

  if (m_planeState == PS_FLYING)
    m_velocity = (getPosition() - m_oldPosition)/dt;

}

void PlaneObject::move(float dt)
{
  if(!m_isPlaneAlive)
    return;

  EulerAngles &planeOrient = m_eaOrient[0];
  Vector3 displacement = Vector3::kZeroVector;

  if (m_planeState == PS_FLYING)
  {

    // Update plane orientation

    processWorldOrientation(dt);

		int bb = 0;
		int pp = 0;
		int hh = 0;

				
    switch(m_turnState)
    {
      case TS_LEFT:
      {
				if(planeOrientFlag==0)
					bb = 1;
				else if(planeOrientFlag==1)
					hh = -1;
				else if(planeOrientFlag==2)
					bb = -1;
				else if(planeOrientFlag==3)
					hh = -1;
				if(m_oldPosition.x > -1.0f * gridWidth)
					m_v3Position[0].x -= 32.0f * dt * m_turnRate;
      } break;
      case TS_RIGHT:
      {
				if(planeOrientFlag==0)
					bb = -1;
				else if(planeOrientFlag==1)
					hh = 1;
				else if(planeOrientFlag==2)
					bb = 1;
				else if(planeOrientFlag==3)
					hh = 1;
				if(m_oldPosition.x < 1.0f * gridWidth)
					m_v3Position[0].x += 32.0f * dt * m_turnRate;
      }
    };
    switch(m_pitchState)
    {
      case PS_CLIMB:
      {
				if(planeOrientFlag==0)
					pp = -1;
				else if(planeOrientFlag==1)
					bb = -1;
				else if(planeOrientFlag==2)
					pp = -1;
				else if(planeOrientFlag==3)
					bb = 1;
				if(m_oldPosition.y < (1.0f * gridWidth) + planeOrigSpawn.y)
					m_v3Position[0].y += 32.0f * dt * m_pitchRate;
      } break;
      case PS_DIVE:
      {
				if(planeOrientFlag==0)
					pp = 1;
				else if(planeOrientFlag==1)
					bb = 1;
				else if(planeOrientFlag==2)
					pp = 1;
				else if(planeOrientFlag==3)
					bb = -1;
				if(m_oldPosition.y > (-1.0f * gridWidth) + planeOrigSpawn.y)
					m_v3Position[0].y -= 32.0f * dt * m_pitchRate;
      }
    }

		float bankRate;

		if(planeOrientFlag==0)
			bankRate = m_turnRate;
		else if(planeOrientFlag==1)
			bankRate = m_pitchRate;
		else if(planeOrientFlag==2)
			bankRate = m_turnRate;
		else if(planeOrientFlag==3)
			bankRate = m_pitchRate;

		planeOrient.bank += m_maxBankRate * bankRate * 1.85f * (float)bb * dt;
		planeOrient.pitch += m_maxPitchRate * m_pitchRate * 1.85f * (float)pp * dt;
		planeOrient.heading += m_maxTurnRate * m_turnRate * 1.5f * (float)hh * dt;

    clamp(planeOrient.pitch,-kPi * 0.15f,kPi * 0.15f);
		clamp(planeOrient.heading,-kPi * 0.15f,kPi * 0.15f);
		clamp(planeOrient.bank,-kPi * 0.15f,kPi * 0.15f);
    float bankCorrect = 1.85f * m_maxBankRate * dt;
    float pitchCorrect = 1.85f * m_maxPitchRate * dt;
		float headingCorrect = 1.5f * m_maxTurnRate * dt;
    if(bb == 0)
      if(planeOrient.bank > bankCorrect)
        planeOrient.bank -= bankCorrect;
      else if(planeOrient.bank < -bankCorrect)
        planeOrient.bank += bankCorrect;
      else planeOrient.bank = 0.0f;
    if(pp == 0)
      if(planeOrient.pitch > pitchCorrect)
        planeOrient.pitch -= pitchCorrect;
      else if(planeOrient.pitch < -pitchCorrect)
        planeOrient.pitch += pitchCorrect;
      else planeOrient.pitch = 0.0f;
		if(hh == 0)
      if(planeOrient.heading > headingCorrect)
        planeOrient.heading -= headingCorrect;
      else if(planeOrient.heading < -headingCorrect)
        planeOrient.heading += headingCorrect;
      else planeOrient.heading = 0.0f;

    // Check move state
    
    switch(m_moveState)
    {
      case MS_STOP:
      {
        m_fSpeed = 0.0f;
      } break;
      case MS_FORWARD:
      {
        m_fSpeed = m_maxSpeed * m_speedRatio;
      } break;
      case MS_BACKWARD:
      {
        m_fSpeed = -m_maxSpeed * m_speedRatio;
      } break;
    }
  } // end if (m_planeState == PS_FLYING)
  if (m_planeState == PS_CRASHING)
  {
      m_eaAngularVelocity[0].bank = 1.0f;
      Vector3 normVelocity = m_velocity;
      normVelocity.normalize();
      m_eaOrient[0].pitch = -(float)asin((double)(normVelocity.y));
      float gravity =  -10.0f;
      
      m_fSpeed = 0.0f;
      m_velocity.y += gravity * dt;      
      m_v3Position[0] += m_velocity * dt;  
  }
	m_fSpeed = 0.0f;
	m_v3Position[0].z += planeZSpeed * dt;
  // Update propeller orientation

  // Move it
  
  GameObject::move(dt);

  // flag that m_reticleLockedOn is out of date
  m_reticleLockOnUpdated = false; 

  // tell the smoke particle system where the plane is if there is smoke
  if (m_smokeID != -1)
    gParticle.setSystemPos(m_smokeID, transformObjectToInertial(m_enginePosition));
  
}

float PlaneObject::getPlaneOrientation() {
  return (1.0f * m_planeOrientation);
};

void PlaneObject::processWorldOrientation(float dt) {

  // The camera and the plane (for some reason) rotate in opposite directions
  // m_planeOrientation actually stores the camera orientation, which must be
  // modified before it can be used as the plane model orientation
	switch(planeOrientFlag){
	case 0:
		setModelOrientation(EulerAngles(kPi, (-0.0425f) * kPi, m_planeOrientation));
		break;
	case 1:
		setModelOrientation(EulerAngles(kPi + (0.0425f) * kPi, 0.0f, m_planeOrientation));
		break;
  case 2:
		setModelOrientation(EulerAngles(kPi, (0.0425f) * kPi, m_planeOrientation));
		break;
	case 3:
		setModelOrientation(EulerAngles(kPi + (-0.0425f) * kPi, 0.0f, m_planeOrientation));
		break;
	default:
		break;
	}		

}

void PlaneObject::reset()
{
  m_isPlaneAlive = true;
  m_v3Position[0].zero(); // collision handling will fix height
  m_eaOrient[0].identity();
  m_eaOrient[1].identity();
  m_fSpeed = 0.0f;
  m_maxSpeed = 1.0f;
  m_turnState = TS_STRAIGHT;
  m_pitchState = PS_LEVEL;
  m_moveState = MS_STOP;
}

void PlaneObject::inputTurnLeft(float turnRatio)
{
  m_turnRate = turnRatio;
  m_turnState = m_turnState == TS_RIGHT ? TS_STRAIGHT : TS_LEFT;
}

void PlaneObject::inputTurnRight(float turnRatio)
{
  m_turnRate = turnRatio;
  m_turnState = m_turnState == TS_LEFT ? TS_STRAIGHT : TS_RIGHT;
}

void PlaneObject::inputStraight()
{
  m_turnState = TS_STRAIGHT;
}

void PlaneObject::inputClimb(float climbRatio)
{
  m_pitchRate = climbRatio;
  m_pitchState = m_pitchState == PS_DIVE ? PS_LEVEL : PS_CLIMB;
}

void PlaneObject::inputDive(float diveRatio)
{
  m_pitchRate = diveRatio;
  m_pitchState = m_pitchState == PS_CLIMB ? PS_LEVEL : PS_DIVE;
}

void PlaneObject::inputLevel()
{
  m_pitchState = PS_LEVEL;
}

void PlaneObject::inputSpeedDown()
{
  m_maxSpeed -= 1.0f;
}

void PlaneObject::inputSpeedUp()
{
  m_maxSpeed += 1.0f;
}

void PlaneObject::inputForward(float speed)
{
  m_speedRatio = speed;
  m_moveState = m_moveState == MS_BACKWARD ? MS_STOP : MS_FORWARD;
}

void PlaneObject::inputBackward()
{
  m_moveState = m_moveState == MS_FORWARD ? MS_STOP : MS_BACKWARD;
}

void PlaneObject::inputStop()
{
  m_moveState = MS_STOP;
}

void PlaneObject::killPlane()
{
  m_planeState = PS_DEAD;
  m_isPlaneAlive = false;
}

bool PlaneObject::isPlaneAlive() const
{
  return m_isPlaneAlive;
}

int PlaneObject::getHP() const
{
  return m_hp;
}

int PlaneObject::getMaxHP() const
{
  return m_maxHP;
}

void PlaneObject::setMaxHP(int maxHP, bool recoverAll)
{
  if(maxHP <= 0)
    return;
  m_maxHP = maxHP;
  if(recoverAll)
    this->recoverAll(); // the function, not the parameter
}

void PlaneObject::damage(int hp)
{
  // if god mode is on leave 
  if (!takeDamage)
    return;

  m_hp -= hp;
  setTextureAndSmoke(); // change to a texture with more damange on it
        
  if(m_isPlaneAlive && m_hp <= 0)
  {
    m_planeState = PS_CRASHING;
    m_velocity = Vector3::kForwardVector;
    m_eaOrient[0].pitch = degToRad(20);
    RotationMatrix r; r.setup(m_eaOrient[0]);
    m_velocity = r.objectToInertial(m_velocity);
    m_velocity *= m_maxSpeed * m_speedRatio * 20.0f;
  }
}

void PlaneObject::recover(int hp)
{
  m_hp += hp;
  if(m_hp > m_maxHP)
    m_hp = m_maxHP;
}

void PlaneObject::recoverAll()
{
  m_hp = m_maxHP;
}

void PlaneObject::setTextureAndSmoke()
{
  
  int textureIndex = m_hp; // index into m_allTextures array
  int smokeIndex = m_hp; // index into m_allParticles array
  
  // make sure the indicies are in range
  if (smokeIndex >= (int)m_allParticles.size())
    smokeIndex = (int)m_allParticles.size() - 1;
  if (smokeIndex < 0) smokeIndex = 0;

    // set texture 
   // remove previous smoke system
   if (m_smokeID != -1)
     gParticle.killSystem(m_smokeID);
   if (m_allParticles[smokeIndex] != "")
   {
     m_smokeID = gParticle.createSystem(m_allParticles[smokeIndex]);
     gParticle.setSystemPos(m_smokeID, transformObjectToInertial(m_enginePosition));
   }
}

Vector3 PlaneObject::getOrigSpawn(){
	return planeOrigSpawn;
}

int PlaneObject::getPlaneOrientFlag(){
	return planeOrientFlag;
}