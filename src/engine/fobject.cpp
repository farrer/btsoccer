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

#include "fobject.h"
#include "ball.h"
#include "../physics/bulletlink.h"
#include "../physics/ogremotionstate.h"
using namespace BtSoccer;

#define UNDEFINED_POS -10000.0f

/***********************************************************************
 *                             Constructor                             *
 ***********************************************************************/
FieldObject::FieldObject(int fType, Ogre::String name, Ogre::String fileName,
      Ogre::SceneManager* ogreSceneManager, Ogre::Real scale,
      Ogre::Real objMass, Ogre::Real colRestitution, 
      Ogre::Real objFriction, Ogre::Real objRollingFriction,
      BulletDebugDraw* debugDraw)
{
   Ogre::Vector3 diff;
   this->debugDraw = debugDraw;
   type = fType;
   pSceneManager = ogreSceneManager;
   mName = name;
   lastCollision.reset();
   floorPosition = UNDEFINED_POS;
   mass = objMass;
   restitution = colRestitution;
   friction = objFriction;
   rollingFriction = objRollingFriction;
   lastDistance = 0.0f;
   float height = UNDEFINED_POS;

   if(pSceneManager)
   {
      /* Get Model */
      model = ogreSceneManager->createEntity(name, fileName, "game");
      model->setRenderQueueGroup(Ogre::RENDER_QUEUE_MAIN);
      /* Add it to the scene */
      sceneNode = ogreSceneManager->getRootSceneNode()->createChildSceneNode();
      sceneNode->attachObject(model);
      /* Set its scale */
      sceneNode->setScale(scale, scale, scale);

      /* Set sphere radius */
      Ogre::AxisAlignedBox box = model->getBoundingBox();
      box.scale(Ogre::Vector3(scale, scale, scale));
      diff = box.getMaximum() - box.getMinimum();
      (diff[0] > diff[2])?setSphere(diff[0]/2.0f):setSphere(diff[2]/2.0f);
      floorPosition = -box.getMinimum()[1];
      height = diff[1];
   }
   else
   {
      model = NULL;
      sceneNode = NULL;
   }

   /* Create physic values */
   motionState = new OgreMotionState(btTransform(btQuaternion(0,0,0,1),
                btVector3(0,12.5f,0) * OGRE_TO_BULLET_FACTOR), sceneNode);
   btVector3 inertia(0,0,0);
   
   switch(type)
   {
      case TYPE_BALL:
      case TYPE_BALL_AI:
      {
         if(floorPosition == UNDEFINED_POS)
         {
            /* Should only happen on unit tests */
            floorPosition = 0.11f;
         }
         /* Ball radius is equal to half Y wich is floorPosition. */
         collisionShape = new btSphereShape(floorPosition * 
                                            OGRE_TO_BULLET_FACTOR);
         //Actual ball radius is 0.11f in ogre units.
      }
      break;
      case TYPE_DISK:
      case TYPE_DISK_AI:
      {
         if(floorPosition == UNDEFINED_POS)
         {
            floorPosition = 0.10335f;
            height = 0.171f;
         }
         /* The disk is a compound shape of a cylinder and a "hat" cone. */
         btTransform transform;
         btCompoundShape* compoundShape;
         compoundShape = new btCompoundShape();

         float segY = 0.024f; // Y coordinate where ends the cylinder shape.

         collisionShape = new btCylinderShape(btVector3(0.6f, segY, 0.6f)
               * OGRE_TO_BULLET_FACTOR);
         transform.setIdentity();
         transform.setOrigin(btVector3(0.0f, -floorPosition + segY, 0.0f)
               * OGRE_TO_BULLET_FACTOR);
         compoundShape->addChildShape(transform, collisionShape);

         float coneHeight = height - 2*segY;
         transform.setIdentity();
         transform.setOrigin(btVector3(0.0f, 
                  (coneHeight / 2.0f) + (2 * segY) - floorPosition, 0.0f)
               * OGRE_TO_BULLET_FACTOR);
         collisionShape = new btConeShape(0.6f * OGRE_TO_BULLET_FACTOR, 
               (coneHeight) * OGRE_TO_BULLET_FACTOR);
         compoundShape->addChildShape(transform, collisionShape);
         
         collisionShape = compoundShape;
      }
      break;
      case TYPE_GOAL_KEEPER:
      case TYPE_GOAL_KEEPER_AI:
      {
         if(floorPosition == UNDEFINED_POS)
         {
            floorPosition = 0.43055f;
         }
         collisionShape = new btBoxShape(btVector3(1.0f, 0.435f, 0.145f)
               * OGRE_TO_BULLET_FACTOR);
      }
      break;
      default:
      { 
         collisionShape = new btSphereShape(0.05f);
      }
   }
   collisionShape->calculateLocalInertia(mass, inertia);
   collisionShape->setUserPointer(this);

   btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass,
         motionState, collisionShape, inertia);
   
   rigidBody = new btRigidBody(rigidBodyCI);
   rigidBody->setFriction(friction);
   rigidBody->setRollingFriction(rollingFriction);
   rigidBody->setSpinningFriction(0.2f);
   /*rigidBody->setAnisotropicFriction(
         collisionShape->getAnisotropicRollingFrictionDirection(),
         btCollisionObject::CF_ANISOTROPIC_ROLLING_FRICTION);*/
   rigidBody->setRestitution(restitution);
   //rigidBody->setContactProcessingThreshold(1.0f);

   BulletLink::addRigidBody(rigidBody);

   /* Default values */
   orientation = 0;
   angleX = 0;
   angleZ = 0;
}

