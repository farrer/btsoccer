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

#include "decourtai.h"

#include "../engine/ball.h"
#include "../engine/field.h"
#include "../engine/rules.h"
#include "../engine/team.h"
#include "../engine/teamplayer.h"
#include "../gui/guimessage.h"

#include <iostream>
#include <OGRE/OgreMath.h>
#include <OGRE/OgreLogManager.h>

using namespace BtSoccer;

/*! Distance to send ball when advancing with it. */ 
#define BALL_ADVANCE_DISTANCE   20.0f
/*! Minimum NearGoalFactor to consider the ball at a positive to shoot
 * position */
#define BALL_MIN_NEAR_GOAL_FACTOR 0.7f
/*! Minimum Z distance to goal center where the ball should be after a
 * direct angle touch from disk to a goal shoot be acceptable. */
#define BALL_MIN_VALID_Z_DISTANCE 90.0f

#define UNDEFINED_POSITION -1234567.0f

/***************************************************************************
 *                                Constructor                              *
 ***************************************************************************/
DecourtAI::DecourtAI(BtSoccer::Team* t, Field* f)
          :BaseAI(t, f)
{
   curStep = STEP_INITIAL;
   for(int i = 0; i < MAX_ELEGIBLE_DISKS; i++)
   {
      electedDisk[i] = NULL;
   }

   query = t->getSceneManager()->createRayQuery(Ogre::Ray());
}

/***************************************************************************
 *                                 Destructor                              *
 ***************************************************************************/
DecourtAI::~DecourtAI()
{
   curTeam->getSceneManager()->destroyQuery(query);
}

/***************************************************************************
 *                            doDiskPosition                               *
 ***************************************************************************/
void DecourtAI::doDiskPosition(BtSoccer::TeamPlayer* tp)
{
   /* TODO: borrowed from fuzzy AI. */
   Ogre::Vector2 tgtGoal; /**< Target goal position */
   Ogre::Vector2 dir; /**< Calculated disk position */

   BtSoccer::Ball* ball = Rules::getBall();
   Ogre::Vector3 ballPos = ball->getPosition();
   Ogre::Real fieldX = Rules::getField()->getHalfSize().x;

   /* Calculate disk to ball minimun distance */
   float ballDist = 20.0f;

   /* Point the disk to the goal */
   if(curTeam == Rules::getUpperTeam())
   {
      /* Will attack at bottom */
      tgtGoal[0] = -fieldX;
      tgtGoal[1] = 0.0f;
   }
   else
   {
      /* Will attack at upper */
      tgtGoal[0] = fieldX;
      tgtGoal[1] = 0.0f;
   }

   /* Calculate direction */
   dir[0] = ballPos.x - tgtGoal[0];
   dir[1] = ballPos.z - tgtGoal[1];
   dir.normalise();

   /* Set its position */
   tp->setPosition(ballPos.x+dir[0]*ballDist, 0.0f,
         ballPos.z+dir[1]*ballDist);

   /* Set shoot force */
   float forceValue;
   /* Calculate force value, depending on type */
   switch(Rules::getState())
   {
      case Rules::STATE_GOAL_KICK:
         forceValue = 130.0f;
         break;
      default:
         forceValue = 40.0f;
         break;
   }

   /* Set force direction */
   initialForce[0] = tp->getPosition().x;
   initialForce[1] = tp->getPosition().z;
   finalForce = initialForce+dir*forceValue;
   curTeamPlayer = tp;
}

/***************************************************************************
 *                           doGoalKeeperPosition                          *
 ***************************************************************************/
void DecourtAI::doGoalKeeperPosition(BtSoccer::GoalKeeper* gk)
{
}

/***************************************************************************
 *                             calculateGoalShoot                          *
 ***************************************************************************/
