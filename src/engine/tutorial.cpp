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

#include "tutorial.h"
#include "core.h"
#include "rules.h"
#include "goalkeeper.h"
#include "../gui/guiscore.h"
#include "../gui/guimessage.h"
#include "../soundfiles.h"

#include <goblin/camera.h>
#include <goblin/screeninfo.h>
#include <kobold/i18n.h>

using namespace BtSoccer;

Ogre::String lessonImageFiles[] = {
   "tutorial/cammove.png",
   "tutorial/zoomin.png",
   "tutorial/zoomout.png",
   "tutorial/canrotlr.png",
   "tutorial/camrotud.png",
   "tutorial/buttonsel.png",
   "tutorial/buttondirec.png",
   "tutorial/buttonforce.png",
   "tutorial/diskball.png",
   "tutorial/foul.png",
   "tutorial/goal_shoot.png",
   "tutorial/gk_rot.png",
   "tutorial/ball_act.png"
};

Ogre::Vector2 lessonImageSizes[] = {
   Ogre::Vector2(128, 256),
   Ogre::Vector2(256, 128),
   Ogre::Vector2(256, 128),
   Ogre::Vector2(256, 256),
   Ogre::Vector2(256, 256),
   Ogre::Vector2(128, 128),
   Ogre::Vector2(256, 256),
   Ogre::Vector2(128, 256),
   Ogre::Vector2(128, 128),
   Ogre::Vector2(256, 256),
   Ogre::Vector2(256, 256),
   Ogre::Vector2(512, 256),
   Ogre::Vector2(64, 256)
};

#define TUTORIAL_LOADING_STATE_NONE          -1
#define TUTORIAL_LOADING_STATE_GUI            0
#define TUTORIAL_LOADING_STATE_GUI_LESSONS    1
#define TUTORIAL_LOADING_STATE_GUI_TEXTS      2
#define TUTORIAL_LOADING_STATE_GUI_BUTTONS    3
#define TUTORIAL_LOADING_STATE_GAME          10
#define TUTORIAL_LOADING_STATE_GAME_TEAM_B   11
#define TUTORIAL_LOADING_STATE_GAME_POINTERS 12

/***********************************************************************
 *                               Constructor                           *
 ***********************************************************************/
Tutorial::Tutorial(Core* core, BtSoccer::Team** teamA, BtSoccer::Team** teamB,
      BulletDebugDraw* debugDraw)
{
   int i;
   
   this->debugDraw = debugDraw;
   loadingState = TUTORIAL_LOADING_STATE_GUI;
   
   timer.reset();
   
   gameCore = core;
   curTutorial = TUTORIAL_CAMERA;
   curTutorialInit = TUTORIAL_CAMERA;
   
   gameCore->setPointers();
   
   teams[0] = teamA;
   teams[1] = teamB;
   deleteCurrentTeams();
   
   diskInput = false;
   diskPositionInput = false;
   goalKeeperPositionInput = false;
   definedDesc = false;
   
   /* Retrieve number of players per team on field */
   prevNumberOfDisks = Rules::getField()->getNumberOfDisks();
   /* Set it to a single one */
   Rules::getField()->setNumberOfDisks(2);
   
   /* Create overlay */
   overlay = Ogre::OverlayManager::getSingletonPtr()->create("tutorialOvl");
   overlay->setZOrder(642);
   overlay->show();
   
   /* Create progress bar */
   progressBar = new Goblin::Ibar(overlay, "main/bar_l.png", "main/bar_c.png",
         "main/bar_r.png", 
         Goblin::ScreenInfo::getHalfWindowWidth() - 128 * 
            Goblin::ScreenInfo::getGuiScale(),
         Goblin::ScreenInfo::getHalfWindowHeight() - 40 * 
            Goblin::ScreenInfo::getGuiScale(), 
         256 * Goblin::ScreenInfo::getGuiScale(),
         (Goblin::ScreenInfo::shouldUseDoubleSizedGui()) ? 26 : 6, "gui");
   progressBar->setPercentual(0.1f);
   progressBar->show();
   
   /* Nullify not initialized things */
   back = NULL;
   lessonTitle = NULL;
   lessonText[0] = NULL;
   lessonText[1] = NULL;
   btAccept = NULL;
   btRestart = NULL;
   btQuit = NULL;
   for(i = 0; i < TUTORIAL_TOTAL_IMAGES; i++)
   {
      lessonImage[i] = NULL;
   }
}

/***********************************************************************
 *                                Destructor                           *
 ***********************************************************************/
Tutorial::~Tutorial()
{
   int i;
   
   if(progressBar)
   {
      delete progressBar;
      progressBar = NULL;
   }
   
   if(back != NULL)
   {
      delete back;
      back = NULL;
   }
   
   if(lessonTitle != NULL)
   {
      delete lessonTitle;
      lessonTitle = NULL;
   }
   
   if(btAccept != NULL)
   {
      delete btAccept;
      btAccept = NULL;
   }
   if(btRestart != NULL)
   {
      delete btRestart;
      btRestart = NULL;
   }
   if(btQuit != NULL)
   {
      delete btQuit;
      btQuit = NULL;
   }
   
   for(i=0; i < 2; i++)
   {
      if(lessonText[i] != NULL)
      {
         delete lessonText[i];
         lessonText[i] = NULL;
      }
   }
   
   for(i = 0; i < TUTORIAL_TOTAL_IMAGES; i++)
   {
      if(lessonImage[i] != NULL)
      {
         delete lessonImage[i];
         lessonImage[i] = NULL;
      }
   }
   
   if(overlay)
   {
      Ogre::OverlayManager::getSingletonPtr()->destroy(overlay);
   }
}

