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



#include "bulletlink.h"
#include "../engine/ball.h"
#include "../engine/fobject.h"
#include "../engine/field.h"
#include "../engine/rules.h"
#include "../engine/team.h"
#include "../engine/teamplayer.h"
#include "../engine/goalkeeper.h"
#include "../btsoccer.h"
#include <kosound/sound.h>
#include <kobold/ogre3d/ogrefilereader.h>

using namespace BtSoccer;


/***********************************************************************
 *                     bulletLinkTickCallback                          *
 ***********************************************************************/
void bulletLinkTickCallback(btDynamicsWorld *world, btScalar timeStep) 
{
   BulletLink::tickCallBack();
}

/***********************************************************************
 *                            setPointers                              *
 ***********************************************************************/
void BulletLink::setPointers(Team* tA, Team* tB, FieldObject* b, 
      Field* f, bool online)
{
   onlineGame = online;
   teamA = tA;
   teamB = tB;
   ball = b;
   field = f;
   if(tA != NULL)
   {
      diskDiameter = tA->getDisk(0)->getSphere().getRadius()*2.0f;
   }
}

/***********************************************************************
 *                          createBulletWorld                          *
 ***********************************************************************/
void BulletLink::createBulletWorld()
{
   collisionConfiguration = new btDefaultCollisionConfiguration();
   dispatcher = new btCollisionDispatcher(collisionConfiguration);
   broadPhase = new btDbvtBroadphase();
   solver = new btSequentialImpulseConstraintSolver();
   dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadPhase, 
         solver,collisionConfiguration);
   dynamicsWorld->setGravity(btVector3(0, -9.8f, 0));
   dynamicsWorld->setInternalTickCallback(bulletLinkTickCallback);
}

/***********************************************************************
 *                        deleteBulletWorld                            *
 ***********************************************************************/
void BulletLink::deleteBulletWorld()
{
   delete dynamicsWorld;
   delete solver;
   delete dispatcher;
   delete collisionConfiguration;
   delete broadPhase;
}

/***********************************************************************
 *                          addRigidBody                               *
 ***********************************************************************/
void BulletLink::addRigidBody(btRigidBody* rigidBody)
{
   dynamicsWorld->addRigidBody(rigidBody);
}

/***********************************************************************
 *                        removeRigidBody                              *
 ***********************************************************************/
void BulletLink::removeRigidBody(btRigidBody* rigidBody)
{
   dynamicsWorld->removeRigidBody(rigidBody);
}

/***********************************************************************
 *                          forcedStep                                 *
 ***********************************************************************/
void BulletLink::forcedStep(btScalar timeStep, int maxSubSteps)
{
   /* Do the fixed step */
   rulesEnabled = false;
   btScalar stepInSeconds = timeStep / 1000.0f;
   dynamicsWorld->stepSimulation(stepInSeconds, maxSubSteps, 
                                 BULLET_FREQUENCY);
   rulesEnabled = true;

   debugDraw();
}

/***********************************************************************
 *                             preStep                                 *
 ***********************************************************************/
void BulletLink::preStep()
{
   if(ball)
   {
      ball->prePhysicStep();
   }
   if(teamA)
   {
      teamA->prePhysicStep();
   }
   if(teamB)
   {
      teamB->prePhysicStep();
   }
}

/***********************************************************************
 *                              step                                   *
 ***********************************************************************/
void BulletLink::step(btScalar timeStep, int maxSubSteps)
{
   /* Things before physics step */
   preStep();

   /* Convert the step time from ms to bullet seconds. */
   btScalar stepInSeconds = timeStep / 1000.0f;

   /* Finally, do the physics step */
   dynamicsWorld->stepSimulation(stepInSeconds, maxSubSteps, 
                                 BULLET_FREQUENCY);

   /* Check ball field limits */
   if((ball) && (ball->getMovedFlag()) && (field))
   {
      checkBallFieldLimits();
   }

   debugDraw();
}

/***********************************************************************
 *                          tickCallBack                               *
 ***********************************************************************/
