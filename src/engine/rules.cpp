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

#include "rules.h"

#include "ball.h"
#include "field.h"
#include "goalkeeper.h"
#include "team.h"
#include "teamplayer.h"
#include "../net/protocol.h"

#include <OGRE/OgreLogManager.h>

using namespace BtSoccer;

#define BALL_ACTION_NONE       0
#define BALL_ACTION_SIDE       1
#define BALL_ACTION_BYLINE     2
#define BALL_ACTION_GOAL       3

/**********************************************************************
 *                                clear                               *
 **********************************************************************/
void Rules::clear()
{
   /* Time */
   halfMinutes = 0;
   halfSeconds = 0;
   periodTimer.reset();

   /* Remaining Things */
   changedBallOwner = false;
   currentDisk = NULL;
   activeTeam = NULL;
   remainingGlobalTouches = maxRemainingGlobalTouches();
   remainingDiskTouches = 1; //Since middlefield kick
   lastBallCollided = NULL;
   ballAction = BALL_ACTION_NONE;
   collidedBallFirst = false;
   collidedOwnDiskFirst = false;
   collidedEnemyDiskFirst = false;

   /* Change the half */
   state = STATE_MIDDLE;
}

/**********************************************************************
 *                      maxRemainingDiskTouches                     *
 **********************************************************************/
int Rules::maxRemainingDiskTouches()
{
   /* To not change remaining at "stoppped" states */
   if(state == STATE_NORMAL)
   {
      //TODO implement other Rules here in a switch!
      //switch(gameType)
      return(3);
   }
   return(remainingDiskTouches);
}

/**********************************************************************
 *                     maxRemainingGlobalTouches                      *
 **********************************************************************/
int Rules::maxRemainingGlobalTouches()
{
   //TODO implement other Rules here in a switch!
   return(12);
}

/**********************************************************************
 *                        clearTouchesCounters                        *
 **********************************************************************/
void Rules::clearTouchesCounters()
{
   remainingGlobalTouches = maxRemainingGlobalTouches();
   remainingDiskTouches = 1;
}

/**********************************************************************
 *                             changeTeamToAct                        *
 **********************************************************************/
void Rules::changeTeamToAct()
{
   remainingGlobalTouches = maxRemainingGlobalTouches();
   if(activeTeam == teamA)
   {
      activeTeam = teamB;
   }
   else
   {
      activeTeam = teamA;
   }
}

/**********************************************************************
 *                               getState                             *
 **********************************************************************/
int Rules::getState()
{
   return(state);
}

/**********************************************************************
 *                               setState                             *
 **********************************************************************/
void Rules::setState(int st)
{
   state = st;
}

/**********************************************************************
 *                           getInactiveTeam                          *
 **********************************************************************/
Team* Rules::getInactiveTeam()
{
   return getOtherTeam(activeTeam);
}

/**********************************************************************
 *                            getActiveTeam                           *
 **********************************************************************/
Team* Rules::getActiveTeam()
{
   return(activeTeam);
}

/**********************************************************************
 *                            setActiveTeam                           *
 **********************************************************************/
void Rules::setActiveTeam(Team* t)
{
   activeTeam = t;
}

/**********************************************************************
 *                            setActiveTeam                           *
 **********************************************************************/
void Rules::set(ProtocolParsedMessage& msg)
{
   Team* lastTeam = activeTeam;
   state = msg.msgInfo;
   activeTeam = (msg.msgAditionalInfo == UPDATE_TYPE_TEAM_A)?teamA:teamB;
   changedBallOwner = (activeTeam != lastTeam);
   setRemainingTouches();
   updateStatistics(state, lastTeam, activeTeam);
}

/**********************************************************************
 *                            getCurrentDisk                          *
 **********************************************************************/
TeamPlayer* Rules::getCurrentDisk()
{
   return currentDisk;
}

/**********************************************************************
 *                            setCurrentDisk                          *
 **********************************************************************/
