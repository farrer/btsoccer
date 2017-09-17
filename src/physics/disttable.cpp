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

#include "disttable.h"
#include "bulletlink.h"
#include "../btsoccer.h"
#include "../engine/field.h"
#include "../engine/fobject.h"
#include "../engine/ball.h"
#include "../engine/teamplayer.h"
#include "../physics/forceio.h"

#include <OGRE/OgreLogManager.h>
#include <OGRE/OgreLog.h>

#include <iostream>
using namespace std;

using namespace BtSoccer;

#define KEY_DISK_DISTANCE "diskDistance"
#define KEY_BALL_DISTANCE "ballDistance"


/***********************************************************************
 *                                init                                 *
 ***********************************************************************/
void DistTable::init(bool load)
{
   ballDistancesSize = (DISTTABLE_MAX_RELATIVE_BALL_DIST - 
                DISTTABLE_MIN_RELATIVE_BALL_DIST) /
               DISTTABLE_RELATIVE_BALL_DIST_INC;

   ballDistances = new float[ballDistancesSize];
   if(load)
   {
      loadFromDisk();
   }
   else
   {
      preCalculateValues();
   }
}

/***********************************************************************
 *                            finish                                   *
 ***********************************************************************/
void DistTable::finish()
{
   delete [] ballDistances;
}

/***********************************************************************
 *                       waitDiskAndBallStable                         *
 ***********************************************************************/
void DistTable::waitDiskAndBallStable(TeamPlayer* disk, Ball* ball)
{
   disk->prePhysicStep();
   ball->prePhysicStep();
   BulletLink::step(BTSOCCER_UPDATE_RATE, 10);
   while(disk->getMovedFlag() || ball->getMovedFlag())
   {
      disk->prePhysicStep();
      ball->prePhysicStep();
      BulletLink::step(BTSOCCER_UPDATE_RATE, 10);
   }
}


/***********************************************************************
 *                        preCalculateValues                           *
 ***********************************************************************/
void DistTable::preCalculateValues()
{
   ForceInput force;
   float forceValue=0.0f, forceX=0.0f, forceZ=0.0f;
 
   Field* field = new Field();
   field->createFieldForTestCases(false);

   /* Create the disk and ball to do the simulation */
   TeamPlayer* disk = new BtSoccer::TeamPlayer(FieldObject::TYPE_DISK, "disk");
   Ball* ball = new BtSoccer::Ball();
   BulletLink::setPointers(NULL, NULL, ball, field, false);

   ball->setPosition(-200, 0, -200);
   disk->setPosition(0, 0, 0);
   waitDiskAndBallStable(disk, ball);

   /* Do all range of impulses to the disk and save distances. */
   float length = DISTTABLE_DISK_DIST_INC;
   for(int i=0; i < DISTTABLE_DISK_DIST_LENGTH; i++) 
   {
      /* Keep current position, for distance calculation */
      Ogre::Vector3 prevPos = disk->getPosition();

      /* Set the force (impulse) for the input vector */
      force.clear();
      force.setInitial(0, 0);
      force.setFinal(-length, 0);
      force.getForce(forceValue, forceX, forceZ, 0.0f, 
            BTSOCCER_MAX_FORCE_VALUE);

      /* Apply the force and wait until the disk stopped. */
      disk->applyForce(forceValue*forceX, 0, forceValue*forceZ);
      waitDiskAndBallStable(disk, ball);

      /* Now, get position and calculate distance */
      Ogre::Vector3 pos = disk->getPosition();
      float distance = Ogre::Math::Sqrt(
            Ogre::Math::Sqr(pos[0]-prevPos[0]) +  
            Ogre::Math::Sqr(pos[2]-prevPos[2]));

      /* Keep it on the distance table. */
      diskDistances[i] = distance;
      //printf("Length: %.3f Dist: %.3f\n", length, distance);
      /*printf("Force: %.3f (%.2f, %.2f) Distance: %.3f length: %.3f\n", 
            forceValue, forceX, forceZ, distance, length);*/

      length += DISTTABLE_DISK_DIST_INC;
   }

   /* Now, let's calculate distances for ball touches after disk input */

   /** Let's calculate */
   int index = 0;
   for(float j = DISTTABLE_MIN_RELATIVE_BALL_DIST; 
           index < ballDistancesSize; 
           j += DISTTABLE_RELATIVE_BALL_DIST_INC)
   {
      float dist = 0.0f;
      for(int i = 0; i < DISTTABLE_BALL_DIST_STEPS; i++) 
      {
         float diskPosX = 22 + (1 * i);
         /* Set calculation positions */
         disk->setPosition(diskPosX, 0, 0);
         ball->setPosition(0, 0, 0);
         waitDiskAndBallStable(disk, ball);

         /* Set the force (impulse) for the input vector */
         force.clear();
         force.setInitial(diskPosX, 0);
         force.setFinal(diskPosX + getDiskInputVectorLength(diskPosX + j), 0);
         force.getForce(forceValue, forceX, forceZ);

         /* Apply the force and wait until disk and ball stopped. */
         disk->applyForce(forceValue*forceX, 0, forceValue*forceZ);
         waitDiskAndBallStable(disk, ball);

         /* Now, get position and calculate distance */
         Ogre::Vector3 pos = ball->getPosition();
         float distance = Ogre::Math::Sqrt(Ogre::Math::Sqr(pos[0]) +  
                                           Ogre::Math::Sqr(pos[2]));
         //printf("%.3f ", distance);
         dist += distance;
      }
      dist /= DISTTABLE_BALL_DIST_STEPS;
      ballDistances[index] = dist;
      index++;
      //printf("T: %.3f D: %.3f\n", j, dist);
   }


   /* Delete things */
   delete ball;
   delete disk;
   delete field;

   flushToDisk();
}