void DecourtAI::calculateGoalShoot()
{
   //TODO: maybe:
   //if ball right, try to send it to the goal's left angle
   //if left, try to right

   // TODO: borrwoed from FuzzyAI
   /* TODO: set better target looking at current gk position*/
   Ogre::Vector3 goalCenter;
   goalCenter[2] = 0.0f;
   goalCenter[1] = 0.0f;
   if(Rules::getUpperTeam() == curTeam)
   {
      goalCenter[0] = Rules::getField()->getHalfSize().x +
         Rules::getField()->getSideDelta()[0];
   }
   else
   {
      goalCenter[0] = Rules::getField()->getHalfSize().x -
         Rules::getField()->getSideDelta()[0];
   }
   calculateForce(curTeamPlayer, Rules::getBall(), goalCenter, false);
}

/***************************************************************************
 *                         calculateForceVector                            *
 ***************************************************************************/
void DecourtAI::calculateForceVector(ActionInfo* action)
{
   Ball* gameBall = Rules::getBall();

#if BTSOCCER_DEBUG_AI
   Ogre::Log::Stream stream = Ogre::LogManager::getSingleton().stream(
         Ogre::LML_NORMAL);

   stream << "\n*****************************\n";
   Ogre::Vector3 ballPos = gameBall->getPosition();
   stream << "With ball at " << ballPos.x << ", " << ballPos.z << " ";
   if(action->getActor() != NULL)
   {
      Ogre::Vector3 acPos = action->getActor()->getPosition();
      stream << action->getActor()->getName() << " at " << acPos.x << ", "
             << acPos.z << " \n";
   } 
   else
   {
      stream << "Unknown";
   }
#endif

   switch(action->getAction())
   {
      case ACTION_GO_TO_BETTER_POSITION:
      {
#if BTSOCCER_DEBUG_AI
         stream << ": Will try a better position";
         GuiMessage::set("Will try a better position");
#endif
         calculateForce(action->getActor(), action->getPos());
      }
      break;
      case ACTION_TURN_AROUND:
      {
#if BTSOCCER_DEBUG_AI
         stream << ": Will turn around";
         GuiMessage::set("Will turn around");
#endif
         calculateForce(action->getActor(), action->getPos());
      }
      break;
      case ACTION_PASS:
      {
#if BTSOCCER_DEBUG_AI
         stream << ": Will pass";
         GuiMessage::set("Will pass");
#endif
         /* Will try a pass to the desired position */
         calculateForce(action->getActor(), gameBall, action->getPos(), true);
      }
      break;
      case ACTION_SIDE_OPENING:
      {
#if BTSOCCER_DEBUG_AI
         stream << ": Will side open to " 
                << action->getPos()[0] << ", " << action->getPos()[2];
         GuiMessage::set("Will side open" );
#endif
         calculateForce(action->getActor(), gameBall, action->getPos(), true);
      }
      break;
      case ACTION_SHOOT_TO_GOAL:
      {
         /* Note: calculateGoalShoot will be called after set opponent's goal 
          * keeper position */
#if BTSOCCER_DEBUG_AI
         stream << ": Will do a goal shoot";
         GuiMessage::set("Will do a goal shoot");
#endif
         tryGoalShoot = true;
      }
      break;
      case ACTION_ADVANCE:
      {
#if BTSOCCER_DEBUG_AI
         stream << ": Will advance with ball";
         GuiMessage::set("Will advance with ball");
#endif
         /* Going to the ball position should be sufficient to
          * advance with it (empirically tested!). */
         calculateForce(action->getActor(), gameBall->getPosition());
         //XXX calculateForce(action->actor, gameBall, action->pos, true);
         //TODO: maybe some target angle definition instead of just 
         // advance with ball: something like: ballPos+var, being
         // var in direction to goal center.
      }
      break;
      case ACTION_DIRECT_ON_BALL_PASS:
      {
#if BTSOCCER_DEBUG_AI
         stream << ": Will pass direct on ball act";
         GuiMessage::set("Will pass direct on ball act");
#endif
         //TODO: should select a target disk to send ball to;
         //      for now, just touching the ball
         initialForce[0] = gameBall->getPosition().x;
         initialForce[1] = gameBall->getPosition().z;
         bool upperTeam = (curTeam == Rules::getUpperTeam());
         int signal = (upperTeam) ? 1 : -1;
         finalForce[0] = initialForce[0] + (signal * 40);
         finalForce[1] = initialForce[1];
      }
      break;
      case ACTION_NONE:
      default:
      {
#if BTSOCCER_DEBUG_AI
         stream << "Will do nothing!";
         GuiMessage::set("Will do nothing!");
#endif
      }
      break;
   }
#if BTSOCCER_DEBUG_AI
   stream << "\n*****************************\n";
#endif
}