/***********************************************************************
 *                              Destructor                             *
 ***********************************************************************/
FieldObject::~FieldObject()
{
   if(pSceneManager)
   {
      /* Unload things */
      sceneNode->detachObject(model);
      pSceneManager->destroySceneNode(sceneNode);
      pSceneManager->destroyEntity(model);
   }
   
   /* Delete bullet related things */
   BulletLink::removeRigidBody(rigidBody);
   delete rigidBody;
   if( (type == TYPE_DISK) || (type == TYPE_DISK_AI) )
   {
      /* Remove each shape of the compound */
      btCompoundShape* compoundShape = (btCompoundShape*)collisionShape;
      int totalShapes = compoundShape->getNumChildShapes();
      for(int i=0; i < totalShapes; i++)
      {
         btCollisionShape* shape = compoundShape->getChildShape(i);
         //compoundShape->removeChildShapeByIndex(i);
         delete shape;
      }
      /* Remove the compound shape itself */
      delete compoundShape;
   }
   else
   {
      /* Just remove the single collision shape */
      delete collisionShape;
   }

   delete motionState;
}

/***********************************************************************
 *                              getType                                *
 ***********************************************************************/
int FieldObject::getType()
{
   return type;
}

/***********************************************************************
 *                              getName                                *
 ***********************************************************************/
Ogre::String FieldObject::getName()
{
   return(mName);
}

/***********************************************************************
 *                         setPositionAsReplay                         *
 ***********************************************************************/
void FieldObject::setPositionAsReplay(Ogre::Vector3& pos, 
                                      Ogre::Quaternion& angles)
{
   /* Only set the scene node for replay render */
   sceneNode->setPosition(pos);
   applyAngles(angles);
}

/***********************************************************************
 *                              applyForce                             *
 ***********************************************************************/
void FieldObject::applyForce(Ogre::Real x, Ogre::Real y, Ogre::Real z)
{
   rigidBody->activate(true);
   rigidBody->applyCentralForce(btVector3(x,y,z));
}

/***********************************************************************
 *                             setPosition                             *
 ***********************************************************************/
void FieldObject::setPosition(Ogre::Real x, Ogre::Real y, Ogre::Real z)
{
   Ogre::Vector3 pos;
   pos.x = x;
   pos.y = y;
   pos.z = z;

   setPosition(pos);
}

/***********************************************************************
 *                             setPosition                             *
 ***********************************************************************/
void FieldObject::setPositionWithoutForcedPhysicsStep(Ogre::Vector3 pos)
{
   /* Verify if not bellow ground */
   if(pos.y < floorPosition)
   {
      pos.y = floorPosition;
   }
   
   /* set new current */
   rigidBody->clearForces();
   btTransform transform = rigidBody->getCenterOfMassTransform();
   transform.setOrigin(btVector3(pos.x,pos.y,pos.z) * OGRE_TO_BULLET_FACTOR);
   rigidBody->setCenterOfMassTransform(transform);
   
   /* Set model */
   if(sceneNode)
   {
      sceneNode->setPosition(pos);
      sphere.setCenter(pos);
   }
}

/***********************************************************************
 *                             setPosition                             *
 ***********************************************************************/
void FieldObject::setPosition(Ogre::Vector3 pos)
{
   /* Set the position */
   setPositionWithoutForcedPhysicsStep(pos);

   /* Do the forced step */
   BulletLink::forcedStep();
   motionState->clearMovedFlag();
}

