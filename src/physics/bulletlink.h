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


#ifndef _btsoccer_bulletlink_h
#define _btsoccer_bulletlink_h

#include <btBulletDynamicsCommon.h>
#include "../debug/bulletdebugdraw.h"
#include "../net/protocol.h"
#include "../btsoccer.h"

namespace BtSoccer
{
   /*! Class that make the link with bullet, initing it,
    * setting the world, etc. */
   class BulletLink
   {
      public:
         /*! Create the bullet world to use */
         static void createBulletWorld();

         /*! Delete the created bulled world */
         static void deleteBulletWorld();

         /*! Set pointers used by the bullet link 
          * \param tA -> pointer to team A 
          * \param tB -> pointer to team B
          * \param b -> pointer to ball
          * \param f -> pointer to field 
          * \param online -> true if online game */
         static void setPointers(Team* tA, Team* tB, FieldObject* b, 
               Field* f, bool online);

         /*! The callback for bullet tick */
         static void tickCallBack();

         /*! Add rigid body to the world
          * \param rigidBody -> pointer to the rigid body to add */
         static void addRigidBody(btRigidBody* rigidBody);
         
         /*! Remove rigid body from the world
          * \param rigidBody -> pointer to the rigid body to remove */
         static void removeRigidBody(btRigidBody* rigidBody);

         /*! Define the debug drawer for bullet
          * \param debugDraw pointer to the debug drawer used. */
         static void setDebugDraw(BulletDebugDraw* debugDraw);

         /*! Do the draw of current physics for debug. */
         static void debugDraw();

         /*! Do the preStep, clearing flags.
          * \note step(timeStep, maxSub) will already call this function. */
         static void preStep();

         /*! Step the rigid body
          * \param timeStep -> current time of this step (in ms).
          * \param maxSubSteps -> numer of bullet sub steps */
         static void step(btScalar timeStep, int maxSubSteps);

         /*! Do a step just to stabilize physics after a position set. */
         static void forcedStep(btScalar timeStep=0.1f, int maxSubSteps=5);

         /*! Check if the world system is stable (ie: with no
          * velocities and forces; aka: static!) */
         static bool isWorldStable();

         /*! Queue at the protocol - to send - all updates to teamPlayers
          * and ball. 
          * \param sendAll -> if true will send all positions, not just
          *                   the ones that changed.*/
         static void queueUpdatesToProtocol(bool sendAll=false);

      protected:

         /*! Check if ball is inner the field and tell rules otherwise. */
         static void checkBallFieldLimits();

      private:
         BulletLink(){};

         static btBroadphaseInterface* broadPhase; 
         static btDefaultCollisionConfiguration* collisionConfiguration;
         static btCollisionDispatcher* dispatcher;
         static btSequentialImpulseConstraintSolver* solver;
         static btDiscreteDynamicsWorld* dynamicsWorld;
         static BulletDebugDraw* bulletDebugDraw;
         static Team* teamA;
         static Team* teamB;
         static Ogre::Real diskDiameter;
         static FieldObject* ball;
         static Field* field;
         static bool rulesEnabled;
         static bool onlineGame;
         static Protocol protocol;
   };

}

#endif

