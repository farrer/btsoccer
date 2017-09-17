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

#ifndef _btsoccer_gui_options_h 
#define _btsoccer_gui_options_h

#include <goblin/ibutton.h>
#include <goblin/textbox.h>
#include <goblin/screeninfo.h>
#include <OGRE/OgreRenderWindow.h>

#include "../engine/field.h"

namespace BtSoccer
{
   
class GuiOptions
{
   public:
      
      /*! Constructor
       * \param ogreWindow -> current window */
      GuiOptions();
      /*! Destructor */
      ~GuiOptions();
      
      /*! Verify Events at the GUI
       * \param mouseButton -> current mouse button state
       * \param mouseX -> current mouseX position
       * \param mouseY -> current mouseY position
       * \return true when finished */
      bool verifyEvents(int mouseX, int mouseY, bool leftButtonPressed);
      
      /*! Hide the gui */
      void hide();
      
      /*! Show the gui */
      void show();
   
      /*! Update positions */
      void update();
   
      /*! Verify if the options gui is updating (moving) or not. */
      bool isUpdating();
   
      /*! Define buttons "hidden" positions */
      void setButtonsHiddenPos();
   
      /*! Save current options selected */
      void save(Field* field);
   
      /*! Get current options (usually, to restore them 
       *  on cancel) */
      void getCurrentOptions();
   
   protected:
   
      /*! Define buttons "displayed" positions */
      void setButtonsDispPos();
   
      Goblin::Ibutton* timeNotSelected[3];
      Goblin::Ibutton* timeSelected[3];
      int curTime;
   
      Goblin::Ibutton* fieldNotSelected[3];
      Goblin::Ibutton* fieldSelected[3];
      int curField;
   
      Goblin::Ibutton* cameraNotSelected[3];
      Goblin::Ibutton* cameraSelected[3];
      int curCamera;
   
      Ogre::Overlay* ogreOverlay;
};
   
}


#endif
 
