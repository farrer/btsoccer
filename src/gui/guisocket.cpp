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


#include "guisocket.h"
#include "guimessage.h"
#include "../soundfiles.h"
#include <kobold/ogre3d/i18n.h>
#include <kobold/userinfo.h>

using namespace BtSoccer;

/***********************************************************************
 *                             Constructor                             *
 ***********************************************************************/
GuiSocket::GuiSocket(bool server)
{
   isServer = server;
   incValue = 1;
   delayBeforeRepeat = 0;
   returnStatus = RETURN_NOTHING;
   
   /* Create the overlay */
   ogreOverlay = Ogre::OverlayManager::getSingletonPtr()->create("socketOvl");
   ogreOverlay->setZOrder(640);
   ogreOverlay->show();

   int fontSize = 16;
   int buttonSize = 64*Goblin::ScreenInfo::getGuiScale();
   int overSize = 65*Goblin::ScreenInfo::getGuiScale();
   int hidePos = -100*Goblin::ScreenInfo::getGuiScale();
   
   /* Create ok buttom */
   buttonOk = new Goblin::Ibutton(ogreOverlay, "initial/accept.png",
         "gui", Kobold::i18n::translate(((isServer)?"Create":"Connect")), 
         "infoFontOut", fontSize);
   buttonOk->setPosition(hidePos, hidePos);
   buttonOk->setDimensions(buttonSize, buttonSize);
   buttonOk->setMouseOverDimensions(overSize, overSize);
   buttonOk->setPressedSound(BTSOCCER_SOUND_GUI_CLICK);
   
   /* Cancel buttom */
   buttonCancel = new Goblin::Ibutton(ogreOverlay, "initial/cancel.png",
         "gui", Kobold::i18n::translate("Cancel"), "infoFontOut", fontSize);
   buttonCancel->setPosition(hidePos, hidePos);
   buttonCancel->setDimensions(buttonSize, buttonSize);
   buttonCancel->setMouseOverDimensions(overSize, overSize);
   buttonCancel->setPressedSound(BTSOCCER_SOUND_GUI_CLICK2);
   
   ipPart[0] = 192;
   ipPart[1] = 168;
   ipPart[2] = 0;
   ipPart[3] = 1;
   port = 7089;
   
   /* IP and port inc and dec buttons */
   for(int i=0; i <5; i++)
   {
      buttonInc[i] = new Goblin::Ibutton(ogreOverlay, "initial/up.png", "gui");
      buttonInc[i]->setPosition(hidePos, hidePos);
      buttonInc[i]->setDimensions(buttonSize, buttonSize);
      buttonInc[i]->setMouseOverDimensions(overSize, overSize);
      
      buttonDec[i] = new Goblin::Ibutton(ogreOverlay, "initial/down.png", 
            "gui");
      buttonDec[i]->setPosition(hidePos, hidePos);
      buttonDec[i]->setDimensions(buttonSize, buttonSize);
      buttonDec[i]->setMouseOverDimensions(overSize, overSize);
      
      textValues[i] = new Goblin::TextBox(-428, 8, 
            256*Goblin::ScreenInfo::getGuiScale(), 
            32*Goblin::ScreenInfo::getGuiScale(), "",
            "GuiSocketText", ogreOverlay, "numberFont", 2*fontSize);
      textValues[i]->setColor(1.0f, 1.0f, 1.0f, 1.0f);
      textValues[i]->setAlignment(Ogre::TextAreaOverlayElement::Center);
      updateTextValue(i, ((i < 4)?ipPart[i]:port));
   }
   
   /* Help Image */
   helpImage = new Goblin::Image(ogreOverlay, "initial/serverhelp.png", "gui");
   helpImage->setPosition(-300*Goblin::ScreenInfo::getGuiScale(), 
         Goblin::ScreenInfo::getHalfWindowHeight() - 
         256*Goblin::ScreenInfo::getGuiScale());
   helpImage->setDimensions(256*Goblin::ScreenInfo::getGuiScale(), 
         256*Goblin::ScreenInfo::getGuiScale());
   helpText = new Goblin::TextBox(0, 0, 256*Goblin::ScreenInfo::getGuiScale(), 
         256*Goblin::ScreenInfo::getGuiScale(),
         Kobold::i18n::translate("serverClientHelp"), "GuiSocketHelp", 
         ogreOverlay, "infoFont", 14);
   helpText->setColor(0.0f, 0.0f, 0.0f, 1.0f);
   helpText->setAlignment(Ogre::TextAreaOverlayElement::Left);
   helpImage->addChild(helpText, 10*Goblin::ScreenInfo::getGuiScale(), 
         10*Goblin::ScreenInfo::getGuiScale());
   
   show();
   
}

