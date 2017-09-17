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

#ifndef _btsoccer_core_h
#define _btsoccer_core_h

#include <kobold/platform.h>
#include <goblin/baseapp.h>

#if KOBOLD_PLATFORM == KOBOLD_PLATFORM_IOS
   #include "../net/gamecenternetwork.h"
#endif

#include "../btsoccer.h"
#include "../physics/forceio.h"
#include "../physics/collision.h"
#include "../physics/disttable.h"
#include "../gui/guiinitial.h"
#include "../gui/guimain.h"
#include "../gui/guimessage.h"
#include "../gui/guipause.h"
#include "../gui/guiscore.h"
#include "../gui/guisaves.h"
#include "../gui/guisocket.h"
#include "cup.h"
#include "rules.h"
#include "options.h"
#include "stats.h"
#include "teams.h"
#include "tutorial.h"
#include "../ai/baseai.h"
#include "../physics/bulletlink.h"
#include "../debug/bulletdebugdraw.h"
#include "../net/tcpnetwork.h"

namespace BtSoccer
{

/*! The main BtSoccer class, responsible for the main game loop. */
class Core : public Goblin::BaseApp
{
   public:

      enum
      {
         BTSOCCER_STATE_INITIAL_SCREEN,
         BTSOCCER_STATE_SLOTS_SCREEN,
         BTSOCCER_STATE_SOCKET_SCREEN,
         BTSOCCER_STATE_LOADING,
         BTSOCCER_STATE_PAUSED,
         BTSOCCER_STATE_NORMAL,
         BTSOCCER_STATE_GOAL_KEEPER_POSITION,
         BTSOCCER_STATE_DISK_POSITION,
         BTSOCCER_STATE_REPLAY,
         BTSOCCER_STATE_STATISTICS,
         BTSOCCER_STATE_OPTIONS,
         BTSOCCER_STATE_TUTORIAL,
         BTSOCCER_STATE_WAITING_CONNECTION,
         BTSOCCER_STATE_CONNECTING,
         BTSOCCER_STATE_WAITING_OTHER_SIDE_INIT_HALF
#if KOBOLD_PLATFORM == KOBOLD_PLATFORM_IOS
         ,BTSOCCER_STATE_GAME_CENTER_AUTHENTICATE,
         BTSOCCER_STATE_GAME_CENTER_MATCH_MAKER
#endif
      };

      /*! Constructor */
      Core(Goblin::CameraConfig& config);
      /*! Destructor */
      ~Core();
 
      /*! Set needed pointers to all subsystems */
      void setPointers();
   
      /*! Get the current ball */
      BtSoccer::Ball* getBall(){return(gameBall);};
   
#if KOBOLD_PLATFORM == KOBOLD_PLATFORM_IOS
      /*! Tell the application that will play a game accepted 
       * (or created by) invitation. */
      void sendWillStartGameByInvitation();
#endif

      /*! Pause the current match
       * \param receivedMessage true when received pause message
       *        from connection, false if user paused the game. */
      void pause(bool receivedMessage=false);
      /*! Resume the current match 
       * \param receivedMessage true when received resume message
       *        from connection, false if user resumed the game.*/
      void resume(bool receivedMessage=false);

      /*! Set core state constant.
       * \note -> only usefull when loading a game */
      void setState(int st);
   
      /*! Set the current selected player
       * \param player pointer to the team player to be selected. */
      void setSelectedPlayer(TeamPlayer* player);
   
      /*! Do a disk shoot.
       * \param player -> player to act to the shoot
       * \param initialX -> initial X force position 
       * \param initialZ -> initial Z force position
       * \param finalX -> final X force position
       * \param finalZ -> final Z force position */
      void doShoot(TeamPlayer* player, float initialX, float initialZ,
                   float finalX, float finalZ);

   protected:

      const Ogre::String getApplicationUnixName() const { return "btsoccer"; };
      const int getDefaultWindowWidth() const { return 1024; };
      const int getDefaultWindowHeight() const { return 720; };
      const Orientation getDefaultOrientation() const { return LANDSCAPE; };
      const int getMinWidthToUseDoubleSizedGui() const { return 2000; };
#if KOBOLD_PLATFORM == KOBOLD_PLATFORM_IOS ||\
    KOBOLD_PLATFORM == KOBOLD_PLATFORM_ANDROID
      const Ogre::String getBaseDataDir() const { return "data"; }
#else
      const Ogre::String getBaseDataDir() const { return "../data"; }
#endif      
      bool shouldQuit() { return shouldExit; };

      bool doCycleInit(int callCount, bool& shouldAbort);
      void getDataDirectories(Ogre::String** dataDirectories,
                  Ogre::String** dataGroups, int& total);
      void doLowMemoryClean();
      void doSendToBackground();
      void doSendToForeground();
      void doBeforeRender();
      void doAfterRender();
 

#if KOBOLD_PLATFORM == KOBOLD_PLATFORM_IOS
      /*! End anything happening before init an invited game. */
      void beforeInitInvitedGame();
#endif

      /*! Enter on replay Mode
       * @param goalReplay if entered replay to show a goal. */
      void enterReplayMode(bool goalReplay);
      /*! Exit from replay Mode */
      void exitReplayMode();
   
