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

#include "guimessage.h"
#include <kobold/i18n.h>

using namespace BtSoccer;

/*********************************************************************
 *                               init                                *
 *********************************************************************/
void GuiMessage::init(Ogre::String imageFile)
{
   /* Create the ogre overlay */
   overlay = Ogre::OverlayManager::getSingletonPtr()->create("GuiMessageOvl");
   overlay->setZOrder(640);
   overlay->show();

   /* Create the back image */
   image = new Goblin::Image(overlay, imageFile, "gui");
   image->setPosition(-300*Goblin::ScreenInfo::getGuiScale(), 0);
   image->setDimensions(256*Goblin::ScreenInfo::getGuiScale(),
                        32*Goblin::ScreenInfo::getGuiScale());

   /* Create the text */
   text  = new Goblin::TextBox(-428, 8, 256*Goblin::ScreenInfo::getGuiScale(), 
         32*Goblin::ScreenInfo::getGuiScale(), "",  "GuiMessageText", 
         overlay, "infoFont", 18);
   text->setColor(0.1f, 0.0f, 0.0f, 1.0f);
   text->setAlignment(Ogre::TextAreaOverlayElement::Center);
   image->addChild(text, 128*Goblin::ScreenInfo::getGuiScale(), 
         8*Goblin::ScreenInfo::getGuiScale());
}

/*********************************************************************
 *                            finish                                 *
 *********************************************************************/
void GuiMessage::finish()
{
   if(image)
   {
      delete(image);
   }
   if(text)
   {
      delete(text);
   }
   if(overlay)
   {
      Ogre::OverlayManager::getSingletonPtr()->destroy(overlay);
   }
}

/*********************************************************************
 *                               set                                 *
 *********************************************************************/
void GuiMessage::set(Ogre::String msg)
{
   if(text != NULL)
   {
      text->setText(Kobold::i18n::translate(msg));
      image->setTargetPosition(0,0);
   
      display = true;
      dispTimer.reset();
   }
}

/*********************************************************************
 *                              update                               *
 *********************************************************************/
void GuiMessage::update()
{
   image->update();

   if( (display) && (dispTimer.getMilliseconds() >= 2000) )
   {
      clear();
   }
}

/*********************************************************************
 *                               clear                               *
 *********************************************************************/
void GuiMessage::clear()
{
   if(display)
   {
      image->setTargetPosition(-300*Goblin::ScreenInfo::getGuiScale(),0);
      display = false;
   }
}

/*********************************************************************
 *                         static members                            *
 *********************************************************************/
Ogre::Overlay* GuiMessage::overlay=NULL;
Goblin::Image* GuiMessage::image=NULL;
Goblin::TextBox* GuiMessage::text=NULL;
Kobold::Timer GuiMessage::dispTimer;
bool GuiMessage::display=false;