/***************************************************************************
 *                               calculateStep                             *
 ***************************************************************************/
void DecourtAI::calculateStep()
{
   switch(curStep)
   {
      case STEP_INITIAL:
      {
         /* The ai just started a turn, should go next step 
          * (note that we do no break here and go next)*/
         for(int i = 0; i < MAX_ELEGIBLE_DISKS; i++)
         {
            electedDisk[i] = NULL;
         }
         curStep = STEP_BALL_INNER_OWN_AREA;
      }
      case STEP_BALL_INNER_OWN_AREA:
      {
         /* Check if ball is inner own area */
         bool isUpper = (curTeam == Rules::getUpperTeam());
         Ball* gameBall = Rules::getBall();
         if(Rules::getField()->isInnerPenaltyArea(gameBall->getPosition().x, 
                  gameBall->getPosition().z, isUpper, !isUpper))
         {
            /* Should do a pass with direct ball act */
            lastAction.set(ACTION_DIRECT_ON_BALL_PASS, NULL, 
                  Ogre::Vector3(0.0f, 0.0f, 0.0f), NULL);
            curTeamPlayer = NULL;
            actOnBall = true;
            calculateForceVector(&lastAction);

            /* As we early known what to do, the calculation is done. */
            return;
         }
         curStep = STEP_SELECT_POTENTIAL_DISKS;
      }
      break;
      case STEP_SELECT_POTENTIAL_DISKS:
      {
         /* Choose disks to set potential actions */
         selectPotentialDisks();
         for(int i = 0; i < MAX_ELEGIBLE_DISKS; i++)
         {
            bestAction[i].set(ACTION_NONE, NULL, 
                  Ogre::Vector3(UNDEFINED_POSITION, 0.0f, UNDEFINED_POSITION),
                  NULL);
         }
         curStep = STEP_CHECK_DISK_ACTIONS;
         curDisk = 0;
      }
      break;
      case STEP_CHECK_DISK_ACTIONS:
      {
         /* Step for checking each disk possible disk action */
         if( (curDisk < MAX_ELEGIBLE_DISKS) && (electedDisk[curDisk] != NULL) )
         {
            calculatedCurrentDiskFreeAreaToBall = false;
            
            const ActionInfo resAction = checkAction(electedDisk[curDisk]);
            bestAction[curDisk].set(resAction);

            curDisk++;
         }
         else
         {
            /* Done with step */
            curStep = STEP_SELECT_DISK_TO_ACT;
         }
      }
      break;
      case STEP_SELECT_DISK_TO_ACT:
      {
         /* Step to choose the best disk action to take */
         ActionInfo* curAction = NULL;
         for(int i = 0; i < MAX_ELEGIBLE_DISKS; i++)
         {
            if((bestAction[i].getAction() != ACTION_NONE) && 
               ((curAction == NULL) || 
                (bestAction[i].getAction()  > curAction->getAction()))) 
            {
               /* Action is better than current chosen one */
               curTeamPlayer = bestAction[i].getActor();
               curAction = &bestAction[i];
            }
         }

         if(curTeamPlayer == NULL)
         {
            //TODO - The AI couldn't select an action. Must do something dumb.
            lastAction.set(ACTION_NONE, NULL, Ogre::Vector3(0.0f, 0.0f, 0.0f),
                  NULL);

            Ogre::LogManager::getSingleton().logMessage(
                  "TODO: do something dumb!", Ogre::LML_CRITICAL);
         }
         else
         {
            lastAction.set(*curAction);

            calculateForceVector(curAction);
         }

      }
      /* Reset for next turn. */
      curStep = STEP_INITIAL;
      break;
   }
}

