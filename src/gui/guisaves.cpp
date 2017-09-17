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

#include "guisaves.h"
#include "../soundfiles.h"
#include "../engine/rules.h"
#include "../engine/team.h"

#include <kobold/userinfo.h>
#include <kobold/defparser.h>
#include <kobold/i18n.h>
#include <OGRE/OgreLogManager.h>

#include <ctime>

#include <iostream>

using namespace BtSoccer;

#define GUISAVES_DEFAULT_UPDATE 20

/***********************************************************************
 *                             Constructor                             *
 ***********************************************************************/
GuiSaves::GuiSaves(Ogre::String listFile, Ogre::String sufix)
{
   int curSlot;
   char buf[16];
   Ogre::String slotImage;
   Ogre::String slotText;

   curListFile = listFile;
   curSufix = sufix;

   /* Create the overlay */
   ogreOverlay=Ogre::OverlayManager::getSingletonPtr()->create("saveSlotsOvl");
   ogreOverlay->setZOrder(641);
   ogreOverlay->hide();

   /* Try to load current slot files */
   loadCurrentSlotFiles();

   int fontSize = 16;

   /* Create each slot button and texts */
   for(curSlot=0; (curSlot < SAVE_SLOTS); curSlot++)
   {
      if(curSlotFile[curSlot].empty())
      {
         slotImage = "saves/empty.png";
         /* No slot texts */
         slotDate[curSlot] = NULL;
         slotTime[curSlot] = NULL;
         slotText = Kobold::i18n::translate("Empty");
      }
      else
      {
         slotImage = "saves/filled.png";
         slotText = curSlotInfo[curSlot];
      }
      slotButton[curSlot] = new Goblin::Ibutton(ogreOverlay, slotImage,
            "gui", slotText, "infoFontOut", fontSize);
      slotButton[curSlot]->setPosition(-200*Goblin::ScreenInfo::getGuiScale(), 
            -200*Goblin::ScreenInfo::getGuiScale());
      slotButton[curSlot]->setDimensions(128*Goblin::ScreenInfo::getGuiScale(), 
            128*Goblin::ScreenInfo::getGuiScale());
      slotButton[curSlot]->setMouseOverDimensions(
            130*Goblin::ScreenInfo::getGuiScale(),
            130*Goblin::ScreenInfo::getGuiScale());
      slotButton[curSlot]->setPressedSound(BTSOCCER_SOUND_GUI_CLICK);
      slotButton[curSlot]->hide();

      /* Create the texts, if needed */
      if(!curSlotFile[curSlot].empty())
      {
         /* Must create slot texts */
         sprintf(buf, "%d", curSlot);
         slotDate[curSlot] = new Goblin::TextBox(
               (-200+64)*Goblin::ScreenInfo::getGuiScale(), 
               (-200+64)*Goblin::ScreenInfo::getGuiScale(), 
               128*Goblin::ScreenInfo::getGuiScale(), 
               32*Goblin::ScreenInfo::getGuiScale(),
               curSlotDate[curSlot],
               Ogre::String("SlotDate")+Ogre::String(buf), ogreOverlay,
               "infoFont", fontSize);
         slotDate[curSlot]->setColor(0.1f, 0.0f, 0.0f, 1.0f);
         slotDate[curSlot]->setAlignment(Ogre::TextAreaOverlayElement::Center);
         
         slotTime[curSlot] = new Goblin::TextBox(
               (-200+64)*Goblin::ScreenInfo::getGuiScale(), 
               (-200+82)*Goblin::ScreenInfo::getGuiScale(), 
               128*Goblin::ScreenInfo::getGuiScale(), 
               32*Goblin::ScreenInfo::getGuiScale(),
               curSlotTime[curSlot],
               Ogre::String("SlotTime")+Ogre::String(buf), ogreOverlay,
               "infoFont", fontSize);
         slotTime[curSlot]->setColor(0.1f, 0.0f, 0.0f, 1.0f);
         slotTime[curSlot]->setAlignment(Ogre::TextAreaOverlayElement::Center);
      }
   }

   /* Create the cancel button */
   cancelButton = new Goblin::Ibutton(ogreOverlay, "initial/cancel.png",
         "gui", Kobold::i18n::translate("Cancel"), "infoFontOut", fontSize);
   cancelButton->setPosition(-100*Goblin::ScreenInfo::getGuiScale(),
         -100*Goblin::ScreenInfo::getGuiScale());
   cancelButton->setDimensions(64*Goblin::ScreenInfo::getGuiScale(), 
         64*Goblin::ScreenInfo::getGuiScale());
   cancelButton->setMouseOverDimensions(65*Goblin::ScreenInfo::getGuiScale(), 
         65*Goblin::ScreenInfo::getGuiScale());
   cancelButton->setPressedSound(BTSOCCER_SOUND_GUI_CLICK2);
}