/***********************************************************************
 *                              Destructor                             *
 ***********************************************************************/
GuiSocket::~GuiSocket()
{
   for(int i=0; i <= 4; i++)
   {
      delete buttonInc[i];
      delete buttonDec[i];
      delete textValues[i];
   }
   delete buttonOk;
   delete buttonCancel;
   delete helpImage;
   delete helpText;
   
   Ogre::OverlayManager::getSingletonPtr()->destroy(ogreOverlay);
}

/***********************************************************************
 *                                getValue                             *
 ***********************************************************************/
Ogre::String GuiSocket::getValue(int value)
{
   char buf[8];
   sprintf(&buf[0], "%d", value);
   return buf;
}

/***********************************************************************
 *                            updateTextValue                          *
 ***********************************************************************/
void GuiSocket::updateTextValue(int i, int value)
{
   if( (!isServer) || (i > 3))
   {
      textValues[i]->setText(getValue(value));
   }
   else
   {
      textValues[i]->setText(Kobold::UserInfo::getIpAddress());
   }
}

/***********************************************************************
 *                                 show                                *
 ***********************************************************************/
void GuiSocket::show()
{
   buttonOk->setTargetPosition(409*Goblin::ScreenInfo::getGuiScale(), 
         430*Goblin::ScreenInfo::getGuiScale());
   buttonCancel->setTargetPosition(585*Goblin::ScreenInfo::getGuiScale(), 
         430*Goblin::ScreenInfo::getGuiScale());
   
   if(!isServer)
   {
      for(int i=0; i < 4; i++)
      {
         textValues[i]->setTargetPosition((390+i*95)*
               Goblin::ScreenInfo::getGuiScale(), 
               330*Goblin::ScreenInfo::getGuiScale());
         buttonInc[i]->setTargetPosition((355+i*95)*
               Goblin::ScreenInfo::getGuiScale(), 
               260*Goblin::ScreenInfo::getGuiScale());
         buttonDec[i]->setTargetPosition((355+i*95)* 
               Goblin::ScreenInfo::getGuiScale(), 
               360*Goblin::ScreenInfo::getGuiScale());
      }
   }
   else
   {
      /* Just show the first ip part that will contain it all */
      textValues[0]->setTargetPosition(530*Goblin::ScreenInfo::getGuiScale(), 
            330*Goblin::ScreenInfo::getGuiScale());
   }
   textValues[4]->setTargetPosition(525*Goblin::ScreenInfo::getGuiScale(), 
         170*Goblin::ScreenInfo::getGuiScale());
   buttonInc[4]->setTargetPosition(490*Goblin::ScreenInfo::getGuiScale(), 
         100*Goblin::ScreenInfo::getGuiScale());
   buttonDec[4]->setTargetPosition(490*Goblin::ScreenInfo::getGuiScale(), 
         200*Goblin::ScreenInfo::getGuiScale());
   helpImage->setTargetPosition(0, 
         Goblin::ScreenInfo::getHalfWindowHeight() - 
         256*Goblin::ScreenInfo::getGuiScale());
}

/***********************************************************************
 *                                 hide                                *
 ***********************************************************************/
void GuiSocket::hide(bool all)
{
   int hidePos = -200*Goblin::ScreenInfo::getGuiScale();
   buttonOk->setTargetPosition(hidePos, hidePos, 15);
   if(all)
   {
      buttonCancel->setTargetPosition(hidePos, hidePos, 15);
   }
   for(int i=0; i <=4; i++)
   {
      buttonDec[i]->setTargetPosition(hidePos, hidePos, 15);
      buttonInc[i]->setTargetPosition(hidePos, hidePos, 15);
      textValues[i]->setTargetPosition(hidePos, hidePos, 15);
   }
   helpImage->setTargetPosition(-300*Goblin::ScreenInfo::getGuiScale(), 
         Goblin::ScreenInfo::getHalfWindowHeight() - 
         256*Goblin::ScreenInfo::getGuiScale());
}

/***********************************************************************
 *                               isStable                              *
 ***********************************************************************/
