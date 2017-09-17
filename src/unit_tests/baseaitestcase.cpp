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


#include "baseaitestcase.h"
using namespace BtSoccerTests;

#include "../engine/goalkeeper.h"
#include "../physics/forceio.h"
#include <OGRE/OgreLogManager.h>

#define MAX_DISK_ERROR 0.15f /**< 85% accuracy for target disk position */
#define MAX_BALL_ERROR 0.15f /**< 85% accuracy for target ball position */

/***********************************************************************
 *                             Constructor                             *
 ***********************************************************************/
BaseAITestCase::BaseAITestCase() : TestCase(false)
{
}

/***********************************************************************
 *                              Destructor                             *
 ***********************************************************************/
BaseAITestCase::~BaseAITestCase()
{
}

/***********************************************************************
 *                      doSpecificScenarioCreation                     *
 ***********************************************************************/
void BaseAITestCase::doSpecificScenarioCreation()
{
   aiTeamA = new BaseAITest(teamA, field);
   aiTeamB = new BaseAITest(teamB, field);
}

/***********************************************************************
 *                       doSpecificScenarioFinish                      *
 ***********************************************************************/
void BaseAITestCase::doSpecificScenarioFinish()
{
   delete aiTeamA;
   delete aiTeamB;
}

/***********************************************************************
 *                                   doRun                             *
 ***********************************************************************/
void BaseAITestCase::doRun()
{
   testGetNearestBallDisk();
   testCalculateForceDisk();
   testCalculateForceBall();
}

/***********************************************************************
 *                         testGetNearestBallDisk                      *
 ***********************************************************************/
void BaseAITestCase::testGetNearestBallDisk()
{
   Ogre::LogManager::getSingleton().getDefaultLog()->logMessage(
       "\ttestGetNearestBallDisk...");
   /* Define team position and ball: one disk ahead (and nearest),
    * other disk behind and near and all other disks behing. */
   teamA->getGoalKeeper()->setPosition(-100, 0, -100);
   teamB->getGoalKeeper()->setPosition(-100, 0, -100);
   for(int i = 0; i < TEAM_MAX_DISKS; i++)
   {
      if(i == 0)
      {
         teamA->getDisk(i)->setPosition(12, 0, 10);
         teamB->getDisk(i)->setPosition(16, 0, 10);
      }
      else if(i == 1)
      {
         teamA->getDisk(i)->setPosition(0, 0, 10);
         teamB->getDisk(i)->setPosition(8, 0, 10);
      }
      else
      {
         teamA->getDisk(i)->setPosition(-100, 0, -100);
         teamB->getDisk(i)->setPosition(-100, 0, -100);
      }
   }
   ball->setPosition(10, 0, 10);

   /* Test with ball ahead restricion */
   BtSoccer::TeamPlayer* disk = aiTeamA->getNearestBallDisk(true);
   assert(disk == teamA->getDisk(1)); 

   /* Test without restriction */
   disk = aiTeamA->getNearestBallDisk(false);
   assert(disk == teamA->getDisk(0)); 

   /* Let's check with teamB, same cases (but attacking down). */
   disk = aiTeamB->getNearestBallDisk(true);
   assert(disk == teamB->getDisk(0));
   disk = aiTeamB->getNearestBallDisk(false);
   assert(disk == teamB->getDisk(1));
}

/***********************************************************************
 *                         testCalculateForceDisk                      *
 ***********************************************************************/
void BaseAITestCase::testCalculateForceDisk()
{
   Ogre::LogManager::getSingleton().getDefaultLog()->logMessage(
         "\ttestCalculateForceDisk...");

   /* let's do it in negative and positive ways */
   for(int signal = -1; signal < 2; signal += 2)
   {
      /* Let's put everyone, except teamA's disk0 far away. */
      ball->setPosition(-150 * signal, 0, -150 * signal);
      teamA->getGoalKeeper()->setPosition(-100 * signal, 0, -100 * signal);
      teamB->getGoalKeeper()->setPosition(-200 * signal, 0, -200 * signal);
      int pos = -100 * signal;
      for(int i = 0; i < TEAM_MAX_DISKS; i++)
      {
         if(i != 0)
         {
            teamA->getDisk(i)->setPosition(pos + signal * (i * 10), 0, pos);
         }
         teamB->getDisk(i)->setPosition(pos + signal * (i * 10), 0, 2 * pos);
      }

      BtSoccer::TeamPlayer* disk = teamA->getDisk(0);
      waitForDiskStable(disk);

      for(int j = 4; j < 200; j += 1)
      {
         checkForceDiskPosition(disk, 0, 10, signal * (j), 10);
         checkForceDiskPosition(disk, 0, 10, signal * j, signal * j);
      }

   }
}

