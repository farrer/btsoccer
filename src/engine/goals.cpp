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


#include "goals.h"
#include "field.h"
#include <OGRE/OgreRenderQueue.h>
#include <OGRE/OgreMaterialManager.h>
using namespace BtSoccer;

#define POLE_RADIUS      0.07f
#define POLE_DIAMETER    (0.1f * POLE_RADIUS)

/*********************************************************************
 *                         Goal Constructor                          *
 *********************************************************************/
Goal::Goal(Ogre::StaticGeometry::Region* modelRegion, Field* f)
{
   staticModel = modelRegion;
   field = f;
   boundingBox = staticModel->getWorldBoundingBox(true); 

   clear();
}

/*********************************************************************
 *                         Goal Constructor                          *
 *********************************************************************/
Goal::Goal(Field* f, bool upGoal)
{
   staticModel = NULL;
   field = f;
   //FIXME: corretly set bounding box for non graphical use!
   boundingBox = Ogre::AxisAlignedBox();

   clear();
}

/*********************************************************************
 *                               clear                               *
 *********************************************************************/
void Goal::clear()
{
   int i;
   for(i=0; i < 3; i++)
   {
      poleShape[i] = NULL; 
      poleMotionState[i] = NULL;
      poleRigidBody[i] = NULL;
   }
   for(i=0; i < 4; i++)
   {
      netShape[i] = NULL; 
      netMotionState[i] = NULL;
      netRigidBody[i] = NULL;
   }

}

/*********************************************************************
 *                          Goal Destructor                          *
 *********************************************************************/
Goal::~Goal()
{
   int i;
   for(i=0; i < 3; i++)
   {
      if(poleRigidBody[i] != NULL)
      {
         BulletLink::removeRigidBody(poleRigidBody[i]);
         delete poleShape[i]; 
         delete poleMotionState[i];
         delete poleRigidBody[i];
      }
   }
   for(i=0; i < 4; i++)
   {
      if(netRigidBody[i] != NULL)
      {
         BulletLink::removeRigidBody(netRigidBody[i]);
         delete netShape[i]; 
         delete netMotionState[i];
         delete netRigidBody[i];
      }
   }
}

/*********************************************************************
 *                           createPole                              *
 *********************************************************************/
void Goal::createPole(int i, btVector3 halfSizes, btVector3 pos, 
      bool sidePole, Ogre::Vector3 goalBBox)
{
   /* Create pole shapes */ 
   if(sidePole)
   {
      poleShape[i] = new btCylinderShape(halfSizes * OGRE_TO_BULLET_FACTOR);
   }
   else
   {
      poleShape[i] = new btCylinderShapeZ(halfSizes * OGRE_TO_BULLET_FACTOR);
   }
   poleMotionState[i] = new btDefaultMotionState(btTransform(
            btQuaternion(0,0,0,1), btVector3(0,-1,0)));
   btRigidBody::btRigidBodyConstructionInfo
      poleRigidBodyCI(0,poleMotionState[i],poleShape[i],btVector3(0,0,0));
   poleRigidBody[i] = new btRigidBody(poleRigidBodyCI);
   poleRigidBody[i]->setRestitution(0.6f);
   poleRigidBody[i]->setFriction(0.2f);
   BulletLink::addRigidBody(poleRigidBody[i]);

   /* Set its position on table */
   btTransform transform = poleRigidBody[i]->getCenterOfMassTransform();
   transform.setOrigin(pos * OGRE_TO_BULLET_FACTOR);
   poleRigidBody[i]->setCenterOfMassTransform(transform);
}

/*********************************************************************
 *                           createPoles                             *
 *********************************************************************/
