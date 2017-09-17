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

#include "ball.h"
#include "teamplayer.h"
#include "../physics/ogremotionstate.h"
using namespace BtSoccer;

/******************************************************************
 *                           Constructor                          *
 ******************************************************************/
Ball::Ball()
     :FieldObject(FieldObject::TYPE_BALL, 
      BALL_NAME, "", NULL, BALL_SCALE_FACTOR,
      BALL_MASS, BALL_RESTITUTION, BALL_FRICTION, BALL_ROLLING_FRICTION,
      NULL)
{
   setInitialValues();
}

/******************************************************************
 *                           Constructor                          *
 ******************************************************************/
Ball::Ball(Ogre::SceneManager* ogreSceneManager, BulletDebugDraw* debugDraw)
     :FieldObject(FieldObject::TYPE_BALL, 
      BALL_NAME, "ball/ball.mesh", ogreSceneManager, BALL_SCALE_FACTOR,
      BALL_MASS, BALL_RESTITUTION, BALL_FRICTION, BALL_ROLLING_FRICTION,
      debugDraw)
{
   setInitialValues();
}

/******************************************************************
 *                           setInitialValues                     *
 ******************************************************************/
void Ball::setInitialValues()
{
   setPosition(0, floorPosition, 0);
   setDamping(0.0f, BALL_ANGULAR_DAMPING);

   /* Avoid collision with AI physic elements. */
   rigidBody->getBroadphaseProxy()->m_collisionFilterMask =
      rigidBody->getBroadphaseProxy()->m_collisionFilterMask ^ AI_FILTER_GROUP;
}


/******************************************************************
 *                            Destructor                          *
 ******************************************************************/
Ball::~Ball()
{
}

/***********************************************************************
 *                         calculateCollisionPos                       *
 ***********************************************************************/
void Ball::calculateCollisionPos(TeamPlayer* disk,
      Ogre::Vector2 target, Ogre::Vector2& colPoint, Ogre::Vector2& colDisk,
      Ogre::Vector2& dir)
{
   Ogre::Vector2 ballPos(getPosition().x, getPosition().z);
   Ogre::Vector2 diskPos(disk->getPosition().x, disk->getPosition().z);

   /* Get ball to target inverted vector */
   Ogre::Vector2 ballTarget(ballPos - target);
   ballTarget.normalise();

   /* Caculate the disk collision point on ball (to send the ball to target) */
   colPoint = (ballTarget * getSphereRadius()) + ballPos;

   /* Calculate inverted vector of disk center -> ball collision point */
   Ogre::Vector2 tpDir(diskPos - colPoint);
   tpDir.normalise();

   /* Calculate target disk position when touching ball */
   colDisk = (tpDir * disk->getSphereRadius()) + colPoint;

   dir = target - ballPos;
   dir.normalise();
}

/***********************************************************************
 *                       getRelativePositionToDisk                     *
 ***********************************************************************/
int Ball::getRelativePositionToDisk(Ogre::Vector2 diskPos, bool teamUpper)
{
   /* First, set diskPos as origin and translate ballPosition to it */
   Ogre::Vector2 ballPos(getPosition().x, getPosition().z);
   Ogre::Vector2 relativeBallPos = ballPos - diskPos;

   /* Lets calculate the angle between the ball position at disk
    * coordinate system and its X axys */
   Ogre::Vector2 originX(1, 0);
   float angle = relativeBallPos.angleTo(originX).valueDegrees();

   /* Now, check if is ahead, behind or at side */
   if( (angle >= 120) && (angle <= 240) )
   {
      return (teamUpper)?BALL_AHEAD:BALL_BEHIND;
   } else if( ((angle >= 0) && (angle <= 60)) ||
              ((angle >= 300) && (angle <= 360)) )
   {
      return (teamUpper)?BALL_BEHIND:BALL_AHEAD;
   }

   /* Not ahead or behind, must be at side. */
   return BALL_AT_SIDE;
}

