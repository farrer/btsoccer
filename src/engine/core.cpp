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


#include <assert.h>

#include "core.h"
#include "replay.h"

   //#include "../btsoccerconfig.h"
#include "../soundfiles.h"

#include "ball.h"
#include "field.h"
#include "team.h"
#include "savefile.h"

#include "../ai/dummyai.h"
#include "../ai/fuzzyai.h"
#include "../ai/decourtai.h"

#include <kosound/sound.h>
#include <kobold/userinfo.h>
#include <kobold/i18n.h>
#include <kobold/timer.h>

#include <goblin/camera.h>
#include <goblin/screeninfo.h>

#include <iostream>

using namespace BtSoccer;


/***********************************************************************
 *                              Constructor                            *
 ***********************************************************************/
Core::Core(Goblin::CameraConfig& config)
     :BaseApp(config)
{
   shouldExit = false;
   ogreRaySceneQuery = NULL;

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
   mustInitInvitedGame = false;
#endif

   teamA = NULL;
   teamAFileName = "";
   teamB = NULL;
   teamBFileName = "";
   aiForTeamB = false;
   gameBall = NULL;
   btsoccerField = NULL;
   selectedPlayer = NULL;
   ballIsSelected = false;
   ballIsUnder = false;
   teamPlayerUnder = NULL;
   cup = NULL;
   initedTurn = true;
   verifyCollisions = false;
   bulletDebugDraw = NULL;

   server = NULL;
   client = NULL;
   onlineGame = false;

   replayer = NULL;
   guiMain = NULL;
   guiInitial = NULL;
   guiPause = NULL;
   guiSaves = NULL;
   guiSocket = NULL;
   tutorial = NULL;

   enableIO = true;

   state = BTSOCCER_STATE_INITIAL_SCREEN;
   previousState = BTSOCCER_STATE_NORMAL;
   stateBeforePause = BTSOCCER_STATE_NORMAL;
   stateBeforeReplay = BTSOCCER_STATE_NORMAL;

   /* Create bullet world */
   BulletLink::createBulletWorld();

   /* Set a new seed */
   srand((unsigned int)time(NULL));
};

/***********************************************************************
 *                               Destructor                            *
 ***********************************************************************/
Core::~Core()
{
   if(cup)
   {
      delete cup;
   }
   if(replayer)
   {
      delete replayer;
   }
   if(btsoccerField)
   {
      btsoccerField->deleteField();
      delete btsoccerField;
   }
   if(tutorial)
   {
      delete tutorial;
   }
   if(teamA)
   {
      delete teamA;
   }
   if(teamB)
   {
      delete teamB;
   }
   if(gameBall)
   {
      delete gameBall;
   }
   if(guiMain)
   {
      delete guiMain;
   }
   if(guiInitial)
   {
      delete guiInitial;
   }
   if(guiPause)
   {
      delete guiPause;
   }
   if(guiSaves)
   {
      delete guiSaves;
   }
   if(guiSocket)
   {
      delete guiSocket;
   }
   if(bulletDebugDraw)
   {
      delete bulletDebugDraw;
   }
   if(server)
   {
      delete server;
   }
   if(client)
   {
      delete client;
   }
   GuiMessage::finish();
   GuiScore::finish();
   Stats::finish();
   Regions::clear();
   DistTable::finish();
   
   if(ogreRaySceneQuery)
   {
      ogreSceneManager->destroyQuery(ogreRaySceneQuery);
   }

   BulletLink::deleteBulletWorld();
}

/***********************************************************************
 *                          lowMemoryClean                             *
 ***********************************************************************/
void Core::doLowMemoryClean()
{
   if(state != BTSOCCER_STATE_TUTORIAL)
   {
      /* Not on tutorial, we can delete it. */
      if(tutorial)
      {
         delete(tutorial);
      }
      tutorial = NULL;
   }
   
   if(state == BTSOCCER_STATE_INITIAL_SCREEN)
   {
      /* On initial screen, we can delete any loaded team */
      if(teamA)
      {
         delete(teamA);
         teamA = NULL;
      }
      if(teamB)
      {
         delete(teamB);
         teamB = NULL;
      }
   }
   else if ( (state != BTSOCCER_STATE_SLOTS_SCREEN) &&
             (state != BTSOCCER_STATE_SOCKET_SCREEN) &&
             (state != BTSOCCER_STATE_LOADING) &&
             (state != BTSOCCER_STATE_WAITING_CONNECTION) &&
             (state != BTSOCCER_STATE_CONNECTING) &&
             (state != BTSOCCER_STATE_WAITING_OTHER_SIDE_INIT_HALF)
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
           && (state != BTSOCCER_STATE_GAME_CENTER_AUTHENTICATE) &&
             (state != BTSOCCER_STATE_GAME_CENTER_MATCH_MAKER)
#endif
           )
   {
      /* We can clean initial screen pointer */
      if(guiInitial)
      {
         delete(guiInitial);
         guiInitial = NULL;
      }
   }
   /* Try to save some memory from texture and material managers */
   Ogre::TextureManager::getSingleton().unloadUnreferencedResources();
   Ogre::MaterialManager::getSingleton().unloadUnreferencedResources();
}

/***********************************************************************
 *                             setState                                *
 ***********************************************************************/
void Core::setState(int st)
{
   if(st == BTSOCCER_STATE_GOAL_KEEPER_POSITION)
   {
      /* Must make goals transparent. */
      btsoccerField->getGoals()->setTransparent();
   }
   previousState = state;
   state = st;
}

/***********************************************************************
 *                         sendToBackground                            *
 ***********************************************************************/
void Core::doSendToBackground()
{
   if( (state != BTSOCCER_STATE_INITIAL_SCREEN) &&
       (state != BTSOCCER_STATE_PAUSED) &&
       (state != BTSOCCER_STATE_CONNECTING) &&
       (state != BTSOCCER_STATE_WAITING_CONNECTION) &&
       (state != BTSOCCER_STATE_SOCKET_SCREEN) &&
       (state != BTSOCCER_STATE_REPLAY) &&
       (state != BTSOCCER_STATE_STATISTICS) &&
       (state != BTSOCCER_STATE_LOADING) &&
       (state != BTSOCCER_STATE_TUTORIAL) )
   {
      pause();
   }
}
/***********************************************************************
 *                         sendToForeground                            *
 ***********************************************************************/
void Core::doSendToForeground()
{
}

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
/***********************************************************************
 *                      beforeInitInvitedGame                          *
 ***********************************************************************/
void Core::beforeInitInvitedGame()
{
   mustInitInvitedGame = false;
   /* TODO: must end any inited game. */
   switch(state)
   {
      case BTSOCCER_STATE_TUTORIAL:
      {
         /* Must end tutorial*/
         delete(tutorial);
         tutorial = NULL;
      }
      break;
      case BTSOCCER_STATE_PAUSED:
      {
         /* Must resume game, and end it. */
         resume();
      }
      case BTSOCCER_STATE_NORMAL:
      case BTSOCCER_STATE_DISK_POSITION:
      case BTSOCCER_STATE_GOAL_KEEPER_POSITION:
      case BTSOCCER_STATE_REPLAY:
      case BTSOCCER_STATE_STATISTICS:
      case BTSOCCER_STATE_WAITING_OTHER_SIDE_INIT_HALF:
      {
         /* Must end current match */
         if(onlineGame)
         {
            endOnline();
         }
         if(guiMain != NULL)
         {
            guiMain->hide();
         }
         GuiScore::hide();
      }
      break;
      case BTSOCCER_STATE_SLOTS_SCREEN:
      {
         if(guiSaves != NULL)
         {
            delete guiSaves;
            guiSaves = NULL;
         }
      }
      break;
      case BTSOCCER_STATE_CONNECTING:
      case BTSOCCER_STATE_SOCKET_SCREEN:
      case BTSOCCER_STATE_WAITING_CONNECTION:
      {
         endOnline();
         if(guiSocket != NULL)
         {
            delete guiSocket;
            guiSocket = NULL;
         }
      }
      break;
      case BTSOCCER_STATE_GAME_CENTER_AUTHENTICATE:
      case BTSOCCER_STATE_GAME_CENTER_MATCH_MAKER:
      break;
   }
   showInitialScreen();
   state = BTSOCCER_STATE_GAME_CENTER_MATCH_MAKER;
   
   /* Set a random team to play with. */
   guiInitial->setRandomTeamA();
   protocol.setTeam(guiInitial->getTeamA());
   
   /* TODO: must set some way to select a team, instead of 
    * chosing a random one. */
}

