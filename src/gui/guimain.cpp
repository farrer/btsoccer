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

#include "guimain.h"
#include "../soundfiles.h"
#include <kobold/i18n.h>

using namespace BtSoccer;

/***********************************************************************
 *                             Constructor                             *
 ***********************************************************************/
GuiMain::GuiMain(Ogre::Overlay* overlay, Ogre::SceneManager* sceneManager)
{
   int fontSize = 16;
   int buttonSize = 64*Goblin::ScreenInfo::getGuiScale();
   int buttonOver = 65*Goblin::ScreenInfo::getGuiScale();
   int hidePos = -80*Goblin::ScreenInfo::getGuiScale();
   
   /* Create the buttons */
   buttons[BUTTON_SHOOT] = new Goblin::Ibutton(overlay, "main/goal.png", "gui",
         Kobold::i18n::translate("Goal"), "infoFontOut", fontSize);
   buttons[BUTTON_SHOOT]->setPosition(hidePos, hidePos);
   buttons[BUTTON_SHOOT]->setDimensions(buttonSize, buttonSize);
   buttons[BUTTON_SHOOT]->setMouseOverDimensions(buttonOver, buttonOver);
   buttons[BUTTON_SHOOT]->setPressedSound(BTSOCCER_SOUND_GUI_CLICK);

   buttons[BUTTON_REPLAY] = new Goblin::Ibutton(overlay, "main/replay.png",
         "gui", Kobold::i18n::translate("Replay"), "infoFontOut", fontSize);
   buttons[BUTTON_REPLAY]->setPosition(hidePos, hidePos);
   buttons[BUTTON_REPLAY]->setDimensions(buttonSize, buttonSize);
   buttons[BUTTON_REPLAY]->setMouseOverDimensions(buttonOver, buttonOver);
   buttons[BUTTON_REPLAY]->setPressedSound(BTSOCCER_SOUND_GUI_CLICK);

   buttons[BUTTON_PAUSE] = new Goblin::Ibutton(overlay, "main/pause.png",
         "gui", Kobold::i18n::translate("Pause"), "infoFontOut", fontSize);
   buttons[BUTTON_PAUSE]->setPosition(hidePos, hidePos);
   buttons[BUTTON_PAUSE]->setDimensions(buttonSize, buttonSize);
   buttons[BUTTON_PAUSE]->setMouseOverDimensions(buttonOver, buttonOver);
   buttons[BUTTON_PAUSE]->setPressedSound(BTSOCCER_SOUND_GUI_CLICK2);
   
   buttons[BUTTON_ACCEPT] = new Goblin::Ibutton(overlay, "initial/accept.png",
         "gui", Kobold::i18n::translate("Accept"), "infoFontOut", fontSize);
   buttons[BUTTON_ACCEPT]->setPosition(hidePos, hidePos);
   buttons[BUTTON_ACCEPT]->setDimensions(buttonSize, buttonSize);
   buttons[BUTTON_ACCEPT]->setMouseOverDimensions(buttonOver, buttonOver);
   buttons[BUTTON_ACCEPT]->setPressedSound(BTSOCCER_SOUND_GUI_CLICK);
   
   /* Create the force bar */
   forceBar = new Goblin::Ibar(overlay, "main/bar_l.png", "main/bar_c.png",
         "main/bar_r.png", 0, Goblin::ScreenInfo::getWindowHeight() -
         (98 * Goblin::ScreenInfo::getGuiScale()),
         256 * Goblin::ScreenInfo::getGuiScale(), 
         (Goblin::ScreenInfo::shouldUseDoubleSizedGui())?26:6, "gui");
   forceBar->hide();
   
   /* Create the force direction */
   forceDir = new Goblin::Model3d("forceDirec", "direc/direc.mesh", "game",
         sceneManager, Goblin::Model3d::MODEL_DYNAMIC);
   forceDir->setOrientationNow(-90.0f, 0.0f, 0.0f);
   forceDir->setPositionNow(Ogre::Vector3(0.0f, 0.15f, 0.0f));
   forceDir->setScaleNow(0.16f, 0.16f, 0.16f);
   forceDir->hide();
   
   onlineGame = false;
   active = false;
   curState = STATE_HIDDEN;
   previousState = STATE_HIDDEN;
   lastEvent = EVENT_NONE;
   show();
}

/***********************************************************************
 *                              Destructor                             *
 ***********************************************************************/
GuiMain::~GuiMain()
{
   int i;
   for(i=0; i < TOTAL_BUTTONS; i++)
   {
      delete(buttons[i]);
   }
   delete forceBar;
   delete forceDir;
}

/***********************************************************************
 *                              setOnlineGame                          *
 ***********************************************************************/
void GuiMain::setOnlineGame()
{
   onlineGame = true;
}

/***********************************************************************
 *                              setLocalGame                           *
 ***********************************************************************/
void GuiMain::setLocalGame()
{
   onlineGame = false;
}

