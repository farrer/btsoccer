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


#ifndef _btsoccer_consts_h
#define _btsoccer_consts_h

#include <OGRE/OgreString.h>
#include <OGRE/OgreRenderWindow.h>

#include "btsoccerconfig.h"
#include "soundfiles.h"

namespace BtSoccer
{

#define BTSOCCER_VERSION_MAJOR  1
#define BTSOCCER_VERSION_MINOR  0
   
/*! If will enable the use of AI and single player games */
#define BTSOCCER_HAS_AI

/*! Constant to multiply to convert from Bullet world to Ogre world */
#define BULLET_TO_OGRE_FACTOR 1.0f
/*! Constant to multiply to convert from Ogre world to Bullet world */
#define OGRE_TO_BULLET_FACTOR (1.0f / BULLET_TO_OGRE_FACTOR)

/*! Internal bullet frequency used */
#define BULLET_FREQUENCY (1.0f / 320.0f)

/***********************************************************************
 *                       Screen update related                         *
 ***********************************************************************/
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
   #define BTSOCCER_NORMAL_FPS          30
#else
   #define BTSOCCER_NORMAL_FPS          30
#endif
#define BTSOCCER_UPDATE_RATE            (1000.0f / BTSOCCER_NORMAL_FPS)
   
/***********************************************************************
 *                        Size and Position related                    *
 ***********************************************************************/

/*! Scale factor to use for current 3D models */
#define TEAM_PLAYER_SCALE_FACTOR   0.25f
/*! Scale factor for ball 3D model */
#define BALL_SCALE_FACTOR         0.11f
/*! Scale factor for Goal 3D model */
#define GOAL_SCALE_FACTOR         0.25f
   
/***********************************************************************
 *                          Physic constants                           *
 ***********************************************************************/

#define FIELD_FRICTION               0.5f
#define FIELD_RESTITUTION            0.6f
#define FIELD_ROLLING_FRICTION       0.0f
#define FIELD_SPINNING_FRICTION      0.0f
   
/*! Current bullet physics normalization.
 * \note If we use too low values for masses (ie: real world ones),
 * the bullet physics resolver won't act as expected, thus we ranged 
 * the mass/forces system by this multiplier factor. */
#define BTSOCCER_MASS_FORCE_MULTIPLIER  1000
   
#define DISK_MASS             (0.002f * BTSOCCER_MASS_FORCE_MULTIPLIER)
#define DISK_RESTITUTION      0.8f
#define DISK_FRICTION         0.7f
#define DISK_ROLLING_FRICTION 0.0f
#define DISK_ANGULAR_DAMPING  1.0f
   
#define GKEEPER_MASS          (0.01f * BTSOCCER_MASS_FORCE_MULTIPLIER)

#define BALL_MASS             (0.0005f * BTSOCCER_MASS_FORCE_MULTIPLIER)
#define BALL_RESTITUTION          0.8f
#define BALL_FRICTION             0.8f
/*! Rolling frictions is a mess in bullet.
 * (when landing, it takes another direction - versions 2.82 and 2.83). 
 * (no rotation on 2.85+). Using a Linear damping will affect everything 
 * not only rolling, and the result is awful. So, we are just not using rolling
 * at all, but 'pretending we are', by just setting it visually to the ball
 * when it's moving on the ground. */
#define BALL_ROLLING_FRICTION     0.0f
/*! The Angular damping for ball */
#define BALL_ANGULAR_DAMPING 0.992f
 
/*! A factor to multiply the force calculation for better ranged values. */
#define BTSOCCER_FORCE_MULTIPLIER  (BTSOCCER_MASS_FORCE_MULTIPLIER * OGRE_TO_BULLET_FACTOR / 24.0f)
#define BTSOCCER_MAX_FORCE_VALUE   (75 * BTSOCCER_FORCE_MULTIPLIER) 
#define BTSOCCER_MIN_FORCE_VALUE   (5 * BTSOCCER_FORCE_MULTIPLIER)
/*! How much a force done directed to the ball should be lesser than
 * a force done to the disk. */
#define BTSOCCER_BALL_FORCE_DIVIDER    12.5f

/***********************************************************************
 *                        Collision constants                          *
 ***********************************************************************/
#define BTSOCCER_VALUE_NOT_DEFINED       -1000
   
/*! Number of steps before validate a new ball collision with some element */
#define BALL_MIN_STEPS_BEFORE_NEXT_COLLISION  5

/*! Time without collision to mark detected ones as new */
#define BTSOCCER_MIN_NEW_COLLISION_TIME   50 

/*! Minimun distance to keep between disks, to avoid some undesired "stops"
 * collisions. */
#define BTSOCCER_COLLISION_THRESHOLD_DELTA 0.05f

/***********************************************************************
 *                         Class foward declarations                   *
 ***********************************************************************/
class Ball;
class Collision;
class Field;
class FieldObject;
class ForceInput;
class GoalKeeper;
class Goal;
class Goals;
class Rules;
class TeamPlayer;
class Team;
class Region;
class Regions;
class TeamInfo;

class Cup;
class Core;

class BaseAI;
class DummyAI;  
class FuzzyAI;
class DecourtAI;

class GuiMessage;
class GuiScore;
class GuiPause;
class GuiInitial;
class GuiSaves;

class ReplayData;
class Replay;

class Stats;

class Tutorial;

class BulletLink;
class OgreMotionState;

}

#endif