/***********************************************************************
 *                   sendWillStartGameByInvitation                     *
 ***********************************************************************/
void Core::sendWillStartGameByInvitation()
{
   mustInitInvitedGame = true;
}
#endif

/***********************************************************************
 *                               pause                                 *
 ***********************************************************************/
void Core::pause(bool receivedMessage)
{
   /* pause the clock */
   Rules::pause();

   /* Set state as Paused */
   stateBeforePause = state;
   state = BTSOCCER_STATE_PAUSED;

   /* Define active GUI */
   guiPause->show((guiMain->isActive() && (!onlineGame)), receivedMessage);
   guiMain->hide();

   if((onlineGame) && (!receivedMessage))
   {
      protocol.queuePause();
   }
}

/***********************************************************************
 *                              resume                                 *
 ***********************************************************************/
void Core::resume(bool receivedMessage)
{
   Rules::resume();
   state = stateBeforePause;
   guiMain->restoreState();
   guiPause->hide();
   if((onlineGame) && (!receivedMessage))
   {
      protocol.queueResume();
   }
}

/***********************************************************************
 *                          getDataDirectories                         *
 ***********************************************************************/
void Core::getDataDirectories(Ogre::String** dataDirectories,
      Ogre::String** dataGroups, int& total)
{
   Ogre::String guiDir = "gui";
   Ogre::String fontsDir = "fonts";
   if(Goblin::ScreenInfo::shouldUseDoubleSizedGui())
   {
      guiDir += "2x";
      fontsDir += "2x";
   }
   static Ogre::String dirs[] = { "other", "game", guiDir,
      "sndfx", "lang", fontsDir}; 
   (*dataDirectories) = &dirs[0];
   static Ogre::String groups[] = { "other", "game", "gui",
      "sndfx", "lang", "fonts"}; 
   (*dataGroups) = &groups[0];
   total = 6;
}

/***********************************************************************
 *                            doCycleInit                              *
 ***********************************************************************/
bool Core::doCycleInit(int callCount, bool& shouldAbort)
{
   /* Load current options */ 
   Options::load();

   /* Load precalculated distance values */
   DistTable::init(true);

   /* Define camera position */
   Goblin::Camera::set(0.0f, 1.5f, -0.25f, -166.5f, 35.0f, 30.0f);

   /* Load all teams' info */
   Regions::load();

   /* Init the messages controller */
   GuiMessage::init("main/info.png");
   GuiScore::init();
   
   guiMain = new GuiMain(GuiScore::getOverlay(), ogreSceneManager);
   guiMain->hide();
   showInitialScreen();

   /* Create the debug drawer for physics */
#if BTSOCCER_RENDER_DEBUG
   bulletDebugDraw = new BulletDebugDraw(ogreSceneManager);
   bulletDebugDraw->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
   BulletLink::setDebugDraw(bulletDebugDraw);
   BulletLink::debugDraw();
#else
   bulletDebugDraw = NULL;
#endif
   
   guiPause = new GuiPause();
   Stats::init();

   /* Create a SceneQuery */
   ogreRaySceneQuery = ogreSceneManager->createRayQuery(Ogre::Ray());

   /* Load Ball */
   gameBall = new BtSoccer::Ball(ogreSceneManager, bulletDebugDraw);

   /* Load Field */
   btsoccerField = new BtSoccer::Field();
   btsoccerField->createField(Options::getFieldFile(), ogreSceneManager);

   /* Create and set the replayer */
   replayer = new Replay(btsoccerField);

   /* Set ambient light */
   ogreSceneManager->setAmbientLight(Ogre::ColourValue(0.72f, 0.72f, 0.72f));
   ogreSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
   
   return true;
}

/***********************************************************************
 *                            doAfterRender                            *
 ***********************************************************************/