/***********************************************************************
 *                              Destructor                             *
 ***********************************************************************/
GuiSaves::~GuiSaves()
{
   int i;
   for(i=0; i<SAVE_SLOTS; i++)
   {
      delete slotButton[i];
      if(slotDate[i])
      {
         delete slotDate[i];
      }
      if(slotTime[i])
      {
         delete slotTime[i];
      }
   }
   delete cancelButton;
   /* Bye overlay */
   Ogre::OverlayManager::getSingletonPtr()->destroy(ogreOverlay);
}

/***********************************************************************
 *                       loadCurrentSlotFiles                          *
 ***********************************************************************/
void GuiSaves::loadCurrentSlotFiles()
{
   Kobold::DefParser def;
   Ogre::String key, value;
   int slotNumber=0;

   /* Try to load the save slots */
   if(!def.load(Kobold::UserInfo::getSaveDirectory()+curListFile, true))
   {
      /* Couldn't load save list. Thus all slots are empty! */
      return;
   }

   while(def.getNextTuple(key, value))
   {
      if(key == "date")
      {
         /* Get date for current slot */
         curSlotDate[slotNumber] = value;
      }
      else if(key == "time")
      {
         /* Get time for current slot */
         curSlotTime[slotNumber] = value;
      }
      else if(key == "info")
      {
         /* Get info for current slot */
         curSlotInfo[slotNumber] = value;
      }
      else
      {
         /* Get the slot number */
         sscanf(key.c_str(), "%d", &slotNumber);
         if( (slotNumber >= 0) && (slotNumber < SAVE_SLOTS) )
         {
            curSlotFile[slotNumber] = value;
         }
         else
         {
            Ogre::LogManager::getSingleton().stream(Ogre::LML_CRITICAL)
               << "ERROR: Unknown save slot: '" << value << "'";
            slotNumber = 0;
         }
      }
   }
}

/***********************************************************************
 *                       saveCurrentSlotFiles                          *
 ***********************************************************************/
void GuiSaves::saveCurrentSlotFiles()
{
   int i;
   std::ofstream file;
   Ogre::String fName = Kobold::UserInfo::getSaveDirectory()+curListFile;

   /* Open/create the file */
   file.open(fName.c_str(), std::ios::out | std::ios::binary);
   if(!file)
   {
      Ogre::LogManager::getSingleton().logMessage(
            "ERROR: couldn't write save-slots list!", Ogre::LML_CRITICAL);
      return;
   }

   /* Save each defined slot */
   for(i=0; i < SAVE_SLOTS; i++)
   {
      if(!curSlotFile[i].empty())
      {
         file << i << " = " << curSlotFile[i] << std::endl;
         file << "date = " << curSlotDate[i] << std::endl;
         file << "time = " << curSlotTime[i] << std::endl;
         file << "info = " << curSlotInfo[i] << std::endl;
      }
   }

   file.close();
}

/***********************************************************************
 *                                hide                                 *
 ***********************************************************************/
void GuiSaves::hide()
{
   int i;

   for(i=0; i < SAVE_SLOTS; i++)
   {
      slotButton[i]->hide();
   }
   cancelButton->hide();
   ogreOverlay->hide();
}

/***********************************************************************
 *                                show                                 *
 ***********************************************************************/
void GuiSaves::show(bool save)
{
   int i, x, y;

   i = 0;
   for(y=0; ((y < 4) && (i < SAVE_SLOTS)); y++)
   {
      for(x=0; ((x < 4) && (i < SAVE_SLOTS)); x++)
      {
         slotButton[i]->show();
         slotButton[i]->setTargetPosition((x*150+223) * 
               Goblin::ScreenInfo::getGuiScale(),
               (y*160*Goblin::ScreenInfo::getGuiScale()) + 
                  Goblin::ScreenInfo::getQuarterWindowHeight()-40, 
                  GUISAVES_DEFAULT_UPDATE + 10);
         if(slotDate[i])
         {
            slotDate[i]->setTargetPosition(
                  (x*150+223+64)*Goblin::ScreenInfo::getGuiScale(), 
                  (y*160*Goblin::ScreenInfo::getGuiScale()) + 
                     Goblin::ScreenInfo::getQuarterWindowHeight()+24, 
                     GUISAVES_DEFAULT_UPDATE);
         }
         if(slotTime[i])
         {
            slotTime[i]->setTargetPosition(
                  (x*150+223+64)*Goblin::ScreenInfo::getGuiScale(), 
                  (y*160*Goblin::ScreenInfo::getGuiScale()) + 
                     Goblin::ScreenInfo::getQuarterWindowHeight()+42, 
                     GUISAVES_DEFAULT_UPDATE);
         }
         i++;
      }
   }
   cancelButton->show();
   cancelButton->setTargetPosition(50*Goblin::ScreenInfo::getGuiScale(), 
         50*Goblin::ScreenInfo::getGuiScale(), GUISAVES_DEFAULT_UPDATE + 10);
   ogreOverlay->show();
   
   /* Clear and set things */
   curSelectedFile = "";
   returnStatus = ACTION_NONE;
   saving = save;
}

