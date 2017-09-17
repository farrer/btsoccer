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


#ifndef _btsoccer_options_h
#define _btsoccer_options_h

#include <OGRE/OgreString.h>

namespace BtSoccer
{

/*! The options class keep (and load/save) the options
 * related to theBtSoccer game. */
class Options
{
   public:

      /*! Types of Camera */
      enum
      {
         CAMERA_FIXED,
         CAMERA_NORMAL,
         CAMERA_FOLLOW_BALL
      };
   
      /*! Types of Field */
      enum
      {
         FIELD_CHILD,
         FIELD_MEDIUM,
         FIELD_PROFESSIONAL
      };
   
      /*! Load options from file. */
      static void load();
      /*! Save current options value to file */
      static void save();
      /*! Define default minutes per half
       * \param minutes number of minutes per half */
      static void setMinutesPerHalf(int minutes);
      /*! Define current camera type
       * \param cam new camera type */
      static void setCameraType(int cam);
      /*! Define current field size
       * \param field new field size */
      static void setFieldSize(int field);
      /*! Get current field filename */
      static Ogre::String getFieldFile();
      /*! Get field filename for fieldSizeConst */
      static Ogre::String getFieldFile(int fieldSizeConst);
      /*! Get current field size */
      static int getFieldSize();
      /*! Get current camera type */
      static int getCameraType();
      /*! Get current minutes per half */
      static int getMinutesPerHalf();

   protected:
      /*! No allowed instances */
      Options(){};
      static int cameraType;     /**< Camera Type */
      static int fieldSize;      /**< Field Size */
      static int minutesPerHalf; /**< Minutes Per Half */

};

}

#endif