bool GuiSocket::isStable()
{
   if(buttonOk->isUpdating())
   {
      return false;
   }
   if(buttonCancel->isUpdating())
   {
      return false;
   }
   for(int i=0; i<=4; i++)
   {
      if(buttonDec[i]->isUpdating())
      {
         return false;
      }
      if(buttonInc[i]->isUpdating())
      {
         return false;
      }
      if(textValues[i]->isUpdating())
      {
         return false;
      }
   }
   if(helpImage->isUpdating())
   {
      return false;
   }
   
   return true;
}

/***********************************************************************
 *                             verifyEvents                            *
 ***********************************************************************/
int GuiSocket::verifyEvents(int mouseX, int mouseY, bool leftButtonPressed)
{
   int ret;
   
   helpImage->update();
   
   for(int i=0; i <= 4; i++)
   {
      /* Update text position */
      textValues[i]->update();
      ret = buttonInc[i]->verifyEvents(mouseX, mouseY, leftButtonPressed);
      /* Check increment buttons */
      if( (ret == IBUTTON_EVENT_PRESSED) || (ret == IBUTTON_EVENT_ON_PRESS) )
      {
         if(ret == IBUTTON_EVENT_ON_PRESS)
         {
            if(delayBeforeRepeat < 10)
            {
               /* ONly allow to inc on on_press after some time. */
               delayBeforeRepeat++;
               return RETURN_OTHER;
            }
         }
         else
         {
            incValue = 1;
            delayBeforeRepeat = 0;
         }
         if(i<4)
         {
            ipPart[i] = (ipPart[i] + 1) % 256;
         }
         else
         {
            port = (port + incValue) % 10000;
            if( (ret == IBUTTON_EVENT_ON_PRESS) && (incValue < 100) )
            {
               incValue += 1;
            }
         }
         updateTextValue(i, ((i < 4)?ipPart[i]:port));
         return RETURN_OTHER;
      }
      /* Check decrement buttons */
      ret = buttonDec[i]->verifyEvents(mouseX, mouseY, leftButtonPressed);
      if( (ret == IBUTTON_EVENT_PRESSED) || (ret == IBUTTON_EVENT_ON_PRESS) )
      {
         if(ret == IBUTTON_EVENT_ON_PRESS)
         {
            if(delayBeforeRepeat < 10)
            {
               /* Only allow to inc on on_press after some time. */
               delayBeforeRepeat++;
               return RETURN_OTHER;
            }
         }
         else
         {
            delayBeforeRepeat = 0;
            incValue = 1;
         }
         if(i<4)
         {
            ipPart[i] -= 1;
            if(ipPart[i] < 0)
            {
               ipPart[i] = 255;
            }
         }
         else
         {
            port -= incValue;
            if(port < 0)
            {
               port = 9999;
            }
            if( (ret == IBUTTON_EVENT_ON_PRESS) && (incValue < 100) )
            {
               incValue += 1;
            }
         }
         updateTextValue(i, ((i < 4)?ipPart[i]:port));
         return RETURN_OTHER;
      }
   }
   /* Check ok button */
   if(buttonOk->verifyEvents(mouseX, mouseY, leftButtonPressed) ==
      IBUTTON_EVENT_PRESSED)
   {
      hide(!isServer);
      if(isServer)
      {
         returnStatus = RETURN_CREATE;
         GuiMessage::set("Waiting for connections");
      }
      else
      {
         GuiMessage::set("Trying to connect");
         returnStatus = RETURN_CONNECT;
      }
      return RETURN_OTHER;
   }
   /* Check cancel button */
   else if(buttonCancel->verifyEvents(mouseX, mouseY, leftButtonPressed) ==
      IBUTTON_EVENT_PRESSED)
   {
      hide(true);
      returnStatus = RETURN_CANCEL;
      return RETURN_OTHER;
   }
   
   if(returnStatus != RETURN_NOTHING)
   {
      /* Wait until stable */
      if(isStable())
      {
         int ret = returnStatus;
         returnStatus = RETURN_NOTHING;
         return ret;
      }
   }
   
   incValue = 1;
   return RETURN_OTHER;
}


/***********************************************************************
 *                             verifyEvents                            *
 ***********************************************************************/
Ogre::String GuiSocket::getAddress()
{
   return getValue(ipPart[0]) + "." + getValue(ipPart[1]) + "." +
          getValue(ipPart[2]) + "." + getValue(ipPart[3]);
}

/***********************************************************************
 *                             verifyEvents                            *
 ***********************************************************************/
unsigned short int GuiSocket::getPort()
{
   return port;
}
