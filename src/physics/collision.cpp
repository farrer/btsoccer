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

#include "collision.h"

#include "../soundfiles.h"
#include "../engine/ball.h"
#include "../engine/field.h"
#include "../engine/goalkeeper.h"
#include "../engine/teamplayer.h"

using namespace BtSoccer;

/*****************************************************************
 *                           Constructor                         *
 *****************************************************************/
Collision::Collision()
{
   teamA = NULL;
   teamB = NULL;
   field = NULL;
   gameBall = NULL;
}

/*****************************************************************
 *                           Destructor                          *
 *****************************************************************/
Collision::~Collision()
{
}

/*****************************************************************
 *                            setTeamA                           *
 *****************************************************************/
void Collision::setTeamA(Team* t)
{
   teamA = t;
}

/*****************************************************************
 *                            setTeamB                           *
 *****************************************************************/
void Collision::setTeamB(Team* t)
{
   teamB = t;
}

/*****************************************************************
 *                            setBall                            *
 *****************************************************************/
void Collision::setBall(Ball* b)
{
   gameBall = b;
}

/*****************************************************************
 *                            setField                           *
 *****************************************************************/
void Collision::setField(Field* f)
{
   field = f;
}

/*****************************************************************
 *                       checkOrientations                       *
 *****************************************************************/
void Collision::checkOrientations()
{
   int i;

   /* Verify Each Team */
   Team* curTeam = teamA;
   while(curTeam)
   {
      /* Verify Each Disk */
      for(i=0; i < field->getNumberOfDisks(); i++)
      {
         if(!curTeam->getDisk(i)->isFacingUp())
         {
            curTeam->getDisk(i)->setOrientation(
                  Ogre::Degree(curTeam->getDisk(i)->getOrientationY()));
         }
      }

      /* Verify GoalKeeper */
      if(!curTeam->getGoalKeeper()->isFacingUp())
      {
         Ogre::Vector3 pos = curTeam->getGoalKeeper()->getPosition();
         /* Must set position of Y to 0, as, rotated, propably is lesser. */
         curTeam->getGoalKeeper()->setPosition(pos.x, 0.0f, pos.z);
         /* Finally, set the angle */
         curTeam->getGoalKeeper()->setOrientation(
                  Ogre::Degree(curTeam->getGoalKeeper()->getOrientationY()));

      }

      /* Select next team or end */
      if(curTeam != teamB)
      {
         curTeam = teamB;
      }
      else
      {
         curTeam = NULL;
      }
   }
}

/*****************************************************************
 *                         removeContacts                        *
 *****************************************************************/
void Collision::removeContacts(bool isolateBall, Field* f)
{
   int i;

   /* Check orientations to be facing up */
   checkOrientations();

   /* Verify Each Team */
   Team* curTeam = teamA;
   while(curTeam)
   {
      /* Verify Each Disk */
      for(i=0; i < field->getNumberOfDisks(); i++)
      {
         if(removeContact(isolateBall, curTeam->getDisk(i), f))
         {
            /* One or more contacts were removed. Must verify all
             * disks again with the new defined position. */
            i = 0;
            curTeam = teamA;
         }
      }

      /* FIXME: Verify GoalKeeper */
      //removeContact(isolateBall, curTeam->getGoalKeeper());

      /* Select next team or end */
      if(curTeam != teamB)
      {
         curTeam = teamB;
      }
      else
      {
         curTeam = NULL;
      }
   }
}

/*****************************************************************
 *                         removeContact                         *
 *****************************************************************/
