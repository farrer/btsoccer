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

#ifndef _btsoccer_team_h
#define _btsoccer_team_h

#include <OGRE/OgreSceneManager.h>

#include <kobold/defparser.h>

#include "../btsoccer.h"
#include "../net/protocol.h"
#include "../debug/bulletdebugdraw.h"

#include <vector> 

namespace BtSoccer
{

#define TEAM_MAX_DISKS             10
#define DEFAULT_DISK_MODEL         "disk/disk.mesh"
#define DEFAULT_GOAL_KEEPER_MODEL  "goalkeeper/goalkeeper.mesh"

/*! The team is the entity the user plays with. Each team is composed of 
 * 11 teamPlayers. 1 goal keeper and 10 disks. Each team has its
 * own models for the disks and for the goal keeper. */
class Team
{
   public:

      /*! Constructor, with first team color.
       * \param fileName -> fileName of team to load
       * \param ogreSceneManager -> pointer to the scene manager used */
      Team(Ogre::String fileName, Ogre::SceneManager* ogreSceneManager,
           Field* f, BulletDebugDraw* debugDraw, bool createAI=false);
      /*! Constructor, with selector for second team color, is it's equal to
       * opponent's one.
       * \param fileName -> fileName of team to load
       * \param ogreSceneManager -> pointer to the scene manager used
       * \param oponentPredominantColor -> predominant color of oponent. */
      Team(Ogre::String fileName, Ogre::SceneManager* ogreSceneManager,
           Field* f, Ogre::String oponentPredominantColor, 
           BulletDebugDraw* debugDraw,  bool createAI=false);
      /*! Constructor for dummy team (without graphic elements), used 
       * only at test cases. */
      Team(Ogre::String teamName, bool createAI=false);
      /*! Destructor */
      ~Team();

      /*! \return the AI which controlls the team, if any */
      BaseAI* getAI() { return ai; };

      /*! Put the team at a start position on the field.
       * \param upper -> true if the team is at upper side of the field, false
       *                 if at botton side.
       * \param f -> pointer to the field in use.
       * \param withBall -> true if has ball pocession, false if has't. */
      void startPositionAtField(bool upper, bool withBall, Field* f);

      /*! Get the team player related with the scene node
       * \param scNode -> scene node of the team player to get
       * \return pointer to the team player or NULL */
      TeamPlayer* getTeamPlayer(Ogre::SceneNode* scNode);

      /*! Get the active team player on last turn. 
       * \return pointer to the TeamPlayer who act on last turn, or
       *         NULL if last turn was an oponnent's turn or if
       *         directly act on ball */
      TeamPlayer* getLastActiveTeamPlayer();

      /*! Set the last active team player */
      void setLastActiveTeamPlayer(TeamPlayer* tp);

      /*! Set replayer positions for all the team
       * \param pos -> array of teamPlayer positions
       * \param angles -> array of teamPlayer quaternion orientation */
      void setReplayerPositions(Ogre::Vector3* pos, Ogre::Quaternion* angles);

      /*! Get the nearest player to the position
       * \param x -> x position 
       * \param z -> z position
       * \return -> pointer to the nearest player found */
      TeamPlayer* getNearestPlayer(float x, float z);

      /*! Get a vector with all disks ordered by distance to a point.
       * \param x -> point's x position 
       * \param z -> point's z position
       * \return -> ordered by distance pointers to players vector */
      std::vector<TeamPlayer*> getPlayersNearestFirst(float x, float z);

      /*! Get the team's goal keeper
       * \return -> pointer to the goal keeper teamPlayer */
      GoalKeeper* getGoalKeeper();

      /*! Get the team's disk[i]
       * \param i -> disk index
       * \return -> pointer to the disk or NULL */
      TeamPlayer* getDisk(int i);

      /*! Get the team's disk index: [i]
       * \param d -> disk pointer
       * \return -> index or -1 if not found. */
      int getDiskIndex(TeamPlayer* d);

      /*! Update all team positions
       * \return true if still moves, false if will stop. */
      bool update();

      /*! Update team positions, based on a received message.
       * \note: do not forget to call forcedPhysicsStep after. */
      void update(ProtocolParsedMessage& msg);

      /*! Get the team logo image */
      Ogre::String getLogo();

      /*! Get the team's name
       * \return string with team's name */
      Ogre::String getName();

      /*! Get the team's filename
       * \return string with team's filename */
      Ogre::String getFileName();
   
      /*! \return predominant team color of 1st uniform .*/
      Ogre::String getColorA();
      /*! \return predominant team color of 2nd uniform .*/
      Ogre::String getColorB();


      /*! Verify if the team is controlled by a local human player
       * \return true if human, false if AI or online */
      bool isControlledByHuman();

      /*! Set if the team is controlled by a human player or AI
       * \param b -> true: Human false: AI */
      void setControlledByHuman(bool b);

      /*! Get a pointer to the scene manager used */
      Ogre::SceneManager* getSceneManager();

      /*! Do things before a call to the physics step,
       * like setting its status as not moved, etc. */
      void prePhysicStep();
      
      /*! Verify if the object moved on last physics step
       * \return true if moved on last physics step */
      bool movedOnLastPhysicStep();

      /*! Queue all team updates at communication's send buffer.
       *  @param teamA if teamA or teamB. 
       *  @param sendAll true to send all positions, not just the ones
       *         that changed. */
      void queueUpdatesToSend(bool teamA, bool sendAll);

   protected:
   
      /*! Constructor, with selector for second team color, is is equal to
       * opopent's one.
       * \param fileName -> fileName of team to load
       * \param ogreSceneManager -> pointer to the scene manager used
       * \param oponentPredominantColor -> predominant color of oponent. */
      void load(Ogre::String fileName, Ogre::SceneManager* ogreSceneManager,
           Field* f, Ogre::String oponentPredominantColor);


      Ogre::String fileName;             /**< Team File Name */
      Ogre::String name;                 /**< The Team Name */
      Ogre::String colorA;               /**< First uniform predominant color */
      Ogre::String colorB;               /**< 2nd uniform predominant color */
      GoalKeeper* gKeeper;               /**< The goal keeper */
      TeamPlayer* disk[TEAM_MAX_DISKS];  /**< The disks */

      BaseAI* ai;                        /**< The AI who controlls the team */

      TeamPlayer* lastActiveDisk;    /**< Pointer to the last active, if any */

      Ogre::String logo;                 /**< The team Symbol */

      bool controlledByHuman;            /**< If controlled by human or AI */
      Ogre::SceneManager* scManager;     /**< The scene manager used */

      BulletDebugDraw* debugDraw; /**< Used for draw some debug info */

      Protocol protocol;                 /**< The communication protocol */
};

};

#endif

