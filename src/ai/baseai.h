#ifndef _btsoccer_base_ai_h
#define _btsoccer_base_ai_h

/*
  btsoccer - button football (soccer) game
  Copyright (C) DNTeam <btsoccer@dnteam.org>

  This file is part of btsoccer.

  btsoccer is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  btsoccer is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with btsoccer.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "../engine/team.h"
#include "../engine/teamplayer.h"
#include "../physics/bulletlink.h"
#include "../physics/disttable.h"

namespace BtSoccer
{
   
/*! The BaseAI is a virtual class used to all AI implementations */
class BaseAI
{
   public:
      /*! Constructor
       * \param t -> team the AI will controll
       * \param f -> current game field */
      BaseAI(BtSoccer::Team* t, Field* f);
      /*! Destructor */
      virtual ~BaseAI();

      /*! Select an action to do.
       * \return if action was selected or should be called again. */
      bool selectAction();

      /*! \return if have an action to do */
      const bool hasAction() const { return curTeamPlayer != NULL || 
                                            actOnBall; };

      /*! Get last selected player 
       * \return pointer to last selected player or NULL if cleared */
      BtSoccer::TeamPlayer* getSelectedPlayer() { return curTeamPlayer; };

      /*! Clear last selected action */
      void clearSelectedAction();

      /*! Verify if the desired shoot for AI will be a goal try
       * \return true if the AI will try a goal */
      const bool willGoalShoot() const { return tryGoalShoot; };

      /*! \return if should act directly on ball */
      const bool willActOnBall() const { return actOnBall; };

      /*! Get the initial calculated shoot force (X) */
      const float getInitialForceX() const { return initialForce[0]; };
      /*! Get the initial calculated shoot force (Z) */
      const float getInitialForceZ() const { return initialForce[1]; };
      /*! Get the final calculated shoot force (X) */
      const float getFinalForceX() const { return finalForce[0]; };
      /*! Get the final calculated shoot force (Z) */
      const float getFinalForceZ() const { return finalForce[1]; };

      /*! Clear the last act values
       * \note -> should be called after the act. */
      void clear();

      /*! Calculate the angle (relative to X axys) between two positions
       * \param posA first position
       * \param posB second position
       * \return angle in degreens. */
      float getAngleBetweenPositions(Ogre::Vector2 posA, Ogre::Vector2 posB); 

      /*! Do the disk input-position before freekicks */
      virtual void doDiskPosition(BtSoccer::TeamPlayer* tp)=0;

      /*! Do the goal keeper position before enemy try a shoot */
      virtual void doGoalKeeperPosition(BtSoccer::GoalKeeper* gk)=0;

      /*! When goal shoot, this function will be called just after
       * the enemy goal keeper position is set. */
      virtual void calculateGoalShoot()=0;

      /*! Draw any debug information on scren.
       * \note Only called when BTSOCCER_RENDER_DEBUG and BTSOCCER_DEBUG_AI
       *       are enabled. */
      virtual void debugDraw() {};

   protected:

      /*! Calculate an AI step (selecting a player to act,
       *  and calculating the shoot forces).
       *  \note -> this function must set "curTeamPlayer"
       *           or "actOnBall" when done with the calculation. 
       *           Otherwise, it will be repeatly called.
       *           It should also define the vectors to touch the
       *           target (initialForce -> finalForce) and, if
       *           will try a goal shoot, must set tryGoalShoot to true. */
      virtual void calculateStep()=0;

      /*! Get own disk nearest to the ball
       * \param attackDirection if ball must be ahead of the disk
       * \return disk found or null if none. */
      BtSoccer::TeamPlayer* getNearestBallDisk(bool attackDirection); 

      /*! Get the needed vector length to send a disk to a distance.
       * \note: if the ball is at the distance, it is the necessary to
       * touch it.
       * \param distance needed distance to send disk to.
       * \return length value, that should be used to calculate 
       * force vector.  */
      float getLengthToSendDiskToDistance(float distance);

      /*! Get the needed vector length to send the ball to a distance,
       * using a disk.
       * \param distanceToBall distance from the actor disk to the ball.
       * \param ballDistance target distance (from the ball) to send the 
       *                      ball to. */
      float getLengthToSendBallToDistanceWithDisk(float distanceToBall,
            float ballDistance);

      /*! Calculate and set force vector to disk tp send ball to position 
       * (target).
       * \param tp -> disk to touch the ball
       * \param ball -> pointer to the ball itself
       * \param target -> target position to send ball to
       * \param mustStop -> if ball must stop at target position, or
       *                    if we want it to just pass there. */
      void calculateForce(TeamPlayer* tp, Ball* ball, 
         Ogre::Vector3 target, bool mustStop);

      /*! Calculate and set force vector to send disk to a position
       * \param tp disk to move
       * \param target target position to disk move. */
      void calculateForce(TeamPlayer* tp, Ogre::Vector3 target);

      BtSoccer::Team* curTeam;              /**< Current controlled team */
      BtSoccer::TeamPlayer* curTeamPlayer;  /**< Current team player to act*/
      Ogre::Vector2 initialForce;      /**< intial position for force */
      Ogre::Vector2 finalForce;        /**< final position for force */
      bool tryGoalShoot;               /**< if the shoot is a goal try */
      bool actOnBall;                  /**< if will do a direct ball act */
};

};


#endif