/***********************************************************************
 *                              getSphere                              *
 ***********************************************************************/
Ogre::Sphere FieldObject::getSphere()
{
   return sphere;
}

/***********************************************************************
 *                           getSphereRadius                           *
 ***********************************************************************/
Ogre::Real FieldObject::getSphereRadius()
{
   return sphere.getRadius();
}

/***********************************************************************
 *                              setSphere                              *
 ***********************************************************************/
void FieldObject::setSphere(Ogre::Real r)
{
   sphere.setRadius(r);
}

/***********************************************************************
 *                             getPosition                             *
 ***********************************************************************/
Ogre::Vector3 FieldObject::getPosition()
{
   if(sceneNode != NULL)
   {
      /* Graphical world is available, must use the scene node */
      return sceneNode->getPosition();
   }
   else
   {
      /* No graphics (usually for unity tests), let's direct use bullet. */
      const btVector3& pos = rigidBody->getCenterOfMassPosition();
      return Ogre::Vector3(pos[0], pos[1], pos[2]) * BULLET_TO_OGRE_FACTOR; 
   }
}

/***********************************************************************
 *                            getDistanceTo                            *
 ***********************************************************************/
Ogre::Real FieldObject::getDistanceTo(Ogre::Real x, Ogre::Real z)
{
   Ogre::Vector3 pos = getPosition();
   lastDistance = sqrt( (pos.x - x) * (pos.x - x) +
                        (pos.z - z) * (pos.z - z) );
   return lastDistance;
}

/***********************************************************************
 *                      getLastCalculatedDistance                      *
 ***********************************************************************/
Ogre::Real FieldObject::getLastCalculatedDistance()
{
   return lastDistance;
}

/***********************************************************************
 *                             hasFreeWayTo                            *
 ***********************************************************************/
bool FieldObject::hasFreeWayTo(Ogre::RaySceneQuery* query, 
      Ogre::Real x, Ogre::Real z)
{
   /* To have free way to a point, the object's sphere representation
    * (as this function is usually called for disks and ball), must have
    * no colliders to other disks from 3 rays: the one from origin to point,
    * and both two from perpendicular sphere limits to point relative
    * position with same direction (as if we propagated all the circle
    * projection from sphere from current position to target one). */

   /* Calculate distance to target */
   Ogre::Vector2 delta = Ogre::Vector2(x - getPosition().x,
                                       z - getPosition().z);
   Ogre::Real distance = Ogre::Math::Sqrt(delta[0]*delta[0] + 
                                          delta[1]*delta[1] );
   /* As will 'hit' the target whith its extremety, the distance
    * must be decremented by the field object's radius */
   distance -= getSphereRadius();

   Ogre::Vector3 origin(getPosition().x, 0.0f, getPosition().z);

   /* First check: no potential collider from origin to target. */
   Ogre::Vector3 dir(delta[0], 0.0f, delta[1]);
   dir.normalise();

   query->setRay(Ogre::Ray(origin, dir));
   query->setSortByDistance(true);
#if BTSOCCER_RENDER_DEBUG
   debugDraw->drawRay(origin, dir, distance, Ogre::Vector3(0.0f, 0.0f, 1.0f));
#endif
   if(!checkNoColliders(query, distance))
   {
      /* Collided. */
      return false;
   }

   /* Being (xh,zh) the point at object's circle along direction to target,
    * the two points to check are rotated along circle's origin by 90 and -90
    * degrees. */
    Ogre::Vector3 perpDir(-dir[2], 0.0f, dir[0]);
    Ogre::Vector3 newOrigin = perpDir * getSphereRadius() + origin;

    query->setRay(Ogre::Ray(newOrigin, dir));
    query->setSortByDistance(true);
#if BTSOCCER_RENDER_DEBUG
   debugDraw->drawRay(newOrigin, dir, distance, 
         Ogre::Vector3(0.0f, 1.0f, 1.0f));
#endif
    if(!checkNoColliders(query, distance))
    {
       return false;
    }

    /* Second perpendicular vector */
    perpDir = Ogre::Vector3(dir[2], 0.0f, -dir[0]);
    newOrigin = perpDir * getSphereRadius() + origin;
    query->setRay(Ogre::Ray(newOrigin, dir));
    query->setSortByDistance(true);
#if BTSOCCER_RENDER_DEBUG
   debugDraw->drawRay(newOrigin, dir, distance, 
         Ogre::Vector3(1.0f, 0.0f, 1.0f));
#endif
    if(!checkNoColliders(query, distance))
    {
       return false;
    }

   return true;
}

