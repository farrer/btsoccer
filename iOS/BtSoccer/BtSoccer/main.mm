
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


//|||||||||||||||||||||||||||||||||||||||||||||||

#include "AppDelegate.h"

int main(int argc, char **argv)
{
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	int retVal = UIApplicationMain(argc, argv, @"UIApplication", @"AppDelegate");
	[pool release];
	return retVal;
#elif (OGRE_PLATFORM == OGRE_PLATFORM_APPLE) && __LP64__
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
   
   mAppDelegate = [[AppDelegate alloc] init];
   [[NSApplication sharedApplication] setDelegate:mAppDelegate];
	int retVal = NSApplicationMain(argc, (const char **) argv);
   
	[pool release];
   
	return retVal;
   
#endif
}

//|||||||||||||||||||||||||||||||||||||||||||||||