void BulletLink::tickCallBack()
{
   if(!rulesEnabled)
   {
      /* No need to check rules, if not to check */
      return;
   }

   int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
   for (int i=0;i<numManifolds;i++)
   {
      btPersistentManifold* contactManifold =  
         dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
      const btCollisionObject* obA = (contactManifold->getBody0());
      const btCollisionObject* obB = (contactManifold->getBody1());

      if( (obA->getCollisionShape()->getUserPointer() != NULL) &&
          (obB->getCollisionShape()->getUserPointer() != NULL) )
      {
         int numContacts = contactManifold->getNumContacts();
         for (int j=0;j<numContacts;j++)
         {
            btManifoldPoint& pt = contactManifold->getContactPoint(j);
            
            /* Retrieve object pointers */
            FieldObject* pA = (FieldObject*)
                  obA->getCollisionShape()->getUserPointer();
            FieldObject* pB = (FieldObject*)
               obB->getCollisionShape()->getUserPointer();
            
            bool someoneIsTheActorDisk = (pA == Rules::getCurrentDisk()) ||
                                         (pB == Rules::getCurrentDisk());

            /* Verify if someone is moving (not a static collision).
             * Note: must check current and last, as the movement is
             * made after this callback. Also, this is the reason
             * why actor collisions are always positive: to avoid missing
             * an initial collision.
             * Note: It's possible to miss an initial secundary collision,
             * but as it isn't relevant to the rules, no problem than. */
            if( (someoneIsTheActorDisk) ||
                (pA->getMovedFlag()) || (pA->getLastMovedFlag()) ||
                (pB->getMovedFlag()) || (pB->getLastMovedFlag()) )
               
            {
               bool newCollision =
                  ((pA->getLastCollision() >= BTSOCCER_MIN_NEW_COLLISION_TIME)||
                   (pB->getLastCollision() >= BTSOCCER_MIN_NEW_COLLISION_TIME));
               const btVector3& ptA = pt.getPositionWorldOnA();

               /* TeamPlayer collision with TeamPlayer */
               if( (pA->getType() == FieldObject::TYPE_DISK) &&
                   (pB->getType() == FieldObject::TYPE_DISK) )
               {
                  TeamPlayer* tpA = (TeamPlayer*)pA;
                  TeamPlayer* tpB = (TeamPlayer*)pB;
                  /* Tell Rules:: */
                  Rules::diskCollideDisk(tpA->getTeam(), tpB->getTeam(),
                           ptA.getX() * BULLET_TO_OGRE_FACTOR, 
                           ptA.getZ() * BULLET_TO_OGRE_FACTOR);
                  if(newCollision)
                  {
                     Kosound::Sound::addSoundEffect(
                           ptA.getX() * BULLET_TO_OGRE_FACTOR, 0, 
                           ptA.getZ() * BULLET_TO_OGRE_FACTOR,
                           SOUND_NO_LOOP, BTSOCCER_SOUND_DISK_COLLISION,
                           new Kobold::OgreFileReader());
                     /* Queue message, if online game */
                     if(onlineGame)
                     {
                        protocol.queueSoundEffect(SOUND_TYPE_COLLISION,
                           Ogre::Vector3(ptA.getX(), 0, ptA.getZ()) * 
                           BULLET_TO_OGRE_FACTOR);
                     }
                  }
                  
               }
               /* Team Player collision with goalkeeper */
               else if( ((pA->getType() == FieldObject::TYPE_GOAL_KEEPER) ||
                         (pA->getType() == FieldObject::TYPE_DISK) ) &&
                        ((pB->getType() == FieldObject::TYPE_GOAL_KEEPER) ||
                         (pB->getType() == FieldObject::TYPE_DISK) ) )
               {
                  GoalKeeper* gk = (GoalKeeper*)
                     (((pA->getType()==FieldObject::TYPE_GOAL_KEEPER))?pA:pB);
                  TeamPlayer* tp = (TeamPlayer*)
                     (((pA->getType()==FieldObject::TYPE_GOAL_KEEPER))?pB:pA);
                  /* Set position of collision with goal keeper to "meia-lua"
                   * area entering. */
                  Ogre::Real xPos;
                  if(gk->getTeam() == Rules::getUpperTeam())
                  {
                     xPos = Rules::getField()->getPenaltyAreaDelta()[0];
                  }
                  else
                  {
                     xPos = -Rules::getField()->getPenaltyAreaDelta()[0];
                  }
                  Rules::diskCollideDisk(gk->getTeam(), tp->getTeam(),
                                        xPos, 0.0f);
                  if(newCollision)
                  {
                     Kosound::Sound::addSoundEffect(
                                ptA.getX() * BULLET_TO_OGRE_FACTOR, 0, 
                                ptA.getZ() * BULLET_TO_OGRE_FACTOR,
                                SOUND_NO_LOOP, BTSOCCER_SOUND_DISK_COLLISION,
                                new Kobold::OgreFileReader());
                     /* Queue message, if online game */
                     if(onlineGame)
                     {
                        protocol.queueSoundEffect(SOUND_TYPE_COLLISION,
                           Ogre::Vector3(ptA.getX(), 0, ptA.getZ()) * 
                           BULLET_TO_OGRE_FACTOR);
                     }
                  }
               }
               /* Ball collision with TeamPlayer */
               else if( (pA->getType() == FieldObject::TYPE_BALL) ||
                   (pB->getType() == FieldObject::TYPE_BALL) )
               {
                  TeamPlayer* disk = NULL;
                  if( (pA->getType() == FieldObject::TYPE_DISK) ||
                      (pA->getType() == FieldObject::TYPE_GOAL_KEEPER) )
                  {
                     disk = (TeamPlayer*)pA;
                  }
                  else if( (pB->getType() == FieldObject::TYPE_DISK) ||
                           (pB->getType() == FieldObject::TYPE_GOAL_KEEPER) )
                  {
                     disk = (TeamPlayer*)pB;
                  }
                  if(disk != NULL)
                  {
                     Rules::ballCollideDisk(disk->getTeam());
                  }
               }
            }

            pA->setLastCollision();
            pB->setLastCollision();
         }
      }
   }

   /* Verify if ball entered goal: to enter it must pass fully the line,
    * so must do radius correction to the position. */
   if((ball != NULL) && (field != NULL))
   {
      Ogre::Vector3 ballPosUp = ball->getPosition();
      ballPosUp.x -= ball->getSphereRadius();
      Ogre::Vector3 ballPosDown = ball->getPosition();
      ballPosDown.x += ball->getSphereRadius();
      if(field->getUpGoalBox().contains(ballPosUp))
      {
         Rules::ballEnterGoal(true);
      }
      else if(field->getDownGoalBox().contains(ballPosDown))
      {
         Rules::ballEnterGoal(false);
      }
   }

}