/***************************************************************************
 *                           selectPotentialDisks                          *
 ***************************************************************************/
void DecourtAI::selectPotentialDisks()
{
   int curDisks = 0;
   Ogre::Real x = Rules::getBall()->getPosition().x;
   Ogre::Real z = Rules::getBall()->getPosition().z;

   //TODO: if last action defined a 'better act disk', for example,
   //advanceWithBall should continue to act, sideOpen should define
   //its 'opened' target as desired one, pass idem, etc.

   /* Check if have a disk acting */
   if(curTeam->getLastActiveTeamPlayer() != NULL)
   {
      /* Let's see if it has remaining moves. */
      if(Rules::getRemainingTouches(curTeam->getLastActiveTeamPlayer()) > 0)
      {
         /* Select it to potential act. */
         electedDisk[curDisks] = curTeam->getLastActiveTeamPlayer();
         curDisks++;
      }
   }

   /* Get the remaining disks, nearest to the ball */
   std::vector<TeamPlayer*> disks = curTeam->getPlayersNearestFirst(x, z);
   std::vector<TeamPlayer*>::iterator it = disks.begin();
   while(curDisks < MAX_ELEGIBLE_DISKS)
   {
      if(it != disks.end())
      {
         TeamPlayer* tp = *it;

         if( (tp != curTeam->getLastActiveTeamPlayer()) &&
             (Rules::getRemainingTouches(tp) > 0) )
         {
            /* Elect it */
            electedDisk[curDisks] = tp;
            curDisks++;
         }

         /* Must check next available disk */
         it++;
      }
      else 
      {
         /* No more available disks */
         electedDisk[curDisks] = NULL;
         curDisks++;
      }
   }
}

/***************************************************************************
 *                              checkAction                                *
 ***************************************************************************/
const DecourtAI::ActionInfo DecourtAI::checkAction(BtSoccer::TeamPlayer* tp)
{
   ActionInfo resAction;
   Ogre::Vector2 diskPos(tp->getPosition().x, tp->getPosition().z);
   bool isUpper = (curTeam == Rules::getUpperTeam());
   Ogre::Vector2 ballPos(Rules::getBall()->getPosition().x,
                         Rules::getBall()->getPosition().z);
   Ogre::Vector2 directionToBall = ballPos - diskPos;
   directionToBall.normalise();


   /* Check ball relative position to disk */
   int ballRelativePos = Rules::getBall()->getRelativePositionToDisk(diskPos, 
         isUpper);
   if(shouldTurnAround(tp, ballPos, directionToBall, isUpper, ballRelativePos,
            resAction))
   {
      /* ACTION_TURN_AROUND */
      return resAction;
   }
  
   /* Check advance */ 
   if(shouldAdvanceWithBall(tp, ballPos, directionToBall, isUpper, 
            ballRelativePos, resAction))
   {
      /* ACTION_ADVANCE */
      return resAction;
   }

   /* Check goal shoot */
   if(shouldTryGoalShoot(tp, ballPos, directionToBall, isUpper, resAction))
   {
      /* ACTION_SHOOT_TO_GOAL */
      return resAction;
   }

   /* Check side opening */
   if(shouldSideOpening(tp, ballPos, directionToBall, isUpper, resAction))
   {
      /* ACTION_SIDE_OPENING */
      return resAction;
   }

   /* Check pass */
   if(shouldPass(tp, ballPos, directionToBall, isUpper, resAction))
   {
      /* ACTION_PASS */
      return resAction;
   }

   /* None of the above: should try going to a better position */
   //TODO: select a better position!
   resAction.set(ACTION_GO_TO_BETTER_POSITION, tp, 
         Ogre::Vector3(tp->getPosition().x + (isUpper) ? -10 : 10,
            0.0f, tp->getPosition().z), NULL);

   return resAction;
}

