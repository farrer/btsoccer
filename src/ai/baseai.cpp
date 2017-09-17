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

#include <OGRE/OgreLogManager.h>

#include "baseai.h"
#include "../engine/ball.h"
#include "../engine/field.h"
#include "../engine/teamplayer.h"
#include "../engine/goalkeeper.h"
#include "../engine/rules.h"

namespace BtSoccer
{

/***********************************************************************
 *                              Constructor                            *
 ***********************************************************************/
BaseAI::BaseAI(BtSoccer::Team* t, Field* f)
{
   /* Define the team and set it as AI controlled */
   curTeam = t;
   t->setControlledByHuman(false);
   
   /* Clear AI variables */
   clear();
}

/***********************************************************************
 *                              Destructor                             *
 ***********************************************************************/
BaseAI::~BaseAI()
{
}

/***********************************************************************
 *                                Clear                                *
 ***********************************************************************/
void BaseAI::clear()
{
   curTeamPlayer = NULL;
   tryGoalShoot = false;
   actOnBall = false;
   initialForce = Ogre::Vector2(0.0f, 0.0f);
   finalForce = Ogre::Vector2(0.0f, 0.0f);
}

/***********************************************************************
 *                            selectAction                             *
 ***********************************************************************/
bool BaseAI::selectAction()
{
   calculateStep();
   return curTeamPlayer || actOnBall;
}

/***********************************************************************
 *                        clearSelectedAction                          *
 ***********************************************************************/
void BaseAI::clearSelectedAction()
{
   clear();
}

/***********************************************************************
 *                          getAngleBetweenPositions                   *
 ***********************************************************************/
float BaseAI::getAngleBetweenPositions(Ogre::Vector2 posA, Ogre::Vector2 posB) 
{
   Ogre::Vector2 relativePosB = posB - posA;
   Ogre::Vector2 originX(1, 0);

   return relativePosB.angleTo(originX).valueDegrees();
}

/***********************************************************************
 *                            getNearestBallDisk                       *
 ***********************************************************************/
BtSoccer::TeamPlayer* BaseAI::getNearestBallDisk(bool attackDirection) 
{
   int totalDisks = Rules::getField()->getNumberOfDisks();
   BtSoccer::Ball* ball = Rules::getBall();
   Ogre::Vector3 ballPos = ball->getPosition();
   BtSoccer::TeamPlayer* tp = NULL;
   BtSoccer::TeamPlayer* selected = NULL;
   float curDist, dist;
   Ogre::Vector3 diskPos;
   bool ballAhead = false;
   bool upperTeam = (curTeam == Rules::getUpperTeam());

   /* Get the nearest able to act disk */
   for(int i=0; i < totalDisks; i++)
   {
      tp = curTeam->getDisk(i);
      /* Verify if the player can act */
      if(Rules::getRemainingTouches(tp) > 0)
      {
         /* Calculate disk to ball distance */
         diskPos = tp->getPosition();
         dist = Ogre::Math::Sqrt( 
                   Ogre::Math::Sqr(diskPos.x - ballPos.x) +
                   Ogre::Math::Sqr(diskPos.z - ballPos.z) );

         if(attackDirection)
         {
            /* Verify if ball ahead */
            ballAhead = ball->getRelativePositionToDisk(
                  Ogre::Vector2(diskPos.x, diskPos.z),
                  upperTeam) == Ball::BALL_AHEAD;
         }
         
         if( (!selected) || (curDist > dist) )
         {
            if( (!attackDirection) || (ballAhead) )
            {
               selected = tp;
               curDist = dist;
            }
         }
      }
   }

   return selected;

}

/***********************************************************************
 *                  getForceToSendDiskToDistance                       *
 ***********************************************************************/  
float BaseAI::getLengthToSendDiskToDistance(float distance)
{
   return DistTable::getDiskInputVectorLength(distance);
}

/***********************************************************************
 *               getForceToSendBallToDistanceWithDisk                  *
 ***********************************************************************/  
float BaseAI::getLengthToSendBallToDistanceWithDisk(float distanceToBall,
      float ballDistance)
{
   return DistTable::getDiskInputVectorLength(distanceToBall, ballDistance);
}

/***********************************************************************
 *                          calculateForce                             *
 ***********************************************************************/
void BaseAI::calculateForce(TeamPlayer* tp, Ogre::Vector3 target)
{
   /* Calculate distance and needed force */
   Ogre::Vector3 diskPos = curTeamPlayer->getPosition();
   float dist = Ogre::Math::Sqrt(Ogre::Math::Sqr(diskPos.x - target.x) +
         Ogre::Math::Sqr(diskPos.z - target.z) );
   float length = getLengthToSendDiskToDistance(dist);

   /* Define direction and force vector */ 
   Ogre::Vector2 direction(diskPos.x - target.x, diskPos.z - target.z);
   direction.normalise();

   initialForce[0] = diskPos.x;
   initialForce[1] = diskPos.z;
   finalForce[0] = initialForce[0] + direction[0] * length;
   finalForce[1] = initialForce[1] + direction[1] * length;
}

/***********************************************************************
 *                          calculateForce                             *
 ***********************************************************************/
void BaseAI::calculateForce(TeamPlayer* tp, Ball* ball, 
         Ogre::Vector3 target, bool mustStop)
{
   ball->pointTarget(target);
   Ogre::Vector3 ballPos = ball->getPosition();
   Ogre::Vector3 tpPos = tp->getPosition();
   Ogre::Vector2 colPoint, direction, ballDir, diskPos;
     
   /* Get the desired angle of touch with the ball to send it to 
    * the target */
   ball->calculateCollisionPos(tp, Ogre::Vector2(target.x, target.z), 
         colPoint, diskPos, ballDir);
   //tp->pointTarget(Ogre::Vector3(diskPos[0], 0.0, diskPos[1]));
   direction = Ogre::Vector2(tpPos.x - diskPos[0], tpPos.z - diskPos[1]);
   direction.normalise();

   /* Calculate needed force */ 
   float dist = Ogre::Math::Sqrt(Ogre::Math::Sqr(tpPos.x - diskPos[0]) +
         Ogre::Math::Sqr(tpPos.z - diskPos[1]) );
   float ballDist = Ogre::Math::Sqrt(Ogre::Math::Sqr(target.x - ballPos.x) +
         Ogre::Math::Sqr(target.z - ballPos.z) );
   
   float ballDistDisk = (ball->getPosition() - tp->getPosition()).length() - 
      tp->getSphereRadius() - ball->getSphereRadius(); 
   bool ballTooNear = ballDistDisk < 3.0f;
   float length = getLengthToSendBallToDistanceWithDisk(dist, ballDist);
   length *= 1.14f;

   printf("toonear: %d ballDist: %.3f\n", ballTooNear, ballDistDisk);
   if(!mustStop)
   {
      /* Touch a bit harder */
      length *= 1.4f;
   }
   if(ballTooNear)
   {
      length *= 2.8f;
   }
   Ogre::LogManager::getSingleton().stream(Ogre::LML_NORMAL) 
      << "Length: " << length 
      << ", dir: (" << direction[0]
      << "," << direction[1] << ")";

   /* Set force vector. */
   initialForce[0] = tpPos.x;
   initialForce[1] = tpPos.z;
   finalForce[0] = initialForce[0] + direction[0] * length;
   finalForce[1] = initialForce[1] + direction[1] * length;
}



}

