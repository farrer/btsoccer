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

#include "team.h"

#include "field.h"
#include "teamplayer.h"
#include "goalkeeper.h"

#include "../gui/guiscore.h"
#include "../ai/baseai.h"
#include "../ai/decourtai.h"

#include <OGRE/OgreLogManager.h>
#include <kobold/i18n.h>
#include <algorithm>

using namespace BtSoccer;

/*************************************************************
 *                       Constructor                         *
 *************************************************************/
Team::Team(Ogre::String fileName, Ogre::SceneManager* ogreSceneManager,
           Field* f, BulletDebugDraw* debugDraw, bool createAI)
{
   this->debugDraw = debugDraw;
   load(fileName, ogreSceneManager, f, "");
   if(createAI)
   {
      /* FIXME: Must be somewhere which AI to create */
      ai = new DecourtAI(this, f);
   }
   else
   {
      ai = NULL;
   }
}

/*************************************************************
 *                       Constructor                         *
 *************************************************************/
Team::Team(Ogre::String fileName, Ogre::SceneManager* ogreSceneManager,
           Field* f, Ogre::String oponentPredominantColor, 
           BulletDebugDraw* debugDraw, bool createAI)
{
   this->debugDraw = debugDraw;
   load(fileName, ogreSceneManager, f, oponentPredominantColor);
   if(createAI)
   {
      /* FIXME: Must be somewhere which AI to create */
      ai = new DecourtAI(this, f);
   }
   else
   {
      ai = NULL;
   }
}

/*************************************************************
 *                       Constructor                         *
 *************************************************************/
Team::Team(Ogre::String teamName, bool createAI)
{
   Ogre::StringStream ss;

   this->controlledByHuman = true;
   this->fileName = "Non graphical";
   this->scManager = NULL;
   this->name = teamName;
   //FIXME
   ai = NULL;

   /* Create team elements as non-graphical ones. */
   gKeeper = new GoalKeeper(teamName + "gk");
   for(int i = 0; i < TEAM_MAX_DISKS; i++)
   {
      ss.str("");
      ss << i;
      disk[i] = new TeamPlayer(FieldObject::TYPE_DISK, teamName + ss.str());
   }
   lastActiveDisk = NULL;
}

/*************************************************************
 *                          loadTeam                         *
 *************************************************************/
void Team::load(Ogre::String fileName, Ogre::SceneManager* ogreSceneManager,
           Field* f, Ogre::String oponentPredominantColor)
{
   int i;
   Kobold::DefParser def;
   Ogre::String key, value;
   Ogre::StringStream ss;

   Ogre::String diskFile = DEFAULT_DISK_MODEL;
   Ogre::String diskMaterial = "";
   Ogre::String gKeeperFile = DEFAULT_GOAL_KEEPER_MODEL;
   Ogre::String gKeeperMaterial = "";

   controlledByHuman = true;

   /* Nullify things */
   gKeeper = NULL;
   for(i=0; i<TEAM_MAX_DISKS; i++)
   {
      disk[i] = NULL;
   }
   lastActiveDisk = NULL;

   /* Let's load team definition */
   if(!def.load(fileName))
   {
      Ogre::LogManager::getSingleton().stream(Ogre::LML_CRITICAL)
          << "Couldn't load team: '" << fileName << "'";
      return;
   }

   this->fileName = fileName;

   /* Get all tuples */
   while(def.getNextTuple(key, value))
   {
      if(key == "name")
      {
         /* Team Name */
         name = Kobold::i18n::translate(value);
      }
      else if(key == "symbol")
      {
         /* Team Symbol */
         logo = value;
      }
      else if(key == "diskModel")
      {
         /* Team Disk Model */
         diskFile = value;
      }
      else if(key == "diskMaterial")
      {
         /* Team Disk Material */
         diskMaterial = value;
      }
      else if(key == "goalKeeperModel")
      {
         /* Team goalKeeper Model */
         gKeeperFile = value;
      }
      else if(key == "goalKeeperMaterial")
      {
         /* Team goalKeeper Material */
         gKeeperMaterial = value;
      }
      else if(key == "colorA")
      {
         colorA = value;
      }
      else if(key == "colorB")
      {
         colorB = value;
      }
      else if(key == "numberPosition")
      {
         /* Position of the disks numbers (FIXME) */
      }
   }
   
   Ogre::String gKeeperName = name+Ogre::String("_gkeeper");
   Ogre::String diskBaseName = name+Ogre::String("_disk");
   if(oponentPredominantColor == colorA) {
      /* Must use second textures */
      gKeeperMaterial += "2";
      diskMaterial += "2";
      gKeeperName += "2";
      diskBaseName += "2";
   }

   /* Load Goal Keeper */
   gKeeper = new BtSoccer::GoalKeeper(gKeeperName, gKeeperFile,
         ogreSceneManager, debugDraw);
   gKeeper->hide();
   gKeeper->setTeam(this);
   /* Change its material */
   if(!gKeeperMaterial.empty())
   {
      gKeeper->setMaterial(gKeeperMaterial);
   }

   /* Load All Models */
   for(i = 2; i < f->getNumberOfDisks()+2; i++)
   {
      /* Create the disk */
      ss.str("");
      ss << i;
      disk[i-2] = new BtSoccer::TeamPlayer(FieldObject::TYPE_DISK,
            diskBaseName + ss.str(), diskFile, ogreSceneManager, debugDraw);
      disk[i-2]->setTeam(this);
      disk[i-2]->hide();

      /* TODO: set number on texture */

      /* Change its material */
      if(!diskMaterial.empty())
      {
         disk[i-2]->setMaterial(diskMaterial);
      }

      /* using the same for to set random init angles */
      disk[i-2]->setOrientation(
            Ogre::Degree(360.0 * (rand() / (RAND_MAX + 1.0))));
   }

   scManager = ogreSceneManager;
}

