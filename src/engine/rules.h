#ifndef _btsoccer_rules_h
#define _btsoccer_rules_h

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

#include <kobold/timer.h>

#include "../btsoccer.h"
#include "../gui/guimessage.h"
#include "../gui/guiscore.h"
#include "../engine/stats.h"
#include "../net/protocol.h"

namespace BtSoccer
{

/*! The rules class is the controller of the game rules. By it 
 *  fouls, next player to play, goals, and all rules are defined
 *  based on the active rule system. */
class Rules
{
   public:

      enum RulesStates
      {
         STATE_NORMAL,
         STATE_MIDDLE,
         STATE_CORNER_KICK,
         STATE_FREE_KICK,
         STATE_THROW_IN,
         STATE_GOAL_KICK,
         STATE_PENALTY_KICK
      };

      enum RulesTypes
      {
         TYPE_BALL_FREE,
         TYPE_BALL_12,
         TYPE_BALL_3,
         TYPE_DISK_1
      };

      /*! Define the current game type
       * \param type -> integer constant with the game type */
      static void setGameType(int type);
      /*! Get the current game type */
      static int getGameType(){return(gameType);};
      
      /*! Define the length of each half of the match
       * \param minutes -> number of minutes per half */
      static void setMinutesPerHalf(int minutes);
      /*! Get the current minutes per half */
      static int getMinutesPerHalf(){return(minutesPerHalf);};

      /*! Verify if we are at the first half
       * \return -> true if at the first half. False if at the second. */
      static bool isFirstHalf();

      /*! Set the current half.
       * \param first -> true to first, false for second half */
      static void setHalf(bool first);

      /*! Set the team A pointer
       * \param t -> pointer to the teamA */
      static void setTeamA(Team* t);

      /*! Get team A pointer */
      static Team* getTeamA();

      /*! Set the team B pointer
       * \param t -> pointer to the teamB */
      static void setTeamB(Team* t);

      /*! Get team B pointer */
      static Team* getTeamB();

      /*! Define the used ball
       * \param b -> pointer to the in use ball */
      static void setBall(Ball* b);

      /*! Define the used field
       * \param f -> pointer to the current field */
      static void setField(Field* f);

      /*! Set Current In Use Disk, decrementing global and disk touches.
       * \note Usually called before a shoot or pass 
       * \param disk -> pointer to the current selected teamPlayer
       * \return -> true if can use the disk, false otherwise. */
      static bool setDiskAct(TeamPlayer* disk);

      /*! Set that will act directly on ball, decrementing global touches. */
      static void setBallAct();

      /*! Get the current global remaining touches
       * \return current global remaining touches */
      static int getRemainingTouches();
      /*! Set current global remaining touches */
      static void setGlobalRemainingTouches(int t);
   
      /*! Get the remaining touches for a disk
       * \return current remaining touches for the disk */
      static int getRemainingTouches(TeamPlayer* disk);
      /*! Set current disk remaining touches */
      static void setDiskRemainingTouches(int t);

      /*! Init a turn of play
       * \return -> pointer to the current team to act */
      static Team* newTurn();

      /*! Get the current rules state value
       * \return -> rules state constant. */
      static int getState();
      /*! Set the rule's current state
       * \note -> only usefull when loading a game */
      static void setState(int st);
      /*! Show a visual message of the current state. */
      static void showStateMessage();

      /*! Set the rules, according a received protocol message */
      static void set(ProtocolParsedMessage& msg);

      /*! Get the current active team.
       * \return -> pointer to the team acting */
      static Team* getActiveTeam();
      /*! Set current active team */
      static void setActiveTeam(Team* t);
      /*! \return the team that is not acting at this turn. */
      static Team* getInactiveTeam();

      /*! Get current disk acting
       * \return pointer to the current disk, if any defined */
      static TeamPlayer* getCurrentDisk();
      /*! Set current disk pointer */
      static void setCurrentDisk(TeamPlayer* tp);
   
      /*! Get the current upper team.
       * \return -> pointer to the team that is at upper field side */
      static Team* getUpperTeam();
      /*! Set the current upper team */
      static void setUpperTeam(Team* t);

      /*! Get the team not referenced by the pointer t */
      static Team* getOtherTeam(Team* t);

      /*! Get the current ball
       * \return pointer to the ball used */
      static Ball* getBall();

      /*! Verify if ball exited at upper side */
      static bool isBallUpper(){return ballUpper;};

      /*! Get the current field
       * \return pointer to the field used */
      static Field* getField();

      /*! Tells the opponent that will try a goal shoot now */
      static void prepareToShoot();

      /*! Verify if a goal shoot is defined (if the current player
       *  telled his oponent that he will do a goal shoot)
       *  \return true if the shoot is a goal try */
      static bool goalShootDefined();

