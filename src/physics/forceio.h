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



#ifndef _btsoccer_forceio_h
#define _btsoccer_forceio_h

#include <OGRE/OgreMath.h>
#include "../btsoccer.h"

namespace BtSoccer
{

/*! The ForceInput class do the Input of the force value for a defined
 * position. It also draws the force direction and vector. */
class ForceInput
{
   public:
      /*! Constructor */
      ForceInput();
      /*! Destructor */
      ~ForceInput();
   
      /*! Clear the forceIO vector (usually called each time end the
       * player's shoot or pass) */
      void clear();

      /*! verify if the forceIO vector is cleared (not inited)
       * \return true if no iinited (clear), false otherwise */
      bool isClear();

      /*! Define the Initial Position of the force vector
       * \param x -> new initial X position
       * \param z -> new initial Z position */
      void setInitial(float x, float z);

      /*! Set Final Position of the force vector
       * \param x -> new final X position
       * \param z -> new final Z position */
      void setFinal(float x, float z);

      /*! Get the Force value and direction defined by the current vector
       * \param forceValue -> variable that will receive the force value 
       * \param directionX -> variable that will receive the X direction 
       * \param directionZ -> variable that will receive the Z direction
       * \return if force is calculated (and valid). */
      bool getForce(float& forceValue, float& directionX, float& directionZ,
            float minValue = BTSOCCER_MIN_FORCE_VALUE, 
            float maxValue = BTSOCCER_MAX_FORCE_VALUE);
   
      /*! Get current force value (with explicit angle). */
      bool getForce(float& forceValue, float& directionX,
                  float& directionZ, Ogre::Degree& angle);

      /*! Verify if the force is defined or not
       * \return true if the force is defined. */
      bool defined();

   protected:
      float initialX,      /**< Current Initial X value */
            initialZ;      /**< Current Initial Z value */

      float finalX,        /**< Current Final X value */
            finalZ;        /**< Current Final Z value */
};

};

#endif

