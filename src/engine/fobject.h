/*
  BtSoccer - button football (soccer) game
  Copyright (C) DNTeam <btsoccer@dnteam.org>

  This file is part of BtSoccer.

  BtSoccer is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  BtSoccer is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with BtSoccer.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _btsoccer_field_object_h
#define _btsoccer_field_object_h

#include <OGRE/OgreEntity.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreAxisAlignedBox.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreTimer.h>

#include "../btsoccer.h"
#include "../physics/bulletlink.h"

namespace BtSoccer
{
   
   
#define AI_FILTER_GROUP   256

/*! The fobject is the base abstraction of interactive objects at the
 * game, like the ball, goals, disks, goal-keepers, etc. */
class FieldObject
{
   public:
      /*! Note: AI types are for AI calculations only. */
      enum FieldObjectTypes
      {
         TYPE_BALL,
         TYPE_BALL_AI,
         TYPE_DISK,
         TYPE_DISK_AI,
         TYPE_GOAL_KEEPER,
         TYPE_GOAL_KEEPER_AI,
         TYPE_GOAL,
         TYPE_GOAL_NET
      };

      /*! Constructor 
       * \param fType a FieldObjectTypes with the type of the object
       * \param name name of the object
       * \param fileName fileName with ogre object representing the object
       * \param ogreSceneManager pointer to the scene manager used
       * \param scale scale factor to the ogre object loaded
       * \param objMass mass of the object
       * \param colRestitution restitution factor of the object
       * \param objFriction default friction of the object
       * \param objRollingFriction friction when the object is rolling */
      FieldObject(int fType, Ogre::String name, Ogre::String fileName,
            Ogre::SceneManager* ogreSceneManager, Ogre::Real scale,
            Ogre::Real objMass, Ogre::Real colRestitution, 
            Ogre::Real objFriction, Ogre::Real objRollingFriction,
            BulletDebugDraw* debugDraw);
      /*! Destructor */
      ~FieldObject();

      /*! Get the fobject type
       * \return -> fobject type constant */
      int getType();

      /*! Verify if the object is facing up (ie: no pitch & roll) */
      bool isFacingUp();

      /*! Set the object's position to a new one
       * \param x -> x coordinate
       * \param y -> y coordinate
       * \param z -> z coordinate */
      void setPosition(Ogre::Real x, Ogre::Real y, Ogre::Real z);

      /*! Set the object's position to a new one
       * \param pos -> new position */
      void setPosition(Ogre::Vector3 pos);
   
      /*! Set object'ts position without doing a forced set physics step.
       * \note usefull when doing lots of setPosition on same interval.
       * \note you should call latter the forced steps. 
       * \param pos -> new position. */
      void setPositionWithoutForcedPhysicsStep(Ogre::Vector3 pos);

      /*! Set orientation angle of the object
       * \param a -> orientation angle value */
      void setOrientation(Ogre::Degree a);
      /*! Set float angles  of the object  */
      void setOrientation(Ogre::Quaternion qOri);

      /*! Get the object's current orientation quaternion */
      Ogre::Quaternion getOrientation();
      /*! Get the object's current orientation on Y axys  */
      Ogre::Real getOrientationY();

      /*! Get object's position
       * \return object position vector */
      Ogre::Vector3 getPosition();

      /*! Get the world align bounding box of the object
       * \return bounding box aligned to the world coordinates. */
      Ogre::AxisAlignedBox getBoundingBox();

      /*! Set the friction acceleration to all velocities
       * \param f -> new friction value */
      void setFriction(Ogre::Real f);

      /*! Set the rigid body damping
       * \param linear -> linear damping
       * \param rotational -> rotational damping */
      void setDamping(Ogre::Real linear, Ogre::Real rotational);

      /*! Update the objects positions, base on its velocity and collisions
       * \return false when object will no more move */
      bool update();

      /*! Compare the object's scene node to scNode
       * \param scNode -> scene node to compare with
       * \return true if they are the same. */
      bool compare(Ogre::SceneNode* scNode);

      /*! Get the sphere used as bounding sphere
       * \return -> copy of the sphere used */
      Ogre::Sphere getSphere();
      
      /*! set the sphere as bounding sphere
       * \param r -> sphere radius */
      void setSphere(Ogre::Real r);

      /*! Get the sphere radius, if any
       * \return shpere radius */
      Ogre::Real getSphereRadius();

      /*! Get the 'on-field' distance from object to a point.
       * \param x -> point's x coordinate
       * \param z -> point's z coordinate
       * \return distance from object to point. */
      Ogre::Real getDistanceTo(Ogre::Real x, Ogre::Real z);