void Rules::setCurrentDisk(TeamPlayer* tp)
{
   /* Set it as the acting team player */
   currentDisk = tp;
   /* Set it as the last to act on the active team */
   getActiveTeam()->setLastActiveTeamPlayer(tp);
   /* And make sure the inactive team has no last active player. */
   getInactiveTeam()->setLastActiveTeamPlayer(NULL);
}

/**********************************************************************
 *                            getUpperTeam                            *
 **********************************************************************/
Team* Rules::getUpperTeam()
{
   return upperTeam;
}

/**********************************************************************
 *                            getOtherTeam                            *
 **********************************************************************/
Team* Rules::getOtherTeam(Team* t)
{
   if(t == teamA)
   {
      return teamB;
   }
   return teamA;
}

/**********************************************************************
 *                            setUpperTeam                            *
 **********************************************************************/
void Rules::setUpperTeam(Team* t)
{
   upperTeam = t;
}

/**********************************************************************
 *                               getBall                              *
 **********************************************************************/
Ball* Rules::getBall()
{
   return usedBall;
}


/**********************************************************************
 *                               setBall                              *
 **********************************************************************/
void Rules::setBall(Ball* b)
{
   usedBall = b;
}

/**********************************************************************
 *                              getField                              *
 **********************************************************************/
Field* Rules::getField()
{
   return usedField;
}


/**********************************************************************
 *                              setField                              *
 **********************************************************************/
void Rules::setField(Field* f)
{
   usedField = f;
}

/**********************************************************************
 *                               setTeamA                             *
 **********************************************************************/
void Rules::setTeamA(Team* t)
{
   teamA = t;
}

/**********************************************************************
 *                               setTeamB                             *
 **********************************************************************/
void Rules::setTeamB(Team* t)
{
   teamB = t;
}

/**********************************************************************
 *                               getTeamA                             *
 **********************************************************************/
Team* Rules::getTeamA()
{
   return teamA;
}

/**********************************************************************
 *                               getTeamB                             *
 **********************************************************************/
Team* Rules::getTeamB()
{
   return teamB;
}

/**********************************************************************
 *                             setDiskAct                             *
 **********************************************************************/
bool Rules::setDiskAct(TeamPlayer* disk)
{
   if(currentDisk == disk)
   {
      if(remainingDiskTouches <= 0)
      {
         /* Can't Use this disk! */
         remainingDiskTouches = 0;
         return false;
      }
   }
   else
   {
      /* Clear Disk Touches */
      setCurrentDisk(disk);
      /* Reset disk touches */
      remainingDiskTouches = maxRemainingDiskTouches();
      if(remainingDiskTouches < 0)
      {
         remainingDiskTouches = 0;
      }
   }
 
   Ogre::LogManager::getSingleton().stream()
      << "Will dec: global: " << remainingGlobalTouches
      << " disk: " << remainingDiskTouches;

   /* Decrease Disk Touches */
   remainingDiskTouches--;
   if(remainingDiskTouches < 0)
   {
      /* To avoid underflow  */
      remainingDiskTouches = 0;
   }
   /* Decrease Global Touches */
   remainingGlobalTouches--;

   return true;
}

/**********************************************************************
 *                             setBallAct                             *
 **********************************************************************/
void Rules::setBallAct()
{
   remainingGlobalTouches--;
}

/**********************************************************************
 *                        getRemainingtouches                         *
 **********************************************************************/
int Rules::getRemainingTouches()
{
   return remainingGlobalTouches;
}

/**********************************************************************
 *                        setRemainingtouches                         *
 **********************************************************************/
void Rules::setGlobalRemainingTouches(int t)
{
   remainingGlobalTouches = t;
}

/**********************************************************************
 *                        getRemainingtouches                         *
 **********************************************************************/
int Rules::getRemainingTouches(TeamPlayer* disk)
{
   if(disk == currentDisk)
   {
      /* Is the current, so */
      return remainingDiskTouches;
   }

   /* Isn't the current, so get max remaining */
   int res = maxRemainingDiskTouches();

   /* Make shure the remaining disk touches is lesser or equal to global */
   if(res > remainingGlobalTouches)
   {
      res = remainingGlobalTouches;
   }
   return res;
}