void Core::doAfterRender()
{
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
   if(mustInitInvitedGame)
   {
      beforeInitInvitedGame();
   }
#endif

   /* Do the global update to the main GUI */
   guiMain->update();
   GuiScore::update();

   /* Update messages GUI and FPS display */
   GuiMessage::update();

   /* Update tutorial, if defined */
   if(tutorial != NULL)
   {
      if(!tutorial->update(mouseX, mouseY, leftButtonPressed))
      {
         /* Done with tutorial */
         delete tutorial;
         tutorial = NULL;

         /* back to main menu */
         showInitialScreen();
      }
   }

   switch(state)
   {
      case BTSOCCER_STATE_INITIAL_SCREEN:
      {
         int res;
         res = guiInitial->verifyEvents(mouseX, mouseY, 
               leftButtonPressed, btsoccerField);
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
         shouldExit |= (res == GuiInitial::RETURN_QUIT);
#endif

         if( (res == GuiInitial::RETURN_SINGLE_QUICK_MATCH) ||
               (res == GuiInitial::RETURN_VERSUS_QUICK_MATCH) )
         {
            /* Create a new singe match */
            currentLoadState = 0;
            teamAFileName = guiInitial->getTeamA();
            teamBFileName = guiInitial->getTeamB();
            aiForTeamB = (res == GuiInitial::RETURN_SINGLE_QUICK_MATCH);
            onlineGame = false;
            newMatch();
            state = BTSOCCER_STATE_LOADING;
         }
         else if(res == GuiInitial::RETURN_LOAD_QUICK_MATCH)
         {
            /* Show the Save-Slot Window for load */
            guiSaves = new GuiSaves("qm_saves.lst", ".qms");
            guiSaves->show(false);
            state = BTSOCCER_STATE_SLOTS_SCREEN;
         }
         else if(res == GuiInitial::RETURN_CREATE_SERVER)
         {
            /* Show server/client window to select port. */
            guiSocket = new GuiSocket(true);
            state = BTSOCCER_STATE_SOCKET_SCREEN;
         }
         else if(res == GuiInitial::RETURN_CONNECT_SERVER)
         {
            /* Show server/client window to select
             * address and port. */
            guiSocket = new GuiSocket(false);
            state = BTSOCCER_STATE_SOCKET_SCREEN;
         }
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
         else if(res == GuiInitial::RETURN_CONNECT_GAME_CENTER)
         {
            if(!gameCenterController.isCanceled())
            {
               protocol.setTeam(guiInitial->getTeamA());
               /* Try a connection to game center */
               gameCenterController.authenticate();
               state = BTSOCCER_STATE_GAME_CENTER_AUTHENTICATE;
            }
            else
            {
               /* Game center is canceled by the user before,
                * must return to initial screen*/
               GuiMessage::set("Game Center was canceled.");
               showInitialScreen();
            }
         }
#endif
         else if(res == GuiInitial::RETURN_NEW_CUP)
         {
            /* Create a new cup */
            if(cup)
            {
               delete cup;
            }
            cup = new Cup(guiInitial->getTeamA());
            /* TODO: Enter cup state! */
         }
         else if(res == GuiInitial::RETURN_TUTORIAL_CAMERA)
         {
            /* Create the tutorial at the camera lessons */
            if(!tutorial)
            {
               tutorial = new Tutorial(this, &teamA, &teamB, bulletDebugDraw);
            }
            tutorial->set(TUTORIAL_CAMERA);
            state = BTSOCCER_STATE_TUTORIAL;
         }
         else if(res == GuiInitial::RETURN_TUTORIAL_GAME)
         {
            /* Create the tutorial at the game lessons */
            if(!tutorial)
            {
               tutorial = new Tutorial(this, &teamA, &teamB, bulletDebugDraw);
            }
            tutorial->set(TUTORIAL_GAME);
            state = BTSOCCER_STATE_TUTORIAL;
         }
      }
      break;
      case BTSOCCER_STATE_SOCKET_SCREEN:
      {
         int ev = guiSocket->verifyEvents(mouseX, mouseY,
               leftButtonPressed);
         if(ev == GuiSocket::RETURN_CANCEL)
         {
            delete guiSocket;
            guiSocket = NULL;
            showInitialScreen();
         }
         else if(ev == GuiSocket::RETURN_CONNECT)
         {
            /* Create client and try connection */
            client = new TcpClient(guiInitial->getTeamA(),
                  Options::getFieldSize());
            if(client->connect(guiSocket->getPort(),
                     guiSocket->getAddress()))
            {
               client->createThread();
               state = BTSOCCER_STATE_CONNECTING;
               delete guiSocket;
               guiSocket = NULL;
            }
            else
            {
               GuiMessage::set("Connection failed!");
               guiSocket->show();
               delete client;
               client = NULL;
            }
         }
         else if(ev == GuiSocket::RETURN_CREATE)
         {
            server = new TcpServer(guiSocket->getPort(),
                  guiInitial->getTeamA(),
                  Options::getFieldSize());
            server->init();
            server->createThread();
            state = BTSOCCER_STATE_WAITING_CONNECTION;
         }
      }
      break;
      case BTSOCCER_STATE_WAITING_CONNECTION:
      {
         /* Wait until received a set team by the
          * client who tries a connection. */
         ProtocolParsedMessage msg;
         while(protocol.getNextReceivedMessage(&msg))
         {
            if(msg.msgType == MESSAGE_SET_TEAM)
            {
               currentLoadState = 0;
               teamAFileName = protocol.getTeam();
               teamBFileName = msg.str;
               aiForTeamB = false;
               onlineGame = true;
               if(guiSocket != NULL)
               {
                  delete guiSocket;
                  guiSocket = NULL;
               }
               newMatch();
               state = BTSOCCER_STATE_LOADING;
               return;
            }
         }

         /* Verify Cancel Button! */
         if( (guiSocket != NULL) &&
             (guiSocket->verifyEvents(mouseX, mouseY, leftButtonPressed) == 
              GuiSocket::RETURN_CANCEL) )
         {
            if(server)
            {
               delete server;
               server = NULL;
            }
            delete guiSocket;
            guiSocket = NULL;
            showInitialScreen();
         }

         /** TODO: message of connection!*/

      }
      break;
      case BTSOCCER_STATE_CONNECTING:
      {
         /* Wait until received a set team by the server.
          * Note that before, will receive a setField */
         ProtocolParsedMessage msg;
         while(protocol.getNextReceivedMessage(&msg))
         {
            if(msg.msgType == MESSAGE_SET_TEAM)
            {
               protocol.queueSetTeam(guiInitial->getTeamA());
               currentLoadState = 0;
               teamAFileName = msg.str;
               teamBFileName = protocol.getTeam();
               aiForTeamB = false;
               onlineGame = true;
               newMatch();
               state = BTSOCCER_STATE_LOADING;
            }
            else if(msg.msgType == MESSAGE_SET_FIELD)
            {
               /* Verify if field is different */
               if(Options::getFieldSize() != msg.msgInfo)
               {
                  btsoccerField->createField(Options::getFieldFile(
                           msg.msgInfo), ogreSceneManager);
               }
               /* TODO: Change back field after game! */
            }
         }

      }
      break;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
      case BTSOCCER_STATE_GAME_CENTER_AUTHENTICATE:
      {
         /* Wait authentication subview ends */
         if(!gameCenterController.isAuthenticating())
         {
            if(gameCenterController.isEnabled())
            {
               /* Open game center match maker view */
               gameCenterController.hostMatch();
               state = BTSOCCER_STATE_GAME_CENTER_MATCH_MAKER;
            }
            else
            {
               GuiMessage::set("Game Center was canceled.");
               showInitialScreen();
            }
         }

      }
      break;
      case BTSOCCER_STATE_GAME_CENTER_MATCH_MAKER:
      {
         /* Wait game center match creation process ends */
         if(gameCenterController.matchCreationFailed())
         {
            GuiMessage::set("Match creation failed.");
            showInitialScreen();
         }
         else if(gameCenterController.isMatchCanceled())
         {
            GuiMessage::set("Game Center was canceled.");
            showInitialScreen();
         }
         else if(gameCenterController.isMatchReady())
         {
            /* Init game center thread */
            gameCenterController.createThread();
            /* Init the protocol as defined */
            protocol.initProtocol(true, Options::getFieldSize());
            protocol.setIsTeamA(gameCenterController.isTeamA());

            if(gameCenterController.isTeamA())
            {
               /* Must first act as the server */
               state = BTSOCCER_STATE_WAITING_CONNECTION;
            }
            else
            {
               /* Must first act as client */
               state = BTSOCCER_STATE_CONNECTING;
               protocol.queueHello();
            }
         }
         else if(gameCenterController.isMatchDefined())
         {
            GuiMessage::set("Wait: Defining order");
         }
         else
         {
            GuiMessage::set("Defining match");
         }
      }
      break;
#endif
      case BTSOCCER_STATE_LOADING:
      {
         newMatch();
      }
      break;
      case BTSOCCER_STATE_SLOTS_SCREEN:
      {
         int res;
         res = guiSaves->verifyEvents(mouseX, mouseY, 
               leftButtonPressed);
         if(res != GuiSaves::ACTION_NONE)
         {
            if(res == GuiSaves::ACTION_LOAD)
            {
               //FIXME: use new match is better. Adapt game load for it.
               if(teamA)
               {
                  delete teamA;
                  teamA = NULL;
               }
               if(teamB)
               {
                  delete teamB;
                  teamB = NULL;
               }

               /* FIXME: On Cups, what to do? */
               SaveFile sf;
               if(!sf.load(Kobold::UserInfo::getSaveDirectory() + 
                        guiSaves->getSelectedFileName(),
                        this, &teamA, &teamB, btsoccerField, bulletDebugDraw))
               {
                  Ogre::LogManager::getSingleton().stream(
                        Ogre::LML_CRITICAL)
                     << "Error loading saved file '" 
                     << guiSaves->getSelectedFileName() << "'";

               }
               else
               {
                  /* Show main gui */
                  guiMain->show();
               }
               //XXX: Workaround to fix any strange first physics state.
               for(int j=0; j<200;j++)
               {
                  BulletLink::forcedStep();
               }
            }
            else if(res == GuiSaves::ACTION_SAVE)
            {
               /* FIXME: When at Cup, not only on qm! */
               SaveFile save;
               resume();
               save.save(Kobold::UserInfo::getSaveDirectory() + 
                     guiSaves->getSelectedFileName(), 
                     teamA, teamB, state);
               /* Update slots definitions */
               guiSaves->saveCurrentSlotFiles();
            }
            else if(res == GuiSaves::ACTION_CANCEL)
            {
               /* Go back to the last state, re-showing its GUI. */
               if(guiSaves->isSaveMode())
               {
                  /* Must return to pause mode (call resume
                   * to resume last state, than pause it again) */
                  resume();
                  pause();
               }
               else
               {
                  /* Must return to initial screen */
                  showInitialScreen();
               }
            }

            /* Done with the save-slots gui input, must exit */
            delete(guiSaves);
            guiSaves = NULL;
         }
      }
      break;
      case BTSOCCER_STATE_PAUSED:
      {
         if(onlineGame)
         {
            /* If on online mode, must check if received resume. */
            doOnlineOnGameActions();
         }
         int res = guiPause->verifyEvents(mouseX, mouseY, 
               leftButtonPressed);

         if(res == GuiPause::RETURN_SAVE)
         {
            /* Open the save slot */
            guiSaves = new GuiSaves("qm_saves.lst", ".qms");
            guiSaves->show(true);
            state = BTSOCCER_STATE_SLOTS_SCREEN;
         }
         else if(res == GuiPause::RETURN_QUIT)
         {
            /* Quit game. Enter Initial Screen */
            endCurrentGame();
         }
         else if(res == GuiPause::RETURN_RESUME)
         {
            resume();
         }
      }
      break;
      case BTSOCCER_STATE_WAITING_OTHER_SIDE_INIT_HALF:
      {
         GuiMessage::set("Waiting opponent");
         doOnlineOnGameActions();
      }
      break;
      case BTSOCCER_STATE_STATISTICS:
      {
         if(Stats::verifyEvents(mouseX, mouseY, leftButtonPressed, onlineGame))
         {
            Stats::hide();

            /* Verify if is full time or half time done */
            if(Rules::isFirstHalf())
            {
               /* Just start second half */
               Rules::startHalf(false);
               /* Set the camera to middle position */
               Goblin::Camera::setTarget(0.0f, 30.0f, -5.0f, -90.0f, 
                     33.0f, 335.0f);
               guiMain->show();
               if(!onlineGame)
               {
                  /* Set the state to normal*/
                  state = BTSOCCER_STATE_NORMAL;
               }
               else
               {
                  state = BTSOCCER_STATE_WAITING_OTHER_SIDE_INIT_HALF;
                  if(protocol.isTeamA())
                  {
                     /* TeamA must be the first to send begin half,
                      * alwyas. TeamB will set after receiveing it. */
                     protocol.queueBeginHalf();
                  }
               }
            }
            else
            {
               /* TODO: Check if cup!! */
               /* Go back to main menu */
               showInitialScreen();
               GuiScore::hide();
               if(onlineGame)
               {
                  endOnline();
               }
            }
         }
      }
      break;
      case BTSOCCER_STATE_TUTORIAL:
      default:
      {               
         /* Do the usual game cycle */
         if(gameCycle())
         {
            /* Called to exit. Must pause the game. */
            pause();
         }
      }
      break;
   }
}

