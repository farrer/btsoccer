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


#ifndef _btsoccer_gui_replay_h
#define _btsoccer_gui_replay_h

#include <goblin/ibutton.h>
#include <goblin/textbox.h>
#include <goblin/screeninfo.h>
#include <OGRE/OgreRenderWindow.h>

namespace BtSoccer
{

/*! The GuiReplay class implements a video controller,
 * with stop, forward, backward, play, pause buttons, shutdown
 * and speed buttons */
class GuiReplay
{
   public:
      
      enum buttonTypes
      {
         BUTTON_NONE=-1,
         BUTTON_STOP,
         BUTTON_PLAY,
         BUTTON_PAUSE,
         BUTTON_FORWARD,
         BUTTON_BACKWARD,
         BUTTON_HALF_SPEED,
         BUTTON_NORMAL_SPEED,
         BUTTON_DOUBLE_SPEED,
         BUTTON_SHUTDOWN,
         TOTAL_BUTTONS
      };
    
      /*! Constructor */
      GuiReplay();
      
      /*! Destructor */
      ~GuiReplay();

      /*! Verify Button press at the controller
       * \return -> GuiReplay BUTTON constant, with button selected, if any */
      int verifyEvents(int mouseX, int mouseY, bool leftButtonPressed);

      /*! Hide the guiReplay */
      void hide();

      /*! Display the guiReplay */
      void show();

   protected:

      Goblin::Ibutton* button[TOTAL_BUTTONS]; /**< button textures */
      Goblin::TextBox* rText;                 /**< The R animated text */
      Ogre::Real rSize;                       /**< Current size of R TextBox */
      Ogre::Real rDelta;                      /**< Delta to rSize */
      Ogre::Overlay* ogreOverlay;             /**< The overlay */
};

}

#endif