/**********************************************************************
 *                        setRemainingtouches                         *
 **********************************************************************/
void Rules::setDiskRemainingTouches(int t)
{
   remainingDiskTouches = t;
}

/*********************************************************************
 *                          setPositions                             *
 *********************************************************************/
void Rules::setPositions()
{
   bool isTeamAUpper = (upperTeam == teamA);
   bool isTeamAActing = (activeTeam == teamA);

   //bool ballUpperSide = (usedBall->getPosX() >= FIELD_MIDDLE_X);

   float x=0, z=0;

   Ogre::Vector2 halfSize = usedField->getHalfSize();
   Ogre::Vector2 sideDelta = usedField->getSideDelta();
   Ogre::Vector2 littleAreaDelta = usedField->getLittleAreaDelta();

   if(willShoot)
   {
      /* Called after a goal shoot. Must reset goalkeepers positions 
       * to respective goal middles. */
      teamB->getGoalKeeper()->startPositionAtField(!isTeamAUpper, 
            usedField);
      teamA->getGoalKeeper()->startPositionAtField(isTeamAUpper, 
            usedField);
   }
   
   switch(state)
   {
      case STATE_MIDDLE:
      {
         teamA->startPositionAtField(isTeamAUpper, isTeamAActing,
                                     usedField);
         teamB->startPositionAtField(!isTeamAUpper, !isTeamAActing,
                                     usedField);
         /* Put Ball At Center */
         usedBall->setPosition(FIELD_MIDDLE_X, 0.0f, FIELD_MIDDLE_Z);
      }
      break;

      case STATE_CORNER_KICK:
      {
         /* Define X Position */
         if(ballUpper)
         {
            x = halfSize[0] - sideDelta[0] - 0.05f;
         }
         else
         {
            x = -halfSize[0] + sideDelta[0] + 0.05f;
         }
         
         /* Define Z position */
         if(pZ >= FIELD_MIDDLE_Z)
         {
            z = halfSize[1] - sideDelta[1] - 0.05f;
         }  
         else
         {
            z = -halfSize[1] + sideDelta[1] + 0.05f;
         }

         /* Put Ball At Corner */
         usedBall->setPosition(x, 0.0, z);
      }
      break;

      case STATE_THROW_IN:
      {
         /* Put Ball At Exit X Position, and SIDE */
         x = pX;
         if(pZ > 0)
         {
            z = halfSize[1] - sideDelta[1] - 0.05f;
         }
         else
         {
            z = -halfSize[1] + sideDelta[1] + 0.05f;
         }
         usedBall->setPosition(pX, 0.0, z);
      }
      break;

      case STATE_GOAL_KICK:
      {
         /* Define X Position */
         if(ballUpper)
         {
            x = halfSize[0] - littleAreaDelta[0];
         }
         else
         {
            x = -halfSize[0] + littleAreaDelta[0];
         }
         
         /* Define Z position */
         if(pZ >= FIELD_MIDDLE_Z)
         {
            z = FIELD_MIDDLE_Z + littleAreaDelta[1];
         }  
         else
         {
            z = FIELD_MIDDLE_Z - littleAreaDelta[1];
         }

         /* Put Ball At Little Area */
         usedBall->setPosition(x, 0.0, z);
      }
      break;

      case STATE_FREE_KICK:
      {
         /* Put Ball At Foul Position, making sure it's in-field */
         usedField->getNearestPointInPlayableArea(pX, pZ);
         usedBall->setPosition(pX, 0.0, pZ);
      }
      break;

      case STATE_PENALTY_KICK:
      {
         /* Set X Position */  
         if(pX <= FIELD_MIDDLE_X)
         {
            x = -halfSize[0] + usedField->getPenaltyMark();
         }
         else
         {
            x = halfSize[0] - usedField->getPenaltyMark();
         }
         
         /* Put Ball At Penalty Mark */
         usedBall->setPosition(x, 0.0, z);
      }
      break;
   }
}