/***********************************************************************
 *                           doBeforeRender                            *
 ***********************************************************************/
void Core::doBeforeRender()
{
#if BTSOCCER_RENDER_DEBUG
   btsoccerField->debugDraw();
   #if BTSOCCER_DEBUG_AI
   Team* actTeam = Rules::getActiveTeam();
   if((actTeam != NULL) && (actTeam->getAI() != NULL))
   {
      actTeam->getAI()->debugDraw();      
   }
   #endif
#endif
}

/********************************************************************
 *                         gameCycle                                *
 ********************************************************************/
bool Core::gameCycle()
{
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS &&\
    OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
   /* Verify keyboard */
   if(Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_ESCAPE))
   {
      if((state != BTSOCCER_STATE_REPLAY) && (state != BTSOCCER_STATE_PAUSED) &&
         (state != BTSOCCER_STATE_TUTORIAL))
      {
         pause();
      }
   }


   if( (Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_R)) &&
         (state == BTSOCCER_STATE_NORMAL) )
   {
      enterReplayMode(false);
   }

#endif

   teamPlayerUnder = NULL;

   /* Do a ray cast */
   Ogre::Ray mouseRay;
   Goblin::Camera::getCameraToViewportRay(
         mouseX / Ogre::Real(ogreWindow->getWidth()),
         mouseY / Ogre::Real(ogreWindow->getHeight()), &mouseRay);

   /* Ray cast to Y=0 plane, to calculate mouse World coordinate */
   std::pair< bool, Ogre::Real > res;
   res = Ogre::Math::intersects(mouseRay, 
         Ogre::Plane(Ogre::Vector3(0.0f, 1.0f, 0.0f), 0.0f));
   if(res.first)
   {
      fieldMouse = mouseRay.getPoint(res.second);
   }
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS ||\
    OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
   if(Kobold::MultiTouchController::totalTouches() >= 2)
   {
      Ogre::Ray secRay;
      Kobold::TouchInfo touch;
      Kobold::MultiTouchController::getTouch(1, touch);
      
      Goblin::Camera::getCameraToViewportRay(
          touch.x / Ogre::Real(ogreWindow->getWidth()),
          touch.y / Ogre::Real(ogreWindow->getHeight()), &secRay);
      
      res = Ogre::Math::intersects(secRay, Ogre::Plane(Ogre::Vector3(0.0f,
                  1.0f, 0.0f), 0.0f));
      if(res.first)
      {
         fieldTouch2 = secRay.getPoint(res.second);
      }
   }
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS ||\
    OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
   /* When on multitouch interfaces, only need to get teamPlayer
    * under 'mouse' (first finger) if we are at a single touch moment. */
   if(Kobold::MultiTouchController::totalTouches() == 1)
   {
#endif
   /* Execute query */
   ogreRaySceneQuery->setRay(mouseRay);
   ogreRaySceneQuery->setSortByDistance(true);
   Ogre::RaySceneQueryResult &result = ogreRaySceneQuery->execute();
   Ogre::RaySceneQueryResult::iterator itr;

   for( itr = result.begin( ); itr != result.end(); itr++ )
   {
      if(itr->movable)
      {
         /* Only get if not the field */
         if(itr->movable->getName().find(Ogre::String("Field")) == 
               Ogre::String::npos )
         {
            /* Let's get the teamPlayer under mouse, if any */
            Ogre::SceneNode* obj = itr->movable->getParentSceneNode();
            
            ballIsUnder = gameBall->compare(obj);

            if(Rules::getActiveTeam() != NULL)
            {
               teamPlayerUnder =
                  Rules::getActiveTeam()->getTeamPlayer(obj);
            }
         }
      }
   }
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS ||\
    OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
   }
#endif

#if 0
   //Uncomment to debug function hasFreeWayTo
   if((teamPlayerUnder) && (gameBall))
   {
      teamPlayerUnder->hasFreeWayTo(ogreRaySceneQuery, 
            gameBall->getPosition().x, gameBall->getPosition().z);
      bulletDebugDraw->update();
   }
#endif
   
   /* Check if ball is inner acting team little area and we are not setting
    * a goal keeper position. In this case, set goal and gk to be transparent,
    so we'll always see the ball. */
   if( (state != BTSOCCER_STATE_GOAL_KEEPER_POSITION) &&
       (state != BTSOCCER_STATE_TUTORIAL) && (Rules::getActiveTeam() != NULL) )
   {
      bool isUpperTeam =
      (Rules::getActiveTeam() == Rules::getUpperTeam());
      if( (state != BTSOCCER_STATE_REPLAY) &&
          (btsoccerField->isInnerPenaltyArea(gameBall->getPosition().x,
                                             gameBall->getPosition().z,
                                             isUpperTeam, !isUpperTeam)) )
      {
         Rules::getActiveTeam()->getGoalKeeper()->setTransparent();
         Rules::getInactiveTeam()->getGoalKeeper()->setOpaque();
         btsoccerField->getGoals()->setTransparent();
      }
      else
      {
         Rules::getActiveTeam()->getGoalKeeper()->setOpaque();
         Rules::getInactiveTeam()->getGoalKeeper()->setOpaque();
         btsoccerField->getGoals()->setOpaque();
      }
   }

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS ||\
    OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
   if( (state == BTSOCCER_STATE_GOAL_KEEPER_POSITION) &&
      (Kobold::MultiTouchController::totalTouches() == 2) &&
      (btsoccerField->isInnerLittleArea(fieldMouse.x, fieldMouse.z)))
   {
      /* Inner little area and rotating GK, no need to check camera */
      if(enableIO)
      {
         normalGameIO();
      }
   }
#endif
   /* Only do normal input if not moved the camera, and if enabled */
   if(!receivedCameraInput)
   {
      if(enableIO)
      {
         normalGameIO();
      }
   }

   if(onlineGame)
   {
      doOnlineOnGameActions();
   }


   /* Only need to update the physics or replay at normal state,
    * or in TUTORIAL */
   if( (state == BTSOCCER_STATE_NORMAL) ||
       (state == BTSOCCER_STATE_TUTORIAL) )
   {
      /* Do the Physics! */
      if(verifyCollisions)
      {
         if( (!onlineGame) || 
             (Rules::getActiveTeam()->isControlledByHuman()) )
         {
            /* Do the bullet step */
            /* FIXME: Must set timeElapsed, and ignore subSteps! */
            BulletLink::step(timeElapsed, 10);
            if(onlineGame)
            {
               /* Must queue all updates or, if physics is stable,
                * must queue all positions, to make sure they are ok
                * at the other side (with ack). */
               BulletLink::queueUpdatesToProtocol(BulletLink::isWorldStable());
            }

            //FIXME: Follow ball and gui hide/show on ONLINE mode!
            if(BulletLink::isWorldStable())
            {
               if( (state == BTSOCCER_STATE_NORMAL) &&
                     (!initedTurn) )
               {
                  guiMain->show();
                  verifyRulesResult();
               }
               updateClock();
               verifyCollisions = false;
               enableIO = true;
            }
            else
            {
               if(state == BTSOCCER_STATE_NORMAL)
               {
                  /* update replay data */
                  replayer->updateData();
                  if(enableIO)
                  {
                     guiMain->hide();
                     enableIO = false;
                  }
               }

               /* Follow ball, if the camera is defined to it */
               if(Options::getCameraType() ==
                     Options::CAMERA_FOLLOW_BALL)
               {
                  Goblin::Camera::set(gameBall->getPosition().x,
                        0.0f, gameBall->getPosition().z,
                        Goblin::Camera::getPhi(), Goblin::Camera::getTheta(),
                        Goblin::Camera::getZoom());
               }
            }

         }
      }
   }
   else if(state == BTSOCCER_STATE_REPLAY)
   {
      if(replayer->verifyEvents(mouseX, mouseY, leftButtonPressed) ==
            Replay::EVENT_EXIT)
      {
         /* Done with replay */
         exitReplayMode();
      }
   }

   return shouldExit;
}

/********************************************************************
 *                           endCurrentGame                         *
 ********************************************************************/
void Core::endCurrentGame()
{
   guiMain->hide();
   GuiScore::hide();
   showInitialScreen();
   endOnline();
}

/********************************************************************
 *                            updateClock                           *
 ********************************************************************/
