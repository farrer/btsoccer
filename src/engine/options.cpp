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

#include "options.h"
#include <kobold/ogre3d/ogredefparser.h>
#include <kobold/userinfo.h>

using namespace BtSoccer;
#include <iostream>
using namespace std;

#define OPTIONS_TOKEN_MINUTES_PER_HALF "minutesPerHalf"
#define OPTIONS_TOKEN_FIELD_SIZE "fieldSize"
#define OPTIONS_TOKEN_CAMERA_TYPE "cameraType"

#define OPTIONS_FILENAME "options.cfg"

/***********************************************************************
 *                               load                                  *
 ***********************************************************************/
void Options::load()
{
   Kobold::OgreDefParser def;
   Ogre::String key, value;
   
   /* Try to load options */
   if(!def.load(Kobold::UserInfo::getUserHome()+OPTIONS_FILENAME, true, false))
   {
      /* No options saved yet. */
      return;
   }
   
   /* Parse each key/value */
   while(def.getNextTuple(key, value))
   {
      if(key == OPTIONS_TOKEN_MINUTES_PER_HALF)
      {
         sscanf(value.c_str(), "%d", &minutesPerHalf);
      }
      else if(key == OPTIONS_TOKEN_FIELD_SIZE)
      {
         sscanf(value.c_str(), "%d", &fieldSize);
      }
      else if(key == OPTIONS_TOKEN_CAMERA_TYPE)
      {
         sscanf(value.c_str(), "%d", &cameraType);
      }
   }

}

/***********************************************************************
 *                               save                                  *
 ***********************************************************************/
void Options::save()
{
   ofstream file;
   
   /* Open the file */
   file.open((Kobold::UserInfo::getUserHome()+OPTIONS_FILENAME).c_str(),
             ios::out | ios::binary);
   if(!file)
   {
      return;
   }
   
   file << OPTIONS_TOKEN_CAMERA_TYPE << " = " << cameraType << endl;
   file << OPTIONS_TOKEN_FIELD_SIZE << " = " << fieldSize << endl;
   file << OPTIONS_TOKEN_MINUTES_PER_HALF << " = "
        << minutesPerHalf << endl;
   
   /* Close it */
   file.close();
}

/***********************************************************************
 *                           setCameraType                             *
 ***********************************************************************/
void Options::setCameraType(int cam)
{
   cameraType = cam;
}

/***********************************************************************
 *                           getCameraType                             *
 ***********************************************************************/
int Options::getCameraType()
{
   return cameraType;
}

/***********************************************************************
 *                         setMinutesPerHalf                           *
 ***********************************************************************/
void Options::setMinutesPerHalf(int minutes)
{
   minutesPerHalf = minutes;
}

/***********************************************************************
 *                         getMinutesPerHalf                           *
 ***********************************************************************/
int Options::getMinutesPerHalf()
{
   return minutesPerHalf;
}

/***********************************************************************
 *                           setFieldSize                              *
 ***********************************************************************/
void Options::setFieldSize(int fs)
{
   fieldSize = fs;
}

/***********************************************************************
 *                           getFieldSize                              *
 ***********************************************************************/
int Options::getFieldSize()
{
   return fieldSize;
}

/***********************************************************************
 *                           getFieldFile                              *
 ***********************************************************************/
Ogre::String Options::getFieldFile()
{
   return getFieldFile(fieldSize);
}

/***********************************************************************
 *                           getFieldFile                              *
 ***********************************************************************/
Ogre::String Options::getFieldFile(int fieldSizeConst)
{
   switch(fieldSizeConst)
   {
      case FIELD_CHILD:
         return "field/child.cfg";
      case FIELD_MEDIUM:
         return "field/amateur.cfg";
      case FIELD_PROFESSIONAL:
      default:
         return "field/professional.cfg";
   }
}


/***********************************************************************
 *                           static fields                             *
 ***********************************************************************/
int Options::cameraType = CAMERA_NORMAL;
int Options::minutesPerHalf = 5;
int Options::fieldSize = FIELD_PROFESSIONAL;

