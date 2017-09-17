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

#include "guireplay.h"
using namespace BtSoccer;

#define GUI_REPLAY_BUTTON_SIZE    32
#define GUI_REPLAY_DELTA_Y        66

/*********************************************************************
 *                            Constructor                            *
 *********************************************************************/
GuiReplay::GuiReplay()
{
   int i;
   Ogre::Real posX, posY;
   Ogre::String fileName="";

   /* Create the overlay */
   ogreOverlay = Ogre::OverlayManager::getSingletonPtr()->create("replayOvl");
   ogreOverlay->setZOrder(640);
   ogreOverlay->show();

   /* Create the buttons */
   for(i = 0; i < TOTAL_BUTTONS; i++)
   {
      switch(i)
      {
         /* First line */
         case BUTTON_HALF_SPEED:
         {
            posX = 10 * Goblin::ScreenInfo::getGuiScale();
            posY = Goblin::ScreenInfo::getWindowHeight() - (10 + GUI_REPLAY_DELTA_Y +
                        GUI_REPLAY_BUTTON_SIZE) * Goblin::ScreenInfo::getGuiScale();
            fileName = "replay/12x.png";
         }
         break;
         case BUTTON_NORMAL_SPEED:
         {
            posX = (20 + GUI_REPLAY_BUTTON_SIZE) * Goblin::ScreenInfo::getGuiScale();
            posY = Goblin::ScreenInfo::getWindowHeight() - (10 + GUI_REPLAY_DELTA_Y +
                        GUI_REPLAY_BUTTON_SIZE) * Goblin::ScreenInfo::getGuiScale();
            fileName = "replay/1x.png";
         }
         break;
         case BUTTON_DOUBLE_SPEED:
         {
            posX = (30 + 2*GUI_REPLAY_BUTTON_SIZE) * Goblin::ScreenInfo::getGuiScale();
            posY = Goblin::ScreenInfo::getWindowHeight() - (10 + GUI_REPLAY_DELTA_Y +
                        GUI_REPLAY_BUTTON_SIZE) * Goblin::ScreenInfo::getGuiScale();
            fileName = "replay/2x.png";
         }
         break;

         /* Second Line */
         case BUTTON_PAUSE:
         {
            posX = (20 + GUI_REPLAY_BUTTON_SIZE) * Goblin::ScreenInfo::getGuiScale();
            posY = Goblin::ScreenInfo::getWindowHeight() - (40 + GUI_REPLAY_DELTA_Y +
                        4*GUI_REPLAY_BUTTON_SIZE) * Goblin::ScreenInfo::getGuiScale();
            fileName = "replay/pause.png";
         }
         break;

         /* Third Line */
         case BUTTON_BACKWARD:
         {
            posX = 10 * Goblin::ScreenInfo::getGuiScale();
            posY = Goblin::ScreenInfo::getWindowHeight() - (30 + GUI_REPLAY_DELTA_Y +
                        3*GUI_REPLAY_BUTTON_SIZE) * Goblin::ScreenInfo::getGuiScale();
            fileName = "replay/backward.png";
         }
         break;
         case BUTTON_PLAY:
         {
            posX = (20 + GUI_REPLAY_BUTTON_SIZE) * Goblin::ScreenInfo::getGuiScale();
            posY = Goblin::ScreenInfo::getWindowHeight() - (30 + GUI_REPLAY_DELTA_Y +
                        3*GUI_REPLAY_BUTTON_SIZE) * Goblin::ScreenInfo::getGuiScale();
            fileName = "replay/play.png";
         }
         break;
         case BUTTON_FORWARD:
         {
            posX = (30 + 2*GUI_REPLAY_BUTTON_SIZE) * Goblin::ScreenInfo::getGuiScale();
            posY = Goblin::ScreenInfo::getWindowHeight() - (30 + GUI_REPLAY_DELTA_Y +
                        3*GUI_REPLAY_BUTTON_SIZE) * Goblin::ScreenInfo::getGuiScale();
            fileName = "replay/forward.png";
         }
         break;

         /* Forth Line */
         case BUTTON_STOP:
         {
            posX = (20 + GUI_REPLAY_BUTTON_SIZE) * Goblin::ScreenInfo::getGuiScale();
            posY = Goblin::ScreenInfo::getWindowHeight() - (20 + GUI_REPLAY_DELTA_Y +
                        2*GUI_REPLAY_BUTTON_SIZE) * Goblin::ScreenInfo::getGuiScale();
            fileName = "replay/stop.png";
         }
         break;

         /* Sixth Line */
         case BUTTON_SHUTDOWN:
         {
            posX = 10 * Goblin::ScreenInfo::getGuiScale();
            posY = Goblin::ScreenInfo::getWindowHeight() - (50 + GUI_REPLAY_DELTA_Y +
                        5*GUI_REPLAY_BUTTON_SIZE) * Goblin::ScreenInfo::getGuiScale();
            fileName = "replay/shutdown.png";
         }
         break;
      }
   
      /* Create the button */
      button[i] = new Goblin::Ibutton(ogreOverlay, fileName, "gui");
      button[i]->setDimensions(GUI_REPLAY_BUTTON_SIZE * 
            Goblin::ScreenInfo::getGuiScale(),
            GUI_REPLAY_BUTTON_SIZE * Goblin::ScreenInfo::getGuiScale());
      button[i]->setPosition(posX, posY);
   }

   /* Create the R text */
   rText = new Goblin::TextBox(Goblin::ScreenInfo::getWindowWidth() -
         150 * Goblin::ScreenInfo::getGuiScale(), 10 * Goblin::ScreenInfo::getGuiScale(),
         32 * Goblin::ScreenInfo::getGuiScale(), 32 * Goblin::ScreenInfo::getGuiScale(),
         "R", "GuiReplayText", ogreOverlay, "infoFont", 18);
   rText->setColor(1.0f, 1.0f, 1.0f, 1.0f);
   rText->setAlignment(Ogre::TextAreaOverlayElement::Center);
   rSize = 18.0f * Goblin::ScreenInfo::getGuiScale();
   rDelta = -0.5f * Goblin::ScreenInfo::getGuiScale();

   /* Finally, hide it. */
   hide();
}