void Core::updateClock()
{
   //FIXME: calculate shouldUpdateClock outside, instead of every frame.
   bool shouldUpdateClock = false;
   
   if(!onlineGame)
   {
      /* Not online, should update. */
      shouldUpdateClock = true;
   }
   else if(protocol.isTeamA())
   {
      /* Online and is teamA, should update */
      shouldUpdateClock = true;
   }

   if(shouldUpdateClock)
   {
      if( (state == BTSOCCER_STATE_NORMAL) &&
          (Rules::updateClock()) )
      {
         /* Time Over */
         Stats::show();
         guiMain->hide();
         state = BTSOCCER_STATE_STATISTICS;
         if(onlineGame)
         {
            protocol.queueEndHalf();
         }
      }
   }
}

/********************************************************************
 *                      doOnlineOnGameActions                       *
 ********************************************************************/
void Core::doOnlineOnGameActions()
{
   /* Check messages received */
   bool updatedPositions = false;
   bool manualInput = false;
   ProtocolParsedMessage msg;
   while(protocol.getNextReceivedMessage(&msg))
   {
      switch(msg.msgType)
      {
         case MESSAGE_UPDATE_POSITIONS:
         {
            if( (!Rules::getActiveTeam()->isControlledByHuman()) ||
               (state == BTSOCCER_STATE_GOAL_KEEPER_POSITION) )
            {
               /* Other side is active, must set update */
               if(msg.msgInfo == UPDATE_TYPE_BALL)
               {
                  /* Update ball position */
                  gameBall->setOrientation(msg.angles);
                  gameBall->setPositionWithoutForcedPhysicsStep(
                                                                msg.position);
                  updatedPositions = true;
                  manualInput = false;
               }
               else if( (msg.msgInfo == UPDATE_TYPE_TEAM_A) ||
                        (msg.msgInfo == UPDATE_TYPE_MANUAL_TEAM_A_INPUT) )
               {
                  /* Update a teamplayer position of team A */
                  teamA->update(msg);
                  updatedPositions = true;
                  manualInput = (msg.msgInfo==UPDATE_TYPE_MANUAL_TEAM_A_INPUT);
               }
               else if( (msg.msgInfo == UPDATE_TYPE_TEAM_B) ||
                        (msg.msgInfo == UPDATE_TYPE_MANUAL_TEAM_B_INPUT) )
               {
                  /* Update a teamplayer position of team B */
                  teamB->update(msg);
                  updatedPositions = true;
                  manualInput = (msg.msgInfo==UPDATE_TYPE_MANUAL_TEAM_B_INPUT);
               }
            }
         }
         break;
         case MESSAGE_RULES_RESULT:
         {
            /* Set the rules and do changes according to it. */
            Rules::set(msg);
            Rules::showStateMessage();
            verifyRulesResult(true);
            BulletLink::preStep();
            verifyCollisions = false;
            enableIO = true;
         }
         break;
         case MESSAGE_GOAL:
         {
            /* A goal happened. Must set score. */
            
            /* Note: no need to call replay, as it will be called when received
             * the rules update to MIDDLE status. */
            GuiMessage::set("Goal!");
            if(msg.msgInfo == UPDATE_TYPE_TEAM_A)
            {
               GuiScore::goalTeamA();
            }
            else
            {
               GuiScore::goalTeamB();
            }
         }
         break;
         case MESSAGE_WILL_SHOOT:
         {
            /* Set the engine to prepare to shoot */
            prepareToShoot(
               (Rules::getState() == Rules::STATE_PENALTY_KICK),
               false);
         }
         break;
         case MESSAGE_GOAL_KEEPER_DONE:
         {
            /* Received that the other side finished the goal keeper
             *positionate: must finish it and start the shoot. */
            doneWithGoalKepperIO();
         }
         break;
         case MESSAGE_PLAY_SOUND:
         {
            Ogre::String soundFile;
            
            if(msg.msgInfo == SOUND_TYPE_DISK_ACT)
            {
               soundFile = BTSOCCER_SOUND_DISK_SHOOT;
            }
            else
            {
               soundFile = BTSOCCER_SOUND_DISK_COLLISION;
            }
            
            /* Play a collision sound */
            Kosound::Sound::addSoundEffect(msg.position.x,
                  msg.position.y, msg.position.z, SOUND_NO_LOOP, soundFile);
         }
         break;
         case MESSAGE_PAUSE:
         {
            GuiMessage::set("Your opponent paused the game.");
            pause(true);
         }
         break;
         case MESSAGE_RESUME:
         {
            GuiMessage::set("Your opponent resumed the game.");
            resume(true);
         }
         break;
         case MESSAGE_END_HALF:
         {
            /* Received Time Over */
            Stats::show();
            guiMain->hide();
            state = BTSOCCER_STATE_STATISTICS;
         }
         break;
         case MESSAGE_BEGIN_HALF:
         {
            if(state == BTSOCCER_STATE_WAITING_OTHER_SIDE_INIT_HALF)
            {
               /* Send begin half, if is "second team" (teamB) */
               if(!protocol.isTeamA())
               {
                  protocol.queueBeginHalf();
               }
               /* Set the state to normal */
               if(Rules::isFirstHalf())
               {
                  GuiMessage::set("The Match Begins!");
               }
               state = BTSOCCER_STATE_NORMAL;
            }
         }
         break;
      }
   }
   if(updatedPositions)
   {
      BulletLink::forcedStep();
      /* Call the preStep to clear all flags.
       * Note: it's after the step to avoid the receiver
       * try to do some physics-after step related actions. */
      BulletLink::preStep();
      if(!manualInput)
      {
         /* update replay data */
         replayer->updateData();
      }
   }
   
   /* Verify if still connected */
   bool stillConnected = true;
   
   if( ( (server != NULL) && (server->getTotal() == 0) ) ||
       ( (client != NULL) && (!client->isRunning()) ) )
   {
      stillConnected = false;
   }

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
   if( protocol.isUsingGameCenter() )
   {
      stillConnected = gameCenterController.isMatchDefined();
      if(!stillConnected)
      {
         gameCenterController.finishMatch();
      }
   }
#endif
   
   if(!stillConnected)
   {
      /* Got disconnection. Must end game. */
      GuiMessage::set("Connection closed.");
      if(guiPause)
      {
         guiPause->hide();
      }
      GuiScore::hide();
      endOnline();
      showInitialScreen();
   }
}

/********************************************************************
 *                        normalGameIO                              *
 ********************************************************************/
void Core::normalGameIO()
{
   int event;
   
   /* Verify Main GUI Actions */
   event = guiMain->verifyEvents(mouseX, mouseY, leftButtonPressed);
   if( (event != GuiMain::EVENT_NONE) &&
       (event != GuiMain::BUTTON_ACCEPT))
   {
      switch(event)
      {
         case GuiMain::BUTTON_SHOOT:
         {
            if(Rules::getActiveTeam()->isControlledByHuman())
            {
               /* Only Input GUI on SHOOT when player's turn */
               prepareToShoot(false, true);
            }
         }
         break;
         case GuiMain::BUTTON_REPLAY:
         {
            /* Enter replay mode */
            enterReplayMode(false);
         }
         break;
         case GuiMain::BUTTON_PAUSE:
         {
            pause();
         }
         break;
      }
      
      /* Must return (done with verifications) */
      return;
   }
   
   /* Verify player input */
   if( (state == BTSOCCER_STATE_NORMAL) ||
       ( (state == BTSOCCER_STATE_TUTORIAL) &&
         (tutorial->isDiskInputEnabled()) ) )
   {
      diskIO();
   }
   else if( (state == BTSOCCER_STATE_DISK_POSITION) ||
           ( (state == BTSOCCER_STATE_TUTORIAL) &&
             (tutorial->isDiskPositionInputEnabled()) ) )
   {
      diskPositionIO();
   }
   else if( (state == BTSOCCER_STATE_GOAL_KEEPER_POSITION) ||
           ( (state == BTSOCCER_STATE_TUTORIAL) &&
             (tutorial->isGoalKeeperPositionInputEnabled()) ) )
   {
      goalKeeperPositionIO();
   }
}

/********************************************************************
 *                         enterReplayMode                          *
 ********************************************************************/
void Core::enterReplayMode(bool goalReplay)
{
   if(state != BTSOCCER_STATE_REPLAY)
   {
      /* Retrieve camera */
      Goblin::Camera::push();
      /* pause the clock */
      Rules::pause();

      /* Start the replay display */
      replayer->initReplay(goalReplay, onlineGame);
      guiMain->hide();
      stateBeforeReplay = state;
      state = BTSOCCER_STATE_REPLAY;
   }
}

/********************************************************************
 *                          exitReplayMode                          *
 ********************************************************************/
