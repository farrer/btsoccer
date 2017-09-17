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

#ifndef _btsoccer_decourt_ai_h
#define _btsoccer_decourt_ai_h

#include "baseai.h"

namespace BtSoccer
{

#define MAX_ELEGIBLE_DISKS   5

/*! Being it the default AI for BtSoccer, more than just to it have the
 * name of button soccer inventor: Geraldo Cardoso Decourt.
 * The idea behind this AI is to have some set of specific types of moves,
 * from each the AI choose what to do next, based on what it done previously,
 * how many plays it have (both for current acting disk and team) and field / 
 * / ball / players positions and current rules (for example, if are doing
 * a throw-in, a corner-kick, etc).
 * The actions are:
 *    - Advance (a disk 'carry' the ball with him);
 *    - Side opening (a disk touches the ball laterally to another
 *                    disk advance with it on next turn);
 *    - Pass (a disk send's the ball to another disk);
 *    - Turnout (the disk touches the ball that was behind him, being able
 *               to set it ahead at the end of movement);
 *    - Goal shoot (the disk try to score a goal); 
 *    - Repositionate (the disk try to go to a better location on the field) */
class DecourtAI : public BaseAI
{
   public:
      /*! Constructor
       * \param t -> team the AI will controll
       * \param f -> current game field */
      DecourtAI(BtSoccer::Team* t, Field* f);

      /*! Destructor */
      ~DecourtAI();

      /*! Do the disk input-position before freekicks */
      void doDiskPosition(BtSoccer::TeamPlayer* tp);

      /*! Do the goal keeper position before enemy try a shoot */
      void doGoalKeeperPosition(BtSoccer::GoalKeeper* gk);

      /*! When goal shoot, this function will be called just after
       * the enemy goal keeper position is set. */
      void calculateGoalShoot();

      void debugDraw();

    protected:

      enum DecourtAISteps
      {
         STEP_INITIAL = 0,
         STEP_BALL_INNER_OWN_AREA,
         STEP_SELECT_POTENTIAL_DISKS,
         STEP_CHECK_DISK_ACTIONS,
         STEP_SELECT_DISK_TO_ACT
      };

      /*! Actions enum: greater values -> higher action priority */
      enum DecourtAIActions
      {
         ACTION_NONE = 0,
         ACTION_GO_TO_BETTER_POSITION,
         ACTION_TURN_AROUND,
         ACTION_PASS,
         ACTION_SIDE_OPENING,
         ACTION_SHOOT_TO_GOAL,
         ACTION_ADVANCE,
         ACTION_DIRECT_ON_BALL_PASS
      };

      /*! Information about action to take. Usually kept for each 
       * potential-to-act disk */
      class ActionInfo
      {
         public:
            ActionInfo() { action = ACTION_NONE; actor = NULL; target = NULL;};

            const DecourtAIActions getAction() const { return action; };
            BtSoccer::TeamPlayer* getActor() const { return actor; };
            BtSoccer::TeamPlayer* getTarget() const { return target; };
            const Ogre::Vector3& getPos() const { return pos; };
            const Ogre::Vector3& getActorInitialPos() const 
            { 
               return actorInitialPos; 
            };
            const Ogre::Vector3& getTargetInitialPos() const 
            { 
               return targetInitialPos; 
            };
            void set(const DecourtAIActions act)
            {
               this->action = act;
               this->actor = NULL;
               this->pos = Ogre::Vector3(0.0f, 0.0f, 0.0f);
               this->target = NULL;
               this->targetInitialPos = Ogre::Vector3(0.0f, 0.0f, 0.0f);
            }
            void set(const ActionInfo& info)
            {
               this->set(info.action, info.actor, info.pos, info.target);
            }
            void set(const DecourtAIActions action, BtSoccer::TeamPlayer* actor,
                  Ogre::Vector3 pos, BtSoccer::TeamPlayer* target)
            {
               this->action = action;
               this->actor = actor;
               this->pos = pos;
               this->actorInitialPos = (actor != NULL) ?
                  actor->getPosition() : Ogre::Vector3(0.0f, 0.0f, 0.0f);
               this->target = target;
               this->targetInitialPos = (target != NULL) ?
                  target->getPosition() : Ogre::Vector3(0.0f, 0.0f, 0.0f);
            };
             

         private:
            DecourtAIActions action; /**< DecourtAIActions identifier */
            BtSoccer::TeamPlayer* actor; /**< The disk to act */
            Ogre::Vector3 pos; /**< Related action coordinate */
            Ogre::Vector3 actorInitialPos; /**< Actor position when defined */
            BtSoccer::TeamPlayer* target; /**< A target of the action */
            Ogre::Vector3 targetInitialPos; /**< Target position when defined */
      };

