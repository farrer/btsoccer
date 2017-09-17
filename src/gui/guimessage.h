/*
  BtSoccer - button football (soccer) game
  Copyright (C) C) DNTeam <btsoccer@dnteam.org>

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


#ifndef _btsoccer_guimessage_h
#define _btsoccer_guimessage_h

#include <goblin/image.h>
#include <goblin/textbox.h>
#include <goblin/screeninfo.h>

#include <kobold/timer.h>


#include <OGRE/OgreRenderWindow.h>

namespace BtSoccer
{

/*! The guiMessage class is simple a box where messages
 * texts are written to the users. */
class GuiMessage
{
   public:
      /*! Finish the use of GuiMessage
       * \note -> must be called before quit the program. */
      static void finish();

      /*! Update the guiMessage display/hide */
      static void update();

      /*! Init the guiMessage to use
       * \param imageFile -> fileName of the image to use as back */
      static void init(Ogre::String imageFile);

      /*! Set the message on the guiMessage
       * \param msg -> string with message to show */
      static void set(Ogre::String msg);
   
      /*! Clear all current messages */
      static void clear();

   private:
      GuiMessage(){};

      static Ogre::Overlay* overlay;    /**< The overlay used */

      static Goblin::Image*    image;   /**< The image used as back */
      static Goblin::TextBox*  text;    /**< The displayed text */

      static Kobold::Timer dispTimer;   /**< Display timer */
      static bool display;              /**< True if displaying */
   
};

}

#endif