void Core::exitReplayMode()
{
   if(state == BTSOCCER_STATE_REPLAY)
   {
      /* Restore the camera */
      Goblin::Camera::pop(true);
      /* restart the clock */
      Rules::resume();

      state = stateBeforeReplay;
      guiMain->show();
   }
}



/********************************************************************
 *                         prepareToShoot                           *
 ********************************************************************/
void Core::prepareToShoot(bool restric, bool sendMessageIfOnline)
{
   /* tell that will shoot */
   Rules::prepareToShoot();

   /* Select the goal keeper to move */
   GoalKeeper* gk;
   if(Rules::getActiveTeam() == teamA)
   {
      gk = teamB->getGoalKeeper();
   }
   else
   {
      gk = teamA->getGoalKeeper();
   }
   gk->setRestrictMove(restric);
   selectedPlayer = gk;

   /* Change camera to the target goal */
   Ogre::Vector3 playerPos = selectedPlayer->getPosition();
   Goblin::Camera::push();
   Goblin::Camera::set(playerPos.x, 0.0f, playerPos.z, 
         Goblin::Camera::getPhi(), Goblin::Camera::getTheta(), 16.75);
   
   if(playerPos.x > (btsoccerField->getHalfSize().x / 2.0f))
   {
      /* Upper goal */
      Goblin::Camera::setPhi(90);
   }
   else
   {
      /* Down goal */
      Goblin::Camera::setPhi(270);
   }

   if( (onlineGame) && (sendMessageIfOnline) )
   {
      protocol.queueWillShoot();
   }
   else
   {
      /* Only show accept button if not online mode or if received
       * the message to input our goal keeper. */
      guiMain->showAcceptButton();
   }

   /* Enter goalKeeper position mode */
   setState(BTSOCCER_STATE_GOAL_KEEPER_POSITION);
}

/*********************************************************************
 *                               diskIO                              *
 *********************************************************************/
void Core::diskIO()
{
   Team* activeTeam;
   /* Get Team to act */
   activeTeam = Rules::getActiveTeam();

   if(activeTeam->isControlledByHuman())
   {
      /* It's an human player */
      if( (selectedPlayer == NULL) && (!ballIsSelected) )
      {
         if((activeTeam != NULL) && (teamPlayerUnder))
         {
            /* Try to get a player */
            if(leftButtonPressed)
            {
               selectedPlayer = teamPlayerUnder;
               if(selectedPlayer)
               {
                  if(Rules::setDiskAct(selectedPlayer))
                  {
                     force.setInitial(fieldMouse[0], fieldMouse[2]);
                  }
                  else
                  {
                     /* Unselect it */
                     selectedPlayer = NULL;
                     GuiMessage::set("No more moves for disk!");
                  }
               }
            }
         }
         else if((activeTeam != NULL) && (ballIsUnder))
         {
            /* Verify if can act with the ball: if it is inner its own
             * penalty area (where goalkeeper can act). */
            bool isUpperTeam = (activeTeam == Rules::getUpperTeam());
            if( (leftButtonPressed) &&
                (btsoccerField->isInnerPenaltyArea(gameBall->getPosition().x,
                                                   gameBall->getPosition().z,
                                                   isUpperTeam, !isUpperTeam)) )
            {
               /* Select the ball */
               ballIsSelected = true;
               force.setInitial(fieldMouse[0], fieldMouse[2]);
            }
         }
      }
      else
      {
         if(leftButtonPressed)
         {
            /* Is Doing Force Calculation */
            force.setFinal(fieldMouse[0], fieldMouse[2]);
            Ogre::Degree angle;
            float dX=0.0f, dZ=0.0f, fv=0.0f;
            force.getForce(fv, dX, dZ, angle);
            
            if(selectedPlayer != NULL)
            {
               guiMain->setCurrentForce(fv,
                  BTSOCCER_MAX_FORCE_VALUE, dX, dZ, angle,
                  selectedPlayer->getPosition().x,
                  selectedPlayer->getPosition().z,
                  selectedPlayer->getSphereRadius());
            }
            else if(ballIsSelected)
            {
               Rules::setBallAct();
               guiMain->setCurrentForce(fv,
                  BTSOCCER_MAX_FORCE_VALUE, dX, dZ, angle,
                  gameBall->getPosition().x,
                  gameBall->getPosition().z,
                  gameBall->getSphereRadius());
            }
         }
         else
         {
            /* Done with force calculation. Do the Shoot! */
            if(doTheShoot())
            {
               guiMain->hideForceBar();
            }
         }
      }
   }
   else if(!onlineGame)
   {
      assert(activeTeam->getAI() != NULL);
      BaseAI* ai = activeTeam->getAI();

      /* It's an AI controlled team. Must do the input by AI. */
      if(!ai->hasAction())
      {
         /* Select an action */
         if(ai->selectAction())
         {
            ballIsSelected = ai->willActOnBall(); 
            selectedPlayer = ai->getSelectedPlayer();
            if(selectedPlayer != NULL)
            {
               if(Rules::setDiskAct(selectedPlayer))
               {
                  if(!Rules::goalShootDefined())
                  {
                     if(ai->willGoalShoot())
                     {
                        prepareToShoot(false, true);
                     }
                  }
               }
               else
               {
                  cerr << "Warn: AI defined an invalid disk to act!" << endl;
               }
            }
            else
            {
               Rules::setBallAct();
            }
         }
      }
      else
      {
         /* Retrieve again the pointer, as prepareToShoot might NULLed it */
         selectedPlayer = ai->getSelectedPlayer();
         
         if(ai->willGoalShoot())
         {
            /* Calculate the goal shoot after the goalkeeper is defined */
            ai->calculateGoalShoot();
         }
         
         /* Get the force and do the shoot! */
         force.setInitial(ai->getInitialForceX(), ai->getInitialForceZ());
         force.setFinal(ai->getFinalForceX(), ai->getFinalForceZ());
         if(doTheShoot())
         {
            ai->clear();
         }
      }
   }
}

/*********************************************************************
 *                       goalKeeperPositionIO                        *
 *********************************************************************/
void Core::goalKeeperPositionIO()
{
   bool done = false;
   Goblin::Camera::disableTranslations();
   Goblin::Camera::disableZoomChanges();

   if(selectedPlayer)
   {
      GoalKeeper* gk = (GoalKeeper*)selectedPlayer;

      if(gk->getTeam()->isControlledByHuman())
      {
         if(guiMain->getLastEvent() == GuiMain::BUTTON_ACCEPT)
         {
            done = true;
            if(onlineGame)
            {
               protocol.queueGoalKeeperDone();
            }
         }
         else
         {
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS ||\
    OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
            gk->positionInput(fieldMouse, fieldTouch2,
                              btsoccerField);
#else
            gk->positionInput(fieldMouse, leftButtonPressed,
                              btsoccerField);
#endif
            /* Send new gk position */
            if(onlineGame)
            {
               protocol.queueTeamPlayerUpdateToSend(
                     (teamA == gk->getTeam()), true, UPDATE_GK_INDEX,
                     gk->getPosition(), gk->getOrientation(), false);
            }
         }
      }
      else if(!onlineGame)
      {
         gk->getTeam()->getAI()->doGoalKeeperPosition(gk);
         done = true;
      }
   }

   if(done)
   {
      if(onlineGame)
      {
         /* Send gk final position with ack. */
         protocol.queueTeamPlayerUpdateToSend(
               (teamA == selectedPlayer->getTeam()), true, UPDATE_GK_INDEX,
               selectedPlayer->getPosition(), selectedPlayer->getOrientation(),
               true);
      }
      /* Done with goalKeeper input */
      doneWithGoalKepperIO();
   }
}

/*********************************************************************
 *                        doneWithGoalKeeperIO                       *
 *********************************************************************/
void Core::doneWithGoalKepperIO()
{
   /*Enable the camera */
   Goblin::Camera::enableTranslations();
   Goblin::Camera::enableZoomChanges();

   /* Restore state and hide/show things */
   state = previousState;
   selectedPlayer = NULL;
   guiMain->show();
   guiMain->hideAcceptButton();

   /* Must show goals again. */
   btsoccerField->getGoals()->setOpaque();

   /* Take to previous position the camera */
   Goblin::Camera::pop();
}

/*********************************************************************
 *                          diskPositionIO                           *
 *********************************************************************/
