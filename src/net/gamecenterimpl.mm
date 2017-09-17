#include "gamecenterimpl.h"

#ifdef __OBJC__

@implementation GameKitImpl

/***********************************************************************
 *                       singletonGameKitImpl                          *
 ***********************************************************************/
+ (instancetype)singletonGameKitImpl
{
   static GameKitImpl *singletonGameKitImpl;
   static dispatch_once_t onceToken;
   dispatch_once(&onceToken, ^{
      singletonGameKitImpl = [[GameKitImpl alloc] init];
   });
   return singletonGameKitImpl;
}

/***********************************************************************
 *                            setUsedView                              *
 ***********************************************************************/
- (void)setUsedView:(UIViewController*) ogreView
{
   _pView = ogreView;
}

/***********************************************************************
 *                            setCorePtr                               *
 ***********************************************************************/
- (void)setCorePtr:(BtSoccer::Core*) core
{
   _pCore = core;
}

/***********************************************************************
 *                      authenticateLocalPlayer                        *
 ***********************************************************************/
- (void)authenticateLocalPlayer:(bool)showViewControllerIfNeeded
{
   _authenticating = YES;
   _needToShow = NO;
   _canceledGameCenter = NO;
   GKLocalPlayer *localPlayer = [GKLocalPlayer localPlayer];
   
   localPlayer.authenticateHandler  =
   ^(UIViewController *viewController, NSError *error)
   {
      [self setLastError:error];
      
      if(viewController != nil)
      {
         /* Defined the view controller, must show it. */
         _needToShow = YES;
         [self setAuthenticationViewController:viewController];
         if(showViewControllerIfNeeded)
         {
            [self showAuthenticationViewController];
         }
      }
      else if([GKLocalPlayer localPlayer].isAuthenticated)
      {
         /* User is authenticated */
         _enableGameCenter = YES;
         _authenticating = NO;
         /* Set the self to listen for invite events. */
         [[GKLocalPlayer localPlayer] registerListener:self];
      }
      else
      {
         /* User canceled authentication and game center use. */
         _enableGameCenter = NO;
         _authenticating = NO;
         _canceledGameCenter = YES;
      }
   };
}

/***********************************************************************
 *                  setAuthenticationViewController                    *
 ***********************************************************************/
- (void)setAuthenticationViewController:(UIViewController *)authenticationViewController
{
   if (authenticationViewController != nil) {
      _authenticationViewController = authenticationViewController;
   }
}

/***********************************************************************
 *                 showAuthenticationViewController                    *
 ***********************************************************************/
-(void)showAuthenticationViewController
{
   if(_authenticationViewController != nil)
   {
      _needToShow = NO;
      [_pView presentViewController:
       _authenticationViewController
                           animated:YES
                         completion:nil];
   }
}

/***********************************************************************
 *                           setLastError                              *
 ***********************************************************************/
- (void)setLastError:(NSError *)error
{
   _lastError = [error copy];
   if (_lastError)
   {
      NSLog(@"GameCenterImpl ERROR: %@",
            [[_lastError userInfo] description]);
   }
}

/***********************************************************************
 *                              hostMatch                              *
 ***********************************************************************/
- (void)hostMatch
{
   /* BtSoccer uses only 2 players match. */
   GKMatchRequest *request = [[GKMatchRequest alloc] init];
   request.minPlayers = 2;
   request.maxPlayers = 2;
   
   _playerTeamA = nil;
   _matchCanceled = NO;
   _matchFailed = NO;
   _invitationGame = NO;
   
   /* Create and show the match creator view */
   GKMatchmakerViewController *mmvc = [[GKMatchmakerViewController alloc]
                                       initWithMatchRequest:request];
   mmvc.matchmakerDelegate = self;
   
   [_pView presentViewController:mmvc animated:YES completion:nil];
}

/***********************************************************************
 *                          didAcceptInvite                            *
 ***********************************************************************/
- (void)player:(GKPlayer *)player didAcceptInvite:(GKInvite *)invite
{
   /* Must tell core that we accepted an invitation */
   _pCore->sendWillStartGameByInvitation();
   /* Open the dialog. */
   GKMatchmakerViewController *mmvc = [[GKMatchmakerViewController alloc] initWithInvite:invite];
   mmvc.matchmakerDelegate = self;
   /* On invitation, player who invited is always teamA */
   _playerTeamA = invite.sender;
   _matchCanceled = NO;
   _matchFailed = NO;
   _invitationGame = YES;
   [_pView presentViewController:mmvc animated:YES completion:nil];
}

/***********************************************************************
 *                     didRequestMatchWithRecipients                   *
 ***********************************************************************/
