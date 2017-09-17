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

#include <OGRE/OgreMaterialManager.h>
#include "teamplayer.h"
#include "ball.h"
#include "field.h"

using namespace BtSoccer;

/******************************************************************
 *                           Constructor                          *
 ******************************************************************/
TeamPlayer::TeamPlayer(int type, Ogre::String name, Ogre::String fileName, 
      Ogre::SceneManager* ogreSceneManager, BulletDebugDraw* debugDraw)
      :FieldObject(type, name, fileName, ogreSceneManager, 
            TEAM_PLAYER_SCALE_FACTOR,
            (type == TYPE_DISK)?DISK_MASS:GKEEPER_MASS,
            DISK_RESTITUTION, DISK_FRICTION, DISK_ROLLING_FRICTION, debugDraw)
{
   init();
}

/******************************************************************
 *                           Constructor                          *
 ******************************************************************/
TeamPlayer::TeamPlayer(int type, Ogre::String name)
           :FieldObject(type, name, "", NULL, 
                        TEAM_PLAYER_SCALE_FACTOR,
                        (type == TYPE_DISK)?DISK_MASS:GKEEPER_MASS,
                        DISK_RESTITUTION, DISK_FRICTION, DISK_ROLLING_FRICTION,
                        NULL)
{
   init();
}

/******************************************************************
 *                               init                             *
 ******************************************************************/
void TeamPlayer::init()
{
   lastBallCollision.reset();
   positionDone = false;
   materialName = "";
   isOpaque = true;
   /* Avoid collision with AI physic elements. */
   rigidBody->getBroadphaseProxy()->m_collisionFilterMask =
      rigidBody->getBroadphaseProxy()->m_collisionFilterMask ^ AI_FILTER_GROUP;
   setDamping(0.0f, DISK_ANGULAR_DAMPING);
}

/******************************************************************
 *                            Destructor                          *
 ******************************************************************/
TeamPlayer::~TeamPlayer()
{
   /* TODO! */
}

/******************************************************************
 *                             setTeam                            *
 ******************************************************************/
void TeamPlayer::setTeam(BtSoccer::Team* t)
{
   curTeam = t;
}

/******************************************************************
 *                             getTeam                            *
 ******************************************************************/
BtSoccer::Team* TeamPlayer::getTeam()
{
   return(curTeam);
}

/******************************************************************
 *                           setMaterial                          *
 ******************************************************************/
void TeamPlayer::setMaterial(Ogre::String materialName)
{
   this->materialName = materialName;
   model->setMaterialName(materialName);
}

/******************************************************************
 *                     getLastBallCollision                       *
 ******************************************************************/
unsigned long TeamPlayer::getLastBallCollision()
{
   return(lastBallCollision.getMilliseconds());
}

/******************************************************************
 *                     setLastBallCollision                       *
 ******************************************************************/
void TeamPlayer::setLastBallCollision()
{
   lastBallCollision.reset();
}
/******************************************************************
 *                         positionInput                          *
 ******************************************************************/
void TeamPlayer::setPositionWithBall(float posX, float posZ, Ball* gameBall,
                                     Field* field)
{
   float diskRadius = sphere.getRadius();
   float ballRadius = gameBall->getSphere().getRadius();
   
   /* calculate mouse distance to the ball */
   Ogre::Vector3 ballPos = gameBall->getPosition();
   Ogre::Vector2 vec;
   vec.x = posX - ballPos.x;
   vec.y = posZ - ballPos.z;
   float dist = Ogre::Math::Sqrt( (vec.x*vec.x) + (vec.y*vec.y));
   vec.normalise();

   /* Default disk distances to ball */
   float maxDist = 2.0f*diskRadius;
   float minDist = ballRadius+diskRadius;

   /* Check if maxDist need to be smaller (on some fields the disk,
    * when on "throw-ins" or "corner-kicks", must not be so far
    * to avoid entering field's border). */
   Ogre::Vector2 halfSize = field->getHalfSize();
   Ogre::Real sideDelta = field->getSideDelta()[1];

   if( (ballPos.x - maxDist <= -halfSize[0] + diskRadius) ||
       (ballPos.x + maxDist >= halfSize[0] - diskRadius) ||
       (ballPos.z - maxDist <= -halfSize[1] + diskRadius) ||
       (ballPos.z + maxDist >= halfSize[1] - diskRadius))
   {
      maxDist = sideDelta - diskRadius;
   }
   
   /* Verify if not too far away from the ball */
   if(dist > maxDist)
   {
      posX = ballPos.x + maxDist*vec.x;
      posZ = ballPos.z + maxDist*vec.y;
   }
   /* Or not too near */
   else if(dist < minDist)
   {
      posX = ballPos.x + minDist*vec.x;
      posZ = ballPos.z + minDist*vec.y;
   }
   
   /* define the new position */
   setPosition(posX, 0.0f, posZ);
}

/******************************************************************
 *                         positionInput                          *
 ******************************************************************/
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS ||\
    OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
bool TeamPlayer::positionInput(Ogre::Vector3 mouseField, bool leftButtonPressed,
                   Ball* gameBall, Field* field)
{
   /* Set new position */
   setPositionWithBall(mouseField.x, mouseField.z, gameBall, field);
   
   if(leftButtonPressed)
   {
      /* Defined the position, must wait until mouse press end */
      positionDone = true;
   }
   
   if( (positionDone) && (!leftButtonPressed) )
   {
      /* Finish position input */
      positionDone = false;
      return(true);
   }
   
   
   return(false);
}
#else
/******************************************************************
 *                         positionInput                          *
 ******************************************************************/
bool TeamPlayer::positionInput(Ogre::Vector3 mouseField,
      bool leftButtonPressed, Ball* gameBall, Field* field)
{
   if( (positionDone) && (!leftButtonPressed) )
   {
      /* Finish position input */
      positionDone = false;
      return(true);
   }

   if(!positionDone)
   {
      /* Set new position */
      setPositionWithBall(mouseField.x, mouseField.z, gameBall, field);
      
      if(leftButtonPressed)
      {
         /* Defined the position, must wait until mouse press end */
         positionDone = true;
      }
   }
   return(false);
}
#endif

/*********************************************************************
 *                          setTransparent                           *
 *********************************************************************/
void TeamPlayer::setTransparent()
{
   if(isOpaque)
   {
      Ogre::MaterialManager::getSingleton().getByName(materialName)->setDiffuse(
            0.8f, 0.8f, 0.8f, 0.4f);
      isOpaque = false;
   }
}

/*********************************************************************
 *                           setOpaque                               *
 *********************************************************************/
void TeamPlayer::setOpaque()
{
   if(!isOpaque)
   {
      Ogre::MaterialManager::getSingleton().getByName(materialName)->setDiffuse(
            0.8f, 0.8f, 0.8f, 1.0f);
      isOpaque = true;
   }
}

