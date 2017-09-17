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

#include "savefile.h"

#include "rules.h"
#include "team.h"
#include "teamplayer.h"
#include "goalkeeper.h"
#include "ball.h"
#include "core.h"

#include <goblin/camera.h>

#include "../gui/guiscore.h"
#include "../gui/guimessage.h"

#include "../ai/baseai.h"
#include "../ai/dummyai.h"

#include <iostream>
using namespace std;

using namespace BtSoccer;

#define SAVE_TOKEN_CUP_FILE      "cupFile"
#define SAVE_TOKEN_CORE_STATE    "coreState"
#define SAVE_TOKEN_NUM_HUMANS    "numHumans"

#define SAVE_TOKEN_FIELD_FILE       "fieldFile"
#define SAVE_TOKEN_TEAM_FILE        "teamFile"
#define SAVE_TOKEN_TEAM_ACTIVE      "teamActive"
#define SAVE_TOKEN_TEAM_UPPER       "teamUpper"
#define SAVE_TOKEN_TEAM_POS         "teamPos"
#define SAVE_TOKEN_TEAM_ORIENTATION "teamOrientation"
#define SAVE_TOKEN_CURRENT_DISK     "teamDiskCurrent"
#define SAVE_TOKEN_DISK_TOUCHES     "teamDiskTouches"
#define SAVE_TOKEN_GK_ANGLE         "teamGkAngle"
#define SAVE_TOKEN_TEAM_SCORE       "teamScore"

#define SAVE_TOKEN_BALL_POS      "ballPos"
#define SAVE_TOKEN_BALL_ANGLE    "ballAngle"

/* From Camera */
#define SAVE_TOKEN_CAMERA        "camera"

/* From Rules */
#define SAVE_TOKEN_RULE_STATE       "ruleState"
#define SAVE_TOKEN_GAME_TYPE        "gameType"
#define SAVE_TOKEN_MINUTES_PER_HALF "minutesPerHalf"
#define SAVE_TOKEN_SECOND_HALF      "secondHalf"
#define SAVE_TOKEN_CUR_HALF_TIME    "curHalfTime"
#define SAVE_TOKEN_WILL_SHOOT       "willShoot"
#define SAVE_TOKEN_GLOBAL_TOUCHES   "globalTouches"

/* From Statistics */
#define SAVE_TOKEN_STATS_FOULS         "statsFouls"
#define SAVE_TOKEN_STATS_GOAL_SHOOTS   "statsGoalShoots"
#define SAVE_TOKEN_STATS_CORNERS       "statsCorners"
#define SAVE_TOKEN_STATS_THROWS        "statsThrows"
#define SAVE_TOKEN_STATS_GOAL_KICKS    "statsGoalKicks"
#define SAVE_TOKEN_STATS_PENALTIES     "statsPenalties"
#define SAVE_TOKEN_STATS_TOTAL_MOVES   "statsTotalMoves"

/***********************************************************************
 *                           Constructor                               *
 ***********************************************************************/
SaveFile::SaveFile()
{
   numHumans = 0;
}

/***********************************************************************
 *                            Destructor                               *
 ***********************************************************************/
SaveFile::~SaveFile()
{
}

/***********************************************************************
 *                                save                                 *
 ***********************************************************************/