void Core::diskPositionIO()
{
   if(selectedPlayer)
   {
      Team* activeTeam;
      /* Get Team to act */
      activeTeam = Rules::getActiveTeam();

      if(activeTeam->isControlledByHuman())
      {
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS ||\
    OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
         if(selectedPlayer->positionInput(fieldMouse, leftButtonPressed,
                  gameBall, btsoccerField))
#else
         if(selectedPlayer->positionInput(fieldMouse, leftButtonPressed,
                 gameBall, btsoccerField))
#endif
         {
            /* Done disk Input */
            state = previousState;

            /* Send disk position, with ack as it's a final position */
            if(onlineGame)
            {
               protocol.queueTeamPlayerUpdateToSend(
                     (teamA == selectedPlayer->getTeam()), true,
                     selectedPlayer->getTeam()->getDiskIndex(selectedPlayer),
                     selectedPlayer->getPosition(),
                     selectedPlayer->getOrientation(), true);
            }

            /* Unselect it */
            selectedPlayer = NULL;
            
            /* Verify if disk input was for penalty shoot */
            if(Rules::getState() == Rules::STATE_PENALTY_KICK)
            {
               /* Penalty shoot: in this case, the "tell we'll shoot
                * is automatic */
               prepareToShoot(true, true);
            }
            
            return;
         } else if(onlineGame)
         {
            protocol.queueTeamPlayerUpdateToSend(
                  (teamA == selectedPlayer->getTeam()), true,
                  selectedPlayer->getTeam()->getDiskIndex(selectedPlayer),
                  selectedPlayer->getPosition(),
                  selectedPlayer->getOrientation(), false);
         }
      }
      else if(!onlineGame)
      {
         /* AI controlled team */
         activeTeam->getAI()->doDiskPosition(selectedPlayer);
         state = BTSOCCER_STATE_NORMAL;
         return;
      }
   }
}

/********************************************************************
 *                             doShoot                              *
 ********************************************************************/
void Core::doShoot(TeamPlayer* player, float initialX, float initialZ,
             float finalX, float finalZ)
{
   selectedPlayer = player;
   force.setInitial(initialX, initialZ);
   force.setFinal(finalX, finalZ);
   doTheShoot();
}

/********************************************************************
 *                           doTheShoot                             *
 ********************************************************************/
bool Core::doTheShoot()
{
   float value=0, dX=0, dZ=0;

   if( (!selectedPlayer) && (!ballIsSelected) )
   {
      return false;
   }

   bool res = false;
   
   /* Clear any rules system state */
   Rules::clearFlags();

   /* Apply the force */
   if(force.getForce(value, dX, dZ))
   {
      if(selectedPlayer != NULL)
      {
         /* Act with disk */
         selectedPlayer->applyForce(value*dX, 0.0f, value*dZ);
         /* init a contact sound */
         Ogre::Vector3 playerPos = selectedPlayer->getPosition();
         Kosound::Sound::addSoundEffect(playerPos.x, 0, playerPos.z,
               SOUND_NO_LOOP, BTSOCCER_SOUND_DISK_SHOOT);
         if(onlineGame)
         {
            protocol.queueSoundEffect(SOUND_TYPE_DISK_ACT, playerPos);
         }
      }
      else
      {
         /* Act with ball */
         value /= BTSOCCER_BALL_FORCE_DIVIDER;
         gameBall->applyForce(value*dX, 0.0f, value*dZ);
         /* And emulate to rules as a team disk collided with it */
         Rules::ballCollideDisk(Rules::getActiveTeam());
      }
      res = true;
      
   } 
   else if( (!onlineGame) || 
            (Rules::getActiveTeam()->isControlledByHuman()) ) 
   {
      /* if current acting team is AI, must accept 0 values for force */
      res = true;
   }

   if(res)
   {
      /* Enable physic simulation */
      initedTurn = false;
      verifyCollisions = true;
#if 0
      force.clear();
#endif
      
      /* update replay data before goes to next position */
      replayer->updateData();
      
      /* Unselect the actor */
      selectedPlayer = NULL;
      ballIsSelected = false;
   }
  
   return res;
}

/*********************************************************************
 *                         getBaseCameraPhi                          *
 *********************************************************************/
float Core::getBaseCameraPhi()
{
   bool isTeamAUpper = (Rules::getUpperTeam() == teamA);
   bool isTeamAActing = (Rules::getActiveTeam() == teamA);
   float phi = 0.0f;
   
   /* Set camera, based on position */
   if( ( (isTeamAUpper) && (isTeamAActing) ) ||
      ( (!isTeamAUpper) && (!isTeamAActing) ) )
   {
      phi = 90;
   }
   else
   {
      phi = -90;
   }
   
   return phi;
}

/*********************************************************************
 *                           defineCamera                            *
 *********************************************************************/
void Core::defineCamera()
{
   bool isTeamAUpper = (Rules::getUpperTeam() == teamA);
   bool isTeamAActing = (Rules::getActiveTeam() == teamA);
   float phi = 0.0f;
   float factor=1.0f;

   /* Only need to set camera if not defined it to be FIXED. */
   if(Options::getCameraType() != Options::CAMERA_FIXED)
   {
      Ogre::Vector3 ballPos = gameBall->getPosition();
      if( (Rules::getState() == Rules::STATE_CORNER_KICK) ||
          (Rules::getState() == Rules::STATE_THROW_IN) )
      {
         /* Set Camera at Corner */
         Goblin::Camera::set(ballPos.x, Goblin::Camera::getCenterY(),ballPos.z, 
               Goblin::Camera::getPhi(), Goblin::Camera::getTheta(), 
               Goblin::Camera::getZoom());
         if(!Goblin::Camera::isTopView())
         {
            if(Rules::getState() == Rules::STATE_CORNER_KICK)
            {
               phi = (ballPos.z >= FIELD_MIDDLE_Z)?30.0f:120.0f;
               factor = (Rules::isBallUpper())?1.0f:-1.0f;
            }
            else
            {
               phi = (ballPos.z > 0)?60.0f:120.0f;
               if( ( (isTeamAUpper) && (isTeamAActing) ) ||
                   ( (!isTeamAUpper) && (!isTeamAActing) ) )
               {
                  factor = 1.0f;
               }
               else
               {
                  factor = -1.0f;
               }
            }
            Goblin::Camera::setPhi(factor*phi);
         }
      }

      /* Always set the camera to a new position if changed the 
       * ball owner and not already set to some state special case
       * (like corners) */
      else if( Rules::changedTeamToAct() )
      {
         /* Set camera, based on position */
         phi = getBaseCameraPhi();

         Goblin::Camera::set(ballPos.x, Goblin::Camera::getCenterY(), ballPos.z,
               Goblin::Camera::getPhi(), Goblin::Camera::getTheta(), 
               Goblin::Camera::getZoom());
         if(!Goblin::Camera::isTopView())
         {
            Goblin::Camera::setPhi(phi);
         }
      }
      /* If the ball isn't at the visible camera culling,
       * put the camera to see it again! */
      else if(!Goblin::Camera::isVisible(gameBall->getBoundingBox()))
      {
         Ogre::Vector3 ballPos = gameBall->getPosition();
         Goblin::Camera::setTarget(ballPos.x, Goblin::Camera::getCenterY(),
               ballPos.z, Goblin::Camera::getPhi(), Goblin::Camera::getTheta(),
               Goblin::Camera::getZoom());
      }
   }
}

/*********************************************************************
 *                        verifyRulesResult                          *
 *********************************************************************/