/*************************************************************
 *                        Destructor                         *
 *************************************************************/
Team::~Team()
{
   if(ai)
   {
      delete ai;
   }
   if(gKeeper)
   {
      delete gKeeper;
   }
   for(int i=0; i<TEAM_MAX_DISKS; i++)
   {
      if(disk[i])
      {
         delete disk[i];
      }
   }
}

/*************************************************************
 *                      getSceneManager                      *
 *************************************************************/
Ogre::SceneManager* Team::getSceneManager()
{
   return scManager;
}

/*************************************************************
 *                  startPositionAtField                     *
 *************************************************************/
void Team::startPositionAtField(bool upper, bool withBall,
                                Field* f)
{
   /* Set the multiplier to the position, if upper or not. */
   int m = (upper)?1:-1;
   
   Ogre::Vector2 halfSize = f->getHalfSize();
   Ogre::Vector2 sideDelta = f->getSideDelta();

   /* Set Goal Keeper */
   gKeeper->startPositionAtField(upper, f);

   /* Set Players */
   if(!withBall)
   {
      /* 2 FWD at sides */
      if(disk[8])
      {
         disk[8]->setPosition(m * 0.75f, 0,
               halfSize[1] - sideDelta[1] - 2.1);
      }
      if(disk[9])
      {
         disk[9]->setPosition(m * 0.75, 0,
               -halfSize[1] + sideDelta[1] + 2.1);
      }

      /* Center FWD */
      if(disk[2])
      {
         disk[2]->setPosition(m * 3.75, 0, 0);
      }

      /* 2 Midfields */
      if(disk[1])
      {
         disk[1]->setPosition(m * 0.6f, 0, 4.0f);
      }
      if(disk[0])
      {
         disk[0]->setPosition(m * 0.6f, 0, -4.0f);
      }

      /* 5 Defenders */
      if(disk[5])
      {
         disk[5]->setPosition(m * 5.5f, 0, 3.25f);
      }
      if(disk[3])
      {
         disk[3]->setPosition(m * 5.5, 0, -3.25f);
      }
      if(disk[4])
      {
         disk[4]->setPosition(m * 7.0f, 0, 0);
      }
      if(disk[6])
      {
         disk[6]->setPosition(m * 9.75f, 0, -2.5f);
      }
      if(disk[7])
      {
         disk[7]->setPosition(m * 9.75f, 0, 2.5f);
      }
   }
   else
   {
      /* 2 FWD at sides */
      if(disk[2])
      {
         disk[2]->setPosition(m * 0.75f, 0,
               halfSize[1] - sideDelta[1] - 0.6f);
      }
      if(disk[4])
      {
         disk[4]->setPosition(m * 0.75f, 0,
               -halfSize[1] + sideDelta[1] + 0.6f);
      }

      /* 2 Midfields and 1 FWD inner circle */
      if(disk[1])
      {
         disk[1]->setPosition(m, 0, -1.1f);
      }
      if(disk[0])
      {
         disk[0]->setPosition(m * 0.6f, 0, 0.5f);
      }
      if(disk[9])
      {
         disk[9]->setPosition(m * 2.0f, 0, 1.9f);
      }

      /* 5 Defenders */
      if(disk[5])
      {
         disk[5]->setPosition(m * 5.5f, 0, 3.25f);
      }
      if(disk[3])
      {
         disk[3]->setPosition(m * 5.5f, 0, -3.25f);
      }
      if(disk[8])
      {
         disk[8]->setPosition(m * 7.0f, 0, 0);
      }
      if(disk[6])
      {
         disk[6]->setPosition(m * 9.75f, 0, -2.5f);
      }
      if(disk[7])
      {
         disk[7]->setPosition(m * 9.75f, 0, 2.5f);
      }
   }
   
   /* Set the models to show */
   int i;
   gKeeper->show();
   for(i = 0; i < f->getNumberOfDisks(); i++)
   {
      disk[i]->show();
   }
}