/***********************************************************************
 *                           checkNoColliders                          *
 ***********************************************************************/
bool FieldObject::checkNoColliders(Ogre::RaySceneQuery* query, 
      Ogre::Real targetDistance)
{
   Ogre::RaySceneQueryResult &result = query->execute();
   Ogre::RaySceneQueryResult::iterator itr;

   bool done = false;
   for( itr = result.begin(); ((itr != result.end()) && (!done)); itr++ )
   {
      if(itr->movable)
      {
         if( (itr->movable->getName() != BALL_NAME) && 
             (itr->movable->getName() != getName()) )
         {
            if(itr->distance > targetDistance)
            {
               /* Collider is after target, so no need to continue the 
                * the query here (as sorted by distance), and no colliders
                * to target found. */
               break;
            }
            else
            {
               /* Not a ball neither itself and before target position:
                *  a collider was found. */
               printf("%s will collide with %s\n", getName().c_str(),
                      itr->movable->getName().c_str());
               return false;
            }
         }
      }
   }

   return true;
}

/***********************************************************************
 *                          getLinearVelocity                          *
 ***********************************************************************/
btVector3 FieldObject::getLinearVelocity()
{
   return rigidBody->getLinearVelocity();
}

/***********************************************************************
 *                          setLinearVelocity                          *
 ***********************************************************************/
void FieldObject::setLinearVelocity(btVector3 vel)
{
   rigidBody->setLinearVelocity(vel);
}

/***********************************************************************
 *                         getAngularVelocity                          *
 ***********************************************************************/
btVector3 FieldObject::getAngularVelocity()
{
   return rigidBody->getAngularVelocity();
}

/***********************************************************************
 *                         setAngularVelocity                          *
 ***********************************************************************/
void FieldObject::setAngularVelocity(btVector3 vel)
{
   rigidBody->setAngularVelocity(vel);
}

/***********************************************************************
 *                           getBoundingBox                            *
 ***********************************************************************/
Ogre::AxisAlignedBox FieldObject::getBoundingBox()
{
   return sceneNode->_getWorldAABB();
}

/***********************************************************************
 *                            checkValueDelta                          *
 ***********************************************************************/
bool FieldObject::checkValueDelta(Ogre::Real value, Ogre::Real target, 
      Ogre::Real delta)
{
   return ((value >= target-delta) && (value <= target+delta));
}

/***********************************************************************
 *                             isFacingUp                              *
 ***********************************************************************/
bool FieldObject::isFacingUp()
{
   Ogre::Real pitch = sceneNode->getOrientation().getPitch().valueDegrees();
   Ogre::Real roll = sceneNode->getOrientation().getRoll().valueDegrees();

   if( ( (checkValueDelta(pitch, 0.0f, 0.1f)) ||
         (checkValueDelta(pitch, 180.0f, 0.1f)) ||
         (checkValueDelta(pitch, -180.0f, 0.1f)) ) &&
       ( (checkValueDelta(roll, 0.0f, 0.1f)) ||
         (checkValueDelta(roll, 180.0f, 0.1f)) ||
         (checkValueDelta(roll, -180.0f, 0.1f)) ) )
   { 
      return true;
   }

   return false;
}

/***********************************************************************
 *                           setOrientation                            *
 ***********************************************************************/
void FieldObject::setOrientation(Ogre::Degree a)
{
   orientation = a;
   angleX = 0;
   angleZ = 0;
   applyAngles();
}

/***********************************************************************
 *                           setOrientation                            *
 ***********************************************************************/
void FieldObject::setOrientation(Ogre::Quaternion qOri)
{
   applyAngles(qOri);
}

/***********************************************************************
 *                             applyAngles                             *
 ***********************************************************************/
void FieldObject::applyAngles()
{
   Ogre::Quaternion qX(Ogre::Degree(angleX), Ogre::Vector3::UNIT_X);
   Ogre::Quaternion qY(orientation, Ogre::Vector3::UNIT_Y);
   Ogre::Quaternion qZ(Ogre::Degree(angleZ), Ogre::Vector3::UNIT_Z);
   Ogre::Quaternion res= qX*qY*qZ;

   applyAngles(res);
}

/***********************************************************************
 *                             applyAngles                             *
 ***********************************************************************/
void FieldObject::applyAngles(Ogre::Quaternion transform)
{
   /* set ogre scene node */
   if(sceneNode)
   {
      sceneNode->setOrientation(transform);
   }
   /* set bullet */
   btTransform btTrans = rigidBody->getCenterOfMassTransform();
   btTrans.setRotation(btQuaternion(transform.x, transform.y, 
                                    transform.z, transform.w));
   rigidBody->setCenterOfMassTransform(btTrans);
}

