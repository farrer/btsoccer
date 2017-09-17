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

#include "goalkeeper.h"
#include "field.h"

namespace BtSoccer
{

/***********************************************************************
 *                              Constructor                            *
 ***********************************************************************/
GoalKeeper::GoalKeeper(Ogre::String name, Ogre::String fileName, 
      Ogre::SceneManager* ogreSceneManager, BulletDebugDraw* debugDraw) : 
   TeamPlayer(FieldObject::TYPE_GOAL_KEEPER, name, fileName, ogreSceneManager,
              debugDraw)
{
   float scale = TEAM_PLAYER_SCALE_FACTOR;
   positionDone = false;

   /* Calculate the rectangle, based on Model' bbox */
   Ogre::AxisAlignedBox box = model->getBoundingBox();
   box.scale(Ogre::Vector3(scale, scale, scale));
   size = box.getMaximum() - box.getMinimum();
   halfSize = size / 2;
}

/***********************************************************************
 *                              Constructor                            *
 ***********************************************************************/
GoalKeeper::GoalKeeper(Ogre::String name)
           :TeamPlayer(FieldObject::TYPE_GOAL_KEEPER, name)
{
   positionDone = false;
   size = 20;
   halfSize = 10;
}

/***********************************************************************
 *                         startPositionAtField                        *
 ***********************************************************************/
void GoalKeeper::startPositionAtField(bool upper, Field* f)
{
   Ogre::Vector2 halfSize = f->getHalfSize();
   Ogre::Real sideDelta = f->getSideDelta()[0];
   
   if(upper)
   {
      setPosition(halfSize[0] - sideDelta, 0.0f, 0);
      setOrientation(Ogre::Degree(90));
   }
   else
   {
      setPosition(-halfSize[0] + sideDelta, 0.0f, 0);
      setOrientation(Ogre::Degree(-90));
   }
}

/***********************************************************************
 *                                getSize                              *
 ***********************************************************************/
Ogre::Vector3 GoalKeeper::getSize()
{
   return(size);
}

/***********************************************************************
 *                              getHalfSize                            *
 ***********************************************************************/
Ogre::Vector3 GoalKeeper::getHalfSize()
{
   return(halfSize);
}

/***********************************************************************
 *                            setRestrictMove                          *
 ***********************************************************************/
void GoalKeeper::setRestrictMove(bool restric)
{
   restrictMove = restric;
}
   
/***********************************************************************
 *                             positionInput                           *
 ***********************************************************************/
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS ||\
    OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
bool GoalKeeper::positionInput(Ogre::Vector3 mouseField,
      Ogre::Vector3 touch2Field, Field* field)
{
   Ogre::Real x, z;
   
   if(Kobold::MultiTouchController::totalTouches() == 2)
   {
      /* Move the goal keeper to middle point */
      x = (mouseField.x + touch2Field.x) / 2.0f;
      z = (mouseField.z + touch2Field.z) / 2.0f;
      field->getNearestPointInLittleArea(x, z);
      
      /* Rotate the goal keeper */
      Ogre::Degree an;
      if(!restrictMove)
      {
         Ogre::Vector2 t1;
         if(mouseField.z > touch2Field.z)
         {
            t1 = Ogre::Vector2(touch2Field.x-mouseField.x,
                               touch2Field.z-mouseField.z);
         }
         else
         {
            t1 = Ogre::Vector2(mouseField.x-touch2Field.x,
                               mouseField.z-touch2Field.z);
         }
         Ogre::Vector2 t2(1.0f, 0.0f);
         an = Ogre::Degree(t1.angleBetween(t2).valueDegrees());
         if(x < 0)
         {
            /* Team isn't upper, so angle must be -180. */
            an = an - Ogre::Degree(180);
         }
      }
      else
      {
         an = Ogre::Degree(getOrientationY());
      }
      
      Ogre::Real prevAngle = getOrientationY();
      Ogre::Vector3 prevPos = getPosition();
      
      setOrientation(an);
      setPositionWithoutForcedPhysicsStep(
         Ogre::Vector3((!restrictMove)?x:getPosition().x, getPosition().y, z));
      
      /* Check if goal keeper can occupy the position (ie: no interception
       * with goal poles) */
      btVector3 boundMin, boundMax;
      rigidBody->getAabb(boundMin, boundMax);
      Ogre::AxisAlignedBox bbox(
            Ogre::Vector3(boundMin[0], boundMin[1], boundMin[2]),
            Ogre::Vector3(boundMax[0], boundMax[1], boundMax[2]));
      
      if(field->getGoals()->intersectsSidePoles(!(x < 0), bbox))
      {
         /* Must restore position, as intersect side pole. */
         setOrientation(Ogre::Degree(prevAngle));
         setPositionWithoutForcedPhysicsStep(prevPos);
      }
      BulletLink::forcedStep();
      
   }
   return(true);
}
#else
/***********************************************************************
 *                             positionInput                           *
 ***********************************************************************/
bool GoalKeeper::positionInput(Ogre::Vector3 mouseField, 
      bool leftButtonPressed, Field* field)
{
   Ogre::Vector2 littleAreaDelta = field->getLittleAreaDelta();
   Ogre::Real fieldX = field->getHalfSize().x;
   if( (positionDone) && (!leftButtonPressed) )
   {
      /* Done with press */
      positionDone = false;
      return(true);
   }

   if(!positionDone)
   {
      Ogre::Vector3 pos = getPosition();

      /* define the new Z position */
      if( (mouseField.z >= - littleAreaDelta[1]) &&
          (mouseField.z <= + littleAreaDelta[1]) )
      {
         pos.z = mouseField.z;
      }

      /* verify if is up or down */
      if(pos.x > 0)
      {
         /* Up */
         if( (mouseField.x <= fieldX) &&
             (mouseField.x >= fieldX - littleAreaDelta[0]) )
         {
            pos.x = mouseField.x;
         }
      }
      else
      {
         /* Down */
         if( (mouseField.x >= -fieldX) &&
             (mouseField.x <= -fieldX+littleAreaDelta[0]) )
         {
            pos.x = mouseField.x;
         }
      }
      
      Ogre::Real prevAngle = getOrientationY();
      Ogre::Vector3 prevPos = getPosition();

      /* Changes to the orientation angle */
      if(!restrictMove)
      {
         if(Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_COMMA) )
         {
            setOrientation(Ogre::Degree(getOrientationY()+2));
         }
         else if( Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_PERIOD) )
         {
            setOrientation(Ogre::Degree(getOrientationY()-2));
         }
      }
      
      /* Define new position */
      setPositionWithoutForcedPhysicsStep(pos);
      
      /* Check if goal keeper can occupy the position (ie: no interception
       * with goal poles) */
      btVector3 boundMin, boundMax;
      rigidBody->getAabb(boundMin, boundMax);
      Ogre::AxisAlignedBox bbox(
            Ogre::Vector3(boundMin[0], boundMin[1], boundMin[2]),
            Ogre::Vector3(boundMax[0], boundMax[1], boundMax[2]));
      
      if(field->getGoals()->intersectsSidePoles(!(pos.x < 0), bbox))
      {
         /* Must restore position, as intersect side pole. */
         setOrientation(Ogre::Degree(prevAngle));
         setPositionWithoutForcedPhysicsStep(prevPos);
      }
      BulletLink::forcedStep();

      if(leftButtonPressed)
      {
         /* Defined the position, must wait until mouse press end */
         positionDone = true;
      }
   }

   return(false);
}
#endif

};