/***********************************************************************
 *                          isWorldStable                              *
 ***********************************************************************/
bool BulletLink::isWorldStable()
{
   if( ( (teamA) && (teamA->movedOnLastPhysicStep()) ) ||
       ( (teamB) && (teamB->movedOnLastPhysicStep()) ) ||
       (ball->getMovedFlag()) )
   {
      /* Someone moved: not stable yet. */
      return false;
   }

   return true;
}

/***********************************************************************
 *                       queueUpdatesToProtocol                        *
 ***********************************************************************/
void BulletLink::queueUpdatesToProtocol(bool sendAll)
{
   if((sendAll) || (ball->getMovedFlag()))
   {
      protocol.queueBallUpdateToSend(ball->getPosition(), 
            ball->getOrientation(), sendAll);
   }
   if(teamA != NULL)
   {
      teamA->queueUpdatesToSend(true, sendAll);
   }
   if(teamB != NULL)
   {
      teamB->queueUpdatesToSend(false, sendAll);
   }
}

/***********************************************************************
 *                            debugDraw                                *
 ***********************************************************************/
void BulletLink::debugDraw()
{
   if(bulletDebugDraw != NULL)
   {
      dynamicsWorld->debugDrawWorld();
      bulletDebugDraw->update();
   }
}

/***********************************************************************
 *                          setDebugDraw                               *
 ***********************************************************************/
void BulletLink::setDebugDraw(BulletDebugDraw* debugDraw)
{
   dynamicsWorld->setDebugDrawer(debugDraw);
   bulletDebugDraw = debugDraw;
}

/*****************************************************************
 *                      checkBallFieldLimits                     *
 *****************************************************************/
void BulletLink::checkBallFieldLimits()
{
   /* Must check if full ball went over limits */
   float ballRadius = ball->getSphere().getRadius();
   Ogre::Vector3 pos = ball->getPosition();
   Ogre::Vector2 halfSize = field->getHalfSize();
   Ogre::Vector2 sideDelta = field->getSideDelta();

   /* cheking if ball has exited through the field byline */
   if(pos.x >= halfSize[0] - sideDelta[0] + ballRadius)
   {
      Rules::ballExitAtByline(true, pos.z);
   }
   else if(pos.x <= -halfSize[0] + sideDelta[0] - ballRadius)
   {
      Rules::ballExitAtByline(false, pos.z);
   }

   /* checking if ball has exited through the field sides */
   if(pos.z >= halfSize[1] - sideDelta[1] + ballRadius)
   {
      Rules::ballExitAtSide(pos.x, halfSize[1] - sideDelta[1]);
   }
   else if(pos.z <= -halfSize[1]  + sideDelta[1] - ballRadius)
   {
      Rules::ballExitAtSide(pos.x, -halfSize[1]+sideDelta[1]);
   }   
}


/***********************************************************************
 *                           static fields                             *
 ***********************************************************************/
btBroadphaseInterface* BulletLink::broadPhase = NULL; 
btDefaultCollisionConfiguration* BulletLink::collisionConfiguration = NULL; 
btCollisionDispatcher* BulletLink::dispatcher = NULL;
btSequentialImpulseConstraintSolver* BulletLink::solver = NULL;
btDiscreteDynamicsWorld* BulletLink::dynamicsWorld = NULL;
BulletDebugDraw* BulletLink::bulletDebugDraw = NULL;
Team* BulletLink::teamA = NULL;
Team* BulletLink::teamB = NULL;
Ogre::Real BulletLink::diskDiameter = 0.0f;
FieldObject* BulletLink::ball = NULL;
Field* BulletLink::field = NULL;
bool BulletLink::rulesEnabled = true;
bool BulletLink::onlineGame = false;
Protocol BulletLink::protocol;
