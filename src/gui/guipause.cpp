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

#include "guipause.h"
#include "../soundfiles.h"
#include <kobold/ogre3d/i18n.h>

#define PAUSE_BUTTON_Y   120

#define RESUME_BUTTON_X  400
#define QUIT_BUTTON_X    528
#define SAVE_BUTTON_X    464

using namespace BtSoccer;

/***********************************************************************
 *                             Constructor                             *
 ***********************************************************************/
GuiPause::GuiPause()
{
   returnStatus = RETURN_OTHER;

   /* Create the overlay */
   ogreOverlay = Ogre::OverlayManager::getSingletonPtr()->create("pauseOvl");
   ogreOverlay->setZOrder(640);
   ogreOverlay->show();

   int fontSize = 16;
   int buttonSize = 64*Goblin::ScreenInfo::getGuiScale();
   int overSize = 65*Goblin::ScreenInfo::getGuiScale();
   int hidePos = -100*Goblin::ScreenInfo::getGuiScale();

   /* Create the buttons */
   buttons[BUTTON_RESUME] = new Goblin::Ibutton(ogreOverlay, "main/resume.png",
         "gui", Kobold::i18n::translate("Resume"), "infoFontOut", fontSize);
   buttons[BUTTON_RESUME]->setPosition(hidePos, hidePos);
   buttons[BUTTON_RESUME]->setDimensions(buttonSize, buttonSize);
   buttons[BUTTON_RESUME]->setMouseOverDimensions(overSize, overSize);
   buttons[BUTTON_RESUME]->setPressedSound(BTSOCCER_SOUND_GUI_CLICK);

   buttons[BUTTON_QUIT] = new Goblin::Ibutton(ogreOverlay, "initial/quit.png",
         "gui", Kobold::i18n::translate("Quit"), "infoFontOut", fontSize);
   buttons[BUTTON_QUIT]->setPosition(hidePos, hidePos);
   buttons[BUTTON_QUIT]->setDimensions(buttonSize, buttonSize);
   buttons[BUTTON_QUIT]->setMouseOverDimensions(overSize, overSize);
   buttons[BUTTON_QUIT]->setPressedSound(BTSOCCER_SOUND_GUI_CLICK2);

   buttons[BUTTON_SAVE] = new Goblin::Ibutton(ogreOverlay, "initial/save.png",
         "gui", Kobold::i18n::translate("Save"), "infoFontOut", fontSize);
   buttons[BUTTON_SAVE]->setPosition(hidePos, hidePos);
   buttons[BUTTON_SAVE]->setDimensions(buttonSize, buttonSize);
   buttons[BUTTON_SAVE]->setMouseOverDimensions(overSize, overSize);
   buttons[BUTTON_SAVE]->setPressedSound(BTSOCCER_SOUND_GUI_CLICK);

   hide();
}

/*********************************************************************
 *                             Destructor                            *
 *********************************************************************/
GuiPause::~GuiPause()
{
   int i;
   for(i=0; i<TOTAL_BUTTONS; i++)
   {
      delete buttons[i];
   }
   /* Bye overlay */
   Ogre::OverlayManager::getSingletonPtr()->destroy(ogreOverlay);
}


/*********************************************************************
 *                                show                               *
 *********************************************************************/
void GuiPause::show(bool enableSave, bool receivedMessage)
{
   if(!receivedMessage){
      buttons[BUTTON_RESUME]->setTargetPosition(
            RESUME_BUTTON_X*Goblin::ScreenInfo::getGuiScale(),
            PAUSE_BUTTON_Y*Goblin::ScreenInfo::getGuiScale(), 50);
   }
   buttons[BUTTON_QUIT]->setTargetPosition(
         QUIT_BUTTON_X*Goblin::ScreenInfo::getGuiScale(), 
         PAUSE_BUTTON_Y*Goblin::ScreenInfo::getGuiScale(), 50);

   if(enableSave)
   {
      buttons[BUTTON_SAVE]->setTargetPosition(
            SAVE_BUTTON_X*Goblin::ScreenInfo::getGuiScale(), 
            (PAUSE_BUTTON_Y+70)*Goblin::ScreenInfo::getGuiScale(), 50);
   }
   ogreOverlay->show();
}

/*********************************************************************
 *                                hide                               *
 *********************************************************************/
void GuiPause::hide()
{
   ogreOverlay->hide();
}

/********************************************************************
 *                          verifyEvents                            *
 ********************************************************************/
int GuiPause::verifyEvents(int mouseX, int mouseY, bool leftButtonPressed)
{
   bool isUpdating = false;
   bool buttonPressed = false;

   int i;
   for(i=0; i<TOTAL_BUTTONS; i++)
   {
      buttons[i]->update();
      isUpdating |= buttons[i]->isUpdating();


      if(buttons[i]->verifyEvents(mouseX, mouseY, leftButtonPressed) == 
            IBUTTON_EVENT_PRESSED)
      {
         switch(i)
         {
            case BUTTON_RESUME:
               buttonPressed = true;
               returnStatus = RETURN_RESUME;
            break;
            case BUTTON_QUIT:
               buttonPressed = true;
               returnStatus = RETURN_QUIT;
            break;
            case BUTTON_SAVE:
               buttonPressed = true;
               returnStatus = RETURN_SAVE;
            break;
         }
      }
   }

   if(buttonPressed)
   {
      /* Send buttons to quit */
      int hidePos = -100*Goblin::ScreenInfo::getGuiScale();
      buttons[BUTTON_RESUME]->setTargetPosition(hidePos, hidePos, 50);
      buttons[BUTTON_QUIT]->setTargetPosition(hidePos, hidePos, 50);
      buttons[BUTTON_SAVE]->setTargetPosition(hidePos, hidePos, 50);
   }
   
   if( (!isUpdating) && (!buttonPressed) )
   {
      int ret = returnStatus;
      returnStatus = RETURN_OTHER;
      return ret;
   }
   return RETURN_OTHER;
}