/***********************************************************************
 *                           loadFromDisk                              *
 ***********************************************************************/
void DistTable::loadFromDisk()
{
   Kobold::DefParser def;
   if(!def.load("distTable.xut"))
   {
      Ogre::LogManager::getSingleton().getDefaultLog()->logMessage(
            "Couldn't load distances file!", Ogre::LML_CRITICAL);
      return;
   }
   Ogre::String key, value;
   int ballIndex = 0;
   int diskIndex = 0;
   while(def.getNextTuple(key, value))
   {
      if(key == KEY_DISK_DISTANCE)
      {
         /* Got a disk distance */
         if(diskIndex >= DISTTABLE_DISK_DIST_LENGTH)
         {
            Ogre::LogManager::getSingleton().getDefaultLog()->logMessage(
                  "diskDistances index overflow!", Ogre::LML_CRITICAL);
         } 
         else
         {
            diskDistances[diskIndex] = Ogre::StringConverter::parseReal(value);
         }
         diskIndex++;
      }
      else if(key == KEY_BALL_DISTANCE)
      {
         /* Got a disk distance */
         if(ballIndex >= ballDistancesSize)
         {
            Ogre::LogManager::getSingleton().getDefaultLog()->logMessage(
                  "ballDistances index overflow!", Ogre::LML_CRITICAL);
         } 
         else
         {
            ballDistances[ballIndex] = Ogre::StringConverter::parseReal(value);
         }
         ballIndex++;
      }

   }
}

/***********************************************************************
 *                           flushToDisk                               *
 ***********************************************************************/
void DistTable::flushToDisk() 
{
   ofstream file;

   /* Open the file to flush values to */
   file.open("distTable.xut", ios::out | ios::binary);
   if(!file) 
   {
      return;
   }

   /* Flush disk distances */
   for(int i=0; i < DISTTABLE_DISK_DIST_LENGTH; i++) 
   {
      file << KEY_DISK_DISTANCE << " = " << diskDistances[i] << endl;
   }
   /* Flush ball distances by disk input. */
   for(int i=0; i < ballDistancesSize; i++) 
   {
      file << KEY_BALL_DISTANCE << " = " << ballDistances[i] << endl;
   }

   /* Close and done */
   file.close();
}