/***************************************************************************
 *                           shouldTurnAround                              *
 ***************************************************************************/
bool DecourtAI::shouldTurnAround(BtSoccer::TeamPlayer* tp, 
      Ogre::Vector2 ballPos, Ogre::Vector2 directionToBall, bool isUpper,
      int relativeBallPos, ActionInfo& info)
{
   if(relativeBallPos == Ball::BALL_BEHIND)
   {
      /* Will try to touch the ball almost tangently, trying to stop with 
       * the bal just ahead us */
      float ballRadius = Rules::getBall()->getSphere().getRadius();
      float ballZ;
      if(tp->getPosition().z > ballPos[1])
      {
         ballZ = ballPos[1] + ballRadius; 
      }
      else
      {
         ballZ = ballPos[1] - ballRadius; 
      }

      Ogre::Vector2 diskPos(tp->getPosition().x, tp->getPosition().z);
      Ogre::Vector2 tangPos(ballPos[0], ballZ - tp->getSphere().getRadius());
      Ogre::Vector2 tangDir = tangPos - diskPos;

      /* Calculate distance to tangent position */
      float dist = Ogre::Math::Sqrt(tangDir[0] * tangDir[0] + 
            tangDir[1] * tangDir[1]);

      /* Normalise tangent direction */
      tangDir.normalise();

      /* Let's define the target disk position: note that a bit further
       * than the calculated tangent position, trying to be with the ball
       * ahead. */
      Ogre::Vector2 tgtPos = diskPos + (tangDir * (dist + 10.0f));

      info.set(ACTION_TURN_AROUND, tp,
            Ogre::Vector3(tgtPos[0], 0.0f, tgtPos[1]), NULL);

      return true;
   }

   return false;
}


/***************************************************************************
 *                          isTooNearToAdvance                             *
 ***************************************************************************/
bool DecourtAI::isTooNearToAdvance(bool isUpper, const Ogre::Vector2& ballPos)
{
   Field* field = Rules::getField();

   Ogre::Vector2 halfSize = field->getHalfSize();
   Ogre::Vector2 littleAreaDelta = field->getLittleAreaDelta();

   if(isUpper)
   {
      return (ballPos[0] < -halfSize.x + (1.2f * littleAreaDelta[0]));
   }
   return (ballPos[0] > halfSize.x - (1.2f * littleAreaDelta[0]));
}

/***************************************************************************
 *                         shouldAdanceWithBall                            *
 ***************************************************************************/