void Goal::createPoles(float halfHeight, float halfZ, bool upGoal,
      Ogre::Vector3 goalBBox)
{
   float xPos = (upGoal)?field->getGoalPosition():-field->getGoalPosition();
   float sidePoleCor = 0.025f;/* correction factor for side pole to side goal */

   /* Two side poles */
   btVector3 pPos = btVector3(xPos, halfHeight, -halfZ+POLE_RADIUS+sidePoleCor);
   createPole(0, 
         btVector3(POLE_RADIUS, halfHeight-(POLE_RADIUS/2.0f), POLE_RADIUS),
         pPos, true, goalBBox);
   sidePolesChecker[0].setRadius(POLE_RADIUS);
   sidePolesChecker[0].setCenter(Ogre::Vector3(pPos[0], POLE_RADIUS, pPos[2]));
   
   pPos = btVector3(xPos,halfHeight,halfZ-POLE_RADIUS-sidePoleCor);
   createPole(1, 
         btVector3(POLE_RADIUS, halfHeight-(POLE_RADIUS/2.0f), POLE_RADIUS),
         pPos, true, goalBBox);
   sidePolesChecker[1].setRadius(POLE_RADIUS);
   sidePolesChecker[1].setCenter(Ogre::Vector3(pPos[0], POLE_RADIUS, pPos[2]));

   /* Upper Pole (travessao) */
   createPole(2, btVector3(POLE_RADIUS, POLE_RADIUS, halfZ-POLE_RADIUS),
         btVector3(xPos, goalBBox.y-POLE_RADIUS, 0.0f), false,
         goalBBox);
}

/*********************************************************************
 *                       intersectSidePoles                          *
 *********************************************************************/
bool Goal::intersectsSidePoles(Ogre::AxisAlignedBox bbox)
{
   if(sidePolesChecker[0].intersects(bbox))
   {
      return true;
   }
   
   return sidePolesChecker[1].intersects(bbox);
}

/*********************************************************************
 *                           createNet                               *
 *********************************************************************/
void Goal::createNet(int i, btVector3 halfSizes, btVector3 pos, 
      Ogre::Vector3 goalBBox)
{
   netShape[i] = new btBoxShape(halfSizes * OGRE_TO_BULLET_FACTOR);
   netMotionState[i] = new btDefaultMotionState(btTransform(
            btQuaternion(0,0,0,1), btVector3(0,-1,0)));
   btRigidBody::btRigidBodyConstructionInfo
      poleRigidBodyCI(0,netMotionState[i],netShape[i],btVector3(0,0,0));
   netRigidBody[i] = new btRigidBody(poleRigidBodyCI);
   netRigidBody[i]->setRestitution(0.2f);
   netRigidBody[i]->setFriction(0.2f);
   BulletLink::addRigidBody(netRigidBody[i]);

   /* Set its position on field table */
   btTransform transform = netRigidBody[i]->getCenterOfMassTransform();
   transform.setOrigin(pos * OGRE_TO_BULLET_FACTOR);
   netRigidBody[i]->setCenterOfMassTransform(transform);

}

/*********************************************************************
 *                           createNets                              *
 *********************************************************************/
void Goal::createNets(float halfX, float halfHeight, float halfZ, bool upGoal,
      Ogre::Vector3 goalBBox)
{
   float xPos = (upGoal)?field->getGoalPosition():-field->getGoalPosition();
   float signal = (upGoal)?1.0f:-1.0f;

   /* side nets */
   createNet(0, btVector3(halfX-POLE_RADIUS-0.01f, halfHeight-0.05f, 0.1),
         btVector3(xPos+signal*(halfX-0.01f), halfHeight-0.01f, 
                   -halfZ+POLE_RADIUS), goalBBox);
   createNet(1, btVector3(halfX-POLE_RADIUS-0.01f, halfHeight-0.05f, 0.1),
         btVector3(xPos+signal*(halfX-0.01f), halfHeight-0.01f, 
                   halfZ-POLE_RADIUS), goalBBox);

   /* Back net */
   createNet(2, btVector3(0.005f, halfHeight-0.05f, halfZ-POLE_RADIUS-0.01f),
         btVector3(xPos+signal*(goalBBox.x-0.09f), halfHeight-0.01f, 
                   0.0f), goalBBox);

   /* Top net */
   createNet(3, btVector3(halfX-POLE_RADIUS, 0.005f, halfZ-POLE_RADIUS-0.01f),
         btVector3(xPos+signal*(halfX-0.01f), goalBBox.y-0.09f, 
                   0.0f), goalBBox);
}