/*********************************************************************
 *                             Destructor                            *
 *********************************************************************/
GuiReplay::~GuiReplay()
{
   int i;
   for(i=0; i<TOTAL_BUTTONS; i++)
   {
      delete button[i];
   }
   delete rText;
   /* Bye overlays */
   Ogre::OverlayManager::getSingletonPtr()->destroy(ogreOverlay);
}

/****************************************************************
 *                        verifyButtons                         *
 ****************************************************************/
int GuiReplay::verifyEvents(int mouseX, int mouseY, 
      bool leftButtonPressed)
{
   int i;

   /* Update the rText size */
   rSize += rDelta;
   if( (rDelta > 0) && (rSize >= 20.0f * Goblin::ScreenInfo::getGuiScale()) )
   {
      rDelta = -0.5f * Goblin::ScreenInfo::getGuiScale();
      rSize = 20.0f * Goblin::ScreenInfo::getGuiScale();
   }
   else if( (rDelta < 0) && (rSize <= 8.0f * Goblin::ScreenInfo::getGuiScale()) )
   {
      rDelta = 0.5f * Goblin::ScreenInfo::getGuiScale();
      rSize = 8.0f * Goblin::ScreenInfo::getGuiScale();
   }
   rText->setFont("infoFont", rSize);

   for(i=0; i < TOTAL_BUTTONS; i++)
   {
      if( (i != BUTTON_FORWARD) && (i != BUTTON_BACKWARD) )
      {
         /* Normal button pressed */
         if(button[i]->verifyEvents(mouseX, mouseY, leftButtonPressed) == 
               IBUTTON_EVENT_PRESSED)
         {
            /* Pressed button! */
            return(i);
         }
      }
      else
      {
         /* Backward and Forward buttons must flow (continous press) */
         if(button[i]->verifyEvents(mouseX, mouseY, leftButtonPressed) == 
               IBUTTON_EVENT_ON_PRESS)
         {
            /* Pressing button! */
            return(i);
         }
      }
   }

   return(BUTTON_NONE);
}

/****************************************************************
 *                             hide                             *
 ****************************************************************/
void GuiReplay::hide()
{
   int i;
   for(i=0; i < TOTAL_BUTTONS; i++)
   {
      button[i]->hide();
   }
   rText->hide();
   ogreOverlay->hide();
}

/****************************************************************
 *                             show                             *
 ****************************************************************/
void GuiReplay::show()
{
   int i;
   for(i=0; i < TOTAL_BUTTONS; i++)
   {
      button[i]->show();
   }
   rText->show();
   ogreOverlay->show();
}

