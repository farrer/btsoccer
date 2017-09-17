#ifndef _btsoccer_gamecenter_impl_h
#define _btsoccer_gamecenter_impl_h

#import <GameKit/GameKit.h>
#include "protocol.h"
#include "../engine/core.h"

#ifdef __OBJC__

/*! The Game Kit Implementation: a singleton class to talk with game kit. */
@interface GameKitImpl : NSObject<GKMatchmakerViewControllerDelegate, GKMatchDelegate, GKLocalPlayerListener>

@property (nonatomic, readonly) UIViewController *authenticationViewController;
@property (nonatomic, readonly) NSError *lastError;
@property (nonatomic, readonly) UIViewController *pView;
@property (nonatomic, readonly) BtSoccer::Core *pCore;
@property (nonatomic, readonly) BOOL enableGameCenter;
@property (nonatomic, readonly) BOOL canceledGameCenter;
@property (nonatomic, readonly) BOOL authenticating;
@property (nonatomic, readonly) BOOL needToShow;
@property (atomic, readonly) GKMatch *match;
@property (nonatomic, readonly) BOOL matchStarted;
@property (nonatomic, readonly) BOOL matchCanceled;
@property (nonatomic, readonly) BOOL matchFailed;
@property (nonatomic, readonly) BOOL invitationGame;
@property (nonatomic, readonly) GKPlayer* playerTeamA;

+ (instancetype)singletonGameKitImpl;
- (void)authenticateLocalPlayer:(bool)showViewControllerIfNeeded;
- (void)setUsedView:(UIViewController*) ogreView;
- (void)setCorePtr:(BtSoccer::Core*) core;
- (void)showAuthenticationViewController;
- (void)hostMatch;
- (bool)matchIsReady;
- (bool)matchIsCanceled;
- (bool)matchCreationFailed;
- (bool)isTeamA;
- (void)startSoccerMatch;
- (void)finishSoccerMatch;
- (bool)queueMessage:(BtSoccer::ProtocolMessage*)msg withSize:(unsigned long)size;

@end

#endif

#endif
