#include "gamecenternetwork.h"
#include "gamecenterimpl.h"
#include "protocol.h"
#include "../btsoccer.h"

using namespace BtSoccer;

#define GAME_CENTER_SLEEP_MS BTSOCCER_UPDATE_RATE / 2

/****************************************************************
 *                          Constructor                         *
 ****************************************************************/
GameCenterController::GameCenterController()
{
}

/****************************************************************
 *                           Destructor                         *
 ****************************************************************/
GameCenterController::~GameCenterController()
{
}

/***********************************************************************
 *                      authenticateGameCenter                         *
 ***********************************************************************/
void GameCenterController::authenticate()
{
   /* First verify if the initial authentication didn't show the view */
   if([[GameKitImpl singletonGameKitImpl]needToShow])
   {
      /* Pending to show view, show it. */
      [[GameKitImpl singletonGameKitImpl]showAuthenticationViewController];
   }
   
   /* Avoid double athentication. */
   else if( (!isAuthenticating()) && (!isEnabled()) && (!isCanceled()) )
   {
      [[GameKitImpl singletonGameKitImpl]authenticateLocalPlayer:true];
   }
}

/***********************************************************************
 *                         isAuthenticating                            *
 ***********************************************************************/
bool GameCenterController::isAuthenticating()
{
   return [[GameKitImpl singletonGameKitImpl]authenticating];
}

/***********************************************************************
 *                         enabledGameCenter                           *
 ***********************************************************************/
bool GameCenterController::isEnabled()
{
   return [[GameKitImpl singletonGameKitImpl]enableGameCenter];
}

/***********************************************************************
 *                       gameCenterIsCanceled                          *
 ***********************************************************************/
bool GameCenterController::isCanceled()
{
   return [[GameKitImpl singletonGameKitImpl]canceledGameCenter];
}

/***********************************************************************
 *                                hostMatch                            *
 ***********************************************************************/
void GameCenterController::hostMatch()
{
   [[GameKitImpl singletonGameKitImpl]hostMatch];
}

/***********************************************************************
 *                              matchIsReady                           *
 ***********************************************************************/
bool GameCenterController::isMatchReady()
{
   return [[GameKitImpl singletonGameKitImpl]matchIsReady];
}

/***********************************************************************
 *                            matchIsCanceled                          *
 ***********************************************************************/
bool GameCenterController::isMatchCanceled()
{
   return [[GameKitImpl singletonGameKitImpl]matchIsCanceled];
}

/***********************************************************************
 *                            matchIsDefined                           *
 ***********************************************************************/
bool GameCenterController::isMatchDefined()
{
   return [[GameKitImpl singletonGameKitImpl]matchStarted];
}

/***********************************************************************
 *                           matchCreationFailed                       *
 ***********************************************************************/
bool GameCenterController::matchCreationFailed()
{
   return [[GameKitImpl singletonGameKitImpl]matchCreationFailed];
}

/***********************************************************************
 *                                isTeamA                              *
 ***********************************************************************/
bool GameCenterController::isTeamA()
{
   return [[GameKitImpl singletonGameKitImpl]isTeamA];
}

/***********************************************************************
 *                         gameCenterQueueMessage                      *
 ***********************************************************************/
bool GameCenterController::queueMessage(BtSoccer::ProtocolMessage* msg,
                                        unsigned long size)
{
   return [[GameKitImpl singletonGameKitImpl]queueMessage:msg withSize:size];
}

/***********************************************************************
 *                         gameCenterFinishMatch                       *
 ***********************************************************************/
void GameCenterController::finishMatch()
{
   [[GameKitImpl singletonGameKitImpl]finishSoccerMatch];
   endThread();
}

/***********************************************************************
 *                            getSleepTime                             *
 ***********************************************************************/
unsigned int GameCenterController::getSleepTime()
{
   return GAME_CENTER_SLEEP_MS;
}

/***********************************************************************
 *                                  step                               *
 ***********************************************************************/
bool GameCenterController::step()
{
   ProtocolMessage msg;
   
   //FIXME: error control.
   while(protocol.getNextMessageToSend(&msg))
   {
#ifdef BTSOCCER_NET_DEBUG
      printf("Will send data msg->type: %d\n", msg.type);
#endif
      queueMessage(&msg, sizeof(ProtocolMessage));
   }
   
   return true;
}

