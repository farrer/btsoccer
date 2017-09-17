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

#ifndef _btsoccer_test_base_ai_h_
#define _btsoccer_test_base_ai_h_

#include "testcase.h"

#include "../ai/baseai.h"

namespace BtSoccerTests
{

/*! A Base AI friend to the test */
class BaseAITest : public BtSoccer::BaseAI
{
   public:
      friend class BaseAITestCase;

      BaseAITest(BtSoccer::Team* t, BtSoccer::Field* f);
      ~BaseAITest();

      void doDiskPosition(BtSoccer::TeamPlayer* tp);
      void doGoalKeeperPosition(BtSoccer::GoalKeeper* gk);
      void calculateGoalShoot();

   protected:

      void calculateStep();
};

/*! A test case implementation for the BaseAI class */
class BaseAITestCase : public TestCase 
{
   public:
      BaseAITestCase();
      ~BaseAITestCase();
   protected:
      void doSpecificScenarioCreation();

      void doSpecificScenarioFinish();

      void doRun();

      /*! Test the function BaseAI::getNearestBallDisk */
      void testGetNearestBallDisk();
      
      /*! Test the function #calculateForce(TeamPlayer*, Ball*, 
                                            Ogre::Vector3, bool) */
      void testCalculateForceBall();

      /*! Test the function #calculateForce(TeamPlayer*,Vector3) */
      void testCalculateForceDisk();

      /*! Inner check for #testCalculateForceDisk() */
      void checkForceDiskPosition(BtSoccer::TeamPlayer* disk,
              Ogre::Real origX, Ogre::Real origZ, Ogre::Real x, Ogre::Real z);

      /*! Wait for a disk stop to move - ie: to be physical stable */
      void waitForDiskStable(BtSoccer::TeamPlayer* disk);

      /*! Inner test for #textCalculatedFocerBall() */
      void checkForceBallPosition(BtSoccer::TeamPlayer* disk, 
            BtSoccer::Ball* ball, Ogre::Real diskX, Ogre::Real diskZ,
            Ogre::Real ballX, Ogre::Real ballZ, 
            Ogre::Real ballTgtX, Ogre::Real ballTgtZ); 

      /*! Wait for ball and disk became stable */
      void waitForDiskAndBallStable(BtSoccer::TeamPlayer* disk, 
            BtSoccer::Ball* ball);

      BaseAITest* aiTeamA;
      BaseAITest* aiTeamB;
};


}

#endif