/***********************************************************************
 *                          deleteCurrentTeams                         *
 ***********************************************************************/
void Tutorial::deleteCurrentTeams()
{
   if(*teams[0] != NULL)
   {
      delete *teams[0];
      *teams[0] = NULL;
   }
   if(*teams[1] != NULL)
   {
      delete *teams[1];
      *teams[1] = NULL;
   }
   BulletLink::setPointers(NULL, NULL, gameCore->getBall(), Rules::getField(),
                       false);
}

/***********************************************************************
 *                               doLoading                             *
 ***********************************************************************/
void Tutorial::doLoading()
{
   int i;
   
   switch(loadingState)
   {
      case TUTORIAL_LOADING_STATE_GUI:
      {
         /* Load back image */
         back = new Goblin::Image(overlay, "tutorial/back.png", "gui");
         back->setDimensions(1024 * Goblin::ScreenInfo::getGuiScale(), 
               512 * Goblin::ScreenInfo::getGuiScale());
         back->setPosition(-600 - 512 * Goblin::ScreenInfo::getGuiScale(), 
               Goblin::ScreenInfo::getHalfWindowHeight() - 
               276 * Goblin::ScreenInfo::getGuiScale());
         progressBar->setPercentual(0.3f);
         loadingState++;
      }
      break;
      case TUTORIAL_LOADING_STATE_GUI_LESSONS:
      {
         /* Load each lesson image */
         for(i=0; i < TUTORIAL_TOTAL_IMAGES; i++)
         {
            lessonImage[i] = new Goblin::Image(overlay, lessonImageFiles[i], 
                  "gui");
            lessonImage[i]->setDimensions(
                  lessonImageSizes[i].x*Goblin::ScreenInfo::getGuiScale(),
                  lessonImageSizes[i].y*Goblin::ScreenInfo::getGuiScale());
            lessonImage[i]->setPosition(-600*Goblin::ScreenInfo::getGuiScale(),
                  Goblin::ScreenInfo::getHalfWindowHeight() - 
                  (lessonImageSizes[i].y / 2)*Goblin::ScreenInfo::getGuiScale());
            lessonImage[i]->hide();
         }
         loadingState++;
         progressBar->setPercentual(0.5f);
      }
      break;
      case TUTORIAL_LOADING_STATE_GUI_TEXTS:
      {
         /* Create lesson text areas */
         lessonTitle = new Goblin::TextBox(0, 0,
               1024*Goblin::ScreenInfo::getGuiScale(), 
               32*Goblin::ScreenInfo::getGuiScale(), 
               "", "lessonTitleText", overlay, "freeSans24", 
               32*Goblin::ScreenInfo::getGuiScale());
         lessonTitle->setColor(0.0f, 0.0f, 0.0f, 1.0f);
         lessonTitle->setAlignment(Ogre::TextAreaOverlayElement::Left);
         back->addChild(lessonTitle, 180*Goblin::ScreenInfo::getGuiScale(), 
               20*Goblin::ScreenInfo::getGuiScale());
   
         lessonText[0] = new Goblin::TextBox(0, 0, 
               1024*Goblin::ScreenInfo::getGuiScale(), 
               256*Goblin::ScreenInfo::getGuiScale(), "",
               "lessonText0", overlay, "freeSans24", 
               24*Goblin::ScreenInfo::getGuiScale());
         lessonText[0]->setColor(0.45f, 0.45f, 0.45f, 1.0f);
         lessonText[0]->setAlignment(Ogre::TextAreaOverlayElement::Left);
         back->addChild(lessonText[0], 185*Goblin::ScreenInfo::getGuiScale(), 
               86*Goblin::ScreenInfo::getGuiScale());
         lessonText[1] = new Goblin::TextBox(0, 0,
               1024*Goblin::ScreenInfo::getGuiScale(), 
               256*Goblin::ScreenInfo::getGuiScale(), 
               "", "lessonText1", overlay, "freeSans24", 
               24*Goblin::ScreenInfo::getGuiScale());
         lessonText[1]->setColor(0.0f, 0.0f, 0.0f, 1.0f);
         lessonText[1]->setAlignment(Ogre::TextAreaOverlayElement::Center);
         back->addChild(lessonText[1], 512*Goblin::ScreenInfo::getGuiScale(), 
               422*Goblin::ScreenInfo::getGuiScale());
         loadingState++;
         progressBar->setPercentual(0.7f);
      }
      break;
      case TUTORIAL_LOADING_STATE_GUI_BUTTONS:
      {
         /* Create buttons */
         int fontSize = 16;
         int buttonSize = 64*Goblin::ScreenInfo::getGuiScale();
         int overSize = 65*Goblin::ScreenInfo::getGuiScale();
         btAccept = new Goblin::Ibutton(overlay, "initial/accept.png", "gui",
            Kobold::i18n::translate("Accept"), "infoFontOut", fontSize);
         btAccept->setPosition(-600*Goblin::ScreenInfo::getGuiScale(), 
               Goblin::ScreenInfo::getHalfWindowHeight() + 
               240*Goblin::ScreenInfo::getGuiScale());
         btAccept->setDimensions(buttonSize, buttonSize); 
         btAccept->setMouseOverDimensions(overSize, overSize);
         btAccept->setPressedSound(BTSOCCER_SOUND_GUI_CLICK);
         btRestart = new Goblin::Ibutton(overlay, "main/resume.png", "gui",
            Kobold::i18n::translate("Restart"), "infoFontOut", fontSize);
         btRestart->setPosition(-200*Goblin::ScreenInfo::getGuiScale(), 
               280*Goblin::ScreenInfo::getGuiScale());
         btRestart->setDimensions(buttonSize, buttonSize);
         btRestart->setMouseOverDimensions(overSize, overSize);
         btRestart->setPressedSound(BTSOCCER_SOUND_GUI_CLICK2);
         btQuit = new Goblin::Ibutton(overlay, "initial/quit.png", "gui",
            Kobold::i18n::translate("Quit"), "infoFontOut", fontSize);
         btQuit->setPosition(-200*Goblin::ScreenInfo::getGuiScale(), 
               350*Goblin::ScreenInfo::getGuiScale());
         btQuit->setDimensions(buttonSize, buttonSize);
         btQuit->setMouseOverDimensions(overSize, overSize);
         btQuit->setPressedSound(BTSOCCER_SOUND_GUI_CLICK2);
         progressBar->setPercentual(1.0f);
         loadingState++;
      }
      break;
      case TUTORIAL_LOADING_STATE_GAME:
      {
         deleteCurrentTeams();
         /* Create the teamA with 2 disks */
         *teams[0] = new BtSoccer::Team(
                        "countries/south_america/bolivia/bolivia.xut",
                        gameCore->getSceneManager(), Rules::getField(), 
                        debugDraw);
         (*teams[0])->startPositionAtField(false, true, Rules::getField());
         progressBar->setPercentual(0.4f);
         loadingState++;
      }
      break;
      case TUTORIAL_LOADING_STATE_GAME_TEAM_B:
      {
         /* Create the teamB with 2 disks */
         *teams[1] = new BtSoccer::Team(
                        "countries/africa/mozambique/mozambique.xut",
                        gameCore->getSceneManager(), Rules::getField(),
                        debugDraw);
         (*teams[1])->startPositionAtField(true, false, Rules::getField());
         progressBar->setPercentual(0.8f);
         loadingState++;
      }
      break;
      case TUTORIAL_LOADING_STATE_GAME_POINTERS:
      {
         gameCore->setPointers();
         Rules::setActiveTeam(*teams[0]);
         Rules::setUpperTeam(*teams[1]);
         diskInput = true;
         progressBar->setPercentual(1.0f);
         loadingState++;
      }
      break;
      default:
      {
         loadingState = TUTORIAL_LOADING_STATE_NONE;
         progressBar->hide();
      }
   }
   GuiScore::hide();
}

