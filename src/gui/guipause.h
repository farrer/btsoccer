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


#ifndef _btsoccer_gui_pause_h
#define _btsoccer_gui_pause_h

#include <goblin/ibutton.h>
#include <goblin/textbox.h>
#include <goblin/screeninfo.h>
#include <OGRE/OgreRenderWindow.h>

namespace BtSoccer
{

/*! The GuiPause have interactive GUI needed when game is paused */
class GuiPause
{
   public:

      enum
      {
         BUTTON_RESUME=0,
         BUTTON_QUIT,
         BUTTON_SAVE,
         TOTAL_BUTTONS,
      };

      enum
      {
         RETURN_OTHER,
         RETURN_RESUME,
         RETURN_SAVE,
         RETURN_QUIT
      };

      /*! Costructor */
      GuiPause();
      /*! Destructor */
      ~GuiPause();

      /*! Verify Events at the GUI
       * \param mouseButton -> current mouse button state
       * \param mouseX -> current mouseX position 
       * \param mouseY -> current mouseY position
       * \return button constant or NO_EVENTS */
      int verifyEvents(int mouseX, int mouseY, bool leftButtonPressed);

      /*! Hide the gui */
      void hide();

      /*! Show the gui 
       * \param enableSave -> true if will enable save button
       * \param receivedMessage -> true iof the pause was received at
      *         a protocol message.*/
      void show(bool enableSave, bool receivedMessage);

   protected:
      Ogre::Overlay* ogreOverlay;  /**< the overlay used */
      Goblin::Ibutton* buttons[TOTAL_BUTTONS];  /**< The buttons */
      int returnStatus;      /**< internal return status */
};

}


#endif

