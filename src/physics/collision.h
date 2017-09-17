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



#ifndef _btsoccer_collision_h
#define _btsoccer_collision_h

#include <OGRE/OgreAxisAlignedBox.h>

#include "../engine/team.h"
#include "../engine/ball.h"
#include "../engine/field.h"

namespace BtSoccer
{

/*! The collision class which verify physical collisions
 * between the players, the ball and field elements. */
class Collision
{
   public:
      /*! Constructor */
      Collision();
      /*! Destructor */
      ~Collision();

      /*! Define the Team A
       * \param t -> pointer to the teamA */
      void setTeamA(Team* t);

      /*! Define the Team B
       * \param t -> pointer to the teamB */ 
      void setTeamB(Team* t);
   
      /*! Define the ball
       * \param b -> pointer to the ball */
      void setBall(Ball* b);
   
      /*! Define the Field
       * \param f -> pointer to the field */
      void setField(Field* f);
      
      /*! Remove all collisions from the system (re-ordering the players
       * on the field)
       * \param isolateBall -> if will put the ball isolated in a radius 
       *                       (usually used for fouls, etc.)
       * \param f -> pointer to current field */
      void removeContacts(bool isolateBall, Field* f);
   
      /*! Remove all players - except goalkeepers - from both penalty areas */
      void removeFromPenaltyAreas();

   protected:
      Team* teamA;        /**< The teamA pointer */
      Team* teamB;        /**< The teamB pointer */
      Ball* gameBall;     /**< The ball used pointer */
      Field* field;       /**< The current field pointer */

      /*! Remove All contacts of a disk.
       * \param isolateBall -> if will isolate the ball
       * \param disk -> pointer to the disk to remove contacts */
      bool removeContact(bool isolateBall, TeamPlayer* disk,
                         Field* f);

      /*! Put the disk away from the position
       * \param disk -> disk to put away
       * \param posX -> X position
       * \param posZ -> Z position
       * \param radius -> radius to keep away from */
      void getAwayFrom(TeamPlayer* disk, float posX, float posZ, 
                       float radius);

      /* Put 2 disks away from each other
       * \param disk -> first disk to get away
       * \param disk2 -> second disk to get away
       * \radius -> min radius from each other */
      void getAwayFrom(TeamPlayer* disk, TeamPlayer* disk2, float radius);
      
      /*! Put the disk away from the rectangular area
       * \param disk -> disk to put away
       * \param x1 -> lower x coordinate
       * \param z1 -> lower z coordinate
       * \param x2 -> upper x coordinate
       * \param z2 -> upper z coordinate */
      void getAwayFrom(TeamPlayer* disk, float x1, float z1, 
                       float x2, float z2);
   
      /*! Remove all players - except goalkeepers - from a penalty area
       * \param upper -> if upper penalty area or bottom. */
      void removeFromPenaltyArea(bool upper);
   
      /*! Verify if the disk has a potential contact (inner bounding box). */
      bool hasPotentialContact(TeamPlayer* disk, Ogre::Real radius);

      /*! Check all players orientations to be sure they are "up" */
      void checkOrientations();

};

};


#endif