/***********************************************************************
 *                         getVectorValue                              *
 ***********************************************************************/
float DistTable::getVectorValue(float distance, float* distances, 
                                int vectorSize, float incValue,
                                float firstIndex)
{
   /* Do a binary search for the nearest distance */
   int min = 0;
   int max = vectorSize;
   int mid;

   while(max >= min)
   {
      mid = (max + min) / 2;

      if(distances[mid] < distance)
      {
         if(mid+1 < vectorSize)
         {
            if(distances[mid+1] > distance)
            {
               /* Must interpolate those mid and mid+1 */
               float res =(((distances[mid+1]-distance) * (mid)) +
                           ((distance-distances[mid]) * (mid + 1)) ) /
                           (distances[mid+1] - distances[mid]);
               return (res * incValue) + firstIndex;
            }
         }
         else
         {
            /* length must be the last element */
            return ((vectorSize - 1) * incValue) + firstIndex;
         }

         /* not found, continue the search */
         min = mid + 1;
      }
      else if(distances[mid] > distance)
      {
         if(mid-1 >= 0)
         {
            if(distances[mid-1] < distance)
            {
               /* Must interpolate mid-1 and mid */
               float res =(((distances[mid] - distance)*(mid - 1)) +
                         ((distance-distances[mid-1])*(mid)) ) /
                           (distances[mid] - distances[mid-1]);
               return (res * incValue) + firstIndex;
            }
         }
         else
         {
            /* length must be the first element */
            return firstIndex;
         }
         
         /* not found, continue the search */
         max = mid - 1;
      }
      else
      {
         /* Found the exact distance on table, no need to interpolate */
         return (mid * incValue) + firstIndex;
      }
   }

   /* None found */
   return 0.0f;
}

/***********************************************************************
 *                      getDiskInputVectorLength                       *
 ***********************************************************************/
float DistTable::getDiskInputVectorLength(float distance)
{
   return getVectorValue(distance, diskDistances, DISTTABLE_DISK_DIST_LENGTH,
         DISTTABLE_DISK_DIST_INC, DISTTABLE_DISK_DIST_INC);
}

/***********************************************************************
 *                      getDiskInputVectorLength                       *
 ***********************************************************************/
float DistTable::getDiskInputVectorLength(float distanceToBall, 
      float ballDistance) 
{
   /* Constant to multiply, due to innelastic contact). */
   float factor = 1.038f;

   /* Get the 'target-distance' to the ball we need to send the disk to move
    * the ball as much as desired. */
   float relativeBallDistance = getVectorValue(ballDistance, ballDistances,
         ballDistancesSize, DISTTABLE_RELATIVE_BALL_DIST_INC, 
         DISTTABLE_MIN_RELATIVE_BALL_DIST);
   
   float targetDiskDistance = distanceToBall + relativeBallDistance;

   /* if just touching very 'little' the disk, the factor must be greater,
    * otherwise, we'll probably won't move due to static friction */
   if(targetDiskDistance < distanceToBall)
   {
      factor = 4.6f;
   }

   /* Let's get the input to the disk needed to send it to the 'potential' 
    * target positioni */
   
   Ogre::LogManager::getSingleton().getDefaultLog()->stream()
         << "distanceToBall: " << distanceToBall << " relativeBallDistance: "
         << relativeBallDistance << " tagetDiskDistance: " 
         << targetDiskDistance;
   return getDiskInputVectorLength(targetDiskDistance) * factor;
}


float DistTable::diskDistances[DISTTABLE_DISK_DIST_LENGTH];
float* DistTable::ballDistances = NULL;
int DistTable::ballDistancesSize = 0;