bool DecourtAI::shouldAdvanceWithBall(BtSoccer::TeamPlayer* tp, 
      Ogre::Vector2 ballPos, Ogre::Vector2 directionToBall, bool isUpper,
      int relativeBallPos, ActionInfo& info)
{
   if(relativeBallPos != Ball::BALL_AHEAD)
   {
      /* Should not try to advance with ball if it isn't ahead of player */
      return false;
   }

   /* Check if have more than one remaining move */
   if(Rules::getRemainingTouches(tp) <= 1)
   {
      /* Shouldn't advance as won't be able to do anything on next turn. */
      return false;
   }

   /* if too near to opponent's goal, should not advance */
   if(isTooNearToAdvance(isUpper, ballPos))
   {
      return false;
   }

   /* Check if target position is before opponent's penalty area */
   Ogre::Vector2 targetBallPos = ballPos + 
      BALL_ADVANCE_DISTANCE * directionToBall;
#if BTSOCCER_DEBUG_AI
   Ogre::Log::Stream stream = Ogre::LogManager::getSingleton().stream(
         Ogre::LML_NORMAL);
   stream << "\n\nPlayer: " << tp->getPosition().x << ", " 
          <<  tp->getPosition().z << " Ball: " <<  ballPos[0] << ", " 
          << ballPos[1] << " targetBallPos: " << targetBallPos[0] << ", "
          << targetBallPos[1] << "\n\n";
#endif
   if(Rules::getField()->isInnerPenaltyArea(targetBallPos[0], targetBallPos[1],
            !isUpper, isUpper))
   {
      /* Ball target position is inner the penalty area of oponents team,
       * we must not advance with this disk. */
      return false;
   }

   /* Check if have free area to ball */
   if(!checkFreeAreaToBall(tp, ballPos))
   {
      /* Way to the ball is blocked: we must not advance. */
      return false;
   }
   tp->pointTarget(Ogre::Vector3(targetBallPos[0], 1.0f, targetBallPos[1]));

   /* Check if have free area ahead the ball to its target position */
   if(!Rules::getBall()->hasFreeWayTo(query, 
            targetBallPos[0], targetBallPos[1]))
   {
      return false;
   }

   /* We can advance! */
   info.set(ACTION_ADVANCE, tp, 
         Ogre::Vector3(targetBallPos[0], 0.0f, targetBallPos[1]), NULL);
   return true;
}

/***************************************************************************
 *                          checkFreeAreaToBall                            *
 ***************************************************************************/
bool DecourtAI::checkFreeAreaToBall(BtSoccer::TeamPlayer* tp, 
            Ogre::Vector2 ballPos)
{
   if(!calculatedCurrentDiskFreeAreaToBall)
   {
      calculatedCurrentDiskFreeAreaToBall = true;
      hasFreeAreaToBall = tp->hasFreeWayTo(query, ballPos[0], ballPos[1]);
   }

#if BTSOCCER_DEBUG_AI
   if(hasFreeAreaToBall)
   {
      printf("%s has free area to ball!\n", tp->getName().c_str());
   }
#endif

   return hasFreeAreaToBall;
}

/***************************************************************************
 *                          shouldTryGoalShoot                             *
 ***************************************************************************/
bool DecourtAI::shouldTryGoalShoot(BtSoccer::TeamPlayer* tp,
      Ogre::Vector2 ballPos, Ogre::Vector2 directionToBall, bool isUpper,
      ActionInfo& info)
{
#if BTSOCCER_DEBUG_AI
   Ogre::Log::Stream stream = Ogre::LogManager::getSingleton().stream(
         Ogre::LML_NORMAL);
#endif

   /* Check if have free way to the ball */
   if(!checkFreeAreaToBall(tp, ballPos))
   {
      /* If path to the ball is blocked, must not try a shoot. */
#if BTSOCCER_DEBUG_AI
      stream << "Shouldn't shoot: ball is blocked!\n";
#endif
      return false;
   }

   /* Check if ball position to goal is favorable */
   if(Rules::getField()->getNearGoalFactor(isUpper, ballPos[0], 
            ballPos[1]) < BALL_MIN_NEAR_GOAL_FACTOR)
   {
      /* Ball position is not favorable to shoot. */
#if BTSOCCER_DEBUG_AI
      stream << "Ball position isn't favorable. Factor: "
             << Rules::getField()->getNearGoalFactor(isUpper, ballPos[0],
                ballPos[1]) << "\n";
#endif
      return false;
   }

   /* Check if was advancing with ball and have just one action remaining */
   if((lastAction.getAction() == ACTION_ADVANCE) &&
      (lastAction.getActor() == tp) && 
      (Rules::getRemainingTouches(tp) == 1))
   {
      /* Should shoot */
      info.set(ACTION_SHOOT_TO_GOAL, tp, 
            Ogre::Vector3(0.0f, 0.0f, 0.0f), NULL); 
      return true;
   }

   /* Check if angle to touch ball is favorable:
    * It's a good angle to shoot if the direction to ball could send
    * the ball not too far away from the goal (so, with a little adjustment
    * to the touching angle, the disk can send it to the target goal) */
   Ogre::Real n = Rules::getField()->getByline(isUpper) / directionToBall[0];
   Ogre::Real bylineBallZ = directionToBall[1] * n;
   if( (n < 0) && (Ogre::Math::Abs(bylineBallZ) > BALL_MIN_VALID_Z_DISTANCE) )
   {
      /* Either the ball is behind disk (shouldn't) or the usual angle to 
       * direct touch it will send it too far away from the goal center (0).*/
#if BTSOCCER_DEBUG_AI
      stream << "Shouldn't goal shoot. n: " << n << "bylineBallZ: " 
             << bylineBallZ << "\n";
#endif
      return false;
   }

   /* Seems like it's a good moment to shoot! */
   info.set(ACTION_SHOOT_TO_GOAL, tp, Ogre::Vector3(0.0f, 0.0f, 0.0f), NULL); 
   return true;
}