/***********************************************************************
 *                              setPositions                           *
 ***********************************************************************/
void GuiMain::setPositions()
{
   buttons[BUTTON_SHOOT]->setTargetPosition(2, 
         140*Goblin::ScreenInfo::getGuiScale());
   buttons[BUTTON_PAUSE]->setTargetPosition(2, 
         70*Goblin::ScreenInfo::getGuiScale());
   if(!onlineGame)
   {
      buttons[BUTTON_REPLAY]->setTargetPosition(2,
            210 * Goblin::ScreenInfo::getGuiScale());
   }
}

/***********************************************************************
 *                              setPositions                           *
 ***********************************************************************/
void GuiMain::setHidePositions()
{
   int hidePos = -80 * Goblin::ScreenInfo::getGuiScale();
   buttons[BUTTON_SHOOT]->setTargetPosition(hidePos, hidePos);
   buttons[BUTTON_REPLAY]->setTargetPosition(hidePos, hidePos);
   buttons[BUTTON_PAUSE]->setTargetPosition(hidePos, hidePos);
}

/***********************************************************************
 *                                update                               *
 ***********************************************************************/
void GuiMain::update()
{
   int i;

   /* Update all button states */
   for(i = 0; i < TOTAL_BUTTONS; i++)
   {
      buttons[i]->update();
   }
}

/***********************************************************************
 *                                update                               *
 ***********************************************************************/
void GuiMain::setCurrentForce(float curForce, float maxForce,
                              float dX, float dZ, Ogre::Degree angle,
                              float cX, float cZ, float r)
{
   /* Define the bar representing the force value */
   float perc = (curForce / (float)maxForce);
   forceBar->setPercentual(perc);
   
   if((dX != 0.0f) && (dZ != 0.0f))
   {
      /* Define the model representing force orientation */
      forceDir->setOrientationNow(-90.0f, 0.0f, angle.valueDegrees());
      forceDir->setPositionNow(Ogre::Vector3(cX+(r+0.25f)*dX, 0.15f,
                                             cZ+(r+0.25f)*dZ));
      forceDir->show();
   }
}

/***********************************************************************
 *                                update                               *
 ***********************************************************************/
void GuiMain::hideForceBar()
{
   forceBar->hide();
   forceDir->hide();
}

/***********************************************************************
 *                              verifyEvents                           *
 ***********************************************************************/
int GuiMain::verifyEvents(int mouseX, int mouseY, bool leftButtonPressed)
{
   int i, event;

   /* Verify All Buttons */
   for(i = 0; i < TOTAL_BUTTONS; i++)
   {
      event = buttons[i]->verifyEvents(mouseX,mouseY,leftButtonPressed);
      if(event == IBUTTON_EVENT_PRESSED)
      {
         lastEvent = i;
         return i;
      }
      else if(event == IBUTTON_EVENT_ON_PRESS)
      {
         return EVENT_ON_PRESS;
      }
   }
   
   lastEvent = EVENT_NONE;
   return EVENT_NONE;
}

/***********************************************************************
 *                                 hide                                *
 ***********************************************************************/
void GuiMain::hide()
{
   hideAcceptButton();
   setHidePositions();
   forceBar->hide();
   active = false;
   previousState = curState;
   curState = STATE_HIDDEN;
}

/***********************************************************************
 *                                 show                                *
 ***********************************************************************/
void GuiMain::show()
{
   setPositions();
   active = true;
   previousState = curState;
   curState = STATE_VISIBLE;
}

/***********************************************************************
 *                            restoreState                             *
 ***********************************************************************/
void GuiMain::restoreState()
{
   if(previousState == STATE_VISIBLE)
   {
      show();
   }
   else if(previousState == STATE_ACCEPT_BUTTON)
   {
      showAcceptButton();
   }
   else if(previousState == STATE_HIDDEN)
   {
      hide();
   }
}

/***********************************************************************
 *                           hideAcceptButton                          *
 ***********************************************************************/
void GuiMain::hideAcceptButton()
{
   int hidePos = -80 * Goblin::ScreenInfo::getGuiScale();
   buttons[BUTTON_ACCEPT]->setTargetPosition(hidePos, hidePos);
   setPositions();
}

/***********************************************************************
 *                           showAcceptButton                          *
 ***********************************************************************/
void GuiMain::showAcceptButton()
{
   buttons[BUTTON_ACCEPT]->setTargetPosition(
         80 * Goblin::ScreenInfo::getGuiScale(),
         80 * Goblin::ScreenInfo::getGuiScale());
   setHidePositions();
   previousState = curState;
   curState = STATE_ACCEPT_BUTTON;
}

/***********************************************************************
 *                             isActive                                *
 ***********************************************************************/
bool GuiMain::isActive()
{
   return(active);
}

/***********************************************************************
 *                            getLastEvent                             *
 ***********************************************************************/
int GuiMain::getLastEvent()
{
   return(lastEvent);
}

