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

/// \file Ned3DObjectManager.h
/// \brief Defines the object management class for Ned3D

/// Last updated July 12, 2005

#ifndef __NED3DOBJECTMANAGER_H_INCLUDED__
#define __NED3DOBJECTMANAGER_H_INCLUDED__

#include "Common/Vector3.h"
#include "Common/EulerAngles.h"
#include "Objects/GameObjectManager.h"
#include "ObjectTypes.h"

class Model;
class ModelManager;
class PlaneObject;
class Water;

/// \brief Derived object manager to handle Ned3D objects specifically.
class Ned3DObjectManager : public GameObjectManager
{
  public:
    
    Ned3DObjectManager(); ///< Constructs a Ned3DObjectManager.
    
    /// \brief Tells the object manager which model manager to use.
    /// \param models The model manager to use for the objects.
    void setModelManager(ModelManager &models);
  
    virtual void clear(); ///< Clears the object manager, removing all objects.

    /// \brief Spawns a plane object.
    /// \param position Position to place the object.
    /// \param orientation Initial orientation of the object.
    /// \return The unique ID of the new object.
    unsigned int spawnPlane(const Vector3 &position = Vector3::kZeroVector, const EulerAngles &orientation = EulerAngles::kEulerAnglesIdentity);

    /// \brief Spawns a water object.
    /// \param water The water data
    /// \return The unique ID of the new object.
    unsigned int spawnWater(Water *water);

    WaterObject *getWaterObject() { return m_water; } ///< Returns the pointer to the water object

    /// \brief Returns the pointer to the plane object.
    /// \return Pointer to the plane object.
    PlaneObject *getPlaneObject() { return m_plane; }
    
    /// \brief Deletes a particular object.
    /// \param object Pointer to the object to be deleted.
    virtual void deleteObject(GameObject *object);

		Vector3 getOrigSpawnPlane();

  protected:
    
    bool enforcePosition(GameObject &moving, GameObject &stationary); ///< Blocks a moving object from intersecting a stationary object.
    bool enforcePositions(GameObject &obj1, GameObject &obj2); ///< Blocks two moving objects from intersecting each other.
    
    // Ned3D-specific references
    
    ModelManager *m_models; ///< Pointer to the model manager to be used when creating objects.

    Model *m_planeModel; ///< Pointer to the plane model.
    
    PlaneObject *m_plane;  ///> Points to the sole plane object.
    WaterObject *m_water; ///> Points to the sole water object.  (not owned)
};


#endif

