/*
  BtSoccer - button football (soccer) game
  Copyright (C) 2008-2015 DNTeam <btsoccer@dnteam.org>

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


#include "forceio.h"

#include <OGRE/OgreMath.h>
#include <OGRE/Ogre.h>

using namespace BtSoccer;

/*************************************************************
 *                       Constructor                         *
 *************************************************************/
ForceInput::ForceInput()
{
   clear();
}

/*************************************************************
 *                        Destructor                         *
 *************************************************************/
ForceInput::~ForceInput()
{
}

/*************************************************************
 *                          clear                            *
 *************************************************************/
void ForceInput::clear()
{
   initialX = BTSOCCER_VALUE_NOT_DEFINED;
   initialZ = BTSOCCER_VALUE_NOT_DEFINED;

   finalX = BTSOCCER_VALUE_NOT_DEFINED;
   finalZ = BTSOCCER_VALUE_NOT_DEFINED;
}

/*************************************************************
 *                        isClear                            *
 *************************************************************/
bool ForceInput::isClear()
{
   return( (initialX == BTSOCCER_VALUE_NOT_DEFINED) &&
           (initialZ == BTSOCCER_VALUE_NOT_DEFINED));
}

/*************************************************************
 *                       setInitial                          *
 *************************************************************/
void ForceInput::setInitial(float x, float z)
{
   initialX = x;
   initialZ = z;
}

/*************************************************************
 *                        setFinal                           *
 *************************************************************/
void ForceInput::setFinal(float x, float z)
{
   finalX = x;
   finalZ = z;
}

/*************************************************************
 *                         defined                           *
 *************************************************************/
bool ForceInput::defined()
{
   return( (initialX != BTSOCCER_VALUE_NOT_DEFINED) &&
           (initialZ != BTSOCCER_VALUE_NOT_DEFINED) &&
           (finalX != BTSOCCER_VALUE_NOT_DEFINED) &&
           (finalZ != BTSOCCER_VALUE_NOT_DEFINED) );
}

/*************************************************************
 *                        getForce                           *
 *************************************************************/
bool ForceInput::getForce(float& forceValue, float& directionX,
           float& directionZ, Ogre::Degree& angle)
{
   if(getForce(forceValue, directionX, directionZ))
   {
      if(!defined())
      {
         angle = 0;
      }
      else
      {
         angle = Ogre::Vector2(directionX, directionZ).angleTo(
            Ogre::Vector2(0,-1)).valueDegrees();
      }
      return true;
   }
   return false;
}

/*************************************************************
 *                        getForce                           *
 *************************************************************/
bool ForceInput::getForce(float& forceValue, float& directionX,
      float& directionZ, float minValue, float maxValue)
{
   float dX, dZ;
   
   if(!defined())
   {
      /* Some value isn't defined yet! */
      forceValue = 0;
      directionX = 0;
      directionZ = 0;
      return false;
   }

   /* divided by FACTOR for better ranged values */
   dX = (finalX - initialX);
   dZ = (finalZ - initialZ);
   forceValue = Ogre::Math::Sqrt( (dX*dX) + (dZ*dZ) );
   
   /* Normalize vector */
   if(forceValue != 0)
   {
      directionX = -dX / forceValue;
      directionZ = -dZ / forceValue;
   }
   else
   {
      directionX = 0;
      directionZ = 0;
   }

   forceValue *= BTSOCCER_FORCE_MULTIPLIER * 20.0f;
   
   /* Apply force value limits */
   if(forceValue < minValue)
   {
      /* Not greather than the minimun threshold */
      forceValue = minValue;
   }
   else if(forceValue > maxValue)
   {
      forceValue = maxValue;
   }
   
   return true;
}