/*************************************************************
 *                     getGoalKeeper                         *
 *************************************************************/
GoalKeeper* Team::getGoalKeeper()
{
   return gKeeper;
}

/*************************************************************
 *                         getDisk                           *
 *************************************************************/
TeamPlayer* Team::getDisk(int i)
{
   if((i >= 0) && (i < TEAM_MAX_DISKS))
   {
      return disk[i];
   }

   return NULL;
}

/*************************************************************
 *                       getDiskIndex                        *
 *************************************************************/
int Team::getDiskIndex(TeamPlayer* d)
{
   for(int i=0; i < TEAM_MAX_DISKS; i++)
   {
      if(disk[i] == d)
      {
         return i;
      }
   }
   return -1;
}

/*************************************************************
 *                           nearest                         *
 *************************************************************/
bool nearest(TeamPlayer* i, TeamPlayer* j) 
{ 
   return i->getLastCalculatedDistance() < j->getLastCalculatedDistance(); 
}

/*************************************************************
 *                 getPlayersNearestFirst                    *
 *************************************************************/
std::vector<TeamPlayer*> Team::getPlayersNearestFirst(float x, float z)
{
   std::vector<TeamPlayer*> res;

   for(int i = 0; i < TEAM_MAX_DISKS; i++)
   {
      if(disk[i])
      {
         /* Calculate distance (to valid check later on sort with
          * disk->getLastCalculatedDistance()). */
         disk[i]->getDistanceTo(x, z);

         /* Add to vector */
         res.insert(res.end(), disk[i]);
      }
   }

   /* Sort them, nearest first. */
   std::sort(res.begin(), res.end(), nearest);

   return res;
}

/*************************************************************
 *                    getNearestPlayer                       *
 *************************************************************/
TeamPlayer* Team::getNearestPlayer(float x, float z)
{
   TeamPlayer* res = disk[0];
   float minDist = res->getDistanceTo(x, z);
   float dist;
   int i;

   for(i = 1; i < TEAM_MAX_DISKS; i++)
   {
      if(disk[i])
      {
         /* Calculate distance */
         dist = disk[i]->getDistanceTo(x, z);

         /* Swap it, if is lesser than the current */
         if(dist < minDist)
         {
            minDist = dist;
            res = disk[i];
         }
      }
   }

   return res;
}

/*************************************************************
 *                           update                          *
 *************************************************************/
bool Team::update()
{
   int i;
   bool moves = false;

   /* Update goal keeper position */
   moves |= gKeeper->update();
   
   for(i=0; i<TEAM_MAX_DISKS; i++)
   {
      if(disk[i])
      {
         moves |= disk[i]->update();
      }
   }

   return moves;
}

/*************************************************************
 *                           update                          *
 *************************************************************/
void Team::update(ProtocolParsedMessage& msg)
{
   if(msg.msgAditionalInfo == UPDATE_GK_INDEX)
   {
      /* Must update goal keeper */
      gKeeper->setOrientation(msg.angles);
      gKeeper->setPositionWithoutForcedPhysicsStep(msg.position);
   }
   else
   {
      /* Must update a disk */
      int diskIndex = msg.msgAditionalInfo;
      disk[diskIndex]->setOrientation(msg.angles);
      disk[diskIndex]->setPositionWithoutForcedPhysicsStep(msg.position);
   }
}