- (void)player:(GKPlayer *)player didRequestMatchWithRecipients:(NSArray *)recipientPlayers
{
   //[_pView dismissViewControllerAnimated:NO completion:nil];
   
   /* Must tell core that we started the game with game center. */
   _pCore->sendWillStartGameByInvitation();

   /* Create a match for the chosen players */
   GKMatchRequest *request = [[GKMatchRequest alloc]init];
   request.recipients = recipientPlayers;
   request.minPlayers = 2;
   request.maxPlayers = 2;
   
   /* On invitation, player who invited is always teamA */
   _playerTeamA = [GKLocalPlayer localPlayer];
   _matchCanceled = NO;
   _matchFailed = NO;
   _invitationGame = YES;
   
   /* Create a matchmaking viewcontroller for that match */
   GKMatchmakerViewController *mmvc = [[GKMatchmakerViewController alloc]initWithMatchRequest:request];
   mmvc.matchmakerDelegate = self;
   //[_pView presentViewController:mmvc animated:YES completion:nil];
   [_pView dismissViewControllerAnimated:YES completion:^{
      [_pView presentViewController:mmvc animated:YES completion:nil];
   }];
}

/***********************************************************************
 *                matchmakerViewControllerWasCancelled                 *
 ***********************************************************************/
- (void)matchmakerViewControllerWasCancelled:(GKMatchmakerViewController *)viewController
{
   [_pView dismissViewControllerAnimated:YES completion:nil];
   _matchCanceled = YES;
   _match = nil;
}

/***********************************************************************
 *                          didFailWithError                           *
 ***********************************************************************/
- (void)matchmakerViewController:(GKMatchmakerViewController *)viewController didFailWithError:(NSError *)error
{
   [_pView dismissViewControllerAnimated:YES completion:nil];
   _matchFailed = YES;
   _match = nil;
}

/***********************************************************************
 *                      didChangeConnectionState                       *
 ***********************************************************************/
- (void)match:(GKMatch *)match
      player:(GKPlayer *)player
      didChangeConnectionState:(GKPlayerConnectionState)state
{
   if(_match != match)
   {
      return;
   }
   
   if(state == GKPlayerStateConnected)
   {
      if( (!_matchStarted) && (_match.expectedPlayerCount == 0) )
      {
         [self startSoccerMatch];
      }
   }
   else if(state == GKPlayerStateDisconnected)
   {
      _matchStarted = NO;
   }
}

/***********************************************************************
 *                            startMatch                               *
 ***********************************************************************/
- (void)startSoccerMatch
{
   _matchStarted = YES;
   
   if(!_invitationGame)
   {
      GKLocalPlayer *localPlayer = [GKLocalPlayer localPlayer];
      GKPlayer* player;
      player = _match.players[0];
      /* Define player of teamA by ID precedence. */
      if( [player.playerID compare:localPlayer.playerID] == NSOrderedAscending)
      {
         _playerTeamA = player;
      }
      else
      {
         _playerTeamA = localPlayer;
      }
   }
   [_pView dismissViewControllerAnimated:YES completion:nil];
}

/***********************************************************************
 *                            didFindMatch                             *
 ***********************************************************************/
- (void)matchmakerViewController:(GKMatchmakerViewController *)viewController didFindMatch:(GKMatch *)match
{
   _match = match;
   _match.delegate = self;
   if( (!_matchStarted) && (_match.expectedPlayerCount == 0) )
   {
      [self startSoccerMatch];
   }
}

/***********************************************************************
 *                            matchIsReady                            *
 ***********************************************************************/
- (bool)matchIsReady
{
   return (_matchStarted && _playerTeamA != nil);
}

/***********************************************************************
 *                          matchIsCanceled                            *
 ***********************************************************************/
- (bool)matchIsCanceled
{
   return _matchCanceled;
}

/***********************************************************************
 *                       matchCreationFailed                           *
 ***********************************************************************/
- (bool)matchCreationFailed
{
   return _matchFailed;
}

/***********************************************************************
 *                              isTeamA                                *
 ***********************************************************************/
- (bool)isTeamA
{
   return ([GKLocalPlayer localPlayer] == _playerTeamA);
}

/***********************************************************************
 *                           queueMessage                              *
 ***********************************************************************/
- (bool)queueMessage:(BtSoccer::ProtocolMessage*)msg withSize:(unsigned long)size
{
   NSError *error = nil;
   NSData *data = [NSData dataWithBytes:msg length:size];
   
   if( (!_matchStarted) || (_match == nil) )
   {
      /* Won't send message if no match is active. */
      return false;
   }

   [_match sendData:data toPlayers: _match.players
      dataMode: GKMatchSendDataUnreliable error:&error];
   if(error != nil)
   {
      NSLog(@"GameCenterImpl ERROR: %@",
            [[error userInfo] description]);
      return false;
   }
   return true;
}

/***********************************************************************
 *                          didReceiveData                             *
 ***********************************************************************/
- (void)match:(GKMatch *)match didReceiveData:(NSData *)data fromPlayer:(NSString *)playerID
{
   BtSoccer::ProtocolMessage *p = (BtSoccer::ProtocolMessage*)[data bytes];
   BtSoccer::Protocol btProt;
   if(!btProt.parseReceivedMessage(p))
   {
      /* Treat goodbye. */
      _matchStarted = NO;
   }
}

/***********************************************************************
 *                           finishMatch                               *
 ***********************************************************************/
-(void)finishSoccerMatch
{
   [_match disconnect];
   _match = nil;
   _matchStarted = NO;
}



@end

#endif