/***************************************************************************
 *                            getMostAdvancedX                             *
 ***************************************************************************/
Ogre::Real DecourtAI::getMostAdvancedX(Ogre::Real x1, Ogre::Real x2,
      Ogre::Real x3, bool isUpper) 
{
   /* Let's apply upper factor */
   int m = (isUpper) ? -1 : 1;
   x1 *= m;
   x2 *= m;
   x3 *= m;

   if((x1 >= x2) && (x1 >= x3))
   {
      return x1;
   }
   else if((x2 >= x3) && (x2 >= x1))
   {
      return x2;
   }

   return x3;
}

/***************************************************************************
 *                          shouldSideOpening                              *
 ***************************************************************************/
bool DecourtAI::shouldSideOpening(BtSoccer::TeamPlayer* tp, 
      Ogre::Vector2 ballPos, Ogre::Vector2 directionToBall, bool isUpper,
      ActionInfo& info)
{
   /* Check if have free way to the ball */
   if(!checkFreeAreaToBall(tp, ballPos))
   {
      /* If path to the ball is blocked, must not try to touch it. */
      return false;
   }

   /* Let's check, within the nearest disks, if one is elegible for a side
    * opening: ie., if it has free area ahead and if is possible to touch
    * the ball and send it to this free area. */

   /* Get nearest disks */
   Ogre::Vector3 pos = tp->getPosition();
   std::vector<TeamPlayer*> disks = tp->getTeam()->getPlayersNearestFirst(
         pos.x, pos.z);
   std::vector<TeamPlayer*>::iterator it = disks.begin();
   while(it != disks.end())
   {
      TeamPlayer* curDisk = *it;
      if(curDisk != tp)
      {
         /* Check if the disk is not too much ahead of the potential actor */
         if((curDisk->getPosition().x - tp->getPosition().x) <= 10.0f)
         {
            /* Check if disk and ball are at the same side of potential actor */
            Ogre::Real sideDisk = curDisk->getPosition().z - 
                                     tp->getPosition().z;
            if(((sideDisk >= 0) && (directionToBall[1] >= 0)) ||
               ((sideDisk < 0) && (directionToBall[1] < 0)))
            {
               /* define direction signals */
               int m = (isUpper) ? -1 : 1;

               /* Check if disk has free front area to potential ball
                * target position (upper team attack to negative side). */
                /* Note: target disk is relative to most advanced element
                 * (disk or ball), to always do an agressive side open. */
                Ogre::Real tgtX = getMostAdvancedX(tp->getPosition().x,
                   ballPos.x, curDisk->getPosition().x, isUpper) + 
                   m * (curDisk->getSphereRadius() + BALL_ADVANCE_DISTANCE);
                Ogre::Real tgtZ = curDisk->getPosition().z;
                if(curDisk->hasFreeWayTo(query, tgtX, tgtZ))
                {
                   /* Should do a side opening to this disk. */
                   //TODO: set target disk and act with it next turn.
                   info.set(ACTION_SIDE_OPENING, tp, 
                         Ogre::Vector3(tgtX, 0.0f, tgtZ), curDisk); 
                   return true;
                }
            }
         }
      }
      it++;
   }

   return false;
}

