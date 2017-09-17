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

#ifndef _btsoccer_team_player_h
#define _btsoccer_team_player_h

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

namespace BtSoccer
{

/*! The teamPlayer is the representation of disks and goal keepers.
 * Each team has 1 goal keeper and 10 disks to play with;*/
class TeamPlayer: public FieldObject
{
   public:
      /*! Constructor
       * \param type -> FieldObject::TYPE_DISK or FieldObject::TYPE_GOAL_KEEPER
       * \param name -> internal name of the team player
       * \param fileName -> model's filename 
       * \param ogreSceneManager -> pointer to the used scene manager */
      TeamPlayer(int type, Ogre::String name, Ogre::String fileName, 
            Ogre::SceneManager* ogreSceneManager, BulletDebugDraw* debugDraw);

      /*! Constructor without graphical elements. Usually used in test cases.*/
      TeamPlayer(int type, Ogre::String name);

      /*! Destructor */
      ~TeamPlayer();

      /*! Set the teamPlayer material 
       * \param materialName -> name of the new material */
      void setMaterial(Ogre::String materialName);

      /*! Get the ammount of time after last collision with the ball
       * \return timer after last ball collision in ms */
      unsigned long getLastBallCollision();

      /*! Set the time the last collision with the ball occurred */
      void setLastBallCollision();

      /*! Do the position input to the goalKeeper
       * \return true when input is done. */
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS ||\
    OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
      bool positionInput(Ogre::Vector3 mouseField, bool leftButtonPressed,
                      Ball* gameBall, Field* field);
#else
      bool positionInput(Ogre::Vector3 mouseField, bool leftButtonPressed,
            Ball* gameBall, Field* field);
#endif

      /*! Set team which player belongs to */
      void setTeam(BtSoccer::Team* t);
      /*! Get team which player belongs to */
      BtSoccer::Team* getTeam();
   
      /*! Set the TeamPlayer model to be somewhat transparent.
       * \note it will affect all models with the same material.
       * \note the material need to be with scene_blend alpha_blend
       * and (optionally) transparent_sorting force defined. */
      void setTransparent();
      /*! Set the TeamPlayer model to be opaque */
      void setOpaque();
   
   protected:
      Ogre::Timer lastBallCollision;

      /*! Do some initialization common for all constructors */
      void init();
  
      /*! Checks if teamplayer is outside of the field. If so, it's moved back 
       *  to the field limits and its direction is changed */
      void checkFieldCollision();
   
      /*! Set the position to near posX, posZ, depending on ball position
       * \note -> this funciton is usually called when setting position
       *          to do a free, corner or penalty kick. */
      void setPositionWithBall(float posX, float posZ, Ball* gameBall,
                               Field* field);

      bool positionDone; /**< Position Input flux controll */
      Ogre::String materialName; /**< Current model material name */
      bool isOpaque; /**< if the model is currently opaque */

      BtSoccer::Team* curTeam; /**< current team which player belongs to */
};

};


#endif