/***********************************************************************
 *                             applyAngles                             *
 ***********************************************************************/
void FieldObject::applyAngles(Ogre::Real aX, Ogre::Real aY, Ogre::Real aZ)
{
   Ogre::Quaternion qX(Ogre::Degree(aX), Ogre::Vector3::UNIT_X);
   Ogre::Quaternion qY(Ogre::Degree(aY), Ogre::Vector3::UNIT_Y);
   Ogre::Quaternion qZ(Ogre::Degree(aZ), Ogre::Vector3::UNIT_Z);

   sceneNode->setOrientation(qX*qY*qZ);
}

/***********************************************************************
 *                           getOrientation                            *
 ***********************************************************************/
Ogre::Real FieldObject::getOrientationY()
{
   return orientation.valueDegrees();
}

/***********************************************************************
 *                           getOrientation                            *
 ***********************************************************************/
Ogre::Quaternion FieldObject::getOrientation()
{
   return sceneNode->getOrientation();
}

/***********************************************************************
 *                           setFriction                               *
 ***********************************************************************/
void FieldObject::setFriction(Ogre::Real f)
{
   friction = f;
   rigidBody->setFriction(f);
}

/***********************************************************************
 *                           setDamping                                *
 ***********************************************************************/
void FieldObject::setDamping(Ogre::Real linear, Ogre::Real rotational)
{
   rigidBody->setDamping(linear, rotational);
}

/***********************************************************************
 *                               compare                               *
 ***********************************************************************/
bool FieldObject::compare(Ogre::SceneNode* scNode)
{
   return sceneNode == scNode;
}

/***********************************************************************
 *                                update                               *
 ***********************************************************************/
bool FieldObject::update()
{
   bool willMove = false; /* If will move at next update */

   return willMove;
}

/******************************************************************
 *                       getLastCollision                         *
 ******************************************************************/
unsigned long FieldObject::getLastCollision()
{
   return lastCollision.getMilliseconds();
}

/******************************************************************
 *                       setLastCollision                         *
 ******************************************************************/
void FieldObject::setLastCollision()
{
   lastCollision.reset();
}

/***********************************************************************
 *                           prePhysicStep                             *
 ***********************************************************************/
void FieldObject::prePhysicStep()
{
   motionState->clearMovedFlag();
   if((type == TYPE_BALL) && (sceneNode))
   {
      /* If we are moving at the floor, must do some fake-rolling (visual 
       * effect only, as we do not desire Bullet to treat the rolling by
       * itself, as it is pretty buggy - or not suitable to our simulation). */
      Ogre::Real posY = sceneNode->getPosition().y;
      bool atFloor = (posY - 0.1f <= floorPosition) && 
                     (posY + 0.1f >= floorPosition);
      float factor = (atFloor) ? 10.0f : 2.0f;
      btVector3 vel = rigidBody->getLinearVelocity();

      if((vel[0] != 0.0f) || (vel[2] != 0.0f))
      {
         angleX = ((int)(angleX + factor * vel[2])) % 360;
         angleZ = ((int)(angleZ + factor * vel[0])) % 360;
         applyAngles(angleX, orientation.valueDegrees(), angleZ);
      }
   }
}

/***********************************************************************
 *                        movedOnLastPhysicStep                        *
 ***********************************************************************/
bool FieldObject::getMovedFlag()
{
   return motionState->getMovedFlag();
}

/***********************************************************************
 *                        movedOnLastPhysicStep                        *
 ***********************************************************************/
bool FieldObject::getLastMovedFlag()
{
   return motionState->getLastMovedFlag();
}

/***********************************************************************
 *                           pointTarget                               *
 ***********************************************************************/
void FieldObject::pointTarget(Ogre::Vector3 target)
{
#if BTSOCCER_RENDER_DEBUG
   debugDraw->drawLine(getPosition(), target, Ogre::Vector3(1.0f, 0.0f, 1.0f));
#endif
}

/***********************************************************************
 *                                hide                                 *
 ***********************************************************************/
void FieldObject::hide()
{
   if(sceneNode != NULL)
   {
      sceneNode->setVisible(false);
   }
}

/***********************************************************************
 *                                show                                 *
 ***********************************************************************/
void FieldObject::show()
{
   if(sceneNode != NULL)
   {
      sceneNode->setVisible(true);
   }
}