void Core::verifyRulesResult(bool stateAlreadySet)
{
   if(!stateAlreadySet)
   {
      /* Verify Rules Result  */
      Rules::ballAtFinalPosition(onlineGame);
   }

   switch(Rules::getState())
   {
      case Rules::STATE_MIDDLE:
      {
         /* A goal happened, so call the goal sound effect */
         Kosound::Sound::addSoundEffect(SOUND_NO_LOOP, BTSOCCER_SOUND_GOAL);
         if(!stateAlreadySet)
         {
            /* and reset the teams and put ball at middle */
            Rules::setPositions();
            Rules::clearFlags();
         }
         Ogre::Vector3 ballPos = gameBall->getPosition();
         /* Set the camera to be at field center */
         Goblin::Camera::set(ballPos.x, Goblin::Camera::getCenterY(), 
               ballPos.z, Goblin::Camera::getPhi(), Goblin::Camera::getTheta(),
               Goblin::Camera::getZoom());
         float phi = 0.0f;
         if(Goblin::Camera::getTheta() < 89)
         {
            /* Set angle from the ball to the desired goal */
            phi = getBaseCameraPhi();
            Goblin::Camera::setCurrentPhi(phi);
         }
         /* And the replay */
         enterReplayMode(true);
         if(Goblin::Camera::getTheta() < 89)
         {
            /* Set the camera to face where goal occurred, to it be visible
             * at the replay. */
            Goblin::Camera::setCurrentPhi(phi+180);
         }
         
         /* And the referee horn! */
         Kosound::Sound::addSoundEffect(SOUND_NO_LOOP, BTSOCCER_SOUND_SIFF);
      }
      break;

      case Rules::STATE_GOAL_KICK:
      case Rules::STATE_FREE_KICK:
      case Rules::STATE_PENALTY_KICK:
      {
         if(!stateAlreadySet)
         {
            /* Remove disks from penalty areas */
            coldet.removeFromPenaltyAreas();
            BulletLink::queueUpdatesToProtocol(true);
         }
      }
      case Rules::STATE_CORNER_KICK:
      case Rules::STATE_THROW_IN:
      {
         /* the referee horn! */
         Kosound::Sound::addSoundEffect(SOUND_NO_LOOP, BTSOCCER_SOUND_SIFF);
         
         if(!stateAlreadySet)
         {
            /* Set The Position */
            Rules::setPositions();

            /* Remove all contacts, isolating the ball */
            coldet.removeContacts(true, btsoccerField);
            //FIXME: contact with disk and goal keeper.
            //FIXME: remove disks from area when ball owner changed or a 
            //free-kick happened.
         }

         /* Select disk to do the kick */
         if( (!onlineGame) || 
             (Rules::getActiveTeam()->isControlledByHuman()) )
         {
            /* set state to the disk position adjust */
            setState(BTSOCCER_STATE_DISK_POSITION);
            Ogre::Vector3 ballPos = gameBall->getPosition();
            selectedPlayer = Rules::getActiveTeam()->getNearestPlayer(
                  ballPos.x, ballPos.z);
         }
      }
      break;

      case Rules::STATE_NORMAL:
      default:
      {
         if(!stateAlreadySet)
         {
            /* Remove all contacts */
            coldet.removeContacts(false, btsoccerField);
            /* Change the camera if ball owner changed,
             * or set the camera if the ball isn't visible! */
            Rules::setPositions();
         }
      }
      break;
   }

   /* Define camera positions */
   if(Rules::getState() != Rules::STATE_MIDDLE)
   {
      defineCamera();
   }

   /* Init a new turn */
   initedTurn = true;
   Rules::newTurn();
   replayer->newTurnStarted();

   /* Only need to send to other side, if the rules were verified here. */
   if(!stateAlreadySet)
   {
      /* Send all team and ball positions, to make sure both connection
       * sides are equal. */
      BulletLink::queueUpdatesToProtocol(true);
      /* Send the rules result */
      protocol.queueRulesResult(Rules::getState(), 
            (Rules::getActiveTeam() == teamA));
   }
}

/*********************************************************************
 *                            newMatch                               *
 *********************************************************************/
void Core::newMatch()
{
   singlePlayer = aiForTeamB;
   switch(currentLoadState)
   {
      case 0:
      {
         guiInitial->setLoadingPercentual(0.20f);
         /*! Delete things, if any */
         if(teamA)
         {
            delete teamA;
         }
         if(teamB)
         {
            delete teamB;
         }
      }
      break;
      case 1:
      {
         guiInitial->setLoadingPercentual(0.40f);
         //FIXME: setting AI vs AI for test.
         teamA = new BtSoccer::Team(teamAFileName, ogreSceneManager,
               btsoccerField, bulletDebugDraw, aiForTeamB);
         if(onlineGame)
         {
            if(protocol.isUsingGameCenter())
            {
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
               /* Must be playing with gamecenter, get if teamA. */
               teamA->setControlledByHuman(gameCenterController.isTeamA());
#endif
            }
            else if(client != NULL)
            {
               /* Team A is controlled by the server player */
               teamA->setControlledByHuman(false);
            }
         }
      }
      break;
      case 2:
      {
         guiInitial->setLoadingPercentual(0.60f);
         teamB = new BtSoccer::Team(teamBFileName, ogreSceneManager,
               btsoccerField, teamA->getColorA(), bulletDebugDraw, aiForTeamB);
         if(onlineGame)
         {
            /* Side if teamB is local human, or other side */
            teamB->setControlledByHuman(!protocol.isTeamA());
         }
      }
      break;
      case 3:
      {
         guiInitial->setLoadingPercentual(0.80f);
      }
      break;
      case 4:
      {
         guiInitial->setLoadingPercentual(1.0f);

         /* Set start positions and pointers */
         teamA->startPositionAtField(true, true, btsoccerField);
         teamB->startPositionAtField(false, false, btsoccerField);
         setPointers();
   
         //XXX: Workaround to fix any strange first physics state.
         for(int j=0; j<200;j++)
         {
            BulletLink::forcedStep();
         }

      }
      case 5:
      {
         /* Set the camera to middle initial position */
         Goblin::Camera::setTarget(0.0f, 1.5f, -0.25f, -90.0f, 33.0f, 16.75f);
         (onlineGame)?guiMain->setOnlineGame():guiMain->setLocalGame();
         guiMain->show();

         /* Clear memory used by initial gui */
         guiInitial->hide();
         delete guiInitial;
         guiInitial = NULL;
         Ogre::TextureManager::getSingleton().unloadUnreferencedResources();

         /* Begin the half */
         Rules::setMinutesPerHalf(Options::getMinutesPerHalf());
         Rules::startHalf(true);
         Stats::clear();
         GuiScore::showInitialUpperDownTeams();
         
         if(!onlineGame)
         {
            /* Set the state to normal*/
            GuiMessage::set("The Match Begins!");
            state = BTSOCCER_STATE_NORMAL;
         }
         else
         {
            state = BTSOCCER_STATE_WAITING_OTHER_SIDE_INIT_HALF;
            /* TeamA must be the first to send begin half,
             * alwyas. TeamB will set after receiveing it. */
            if(protocol.isTeamA())
            {
               protocol.queueBeginHalf();
            }
         }
      }
      break;
   }
   currentLoadState++;
}

/*********************************************************************
 *                        showInitialScreen                          *
 *********************************************************************/
void Core::showInitialScreen()
{
   state = BTSOCCER_STATE_INITIAL_SCREEN;
   if(!guiInitial)
   {
      /* Recreate the guiInitial, if no more */
      guiInitial = new GuiInitial();
   }
   guiInitial->show();
}

/*********************************************************************
 *                        setSelectedPlayer                          *
 *********************************************************************/
void Core::setSelectedPlayer(TeamPlayer* player)
{
   selectedPlayer = player;
}

/*********************************************************************
 *                           endOnline                               *
 *********************************************************************/
void Core::endOnline()
{
   /* Finish client and server, if any */
   if(client != NULL)
   {
      delete client;
      client = NULL;
   }
   if(server != NULL)
   {
      delete server;
      server = NULL;
   }
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
   if(gameCenterController.isMatchReady())
   {
      gameCenterController.finishMatch();
   }
#endif
   if( (guiMain != NULL) && (guiMain->isActive()) )
   {
      guiMain->hide();
   }
}

/*********************************************************************
 *                           setPointers                             *
 *********************************************************************/
void Core::setPointers()
{
   /* Set pointers */
   if( (teamA) && (teamB) )
   {
      if(onlineGame)
      {
         /* Defne who you are to show. */
         Ogre::String you = " " + Kobold::i18n::translate("(You)");
         if(teamA->isControlledByHuman())
         {
            GuiScore::reset(teamA->getLogo(), teamA->getName() + you,
                           teamB->getLogo(), teamB->getName());
         }
         else
         {
            GuiScore::reset(teamA->getLogo(), teamA->getName(),
                           teamB->getLogo(), teamB->getName() + you);
         }
      }
      else
      {
         /* Just show teams (both are "you", or single play) */
         GuiScore::reset(teamA->getLogo(), teamA->getName(),
                        teamB->getLogo(), teamB->getName());
      }
      Stats::setTeams(teamA->getLogo(), teamB->getLogo());
   }

   /* Set the collision system */
   coldet.setTeamA(teamA);
   coldet.setTeamB(teamB);
   coldet.setBall(gameBall);
   coldet.setField(btsoccerField);
   BulletLink::setPointers(teamA, teamB, gameBall, btsoccerField, onlineGame);

   /* Set the rules */
   Rules::setTeamA(teamA);
   Rules::setTeamB(teamB);
   Rules::setBall(gameBall);
   Rules::setField(btsoccerField);
   replayer->clear();
   replayer->setTeamA(teamA);
   replayer->setTeamB(teamB);
   replayer->setBall(gameBall);

   selectedPlayer = NULL;
   teamPlayerUnder = NULL;
}