/**********************************************************************
 *                              clearFlags                            *
 **********************************************************************/
void Rules::clearFlags()
{
   collidedBallFirst = false;
   collidedOwnDiskFirst = false;
   collidedEnemyDiskFirst = false;
   ballAction = BALL_ACTION_NONE;
   pX = -1;
   pZ = -1;
   lastBallCollided = activeTeam;
}

/**********************************************************************
 *                               newTurn                              *
 **********************************************************************/
Team* Rules::newTurn()
{
   /* Clear things */
   willShoot = (state == STATE_PENALTY_KICK);
   clearFlags();
   
   /* Tell GUI which team is active */
   GuiScore::newTurn(activeTeam == teamA);

   Ogre::Log::Stream stream = Ogre::LogManager::getSingleton().stream();
   stream << "\n***************************************************\n"
          << "* New Turn. Active Team: " << activeTeam->getName() << "\n";
   if(currentDisk != NULL)
   {
      stream << "* Active Disk: " << currentDisk->getName() << "\n";
   }
   else
   {
      stream << "* Active Disk: None\n";
   }
   stream << "* Remaining Global Touches: " << remainingGlobalTouches << "\n"
          << "* Remaining active disk touches: " << remainingDiskTouches << "\n"
          << "* State: " << state;

   return activeTeam;
}

/**********************************************************************
 *                              startHalf                             *
 **********************************************************************/
void Rules::startHalf(bool firstHalf)
{
   clear();

   secondHalf = !firstHalf;

   /* Set first player to act */
   if(firstHalf)
   {
      activeTeam = teamA;
      upperTeam = teamB;
   }
   else
   {
      activeTeam = teamB;
      upperTeam = teamA;
   }

   /* Put everyone at middle state positions */
   state = STATE_MIDDLE;
   teamA->startPositionAtField((upperTeam == teamA), (activeTeam == teamA),
                               usedField);
   teamB->startPositionAtField((upperTeam == teamB), (activeTeam == teamB),
                               usedField);
   usedBall->setPosition(FIELD_MIDDLE_X, 0.0f, FIELD_MIDDLE_Z);
   
   /* Tell GUI which team is active */
   if(GuiScore::isInited())
   {
      GuiScore::newTurn(activeTeam == teamA);
   }
}

/**********************************************************************
 *                             updateClock                            *
 **********************************************************************/
bool Rules::updateClock()
{
   unsigned long time = periodTimer.getMilliseconds();

   halfSeconds = ((time / 1000) % 60);
   halfMinutes = time / 60000;

   return (halfMinutes >= minutesPerHalf);
}

/**********************************************************************
 *                          getCurentHalfTime                         *
 **********************************************************************/
unsigned long Rules::getCurrentHalfTime()
{
   return periodTimer.getMilliseconds();
}

/**********************************************************************
 *                          setCurentHalfTime                         *
 **********************************************************************/
void Rules::setCurrentHalfTime(unsigned long ms)
{
   periodTimer.reset(ms);
}

/**********************************************************************
 *                                pause                               *
 **********************************************************************/
void Rules::pause()
{
   periodTimer.pause();
}

/**********************************************************************
 *                                resume                              *
 **********************************************************************/
void Rules::resume()
{
   periodTimer.resume();
}

/**********************************************************************
 *                             setGameType                            *
 **********************************************************************/
void Rules::setGameType(int type)
{
   gameType = type;
}

/**********************************************************************
 *                          setMinutesPerHalf                         *
 **********************************************************************/
void Rules::setMinutesPerHalf(int minutes)
{
   minutesPerHalf = minutes;
}

/**********************************************************************
 *                             isFirstHalf                            *
 **********************************************************************/
bool Rules::isFirstHalf()
{
   return !secondHalf;
}

/**********************************************************************
 *                               setHalf                              *
 **********************************************************************/
void Rules::setHalf(bool first)
{
   secondHalf = !first;
}

