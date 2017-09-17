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

#include "replay.h"
#include "rules.h"
#include <goblin/camera.h>
using namespace BtSoccer;

#define REPLAY_TEXTURE_DELTA  0.05

/**************************************************************
 *                          Constructor                       *
 **************************************************************/
Replay::Replay(Field* pfield)
{
   teamA = NULL;
   teamB = NULL;
   gameBall = NULL;
   
   gameField = pfield;
   
   start = 0;
   currentFrame = 0;
   end = 0;
   replaying = false;
   lastTurnFrames[0] = 0;
   lastTurnFrames[1] = 1;
   onlineGame = false;

   /* Load the Controller Texture */
   replayGui = new GuiReplay();
}

/**************************************************************
 *                          Destructor                        *
 **************************************************************/
Replay::~Replay()
{
   /* Delete the video controller */
   delete(replayGui);
}

/**************************************************************
 *                           setTeamA                         *
 **************************************************************/
void Replay::setTeamA(Team *t)
{
   teamA = t;
}

/**************************************************************
 *                           setTeamB                         *
 **************************************************************/
void Replay::setTeamB(Team *t)
{
   teamB = t;
}

/**************************************************************
 *                           setBall                          *
 **************************************************************/
void Replay::setBall(Ball *b)
{
   gameBall = b;
}

/**************************************************************
 *                            clear                           *
 **************************************************************/
void Replay::clear()
{
   end = start;
}

/**************************************************************
 *                          updateData                        *
 **************************************************************/
void Replay::updateData(ReplayData* repData)
{
   int i;

   if( (teamA) && (teamB) )
   {
      /* TeamA Goal Keeper */
      repData->teamAPositions[0] = teamA->getGoalKeeper()->getPosition();
      repData->teamAAngles[0] = teamA->getGoalKeeper()->getOrientation();

      /* TeamB Goal Keeper */
      repData->teamBPositions[0] = teamB->getGoalKeeper()->getPosition();
      repData->teamBAngles[0] = teamB->getGoalKeeper()->getOrientation();

      /* Get disk positions and angles*/
      for(i=0; i < gameField->getNumberOfDisks(); i++)
      {
         /* TeamA Disk */
         repData->teamAPositions[i+1] = teamA->getDisk(i)->getPosition();
         repData->teamAAngles[i+1] = teamA->getDisk(i)->getOrientation();

         /* TeamB Disk */
         repData->teamBPositions[i+1] = teamB->getDisk(i)->getPosition();
         repData->teamBAngles[i+1] = teamB->getDisk(i)->getOrientation();
      }
   }
   
   /* getting ball position and angles */
   if(gameBall)
   {
      repData->ballPosition = gameBall->getPosition();;
      repData->ballAngle = gameBall->getOrientation();
   }
}


/**************************************************************
 *                          updateData                        *
 **************************************************************/
void Replay::updateData()
{
   if(replaying) 
   {
      return;
   }
 
   /* update date to current replay frame */
   updateData(&data[end]);     
    
   /* setting new vector end position */
   end = (end+1) % FRAMES_TO_REPLAY;
   
   /* if vector is full, shift elements */
   if(end == start)
   {
      start = (start + 1) % FRAMES_TO_REPLAY;
   }
}

/**************************************************************
 *                        newTurnStarted                      *
 **************************************************************/
void Replay::newTurnStarted()
{
   lastTurnFrames[0] = lastTurnFrames[1];
   lastTurnFrames[1] = (end+1) % FRAMES_TO_REPLAY;
}

/**************************************************************
 *                           getData                          *
 **************************************************************/   
ReplayData* Replay::getData(int frame)
{
   /* Transforming frame index from 0-FRAMES_TO_REPLAY to start-end */
   frame = frame+start;
   
   /* Making sure frame is inside start-end interval */
   if(end < start)
   {
      int end2 = end+FRAMES_TO_REPLAY;
      if(frame >= end2)
      {
         frame = end2 - 1;
      }
   }
   else if(frame >= end)
   {
      frame = end-1;
   }
   frame = frame % FRAMES_TO_REPLAY;
   
   return(&data[frame]);
}

/**************************************************************
 *                          initReplay                        *
 **************************************************************/
