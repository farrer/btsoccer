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

#ifndef _btsoccer_replay_h
#define _btsoccer_replay_h

#include "../btsoccer.h"

#include "team.h"
#include "teamplayer.h"
#include "ball.h"
#include "goalkeeper.h"

#include "../gui/guireplay.h"

#define FRAMES_TO_REPLAY 500  /**< Quantity of frames to replay */

namespace BtSoccer
{

/*! The replay data has all positions and angles
 * of all players, ball, etc on a frame. */
class ReplayData
{
   public:
      Ogre::Vector3 teamAPositions[11];  /**< Positions of teamA players */
      Ogre::Quaternion teamAAngles[11];  /**< Angles of teamA players */
      
      Ogre::Vector3 teamBPositions[11];  /**< Positions of teamB players */
      Ogre::Quaternion teamBAngles[11];  /**< Angles of teamB players */
   
      Ogre::Vector3 ballPosition;        /**< Position of the ball */
      Ogre::Quaternion ballAngle;        /**< Angle of the ball */
};

/*! The replay class saves FRAMES_TO_REPLAY frames and replay them at
 * any time, with a simple video controller. */
class Replay
{
   public:

      enum
      {
         EVENT_NONE,
         EVENT_EXIT,
         EVENT_OTHER
      };

      /*! Constructor
       * \param pfield -> field used */
      Replay(Field* pfield);

      /*! Destructor */
      ~Replay();

      /*! Define team A 
       * \param t -> pointer to the team A */
      void setTeamA(Team *t);

      /*! Define team B 
       * \param t -> pointer to the team B */
      void setTeamB(Team *t);

      /*! Define ball
       * \param b -> pointer to the current ball */
      void setBall(Ball *b);

      /*! Clear all frames current stored on the replay */
      void clear();

      /*! Update all data relative to the frame */
      void updateData();
   
      /*! Tell replay that a new turn has just started */
      void newTurnStarted();

      /*! Get data for a desired frame
       * \param frame -> frame number to get
       * \return -> data of the desired frame */
      ReplayData* getData(int frame);

      /*! Init the replay display
       * @param startAtLastTwoTurns -> start the replay at last two turns,
       * usually it is true for goal replays, false otherwise. 
       * @param onlineGame -> if game is online or local */
      void initReplay(bool startAtLastTwoTurns, bool onlineGame);

      /*! Treat replay mode */
      int verifyEvents(int mouseX, int mouseY, bool leftButtonPressed);

   protected:

      /*! Set positions to a replay frame */
      void setFramePositions(int frame);

      /*! set positions prior to call of the replay */
      void restorePositions();

      /*! Update current data to an specific replay data */
      void updateData(ReplayData* data);
      
      /*! Set all positions to an specific replay data */
      void setPositions(ReplayData* repData);
   
      /*! To be called before exit replay exibition. */
      void beforeExitReplay();

      BtSoccer::Team* teamA;              /**< Pointer to teamA */
      BtSoccer::Team* teamB;              /**< Pointer to teamB */
      BtSoccer::Ball* gameBall;           /**< Pointer to the game ball */
      BtSoccer::Field* gameField;         /**< The field used */
   
      bool onlineGame;                    /**< If game is online or local */

      GuiReplay* replayGui;               /**< The GUI for replay */

      float delta;                        /**< the variation for frame change */
      float position;                     /**< current position for frame */
      float curSpeed;                     /**< current frame change speed */
      
      int start,                          /**< start frame on the vector */
          currentFrame,                   /**< current frame on the vector */
          end;                            /**< end frame on the vector */
      bool replaying;                     /**< true if is rendering replay */
      ReplayData data[FRAMES_TO_REPLAY];  /**< The replay Vector */
      ReplayData prevPositions;           /**< Positions before call replay */
   
      int lastTurnFrames[2];              /**< Init frame of last 2 turns */
   
};

}

#endif