/**********************************************************************
 *                           prepareToShoot                           *
 **********************************************************************/
void Rules::prepareToShoot()
{
   willShoot = true;
   Stats::goalShoot(activeTeam == teamA);
}

/**********************************************************************
 *                          goalShootDefined                          *
 **********************************************************************/
bool Rules::goalShootDefined()
{
   return willShoot;
}

/**********************************************************************
 *                          diskCollideDisk                           *
 **********************************************************************/
void Rules::diskCollideDisk(Team* diskPlayer1, Team* diskPlayer2,
                            float x, float z)
{
   if( (!collidedBallFirst) && 
       (!collidedOwnDiskFirst) && 
       (!collidedEnemyDiskFirst) &&
       ( (activeTeam == diskPlayer1) || 
         (activeTeam == diskPlayer2)) )
   {
      pX = x;
      pZ = z;
      if(diskPlayer1 != diskPlayer2)
      {
         collidedEnemyDiskFirst = true;
      }
      else
      {
         collidedOwnDiskFirst = true;
      }
   }
}

/**********************************************************************
 *                          ballCollideDisk                           *
 **********************************************************************/
void Rules::ballCollideDisk(Team* player)
{
   /* Define last player contact the ball, if ball not already exited
    * the field */
   if(ballAction == BALL_ACTION_NONE)
   {
      lastBallCollided = player;
   }
   

   /* Define if activeTeam disk player collidedd with ball first */
   if( (ballAction == BALL_ACTION_NONE) && 
       (player == activeTeam) && 
       (!collidedOwnDiskFirst) && (!collidedEnemyDiskFirst) )
   {
      collidedBallFirst = true;
   }
}

/**********************************************************************
 *                          ballExitAtSide                            *
 **********************************************************************/
void Rules::ballExitAtSide(float x, float z)
{
   if( (!collidedEnemyDiskFirst) && (ballAction == BALL_ACTION_NONE) )
   {
      ballAction = BALL_ACTION_SIDE;
      pX = x;
      pZ = z;
   }
}

/**********************************************************************
 *                         ballExitAtByline                           *
 **********************************************************************/
void Rules::ballExitAtByline(bool upper, float z)
{
   if( (!collidedEnemyDiskFirst) && (ballAction == BALL_ACTION_NONE) )
   {
      ballAction = BALL_ACTION_BYLINE;
      pX = -1;
      pZ = z;
      ballUpper = upper;
   }
}

/**********************************************************************
 *                           ballEnterGoal                            *
 **********************************************************************/
void Rules::ballEnterGoal(bool upper)
{
   if( (!collidedEnemyDiskFirst) && (ballAction == BALL_ACTION_NONE) )
   {
      ballAction = BALL_ACTION_GOAL;
      ballUpper = upper;
   }
}

/**********************************************************************
 *                         verifyGoalValid                            *
 **********************************************************************/
void Rules::verifyGoalValid(bool onlineMode)
{
   /* Verify of witch team the goal is from */
   bool teamAGoal = ( ( (ballUpper) && (upperTeam != teamA) ) ||
                      ( (!ballUpper) && (upperTeam == teamA) ) );
   
   bool valid = false;
   
   /* If auto goal, always valid. */
   if( ((teamAGoal) && (activeTeam == teamB)) ||
       ((!teamAGoal) && (activeTeam == teamA)) )
   {
      valid = true;
   }
   /* Otherwise, will be valid if defined goal shoot */
   else if(willShoot)
   {
      /* AND not knocked-down opponent's gk. */
      if(teamAGoal)
      {
         valid = teamB->getGoalKeeper()->isFacingUp();
      }
      else
      {
         valid = teamA->getGoalKeeper()->isFacingUp();
      }
   }
   
   /* Set if valid or not. */
   if(valid)
   {
      GuiMessage::set("Goal!");
      if(teamAGoal)
      {
         GuiScore::goalTeamA();
      }
      else
      {
         GuiScore::goalTeamB();
      }
      if(onlineMode)
      {
         Protocol protocol;
         protocol.queueGoalHappened(teamAGoal);
      }
      state = STATE_MIDDLE;
      /* Reset Touches */
      remainingGlobalTouches = maxRemainingGlobalTouches();
      remainingDiskTouches = 1;
   }
   else
   {
      state = STATE_GOAL_KICK;
   }
}

