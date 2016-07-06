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

/// \file Ned3DObjectManager.cpp
/// \brief Code for the Ned3DObjectManager class, which is responsible for all
/// objects specific to this demo.

/// Edited by the SAGE team, 2005
/// Last updated June 13, 2006

#include <algorithm>
#include "Common/MathUtil.h"
#include "Common/RotationMatrix.h"
#include "Graphics/ModelManager.h"
#include "Objects/GameObject.h"
#include "Particle/ParticleEngine.h"
#include "Water/Water.h"
#include "Ned3DObjectManager.h"
#include "Objects.h"
#include "ObjectTypes.h"
#include "DirectoryManager/DirectoryManager.h"

PlaneObject* gPlane;

Ned3DObjectManager::Ned3DObjectManager() :
  m_models(NULL),
  m_planeModel(NULL),
  m_plane(NULL),
  m_water(NULL)
{
}

void Ned3DObjectManager::setModelManager(ModelManager &models)
{
  if(m_models == &models)
    return;
  m_models = &models;
  m_planeModel = NULL;
}

void Ned3DObjectManager::clear()
{
  m_planeModel = NULL;
  GameObjectManager::clear();
}

unsigned int Ned3DObjectManager::spawnPlane(const Vector3 &position, const EulerAngles &orientation)
{
  if(m_plane != NULL)
    return 0;  // Only one plane allowed
  if(m_planeModel == NULL)
    m_planeModel = m_models->getModelPointer("Plane"); // Cache plane model
  if(m_planeModel == NULL)
    return 0;  // Still NULL?  No such model
  m_plane = new PlaneObject(m_planeModel);
  m_plane->setPosition(position);
  m_plane->setOrientation(orientation);
  unsigned int id = addObject(m_plane, "Plane");
  return id;
}

unsigned int Ned3DObjectManager::spawnWater(Water *water)
{
  m_water = new WaterObject(water);
  return addObject(m_water, false, false, false, "Water");
}

void Ned3DObjectManager::deleteObject(GameObject *object)
{
  if(object == m_plane)
    m_plane = NULL;
  else if(object == m_water)
    m_water = NULL;
  GameObjectManager::deleteObject(object);
}


bool Ned3DObjectManager::enforcePosition(GameObject &moving, GameObject &stationary)
{
  const AABB3 &box1 = moving.getBoundingBox(), &box2 = stationary.getBoundingBox();
  AABB3 intersectBox;
  if(AABB3::intersect(box1, box2, &intersectBox))
  {
    // Collision:  Knock back obj1
    //   - Kludge method:  Push back on smallest dimension
    Vector3 delta = intersectBox.size();
    Vector3 obj1Pos = moving.getPosition(), obj2Pos = stationary.getPosition();
    if(delta.x <= delta.y)
      if(delta.x <= delta.z)
      {
        // Push back on x
        obj1Pos.x += (box1.min.x < box2.min.x) ? -delta.x : delta.x;
      }
      else
      {
        // Push back on z
        obj1Pos.z += (box1.min.z < box2.min.z) ? -delta.z : delta.z;
      }
    else if(delta.y <= delta.z)
    {
      // Push back on y
      obj1Pos.y += (box1.min.y < box2.min.y) ? -delta.y : delta.y;
    }
    else
    {
      // Push back on z
      obj1Pos.z += (box1.min.z < box2.min.z) ? -delta.z : delta.z;
    }
    moving.setPosition(obj1Pos);
    return true;
  }
  return false;
}

bool Ned3DObjectManager::enforcePositions(GameObject &obj1, GameObject &obj2)
{
  const AABB3 &box1 = obj1.getBoundingBox(), &box2 = obj2.getBoundingBox();
  AABB3 intersectBox;
  if(AABB3::intersect(box1, box2, &intersectBox))
  {
    // Collision:  Knock back both objects
    //   - Kludge method:  Push back on smallest dimension
    Vector3 delta = intersectBox.size();
    Vector3 obj1Pos = obj1.getPosition(), obj2Pos = obj2.getPosition();
    if(delta.x <= delta.y)
      if(delta.x <= delta.z)
      {
        // Push back on x
        float dx = (box1.min.x < box2.min.x) ? -delta.x : delta.x;
        obj1Pos.x += dx;
        obj2Pos.x -= dx;
      }
      else
      {
        // Push back on z
        float dz = (box1.min.z < box2.min.z) ? -delta.z : delta.z;
        obj1Pos.z += dz;
        obj2Pos.z -= dz;
      }
    else if(delta.y <= delta.z)
    {
        // Push back on y
        float dy = (box1.min.y < box2.min.y) ? -delta.y : delta.y;
        obj1Pos.y += dy;
        obj2Pos.y -= dy;
    }
    else
    {
      // Push back on z
      float dz = (box1.min.z < box2.min.z) ? -delta.z : delta.z;
      obj1Pos.z += dz;
      obj2Pos.z -= dz;
    }
    obj1.setPosition(obj1Pos);
    obj2.setPosition(obj2Pos);
    return true;
  }
  return false;
}

Vector3 Ned3DObjectManager::getOrigSpawnPlane(){
	return gPlane->getOrigSpawn();
}