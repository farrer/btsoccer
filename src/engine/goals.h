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

#ifndef _btsoccer_goal_h
#define _btsoccer_goal_h

#include <OGRE/OgreEntity.h>
#include <OGRE/OgreStaticGeometry.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSphere.h>

#include "../physics/bulletlink.h"
#include "../btsoccer.h"

namespace BtSoccer
{

/*! A single goal representation */
class Goal
{
   public:
      /*! Constructor*/
      Goal(Ogre::StaticGeometry::Region* modelRegion, Field* f);
      /*! Constructor for non-graphical enviroment (usually for tests). */
      Goal(Field* f, bool upGoal);
      /*! Destructor */
      ~Goal();
      
      /*! Create poles collision entities 
       * \param halfHeight of the goal
       * \param halfZ half width of the goal 
       * \param goalBBox 3D model bouding box for goal
       * \param upGoal true if it is at up side. false at down. */
      void createPoles(float halfHeight, float halfZ, bool upGoal,
            Ogre::Vector3 goalBBox);

      /*! Create nets collision entities
       * \param halfX half width of the goal 
       * \param halfHeight of the goal
       * \param halfZ half width of the goal 
       * \param goalBBox 3D model bouding box for goal
       * \param upGoal true if it is at up side. false at down. */
      void createNets(float halfX, float halfHeight, float halfZ, bool upGoal,
            Ogre::Vector3 goalBBox);
   
      /*! @return if the bounding box intersects the goal poles.
       * @param bbox bounding box to check. */
      bool intersectsSidePoles(Ogre::AxisAlignedBox bbox);

      /*! \return bounding box for the goal at world's position. */
      Ogre::AxisAlignedBox getBoundingBox();

      Ogre::StaticGeometry::Region* staticModel;  /**< The static 3d model */

      /*! Collision for poles */
      btCollisionShape* poleShape[3]; 
      btDefaultMotionState* poleMotionState[3];
      btRigidBody* poleRigidBody[3];

      /*! Collision for nets */
      btCollisionShape* netShape[4]; 
      btDefaultMotionState* netMotionState[4];
      btRigidBody* netRigidBody[4];

   protected:
      /*! Create a single pole of size
       * \param i pole index
       * \param halfSizes each half size of pole cylinder
       * \param pos pole position
       * \param sidePole true if is side, false if up (travessao).
       * \param goalBBox 3D model bouding box for goal */
      void createPole(int i, btVector3 halfSizes, btVector3 pos,
            bool sidePole, Ogre::Vector3 goalBBox);

      /*! Create a single net of size
       * \param i net index
       * \param halfSizes each half size of net box
       * \param pos net position
       * \param goalBBox 3D model bouding box for goal */
      void createNet(int i, btVector3 halfSizes, btVector3 pos,
            Ogre::Vector3 goalBBox);

      /*! Nullify components. */
      void clear();
   
      Field* field;      /**< Current field */
      Ogre::Sphere sidePolesChecker[2]; /**< Sphere to check intersection
                                             with side poles */
      Ogre::AxisAlignedBox boundingBox;
};

/*! The goals representation (one at each side of the field): 
 * A goal  basically two poles and a place to send ball in. */
class Goals
{
   public:

      /*! Constructor 
       * \param f current field;
       * \param ogreSceneManager -> pointer to used scene manager */
      Goals(Ogre::SceneManager* ogreSceneManager, Field* f);
      /*! Constructor for non graphical, ie: tests */
      Goals(Field* f);
      /*! Destructor */
      ~Goals();

      /*! Get the bounding box related with the up goal */
      Ogre::AxisAlignedBox getUpGoalBox();
      /*! Get the bounding box related with the down goal */
      Ogre::AxisAlignedBox getDownGoalBox();
   
      /*! @return if the bounding box intersects the goal poles.
       * @param upper if to check upper or down goal
       * @param bbox bounding box to check. */
      bool intersectsSidePoles(bool upper, Ogre::AxisAlignedBox bbox);

      /*! Get near goal factor value for posX,posZ
       * \param upperTeam -> if is relative to upperTeam
       * \param posX -> X coordinate
       * \param posZ -> Z coordinate 
       * \return -> factor relative to position and team */
      float getNearGoalFactor(bool upperTeam, float posX, float posZ);
   
      /*! Partial hide goals (with an alpha blend), to make possible
       * better seeing throught it (for example, on goal keeper position set). */
      void setTransparent();
   
      /*! Make goals full visible again, after a partial hide call. */
      void setOpaque();

   protected:

      void createCollisionShapes();

      Ogre::SceneManager* pSceneManager; /**< Pointer to scene manager */
      
      Ogre::StaticGeometry* goals;      /**< Geometry for goals */
      bool isOpaque; /**< if goals are currently opaque or transparent */

      Ogre::Vector3 goalSize; /**< Goal sizes, based on 3D model */
   
      Field* field;      /**< Current field */

      Goal* upGoal;     /**< The upper goal */
      Goal* downGoal;   /**< The down goal */
};

}

#endif

