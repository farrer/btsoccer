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

#include "ogremotionstate.h"
#include "../btsoccer.h"
using namespace BtSoccer;

/***********************************************************************
 *                           OgreMotionState                           *
 ***********************************************************************/
OgreMotionState::OgreMotionState(const btTransform &initialpos, 
      Ogre::SceneNode *node)
{
   mVisibleObj = node;
   mPos1 = initialpos;
   moved = false;
   lastMoved = false;
}

/***********************************************************************
 *                           OgreMotionState                           *
 ***********************************************************************/
OgreMotionState::~OgreMotionState()
{
}

/***********************************************************************
 *                               setNode                               *
 ***********************************************************************/
void OgreMotionState::setNode(Ogre::SceneNode *node)
{
   mVisibleObj = node;
}

/***********************************************************************
 *                         getWorldTransform                           *
 ***********************************************************************/
void OgreMotionState::getWorldTransform(btTransform &worldTrans) const
{
   worldTrans = mPos1;
}

/***********************************************************************
 *                        setWorldTransform                            *
 ***********************************************************************/
void OgreMotionState::setWorldTransform(const btTransform &worldTrans)
{
//#ifndef CHECK_MOVES_AT_EACH_WORLD_TRANSFORM
#if 0
   moved = true;
   
   /* Verify if defined */
   if(mVisibleObj == NULL)
   {
      return;
   }
   
   /* Define the node */
   btQuaternion rot = worldTrans.getRotation();
   mVisibleObj->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
   
   btVector3 pos = worldTrans.getOrigin();
   mVisibleObj->setPosition(pos.x() * BULLET_TO_OGRE_FACTOR,
                            pos.y() * BULLET_TO_OGRE_FACTOR,
                            pos.z() * BULLET_TO_OGRE_FACTOR);
   
   mPos1 = worldTrans;
   
#else

   /* Verify if defined */
   if(mVisibleObj == NULL)
   {
      /* Direct check the quaternion, as no visible object to check.
       * It only happens on unit test cases. */
      if(!moved)
      {
         moved = !(fabs(mPos1.getRotation().dot(worldTrans.getRotation())) >
                 (1.0f - 0.1f));
         moved |= mPos1.getOrigin() != worldTrans.getOrigin();
      }
      mPos1 = worldTrans;
      return; 
   }

#if 1
   /* NOTE: we are ignoring angles, as we no longer trying to use
    * Bullet for rolling. If someday use it again, must un#if0 this code. */
   /* Define the node */
   btQuaternion rot = worldTrans.getRotation();
   Ogre::Quaternion prevRot = mVisibleObj->getOrientation();
   mVisibleObj->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());

   /* Verify if quaternion changed (no need to re-verify if got change 
    * already at this frame) */
   if(!moved)
   {
      moved |= !prevRot.orientationEquals(mVisibleObj->getOrientation(), 0.01f);
   }
#endif

   btVector3 pos = worldTrans.getOrigin();
   Ogre::Vector3 prevPos = mVisibleObj->getPosition();
   mVisibleObj->setPosition(pos.x() * BULLET_TO_OGRE_FACTOR, 
                            pos.y() * BULLET_TO_OGRE_FACTOR, 
                            pos.z() * BULLET_TO_OGRE_FACTOR);

   /* Only check position change if not already got orientation change */
   if(!moved)
   {
      moved |= !prevPos.positionEquals(mVisibleObj->getPosition());
   }

   mPos1 = worldTrans;
#endif
}

/***********************************************************************
 *                             getMovedFlag                            *
 ***********************************************************************/
bool OgreMotionState::getMovedFlag()
{
   return moved;
}

/***********************************************************************
 *                             clearMovedFlag                          *
 ***********************************************************************/
void OgreMotionState::clearMovedFlag()
{
   lastMoved = moved;
   moved = false;
}

/***********************************************************************
 *                           getLastMovedFlag                          *
 ***********************************************************************/
bool OgreMotionState::getLastMovedFlag()
{
   return lastMoved;
}