/***********************************************************************
 *                         checkForceDiskPosition                      *
 ***********************************************************************/
void BaseAITestCase::checkForceDiskPosition(BtSoccer::TeamPlayer* disk,
        Ogre::Real origX, Ogre::Real origZ, Ogre::Real x, Ogre::Real z)
{
   /* Calculate max error */
   Ogre::Real maxErrorX = fabs(MAX_DISK_ERROR * (x - origX));
   if(maxErrorX < 0.2f)
   {
      maxErrorX = 0.2f;
   }
   Ogre::Real maxErrorZ = fabs(MAX_DISK_ERROR * (z - origZ));
   if(maxErrorZ < 0.2f)
   {
      maxErrorZ = 0.2f;
   }

   /* Redefine disk position */
   disk->setPosition(origX, 0, origZ);

   /* Calculate the force*/
   aiTeamA->curTeamPlayer = disk;
   aiTeamA->calculateForce(disk, Ogre::Vector3(x, 0, z));

   /* Retrive it */
   BtSoccer::ForceInput force;
   Ogre::Real forceValue=0.0f, forceX=0.0f, forceZ=0.0f;
   force.setInitial(aiTeamA->getInitialForceX(), 
         aiTeamA->getInitialForceZ());
   force.setFinal(aiTeamA->getFinalForceX(), aiTeamA->getFinalForceZ());
   force.getForce(forceValue, forceX, forceZ, 0, BTSOCCER_MAX_FORCE_VALUE);

   /* Apply the force and wait until the disk stopped. */
   disk->applyForce(forceValue*forceX, 0, forceValue*forceZ);
   waitForDiskStable(disk);

   /* Check if difference is aceptable or not */
   Ogre::Real diffX = fabs(disk->getPosition().x - x);
   Ogre::Real diffZ = fabs(disk->getPosition().z - z);

   if((diffX > maxErrorX) || (diffZ > maxErrorZ))
   {
      Ogre::LogManager::getSingleton().getDefaultLog()->stream()
           << "\t\tUnacceptable diff for target: " << x << ", " << z
           << " Diff: " << diffX << ", " << diffZ << " (max: " 
           << maxErrorX << ", " << maxErrorZ << ")";
      //assert(false);
   }
}

/***********************************************************************
 *                            waitForDiskStable                        *
 ***********************************************************************/
void BaseAITestCase::waitForDiskStable(BtSoccer::TeamPlayer* disk)
{
   disk->prePhysicStep();
   BtSoccer::BulletLink::step(BTSOCCER_UPDATE_RATE, 10);
   while(disk->getMovedFlag())
   {
      disk->prePhysicStep();
      BtSoccer::BulletLink::step(BTSOCCER_UPDATE_RATE, 10);
   }
}

/***********************************************************************
 *                         testCalculateForceBall                      *
 ***********************************************************************/
void BaseAITestCase::testCalculateForceBall()
{
   Ogre::LogManager::getSingleton().getDefaultLog()->logMessage(
         "\ttestCalculateForceBall...");

   int signal = 1;

   /* Let's put everyone, except teamA's disk0 and ball far away. */
   teamA->getGoalKeeper()->setPosition(-100 * signal, 0, -100 * signal);
   teamB->getGoalKeeper()->setPosition(-200 * signal, 0, -200 * signal);
   int pos = -100 * signal;
   for(int i = 0; i < TEAM_MAX_DISKS; i++)
   {
      if(i != 0)
      {
         teamA->getDisk(i)->setPosition(pos + signal * (i * 10), 0, pos);
      }
      teamB->getDisk(i)->setPosition(pos + signal * (i * 10), 0, 2 * pos);
   }

   BtSoccer::TeamPlayer* disk = teamA->getDisk(0);
   
   for(int j = 4; j < 200; j += 1)
   {
      checkForceBallPosition(disk, ball, (-signal * 30), 0, 0, 0, 
            (signal * j), 0);
   }
}

/***********************************************************************
 *                         checkForceBallPosition                      *
 ***********************************************************************/