      /*! \return the last distance calculated by #getDistanceTo 
       * \note this value isn't thread-safe. */
      Ogre::Real getLastCalculatedDistance();

      /*! Verify if the object has free way to a point. A 'free way' is
       * defined by no potential colisions with other disks (a colision with
       * the ball is allowed).
       * \note this function is somewhat expensive (three ray casts), so 
       *       optimize its calls.
       * \param query RaySceneQuery to use for each ray cast.
       * \param x -> point's x coordinate
       * \param z -> point's z coordinate
       * \return if has free way  */
      bool hasFreeWayTo(Ogre::RaySceneQuery* query, Ogre::Real x, Ogre::Real z);

      /*! Get Model name
       * \return -> string with model internal name on Ogre */
      Ogre::String getName();

      /*! Set the object's position as a replay-mode */
      void setPositionAsReplay(Ogre::Vector3& pos, Ogre::Quaternion& angles);

      /* Set a visual target mark for debug porpouses  */
      void pointTarget(Ogre::Vector3 target);

      /*! Apply a central force at the object */ 
      void applyForce(Ogre::Real x, Ogre::Real y, Ogre::Real z);

      /*! Get the ammount of time after last collision with the object
       * \return timer after last collision in ms */
      unsigned long getLastCollision();

      /*! Set the time the last collision with the object occurred */
      void setLastCollision();

      /*! Do things before a call to the physics step,
       * like setting its status as not moved, etc. */
      void prePhysicStep();
      
      /*! Verify if the object moved at current happened physics step
       * \return true if moved */
      bool getMovedFlag();
   
      /*! Verify if the object moved at physics step before the 
       * current happened one.
       * \return true if moved */
      bool getLastMovedFlag();

      /*! \return current linear velocity from rigid body */
      btVector3 getLinearVelocity();
      /*! Set current linear velocity for rigid body
       * \param vel linear velocity value */
      void setLinearVelocity(btVector3 vel);

      /*! \return current angular velocity from rigid body */
      btVector3 getAngularVelocity();
      /*! Set current angular velocity for rigid body
       * \param vel angular velocity value */
      void setAngularVelocity(btVector3 vel);

      /*! Hide the model */
      void hide();
      /*!  Show the previously hidden model */
      void show();

      /*! \return Bullet debug draw used */ 
      BulletDebugDraw* getDebugDraw() { return debugDraw; };

   protected:
      
      /*! Apply friction to a velocity component
       * \param v -> velocity component
       * \param f -> friction value 
       * \return false when velocity reaches 0 */
      bool applyFriction(Ogre::Real& v, Ogre::Real f);

      /*! Apply current angles to the sceneNode and bullet */
      void applyAngles();
      /*! Apply specific angles to the scene Node and bullet */
      void applyAngles(Ogre::Real aX, Ogre::Real aY, Ogre::Real aZ);
      /*! Apply tranformation quaternion to scene node and bullet */
      void applyAngles(Ogre::Quaternion transform);

      /*! Check if value is in target -with delta */
      bool checkValueDelta(Ogre::Real value, Ogre::Real target, 
            Ogre::Real delta);

      /*! Internal check for #hasFreeWayTo ray casts */
      bool checkNoColliders(Ogre::RaySceneQuery* query, 
            Ogre::Real targetDistance);

      int type;    /**< The fobject type */

      Ogre::SceneManager* pSceneManager; /**< Pointer to the scenemgr used */
      Ogre::Entity* model;        /**< Model used for object */
      Ogre::SceneNode* sceneNode; /**< Scene node used for object */

      Ogre::String mName;         /**< Model Name */
 
      Ogre::Real friction;        /**< Friction value */
      Ogre::Real rollingFriction; /**< Friction value when rolling */
      Ogre::Sphere sphere;        /**< Sphere used as bounding 'box' */
      Ogre::Real angleX;          /**< Angle along X axys */
      Ogre::Real angleZ;          /**< Angle along Z axys */
      Ogre::Degree orientation;   /**< Angle along y axys */

      Ogre::Real floorPosition;   /**< Y Position where the object is at 
                                       the floor*/

      OgreMotionState* motionState;  /**< Bullet motion state */ 
      btCollisionShape* collisionShape; /**< Shape for collision */
      btRigidBody* rigidBody;           /**< fobject in bullet world */

      Ogre::Timer lastCollision;   /**< Last collision */

      Ogre::Real lastDistance;/**< last distance calculated by getDistanceTo */
      BulletDebugDraw* debugDraw; /**< use for draw debugs */
      
   private:
      btScalar mass;
      btScalar restitution;
};

};

#endif