      /*! Select the potential disks to act.
       * \image html ../../docs/ai/00_basic_flux.png */
      void selectPotentialDisks();

      /*! Calculate an AI step (selecting a player to act,
       *  and calculating the shoot forces).
       * \image html ../../docs/ai/00_basic_flux.png */
      void calculateStep();

      /*! Check action to do with disk.
       * \image html ../../docs/ai/01_elegible_decision_map.png
       * \param tp disk to check
       * \return information of action to take. */
      const ActionInfo checkAction(BtSoccer::TeamPlayer* tp);

      /*! Check if current disk should turn around using ball as axys */
      bool shouldTurnAround(BtSoccer::TeamPlayer* tp, Ogre::Vector2 ballPos,
            Ogre::Vector2 directionToBall, bool isUpper, int relativeBallPos,
            ActionInfo& info);

      /*! Check if ball is too near opponent's goal to try to advance with it.
       * \param isUpper if active team is upper
       * \param ballPos current ball position
       * \return true if too near or false if can try to advance with ball */
      bool isTooNearToAdvance(bool isUpper, const Ogre::Vector2& ballPos);

      /*! Check if elected disk should advance with ball.
       * \image html ../../docs/ai/02_should_advance_decision_map.png
       * \param tp disk to check. 
       * \param ballPos ball position at the field.
       * \param directionToBall normalised vector do act on disk to
       *        go to the ball. 
       * \param isUpper if is upper team or down.*/
      bool shouldAdvanceWithBall(BtSoccer::TeamPlayer* tp, 
            Ogre::Vector2 ballPos, Ogre::Vector2 directionToBall, bool isUpper,
            int relativeBallPos, ActionInfo& info);

      /*! Check if elected disk should try a goal shoot.
       * \image html ../../docs/ai/03_should_try_goal_shoot_decision_map.png
       * \param tp disk to check 
       * \param ballPos ball position at the field.
       * \param directionToBall normalised vector do act on disk to
       *        go to the ball. 
       * \param isUpper if is upper team or down.*/
      bool shouldTryGoalShoot(BtSoccer::TeamPlayer* tp,
            Ogre::Vector2 ballPos, Ogre::Vector2 directionToBall, bool isUpper,
            ActionInfo& info);

      /*! Check if disk should 'side-open' the ball to another disk act
       * on next turn.
       * \image html ../../docs/ai/04_should_side_opening.png
       * \param tp disk to check
       * \param posX if returned true will receive ball target X coordinate.
       * \param posZ if returned true will receive ball target Z coordinate. */
      bool shouldSideOpening(BtSoccer::TeamPlayer* tp, Ogre::Vector2 ballPos,
            Ogre::Vector2 directionToBall, bool isUpper, ActionInfo& info);

      /*! Check if disk should pass the ball to another disk
       * \image html ../../docs/ai/05_should_pass.png
       * \param tp disk to check
       * \param posX if returned true, will receive ball target X coordinate
       * \param posZ if returned true will receive ball target Z coordinate */
      bool shouldPass(BtSoccer::TeamPlayer* tp, Ogre::Vector2 ballPos,
            Ogre::Vector2 directionToBall, bool isUpper, ActionInfo& info);

      /*! Check, with cache, if disk has free area to the ball. */ 
      bool checkFreeAreaToBall(BtSoccer::TeamPlayer* tp, 
            Ogre::Vector2 ballPos);

      /*! Calculate the force vector to do with the action. */
      void calculateForceVector(ActionInfo* action);

      /*! \return nearest to opponent's goal X position, from three ones. */
      Ogre::Real getMostAdvancedX(Ogre::Real x1, Ogre::Real x2,
            Ogre::Real x3, bool isUpper); 

   private:
      int curStep; /**< current step */
      int curDisk; /**< current elected 'on-analise' disk */
 
      /*! Disks that will check for action */
      BtSoccer::TeamPlayer* electedDisk[MAX_ELEGIBLE_DISKS];
      /*! Best action calculated for each elected disk */
      ActionInfo bestAction[MAX_ELEGIBLE_DISKS];

      ActionInfo lastAction; /**< Last taken action */

      Ogre::RaySceneQuery* query; /**< Query used for ray casts. */

      /*! If the value at #hasFreeAreaToBall is up-to-date for the 
       * current in check disk. */
      bool calculatedCurrentDiskFreeAreaToBall;
      /*! If disk has free area to ball - cache */
      bool hasFreeAreaToBall;

};


}

#endif

