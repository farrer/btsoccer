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

#include "teams.h"
#include <kobold/defparser.h>
#include <kobold/i18n.h>

using namespace BtSoccer;

/***********************************************************************
 *                             Constructor                             *
 ***********************************************************************/
Region::Region()
{
}


/***********************************************************************
 *                              Destructor                             *
 ***********************************************************************/
Region::~Region()
{
}

/***********************************************************************
 *                              sortedInsert                           *
 ***********************************************************************/
bool Region::sortedInsert(TeamInfo* t)
{
   if(getTotal() == 0)
   {
      /* no elements, insert as first */
      return insert(t);
   }
   
   /* Search for position to insert */
   TeamInfo* w = (TeamInfo*)getFirst();
   for(int i=0; i<getTotal(); i++)
   {
      if(t->name.compare(w->name) < 0)
      {
         /* t name lesser than w, must insert before it. */
         return insertBefore(t, w);
      }
      w = (TeamInfo*)w->getNext();
   }
   
   /* Not inserted yet, insert as last. */
   return insertAtEnd(t);
}

/***********************************************************************
 *                         Load team definitions                       *
 ***********************************************************************/
void Regions::load()
{
   Ogre::String fileName="";
   Kobold::DefParser def;
   Ogre::String key, value;
   int curRegion=-1;
   
   totalTeams = 0;
   totalRegions=0;
   def.load("teams.lst");
   while(def.getNextTuple(key, value))
   {
      if(key == "totalRegions")
      {
         /* Get number of teams */
         sscanf(value.c_str(), "%d", &totalRegions);
         regions = new Region[totalRegions];
      }
      else if(key == "region")
      {
         /* team geographic region */
         curRegion++;
         regions[curRegion].id = curRegion;
         regions[curRegion].name = Kobold::i18n::translate(value);
      }
      else if(key == "regionImage")
      {
         regions[curRegion].imageFile = value;
      }
      else
      {
         TeamInfo* t = new TeamInfo();
         t->name = Kobold::i18n::translate(key);
         t->prefix = value;
         t->region = &regions[curRegion];
         t->fileName = value+Ogre::String(".xut");
         
         regions[curRegion].sortedInsert(t);
         totalTeams++;
      }
   }
}

/***********************************************************************
 *                                 clear                               *
 ***********************************************************************/
void Regions::clear()
{
   if(regions)
   {
      delete[] regions;
      regions = NULL;
   }
}

/***********************************************************************
 *                              getTeam                                *
 ***********************************************************************/
TeamInfo* Regions::getTeam(Ogre::String fileName)
{
   int i;
   TeamInfo* t = getFirstTeam();
   for(i=0; i < totalTeams; i++)
   {
      if(t->fileName == fileName)
      {
         return t;
      }
      t = getNextTeam(t);
   }

   return NULL;
}

/***********************************************************************
 *                            getFirstTeam                             *
 ***********************************************************************/
TeamInfo* Regions::getFirstTeam()
{
   Region* region = getRegion(0);
   
   if(region)
   {
      return (TeamInfo*)region->getFirst();
   }
   
   return NULL;
}

/***********************************************************************
 *                            getFirstTeam                             *
 ***********************************************************************/
TeamInfo* Regions::getFirstTeam(int regionIndex)
{
   Region* region = getRegion(regionIndex);
   
   if(region)
   {
      return (TeamInfo*)region->getFirst();
   }
   
   return NULL;
}

/***********************************************************************
 *                            getNextTeam                             *
 ***********************************************************************/
TeamInfo* Regions::getNextTeam(TeamInfo* curTeam)
{
   if(!curTeam)
   {
      return NULL;
   }
   int curId = curTeam->region->id;
   if(curTeam == curTeam->region->getLast())
   {
      /* is last team of region, get the first of next region  */
      TeamInfo* t = NULL;
      do
      {
         curId = ((curId+1) % totalRegions);
         t = getFirstTeam(curId);
      } while(t == NULL);
      return t;
   }
   /* Not last of region, get next team of same region */
   return (TeamInfo*)curTeam->getNext();
}

/************************************************************************
 *                                getRegion                             *
 ************************************************************************/
Region* Regions::getRegion(int i)
{
   if((i >= 0) && (i < totalRegions))
   {
      return(&regions[i]);
   }

   return(NULL);
}

/************************************************************************
 *                              Static Members                          *
 ************************************************************************/
int Regions::totalTeams = 0;
Region* Regions::regions = NULL;
int Regions::totalRegions = 0;

