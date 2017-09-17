#ifndef __AppDelegate_H__
#define __AppDelegate_H__

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

#include "OgrePlatform.h"

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
#error This header is for use with iOS only
#endif

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#include "core.h"
#include "touchview.h"
#include "gamecenterimpl.h"



// Defaulting to 2 means that we run at 30 frames per second. For 60 frames, use a value of 1.
// 30 FPS is usually sufficient and results in lower power consumption.
#define DISPLAYLINK_FRAME_INTERVAL      2

#ifdef __OBJC__

@interface AppDelegate : NSObject <UIApplicationDelegate, UIAccelerometerDelegate>
{
   BtSoccer::Core* gameApp;
   CADisplayLink *mDisplayLink;
   NSDate* mDate;
   NSTimeInterval mLastFrameTime;
}

- (void)go;
- (void)renderOneFrame:(id)sender;

@property (nonatomic) NSTimeInterval mLastFrameTime;

@end

@implementation AppDelegate

@dynamic mLastFrameTime;

- (NSTimeInterval)mLastFrameTime
{
   return mLastFrameTime;
}

/**
 Returns the URL to the application's Documents directory.
 */
- (NSURL *)applicationDocumentsDirectory
{
   return [[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory
                                                  inDomains:NSUserDomainMask] lastObject];
}

- (void)setLastFrameTime:(NSTimeInterval)frameInterval
{
   // Frame interval defines how many display frames must pass between each time the
   // display link fires. The display link will only fire 30 times a second when the
   // frame internal is two on a display that refreshes 60 times a second. The default
   // frame interval setting of one will fire 60 times a second when the display refreshes
   // at 60 times a second. A frame interval setting of less than one results in undefined
   // behavior.
   if (frameInterval >= 1)
   {
      mLastFrameTime = frameInterval;
   }
}

- (void)go
{
   NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
   NSURL * docUrl = [self applicationDocumentsDirectory];
   NSString* docString = [docUrl absoluteString];
   
   CGRect screenBounds = [[UIScreen mainScreen] bounds];
   CGFloat screenScale = [[UIScreen mainScreen] scale];
   CGSize screenSize = CGSizeMake(screenBounds.size.width ,
                                  screenBounds.size.height );
   
   [[GameKitImpl singletonGameKitImpl]authenticateLocalPlayer:false];
   
   /* Create the game */
   gameApp = new BtSoccer::Core();
   gameApp->create([docString UTF8String], screenSize.width, screenSize.height, screenScale);
   
   [pool release];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
   mLastFrameTime = DISPLAYLINK_FRAME_INTERVAL;
   mDisplayLink = nil;
   
   [self go];
   
   /* Retrieve Ogre UiViewController and UiView */
   unsigned long hView = 0;
   gameApp->getWindow()->getCustomAttribute("VIEWCONTROLLER", &hView);
   UIViewController* ogreViewController = (UIViewController*)hView;
   gameApp->getWindow()->getCustomAttribute("VIEW", &hView);
   UIView* ogreView = (UIView*)hView;
   
   [[GameKitImpl singletonGameKitImpl]setUsedView:ogreViewController];
   [[GameKitImpl singletonGameKitImpl]setCorePtr:gameApp];
   
   /* Create a subview only for touch input */
   Ogre::RenderWindow *mWindow = gameApp->getWindow();
   UIWindow *mUIWindow = nil;
   mWindow->getCustomAttribute("WINDOW", &mUIWindow);
   TouchView *myView = [[TouchView alloc] initWithFrame:[mUIWindow bounds]];
   [myView setBackgroundColor:[UIColor clearColor]];
   myView.multipleTouchEnabled = YES;
   [myView setExclusiveTouch:false];
   [ogreView addSubview:myView];
   
   /* Wait to show authentication view controller, if needed. */
   if([[GameKitImpl singletonGameKitImpl]needToShow])
   {
      [[GameKitImpl singletonGameKitImpl]showAuthenticationViewController];
   }
   
   return YES;
}


- (void)applicationWillTerminate:(UIApplication *)application
{
   delete(gameApp);
}

- (void)applicationWillResignActive:(UIApplication *)application
{
   //    Ogre::Root::getSingleton().saveConfig();
   gameApp->sendToBackground();
   
   [mDate release];
   mDate = nil;
   
   [mDisplayLink invalidate];
   mDisplayLink = nil;
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
   gameApp->sendToForeground();
   mDate = [[NSDate alloc] init];
   mLastFrameTime = DISPLAYLINK_FRAME_INTERVAL; // Reset the timer
   
   mDisplayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(renderOneFrame:)];
   [mDisplayLink setFrameInterval:mLastFrameTime];
   [mDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application
{
   /* Some clean up, if possible */
   gameApp->lowMemoryClean();
}

- (void)renderOneFrame:(id)sender
{
   //[sb.mGestureView becomeFirstResponder];
   
   // NSTimeInterval is a simple typedef for double
   NSTimeInterval currentFrameTime = -[mDate timeIntervalSinceNow];
   //NSTimeInterval differenceInSeconds = currentFrameTime - mLastFrameTime;
   mLastFrameTime = currentFrameTime;
   
   dispatch_async(dispatch_get_main_queue(), ^(void)
                  {
                     gameApp->run();
                  });
}


@end

#endif

#endif