bool Collision::removeContact(bool isolateBall, TeamPlayer* disk,
                              Field* f)
{
   float diskRadius = disk->getSphere().getRadius() +
                        BTSOCCER_COLLISION_THRESHOLD_DELTA;
   float diskDiameter = 2*diskRadius;
   Ogre::Vector3 diskPos = disk->getPosition(); 
   float dist;
   Ogre::Vector2 halfSize = f->getHalfSize();
   Ogre::Vector2 sideDelta = f->getSideDelta();
   
   /* If is to isolateBall, must make sure the disks are inner the playable
    * field area (and not between side lines and borders)*/
   Ogre::Vector2 inDelta = (isolateBall)?sideDelta:Ogre::Vector2(0.0f, 0.0f);

   /* Make sure the disk is in the field  */
   if(diskPos.x-diskRadius < -halfSize[0] + inDelta[0])
   {
      disk->setPosition(diskRadius+sideDelta[0]-halfSize[0],
            0.0f, diskPos.z);
      diskPos = disk->getPosition();
   }
   else if(diskPos.x+diskRadius > halfSize[0] - inDelta[0])
   {
      disk->setPosition(halfSize[0]-diskRadius-sideDelta[0],
            0.0f, diskPos.z);
      diskPos = disk->getPosition();
   }
   if(diskPos.z-diskRadius < -halfSize[1] + inDelta[1])
   { 
      disk->setPosition(diskPos.x, 0.0f, 
            diskRadius+sideDelta[1]-halfSize[1]);
      diskPos = disk->getPosition();
   }
   else if(diskPos.z+diskRadius > halfSize[1] - inDelta[1])
   {
      disk->setPosition(diskPos.x, 0.0f, 
            halfSize[1]-diskRadius-sideDelta[1]);
      diskPos = disk->getPosition();
   }

   /* Verify contact with the ball */
   float ballRadius = gameBall->getSphere().getRadius();
   Ogre::Vector3 ballPos = gameBall->getPosition();
   if(isolateBall)
   {
      ballRadius += 3.0f*diskRadius;
   }
   dist = sqrt( (ballPos.x - diskPos.x) * 
                (ballPos.x - diskPos.x) +
                (ballPos.z - diskPos.z) * 
                (ballPos.z - diskPos.z) );
   if(dist <= ballRadius+diskRadius)
   { 
      /* Have a contact! Must get away from the ball */
      getAwayFrom(disk, ballPos.x, ballPos.z,
                  ballRadius+diskRadius);
      return(true);
   }

   /* Verify contact with all other disks */
   Team* curTeam = teamA;
   TeamPlayer* teamDisk;
   Ogre::Vector3 disk2Pos;
   int i;
   while(curTeam != NULL)
   {
      for(i = 0; i < field->getNumberOfDisks(); i++)
      {
         teamDisk = curTeam->getDisk(i);
         disk2Pos = teamDisk->getPosition();
         /* Verify contact, if the disks are distinct */
         if(disk != teamDisk)
         {
            dist =  sqrt( (disk2Pos.x - diskPos.x) * 
                          (disk2Pos.x - diskPos.x) +
                          (disk2Pos.z - diskPos.z) * 
                          (disk2Pos.z - diskPos.z) );
            if(dist < diskDiameter)
            {
               /* Have a contact! Must get away from the disk */
               getAwayFrom(disk, teamDisk, diskDiameter);
               return(true);
            }
         }
      }

      /* Verify Contact with the goal and goalKeeper! */
      //TODO

      /* Look at the next team or end */
      if(curTeam == teamA)
      {
         curTeam = teamB;
      }
      else
      {
         curTeam = NULL;
      }
   }
   return(false);
}

/*****************************************************************
 *                          getAwayFrom                          *
 *****************************************************************/
void Collision::getAwayFrom(TeamPlayer* disk, TeamPlayer* disk2, float radius)
{
   Ogre::Vector3 diskPos = disk->getPosition();
   Ogre::Vector3 disk2Pos = disk2->getPosition();
   
   /* Calculate how much we need to move to be at "radius" distance */
   Ogre::Real curDist = diskPos.distance(disk2Pos);
   Ogre::Real needToMove = radius-curDist;

   /* needToMove+ to be > not >=, and half, as will move each disk. */
   needToMove += BTSOCCER_COLLISION_THRESHOLD_DELTA;
   needToMove /= 2.0f;
   
   /* Get the direction to the point */
   Ogre::Vector2 v;
   v.x = disk2Pos.x - diskPos.x;
   v.y = disk2Pos.z - diskPos.z;
   v.normalise();

   /* And Go away from that direction */
   disk->setPosition(diskPos.x-needToMove*v.x, 0.0f,
         diskPos.z-needToMove*v.y);
   disk2->setPosition(disk2Pos.x+needToMove*v.x, 0.0f,
         disk2Pos.z+needToMove*v.y);
}

/*****************************************************************
 *                          getAwayFrom                          *
 *****************************************************************/
void Collision::getAwayFrom(TeamPlayer* disk, float posX, 
                            float posZ, float radius)
{
   /* Radius+ to be > not >= */
   radius += BTSOCCER_COLLISION_THRESHOLD_DELTA;

   /* Get the direction to the point */
   Ogre::Vector2 v;
   v.x = posX - disk->getPosition().x;
   v.y = posZ - disk->getPosition().z;
   v.normalise();

   /* And Go away from that direction */
   disk->setPosition(posX-radius*v.x, 0.0f, posZ-radius*v.y);
}