/***********************************************************************
 *                            verifyEvents                             *
 ***********************************************************************/
int GuiSaves::verifyEvents(int mouseX, int mouseY, bool leftButtonPressed)
{
   int i;
   int selectedSlot = -1;
   bool isUpdating = false;
   bool buttonPressed = false;

   for(i=0; i<SAVE_SLOTS; i++)
   {
      slotButton[i]->update();
      isUpdating |= slotButton[i]->isUpdating();
      if(slotDate[i])
      {
         slotDate[i]->update();
         isUpdating |= slotDate[i]->isUpdating();
      }
      if(slotTime[i])
      {
         slotTime[i]->update();
         isUpdating |= slotTime[i]->isUpdating();
      }
   }
   cancelButton->update();
   isUpdating |= cancelButton->isUpdating();
   
   for(i=0; i < SAVE_SLOTS; i++)
   {
      if(slotButton[i]->verifyEvents(mouseX, mouseY, leftButtonPressed) ==
            IBUTTON_EVENT_PRESSED)
      {
         /* TODO: Show file information! */

         /* Set file to load or save */
         curSelectedFile = curSlotFile[i];
         selectedSlot = i;
         
         if(saving)
         {
            /* Set date to current */
            time_t now = time(0);
            struct tm tstruct = *localtime(&now);
            char buf[32];
            strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
            curSlotDate[i] = buf;
            strftime(buf, sizeof(buf), "%X", &tstruct);
            curSlotTime[i] = buf;

            /* Set Info of current game
             * FIXME: when saving CUP! */
            curSlotInfo[i] = Rules::getTeamA()->getName() +
               Ogre::String(" x ") + Rules::getTeamB()->getName();

            /* Check if filename is set */
            if(curSelectedFile.empty())
            {
               /* Is saving, must set filename for empty files */
               sprintf(buf, "%d", i);
               curSelectedFile = Ogre::String("slot") + Ogre::String(buf) +
                  curSufix;
               curSlotFile[i] = curSelectedFile;
            }
         }

         returnStatus = (saving)?ACTION_SAVE:ACTION_LOAD;
         buttonPressed = !curSelectedFile.empty();
      }
   }
   if(cancelButton->verifyEvents(mouseX, mouseY, leftButtonPressed) ==
            IBUTTON_EVENT_PRESSED)
   {
      returnStatus = ACTION_CANCEL;
      selectedSlot = -1;
      buttonPressed = true;
   }

   /* Set target, if needed */
   int pX, pY;
   if(buttonPressed)
   {
      for(i=0; i<SAVE_SLOTS; i++)
      {
         /* Define position */
         if(i != selectedSlot)
         {
            pX = -200*Goblin::ScreenInfo::getGuiScale();
            pY = -200*Goblin::ScreenInfo::getGuiScale();
         }
         else
         {
            pX = slotButton[i]->getPosX()*Goblin::ScreenInfo::getGuiScale();
            pY = -200*Goblin::ScreenInfo::getGuiScale();
         }
         
         /* Set as target */
         slotButton[i]->setTargetPosition(pX, pY, GUISAVES_DEFAULT_UPDATE + 10);
         if(slotDate[i])
         {
            slotDate[i]->setTargetPosition(pX + 
                  64*Goblin::ScreenInfo::getGuiScale(), 
                  pY+64*Goblin::ScreenInfo::getGuiScale(), 
                  GUISAVES_DEFAULT_UPDATE);
         }
         if(slotTime[i])
         {
            slotTime[i]->setTargetPosition(pX + 
                  64*Goblin::ScreenInfo::getGuiScale(), 
                  pY+82*Goblin::ScreenInfo::getGuiScale(), 
                  GUISAVES_DEFAULT_UPDATE);
         }
      }
      cancelButton->setTargetPosition(-100*Goblin::ScreenInfo::getGuiScale(), 
            -100*Goblin::ScreenInfo::getGuiScale(), 
            GUISAVES_DEFAULT_UPDATE + 10);
   }

   /* Verify if done */
   if( (!isUpdating) && (!buttonPressed) )
   {
      if( (returnStatus == ACTION_CANCEL) || (!curSelectedFile.empty()) )
      {
         int ret = returnStatus;
         returnStatus = ACTION_NONE;
         hide();
         return(ret);
      }
   }

   return ACTION_NONE;
}

/***********************************************************************
 *                           isSaveMode                                *
 ***********************************************************************/
bool GuiSaves::isSaveMode()
{
   return saving;
}

/***********************************************************************
 *                       getSelectedFileName                           *
 ***********************************************************************/
Ogre::String GuiSaves::getSelectedFileName()
{
   return curSelectedFile;
}

