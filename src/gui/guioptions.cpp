
#include "guioptions.h"
#include "../engine/options.h"
#include <kobold/ogre3d/i18n.h>

namespace BtSoccer
{
   
Ogre::String optTimes[] = {"3 min.", "5 min.", "10 min."};
Ogre::String optFields[] = {"Child", "Amateur", "Professional"};
Ogre::String optCameras[] = {"Fixed", "Normal", "Follow Ball"};
   
Ogre::String fileTimeSel[] = {"options/time_sel.png",
   "options/time_sel_2.png", "options/time_sel_3.png"};
Ogre::String fileTimeUnSel[] = {"options/time_unsel.png",
   "options/time_unsel_2.png", "options/time_unsel_3.png"};
   
Ogre::String fileFieldSel[] = {"options/field_small_sel.png",
   "options/field_med_sel.png", "options/field_sel.png"};
   
Ogre::String fileFieldUnSel[] = {"options/field_small_unsel.png",
   "options/field_med_unsel.png", "options/field_unsel.png"};

#define GUI_OPTIONS_DEFAULT_UPDATE  20

/***********************************************************************
 *                             Constructor                             *
 ***********************************************************************/
GuiOptions::GuiOptions()
{
   /* Create the overlay */
   ogreOverlay = Ogre::OverlayManager::getSingletonPtr()->create(
         "Options::Ovl");
   ogreOverlay->setZOrder(640);
   ogreOverlay->show();
   
   /* Get current Options:: */
   getCurrentOptions();

   int fontSize = 16;
   int buttonSize = 64 * Goblin::ScreenInfo::getGuiScale();
   int overSize = 65 * Goblin::ScreenInfo::getGuiScale();
   int hidePos = -100 * Goblin::ScreenInfo::getGuiScale();
   
   /* Create the buttons */
   for(int i=0; i < 3; i++)
   {
      timeNotSelected[i] = new Goblin::Ibutton(ogreOverlay,
                                 fileTimeUnSel[i], "gui", 
                                 Kobold::i18n::translate(optTimes[i]),
                                 "infoFontOut", fontSize);
      timeNotSelected[i]->setDimensions(buttonSize, buttonSize);
      timeNotSelected[i]->setMouseOverDimensions(buttonSize, buttonSize);
      timeNotSelected[i]->setPosition(hidePos, hidePos);
      
      timeSelected[i] = new Goblin::Ibutton(ogreOverlay,
                              fileTimeSel[i], "gui",
                              Kobold::i18n::translate(optTimes[i]),
                              "infoFontOut", fontSize);
      timeSelected[i]->setDimensions(buttonSize, buttonSize);
      timeSelected[i]->setMouseOverDimensions(overSize, overSize);
      timeSelected[i]->setPosition(hidePos, hidePos);
      
      fieldNotSelected[i] = new Goblin::Ibutton(ogreOverlay,
                              fileFieldUnSel[i], "gui",
                              Kobold::i18n::translate(optFields[i]),
                              "infoFontOut", fontSize);
      fieldNotSelected[i]->setDimensions(buttonSize, buttonSize);
      fieldNotSelected[i]->setMouseOverDimensions(buttonSize, buttonSize);
      fieldNotSelected[i]->setPosition(hidePos, hidePos);
      
      fieldSelected[i] = new Goblin::Ibutton(ogreOverlay,
                              fileFieldSel[i], "gui",
                              Kobold::i18n::translate(optFields[i]),
                              "infoFontOut", fontSize);
      fieldSelected[i]->setDimensions(buttonSize, buttonSize);
      fieldSelected[i]->setMouseOverDimensions(overSize, overSize);
      fieldSelected[i]->setPosition(hidePos, hidePos);
      
      cameraNotSelected[i] = new Goblin::Ibutton(ogreOverlay,
                                  "options/camera_unsel.png", "gui",
                                  Kobold::i18n::translate(optCameras[i]),
                                  "infoFontOut", fontSize);
      cameraNotSelected[i]->setDimensions(buttonSize, buttonSize);
      cameraNotSelected[i]->setMouseOverDimensions(buttonSize, buttonSize);
      cameraNotSelected[i]->setPosition(hidePos, hidePos);
      
      cameraSelected[i] = new Goblin::Ibutton(ogreOverlay,
                               "options/camera_sel.png", "gui",
                               Kobold::i18n::translate(optCameras[i]),
                               "infoFontOut", fontSize);
      cameraSelected[i]->setDimensions(buttonSize, buttonSize);
      cameraSelected[i]->setMouseOverDimensions(overSize, overSize);
      cameraSelected[i]->setPosition(hidePos, hidePos);
   }
}
   
/***********************************************************************
 *                              Destructor                             *
 ***********************************************************************/
GuiOptions::~GuiOptions()
{
   for(int i=0; i < 3; i++)
   {
      delete fieldNotSelected[i];
      delete fieldSelected[i];
      delete timeNotSelected[i];
      delete timeSelected[i];
      delete cameraNotSelected[i];
      delete cameraSelected[i];
   }
   
   Ogre::OverlayManager::getSingletonPtr()->destroy(ogreOverlay);
}

/***********************************************************************
 *                           getCurrentOptions                         *
 ***********************************************************************/
void GuiOptions::getCurrentOptions()
{
   switch(Options::getMinutesPerHalf())
   {
      case 3:
         curTime = 0;
         break;
      case 10:
         curTime = 2;
         break;
      case 5:
      default:
         curTime = 1;
         break;
   }
   curField = Options::getFieldSize();
   curCamera = Options::getCameraType();
}

/***********************************************************************
 *                             verifyEvents                            *
 ***********************************************************************/
bool GuiOptions::verifyEvents(int mouseX, int mouseY, bool leftButtonPressed)
{
   for(int i=0; i < 3; i++)
   {
      /* Check Field buttons */
      if(fieldNotSelected[i]->verifyEvents(
           mouseX, mouseY, leftButtonPressed) == IBUTTON_EVENT_PRESSED)
      {
         /* Show selected */
         fieldSelected[i]->show();
         fieldNotSelected[i]->hide();
         /* Unshow previous */
         fieldSelected[curField]->hide();
         fieldNotSelected[curField]->show();
         /* Change current value */
         curField = i;
      }
      
      /* Check Time buttons */
      if(timeNotSelected[i]->verifyEvents(
            mouseX, mouseY, leftButtonPressed) == IBUTTON_EVENT_PRESSED)
      {
         /* Show selected */
         timeSelected[i]->show();
         timeNotSelected[i]->hide();
         /* Unshow previous */
         timeSelected[curTime]->hide();
         timeNotSelected[curTime]->show();
         /* Change current time */
         curTime = i;
      }
      
      /* Check Camera buttons */
      if(cameraNotSelected[i]->verifyEvents(
            mouseX, mouseY, leftButtonPressed) == IBUTTON_EVENT_PRESSED)
      {
         /* Show selected */
         cameraSelected[i]->show();
         cameraNotSelected[i]->hide();
         /* Unshow previous */
         cameraSelected[curCamera]->hide();
         cameraNotSelected[curCamera]->show();
         /* Change current camera */
         curCamera = i;
      }
   }
   return false;
}
   
/***********************************************************************
 *                                hide                                 *
 ***********************************************************************/
void GuiOptions::hide()
{
   for(int i=0; i < 3; i++)
   {
      fieldNotSelected[i]->hide();
      fieldSelected[i]->hide();
      timeNotSelected[i]->hide();
      timeSelected[i]->hide();
      cameraNotSelected[i]->hide();
      cameraSelected[i]->hide();
   }
}
   
/***********************************************************************
 *                                show                                 *
 ***********************************************************************/
void GuiOptions::show()
{
   for(int i=0; i < 3; i++)
   {
      if(curField == i)
      {
         fieldNotSelected[i]->hide();
         fieldSelected[i]->show();
      }
      else
      {
         fieldNotSelected[i]->show();
         fieldSelected[i]->hide();
      }
      if(curTime == i)
      {
         timeNotSelected[i]->hide();
         timeSelected[i]->show();
      }
      else
      {
         timeNotSelected[i]->show();
         timeSelected[i]->hide();
      }
      if(curCamera == i)
      {
         cameraNotSelected[i]->hide();
         cameraSelected[i]->show();
      }
      else
      {
         cameraNotSelected[i]->show();
         cameraSelected[i]->hide();
      }
   }
   setButtonsDispPos();
}
   
/****************************************************************
 *                          setButtons                          *
 ****************************************************************/
void GuiOptions::setButtonsDispPos()
{
   int midX = Goblin::ScreenInfo::getHalfWindowWidth() - 
      32 * Goblin::ScreenInfo::getGuiScale();
   for(int i=-1; i < 2; i++)
   {
      int posX = i*100*Goblin::ScreenInfo::getGuiScale()+midX;
      fieldNotSelected[i+1]->setTargetPosition(posX, 
            150*Goblin::ScreenInfo::getGuiScale(), GUI_OPTIONS_DEFAULT_UPDATE);
      fieldSelected[i+1]->setTargetPosition(posX,
            150*Goblin::ScreenInfo::getGuiScale(), GUI_OPTIONS_DEFAULT_UPDATE);
      timeNotSelected[i+1]->setTargetPosition(posX, 
            250*Goblin::ScreenInfo::getGuiScale(), GUI_OPTIONS_DEFAULT_UPDATE);
      timeSelected[i+1]->setTargetPosition(posX, 
            250*Goblin::ScreenInfo::getGuiScale(), GUI_OPTIONS_DEFAULT_UPDATE);
      cameraNotSelected[i+1]->setTargetPosition(posX, 
            350*Goblin::ScreenInfo::getGuiScale(), GUI_OPTIONS_DEFAULT_UPDATE);
      cameraSelected[i+1]->setTargetPosition(posX, 
            350*Goblin::ScreenInfo::getGuiScale(), GUI_OPTIONS_DEFAULT_UPDATE);
   }
}
   
/****************************************************************
 *                          setButtons                          *
 ****************************************************************/
void GuiOptions::setButtonsHiddenPos()
{
   int hidePos = -100 * Goblin::ScreenInfo::getGuiScale();
   for(int i=0; i < 3; i++)
   {
      fieldNotSelected[i]->setTargetPosition(hidePos, hidePos, 
            GUI_OPTIONS_DEFAULT_UPDATE);
      fieldSelected[i]->setTargetPosition(hidePos, hidePos,
            GUI_OPTIONS_DEFAULT_UPDATE);
      timeNotSelected[i]->setTargetPosition(hidePos, hidePos, 
            GUI_OPTIONS_DEFAULT_UPDATE);
      timeSelected[i]->setTargetPosition(hidePos, hidePos,
            GUI_OPTIONS_DEFAULT_UPDATE);
      cameraNotSelected[i]->setTargetPosition(hidePos, hidePos,
            GUI_OPTIONS_DEFAULT_UPDATE);
      cameraSelected[i]->setTargetPosition(hidePos, hidePos,
            GUI_OPTIONS_DEFAULT_UPDATE);
   }
}
   
/***********************************************************************
 *                               update                                *
 ***********************************************************************/
bool GuiOptions::isUpdating()
{
   bool updating = false;
   for(int i=0; i < 3; i++)
   {
      updating |= fieldNotSelected[i]->isUpdating();
      updating |= fieldSelected[i]->isUpdating();
      updating |= timeNotSelected[i]->isUpdating();
      updating |= timeSelected[i]->isUpdating();
      updating |= cameraNotSelected[i]->isUpdating();
      updating |= cameraSelected[i]->isUpdating();
   }
   
   return updating;
}
   
/***********************************************************************
 *                               update                                *
 ***********************************************************************/
void GuiOptions::update()
{
   for(int i=0; i < 3; i++)
   {
      fieldNotSelected[i]->update();
      fieldSelected[i]->update();
      timeNotSelected[i]->update();
      timeSelected[i]->update();
      cameraNotSelected[i]->update();
      cameraSelected[i]->update();
   }
}
   
/***********************************************************************
 *                                save                                 *
 ***********************************************************************/
void GuiOptions::save(Field* field)
{
   /* Set current values to current Options::*/
   switch(curTime)
   {
      case 0:
         Options::setMinutesPerHalf(3);
      break;
      case 2:
         Options::setMinutesPerHalf(10);
      break;
      case 1:
      default:
         Options::setMinutesPerHalf(5);
      break;
   }
   Options::setCameraType(curCamera);
   if(curField != Options::getFieldSize())
   {
      Options::setFieldSize(curField);
      field->createField(Options::getFieldFile(), NULL);
   }
   /* Finally, save them */
   Options::save();
}

}