      /*! Tell the rules that a disk collide another
       * \param diskPlayer1 -> team of the first disk
       * \param diskPlayer2 -> team of the second disk
       * \param x -> X coordinate where the collision occurs
       * \param z -> Z coordinate where the collision occurs */
      static void diskCollideDisk(Team* diskPlayer1, Team* diskPlayer2, 
                           float x, float z);

      /*! Tell the rules that the ball collide with a disk of the player
       * \param player -> team of the disk the ball contacts */
      static void ballCollideDisk(Team* player);

      /*! Tells that the ball left to one field side
       * \param x -> X coordinate where the exit occurs
       * \param z -> Z coordinate where the exit occurs */
      static void ballExitAtSide(float x, float z);

      /*! Tells that the ball left to one field byline
       * \param upper -> true if upper line, false if botton
       * \param z -> Z position where the ball exits */
      static void ballExitAtByline(bool upper, float z);

      /*! Tells that the ball enter the goal
       * \param upper -> true if the upper goal, false if botton */
      static void ballEnterGoal(bool upper);

      /*! When ball stop move and the physics is at a stable state.
       * Call this function one time to define the internal state 
       * of the rules system.
       * \param onlineMode true if is at online mode. */
      static void ballAtFinalPosition(bool onlineMode);

      /*! Start the Half Now
       * \param firstHalf -> if first half or not
       * \note -> called two times per game. */
      static void startHalf(bool firstHalf);

      /*! Update the Clock of the current half.
       * \return -> true if the half is over. */
      static bool updateClock();

      /*! Get the current half time (in milliseconds) */
      static unsigned long getCurrentHalfTime();
      /*! Set the current half time (in milliseconds) */
      static void setCurrentHalfTime(unsigned long ms);

      /*! Clear internal states. Usually, before a new game.
       * (it's called at #startHalf) */
      static void clear();
   
      /*! Clear internal flags. Usually, before starting a new action. */
      static void clearFlags();
   
      /*! Clear touches counters. Usually on tutorial. */
      static void clearTouchesCounters();

      /*! Set teams and ball positions based on current rule state */
      static void setPositions();

      /*! Verify if the active team changed after the last 
       * ballAtFinalPosition. 
       * \return -> true if current active team changed from last turn */
      static bool changedTeamToAct();
   
      /*! Pause the game clock */
      static void pause();
      /*! Resume the game clock */
      static void resume();

   protected:
   
      /*! Set remaining touches based on the current state. */
      static void setRemainingTouches();
   
      /*! Get the max number of disk touches per player act
       * \return max number of disk touches per disk act */
      static int maxRemainingDiskTouches();

      /*! Get the max number of global touches per act
       * \return max number of global touches per act  */
      static int maxRemainingGlobalTouches();

     /*! Verify if the goal that occurred was valid or not.
      * \param onlineMode true if is at online mode */
      static void verifyGoalValid(bool onlineMode);

      /*! Change the current team to act */
      static void changeTeamToAct();

      /*! Check if BALL_ACTION_BYLINE was a corner or a goalkick */
      static void checkCornerOrGoalKick();
   
      /*! Update statistics to a new state.
       * \param nextState next rules state
       * \param actingTeam current acting team.
       * \param nextActingTeam acting team at the nextState. */
      static void updateStatistics(int nextState, Team* actingTeam,
                            Team* nextActingTeam);

   private:
      /*! Instances not allowed. */
      Rules(){};


      static bool changedBallOwner;  /**< Flag of changed the active team */

      static int state;              /**< Current Rules State */

      static int minutesPerHalf;     /**< Number of minutes per half time */
      static int gameType;           /**< Current Game Type */

      static int halfMinutes;        /**< Current Half Minutes */
      static int halfSeconds;        /**< Current Half Seconds */

      static Kobold::Timer periodTimer; /**< The time for current period */

      static TeamPlayer* currentDisk;  /**< Current actor disk */

      static bool secondHalf;        /**< True if is the second half */

      static bool willShoot;         /**< True if the player will try a shoot */

      static int remainingGlobalTouches; /**< Touches to do on play */
      static int remainingDiskTouches;   /**< Remaining consecutive touches */

      static Team* teamA;            /**< Current TeamA */
      static Team* teamB;            /**< Current TeamB */
      static Team* activeTeam;       /**< Current Team in Act */
      static Team* upperTeam;        /**< Current team at upper side */
      static Team* lastBallCollided; /**< Last team the ball collide to */

      static Ball* usedBall;         /**< Current Ball */

      static Field* usedField;       /**< Current Field*/

      static bool collidedBallFirst; /**< When the player collided ball first */
      static bool collidedOwnDiskFirst; /**< When the player collided with
                                       another disk first of the same player */
      static bool collidedEnemyDiskFirst; /**< When the player collided with
                                          disk from the other player */

      static int ballAction;        /**< ID of some ball action */
      static float pX;              /**< X coordinate of the action */
      static float pZ;              /**< Z coordinate of the action */
      static bool ballUpper;        /**< True if ball at upper */

};

}

#endif