/*****************************************************************
 *                          getAwayFrom                          *
 *****************************************************************/
void Collision::getAwayFrom(TeamPlayer* disk, float x1, float z1, 
                       float x2, float z2)
{
#if 0
   /* FIXME: BUGGY! */
   float d1, d2, d3, d4;

   d1 = disk->getPosX() - x1;
   d2 = disk->getPosZ() - z1;
   d3 = x2 - disk->getPosX();
   d4 = z2 - disk->getPosZ();

   if( (d1 <= d2) && (d1 <= d3) && (d1 <= d4) )
   {
      disk->setPosition(x1-1, disk->getPosZ());
   }
   else if( (d2 <= d3) && (d2 <= d4))
   {
      disk->setPosition(disk->getPosX(), z1-1);
   }
   else if(d3 <= d4)
   {
      disk->setPosition(x2+1, disk->getPosZ());
   }
   else
   {
      disk->setPosition(disk->getPosX(), z2+1);
   }
#endif
}

/*****************************************************************
 *                    removeFromPenaltyAreas                     *
 *****************************************************************/
void Collision::removeFromPenaltyAreas()
{
   removeFromPenaltyArea(true);
   removeFromPenaltyArea(false);
}

/*****************************************************************
 *                     hasPotentialContact                       *
 *****************************************************************/
bool Collision::hasPotentialContact(TeamPlayer* disk, Ogre::Real radius)
{
   int i;
   /* Retrieve the bounding box. */
   Ogre::Sphere diskSphere = disk->getSphere();
   
   /* Verify Each Team */
   Team* curTeam = teamA;
   while(curTeam)
   {
      /* Verify Each Disk */
      for(i=0; i < field->getNumberOfDisks(); i++)
      {
         TeamPlayer* tp = curTeam->getDisk(i);
         if(tp != disk)
         {
            if(diskSphere.intersects(tp->getSphere()))
            {
               return true;
            }
         }
      }
      /* Select next team or end */
      if(curTeam != teamB)
      {
         curTeam = teamB;
      }
      else
      {
         curTeam = NULL;
      }
   }
   
   return false;
}

/*****************************************************************
 *                    removeFromPenaltyArea                      *
 *****************************************************************/
void Collision::removeFromPenaltyArea(bool upper)
{
   int i;
   Ogre::Real rightPos = field->getHalfSize()[1] - field->getSideDelta()[1];
   Ogre::Real leftPos = -field->getHalfSize()[1] + field->getSideDelta()[1];
   
   /* Verify Each Team */
   Team* curTeam = teamA;
   while(curTeam)
   {
      /* Verify Each Disk */
      for(i=0; i < field->getNumberOfDisks(); i++)
      {
         TeamPlayer* tp = curTeam->getDisk(i);
         if(field->isInnerPenaltyArea(tp->getPosition().x, tp->getPosition().z,
                                      upper, !upper))
         {
            float diskRadius = tp->getSphere().getRadius() +
                               BTSOCCER_COLLISION_THRESHOLD_DELTA;
            /* Try a new position on "line" */
            Ogre::Real xPos;
            Ogre::Real initXPos;
            if(upper)
            {
               xPos = field->getPenaltyAreaDelta()[0]-diskRadius;
            }
            else
            {
               xPos = -field->getPenaltyAreaDelta()[0]+diskRadius;
            }
            initXPos = xPos;
            Ogre::Real zPos = tp->getPosition().z;
            tp->setPosition(xPos, tp->getPosition().y, zPos);
            while(hasPotentialContact(tp, diskRadius))
            {
               /* Let's try a new position. */
               if(upper)
               {
                  /* Go down. */
                  xPos -= diskRadius;
                  if(xPos < 0.0f)
                  {
                     /* After middle point, go right. */
                     xPos = initXPos;
                     zPos += diskRadius;
                     if(zPos >= rightPos)
                     {
                        zPos = leftPos;
                     }
                  }
               }
               else
               {
                  /* Go up */
                  xPos += diskRadius;
                  if(xPos > 0.0f)
                  {
                     /* After middle point, go right. */
                     xPos = initXPos;
                     zPos += diskRadius;
                     if(zPos >= rightPos)
                     {
                        zPos = leftPos;
                     }
                  }
               }
               /* Set new position and verify contacts again. */
               tp->setPosition(xPos, tp->getPosition().y, zPos);
            }
         }
      }
      /* Select next team or end */
      if(curTeam != teamB)
      {
         curTeam = teamB;
      }
      else
      {
         curTeam = NULL;
      }
   }
}