bool SaveFile::save(Ogre::String fileName, 
      BtSoccer::Team* teamA, BtSoccer::Team* teamB,
      int coreState)
{
   int i,t;
   Team* team[2];
   Ball* ball;
   Field* field;
   ofstream file;
   TeamPlayer* disk;
   GoalKeeper* gk;
   int score[2];
   Ogre::Vector3 pos;

   /* Get some pointers */
   team[0] = teamA;
   team[1] = teamB;
   ball = Rules::getBall();
   field = Rules::getField();
   GuiScore::getTeamScore(score[0], score[1]);

   /* Open the file */
   file.open(fileName.c_str(), ios::out | ios::binary);
   if(!file)
   {
      return(false);
   }

   /* Calculate the number of humans */
   numHumans=0;
   for(i=0; i < 2; i++)
   {
      if(team[i]->isControlledByHuman())
      {
         numHumans++;
      }
   }
   file << SAVE_TOKEN_NUM_HUMANS << " = " << numHumans << endl; 

   /* TODO: set cup file and save it! */
   //file << SAVE_TOKEN_CUP_FILE << " = " << end;

   /* Save global things from rules */
   file << SAVE_TOKEN_CORE_STATE << " = " << coreState << endl;
   file << SAVE_TOKEN_RULE_STATE << " = " << Rules::getState() << endl;
   file << SAVE_TOKEN_GAME_TYPE << " = " << Rules::getGameType() << endl;
   file << SAVE_TOKEN_MINUTES_PER_HALF << " = " << 
      Rules::getMinutesPerHalf() << endl;
   file << SAVE_TOKEN_SECOND_HALF << " = " << 
      ((!Rules::isFirstHalf())?"true":"false") << endl;
   file << SAVE_TOKEN_CUR_HALF_TIME << " = " 
      << Rules::getCurrentHalfTime() << endl;
   file << SAVE_TOKEN_WILL_SHOOT << " = " << 
      ((Rules::goalShootDefined())?"true":"false") << endl;
   file << SAVE_TOKEN_GLOBAL_TOUCHES << " = " 
      << Rules::getRemainingTouches() << endl;
   file << SAVE_TOKEN_FIELD_FILE << " = " << field->getFileName() << endl;

   file << SAVE_TOKEN_CAMERA << " = " << Goblin::Camera::getCenterX() << " "
      << Goblin::Camera::getCenterY() 
      << " " << Goblin::Camera::getCenterZ() << " "
      << Goblin::Camera::getPhi() << " " << Goblin::Camera::getTheta() << " "
      << Goblin::Camera::getZoom() << endl;

   /* Save both teams */
   for(t=0; t < 2; t++)
   {
      /* Save team global info */
      file << SAVE_TOKEN_TEAM_FILE << " = " 
         << team[t]->getFileName() << endl;
      file << SAVE_TOKEN_TEAM_ACTIVE << " = " 
         << ((team[t] == Rules::getActiveTeam())?"true":"false") << endl;
      file << SAVE_TOKEN_TEAM_UPPER << " = "
         << ((team[t] == Rules::getUpperTeam())?"true":"false") << endl;
      file << SAVE_TOKEN_TEAM_SCORE << " = " << score[t] << endl; 

      /* Save each disk */
      for(i=0; i < field->getNumberOfDisks(); i++)
      {
         disk = team[t]->getDisk(i);
         pos = disk->getPosition();
         file << SAVE_TOKEN_TEAM_POS << " = " << pos.x << " "
            << pos.z << endl;
         file << SAVE_TOKEN_TEAM_ORIENTATION << " = " 
            << disk->getOrientationY() << endl;
         if(disk == Rules::getCurrentDisk())
         {
            file << SAVE_TOKEN_CURRENT_DISK << " = true " << endl;
            file << SAVE_TOKEN_DISK_TOUCHES << " = " 
               << Rules::getRemainingTouches(disk) << endl;
         }
      }
      
      /* Save goal keeper */
      gk = team[t]->getGoalKeeper();
      pos = gk->getPosition();
      file << SAVE_TOKEN_TEAM_POS << " = " << pos.x << " "
         << pos.z << endl;
      file << SAVE_TOKEN_GK_ANGLE << " = " << gk->getOrientationY() << endl;
   }

   /* Save Ball */
   pos = ball->getPosition();
   Ogre::Quaternion qOri = ball->getOrientation();
   file << SAVE_TOKEN_BALL_POS << " = " << pos.x << " " 
      << pos.y << " " << pos.z << endl;
   file << SAVE_TOKEN_BALL_ANGLE << " = " << qOri.x << " " 
      << qOri.y << " " << qOri.z << " " << qOri.w << endl;

   /* Save Statistics */
   file << SAVE_TOKEN_STATS_FOULS << " = " << Stats::getFouls(true) 
      << " " << Stats::getFouls(false) << endl; 
   file << SAVE_TOKEN_STATS_GOAL_SHOOTS  << " = " << Stats::getGoalShoots(true) 
      << " " << Stats::getGoalShoots(false) << endl; 
   file << SAVE_TOKEN_STATS_CORNERS << " = " << Stats::getCorners(true) 
      << " " << Stats::getCorners(false) << endl; 
   file << SAVE_TOKEN_STATS_THROWS << " = " << Stats::getThrows(true) 
      << " " << Stats::getThrows(false) << endl; 
   file << SAVE_TOKEN_STATS_GOAL_KICKS << " = " << Stats::getGoalKicks(true) 
      << " " << Stats::getGoalKicks(false) << endl; 
   file << SAVE_TOKEN_STATS_PENALTIES << " = " << Stats::getPenalties(true) 
      << " " << Stats::getPenalties(false) << endl; 
   file << SAVE_TOKEN_STATS_TOTAL_MOVES << " = " << Stats::getTotalMoves(true) 
      << " " << Stats::getTotalMoves(false) << endl; 

   /* Close it */
   file.close();

   GuiMessage::set("Game Saved!");

   return(true);
}