/*********************************************************************
 *                          getBoundingBox                           *
 *********************************************************************/
Ogre::AxisAlignedBox Goal::getBoundingBox()
{
   return boundingBox;
}

/*********************************************************************
 *                            Constructor                            *
 *********************************************************************/
Goals::Goals(Ogre::SceneManager* ogreSceneManager, Field* f)
{
   pSceneManager = ogreSceneManager;
   field = f;
   float scale = GOAL_SCALE_FACTOR;
   isOpaque = true;

   /* Load and positionate goals */
   Ogre::Entity* goalModel;
   goalModel = ogreSceneManager->createEntity("goals", "goal/goal.mesh");
   goalModel->setRenderQueueGroup(Ogre::RENDER_QUEUE_9);
   goals = ogreSceneManager->createStaticGeometry("Goals");
   goals->setRenderQueueGroup(Ogre::RENDER_QUEUE_9);

   /* Upper */
   goals->addEntity(goalModel, Ogre::Vector3(field->getGoalPosition(),0,0),
      Ogre::Quaternion(Ogre::Math::Sqrt(0.5),0,-Ogre::Math::Sqrt(0.5),0),
      Ogre::Vector3(scale,scale,scale));

   /* Down */
   goals->addEntity(goalModel, Ogre::Vector3(-field->getGoalPosition(),0,0),
      Ogre::Quaternion(Ogre::Math::Sqrt(0.5),0,Ogre::Math::Sqrt(0.5),0),
      Ogre::Vector3(scale,scale,scale));
   goals->build();
   
   /* Calculate sizes */
   Ogre::AxisAlignedBox box = goalModel->getBoundingBox();
   box.scale(Ogre::Vector3(scale, scale, scale));
   goalSize = box.getSize();
   /* Invert x with z as rotated */
   float aux = goalSize.x;
   goalSize.x = goalSize.z;
   goalSize.z = aux;
   
   /* No more needed entity after add */
   pSceneManager->destroyEntity(goalModel);

   /* Lets get regions */
   Ogre::StaticGeometry::RegionIterator it = goals->getRegionIterator();
   downGoal = new Goal(it.getNext(), field);
   upGoal = new Goal(it.getNext(), field);

   createCollisionShapes();
}

/*********************************************************************
 *                            Constructor                            *
 *********************************************************************/
Goals::Goals(Field* f)
{
   field = f;
   upGoal = new Goal(field, true);
   downGoal = new Goal(field, false);
   pSceneManager = NULL;
}

/*********************************************************************
 *                             Destructor                            *
 *********************************************************************/
Goals::~Goals()
{
   if(pSceneManager != NULL)
   {
      pSceneManager->destroyStaticGeometry(goals);
   }
   delete upGoal;
   delete downGoal;
}

/*********************************************************************
 *                         createCollisionShapes                     *
 *********************************************************************/
void Goals::createCollisionShapes()
{
   float halfHeight = goalSize.y / 2.0f;
   float halfZ = goalSize.z / 2.0f; /* .x as rotated. */
   float halfX = goalSize.x / 2.0f; /* .z as rotated. */

   /* Create and positionate upper goal */
   upGoal->createPoles(halfHeight, halfZ, true, goalSize);
   upGoal->createNets(halfX, halfHeight, halfZ, true, goalSize);

   /* Create and positionate downer goal */
   downGoal->createPoles(halfHeight, halfZ, false, goalSize);
   downGoal->createNets(halfX, halfHeight, halfZ, false, goalSize);
}

/*********************************************************************
 *                            getUpGoalBox                           *
 *********************************************************************/
Ogre::AxisAlignedBox Goals::getUpGoalBox()
{
   return upGoal->getBoundingBox();
}

/*********************************************************************
 *                           getDownGoalBox                          *
 *********************************************************************/
Ogre::AxisAlignedBox Goals::getDownGoalBox()
{
   return downGoal->getBoundingBox();
}

/*********************************************************************
 *                        insersectsSidePoles                        *
 *********************************************************************/
