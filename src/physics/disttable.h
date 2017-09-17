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


#ifndef _btsoccer_disttable_h
#define _btsoccer_disttable_h

#include "../btsoccer.h"

/** Maximun input length to use. */
#define DISTTABLE_DISK_DIST_MAX_INPUT_LENGTH 80.0f
/** Increment to input used when calculating diskDistances vector.  */
#define DISTTABLE_DISK_DIST_INC     0.16f
#define DISTTABLE_DISK_DIST_LENGTH  512 

/** Minimum initial distance to use when calculating ballDistance vector. */
#define DISTTABLE_MIN_RELATIVE_BALL_DIST -14
/** Maximum initial distance to use when calculating ballDistance vector. */
#define DISTTABLE_MAX_RELATIVE_BALL_DIST 160
/** How much we should increment distances when calculating ballDistance.*/
#define DISTTABLE_RELATIVE_BALL_DIST_INC 0.1f
/** How many disk positions to test for each target distance.
  * Too many values could result on 'no moves' and wrong calculated values. */
#define DISTTABLE_BALL_DIST_STEPS 4

namespace BtSoccer
{

/*! Distance per input vector length table, used to precalculate disk and ball
 * input needed to send ball or disk to a distant point (usually usable
 * by AIs). */
class DistTable
{
   public:
      /*! Init the distable to use.
       * \param load true to load it from disk */
      static void init(bool load);
      /*! Finish the distable use. */
      static void finish();

      /*! Flush current calculated values to disk file.
       * \note only used when recalculating 'by-hand'. */
      static void flushToDisk();

      /*! Get the vector length to make a disk 'walk' for a desired distance
       * \param distance -> distance the disk need to reach
       * \return vector length to run the distance */
      static float getDiskInputVectorLength(float distance);

      /*! Get the vector length to make a disk send the ball throught a 
       * a distance.
       * \param distanceToBall -> distance between the disk and the ball.
       * \param ballDistance -> distance the ball needs to go throught.  */
      static float getDiskInputVectorLength(float distanceToBall, 
            float ballDistance);

   protected:
      
      /*! Do a binary search for needed vector value for a distance
       * at a distance vector. 
       * \param distance distance to search as the vector value
       * \param distances the vector to search
       * \param vectorSize size of distances vector.
       * \param incValue increment value to calculate index relative info
       * \param firstIndex value as the first (0) to calculate index relative
       *        information. */
      static float getVectorValue(float distance, float* distances,
            int vectorSize, float incValue, float firstIndex);

      /*! Wait for disk and ball to be stable. */
      static void waitDiskAndBallStable(TeamPlayer* disk, Ball* ball);

   private:
      DistTable(){};

      /*! Precalculate table values. Usually called at application's init */
      static void preCalculateValues();

      /*! Load previously calculated values from disk */
      static void loadFromDisk();

      
      /*! Disk distances per input vector: How far the disk went when done
       * some input on it. */
      static float diskDistances[DISTTABLE_DISK_DIST_LENGTH];

      /*! Ball distances per disk final position's distance to ball.
       * The idea is calculate how far the ball went when doing a force
       * to the disk sufficient to send it to a position relative to the ball.
       * With the distance, we can just use diskDistance to get the needed
       * input vector to send the ball to a positon.. */
      static float* ballDistances;
      /*! ballDistances vector size */
      static int ballDistancesSize;
      
};

}

#endif