/***********************************************************************
 *                                load                                 *
 ***********************************************************************/
bool SaveFile::load(Ogre::String fileName, BtSoccer::Core* core, 
      BtSoccer::Team** teamA, BtSoccer::Team** teamB,
      BtSoccer::Field* field, BulletDebugDraw* debugDraw)
{
   int iaux=0;
   float faux=0.0f;
   Kobold::DefParser def;
   Ogre::String key, value;
   Ball* ball;
   int curTeam=-1;
   int curDisk=-1;
   Team** teams[2];
   int score[2];

   /* Set pointers */
   teams[0] = teamA;
   teams[1] = teamB;
   ball = core->getBall();
 
   /* Try to load the definitions */
   if(!def.load(fileName, true))
   {
      GuiMessage::set("Couldn't Load!");
      return(false);
   }

   /* Clear current rule values */
   Rules::clear();

   /* Parse each key/value */
   while(def.getNextTuple(key, value))
   {
      if(key == SAVE_TOKEN_NUM_HUMANS)
      {
         sscanf(value.c_str(), "%d", &numHumans);
      }
      else if(key == SAVE_TOKEN_FIELD_FILE)
      {
         /* Load the field used */
         field->createField(value, core->getSceneManager());
      }
      else if(key == SAVE_TOKEN_CUP_FILE)
      {
         /* TODO: load cup file and set it! */
      }
      else if(key == SAVE_TOKEN_CORE_STATE)
      {
         /* set core state */
         sscanf(value.c_str(), "%d", &iaux);   
         core->setState(iaux);
      }
      else if(key == SAVE_TOKEN_RULE_STATE)
      {
         /* set rules state */
         sscanf(value.c_str(), "%d", &iaux);   
         Rules::setState(iaux);
      }
      else if(key == SAVE_TOKEN_GAME_TYPE)
      {
         /* set rules game type */
         sscanf(value.c_str(), "%d", &iaux);   
         Rules::setGameType(iaux);
      }
      else if(key == SAVE_TOKEN_MINUTES_PER_HALF)
      {
         /* set rules game type */
         sscanf(value.c_str(), "%d", &iaux);   
         Rules::setMinutesPerHalf(iaux);
      }
      else if(key == SAVE_TOKEN_SECOND_HALF)
      {
         /* Set game current half */
         Rules::setHalf((value != "true"));
      }
      else if(key == SAVE_TOKEN_CUR_HALF_TIME)
      {
         /* set elapsed time of current half */
         unsigned long ul=0;
         sscanf(value.c_str(), "%lu", &ul);
         Rules::setCurrentHalfTime(ul);
      }
      else if(key == SAVE_TOKEN_WILL_SHOOT)
      {
         /* set if will shoot */
         if(value == "true")
         {
            Rules::prepareToShoot();
         }
      }
      else if(key == SAVE_TOKEN_GLOBAL_TOUCHES)
      {
         /* Set remaining global touches for active team */
         sscanf(value.c_str(), "%d", &iaux);   
         Rules::setGlobalRemainingTouches(iaux);
      }
      else if(key == SAVE_TOKEN_CAMERA)
      {
         /* Set camera elements */
         float x=0,y=0,z=0,p=0,t=0,zo=0;
         sscanf(value.c_str(), "%f %f %f %f %f %f",
            &x, &y, &z, &p, &t, &zo);
         Goblin::Camera::setTarget(x, y, z, p, t, zo);
      }
      else if(key == SAVE_TOKEN_TEAM_FILE)
      {
         /* Create a new team */
         curTeam++;
         curDisk=-1;
         if(*teams[curTeam] != NULL)
         {
            delete(*teams[curTeam]);
         }
         if(curTeam == 0)
         {
            /* First team */
            *teams[curTeam] = new BtSoccer::Team(value, core->getSceneManager(),
                  field, debugDraw);
         }
         else
         {
            /* Second team must be aware of first team color. */
            *teams[curTeam] = new BtSoccer::Team(value, core->getSceneManager(),
                  field, (*teams[0])->getColorA(), debugDraw,
                  ((curTeam == 1) && (numHumans < 2)));
         }
         (*teams[curTeam])->startPositionAtField(false, false, field);
      }
      else if(key == SAVE_TOKEN_TEAM_ACTIVE)
      {
         if(value == "true")
         {
            /* Set the current team as the active one */
            Rules::setActiveTeam(*teams[curTeam]);
         }
      }
      else if(key == SAVE_TOKEN_TEAM_UPPER)
      {
         if(value == "true")
         {
            /* Set current team as upper */
            Rules::setUpperTeam(*teams[curTeam]);
         }
      }
      else if(key == SAVE_TOKEN_TEAM_SCORE)
      {
         sscanf(value.c_str(), "%d", &iaux);
         score[curTeam] = iaux;
      }
      else if(key == SAVE_TOKEN_TEAM_POS)
      {
         TeamPlayer* tp;
         float x=0,z=0;
         
         curDisk++;
         sscanf(value.c_str(), "%f %f", &x, &z);

         if(curDisk == field->getNumberOfDisks())
         {
            /* Set goal keeper */
            tp = (*teams[curTeam])->getGoalKeeper();
         }
         else
         {
            /* Set a disk */
            tp = (*teams[curTeam])->getDisk(curDisk);
         }

         tp->setPositionWithoutForcedPhysicsStep(Ogre::Vector3(x, 0, z));
      }
      else if(key == SAVE_TOKEN_TEAM_ORIENTATION)
      {
         sscanf(value.c_str(), "%f", &faux);
         TeamPlayer* tp = (*teams[curTeam])->getDisk(curDisk);
         tp->setOrientation(Ogre::Degree(faux));
      }
      else if(key == SAVE_TOKEN_CURRENT_DISK)
      {
         if(value == "true")
         {
            Rules::setCurrentDisk((*teams[curTeam])->getDisk(curDisk));
         }
      }
      else if(key == SAVE_TOKEN_DISK_TOUCHES)
      {
         sscanf(value.c_str(), "%d", &iaux);
         Rules::setDiskRemainingTouches(iaux);
      }
      else if(key == SAVE_TOKEN_GK_ANGLE)
      {
         sscanf(value.c_str(), "%f", &faux);
         GoalKeeper* gk = (*teams[curTeam])->getGoalKeeper();
         gk->setOrientation(Ogre::Degree(faux));
      }
      else if(key == SAVE_TOKEN_BALL_POS)
      {
         float x=0,y=0,z=0;
         sscanf(value.c_str(), "%f %f %f", &x, &y, &z);
         ball->setPositionWithoutForcedPhysicsStep(Ogre::Vector3(x, y, z));
      }
      else if(key == SAVE_TOKEN_BALL_ANGLE)
      {
         float x=0,y=0,z=0,w=0;
         Ogre::Quaternion qOri;
         sscanf(value.c_str(), "%f %f %f %f", &x, &y, &z, &w);
         qOri.x = x;
         qOri.y = y;
         qOri.z = z;
         qOri.w = w;
         ball->setOrientation(qOri);
      }
   }

   /* Set all pointers on core engine */
   core->setPointers();
   /* set number of goals */
   GuiScore::setGoalsTeamA(score[0]);
   GuiScore::setGoalsTeamB(score[1]);
   
   BulletLink::forcedStep();

   /* Ended with load! */
   GuiMessage::set("Game Loaded!");


   return(true);
}