/**********************************************************************
 *                         changedTeamToAct                           *
 **********************************************************************/
bool Rules::changedTeamToAct()
{
   return changedBallOwner;
}

/**********************************************************************
 *                         showStateMessage                           *
 **********************************************************************/
void Rules::showStateMessage()
{
   switch(state)
   {
      case STATE_PENALTY_KICK:
      {
         GuiMessage::set("Penalty!");
      }
      break;
      case STATE_FREE_KICK:
      {
         GuiMessage::set("Foul!");
      }
      break;
      case STATE_THROW_IN:
      {
         GuiMessage::set("Throw in");
      }
      break;
      case STATE_GOAL_KICK:
      {
         if( (ballAction == BALL_ACTION_GOAL) &&
             (collidedBallFirst) )
         {
            GuiMessage::set("Invalid Goal!");
         }
         else
         {
            GuiMessage::set("Goal Kick");
         }
      }
      break;
      case STATE_CORNER_KICK:
      {
         GuiMessage::set("Corner Kick");
      }
      break;
      case STATE_NORMAL:
      {
         if(remainingGlobalTouches <= 0)
         {
            GuiMessage::set("No remaining moves!");
         }
      }
      break;
   }
}

/**********************************************************************
 *                        ballAtFinalPosition                         *
 **********************************************************************/
void Rules::ballAtFinalPosition(bool onlineMode)
{
   /* The System is stable, so verify things to the next turn */
   bool isUpTeamActing = (activeTeam == upperTeam);

   /* If shooted, always change team to act */
   changedBallOwner = willShoot;

   /* Verify Fouls */
   if(collidedEnemyDiskFirst)
   {
      if(usedField->isInnerPenaltyArea(pX, pZ, isUpTeamActing,
                                       !isUpTeamActing))
      {
         /* Foul occurred inner own disk area, so its a penalty! */
         state = STATE_PENALTY_KICK;
      }
      else
      {
         state = STATE_FREE_KICK;
      }
      changedBallOwner = true;
   }

   /* Verify balls miss */
   else if( (!collidedBallFirst) )
   {
      changedBallOwner = true;
      if(ballAction == BALL_ACTION_NONE)
      {
         /* Do Not Collided with anything, or only 
          * with his own players, so ball's owner change */
         state = STATE_NORMAL;
      }
      else 
      {
         /* Ball moved somewhere outside... */
         if(ballAction == BALL_ACTION_SIDE)
         {
            state = STATE_THROW_IN;
         }
         else if( (ballAction == BALL_ACTION_BYLINE) ||
                  (ballAction == BALL_ACTION_GOAL) )
         {
            /* Must check if action goal too, as it could be an invalid one
             * (and if invalid, a goal kick should be). */
            checkCornerOrGoalKick();      
         }
      }
   }

   /* Verify Throw-ins */
   else if(ballAction == BALL_ACTION_SIDE)
   {
      state = STATE_THROW_IN;
      if(activeTeam == lastBallCollided)
      {
         changedBallOwner = true;
      }
   }

   /* Verify Corner Kicks and Goal Kicks */
   else if(ballAction == BALL_ACTION_BYLINE)
   {
      checkCornerOrGoalKick();      
      changedBallOwner = (activeTeam == lastBallCollided);
   }

   /* Verify goals */
   else if(ballAction == BALL_ACTION_GOAL)
   {
      /* verify goal of who to change (or not) the owner */
      if(ballUpper) 
      {
         if(upperTeam != activeTeam)
         {
            changedBallOwner = true;
         }
      }
      else
      {
         if(upperTeam == activeTeam)
         {
            changedBallOwner = true;
         }
      }

      /* Verify if the goal was valid, changing the state
       *  to middle (valid) or goal kick (invalid) */
      verifyGoalValid(onlineMode);
   }

   /* Verify if global touches is underflowed */
   else if(remainingGlobalTouches <= 0)
   {
      state = STATE_NORMAL;
      changedBallOwner = true;
   }

   /* Finally verify if collided with ball */
   else if(collidedBallFirst)
   {
      state = STATE_NORMAL;
   }
   
   /* Set limits of touches for new state */
   setRemainingTouches();

   /* Verify if the ball's owner changed */
   Team* previousActiveTeam = activeTeam;
   if(changedBallOwner)
   {
      changeTeamToAct();
   }
   
   /* Set statistics */
   updateStatistics(state, previousActiveTeam, activeTeam);

   /* Show a message of the state defined. */
   showStateMessage();
}

