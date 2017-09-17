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

#ifndef _btsoccer_save_file_h
#define _btsoccer_save_file_h

#include <OGRE/OgreString.h>

#include "../btsoccer.h"
#include "team.h"
#include "field.h"

namespace BtSoccer
{

/*! SaveFile defines the structure of saved games, being able to 
 * save and load them. 
 * \note -> after load replay will be empty! */
class SaveFile
{
   public:
      /*! Constructor */
      SaveFile();
      /*! Destructor */
      ~SaveFile();

      /*! Load a savefile, setting the core engine, rules, etc. */
      bool load(Ogre::String fileName, BtSoccer::Core* core,
         BtSoccer::Team** teamA, BtSoccer::Team** teamB,
         BtSoccer::Field* field, BulletDebugDraw* debugDraw);

      /*! Save the current match to the save file of fileName */
      bool save(Ogre::String fileName, BtSoccer::Team* teamA, 
          BtSoccer::Team* teamB, int coreState);

   protected:

      int numHumans;            /**< Number of human players */
      Ogre::String cupFileName; /**< FileName of the cup (if any) */
};

}

#endif