/***********************************************************************
 *                               setTutorial                           *
 ***********************************************************************/
void Tutorial::set(int tutorial)
{
   curTutorial = tutorial;
   curTutorialInit = tutorial;
   definedDesc = false;
}

/***********************************************************************
 *                            updateGuiElements                        *
 ***********************************************************************/
void Tutorial::updateGuiElements()
{
   int i;
   
   back->update();
   btAccept->update();
   btRestart->update();
   btQuit->update();
   
   for(i = 0; i < TUTORIAL_TOTAL_IMAGES; i++)
   {
      if(lessonImage[i] != NULL)
      {
         lessonImage[i]->update();
      }
   }
}

/***********************************************************************
 *                            setTutorialText                          *
 ***********************************************************************/
void Tutorial::setTutorialText(Ogre::String title, Ogre::String textA,
                               Ogre::String textB)
{
   lessonTitle->setText(Kobold::i18n::translate(title));
   lessonText[0]->setText(Kobold::i18n::translate(textA));
   lessonText[1]->setText(Kobold::i18n::translate(textB));
   definedDesc = true;
   
   showLessonDescription();
}

/***********************************************************************
 *                        showLessonDescription                        *
 ***********************************************************************/
void Tutorial::showLessonDescription()
{
   Ogre::Real pX = Ogre::Real(Goblin::ScreenInfo::getHalfWindowWidth()) -
         Ogre::Real(512*Goblin::ScreenInfo::getGuiScale());
   Ogre::Real pY = Goblin::ScreenInfo::getHalfWindowHeight() -
         276*Goblin::ScreenInfo::getGuiScale();
   back->setTargetPosition(pX, pY);
   
   /* Show lesson elements */
   if(Goblin::ScreenInfo::getWindowHeight() >= 760)
   {
      /* Usually, bellow tutorial info. */
      btAccept->setTargetPosition(Goblin::ScreenInfo::getHalfWindowWidth() -
            32 * Goblin::ScreenInfo::getGuiScale(),
            Goblin::ScreenInfo::getHalfWindowHeight() + 
            240*Goblin::ScreenInfo::getGuiScale());
   }
   else
   {
      /* On small screens (like those of phones), must set the accept button
       * to be at right side, optherwise should not be visible. */
      btAccept->setTargetPosition(Ogre::Real(
               Goblin::ScreenInfo::getWindowWidth()) -
            Ogre::Real(80*Goblin::ScreenInfo::getGuiScale()),
            Goblin::ScreenInfo::getWindowHeight() - 
               160 * Goblin::ScreenInfo::getGuiScale());
      
   }
   btAccept->show();

   
   /* Hide action gui */
   btRestart->setTargetPosition(-200*Goblin::ScreenInfo::getGuiScale(), 
         280*Goblin::ScreenInfo::getGuiScale());
   btQuit->setTargetPosition(-200*Goblin::ScreenInfo::getGuiScale(), 
         350*Goblin::ScreenInfo::getGuiScale());
}