      /*! Update the game clock. Only need to check it when world is stable.
       * This function will check if the half ended, and will call statistics
       * window if needed. */
      void updateClock();

      /*! Verify Game Input Events */
      void gameIO();
      /*! Verify input for the normal game */
      void normalGameIO();
      /*! Verify input for camera
       * \return true if camera moved, false otherwise */
      bool cameraInput();
      /*! Verify input for disk on normal game */
      void diskIO();
      /*! Verify the input for goal keeper position definition */
      void goalKeeperPositionIO();
      /*! Restor things after done with GK Input. */
      void doneWithGoalKepperIO();
      /*! Verify input for disk position definition */
      void diskPositionIO();
      /*! Tell that will do a shoot
       * \param restric true to restric move to be at the line (usually on 
       * penalty shoots).
       * \param sendMessageIfOnline -> true to send message to other user
       * telling that will try a shoot, if is an online game. */
      void prepareToShoot(bool restric, bool sendMessageIfOnline);
      /*! To the previous calculated shoot or pass for the selected player
       * \return if shoot happened. */
      bool doTheShoot();
      /*! Set engine, based on last rules result 
       * \param stateAlreadySet must be true when state was defined by 
       *        protocol message.*/
      void verifyRulesResult(bool stateAlreadySet=false);
      /* Define the camera after rules. */
      void defineCamera();

      /*! Do the usual game cycle
       * \return -> true when exit the game */
      bool gameCycle();

      /*! Create and start a new match, based on filenames defined. */
      void newMatch();

      /*! Show the initial screen */
      void showInitialScreen();
      /*! Get desired phi camera angle for current acting player. */
      float getBaseCameraPhi();
   
      /*! Do all the online on-game related actions (receiveing and 
       * treating messages). */
      void doOnlineOnGameActions();
   
      /*! End online client or server, if any. */
      void endOnline();
 
      /*! End the current game (even if online) and go back to initial screen */
      void endCurrentGame();

      Ogre::RaySceneQuery* ogreRaySceneQuery;/**< To ray cast */
   
      Ogre::Vector3 fieldMouse;              /**< Mouse coord on field */
#if KOBOLD_PLATFORM == KOBOLD_PLATFORM_IOS ||\
    KOBOLD_PLATFORM == KOBOLD_PLATFORM_ANDROID
      Ogre::Vector3 fieldTouch2;             /**< Second touch field coord */
#endif

      int currentLoadState;                  /**< State when loading */
      bool singlePlayer;                     /**< If single player or not */
      int state;                             /**< Internal BtSoccer state */
      int previousState;                     /**< State before state change */
      int stateBeforePause;                  /**< State before called pause */
      int stateBeforeReplay;                 /**< State before called replay */
      BtSoccer::Field* btsoccerField;        /**< The field used */
      BtSoccer::Team* teamA;                 /**< The teamA */
      Ogre::String teamAFileName;            /**< Filename of team A */
      BtSoccer::Team* teamB;                 /**< The teamB */
      Ogre::String teamBFileName;            /**< Filename of team B */
      bool aiForTeamB;                       /**< If teamb will have AI */
      BtSoccer::Ball* gameBall;              /**< The ball */

      BtSoccer::Cup* cup;                    /**< Curent Cup, if any */

      bool shouldExit;                       /**< Exit flag. */
      bool initedTurn;                       /**< If already inited the turn */
      bool verifyCollisions;                 /**< If must verify collisions */
      bool enableIO;                         /**< If game input is enabled */

      BtSoccer::Collision coldet;            /**< The collision system */
      BtSoccer::ForceInput force;            /**< The force controller */
      TeamPlayer* selectedPlayer;            /**< Current selected player */
      TeamPlayer* teamPlayerUnder;           /**< teamPlayer under mouse */
      bool ballIsUnder;                      /**< if ball is under mouse */
      bool ballIsSelected;                   /**< if ball is selected */

      BtSoccer::GuiMain* guiMain;            /**< The GUI main */
      BtSoccer::GuiInitial* guiInitial;      /**< The initial GUI */
      BtSoccer::GuiPause* guiPause;          /**< The pause GUI */
      BtSoccer::GuiSaves* guiSaves;          /**< The save slots GUI */
      BtSoccer::GuiSocket* guiSocket;        /**< The gui for socket creation */

      BtSoccer::Tutorial* tutorial;          /**< Tutorial controller */

      BtSoccer::Replay* replayer;            /**< The Replayer */

      BulletDebugDraw* bulletDebugDraw;      /**< Debug draw for physics */
   
#if KOBOLD_PLATFORM == KOBOLD_PLATFORM_IOS
      bool mustInitInvitedGame; /**< True when received an invitation. */
      /*! Controller for running online games at game center. */
      BtSoccer::GameCenterController gameCenterController;
#endif

      BtSoccer::TcpServer* server;     /**< If online, acting as server. */
      BtSoccer::TcpClient* client;     /**< If online, acting as client. */
      BtSoccer::Protocol protocol;     /**< Online communication protocol. */
      bool onlineGame;                 /**< If doing an online game or not */
};

};

#endif

