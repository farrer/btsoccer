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

#ifndef _btsoccer_teams_h
#define _btsoccer_teams_h

#include <kobold/list.h>
#include <goblin/ibutton.h>
#include <goblin/textbox.h>
#include <OGRE/OgreRenderWindow.h>

namespace BtSoccer
{

class Region;
   
/*! The TeamInfo reatains team descriptions and basic informations */
class TeamInfo : public Kobold::ListElement
{
   public:
      Ogre::String name;      /**< team name */
      Region* region;         /**< team geographic region */
      Ogre::String fileName;  /**< Filename */
      Ogre::String prefix;    /**< Team file's prefix */
};

/*! Geographic region where team belongs to. Is also a list of teams. */
class Region : public Kobold::List
{
   public:
      /*! Constructor */
      Region();

      /*! Destructor */
      ~Region();

      int id;                   /**< Region Index*/
      Ogre::String name;        /**< Region Name */
      Ogre::String imageFile;   /**< Image filename to be used as button */
   
      /*! Insert the TeamInfo sorted at the list. */
      bool sortedInsert(TeamInfo* t);
};


/*! The Regions class have all regions and teams of game */
class Regions
{
   public:
      /*! Load all teams information */
      static void load();
      /*! Clear all loaded team infos */
      static void clear();

      /*! get total regions defined */
      static int getTotalRegions(){return totalRegions;};
   
      /*! get total teams defined. */
      static int getTotalTeams(){return totalTeams;};

      /*! Get the team related to its fileName
       * \param fileName -> filename of the team to get
       * \return -> pointer to its TeamInfo */
      static TeamInfo* getTeam(Ogre::String fileName);
   
      /*! Get the first team of first region */
      static TeamInfo* getFirstTeam();
   
      /*! Get the first team of a region
       * \param regionIndex index of the region to get first team. */
      static TeamInfo* getFirstTeam(int regionIndex);
      /*! Get the next team, including teams of other regions.
       * \param curTeam current team to get next. If it's the last
       *                 team of a region, get first team of next region. 
       * \note use this function to cycle throught teams, instead of 
       *       TeamInfo::getNext() wich won't cycle between regions.. */
      static TeamInfo* getNextTeam(TeamInfo* curTeam);

      /*! Get region of index i */
      static Region* getRegion(int i);

   private:
      Regions(){};
      static Region* regions;     /**< All regions */
      static int totalRegions;    /**< Total regions */
      static int totalTeams;      /**< Total Teams */
};

}

#endif

