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

#ifndef _btsoccer_field_h
#define _btsoccer_field_h

#include <OGRE/OgreEntity.h>
#include <OGRE/OgreStaticGeometry.h>
#include <OGRE/OgreSceneManager.h>

#include "../physics/bulletlink.h"
#include "goals.h"
#include "../btsoccer.h"

namespace BtSoccer
{

#define FIELD_MIDDLE_X     0
#define FIELD_MIDDLE_Z     0

#define FIELD_BOUNCINESS  0.8f  /**< Field elasticty on side collision */

/*! The field class */
class Field
{
   public:
      /*! Constructor */
      Field();
      /*! Destructor */
      ~Field();

      /*! Create a field to use
       * \param fileName -> field definition file
       * \param ogreSceneManager -> pointer to the scene manager
       * \note side effect: this set the camera limits to be near. */
      void createField(Ogre::String fileName, 
            Ogre::SceneManager* ogreSceneManager);

      /*! Create a field to use, without any graphical element (usually
       * for test cases). 
       * \param createSideShapes -> if will create field borders, or just
       *                            the field ground (floor). */
      void createFieldForTestCases(bool createSideShapes);

      /*! Delete the created field */
      void deleteField();
   
      /*! Verify if point is inner penalty area */
      bool isInnerPenaltyArea(Ogre::Real x, Ogre::Real z,
                              bool testUp=true, bool testDown=true);
   
      /*! Verify if point is inner little area */
      bool isInnerLittleArea(Ogre::Real x, Ogre::Real z);
   
      /*! Get the nearest point in a little area */
      void getNearestPointInLittleArea(Ogre::Real& x, Ogre::Real& z);
   
      /*! Get nearest point in the playable area of the field.
       * \note Playable area is the one defined inner the side lines.
       * \param x original X coordinate to put inner playable area.
       * \param z original Z coordinate to put inner playable area.
       * \return true if point defined is different than the original. */
      bool getNearestPointInPlayableArea(Ogre::Real&x, Ogre::Real& z);

      /*! Get the bounding box related with the up goal */
      Ogre::AxisAlignedBox getUpGoalBox();
      /*! Get the bounding box related with the down goal */
      Ogre::AxisAlignedBox getDownGoalBox();

      /*! Get near goal factor value for posX,posZ
       * \param upperTeam -> if is relative to upperTeam
       * \param posX -> X coordinate
       * \param posZ -> Z coordinate 
       * \return -> factor relative to position and team */
      float getNearGoalFactor(bool upperTeam, float posX, float posZ);

      /* \return X coordinate relative to the byline of a goal
       * \param isUpper if return upper byline or down goal byline. !*/
      Ogre::Real getByline(bool isUpper);
   
      /*! Get goals used. */
      BtSoccer::Goals* getGoals();
      Ogre::Vector2 getHalfSize();
      Ogre::Vector2 getSideDelta();
      Ogre::Vector3 getBorder();
      Ogre::Vector2 getBorderDelta();
      Ogre::Vector2 getLittleAreaDelta();
      Ogre::Vector2 getPenaltyAreaDelta();
      Ogre::Real getPenaltyMark();
      Ogre::Real getGoalPosition();
      /*! Get number of disks per team allowed in the field */
      int getNumberOfDisks();
      /*! Set the number of disks allowed in the field. Usually
       * called on tutorial or on future "challenges" mode. */
      void setNumberOfDisks(int n);
      /*! Get current filename of the field used */
      Ogre::String getFileName();

#if BTSOCCER_RENDER_DEBUG
      /*! Draw, for debug, field position marks. */
      void debugDraw();
#endif

   protected:
   
      /*! Load field definition file
       * \param fileName -> field definition file
       * \param ogreSceneManager -> pointer to the scene manager */
      void loadField(Ogre::String fileName,
                       Ogre::SceneManager* ogreSceneManager);

      /*! Create field collision shapes */
      void createCollisionShapes(bool createSideShapes);

      BtSoccer::Goals* goals; /**< Goals */

      Ogre::SceneManager* pSceneManager; /**< Pointer to scene manager */
      
      Ogre::StaticGeometry* ogreField;     /**< The model on scene */
   
      Ogre::Vector3 scale;
      Ogre::Vector2 halfSize;
      Ogre::Vector2 sideDelta;
      Ogre::Vector3 border;
      Ogre::Vector2 borderDelta;
      Ogre::Vector2 littleAreaDelta;
      Ogre::Vector2 penaltyAreaDelta;
      Ogre::Real penaltyMark;
      Ogre::Real goalPosition;   /**< Goal X position */
      int numberOfDisks;         /**< Total number of disks per team */
      Ogre::String fieldFile;
   
      btCollisionShape* groundShape;     /**< ground Shape  */
      btDefaultMotionState* groundMotionState;
      btRigidBody* groundRigidBody;

      btCollisionShape* sideShape[4]; /**< Sides collisions */
      btDefaultMotionState* sideMotionState[4]; 
      btRigidBody* sideRigidBody[4];
   
#if BTSOCCER_RENDER_DEBUG
      Ogre::ManualObject          *mLines;
#endif
};

};

#endif