/***********************************************************************
 *                        showTutorialActionGui                        *
 ***********************************************************************/
void Tutorial::showTutorialActionGui()
{
   /* Hide lesson description elements */
   btAccept->setTargetPosition(-600*Goblin::ScreenInfo::getGuiScale(), 
         Goblin::ScreenInfo::getHalfWindowHeight() + 
         240*Goblin::ScreenInfo::getGuiScale());
   back->setTargetPosition((-600-512)*Goblin::ScreenInfo::getGuiScale(), 
         Goblin::ScreenInfo::getHalfWindowHeight() - 
         276*Goblin::ScreenInfo::getGuiScale());
   int i;
   for(i=0; i < TUTORIAL_TOTAL_IMAGES; i++)
   {
      if(lessonImage[i] != NULL)
      {
         lessonImage[i]->setTargetPosition(-600*Goblin::ScreenInfo::getGuiScale(),
               Goblin::ScreenInfo::getHalfWindowHeight() - 
               (lessonImageSizes[i].y / 2)*Goblin::ScreenInfo::getGuiScale());
      }
   }
   
   /* Show action gui */
   btRestart->setTargetPosition(2*Goblin::ScreenInfo::getGuiScale(), 
         280*Goblin::ScreenInfo::getGuiScale());
   btRestart->show();
   btQuit->setTargetPosition(2*Goblin::ScreenInfo::getGuiScale(), 
         350*Goblin::ScreenInfo::getGuiScale());
   btQuit->show();
   
   definedDesc = false;
}

/***********************************************************************
 *                          hideAllLessonImages                        *
 ***********************************************************************/
void Tutorial::hideAllLessonImages()
{
   int i;
   for(i=0; i < TUTORIAL_TOTAL_IMAGES; i++)
   {
      if(lessonImage[i] != NULL)
      {
         lessonImage[i]->hide();
      }
   }
}

/***********************************************************************
 *                             showLessonImage                         *
 ***********************************************************************/
void Tutorial::showLessonImage(int i)
{
   Ogre::Vector2 pos;
   if(lessonImage[i] != NULL)
   {
      pos.x = Goblin::ScreenInfo::getHalfWindowWidth() - 
         (lessonImageSizes[i].x / 2) * Goblin::ScreenInfo::getGuiScale();
      pos.y = Goblin::ScreenInfo::getHalfWindowHeight() - 
         ((lessonImageSizes[i].y / 2) + 30)*Goblin::ScreenInfo::getGuiScale();
      lessonImage[i]->setTargetPosition(pos.x, pos.y);
      lessonImage[i]->show();
   }
}

/***********************************************************************
 *                         verifyDescriptionAccept                     *
 ***********************************************************************/
bool Tutorial::verifyDescriptionAccept(int mouseX, int mouseY,
                                       bool leftButtonPressed)
{
   if(btAccept->verifyEvents(mouseX, mouseY, leftButtonPressed) ==
      IBUTTON_EVENT_PRESSED)
   {
      curTutorial++;
      showTutorialActionGui();
      Rules::clearTouchesCounters();
      return true;
   }
   return false;
}

/***********************************************************************
 *                         verifyBallEnteredGoal                       *
 ***********************************************************************/
bool Tutorial::verifyBallEnteredGoal(bool up)
{
   /* Verify if ball entered goal: entered if fully crossed the line and
    * inner goal. So, we'll need to do a radius correction to ball's position
    * to get if it fully crossed the line before checking if inner or not. */
   Ball* ball = gameCore->getBall();
   Ogre::Vector3 ballPos = ball->getPosition();
   Ogre::AxisAlignedBox goalBox;
   if(up)
   {
      goalBox = Rules::getField()->getUpGoalBox();
      ballPos.x -= ball->getSphereRadius();
   }
   else
   {
      goalBox = Rules::getField()->getDownGoalBox();
      ballPos.x += ball->getSphereRadius();
   }
   return goalBox.contains(ballPos);
}

/***********************************************************************
 *                             updateTutorial                          *
 ***********************************************************************/
