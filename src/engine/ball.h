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

#ifndef _btsoccer_ball_h
#define _btsoccer_ball_h

#include "fobject.h"
#include "team.h"

#define BALL_NAME    "gameBall"

namespace BtSoccer
{

/*! The ball is the game ball implementation with
 * positions and orientations. */
class Ball: public FieldObject
{
   public:
 
      enum 
      {
         BALL_AHEAD,
         BALL_BEHIND,
         BALL_AT_SIDE
      };

      /* Constructor with a visual object with it
       * \param ogreSceneManager scene manager used */
      Ball(Ogre::SceneManager* ogreSceneManager, BulletDebugDraw* debugDraw);

      /* Constructor without any visual object with it.
       * Usefull for AI physics internal calculations */
      Ball();

      /*! Destructor */
      ~Ball();

      /*! Calculate collison position to send ball to target
       * \param disk -> disk that will touch the ball
       * \param target -> position to send ball to
       * \param colPoint -> will receive the collision point
       * \param colDisk -> will receive the disk position when collided
       * \param dir -> will receive the ball direction */
      void calculateCollisionPos(TeamPlayer* disk, Ogre::Vector2 target,
          Ogre::Vector2& colPoint, Ogre::Vector2& colDisk, 
          Ogre::Vector2& dir);


      /*! Get relative position of the ball to a disk.
       * \param diskPos diskPosition at field plane.
       * \param teamUpper if disk is of a upper or down team.
       * \return ball position enum constant 
       * (BALL_AHEAD, BALL_BEHIND, BALL_AT_SIDE). */
      int getRelativePositionToDisk(Ogre::Vector2 diskPos, bool teamUpper);


   protected:
      /*! Set some ball initial default values */
      void setInitialValues();
        
};

};

#endif