/*************************************************************
 *                       getTeamLogo                         *
 *************************************************************/
Ogre::String Team::getLogo()
{
   return logo;
}

/*************************************************************
 *                          getColorA                        *
 *************************************************************/
Ogre::String Team::getColorA()
{
   return colorA;
}

/*************************************************************
 *                          getColorB                        *
 *************************************************************/
Ogre::String Team::getColorB()
{
   return colorB;
}

/*************************************************************
 *                         getTeamName                       *
 *************************************************************/
Ogre::String Team::getName()
{
   return name;
}

/*************************************************************
 *                       getTeamFileName                     *
 *************************************************************/
Ogre::String Team::getFileName()
{
   return fileName;
}

/*************************************************************
 *                       getTeamPlayer                       *
 *************************************************************/
TeamPlayer* Team::getTeamPlayer(Ogre::SceneNode* scNode)
{
   int i;
   /*TODO:  Verify goal Keeper */

   /* Verify disks */
   for(i=0; i < TEAM_MAX_DISKS; i++)
   {
      if((disk[i]) && (disk[i]->compare(scNode)))
      {
         return disk[i];
      }
   }

   return NULL;
}

/*************************************************************
 *                  getLastActiveTeamPlayer                  *
 *************************************************************/
TeamPlayer* Team::getLastActiveTeamPlayer()
{
   return lastActiveDisk;
}

/*************************************************************
 *                  setLastActiveTeamPlayer                  *
 *************************************************************/
void Team::setLastActiveTeamPlayer(TeamPlayer* tp)
{
   lastActiveDisk = tp;
}


/*************************************************************
 *                   setReplayerPositions                    *
 *************************************************************/
void Team::setReplayerPositions(Ogre::Vector3* pos, Ogre::Quaternion* angles)
{
   int i;

   /* Set GoalKeeper */
   gKeeper->setPositionAsReplay(pos[0], angles[0]);

   /* Set disks */
   for(i=0; i < TEAM_MAX_DISKS; i++)
   {
      if(disk[i])
      {
         disk[i]->setPositionAsReplay(pos[i+1], angles[i+1]);
      }
   }
}


/*************************************************************
 *                    setControlledByHuman                   *
 *************************************************************/
void Team::setControlledByHuman(bool b)
{
   controlledByHuman = b;
}

/*************************************************************
 *                     isControlledByHuman                   *
 *************************************************************/
bool Team::isControlledByHuman()
{
   return controlledByHuman;
}

/*************************************************************
 *                        prePhysicStep                      *
 *************************************************************/
void Team::prePhysicStep()
{
   int i;

   gKeeper->prePhysicStep();
   for(i=0; i < TEAM_MAX_DISKS; i++)
   {
      if(disk[i])
      {
         disk[i]->prePhysicStep();
      }
   }
}
 
/*************************************************************
 *                    movedOnLastPhysicStep                  *
 *************************************************************/     
bool Team::movedOnLastPhysicStep()
{
   int i;

   if(gKeeper->getMovedFlag())
   {
      return true;
   }

   for(i=0; i < TEAM_MAX_DISKS; i++)
   {
      if( (disk[i]) && (disk[i]->getMovedFlag()) )
      {
         return true;
      }
   }

   return false;
}

/*************************************************************
 *                     queueUpdatesToSend                    *
 *************************************************************/
void Team::queueUpdatesToSend(bool teamA, bool sendAll)
{
   int i;

   if((sendAll) || (gKeeper->getMovedFlag()))
   {
      protocol.queueTeamPlayerUpdateToSend(teamA, false, UPDATE_GK_INDEX,
         gKeeper->getPosition(), gKeeper->getOrientation(), sendAll);
   }

   for(i=0; i < TEAM_MAX_DISKS; i++)
   {
      if( (disk[i]) && ( (sendAll) || (disk[i]->getMovedFlag()) ) )
      {
         protocol.queueTeamPlayerUpdateToSend(teamA, false, i,
               disk[i]->getPosition(), disk[i]->getOrientation(), sendAll);
      }
   }
}


