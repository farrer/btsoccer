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

#ifndef _btsoccer_goal_keeper_h
#define _btsoccer_goal_keeper_h

#include <OGRE/OgreEntity.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreTimer.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS ||\
    OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
   #include <kobold/multitouchcontroller.h>
#else
   #include <kobold/keyboard.h>
#endif

#include "fobject.h"
#include "field.h"
#include "teamplayer.h"

namespace BtSoccer
{

/*! The goal keeper is an special teamPlayer. In phisics, it's represented
 * by a paralelepiped translated and rotated. */
class GoalKeeper: public TeamPlayer
{
   public:
      /*! Constructor
       * \param name -> internal name of the team player
       * \param fileName -> model's filename 
       * \param ogreSceneManager -> pointer to the used scene manager */
      GoalKeeper(Ogre::String name, Ogre::String fileName, 
            Ogre::SceneManager* ogreSceneManager, BulletDebugDraw* debugDraw);

      /*! Constructor without graphical elements. Usually used for test cases */
      GoalKeeper(Ogre::String name);

      /*! Get X Y Z sizes of paralelepiped */
      Ogre::Vector3 getSize();

      /*! Same as getSize()/2 */
      Ogre::Vector3 getHalfSize();

      /*! Set the goal keeper position to its start position at field */
      void startPositionAtField(bool upper, Field* f);
   
     /* Set the restrict move factor of the goal keeper.
      * \param restric true to restric move to be at the line (usually on
      * penalty shoots) */
      void setRestrictMove(bool restric);

      /*! Do the position input to the goalKeeper
       * \return true when input is done. */
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS ||\
    OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
      bool positionInput(Ogre::Vector3 mouseField, Ogre::Vector3 touch2Field,
                         Field* field);
#else
      bool positionInput(Ogre::Vector3 mouseField, bool leftButtonPressed,
            Field* field);
#endif

   protected:
      Ogre::Vector3 size;      /**< X, Y and Z rectangle sizes */
      Ogre::Vector3 halfSize;  /**< X, Y and Z rectangle half sizes */
      bool restrictMove;       /**< if current move is restrict */

};

};

#endif