void Replay::initReplay(bool startAtLastTwoTurns, bool onlineGame)
{
   if( (start == 0) && (end == 0) )
   {
      /* no replay if no data! */
      replaying = false;
      return;
   }

   /* Retrieve current positions, to put back when exit replay */
   updateData(&prevPositions);

   /* Set to replay' init */
   replaying = true;
   if(startAtLastTwoTurns)
   {
      position = lastTurnFrames[0];
   }
   else
   {
      position = start;
   }
   curSpeed = 1.0;
   delta = curSpeed;
   
   this->onlineGame = onlineGame;
   
   if(!this->onlineGame)
   {
      /* Enable the GUI */
      replayGui->show();
   }

   /* Pause the match! */
   Rules::pause();
}

/**************************************************************
 *                        replayEvents                        *
 **************************************************************/
int Replay::verifyEvents(int mouseX, int mouseY, bool leftButtonPressed)
{
   if(!replaying)
   {
      return(EVENT_EXIT);
   }

   /* Set current frame */
   int frame = (int)(position) % FRAMES_TO_REPLAY;
   int end2 = (end < start)?end+FRAMES_TO_REPLAY:end;

   /* Verify events */
   switch(replayGui->verifyEvents(mouseX, mouseY, leftButtonPressed))
   {
      /* Flux Controls */
      case GuiReplay::BUTTON_STOP:
      {
         /* Pause the replay */
         delta = 0;
         /* Put it at initial position */
         position = start;
      }
      break;
      case GuiReplay::BUTTON_PLAY:
      {
         /* Start the play at current speed */
         delta = curSpeed;
      }
      break;
      case GuiReplay::BUTTON_PAUSE:
      {
         /* Pause the play */
         delta = 0.0;
      }
      break;
      
      /* "Direction Controls" */
      case GuiReplay::BUTTON_BACKWARD:
      {
         /* Backward the play */
         position -= curSpeed;
      }
      break;
      case GuiReplay::BUTTON_FORWARD:
      {
         /* Forward the play */
         position += curSpeed;
      }
      break;

      /* Velocity Controls */
      case GuiReplay::BUTTON_HALF_SPEED:
      {
         curSpeed = 0.5;
         if(delta)
         {
            delta = curSpeed;
            position += delta;
         }
      }
      break;
      case GuiReplay::BUTTON_NORMAL_SPEED:
      {
         curSpeed = 1.0;
         if(delta)
         {
            delta = curSpeed;
            position += delta;
         }
      }
      break;
      case GuiReplay::BUTTON_DOUBLE_SPEED:
      {
         curSpeed = 2.0;
         if(delta)
         {
            delta = curSpeed;
            position += delta;
         }
      }
      break;

      /* ShutDown Controller  */
      case GuiReplay::BUTTON_SHUTDOWN:
      {
         beforeExitReplay();
         return(EVENT_EXIT);
      }
      break;

      case GuiReplay::BUTTON_NONE:
      default:
      {  
         /* Update the position */
         position += delta;
      }
      break;
   }

   /* Verify Position Limits */
   if(position < start) 
   {
      position = start;
   }
   else if(position >= end2)
   {
      position = end2-1;
      if(onlineGame)
      {
         /* At online mode, must exit after end 
          * (as no controllers are available) */
         beforeExitReplay();
         return(EVENT_EXIT);
      }
   }

   /* Render Replay Frame */
   setFramePositions(frame);

   return(EVENT_OTHER);
}

/**************************************************************
 *                       beforeExitReplay                     *
 **************************************************************/
void Replay::beforeExitReplay()
{
   /* Done */
   replaying = false;
   replayGui->hide();
   
   /* Reset to previous values */
   restorePositions();
   
   /* Resume Game */
   Rules::resume();
}

/**************************************************************
 *                          setPositions                      *
 **************************************************************/
void Replay::setPositions(ReplayData* repData)
{
   /* Set Ball */
   if(gameBall)
   {
      gameBall->setPositionAsReplay(repData->ballPosition, 
            repData->ballAngle);
   }

   /* Set Teams */
   if(teamA)
   {
      teamA->setReplayerPositions(repData->teamAPositions, 
            repData->teamAAngles);
   }
   
   if(teamB)
   {
      teamB->setReplayerPositions(repData->teamBPositions, 
            repData->teamBAngles);
   }

}

/**************************************************************
 *                       setFramePositions                    *
 **************************************************************/
void Replay::setFramePositions(int frame)
{
   setPositions(&data[frame]);
}

/**************************************************************
 *                      restorePositions                      *
 **************************************************************/
void Replay::restorePositions()
{
   setPositions(&prevPositions);
}