/***************************************************************************
 *                              shouldPass                                 *
 ***************************************************************************/
bool DecourtAI::shouldPass(BtSoccer::TeamPlayer* tp, Ogre::Vector2 ballPos,
      Ogre::Vector2 directionToBall, bool isUpper, ActionInfo& info) 
{
   if(!checkFreeAreaToBall(tp, ballPos))
   {
      /* If path to the ball is blocked, must not try to touch it. */
      return false;
   }

   /* Get disks by distance to potential actor */
   Ogre::Vector3 pos = tp->getPosition();
   std::vector<TeamPlayer*> disks = tp->getTeam()->getPlayersNearestFirst(
         pos.x, pos.z);

   /* Check potential pass to those ahead. */
   std::vector<TeamPlayer*>::iterator it = disks.begin();
   while(it != disks.end())
   {
      TeamPlayer* curDisk = *it;
      if(curDisk != tp)
      {
         /* Check if is ahead */
         bool isAhead = false;
         if(isUpper)
         {
            isAhead = (curDisk->getPosition().x - tp->getPosition().x) <= 0;
         }
         else
         {
            isAhead = (curDisk->getPosition().x - tp->getPosition().x) >= 0;
         }

         if(isAhead)
         {
            /* Check if angle to touch ball is favorable. The check is similar
             * to the above angle check at shouldGoalShoot: verify if the
             * directionToBall touch will not send the ball too far away
             * from the disk at the z=curDisk.z line. */
            Ogre::Real n = curDisk->getPosition().z / directionToBall[1];
            Ogre::Real tgtX = directionToBall[0] * n;
            if( (n >= 0) && 
                (Ogre::Math::Abs(tgtX - curDisk->getPosition().x) <= 
                 BALL_MIN_VALID_Z_DISTANCE) )
            {
               /* Check if ball target area is free */
               Ball* ball = Rules::getBall();
               if(ball->hasFreeWayTo(query, tgtX, curDisk->getPosition().z))
               {
                  /* TODO: Act with target disk on next turn! */
                  info.set(ACTION_PASS, tp, 
                        Ogre::Vector3(tgtX, 0.0f, curDisk->getPosition().z),
                        curDisk);
                  return true;
               }
            }
         }
      }
      it++;
   }
   
   return false;
}

/***************************************************************************
 *                              debugDraw                                  *
 ***************************************************************************/
void DecourtAI::debugDraw()
{
#if BTSOCCER_DEBUG_AI && BTSOCCER_RENDER_DEBUG
   if(lastAction.getActor() != NULL)
   {
      BulletDebugDraw* debugDraw = lastAction.getActor()->getDebugDraw();
      Ogre::Vector3 targetPos = lastAction.getPos();

      /* Blue target (or related) mark */
      debugDraw->drawLine(
            Ogre::Vector3(targetPos.x - 10, 2.0f, targetPos.z - 10),
            Ogre::Vector3(targetPos.x + 10, 2.0f, targetPos.z + 10),
            Ogre::Vector3(0.2f, 0.2f, 0.8f));
      debugDraw->drawLine(
            Ogre::Vector3(targetPos.x - 10, 2.0f, targetPos.z + 10),
            Ogre::Vector3(targetPos.x + 10, 2.0f, targetPos.z - 10),
            Ogre::Vector3(0.2f, 0.2f, 0.8f));

      /* Red line from initial actor position to mark */
      Ogre::Vector3 initPos = lastAction.getActorInitialPos();
      debugDraw->drawLine(Ogre::Vector3(targetPos.x, 2.0f, targetPos.z),
            Ogre::Vector3(initPos.x, 2.0f, initPos.z),
            Ogre::Vector3(0.8f, 0.2f, 0.2f));

   }
#endif
}