/**********************************************************************
 *                        setRemainingTouches                         *
 **********************************************************************/
void Rules::setRemainingTouches()
{
   switch(state)
   {
      default:
      case STATE_NORMAL:
         if(changedBallOwner)
         {
            remainingGlobalTouches = maxRemainingGlobalTouches();
            remainingDiskTouches = 3;
         }
      break;
      case STATE_MIDDLE:
      case STATE_CORNER_KICK:
      case STATE_FREE_KICK:
      case STATE_THROW_IN:
      case STATE_GOAL_KICK:
      case STATE_PENALTY_KICK:
         remainingGlobalTouches = maxRemainingGlobalTouches();
         remainingDiskTouches = 1;
      break;
   }
}

/**********************************************************************
 *                       checkCornerOrGoalKick                        *
 **********************************************************************/
void Rules::checkCornerOrGoalKick()
{
   if(ballUpper) 
   {
      if(upperTeam == lastBallCollided)
      {
         state = STATE_CORNER_KICK;
      }
      else
      {
         state = STATE_GOAL_KICK;
      }
   }
   else
   {
      if(upperTeam != lastBallCollided)
      {
         state = STATE_CORNER_KICK;
      }
      else
      {
         state = STATE_GOAL_KICK;
      }
   }
}

/**********************************************************************
 *                          updateStatistics                          *
 **********************************************************************/
void Rules::updateStatistics(int nextState, Team* actingTeam,
                             Team* nextActingTeam)
{
   /* Someone just moved. */
   Stats::moved(activeTeam == teamA);
   
   /* And some special states check. */
   switch(nextState)
   {
      case STATE_GOAL_KICK:
         Stats::goalKick(nextActingTeam == teamA);
      break;
      case STATE_CORNER_KICK:
         Stats::corner(nextActingTeam == teamA);
      break;
      case STATE_PENALTY_KICK:
      case STATE_FREE_KICK:
         Stats::foul(activeTeam == teamA);
      break;
      case STATE_THROW_IN:
         Stats::throwIn(nextActingTeam == teamA);
      break;
   }
}


/*  Static Variables   */
int Rules::state;
int Rules::minutesPerHalf = 10;
int Rules::gameType = Rules::TYPE_BALL_12;
int Rules::halfMinutes;
int Rules::halfSeconds;
Kobold::Timer Rules::periodTimer;
TeamPlayer* Rules::currentDisk;
bool Rules::secondHalf = true;
bool Rules::willShoot = false;
int Rules::remainingGlobalTouches;
int Rules::remainingDiskTouches;
Team* Rules::teamA = NULL;
Team* Rules::teamB = NULL;
Team* Rules::activeTeam = NULL;
Team* Rules::upperTeam = NULL;
Team* Rules::lastBallCollided = NULL;
Ball* Rules::usedBall = NULL;
Field* Rules::usedField = NULL;
bool Rules::collidedBallFirst = false;
bool Rules::collidedOwnDiskFirst = false;
bool Rules::collidedEnemyDiskFirst = false;
int Rules::ballAction = BALL_ACTION_NONE;
float Rules::pX = -1;
float Rules::pZ = -1;
bool Rules::ballUpper = false;
bool Rules::changedBallOwner = false;

