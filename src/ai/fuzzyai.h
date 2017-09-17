#ifndef _btsoccer_fuzzy_ai_h
#define _btsoccer_fuzzy_ai_h

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

#include "baseai.h"
#include "../physics/disttable.h"

namespace BtSoccer
{

#define BALL_UNKNOWN  -1

/*! The definition of an action for the fuzzy logic controller */
class FuzzyAction
{
   public:
      /*! Constructor */
      FuzzyAction();
      /*! Destructor */
      ~FuzzyAction();

      /*! Get action percentual (calculated when per disk)  */
      float getPercentual();

      /*! Get the disk which action percentual is relative to
       * \return disk number or -1 when not relative to disk */
      int getRelativeDisk();

      /*! Set a disk percentual */
      void setPercentual(int disk, float perc);
      
      /*! Set action percentual */
      void setPercentual(float perc);

      /*! Clear all values */
      void clear();

   protected:
      float percentual;           /**< Better percentual got */
      int percDisk;               /**< Disk which action percentual is 
                                       currently relative to */
      float toDiskPercentual[10]; /**< Percentual for each other target disk
                                       (for example: to which will pass) */
};

class FuzzyAI;

/*! The disk definition for fuzzy logic usage */
class FuzzyDisk
{
   public:
      enum
      {
         ACTION_PASS=0,
         ACTION_SHOOT,
         ACTION_THROW_AWAY,
         ACTION_BLOCK,
         ACTION_FOUL,
         ACTION_GO_TO_POSITION,
         ACTION_ADVANCE_WITH_BALL,
         TOTAL_ACTIONS
      };

      /*! Constructor */
      FuzzyDisk();
      /*! Destructor */
      ~FuzzyDisk();

      /*! Clear everithing set on disk, for re-usage */
      void clear();

      /*! Set the fuzzy AI used by the disk */
      void setFuzzyAI(FuzzyAI* fuzzyAI);

      /*! Calculate ball position
       * \param upper -> if own team is upper team */
      void calculateBallPosition(Ogre::RaySceneQuery* query);

      /*! Calculate current near goal factor
       * \param field -> pointer to the current field in use
       * \param upperTeam -> if team disk is of upper team or not */
      void calculateNearGoal(Field* field);

      /*! Calculate angle factor
       * \param angle -> angle value in degrees 
       * \return angle factor value */
      float calculateAngleFactor(float angle);

      /*! Calculate actions percentuals */
      void calculateActions(Ogre::RaySceneQuery* query);

      /*! Get the best action to do
       * \return int with action Id, 
       *         and float with its percentual */
      std::pair <int, float> getBestAction();

      FuzzyAction actions[TOTAL_ACTIONS]; /**< Each possible action */
      
      int ballPosition;          /**< Ball relative position to the disk */
      bool ballBlocked;          /**< If path to ball is blocked */
      float angleToBall;         /**< Angle value to the ball */
      float ballDistance;        /**< Distance to ball */
      Ogre::String firstCollide; /**< element the disk will first collide
                                      at the disk-ball ray. */
      float nearGoal;            /**< The Near goal factor */
      bool upperTeam;            /**< If upper team or not */

      TeamPlayer* disk; /**< The real disk it represents */
      FuzzyDisk* enemies; /**< Oponent disks */
      FuzzyDisk* disks;   /**< Own team disks */

   protected:
      /*! Calculate a factor for ball distance
       * \param near -> distance that is near from disk
       * \param far -> distance that is far from disk
       * \return -> current ball distance factor */
      float calculateNearBallFactor(float near, float far);
      
      /*! Calculate factor for ACTION_SHOOT */
      void calculateShootFactor();
      /*! Calculate factor for ACTION_PASS */
      void calculatePassFactor(Ogre::RaySceneQuery* query);
      /*! Calculate factor for ACTION_THROW_AWAY */
      void calculateThrowAwayFactor();
      /*! Calculate factor for ACTION_BLOCK */
      void calculateBlockFactor();
      /*! Calculate factor for ACTION_FOUL */
      void calculateFoulFactor();
      /*! Calculate factor for ACTION_GO_TO_POSITION */
      void calculateGoToBetterPosFactor();
      /*! Calculate factor for ACTION_ADVANCE_WITH_BALL */
      void calculateAdvanceWithBallFactor();

   private:
      FuzzyAI* ai; /**< AI used for disk */

};

/*! The FuzzyAI implements a fuzzy logic decision table for AI
 * actions. */
class FuzzyAI: public BaseAI
{
   public:
      /*! Constructor */
      FuzzyAI(BtSoccer::Team* t, Field* f);
      /*! Destructor */
      ~FuzzyAI();

      /*! Do the disk input-position before freekicks */
      void doDiskPosition(BtSoccer::TeamPlayer* tp);

      /*! Do the goal keeper position before enemy try a shoot */
      void doGoalKeeperPosition(BtSoccer::GoalKeeper* gk);

      /*! Calculate the goal shoot after goalKepper is set */
      void calculateGoalShoot();

   protected:
      /*! The internal AI calculation single step */
      void calculateStep();

      /*! Set action to do with the disk at this turn */
      void setAction(int dsk, int action);

      FuzzyDisk enemyDisk[10]; /**< Each enemy disk */
      FuzzyDisk disk[10]; /**< Each potential disk to act */
      bool defined;       /**< When things are defined */
      Ogre::RaySceneQuery* query; /**< needed query */

      int curDisk; /**< Current checking disk */

};

}

#endif