void BaseAITestCase::checkForceBallPosition(BtSoccer::TeamPlayer* disk, 
      BtSoccer::Ball* ball, Ogre::Real diskX, Ogre::Real diskZ,
      Ogre::Real ballX, Ogre::Real ballZ, 
      Ogre::Real ballTgtX, Ogre::Real ballTgtZ) 
{
   /* Calculate max error */
   Ogre::Real maxErrorX = fabs(MAX_BALL_ERROR * (ballTgtX - ballX));
   if(maxErrorX < 2.0f)
   {
      maxErrorX = 2.0f;
   }
   Ogre::Real maxErrorZ = fabs(MAX_BALL_ERROR * (ballTgtZ - ballZ));
   if(maxErrorZ < 2.0f)
   {
      maxErrorZ = 2.0f;
   }

   /* Redefine disk and ball positions */
   disk->setPosition(diskX, 0.0f, diskZ);
   ball->setPosition(ballX, 0.0f, ballZ);
   waitForDiskAndBallStable(disk, ball);

   /* Calculate the force*/
   aiTeamA->curTeamPlayer = disk;
   aiTeamA->calculateForce(disk, ball, Ogre::Vector3(ballTgtX, 0, ballTgtZ), 
         true);

   /* Retrieve it */
   BtSoccer::ForceInput force;
   Ogre::Real forceValue=0.0f, forceX=0.0f, forceZ=0.0f;
   force.setInitial(aiTeamA->getInitialForceX(), 
         aiTeamA->getInitialForceZ());
   force.setFinal(aiTeamA->getFinalForceX(), aiTeamA->getFinalForceZ());
   force.getForce(forceValue, forceX, forceZ);

   /* Apply the force and wait until the disk and ball stopped. */
   disk->applyForce(forceValue*forceX, 0, forceValue*forceZ);
   waitForDiskAndBallStable(disk, ball);

   /* Check if the difference is acceptable or not */
   Ogre::Real diffX = fabs(ball->getPosition().x - ballTgtX);
   Ogre::Real diffZ = fabs(ball->getPosition().z - ballTgtZ);

   /*Ogre::LogManager::getSingleton().getDefaultLog()->stream()
      << "tgt: " << ballTgtX << ", " << ballTgtZ
      << " ballPos: " << ball->getPosition().x << ", " 
      << ball->getPosition().z;*/
   if((diffX > maxErrorX) || (diffZ > maxErrorZ))
   {
      Ogre::LogManager::getSingleton().getDefaultLog()->stream()
           << "\t\tUnacceptable diff for ball target: " 
           << ballTgtX << ", " << ballTgtZ
           << "\n\t\t\tOrig: " << ballX << ", " << ballZ
           << "\n\t\t\tGot: " << ball->getPosition().x << ", "
                              << ball->getPosition().z
           << "\n\t\t\tDiff: " << diffX << ", " << diffZ << " (max: " 
           << maxErrorX << ", " << maxErrorZ << ")"
           << "\n\t\t\t" << (ballTgtX / ball->getPosition().x);
      //assert(false);
   }

}

/***********************************************************************
 *                         waitForDiskAndBallStable                    *
 ***********************************************************************/
void BaseAITestCase::waitForDiskAndBallStable(BtSoccer::TeamPlayer* disk,
      BtSoccer::Ball* ball)
{
   disk->prePhysicStep();
   ball->prePhysicStep();
   BtSoccer::BulletLink::step(BTSOCCER_UPDATE_RATE, 10);
   while((disk->getMovedFlag()) || (ball->getMovedFlag()))
   {
      disk->prePhysicStep();
      ball->prePhysicStep();
      BtSoccer::BulletLink::step(BTSOCCER_UPDATE_RATE, 10);
   }
   /* Wait a bit more. */
   for(int i = 0; i < 30; i++)
   {
      disk->prePhysicStep();
      ball->prePhysicStep();
      BtSoccer::BulletLink::step(BTSOCCER_UPDATE_RATE, 10);
   }
}

/* The unused implementations of the test AI */
BaseAITest::BaseAITest(BtSoccer::Team* t, BtSoccer::Field* f)
           :BtSoccer::BaseAI(t, f)
{
}
BaseAITest::~BaseAITest()
{
}
void BaseAITest::doDiskPosition(BtSoccer::TeamPlayer* tp)
{
}
void BaseAITest::doGoalKeeperPosition(BtSoccer::GoalKeeper* gk)
{
}
void BaseAITest::calculateGoalShoot()
{
}
void BaseAITest::calculateStep()
{
}

