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

#include "fuzzyai.h"
#include "../engine/ball.h"
#include "../engine/field.h"
#include "../engine/rules.h"

#define FLOAT_DELTA 0.1f /* A delta for calculated float comparasions */
/*! Delta for target X position when sending a ball to the disk */
#define SEND_BALL_TO_DISK_DELTA 20.0f

/* For Debug only */
Ogre::String fuzzyActNames[]={"pass", "shoot", "throw away", "block", "foul",
   "go to position", "advance with ball"};

namespace BtSoccer
{

/***********************************************************************
 *                         FuzzyAction::Constructor                    *
 ***********************************************************************/
FuzzyAction::FuzzyAction()
{
   clear();
}

/***********************************************************************
 *                         FuzzyAction::Destructor                     *
 ***********************************************************************/
FuzzyAction::~FuzzyAction()
{
}

/***********************************************************************
 *                                clear                                *
 ***********************************************************************/
void FuzzyAction::clear()
{
   int i;

   /* Set all percentuals to 0 */
   percentual = 0.0f;
   for(i=0; i < 10; i++)
   {
      toDiskPercentual[i] = 0.0f;
   }
   percDisk = -1;
}

/***********************************************************************
 *                           setPercentual                             *
 ***********************************************************************/
void FuzzyAction::setPercentual(int disk, float perc)
{
   toDiskPercentual[disk] = perc;

   /* Verify if is the best percentual */
   if(perc > percentual)
   {
      percentual = perc;
      percDisk = disk;
   }
}

/***********************************************************************
 *                           setPercentual                             *
 ***********************************************************************/
void FuzzyAction::setPercentual(float perc)
{
   percentual = perc;
   percDisk = -1;
}

/***********************************************************************
 *                           getPercentual                             *
 ***********************************************************************/
float FuzzyAction::getPercentual()
{
   return(percentual);
}

/***********************************************************************
 *                           getRelativeDisk                           *
 ***********************************************************************/
int FuzzyAction::getRelativeDisk()
{
   return(percDisk);
}

/***********************************************************************
 *                          FuzzyDisk::Constructor                     *
 ***********************************************************************/
FuzzyDisk::FuzzyDisk()
{
   clear();
   disk = NULL;
}

/***********************************************************************
 *                          FuzzyDisk::Destructor                      *
 ***********************************************************************/
FuzzyDisk::~FuzzyDisk()
{
}

/***********************************************************************
 *                             setFuzzyAI                              *
 ***********************************************************************/
void FuzzyDisk::setFuzzyAI(FuzzyAI* fuzzyAI)
{
   ai = fuzzyAI;
}

/***********************************************************************
 *                                clear                                *
 ***********************************************************************/
void FuzzyDisk::clear()
{
   int i;

   for(i=0; i < TOTAL_ACTIONS; i++)
   {
      actions[i].clear();
   }
   ballPosition = BALL_UNKNOWN;
   ballBlocked = false;
   angleToBall = 0;
   ballDistance = 10000.0f;
   firstCollide = "";
   nearGoal = 0.0f;
   upperTeam = false;
}

/***********************************************************************
 *                       calculateBallPosition                         *
 ***********************************************************************/
void FuzzyDisk::calculateBallPosition(Ogre::RaySceneQuery* query)
{
   Ogre::Vector3 diskPos = disk->getPosition();

   /* Calculate angle between ball and disk */
   Ball* b = Rules::getBall();
   Ogre::Vector3 ballPos = b->getPosition();
   Ogre::Vector2 pos(diskPos.x, diskPos.z);

   /* Calculate ball distance to disk */
   Ogre::Vector2 delta = Ogre::Vector2(ballPos.x - diskPos.x, 
                                       ballPos.z - diskPos.z);
   ballDistance = Ogre::Math::Sqrt( delta[0]*delta[0] + 
                                    delta[1]*delta[1] );

   /* Check ball relative position to disk */
   ballPosition = b->getRelativePositionToDisk(pos, upperTeam);

   /* Must verify now if the path to ball is blocked or not */
   Ogre::Vector3 origin(diskPos.x, 1.0, diskPos.z);
   Ogre::Vector3 dir(ballPos.x - diskPos.x, 0.0, ballPos.z - diskPos.z);
   dir.normalise();

   Ogre::Ray ray(origin, dir);
   query->setRay(ray);
   query->setSortByDistance(true);

   Ogre::RaySceneQueryResult &result = query->execute();
   Ogre::RaySceneQueryResult::iterator itr;

   bool done = false;
   for( itr = result.begin(); ((itr != result.end()) && (!done)); itr++ )
   {
      if(itr->movable)
      {
         if(itr->movable->getName() == b->getName())
         {
            /* Found ball, must end */
            done = true;
         }
         else if(itr->movable->getName() != disk->getName())
         {
            /* Set first collided, if none */
            if(firstCollide.empty())
            {
               firstCollide = itr->movable->getName();

               /* Change ball position as blocked */
               ballBlocked = true;
               done = true;
            }
         }
      }
   }
}

/***********************************************************************
 *                         calculateNearGoal                           *
 ***********************************************************************/
void FuzzyDisk::calculateNearGoal(Field* field)
{
   nearGoal = field->getNearGoalFactor(upperTeam,
         disk->getPosition().x, disk->getPosition().z);
}

/***********************************************************************
 *                         calculateActions                            *
 ***********************************************************************/
void FuzzyDisk::calculateActions(Ogre::RaySceneQuery* query)
{
   int i;

   /* FIXME: must verify if disk have more moves! */

   /* TODO: if remaining global moves is 1, must shoot or block. */

   /* TODO: if ball inner own area direct act on ball instead of using disk. */

   for(i=0; i < TOTAL_ACTIONS; i++)
   {
      switch(i)
      {
         case ACTION_PASS:
            calculatePassFactor(query);
         break;
         case ACTION_SHOOT:
            calculateShootFactor();
         break;
         case ACTION_THROW_AWAY:
            calculateThrowAwayFactor();
         break;
         case ACTION_BLOCK:
            calculateBlockFactor();
         break;
         case ACTION_FOUL:
            calculateFoulFactor();
         break;
         case ACTION_GO_TO_POSITION:
            calculateGoToBetterPosFactor();
         break;
         case ACTION_ADVANCE_WITH_BALL:
            calculateAdvanceWithBallFactor();
         break;
      }
   }
}

/***********************************************************************
 *                      calculateNearBallFactor                        *
 ***********************************************************************/
float FuzzyDisk::calculateNearBallFactor(float near, float far)
{
   float nearBall = 0.0f;
   float delta = far - near;

   if(ballDistance <= near)
   {
      nearBall = 1.0f;
   }
   else if(ballDistance <= far)
   {
      nearBall = (delta-(ballDistance-near)) / delta;
   }

   return(nearBall);
}

/***********************************************************************
 *                       calculateAngleFactor                          *
 ***********************************************************************/
float FuzzyDisk::calculateAngleFactor(float angle)
{
   printf("BallPos: %d Angle: %.3f\n", ballPosition, angle);
   /* FIXME: to this function make logical sense. */
   float angleFactor = 0.0f;

   return angleFactor;
}

/***********************************************************************
 *                       calculateShootFactor                          *
 ***********************************************************************/
void FuzzyDisk::calculateShootFactor()
{
   Ball* ball = Rules::getBall();
   Ogre::Vector3 dPos = disk->getPosition();
   Ogre::Vector2 sideDelta = Rules::getField()->getSideDelta();
   Ogre::Real fieldX = Rules::getField()->getHalfSize().x;

   /* Shoot is exclusive for ball AHEAD (obviously), and not blocked */
   if((ballPosition == Ball::BALL_AHEAD) && (!ballBlocked))
   {
      /* Get goal center value */
      Ogre::Vector2 goalCenter;
      goalCenter[1] = 0;
      if(upperTeam)
      {
         goalCenter[0] = -fieldX+sideDelta[0];
      }
      else
      {
         goalCenter[0] = fieldX-sideDelta[0];
      }
      /* Calculate the angle of shoot to the goal center */
      Ogre::Vector2 colDisk, colPoint, ballDir;
      ball->calculateCollisionPos(disk, goalCenter, colPoint, 
            colDisk, ballDir);
      Ogre::Vector2 diskPos(dPos.x, dPos.z);

      /*printf("\nBall: %.3f %.3f\n", ball->getPosX(), ball->getPosZ());
      printf("Disk: %.3f %.3f\n", disk->getPosX(), disk->getPosZ());
      printf("Goal: %.3f %.3f\n", goalCenter[0], goalCenter[1]);
      printf("TgtDisk: %.3f %.3f\n", colDisk[0], colDisk[1]);*/
      //FIXME: angle is probably wrong!
      float angleToGoal = diskPos.angleTo(colDisk).valueDegrees();
      float angleFactor = calculateAngleFactor(angleToGoal);
      //printf("angleToGoal: %.3f\n", angleToGoal);
      //printf("angleFactor: %.3f\n", angleFactor);

      /* Calculate distance to ball factor */ 
      float nearBall = calculateNearBallFactor(20.0f, 100.0f);

      /* Calculate percentual (limited to 0.8f) */
      float f = 0.0f;
      f += nearGoal*0.5f;
      f += angleFactor*0.2f;
      f += nearBall*0.1f;

      /* Set it */
      actions[ACTION_SHOOT].setPercentual(f);
   }
}

/***********************************************************************
 *                       calculatePassFactor                           *
 ***********************************************************************/
void FuzzyDisk::calculatePassFactor(Ogre::RaySceneQuery* query)
{
   int i;
   Ball* b = Rules::getBall();
   Field* f = Rules::getField();
   float factor, nearBall, angleFactor;
   Ogre::Vector2 diskPos, colDisk, colPoint, ballDir;

   /* Set Disk position */
   diskPos[0] = disk->getPosition().x;
   diskPos[1] = disk->getPosition().z;

   /* Only need to calculate pass if ball isn't blocked */
   if(!ballBlocked)
   {
      /* Verify each possible target disk */
      for(i=0; i < f->getNumberOfDisks(); i++)
      {
         /* TODO: Verify if target disk have free front area */

         /* If ball ahead and disk behind, it's physically impossible
          * to pass, so must ignore the target disk. */
         bool ballAheadAndDiskBehind = (ballPosition == Ball::BALL_AHEAD) &&
               (disk->getPosition()[0] + SEND_BALL_TO_DISK_DELTA < 
                disks[i].disk->getPosition()[0]);
         /* Analog situation if ball behind and disk ahead. */
         bool ballBehindAndDiskAhead = (!ballAheadAndDiskBehind) &&
               (ballPosition == Ball::BALL_BEHIND) &&
               (disk->getPosition()[0] + SEND_BALL_TO_DISK_DELTA > 
                disks[i].disk->getPosition()[0]);

         /* Also, no need to pass to itself! */
         if( (disks[i].disk != disk) && (!ballAheadAndDiskBehind) && 
             (!ballBehindAndDiskAhead) )
         {
            /* Not the same, probably can pass the ball to */

            /* Near Ball Factor */
            nearBall = calculateNearBallFactor(20.0f, 100.0f);

            /* Calculate angle to 'touch ball and send it to target disk' */
            b->calculateCollisionPos(disk, Ogre::Vector2(
                     disks[i].disk->getPosition().x, 
                     disks[i].disk->getPosition().z),
                     colPoint, colDisk, ballDir);
            angleFactor = calculateAngleFactor(
                  ai->getAngleBetweenPositions(diskPos, colDisk));

            /* Calculate percentual */
            factor = disks[i].nearGoal*0.5f;
            factor += nearBall*0.25f;
            factor += angleFactor*0.25f;

            if(ballPosition == Ball::BALL_BEHIND) 
            {
               /* Must avoid, somewhat, passing 'back'. */
               factor *= 0.8f;
            }

            actions[ACTION_PASS].setPercentual(i, factor);
         }
      }
   }
}

/***********************************************************************
 *                      calculateThrowAwayFactor                       *
 ***********************************************************************/
void FuzzyDisk::calculateThrowAwayFactor()
{
   int i;
   float factor;
   int totalDisks = Rules::getField()->getNumberOfDisks();

   /* Only need to throw away if ball is behind or at side */
   if( ( (ballPosition == Ball::BALL_BEHIND) || 
         (ballPosition == Ball::BALL_AT_SIDE) ) && 
       (!ballBlocked))
   {
      /* Verify each possible enemy disk ball position
       * and near goal factor. */
      for(i=0; i < totalDisks; i++)
      {
         factor = enemies[i].nearGoal * 0.25f;
         if( (enemies[i].ballPosition == Ball::BALL_AHEAD) &&
             (!enemies[i].ballBlocked) )
         {
            factor *= 1.05f;
         }
         actions[ACTION_THROW_AWAY].setPercentual(i, factor);
      }
   }
}

/***********************************************************************
 *                        calculateBlockFactor                         *
 ***********************************************************************/
void FuzzyDisk::calculateBlockFactor()
{
   int i;
   float factor;
   int totalDisks = Rules::getField()->getNumberOfDisks();
   float diskRadius = disk->getSphere().getRadius();

   /* Only need to block if ball is behind or at side */
   if( (ballPosition == Ball::BALL_BEHIND) || 
       (ballPosition == Ball::BALL_AT_SIDE) )
   {
      /* Verify each possible enemy disk ball position
       * and near goal factor. */
      for(i=0; i < totalDisks; i++)
      {
         /* Make sure disk can be between enemy and the ball */
         if(enemies[i].ballDistance >= 3*diskRadius)
         {
            /* max of 0.5f probability */
            factor = enemies[i].nearGoal * 0.4;
            if( (enemies[i].ballPosition == Ball::BALL_AHEAD) &&
                (!enemies[i].ballBlocked) )
            {
               factor += 0.1;
            }
            actions[ACTION_BLOCK].setPercentual(i, factor);
         }
      }
   }
}

/***********************************************************************
 *                         calculateFoulFactor                         *
 ***********************************************************************/
void FuzzyDisk::calculateFoulFactor()
{
   int i;
   float factor;
   int totalDisks = Rules::getField()->getNumberOfDisks();

   /* FIXME: Verify if enemy not in little area! */

   /* TODO: Hard mode: only do fouls far from goal. */

   /* Only need to block if ball is behind or at side */
   if( (ballBlocked) &&
       ( (ballPosition == Ball::BALL_AHEAD) || 
         (ballPosition == Ball::BALL_AT_SIDE) ||
         (ballPosition == Ball::BALL_BEHIND) ) )
   {
      /* Verify each possible enemy disk ball position
       * and near goal factor. */
      for(i=0; i < totalDisks; i++)
      {
         /* max of 0.35f probability */
         factor = enemies[i].nearGoal * 0.3;
         if( (enemies[i].ballPosition == Ball::BALL_AHEAD) &&
             (!enemies[i].ballBlocked) )
         {
            factor += 0.05;
         }
         actions[ACTION_FOUL].setPercentual(i, factor);
      }
   }
}



/***********************************************************************
 *                   calculateGoToBetterPosFactor                      *
 ***********************************************************************/
void FuzzyDisk::calculateGoToBetterPosFactor()
{
   /* Only applicable when ball is blocked or behind */
   if( (ballBlocked) || (ballPosition == Ball::BALL_BEHIND) )
   {
      /* Note: GO_TO_POSITION factor is limited to 0.14f */
      actions[ACTION_GO_TO_POSITION].setPercentual(0.14f);
   }
}

/***********************************************************************
 *                     calculateAdvanceWithBall                        *
 ***********************************************************************/
void FuzzyDisk::calculateAdvanceWithBallFactor()
{
   float factor;
   /* FIXME: must verify remaining actions */

   /* Only applicable when ball isn't blocked and not behind */
   if( (!ballBlocked) && (ballPosition != Ball::BALL_BEHIND) )
   {
      /* Note: ADVANCE_WITH_BALL factor is limited to 0.6f */
      factor = calculateNearBallFactor(16.0f, 60.0f) * 0.6f;
      actions[ACTION_ADVANCE_WITH_BALL].setPercentual(factor);
   }
}

/***********************************************************************
 *                            getBestAction                            *
 ***********************************************************************/
std::pair <int, float> FuzzyDisk::getBestAction()
{
   int i;
   std::pair <int, float> res;
   res.first = 0;
   res.second = actions[0].getPercentual();
   for(i=1; i < TOTAL_ACTIONS; i++)
   {
      if(actions[i].getPercentual() > res.second)
      {
         res.first = i;
         res.second = actions[i].getPercentual();
      }
   }
   return(res);
}

/***********************************************************************
 *                              Constructor                            *
 ***********************************************************************/
FuzzyAI::FuzzyAI(BtSoccer::Team* t, Field* f):BaseAI(t, f)
{
   int i;
   
   defined = true;

   query = t->getSceneManager()->createRayQuery(Ogre::Ray());

   /* Set pointers */
   for(i=0 ; i<10; i++)
   {
      disk[i].setFuzzyAI(this);
      disk[i].disk = t->getDisk(i);
      disk[i].enemies = &enemyDisk[0];
      disk[i].disks = &disk[0];
      enemyDisk[i].enemies = &disk[0];
      enemyDisk[i].disks = &enemyDisk[0];
   }
}

/***********************************************************************
 *                               Destructor                            *
 ***********************************************************************/
FuzzyAI::~FuzzyAI()
{
   curTeam->getSceneManager()->destroyQuery(query);
}

/***********************************************************************
 *                            calculateStep                            *
 ***********************************************************************/
void FuzzyAI::calculateStep()
{
   int i;
   Field* field = Rules::getField();
   int totalDisks = Rules::getField()->getNumberOfDisks();

   /* Verify if is the first call on a new turn */
   if( (defined) && (!curTeamPlayer) )
   {
      Team* enemy = Rules::getOtherTeam(curTeam);
      /* Must clear, as called on a new turn */
      defined = false;
      curDisk = 0;
      for(i=0; i < totalDisks; i++)
      {
         disk[i].clear();

         disk[i].upperTeam = (curTeam == Rules::getUpperTeam());
         /* Pre-calculate each disk "nearess" on field */
         disk[i].calculateNearGoal(field);
         disk[i].calculateBallPosition(query);

         enemyDisk[i].clear();
         enemyDisk[i].disk = enemy->getDisk(i);
         enemyDisk[i].upperTeam = (curTeam != Rules::getUpperTeam());
         enemyDisk[i].calculateNearGoal(field);
         enemyDisk[i].calculateBallPosition(query);
      }
      /* Do the forced physics step */
      BulletLink::forcedStep();
      
      /* Done for this step */
      return;
   }

   /* Do a single step: calculate each action "worth to-do" percentual */
   if(Rules::getRemainingTouches(disk[curDisk].disk) > 0)
   {
      /* Can act, calculate action percentuals */
      disk[curDisk].calculateActions(query);
   } 
   else
   {
      /* Can't act, must clear the disk making sure not using it. */
      disk[curDisk].clear();
   }
   curDisk++;

   /* Verify if all steps done */
   if(curDisk == totalDisks)
   {
      /* Done. Must check which action will take  */
      int bestDisk=0;
      float bestPerc=0;
      int bestAction=0;
      std::pair <int, float>res;
      for(i=0; i < totalDisks; i++)
      {
         /* FIXME: Remove debug printf's */
         printf("Disk[%d]\n", i);
         printf("   BallPos: %d %s NearGoal: %.2f\n", 
               disk[i].ballPosition, ((disk[i].ballBlocked)?"Blocked":""),
               disk[i].nearGoal);
         printf("   PASS: %.2f SHT: %.2f GO: %.2f ADV: %.2f "
                "THW: %.2f BCK: %.2f FOL: %.2f\n", 
               disk[i].actions[FuzzyDisk::ACTION_PASS].getPercentual(),
               disk[i].actions[FuzzyDisk::ACTION_SHOOT].getPercentual(),
              disk[i].actions[FuzzyDisk::ACTION_GO_TO_POSITION].getPercentual(),
           disk[i].actions[FuzzyDisk::ACTION_ADVANCE_WITH_BALL].getPercentual(),
               disk[i].actions[FuzzyDisk::ACTION_THROW_AWAY].getPercentual(),
               disk[i].actions[FuzzyDisk::ACTION_BLOCK].getPercentual(),
               disk[i].actions[FuzzyDisk::ACTION_FOUL].getPercentual());

         res = disk[i].getBestAction();
         if(res.second > bestPerc)
         {
            bestDisk = i;
            bestAction = res.first;
            bestPerc = res.second;
         }
      }

      printf("\n\nWill do %s with disk %d for %d%% (tgt: %d)\n\n",
         fuzzyActNames[bestAction].c_str(), bestDisk, (int)(bestPerc*100.0f),
         disk[bestDisk].actions[bestAction].getRelativeDisk());
      /* Finally set the action to do */
      setAction(bestDisk, bestAction);
 
      defined = true;
   }
}

/***********************************************************************
 *                             setAction                               *
 ***********************************************************************/
void FuzzyAI::setAction(int dsk, int action)
{
   Ball* ball = Rules::getBall();
   Ogre::Vector3 ballPos = ball->getPosition();
   Ogre::Real fieldZ = Rules::getField()->getHalfSize()[1];

   /* First, set disk to act */
   curTeamPlayer = curTeam->getDisk(dsk);
   Ogre::Vector3 curDiskPos = curTeamPlayer->getPosition();
   /* Let's calculate force for each action type */
   switch(action)
   {
      case FuzzyDisk::ACTION_PASS:
      {
         /* Calculate force to send ball ahead the desired disk */
         int tgtDisk;
         Ogre::Vector3 var(SEND_BALL_TO_DISK_DELTA, 0.0f, 0.0f);
         tgtDisk=disk[dsk].actions[FuzzyDisk::ACTION_PASS].getRelativeDisk();

         if(disk[dsk].upperTeam)
         {
            /* Atacking down, ahead must be negative */
            var.x *= -1.0f;
         }
         /* Calculate force to send ahead target disk and stop */
         calculateForce(curTeamPlayer, ball, 
               disk[tgtDisk].disk->getPosition() + var, true);
      }
      break;
      case FuzzyDisk::ACTION_SHOOT:
      {
         tryGoalShoot = true;
         /* No need to calculate force, as it will be on the call to
          * "calculateGoalShoot" and, before that (and after this), 
          * the enemy goalKeeper position should change. */
      }
      break;
      case FuzzyDisk::ACTION_THROW_AWAY:
      {
         /* Will try to send the ball to the side */
         float mult;
         Ogre::Vector3 dir, tgt;
         dir = ball->getPosition() - curTeamPlayer->getPosition();
         dir.normalise();
         if(ballPos.z - curDiskPos.z >= 0)
         {
            /* Must send it to the positive side */
            mult = fieldZ - ballPos.z;
         }
         else
         {
            /* Must send it to the negative side */
            mult = -fieldZ - ballPos.z;
         }
         tgt = ball->getPosition() + (dir * mult);
         /* Send the ball to the side! */
         calculateForce(curTeamPlayer, ball, tgt, false);
      }
      break;
      case FuzzyDisk::ACTION_BLOCK:
      {
         /* Just send the disk to be between target disk and the ball */
         Ogre::Vector3 pos;
         pos = (ball->getPosition() + curTeamPlayer->getPosition()) / 2;
         //TODO: check if the position isn't occupied and also
         // check if the way isn't blocked by another player.
         calculateForce(curTeamPlayer, ball->getPosition());
      }
      break;
      case FuzzyDisk::ACTION_FOUL:
      {
         /* Just send the disk to make a foul under target disk */
         int tgtDisk = 
            disk[dsk].actions[FuzzyDisk::ACTION_FOUL].getRelativeDisk();
         calculateForce(curTeamPlayer, disk[tgtDisk].disk->getPosition());
      }
      break;
      case FuzzyDisk::ACTION_GO_TO_POSITION:
      {
         /* TODO: select a better position to go to; */
         /* Actually, going near the ball */
         calculateForce(curTeamPlayer, ball->getPosition());
      }
      break;
      case FuzzyDisk::ACTION_ADVANCE_WITH_BALL:
      {
         //FIXME: going to the ball position should be sufficient to
         //advance with it (empirically tested!).
         Ogre::Vector3 var(10.0f, 0.0f, 0.0f);
         if(disk[dsk].upperTeam)
         {
            /* Atacking down, ahead must be negative */
            var *= -1.0f;
         }
         /* Calculate force to send ball ahead the current disk */
         calculateForce(curTeamPlayer, ball, 
               curTeamPlayer->getPosition() + var, true);

      }
      break;
   }
}

/***********************************************************************
 *                         calculateGoalShoot                          *
 ***********************************************************************/
void FuzzyAI::calculateGoalShoot()
{
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

/***********************************************************************
 *                            doDiskPositon                            *
 ***********************************************************************/
void FuzzyAI::doDiskPosition(BtSoccer::TeamPlayer* tp)
{

   /* TODO */
   Ogre::Vector2 tgtGoal; /**< Target goal position */
   Ogre::Vector2 dir; /**< Calculated disk position */
   
   BtSoccer::Ball* ball = Rules::getBall();
   Ogre::Vector3 ballPos = ball->getPosition();
   Ogre::Real fieldX = Rules::getField()->getHalfSize().x;

   /* Calculate disk to ball minimun distance */
   float ballDist = 16.0f;

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

/***********************************************************************
 *                         doGoalKeeperPositon                         *
 ***********************************************************************/
void FuzzyAI::doGoalKeeperPosition(BtSoccer::GoalKeeper* gk)
{
   /* TODO */

   /* Calculate direction from the ball to the shooter */

   /* Set the goal keeper there */
}

}