bool Tutorial::update(int mouseX, int mouseY, bool leftButtonPressed)
{
   if(loadingState != TUTORIAL_LOADING_STATE_NONE)
   {
      doLoading();
      return true;
   }
   
   /* Update all gui elements  */
   updateGuiElements();
   
   /* Verify Buttons */
   if(btQuit->verifyEvents(mouseX, mouseY, leftButtonPressed) ==
      IBUTTON_EVENT_PRESSED)
   {
      /* The user want to quit. End tutorial and exit */
      finish();
      return false;
   }
   if(btRestart->verifyEvents(mouseX, mouseY, leftButtonPressed) ==
      IBUTTON_EVENT_PRESSED)
   {
      /* Go back to tutorial description */
      curTutorial = curTutorialInit;
   }
   switch(curTutorial)
   {
      case TUTORIAL_CAMERA:
      {
         /* Just say what tutorial mode we are, and move to next one */
         curTutorial++;
         GuiMessage::set("Started camera tutorial");
         /* Set the camera to middle initial position */
         Goblin::Camera::setTarget(-10.0f, 1.5f, -0.25f,
               -90.0f, 33.0f, 16.75f);
         Goblin::Camera::disableZoomChanges();
         Goblin::Camera::disableRotations();
         Goblin::Camera::disableTranslations();
         definedDesc = false;
      }
      break;
      case TUTORIAL_CAMERA_MOVE:
      {
         if(!definedDesc)
         {
            curTutorialInit = curTutorial;
            hideAllLessonImages();
            setTutorialText("1. Camera Move", "msgCameraMove1",
                            "msgCameraMove2");
            showLessonImage(0);
         }
         else if(verifyDescriptionAccept(mouseX, mouseY, leftButtonPressed))
         {
            Goblin::Camera::enableTranslations();
         }
      }
      break;
      case TUTORIAL_CAMERA_MOVE_ACT:
      {
         /* Wait the camera to be after the center line */
         if(Goblin::Camera::getCenterX() > 6.75f)
         {
            GuiMessage::set("Done!");
            curTutorial++;
         }
      }
      break;
      case TUTORIAL_CAMERA_ZOOM_IN:
      {
         if(!definedDesc)
         {
            curTutorialInit = curTutorial;
            hideAllLessonImages();
            setTutorialText("2. Camera Zoom In", "msgCameraZoomIn1",
                            "msgCameraZoomIn2");
            showLessonImage(1);
         }
         else if(verifyDescriptionAccept(mouseX, mouseY, leftButtonPressed))
         {
            Goblin::Camera::enableZoomChanges();
         }
      }
      break;
      case TUTORIAL_CAMERA_ZOOM_IN_ACT:
      {
         /* Wait for camera to be at zoom min */
         if(Goblin::Camera::getZoom() == 
               Goblin::Camera::getConfiguration().zoomMax)
         {
            GuiMessage::set("Done!");
            curTutorial++;
         }
      }
      break;
      case TUTORIAL_CAMERA_ZOOM_OUT:
      {
         if(!definedDesc)
         {
            curTutorialInit = curTutorial;
            hideAllLessonImages();
            setTutorialText("3. Camera Zoom Out", "msgCameraZoomOut1",
                            "msgCameraZoomOut2");
            showLessonImage(2);
         }
         else
         {
            verifyDescriptionAccept(mouseX, mouseY, leftButtonPressed);
         }
      }
      break;
      case TUTORIAL_CAMERA_ZOOM_OUT_ACT:
      {
         /* Wait for camera to be at zoom min */
         if(Goblin::Camera::getZoom() ==
               Goblin::Camera::getConfiguration().zoomMin)
         {
            GuiMessage::set("Done!");
            curTutorial++;
            value = Goblin::Camera::getPhi();
         }
      }
      break;
      case TUTORIAL_CAMERA_ROTATE_LEFT:
      {
         if(!definedDesc)
         {
            curTutorialInit = curTutorial;
            hideAllLessonImages();
            setTutorialText("4. Camera Side Rotations", "msgCameraRotSide1",
                            "msgCameraRotSide2");
            showLessonImage(3);
         }
         else if(verifyDescriptionAccept(mouseX, mouseY, leftButtonPressed))
         {
            Goblin::Camera::enableRotations();
         }
      }
      break;
      case TUTORIAL_CAMERA_ROTATE_LEFT_ACT:
      {
         /* Big step == range overflow */
         if(Ogre::Math::Abs(Goblin::Camera::getPhi() - value) >= 300)
         {
            Goblin::Camera::setCurrentPhi(180);
         }
         else if(Goblin::Camera::getPhi() > value)
         {
            /* Moved on wrong direction */
            GuiMessage::set("Wrong direction!");
            value = Goblin::Camera::getPhi();
         }
         else if(Goblin::Camera::getPhi() < value)
         {
            GuiMessage::clear();
            value = Goblin::Camera::getPhi();
            if(Goblin::Camera::getPhi() < -140)
            {
               /* Reset the timer, to avoid showing "wrong direction"
                * just after*/
               timer.reset();
               GuiMessage::set("Done! Now rotate right.");
               curTutorial++;
            }
         }
      }
      break;
      case TUTORIAL_CAMERA_ROTATE_RIGHT_ACT:
      {
         /* Big step == range overflow */
         if(Ogre::Math::Abs(Goblin::Camera::getPhi() - value) >= 300)
         {
            Goblin::Camera::setCurrentPhi(-180);
         }
         else if((Goblin::Camera::getPhi() < value) &&
                 (timer.getMilliseconds() > 2000) )
         {
            /* Moved on wrong direction */
            GuiMessage::set("Wrong direction!");
            value = Goblin::Camera::getPhi();
         }
         else if(Goblin::Camera::getPhi() > value)
         {
            GuiMessage::clear();
            value = Goblin::Camera::getPhi();
            if(Goblin::Camera::getPhi() > -40)
            {
               GuiMessage::set("Done!");
               curTutorial++;
            }
         }
      }
      break;
      case TUTORIAL_CAMERA_ROTATE_UP:
      {
         if(!definedDesc)
         {
            curTutorialInit = curTutorial;
            hideAllLessonImages();
            setTutorialText("5. Camera Up/Down Rotations", "msgCameraRotVert1",
                            "msgCameraRotVert2");
            showLessonImage(4);
         }
         else
         {
            verifyDescriptionAccept(mouseX, mouseY, leftButtonPressed);
         }
      }
      break;
      case TUTORIAL_CAMERA_ROTATE_UP_ACT:
      {
         /* Wait for camera to be at up */
         if(Goblin::Camera::getTheta() == 89)
         {
            GuiMessage::set("Done! Now rotate down.");
            curTutorial++;
         }
      }
      break;
      case TUTORIAL_CAMERA_ROTATE_DOWN_ACT:
      {
         /* Wait for camera to be at up */
         if(Goblin::Camera::getTheta() <= 40)
         {
            GuiMessage::set("Done!");
            curTutorial++;
            Goblin::Camera::disableZoomChanges();
            Goblin::Camera::disableRotations();
            Goblin::Camera::disableTranslations();
         }
      }
      break;
      case TUTORIAL_GAME:
      {
         loadingState = TUTORIAL_LOADING_STATE_GAME;
         progressBar->setPercentual(0.1f);
         progressBar->show();
         
         /* Let's really start game tutorial */
         GuiMessage::set("Started game tutorial");
         curTutorial++;
      }
      break;
      case TUTORIAL_DISK_MOVE_SEL:
      {
         if(!definedDesc)
         {
            curTutorialInit = curTutorial;
            hideAllLessonImages();
            setTutorialText("6. Disk Move: Selection", "msgDiskMoveSel1",
                            "msgDiskMoveSel2");
            showLessonImage(5);
            Goblin::Camera::disableZoomChanges();
            Goblin::Camera::disableRotations();
            Goblin::Camera::disableTranslations();
            Goblin::Camera::setTarget(0.0f, 1.5f, -0.25f,
                             -90.0f, 33.0f, 16.75f);
         }
         else
         {
            verifyDescriptionAccept(mouseX, mouseY, leftButtonPressed);
         }
      }
      break;
      case TUTORIAL_DISK_MOVE_DIREC:
      {
         if(!definedDesc)
         {
            hideAllLessonImages();
            setTutorialText("6. Disk Move: Direction", "msgDiskMoveDir1",
                            "msgDiskMoveDir2");
            showLessonImage(6);
         }
         else
         {
            verifyDescriptionAccept(mouseX, mouseY, leftButtonPressed);
         }
      }
      break;
      case TUTORIAL_DISK_MOVE_FORCE:
      {
         if(!definedDesc)
         {
            hideAllLessonImages();
            setTutorialText("6. Disk Move: Strength", "msgDiskMoveStr1",
                            "msgDiskMoveStr2");
            showLessonImage(7);
         }
         else if(verifyDescriptionAccept(mouseX, mouseY, leftButtonPressed))
         {
            Goblin::Camera::enableZoomChanges();
            Goblin::Camera::enableRotations();
            Goblin::Camera::enableTranslations();
         }
      }
      break;
      case TUTORIAL_DISK_MOVE_ACT:
      {
         /* Verify if both teamA disks are at penalty area. */
         Ogre::Vector3 disk1Pos = (*teams[0])->getDisk(0)->getPosition();
         Ogre::Vector3 disk2Pos = (*teams[0])->getDisk(1)->getPosition();
         Field* field = Rules::getField();
         if( (!(*teams[0])->getDisk(0)->getMovedFlag()) &&
             (!(*teams[0])->getDisk(1)->getMovedFlag()) &&
             (field->isInnerPenaltyArea(disk1Pos.x, disk1Pos.z)) &&
             (field->isInnerPenaltyArea(disk2Pos.x, disk2Pos.z)) )
         {
            GuiMessage::set("Done!");
            curTutorial++;
         }
      }
      break;
      case TUTORIAL_DISK_BALL:
      {
         if(!definedDesc)
         {
            hideAllLessonImages();
            setTutorialText("7. Controlling ball with disk", "msgBallCtrl1",
                            "msgBallCtrl2");
            showLessonImage(8);
            Goblin::Camera::disableZoomChanges();
            Goblin::Camera::disableRotations();
            Goblin::Camera::disableTranslations();
            Goblin::Camera::setTarget(0.0f, 1.5f, -0.25f,
                             -90.0f, 33.0f, 16.75f);
            gameCore->getBall()->setPosition(0.0f, 0.0f, 0.0f);
            curTutorialInit = curTutorial;
            
            (*teams[0])->startPositionAtField(false, true, Rules::getField());
            (*teams[1])->startPositionAtField(true, false, Rules::getField());
         }
         else if(verifyDescriptionAccept(mouseX, mouseY, leftButtonPressed))
         {
            Goblin::Camera::enableZoomChanges();
            Goblin::Camera::enableRotations();
            Goblin::Camera::enableTranslations();
         }
      }
      break;
      case TUTORIAL_DISK_BALL_ACT:
      {
         Ogre::Vector3 ballPos = gameCore->getBall()->getPosition();
         Field* field = Rules::getField();
         if( (!gameCore->getBall()->getMovedFlag()) &&
             (field->isInnerPenaltyArea(ballPos.x, ballPos.z)) )
         {
            curTutorial++;
            GuiMessage::set("Done!");
         }
      }
      break;
      case TUTORIAL_DISK_FOUL:
      {
         if(!definedDesc)
         {
            curTutorialInit = curTutorial;
            hideAllLessonImages();
            setTutorialText("8. Fouls", "msgFoul1", "msgFoul2");
            showLessonImage(9);
            Goblin::Camera::disableZoomChanges();
            Goblin::Camera::disableRotations();
            Goblin::Camera::disableTranslations();
            Goblin::Camera::setTarget(0.0f, 1.5f, -0.25f,
                             -90.0f, 33.0f, 16.75f);
            gameCore->getBall()->setPosition(0.0f, 0.0f, 0.0f);
            (*teams[0])->getDisk(0)->setPosition(-0.5f, 0.0f, 0.0f);
            (*teams[0])->getDisk(1)->setPosition(-0.75f, 0.0f, -4.0f);
            (*teams[1])->getDisk(0)->setPosition(0.6f, 0.0f, -4.0f);
            (*teams[1])->getDisk(1)->setPosition(0.6f, 0.0f, 4.0f);
            GoalKeeper* gk = (*teams[1])->getGoalKeeper();
            gk->setPosition(Rules::getField()->getHalfSize().x -
                            Rules::getField()->getSideDelta().x, 0.0f, 4.0f);
         }
         else if(verifyDescriptionAccept(mouseX, mouseY, leftButtonPressed))
         {
            Goblin::Camera::enableZoomChanges();
            Goblin::Camera::enableRotations();
            Goblin::Camera::enableTranslations();
         }
      }
      break;
      case TUTORIAL_DISK_FOUL_WAIT:
      {
         /* Wait for left mouse button be released */
         if(!leftButtonPressed)
         {
            /* Released: set to disk input position */
            gameCore->setState(Core::BTSOCCER_STATE_DISK_POSITION);
            gameCore->setSelectedPlayer((*teams[0])->getDisk(0));
            value = -1.0f;
            curTutorial++;
         }
      }
      break;
      case TUTORIAL_DISK_FOUL_ACT:
      {
         if(verifyBallEnteredGoal(true))
         {
            GuiMessage::set("Done!");
            curTutorial++;
         }
         /* If couldn't make the goal, reset it */
         else if(BulletLink::isWorldStable())
         {
               if(value > 0.0f)
               {
                  /* Moved and bal not inner goal, reset it! */
                  GuiMessage::set("Failed!");
                  curTutorial = curTutorialInit;
               }
         }
         else
         {
            /*  Something moved, now we can check after is stable */
            value = 1.0f;
         }
      }
      break;
      case TUTORIAL_GOAL_SHOOTS:
      {
         if(!definedDesc)
         {
            curTutorialInit = curTutorial;
            hideAllLessonImages();
            setTutorialText("9. Goal shoots", "msgGoalShoot1", "msgGoalShoot2");
            showLessonImage(10);
         }
         else
         {
            verifyDescriptionAccept(mouseX, mouseY, leftButtonPressed);
         }
      }
      break;
      case TUTORIAL_GOAL_KEEPER_MOVE:
      {
         if(!definedDesc)
         {
            curTutorialInit = curTutorial;
            hideAllLessonImages();
            setTutorialText("10. GoalKeeper Move", "msgGkMove1", "msgGkMove2");
            showLessonImage(11);
            Goblin::Camera::disableZoomChanges();
            Goblin::Camera::disableRotations();
            Goblin::Camera::disableTranslations();
            /* Change camera to the target goal */
            Ogre::Vector3 pPos = (*teams[0])->getGoalKeeper()->getPosition();
            Goblin::Camera::setTarget(pPos.x, 0.0f, pPos.z,
                             -90.0f, 33.0f, 16.75f);
            gameCore->getBall()->setPosition(0.0f, 0.0f, 0.0f);
            (*teams[1])->getDisk(0)->setPosition(0.9f, 0.0f, 0.05f);
            (*teams[1])->getDisk(1)->setPosition(0.75f, 0.0f, -4.0f);
            (*teams[0])->getDisk(0)->setPosition(-0.6f, 0.0f, -4.0f);
            (*teams[0])->getDisk(1)->setPosition(-0.6f, 0.0f, 4.0f);
            GoalKeeper* gk = (*teams[0])->getGoalKeeper();
            gk->setPosition(-Rules::getField()->getHalfSize().x +
                            Rules::getField()->getSideDelta().x, 0.0f, 0.0f);
         }
         else
         {
            if(verifyDescriptionAccept(mouseX, mouseY, leftButtonPressed))
            {
               gameCore->setState(Core::BTSOCCER_STATE_GOAL_KEEPER_POSITION);
               if(Rules::getActiveTeam() != (*teams[0]))
               {
                  Rules::setActiveTeam((*teams[0]));
               }
               gameCore->setSelectedPlayer((*teams[0])->getGoalKeeper());
               (*teams[0])->getGoalKeeper()->setRestrictMove(false);
               btAccept->setTargetPosition(80, 80);
               btAccept->show();
            }
         }
      }
      break;
      case TUTORIAL_GOAL_KEEPER_ACT:
      {
         if(btAccept->verifyEvents(mouseX, mouseY, leftButtonPressed) ==
            IBUTTON_EVENT_PRESSED)
         {
            /* Must show goals again. */
            Rules::getField()->getGoals()->setOpaque();
            curTutorial++;
            btAccept->setPosition(-600, 
            Goblin::ScreenInfo::getHalfWindowHeight()+290);
            gameCore->setState(Core::BTSOCCER_STATE_TUTORIAL);
            //FIXME: must recalibrate the shoot!
            gameCore->doShoot((*teams[1])->getDisk(0),
                              0.9f, 0.25f, 3.4f, 0.8f);
            value = 0.0f; // ball did not enter goal.
         }
      }
      break;
      case TUTORIAL_GOAL_KEEPER_ACT_WAIT:
      {
         /* Wait for shoot finalized. */
         if(verifyBallEnteredGoal(false))
         {
            value = 1.0f;
         }
         
         if(BulletLink::isWorldStable())
         {
            if(value > 0.0f)
            {
               /* ball entered goal, reset it! */
               GuiMessage::set("Failed!");
               curTutorial = curTutorialInit;
            }
            else
            {
               /* Ball stoped and didn't entred goal. done! */
               GuiMessage::set("Done!");
               curTutorial++;
            }
         }
         
      }
      break;
      case TUTORIAL_BALL_INNER_OWN_AREA:
      {
         if(!definedDesc)
         {
            curTutorialInit = curTutorial;
            hideAllLessonImages();
            setTutorialText("11. Ball Inner Own Area", "msgBallInner1",
                            "msgBallInner2");
            showLessonImage(12);
            Goblin::Camera::disableZoomChanges();
            Goblin::Camera::disableRotations();
            Goblin::Camera::disableTranslations();
            /* Change camera to the target goal */
            Ogre::Vector3 pPos = (*teams[0])->getGoalKeeper()->getPosition();
            Goblin::Camera::setTarget(pPos.x, 0.0f, pPos.z,
                             -90.0f, 89.0f, 16.75f);
            gameCore->getBall()->setPosition(
               -Rules::getField()->getHalfSize()[0] +
               Rules::getField()->getPenaltyMark(), 0.0f, 0.0f);
            (*teams[0])->getDisk(0)->setPosition(-0.5f, 0.0f, 0.0f);
            (*teams[0])->getDisk(1)->setPosition(-0.75f, 0.0f, -4.0f);
            (*teams[1])->getDisk(0)->setPosition(0.6f, 0.0f, -4.0f);
            (*teams[1])->getDisk(1)->setPosition(0.6f, 0.0f, 4.0f);
            GoalKeeper* gk = (*teams[0])->getGoalKeeper();
            gk->setPosition(-Rules::getField()->getHalfSize().x +
                            Rules::getField()->getSideDelta().x, 0.0f, 0.0f);
            if(Rules::getActiveTeam() != (*teams[0]))
            {
               Rules::setActiveTeam((*teams[0]));
            }
            value = -1.0f;
         }
         else
         {
            if(verifyDescriptionAccept(mouseX, mouseY, leftButtonPressed))
            {
               Goblin::Camera::enableZoomChanges();
               Goblin::Camera::enableRotations();
               Goblin::Camera::enableTranslations();
            }
         }
      }
      break;
      case TUTORIAL_BALL_INNER_OWN_AREA_WAIT:
      {
         /* Wait for stable physic. */
         BulletLink::step(BTSOCCER_UPDATE_RATE, 10);
         if(BulletLink::isWorldStable())
         {
            curTutorial++;
         }
      }
      break;
      case TUTORIAL_BALL_INNER_OWN_AREA_ACT:
      {
         /* If couldn't remove ball from area, reset it */
         if(BulletLink::isWorldStable())
         {
            if(value > 0.0f)
            {
               Ogre::Vector3 ballPos = gameCore->getBall()->getPosition();
               if(!Rules::getField()->isInnerPenaltyArea(ballPos.x, ballPos.z))
               {
                  /* Removed ball from area, done! */
                  GuiMessage::set("Done!");
                  curTutorial++;
               }
               else
               {
                  GuiMessage::set("Failed!");
                  curTutorial = curTutorialInit;
               }
            }
         }
         else
         {
            /*  Something moved, now we can check after is stable */
            value = 1.0f;
         }
      }
      break;
      case TOTAL_TUTORIALS:
      {
         /* Tutorial is over! */
         finish();
         return false;
      }
      break;
   }
   
   return true;
}

/***********************************************************************
 *                                 finish                              *
 ***********************************************************************/
void Tutorial::finish()
{
   /* Enable all camera moves */
   Goblin::Camera::enableRotations();
   Goblin::Camera::enableZoomChanges();
   Goblin::Camera::enableTranslations();
   
   /* Restore number of players per team on field */
   Rules::getField()->setNumberOfDisks(prevNumberOfDisks);
   
   /* Hide all gui elements */
   hideAllLessonImages();
   btAccept->hide();
   btQuit->hide();
   btRestart->hide();
}

/***********************************************************************
 *                          isDiskInputEnabled                         *
 ***********************************************************************/
bool Tutorial::isDiskInputEnabled()
{
   return diskInput;
}

/***********************************************************************
 *                      isDiskPositionInputEnabled                     *
 ***********************************************************************/
bool Tutorial::isDiskPositionInputEnabled()
{
   return diskPositionInput;
}

/***********************************************************************
 *                      isGoalKeeperInputEnabled                       *
 ***********************************************************************/
bool Tutorial::isGoalKeeperPositionInputEnabled()
{
   return goalKeeperPositionInput;
}