bool Goals::intersectsSidePoles(bool upper, Ogre::AxisAlignedBox bbox)
{
   bool res = false;
   if(upper)
   {
      res = upGoal->intersectsSidePoles(bbox);
   }
   else
   {
      res = downGoal->intersectsSidePoles(bbox);
   }
   
   return res;
}

/*********************************************************************
 *                         getNearGoalFactor                         *
 *********************************************************************/
float Goals::getNearGoalFactor(bool upperTeam, float posX, float posZ)
{
   Ogre::Vector2 halfSize = field->getHalfSize();
   float sideDelta = field->getSideDelta()[0];
   Ogre::Vector2 littleAreaDelta = field->getLittleAreaDelta();
   
   if(upperTeam)
   {
      /* Attack down goal */

      if(posX < -halfSize.x+littleAreaDelta[0])
      {
         /* After attack little area: 50% */
         return 0.5f;
      }
      else if( (posX >= -halfSize.x+littleAreaDelta[0]) &&
               (posX <= -halfSize.x+(2.7f*littleAreaDelta[0])) )
      {
         /* Intermediate on area or near */
         if( (posZ >= -halfSize.x+(3.5f*sideDelta)) && 
             (posZ <= halfSize.x-(3.5f*sideDelta)) )
         {
            /* Front Goal. Better position to attack */
            return 1.0f;
         }
         else
         {
            /* Not so front, but still near */
            return 0.75f;
         } 
      }
      else if( (posX >= -halfSize.x+(2.7f*littleAreaDelta[0])) &&
               (posX <= -littleAreaDelta[0]) )
      {
         /* Intermediate area, but not on middle */
         return 0.75f;
      }
      else if( (posX <= littleAreaDelta[0]) &&
               (posX >= -littleAreaDelta[0]) )
      {
         /* Middle field area */
         return 0.5f;
      }
      else if( (posX >= littleAreaDelta[0]) && 
               (posX <= halfSize.x-(2.7f*littleAreaDelta[0])) )
      {
         /* Intermediate defensive area */
         return 0.25f;
      }
   }
   else
   {
      /* Attack upper goal */
      
      if(posX > halfSize.x-littleAreaDelta[0])
      {
         /* After attack little area: 50% */
         return 0.5f;
      }
      else if( (posX <= halfSize.x-littleAreaDelta[0]) &&
               (posX >= halfSize.x-(2.7f*littleAreaDelta[0])) )
      {
         /* Intermediate on area or near */
         if( (posZ >= -halfSize.x+(3.5f*sideDelta)) && 
             (posZ <= halfSize.x-(3.5f*sideDelta)) )
         {
            /* Front Goal. Better position to attack */
            return 1.0f;
         }
         else
         {
            /* Not so front, but still near */
            return 0.75f;
         } 
      }
      else if( (posX <= halfSize.x-(2.7f*littleAreaDelta[0])) &&
               (posX >= littleAreaDelta[0]) )
      {
         /* Intermediate area, but not on middle */
         return 0.75f;
      }
      else if( (posX <= littleAreaDelta[0]) &&
               (posX >= -littleAreaDelta[0]) )
      {
         /* Middle field area */
         return 0.5f;
      }
      else if( (posX <= -littleAreaDelta[0]) && 
               (posX >= -halfSize.x+(2.7f*littleAreaDelta[0])) )
      {
         /* Intermediate defensive area */
         return 0.25f;
      }
   }

   /* Fully defensive area */
   return 0.0f;
}

/*********************************************************************
 *                          setTransparent                           *
 *********************************************************************/
void Goals::setTransparent()
{
   if(isOpaque)
   {
      Ogre::MaterialManager::getSingleton().getByName("goal")->setDiffuse(
            0.64f, 0.64f, 0.64f, 0.2f);
      isOpaque = false;
   }
}

/*********************************************************************
 *                           setOpaque                               *
 *********************************************************************/
void Goals::setOpaque()
{
   if(!isOpaque)
   {
      Ogre::MaterialManager::getSingleton().getByName("goal")->setDiffuse(
            0.64f, 0.64f, 0.64f, 1.0f);
      isOpaque = true;
   }
}




