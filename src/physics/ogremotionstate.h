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


#ifndef _btsoccer_ogremotionstate_h
#define _btsoccer_ogremotionstate_h

#include <btBulletDynamicsCommon.h>
#include <OGRE/OgreSceneNode.h>

namespace BtSoccer
{

/*! Bullet Motion State to set ogre scene nodes.  */
class OgreMotionState : public btMotionState
{
   public:
      /*! Create the motion state with Ogre3D
       * \param initialPos -> initial rigid body position
       * \param node -> pointer to the ogreSceneNode of the rigid body */
      OgreMotionState(const btTransform &initialpos, Ogre::SceneNode *node);

      /*! Destructor */
      virtual ~OgreMotionState();

      /*! Set the SceneNode of the rigid body 
       * \param node -> pointer to the SceneNode used. */
      void setNode(Ogre::SceneNode *node);

      /*! Get the current world transform */
      virtual void getWorldTransform(btTransform &worldTrans) const;

      /*! Function to bullet set the world transform after a move */
      virtual void setWorldTransform(const btTransform &worldTrans);

      /*! Clear the moved flag */
      void clearMovedFlag();

      /*! Check if the object moved since last clear */
      bool getMovedFlag();
   
      /*! Check moved flag before last clear. */
      bool getLastMovedFlag();

   protected:
      Ogre::SceneNode *mVisibleObj;  /**< SceneNode related to the btObject */
      btTransform mPos1;  /**< Current world transform. */
      bool moved;        /**< Current "if moved" flag */
      bool lastMoved;    /**< Last moved flag value */
};

}

#endif

