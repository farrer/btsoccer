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


#include "dummyai.h"
#include "../engine/ball.h"
#include "../engine/rules.h"
#include "../engine/field.h"

namespace BtSoccer
{

/***********************************************************************
 *                              Constructor                            *
 ***********************************************************************/
DummyAI::DummyAI(BtSoccer::Team* t, Field* f):BaseAI(t, f)
{
}

/***********************************************************************
 *                               Destructor                            *
 ***********************************************************************/
DummyAI::~DummyAI()
{
}

/***********************************************************************
 *                            calculateStep                            *
 ***********************************************************************/
void DummyAI::calculateStep()
{
   BtSoccer::Ball* ball = Rules::getBall();
   Ogre::Vector3 ballPos = ball->getPosition();
   Ogre::Vector3 diskPos;

   /* TODO: check if ball inner own area, in case we should do a direct
    * ball input */

   /* Get the nearest able to act disk with ball ahead */
   curTeamPlayer = getNearestBallDisk(true);
   if(!curTeamPlayer)
   {
      /* No disk with ball ahead it, must get nearest */
      curTeamPlayer = getNearestBallDisk(false);
   }

   /* Now, calculate things */
   if(curTeamPlayer)
   {
      diskPos = curTeamPlayer->getPosition();

      float dist = Ogre::Math::Sqrt(Ogre::Math::Sqr(diskPos.x - ballPos.x) +
            Ogre::Math::Sqr(diskPos.z - ballPos.z) );
      float length = getLengthToSendDiskToDistance(dist);

      /* Set vector from the disk to the ball. Dummy, isn't? */
      Ogre::Vector2 direction(diskPos.x - ballPos.x, diskPos.z - ballPos.z);
      direction.normalise();
      initialForce[0] = diskPos.x;
      initialForce[1] = diskPos.z;
      finalForce[0] = initialForce[0] + direction[0] * length;
      finalForce[1] = initialForce[1] + direction[1] * length;
   }
}

/***********************************************************************
 *                        calculateGoalShoot                           *
 ***********************************************************************/
void DummyAI::calculateGoalShoot()
{
   /* Should recalculate the shoot after the goal keeper is set.
    * But this is the DUMMY AI: just do nothing and use
    * previous values. */
}

/***********************************************************************
 *                            doDiskPositon                            *
 ***********************************************************************/
void DummyAI::doDiskPosition(BtSoccer::TeamPlayer* tp)
{
   Ogre::Vector2 tgtGoal; /**< Target goal position */
   Ogre::Vector2 dir; /**< Calculated disk position */
   
   BtSoccer::Ball* ball = Rules::getBall();
   Ogre::Vector3 ballPos = ball->getPosition();
   Ogre::Real fieldX = Rules::getField()->getHalfSize().x;

   /* Calculate disk to ball minimun distance */
   float ballDist = ball->getSphere().getRadius() + 
                    tp->getSphere().getRadius() + 4.0f;

   /* Point the disk to the goal */
   if(curTeam == Rules::getUpperTeam())
   {
      /* Will attack at bottom */
      tgtGoal[0] = -fieldX;
      tgtGoal[1] = 0.0f;
   }
   else
   {
      /* Will attack at upper */
      tgtGoal[0] = fieldX;
      tgtGoal[1] = 0.0f;
   }

   /* Calculate direction */
   dir[0] = ballPos.x - tgtGoal[0];
   dir[1] = ballPos.z - tgtGoal[1];
   dir.normalise();

   /* Set its position */
   tp->setPosition(ballPos.x+dir[0]*ballDist, 0.0f,
                   ballPos.z+dir[1]*ballDist);

   /* Set shoot force */
   float forceValue;
   /* Calculate force value, depending on type */
   switch(Rules::getState())
   {
      case Rules::STATE_GOAL_KICK:
         forceValue = 130.0f;
         break;
      default:
         forceValue = 40.0f;
         break;
   }

   /* Set force direction */
   initialForce[0] = tp->getPosition().x;
   initialForce[1] = tp->getPosition().z;
   finalForce = initialForce+dir*forceValue;
   curTeamPlayer = tp;
}

/***********************************************************************
 *                         doGoalKeeperPositon                         *
 ***********************************************************************/
void DummyAI::doGoalKeeperPosition(BtSoccer::GoalKeeper* gk)
{
   /* Calculate direction from the ball to the shooter */

   /* Set the goal keeper there */
}

}

