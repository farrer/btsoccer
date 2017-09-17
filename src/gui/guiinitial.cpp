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

#include "guiinitial.h"
#include <kobold/defparser.h>
#include <kobold/i18n.h>

using namespace BtSoccer;

Ogre::String buttonFileName[]=
{
   "initial/tutorial.png",
   "initial/quick.png",
#ifdef BTSOCCER_HAS_CUP
   "initial/cup.png",
#endif
   "initial/online_game.png",
   "initial/options.png",
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS &&\
    OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
   "initial/quit.png",
#endif
   "initial/cup_new.png",
#ifdef BTSOCCER_HAS_AI
   "initial/quick_new.png",
#endif
   "initial/quick_versus.png",
   "initial/cancel.png",
   "initial/load.png",
   "initial/save.png",
   "initial/next.png",
   "initial/previous.png",
   "initial/accept.png",
   "initial/tutorial_camera.png",
   "initial/tutorial_disk.png",
   "initial/create_server.png",
   "initial/connect_server.png"
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
   ,"initial/gamecenter.png"
#endif
};

Ogre::String buttonText[]=
{
   "Tutorial",
   "Quick|Match",
#ifdef BTSOCCER_HAS_CUP
   "Cup",
#endif
   "Online|Match",
   "Options",
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS &&\
    OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
   "Quit",
#endif
   "New",
#ifdef BTSOCCER_HAS_AI
   "Single|Player",
#endif
   "Versus",
   "Cancel",
   "Load",
   "Save",
   "",
   "",
   "Accept",
   "Camera|Tutorial",
   "Game|Tutorial",
   "Create|Server",
   "Connect|Server"
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
   ,"Game|Center"
#endif
};

/* Button X position, relative to Middle screen width. */
Ogre::Real buttonPosX[]=
{
   -92,
   28,
   -92,
   28,
#ifdef BTSOCCER_HAS_CUP
   -92,
#endif
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS &&\
    OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
   -32,
#endif
   -137,
#ifdef BTSOCCER_HAS_AI
   -137,
#endif
   3,
   73,
   -67,
   -67,
   124,
   -188,
   -103,
   -102,
   -32,
   -102,
   -32
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
   ,38
#endif
};

Ogre::Real buttonPosY[]=
{
   144,
   204,
   264,
   324,
#ifdef BTSOCCER_HAS_CUP
   384,
#endif
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS &&\
    OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
   484,
#endif
   132,
#ifdef BTSOCCER_HAS_AI
   132,
#endif
   132,
   132,
   132,
   132,
   255,
   255,
   430,
   132,
   132,
   132,
   132
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
   ,132
#endif
};

#define TEAM_LOGO_POS_Y  210
#define TEAM_LOGO_SIZE   180

#define GUIINITIAL_DEFAULT_UPDATE      20
#define GUIINITIAL_DEFAULT_SLOW_UPDATE 35

/*********************************************************************
 *                             TeamInfo                              *
 *********************************************************************/
GuiInitialTeamInfo::GuiInitialTeamInfo()
{
   logo = NULL;
   info = NULL;
}

/*********************************************************************
 *                            ~TeamInfo                              *
 *********************************************************************/
GuiInitialTeamInfo::~GuiInitialTeamInfo()
{
   if(logo)
   {
      delete logo;
      logo = NULL;
   }
   info = NULL;
}


/*********************************************************************
 *                            Constructor                            *
 *********************************************************************/
GuiInitial::GuiInitial()
{
   int i;
   state = STATE_INITIAL;
   returnStatus = RETURN_OTHER;

   modeSelected = -1;
   teamsToSelect = 0;
   selTeamA = -1;
   selTeamB = -1;
   startedSelectedAnimation = false;

   /* Create the overlay */
   ogreOverlay = Ogre::OverlayManager::getSingletonPtr()->create("initialOvl");
   ogreOverlay->setZOrder(640);
   ogreOverlay->show();
   
   int buttonFontSize = 16;
   int buttonSize = 64 * Goblin::ScreenInfo::getGuiScale();
   int overSize = 65 * Goblin::ScreenInfo::getGuiScale();

   /* Create the buttons */
   for(i = 0; i < TOTAL_INITIAL_BUTTONS; i++)
   {
      /* Create the button */
      if(buttonText[i].length() > 0)
      {
         button[i] = new Goblin::Ibutton(ogreOverlay, buttonFileName[i], "gui",
               Kobold::i18n::translate(buttonText[i]), 
               "infoFontOut", buttonFontSize);
      }
      else
      {
         button[i] = new Goblin::Ibutton(ogreOverlay, buttonFileName[i], "gui");
      }
      button[i]->setDimensions(buttonSize, buttonSize);
      button[i]->setMouseOverDimensions(overSize, overSize);
      button[i]->setPosition(-100, -100);
      if(i < BUTTON_CUP_NEW)
      {
         button[i]->setTargetPosition(Goblin::ScreenInfo::getHalfWindowWidth()
               + buttonPosX[i] * Goblin::ScreenInfo::getGuiScale(),
               buttonPosY[i] * Goblin::ScreenInfo::getGuiScale(), 
               GUIINITIAL_DEFAULT_SLOW_UPDATE);
      }
      
      if((i == BUTTON_CANCEL) || (i == BUTTON_PREVIOUS))
      {
         button[i]->setPressedSound(BTSOCCER_SOUND_GUI_CLICK2);
      }
      else
      {
         button[i]->setPressedSound(BTSOCCER_SOUND_GUI_CLICK);
      }
   }
   
   /* Create loading bar */
   loadingBar = new Goblin::Ibar(ogreOverlay, "main/bar_l.png", 
         "main/bar_c.png", "main/bar_r.png", 
         Goblin::ScreenInfo::getHalfWindowWidth() - 
         128 * Goblin::ScreenInfo::getGuiScale(),
         Goblin::ScreenInfo::getHalfWindowHeight() * 
         Goblin::ScreenInfo::getGuiScale() - 40,
         256 * Goblin::ScreenInfo::getGuiScale(), 
         (Goblin::ScreenInfo::shouldUseDoubleSizedGui())?26:6, "gui");
   loadingBar->hide();

   /* Create team name elements */
   teamNameImage = new Goblin::Image(ogreOverlay, "initial/teamtext.png", 
         "gui");
   teamNameImage->setDimensions(128*Goblin::ScreenInfo::getGuiScale(),
         64*Goblin::ScreenInfo::getGuiScale());
   teamNameImage->setPosition(-200, -200);
   teamName = new Goblin::TextBox(-200, -200, 
         128*Goblin::ScreenInfo::getGuiScale(),
         64*Goblin::ScreenInfo::getGuiScale(), "",
         "TeamNameText", ogreOverlay, "infoFont",
         22*Goblin::ScreenInfo::getGuiScale());
   teamName->setColor(0.0f, 0.0f, 0.0f, 1.0f);
   teamName->setAlignment(Ogre::TextAreaOverlayElement::Center);

   /* Create region buttons */
   Ogre::String regionName;
   Region* reg;
   totalRegions = Regions::getTotalRegions();
   regions = new Goblin::Ibutton*[totalRegions];
   overSize = 68*Goblin::ScreenInfo::getGuiScale();
   for(i=0; i < totalRegions; i++)
   {
      reg = Regions::getRegion(i);
      regions[i] = new Goblin::Ibutton(ogreOverlay, reg->imageFile, "gui");
      regions[i]->setDimensions(buttonSize, buttonSize);
      regions[i]->setMouseOverDimensions(overSize, overSize);
      regions[i]->setPosition(-100*Goblin::ScreenInfo::getGuiScale(),
            -100*Goblin::ScreenInfo::getGuiScale());
      regions[i]->setPressedSound(BTSOCCER_SOUND_GUI_CLICK2);
   }

   /* Create the load team icons */
   curSel = new Goblin::Image(ogreOverlay, "initial/teamselected.png", "gui");
   curSel->setDimensions(256*Goblin::ScreenInfo::getGuiScale(),
         256*Goblin::ScreenInfo::getGuiScale());
   curSel->setPosition(-300*Goblin::ScreenInfo::getGuiScale(),
         -300*Goblin::ScreenInfo::getGuiScale());
   teamSel[0] = new Goblin::Image(ogreOverlay, "initial/teamsel.png", "gui");
   teamSel[0]->setPosition(-150*Goblin::ScreenInfo::getGuiScale(),
         -150*Goblin::ScreenInfo::getGuiScale());
   teamSel[1] = new Goblin::Image(ogreOverlay, "initial/teamsel.png", "gui");
   teamSel[1]->setPosition(-150*Goblin::ScreenInfo::getGuiScale(),
         -150*Goblin::ScreenInfo::getGuiScale());
   vsImage = new Goblin::Image(ogreOverlay, "initial/vs.png", "gui");
   vsImage->setPosition(-100*Goblin::ScreenInfo::getGuiScale(),
         -100*Goblin::ScreenInfo::getGuiScale());

   /* Load all team logos */
   totalTeams = Regions::getTotalTeams();
   teams = new GuiInitialTeamInfo[totalTeams];
   TeamInfo* t = Regions::getFirstTeam();
   for(int i = 0; i < totalTeams; i++)
   {
      teams[i].info = t;
      teams[i].logo = new Goblin::Image(ogreOverlay,
            (teams[i].info->prefix+Ogre::String("_symbol.png")), "game");
      teams[i].logo->setDimensions(1,1);
      teams[i].logo->setPosition(-100*Goblin::ScreenInfo::getGuiScale(),
            -100*Goblin::ScreenInfo::getGuiScale());
      
      t = Regions::getNextTeam(t);
   }

   btsoccerLogo = new Goblin::Image(ogreOverlay, "initial/btsoccer_logo.png", 
         "gui");
   btsoccerLogo->setDimensions(256*Goblin::ScreenInfo::getGuiScale(),
         128*Goblin::ScreenInfo::getGuiScale());
   btsoccerLogo->setPosition(Goblin::ScreenInfo::getHalfWindowWidth() -
         128*Goblin::ScreenInfo::getGuiScale(), -200);
   btsoccerLogo->setTargetPosition(Goblin::ScreenInfo::getHalfWindowWidth() -
         128*Goblin::ScreenInfo::getGuiScale(), 0, GUIINITIAL_DEFAULT_UPDATE);
   
   guiOptions = new GuiOptions();
}

/*********************************************************************
 *                             Destructor                            *
 *********************************************************************/
GuiInitial::~GuiInitial()
{
   int i;
   if(guiOptions)
   {
      delete(guiOptions);
   }
   for(i=0; i<TOTAL_INITIAL_BUTTONS; i++)
   {
      delete button[i];
   }
   for(i=0; i<totalRegions; i++)
   {
      delete regions[i];
   }
   delete[] regions;
   delete teamSel[0];
   delete teamSel[1];
   delete curSel;
   delete vsImage;
   delete loadingBar;
   delete teamNameImage;
   delete teamName;
   delete btsoccerLogo;
   delete[] teams;
   /* Bye overlays */
   Ogre::OverlayManager::getSingletonPtr()->destroy(ogreOverlay);
}

/****************************************************************
 *                     setLoadingPercentual                     *
 ****************************************************************/
void GuiInitial::setLoadingPercentual(float perc)
{
   if(!loadingBar->isVisible())
   {
      loadingBar->show();
   }
   loadingBar->setPercentual(perc);
}

/****************************************************************
 *                          setRegion                           *
 ****************************************************************/
void GuiInitial::setRegion(int prevTeam)
{
   int curReg = teams[curTeam].info->region->id;

   int staticSize = 70 * Goblin::ScreenInfo::getGuiScale();
   regions[curReg]->setDimensions(staticSize, staticSize);
   regions[curReg]->setMouseOverDimensions(staticSize, staticSize);

   if( (prevTeam != -1) && 
       (teams[prevTeam].info->region != teams[curTeam].info->region) )
   {
      int prevReg = teams[prevTeam].info->region->id;
      
      regions[prevReg]->setDimensions(
            64 * Goblin::ScreenInfo::getGuiScale(),
            64 * Goblin::ScreenInfo::getGuiScale());
      regions[prevReg]->setMouseOverDimensions(
            68 * Goblin::ScreenInfo::getGuiScale(),
            68 * Goblin::ScreenInfo::getGuiScale());
   }
}

/****************************************************************
 *                        changeTeam                            *
 ****************************************************************/
void GuiInitial::changeTeam(int prevTeam)
{
   /* Hide previous, show current logo */
   teams[prevTeam].logo->setTargetDimensions(1, 1, 20);
   teams[prevTeam].logo->setTargetPosition(
         -100 * Goblin::ScreenInfo::getGuiScale(),
         -100 * Goblin::ScreenInfo::getGuiScale(), GUIINITIAL_DEFAULT_UPDATE);
   teams[curTeam].logo->setTargetDimensions(
         TEAM_LOGO_SIZE * Goblin::ScreenInfo::getGuiScale(),
         TEAM_LOGO_SIZE * Goblin::ScreenInfo::getGuiScale(), 
         GUIINITIAL_DEFAULT_UPDATE);
   teams[curTeam].logo->setTargetPosition(
         Goblin::ScreenInfo::getHalfWindowWidth() -
         90 * Goblin::ScreenInfo::getGuiScale(),
         TEAM_LOGO_POS_Y * Goblin::ScreenInfo::getGuiScale(), 
         GUIINITIAL_DEFAULT_UPDATE);
 
   teamName->setText(teams[curTeam].info->name);
   setRegion(prevTeam);
}

/****************************************************************
 *                        verifyButtons                         *
 ****************************************************************/
int GuiInitial::verifyEvents(int mouseX, int mouseY, 
      bool leftButtonPressed, Field* field)
{
   int i;
   
   guiOptions->update();
   if(state == STATE_OPTIONS)
   {
      bool res = guiOptions->verifyEvents(mouseX, mouseY,
                                          leftButtonPressed);
      if(res)
      {
         /* Done with options, return to initial state */
         state = STATE_INITIAL;
         returnStatus = RETURN_OTHER;
         setButtons(true);
      }
   }
   else if(!guiOptions->isUpdating())
   {
      guiOptions->hide();
   }
   
   for(i=0; i<TOTAL_INITIAL_BUTTONS; i++)
   {
      button[i]->update();

      if(button[i]->verifyEvents(mouseX, mouseY, leftButtonPressed) ==
            IBUTTON_EVENT_PRESSED)
      {
         /* Normal button pressed */
         switch(i)
         {
            case BUTTON_TUTORIAL:
               state = STATE_TUTORIAL;
               returnStatus = RETURN_OTHER;
               setButtons(true);
            break;
            case BUTTON_TUTORIAL_CAMERA:
               state = STATE_QUIT;
               returnStatus = RETURN_TUTORIAL_CAMERA;
               setButtons(true);
            break;
            case BUTTON_TUTORIAL_GAME:
               state = STATE_QUIT;
               returnStatus = RETURN_TUTORIAL_GAME;
               setButtons(true);
            break;
            case BUTTON_QUICK_MATCH:
               state = STATE_QUICK_MATCH;
               returnStatus = RETURN_OTHER;
               setButtons(true);
            break;
#ifdef BTSOCCER_HAS_AI
            case BUTTON_QM_NEW:
               state = STATE_SELECTOR;
               returnStatus = RETURN_OTHER;
               modeSelected = RETURN_SINGLE_QUICK_MATCH;
               selTeamA = -1;
               selTeamB = -1;
               teamsToSelect = 2;
               setButtons(true);
            break;
#endif
            case BUTTON_QM_VERSUS:
               state = STATE_SELECTOR;
               returnStatus = RETURN_OTHER;
               modeSelected = RETURN_VERSUS_QUICK_MATCH;
               selTeamA = -1;
               selTeamB = -1;
               teamsToSelect = 2;
               setButtons(true);
            break;
            case BUTTON_LOAD:
               /* FIXME: when loading from cup! */
               returnStatus = RETURN_LOAD_QUICK_MATCH;
               state = STATE_QUIT;
               setButtons(true);
            break;
#ifdef BTSOCCER_HAS_CUP
            case BUTTON_CUP:
               state = STATE_CUP;
               returnStatus = RETURN_OTHER;
               setButtons(true);
            break;
#endif
            case BUTTON_CUP_NEW:
               state = STATE_SELECTOR;
               returnStatus = RETURN_OTHER;
               modeSelected = RETURN_NEW_CUP;
               selTeamA = -1;
               selTeamB = -1;
               teamsToSelect = 1;
               setButtons(true);
            break;
            case BUTTON_ONLINE:
               state = STATE_ONLINE;
               returnStatus = RETURN_OTHER;
               setButtons(true);
            break;
            case BUTTON_CREATE_SERVER:
               state = STATE_SELECTOR;
               returnStatus = RETURN_OTHER;
               modeSelected = RETURN_CREATE_SERVER;
               selTeamA = -1;
               selTeamB = -1;
               teamsToSelect = 1;
               setButtons(true);
            break;
            case BUTTON_CONNECT_SERVER:
               state = STATE_SELECTOR;
               returnStatus = RETURN_OTHER;
               modeSelected = RETURN_CONNECT_SERVER;
               selTeamA = -1;
               selTeamB = -1;
               teamsToSelect = 1;
               setButtons(true);
            break;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            case BUTTON_GAME_CENTER:
               state = STATE_SELECTOR;
               returnStatus = RETURN_OTHER;
               modeSelected = RETURN_CONNECT_GAME_CENTER,
               selTeamA = -1;
               selTeamB = -1;
               teamsToSelect = 1;
               setButtons(true);
            break;
#endif
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS &&\
    OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
            case BUTTON_QUIT:
               state = STATE_QUIT;
               returnStatus = RETURN_QUIT;
               setButtons(true);
            break;
#endif
            case BUTTON_OPTIONS:
            {
               returnStatus = RETURN_OTHER;
               state = STATE_OPTIONS;
               setButtons(true);
               guiOptions->show();
            }
            break;
            case BUTTON_CANCEL:
               if(state == STATE_OPTIONS)
               {
                  guiOptions->setButtonsHiddenPos();
                  guiOptions->getCurrentOptions();
               }
               state = STATE_INITIAL;
               returnStatus = RETURN_OTHER;
               setButtons(true);
            break;
            case BUTTON_NEXT:
            case BUTTON_PREVIOUS:
            {
               /* Inc or Dec current team */
               int prevTeam = curTeam;
               do
               {
                  if(i == BUTTON_NEXT)
                  {
                     curTeam = (curTeam + 1)%totalTeams;
                  }
                  else
                  {
                     curTeam = (curTeam - 1);
                     if(curTeam < 0)
                     {
                        curTeam = totalTeams-1;
                     }
                  }
               }while(curTeam == selTeamA);
               
               changeTeam(prevTeam);
            }
            break;
            case BUTTON_ACCEPT:
            {
               if(state == STATE_OPTIONS)
               {
                  /* Save options */
                  guiOptions->save(field);
                  /* Back to initial state */
                  state = STATE_INITIAL;
                  returnStatus = RETURN_OTHER;
                  setButtons(true);
                  guiOptions->setButtonsHiddenPos();
               }
               else
               {
                  /* Select the team */
                  startedSelectedAnimation = false;
                  if(selTeamA == -1)
                  {
                     selTeamA = curTeam;
                  }
                  else
                  {
                     selTeamB = curTeam;
                  }
                  teamsToSelect--;
                  if(teamsToSelect > 0)
                  {
                     /* More teams to select */
                     int prevTeam = curTeam;
                     curTeam = (curTeam + 1)%totalTeams;
                     /* Hide previous, show current logo */
                     teams[prevTeam].logo->setTargetDimensions(
                           90 * Goblin::ScreenInfo::getGuiScale(),
                           90 * Goblin::ScreenInfo::getGuiScale(), 
                           GUIINITIAL_DEFAULT_UPDATE);
                     teams[prevTeam].logo->setTargetPosition(
                        teamSel[0]->getPosX() + 
                        19* Goblin::ScreenInfo::getGuiScale(),
                        teamSel[0]->getPosY() + 
                        9 * Goblin::ScreenInfo::getGuiScale(), 
                        GUIINITIAL_DEFAULT_UPDATE);
                     teams[curTeam].logo->setTargetDimensions(
                           TEAM_LOGO_SIZE * Goblin::ScreenInfo::getGuiScale(),
                           TEAM_LOGO_SIZE * Goblin::ScreenInfo::getGuiScale(), 
                           30);
                     teams[curTeam].logo->setTargetPosition(
                           Goblin::ScreenInfo::getHalfWindowWidth() -
                           90 * Goblin::ScreenInfo::getGuiScale(),
                           TEAM_LOGO_POS_Y * 
                           Goblin::ScreenInfo::getGuiScale(), 
                           GUIINITIAL_DEFAULT_UPDATE);
                     setRegion(prevTeam);
                     teamName->setText(teams[curTeam].info->name);
                  }
                  else
                  {
                     /* No more teams to select. Done! */
                     teams[curTeam].logo->setTargetDimensions(
                           90 * Goblin::ScreenInfo::getGuiScale(),
                           90 * Goblin::ScreenInfo::getGuiScale(), 
                           GUIINITIAL_DEFAULT_UPDATE);
                     teams[curTeam].logo->setTargetPosition(
                        teamSel[1]->getPosX() + 
                        19 * Goblin::ScreenInfo::getGuiScale(),
                        teamSel[1]->getPosY() + 
                        9 * Goblin::ScreenInfo::getGuiScale(), 
                        GUIINITIAL_DEFAULT_UPDATE);
                     teamName->setText("");
                     state = STATE_QUIT;
                     returnStatus = modeSelected;
                     setButtons(false);
                  }
               }
            }
            break;
         }
      } 
   }

   /* Verify region buttons */
   for(i=0; i < totalRegions; i++)
   {
      regions[i]->update();

      if(regions[i]->verifyEvents(mouseX, mouseY, leftButtonPressed) == 
            IBUTTON_EVENT_PRESSED)
      {
         Region* reg = Regions::getRegion(i);

         /* Verify if current team isn't of region */
         if(reg != teams[curTeam].info->region)
         {
            /* change to the next team at region, if any */
            bool done=false;
            for(int j=((curTeam+1)%totalTeams); 
                 ((!done) && (j != curTeam)); 
                 (j = (j+1)%totalTeams) )
            {
               /* Make sure the team isn't selected yet */
               if( (teams[j].info->region == reg) &&
                   (j != selTeamA) )
               {
                  int prevTeam = curTeam;
                  curTeam = j;
                  changeTeam(prevTeam);
                  done = true;
               }
            }
         }
      }
   }

   /* Update other images */
   for(i=0; i<totalTeams; i++)
   {
      teams[i].logo->update();
   }
   teamName->update();
   teamNameImage->update();
   btsoccerLogo->update();
   vsImage->update();
   curSel->update();
   for(i=0; i < 2; i++)
   {
      teamSel[i]->update();
   }

   if(!isUpdating())
   {
      if( (returnStatus == RETURN_SINGLE_QUICK_MATCH) ||
          (returnStatus == RETURN_VERSUS_QUICK_MATCH) ||
          (returnStatus == RETURN_NEW_CUP) ||
          (returnStatus == RETURN_CREATE_SERVER) ||
          (returnStatus == RETURN_CONNECT_SERVER) )
      {
         if( (!teamSel[0]->isUpdating()) && (!startedSelectedAnimation) )
         {
            /* Set targets coordinates */
            Ogre::Real tgtX1, tgtX2, tgtY1, tgtY2, tgtVsX, tgtVsY;
            if(Goblin::ScreenInfo::getWindowHeight() > 700)
            {
               /* Normal screens */
               tgtX1 = Goblin::ScreenInfo::getWindowWidth() +
                           300 * Goblin::ScreenInfo::getGuiScale();
               tgtY1 = -150 * Goblin::ScreenInfo::getGuiScale();
               tgtY2 = tgtY1;
               tgtX2 = -300 * Goblin::ScreenInfo::getGuiScale();
               tgtVsX = Goblin::ScreenInfo::getHalfWindowWidth() -
                           32 * Goblin::ScreenInfo::getGuiScale();
               tgtVsY = -100 * Goblin::ScreenInfo::getGuiScale();
            }
            else
            {
               /*  Small screens */
               tgtX1 = Goblin::ScreenInfo::getWindowWidth() +
                           300 * Goblin::ScreenInfo::getGuiScale();
               tgtY1 = Goblin::ScreenInfo::getWindowHeight() +
                           300 * Goblin::ScreenInfo::getGuiScale();
               tgtX2 = tgtX1;
               tgtY2 = -300 * Goblin::ScreenInfo::getGuiScale();
               tgtVsX = Goblin::ScreenInfo::getWindowWidth() +
                           32 * Goblin::ScreenInfo::getGuiScale();
               tgtVsY = Goblin::ScreenInfo::getHalfWindowHeight() -
                           32 * Goblin::ScreenInfo::getGuiScale();
            }
            
            /* Must do the last update */
            startedSelectedAnimation = true;
            if(selTeamB >= 0)
            {
               /* Is selecting for a vs match */
               teamSel[0]->setTargetPosition(tgtX1, tgtY1, 
                  GUIINITIAL_DEFAULT_SLOW_UPDATE);
               teams[selTeamA].logo->setTargetPosition(tgtX1 +
                  14 * Goblin::ScreenInfo::getGuiScale(), tgtY1, 
                     GUIINITIAL_DEFAULT_SLOW_UPDATE);
               vsImage->setTargetPosition(tgtVsX, tgtVsY, 
                  GUIINITIAL_DEFAULT_SLOW_UPDATE);
               teamSel[1]->setTargetPosition(tgtX2, tgtY2, 
                  GUIINITIAL_DEFAULT_SLOW_UPDATE);
               teams[selTeamB].logo->setTargetPosition(tgtX2 + 14 *
                  Goblin::ScreenInfo::getGuiScale(), tgtY2, 
                     GUIINITIAL_DEFAULT_SLOW_UPDATE);
               return(RETURN_OTHER);
            }
            else
            {
               /* Is selecting for a cup or online match */
               teamSel[1]->setTargetPosition(tgtX2, tgtY2, 
                  GUIINITIAL_DEFAULT_SLOW_UPDATE);
               teams[selTeamA].logo->setTargetPosition(tgtX2 +
                  14 * Goblin::ScreenInfo::getGuiScale(), tgtY2, 
                     GUIINITIAL_DEFAULT_SLOW_UPDATE);
               return(RETURN_OTHER);
            }
         }
         else
         {
            /* Must put at "home" initial state */
            teamSel[0]->setPosition(-150 * Goblin::ScreenInfo::getGuiScale(),
                                    -150 * Goblin::ScreenInfo::getGuiScale());
            teamSel[1]->setPosition(-150 * Goblin::ScreenInfo::getGuiScale(),
                                    -150 * Goblin::ScreenInfo::getGuiScale());
            vsImage->setPosition(-100 * Goblin::ScreenInfo::getGuiScale(),
                                 -100 * Goblin::ScreenInfo::getGuiScale());
            if(selTeamA >= 0)
            {
               teams[selTeamA].logo->setPosition(
                                 -100 * Goblin::ScreenInfo::getGuiScale(),
                                 -100 * Goblin::ScreenInfo::getGuiScale());
            }
            if(selTeamB >= 0)
            {
               teams[selTeamB].logo->setPosition(
                                 -100 * Goblin::ScreenInfo::getGuiScale(),
                                 -100 * Goblin::ScreenInfo::getGuiScale());
            }
         }
      }
      int ret = returnStatus;
      returnStatus = RETURN_OTHER;
      return(ret);
   }

   return(RETURN_OTHER);
}

/****************************************************************
 *                        isUpdating                            *
 ****************************************************************/
bool GuiInitial::isUpdating()
{
   int i;
   if(btsoccerLogo->isUpdating())
   {
      return true;
   }
   if(guiOptions->isUpdating())
   {
      return true;
   }
   if(curSel->isUpdating())
   {
      return true;
   }
   if( (teamName->isUpdating()) || (teamNameImage->isUpdating()) )
   {
      return true;
   }
   for(i=0; i < 2; i++)
   {
      if(teamSel[i]->isUpdating())
      {
         return true;
      }
   }
   for(i=0; i<TOTAL_INITIAL_BUTTONS; i++)
   {
      if(button[i]->isUpdating())
      {
         return true;
      }
   }
   for(i=0; i < totalRegions; i++)
   {
      if(regions[i]->isUpdating())
      {
         return true;
      }
   }
   for(i=0; i<totalTeams; i++)
   {
      if(teams[i].logo->isUpdating())
      {
         return true;
      }
   }

   return false;
}

/****************************************************************
 *                          setButtons                          *
 ****************************************************************/
void GuiInitial::setButtons(bool setLogos)
{
   /* First, set all buttons "home" again */
   int i;
   for(i=0; i<TOTAL_INITIAL_BUTTONS; i++)
   {
      button[i]->setTargetPosition(-100 * Goblin::ScreenInfo::getGuiScale(),
                                   -100 * Goblin::ScreenInfo::getGuiScale());
   }
   for(i=0; i < totalRegions; i++)
   {
      regions[i]->setTargetPosition(-200 * Goblin::ScreenInfo::getGuiScale(),
                                    -200 * Goblin::ScreenInfo::getGuiScale());
   }
   curSel->setTargetPosition(-300 * Goblin::ScreenInfo::getGuiScale(),
                             -300 * Goblin::ScreenInfo::getGuiScale());
   teamName->setTargetPosition(-200 * Goblin::ScreenInfo::getGuiScale(),
                               -200 * Goblin::ScreenInfo::getGuiScale());
   teamNameImage->setTargetPosition(-200 * Goblin::ScreenInfo::getGuiScale(),
                                    -200 * Goblin::ScreenInfo::getGuiScale());
   if(setLogos)
   {
      vsImage->setPosition(-100 * Goblin::ScreenInfo::getGuiScale(),
                           -100 * Goblin::ScreenInfo::getGuiScale());
      for(i=0; i < 2; i++)
      {
         teamSel[i]->setTargetPosition(-
               150 * Goblin::ScreenInfo::getGuiScale(),
               -150 * Goblin::ScreenInfo::getGuiScale());
      }
      for(i=0; i<totalTeams; i++)
      {
         teams[i].logo->setTargetDimensions(1,1);
         teams[i].logo->setTargetPosition(
               -100 * Goblin::ScreenInfo::getGuiScale(),
               -100 * Goblin::ScreenInfo::getGuiScale());
      }
   }

   /* Next, set each related button to the screen */
   switch(state)
   {
      case STATE_INITIAL:
      {
         for(i=BUTTON_NONE+1; i<BUTTON_CUP_NEW; i++)
         {
            button[i]->setTargetPosition(
                  Goblin::ScreenInfo::getHalfWindowWidth() + buttonPosX[i] *
                  Goblin::ScreenInfo::getGuiScale(), buttonPosY[i] *
                  Goblin::ScreenInfo::getGuiScale(), 
                  GUIINITIAL_DEFAULT_SLOW_UPDATE);
         }
      }
      break;
      case STATE_TUTORIAL:
      {
         for(i=BUTTON_TUTORIAL_CAMERA; i<=BUTTON_TUTORIAL_GAME; i++)
         {
            button[i]->setTargetPosition(
                  Goblin::ScreenInfo::getHalfWindowWidth() + buttonPosX[i] *
                  Goblin::ScreenInfo::getGuiScale(), buttonPosY[i]
                  * Goblin::ScreenInfo::getGuiScale(), 
                  GUIINITIAL_DEFAULT_SLOW_UPDATE);
         }
         button[BUTTON_CANCEL]->setTargetPosition(
               Goblin::ScreenInfo::getHalfWindowWidth() +
               buttonPosX[BUTTON_CANCEL] -
               32*Goblin::ScreenInfo::getGuiScale(), buttonPosY[BUTTON_CANCEL]
               * Goblin::ScreenInfo::getGuiScale(), 
               GUIINITIAL_DEFAULT_SLOW_UPDATE);
      }
      break;
      case STATE_QUICK_MATCH:
      {
#ifdef BTSOCCER_HAS_AI
         for(i=BUTTON_QM_NEW; i<=BUTTON_LOAD; i++)
         {
            button[i]->setTargetPosition(
                  Goblin::ScreenInfo::getHalfWindowWidth() +
                  buttonPosX[i] * Goblin::ScreenInfo::getGuiScale(),
                  buttonPosY[i] * Goblin::ScreenInfo::getGuiScale(), 
                  GUIINITIAL_DEFAULT_SLOW_UPDATE);
         }
#else
         for(i=BUTTON_QM_VERSUS; i<=BUTTON_LOAD; i++)
         {
            button[i]->setTargetPosition(
                  Goblin::ScreenInfo::getHalfWindowWidth() +
                  buttonPosX[i]-32 * Goblin::ScreenInfo::getGuiScale(),
                  buttonPosY[i] * Goblin::ScreenInfo::getGuiScale(), 
                  GUIINITIAL_DEFAULT_SLOW_UPDATE);
         }
#endif
      }
      break;
      case STATE_CUP:
      {
         for(i=BUTTON_CUP_NEW; i<=BUTTON_LOAD; i++)
         {
#ifdef BTSOCCER_HAS_AI
            if( (i != BUTTON_QM_NEW) && (i != BUTTON_QM_VERSUS) )
#else
            if( (i != BUTTON_QM_VERSUS) )
#endif
            {
               button[i]->setTargetPosition(
                     Goblin::ScreenInfo::getHalfWindowWidth() +
                     buttonPosX[i] * Goblin::ScreenInfo::getGuiScale(),
                     buttonPosY[i] * Goblin::ScreenInfo::getGuiScale(), 
                     GUIINITIAL_DEFAULT_SLOW_UPDATE);
            }
         }
      }
      break;
      case STATE_ONLINE:
      {
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
         button[BUTTON_CANCEL]->setTargetPosition(
               Goblin::ScreenInfo::getHalfWindowWidth() +
                     buttonPosX[BUTTON_CANCEL],
               buttonPosY[BUTTON_CANCEL] * Goblin::ScreenInfo::getGuiScale(), 
               GUIINITIAL_DEFAULT_SLOW_UPDATE);
         for(i=BUTTON_CREATE_SERVER; i<=BUTTON_GAME_CENTER; i++)
         {
            button[i]->setTargetPosition(
                  Goblin::ScreenInfo::getHalfWindowWidth() + (buttonPosX[i] *
                  Goblin::ScreenInfo::getGuiScale()) - (35 *
                  Goblin::ScreenInfo::getGuiScale()), buttonPosY[i] *
                  Goblin::ScreenInfo::getGuiScale(),
                  GUIINITIAL_DEFAULT_SLOW_UPDATE);
         }
#else
         button[BUTTON_CANCEL]->setTargetPosition(
               Goblin::ScreenInfo::getHalfWindowWidth() +
               buttonPosX[BUTTON_CANCEL] -
               32*Goblin::ScreenInfo::getGuiScale(), buttonPosY[BUTTON_CANCEL]
               * Goblin::ScreenInfo::getGuiScale(), 50);
         for(i=BUTTON_CREATE_SERVER; i<=BUTTON_CONNECT_SERVER; i++)
         {
            button[i]->setTargetPosition(
                  Goblin::ScreenInfo::getHalfWindowWidth() +
                     buttonPosX[i] * Goblin::ScreenInfo::getGuiScale(),
                     buttonPosY[i] * Goblin::ScreenInfo::getGuiScale(),
                     GUIINITIAL_DEFAULT_SLOW_UPDATE);
         }
#endif
      }
      break;
      case STATE_SELECTOR:
      {
         button[BUTTON_CANCEL]->setTargetPosition(
               Goblin::ScreenInfo::getHalfWindowWidth() + 37 *
               Goblin::ScreenInfo::getGuiScale(), buttonPosY[BUTTON_ACCEPT] *
               Goblin::ScreenInfo::getGuiScale(), 
               GUIINITIAL_DEFAULT_SLOW_UPDATE);
         button[BUTTON_NEXT]->setTargetPosition(
               Goblin::ScreenInfo::getHalfWindowWidth() +
               buttonPosX[BUTTON_NEXT] * Goblin::ScreenInfo::getGuiScale(),
               buttonPosY[BUTTON_NEXT] * Goblin::ScreenInfo::getGuiScale(), 
               GUIINITIAL_DEFAULT_SLOW_UPDATE);
         button[BUTTON_PREVIOUS]->setTargetPosition(
               Goblin::ScreenInfo::getHalfWindowWidth() +
               buttonPosX[BUTTON_PREVIOUS] * Goblin::ScreenInfo::getGuiScale(),
               buttonPosY[BUTTON_PREVIOUS] * Goblin::ScreenInfo::getGuiScale(),
               GUIINITIAL_DEFAULT_SLOW_UPDATE);
         button[BUTTON_ACCEPT]->setTargetPosition(
               Goblin::ScreenInfo::getHalfWindowWidth() +
               buttonPosX[BUTTON_ACCEPT] * Goblin::ScreenInfo::getGuiScale(),
               buttonPosY[BUTTON_ACCEPT] *
               Goblin::ScreenInfo::getGuiScale(),
               GUIINITIAL_DEFAULT_SLOW_UPDATE);

         /* Background teams to select */
         if(teamsToSelect == 2)
         {
            /* Will select 2 teams for a vs match */
            if(Goblin::ScreenInfo::getWindowHeight() > 700)
            {
               teamSel[0]->setTargetPosition(
                  Goblin::ScreenInfo::getHalfWindowWidth() -
                  164 * Goblin::ScreenInfo::getGuiScale(),
                  510 * Goblin::ScreenInfo::getGuiScale(), 
                  GUIINITIAL_DEFAULT_UPDATE);
               vsImage->setTargetPosition(
                  Goblin::ScreenInfo::getHalfWindowWidth() -
                  32 * Goblin::ScreenInfo::getGuiScale(),
                  540 * Goblin::ScreenInfo::getGuiScale(), 
                  GUIINITIAL_DEFAULT_UPDATE);
               teamSel[1]->setTargetPosition(
                  Goblin::ScreenInfo::getHalfWindowWidth() +
                  36 * Goblin::ScreenInfo::getGuiScale(),
                  510 * Goblin::ScreenInfo::getGuiScale(), 
                  GUIINITIAL_DEFAULT_UPDATE);
            }
            else
            {
               teamSel[0]->setTargetPosition(0,
                  Goblin::ScreenInfo::getHalfWindowHeight() -
                  (128+40) * Goblin::ScreenInfo::getGuiScale(), 
                  GUIINITIAL_DEFAULT_UPDATE);
               vsImage->setTargetPosition(32*Goblin::ScreenInfo::getGuiScale(),
                  Goblin::ScreenInfo::getHalfWindowHeight()-
                  32*Goblin::ScreenInfo::getGuiScale(),
                  GUIINITIAL_DEFAULT_UPDATE);
               teamSel[1]->setTargetPosition(0,
                  Goblin::ScreenInfo::getHalfWindowHeight()+
                  40*Goblin::ScreenInfo::getGuiScale(),
                  GUIINITIAL_DEFAULT_UPDATE);
            }
         }
         else
         {
            /* Will select a single team to a cup or online play */
            if(Goblin::ScreenInfo::getWindowHeight() > 700)
            {
               teamSel[1]->setTargetPosition(
                     Goblin::ScreenInfo::getHalfWindowWidth() -
                  64 * Goblin::ScreenInfo::getGuiScale(),
                  510 * Goblin::ScreenInfo::getGuiScale(), 
                  GUIINITIAL_DEFAULT_UPDATE);
            }
            else
            {
               teamSel[1]->setTargetPosition(0,
                  Goblin::ScreenInfo::getHalfWindowHeight()-
                  64*Goblin::ScreenInfo::getGuiScale(),
                  GUIINITIAL_DEFAULT_UPDATE);
            }
         }
         curSel->setTargetPosition(Goblin::ScreenInfo::getHalfWindowWidth() -
                  128 * Goblin::ScreenInfo::getGuiScale(),
                  TEAM_LOGO_POS_Y * Goblin::ScreenInfo::getGuiScale() -
                     10 * Goblin::ScreenInfo::getGuiScale(), 
                     GUIINITIAL_DEFAULT_UPDATE);
         
         teamName->setTargetPosition(Goblin::ScreenInfo::getHalfWindowWidth() +
                  6 * Goblin::ScreenInfo::getGuiScale(),
                  TEAM_LOGO_POS_Y * Goblin::ScreenInfo::getGuiScale() -
                     46 * Goblin::ScreenInfo::getGuiScale(), 
                     GUIINITIAL_DEFAULT_UPDATE);
         teamNameImage->setTargetPosition(
               Goblin::ScreenInfo::getHalfWindowWidth() -
                  58 * Goblin::ScreenInfo::getGuiScale(),
                  TEAM_LOGO_POS_Y * Goblin::ScreenInfo::getGuiScale() -
                     70 * Goblin::ScreenInfo::getGuiScale(), 
                     GUIINITIAL_DEFAULT_UPDATE);

         curTeam = 0;
         teams[curTeam].logo->setTargetDimensions(
               TEAM_LOGO_SIZE * Goblin::ScreenInfo::getGuiScale(),
               TEAM_LOGO_SIZE * Goblin::ScreenInfo::getGuiScale(), 
               GUIINITIAL_DEFAULT_UPDATE);
         teams[curTeam].logo->setTargetPosition(
               Goblin::ScreenInfo::getHalfWindowWidth() - 
               90 * Goblin::ScreenInfo::getGuiScale(),
               TEAM_LOGO_POS_Y * Goblin::ScreenInfo::getGuiScale(), 
               GUIINITIAL_DEFAULT_UPDATE);
         teamName->setText(teams[curTeam].info->name);
         setRegion();

         /* Show regions */
         int x = Goblin::ScreenInfo::getWindowWidth() - 
            178 * Goblin::ScreenInfo::getGuiScale();
         int y = ((Goblin::ScreenInfo::getWindowHeight() > 700)?120:40) *
                     Goblin::ScreenInfo::getGuiScale();
         for(i=0; i < totalRegions; i++)
         {
            regions[i]->setTargetPosition(x, y, GUIINITIAL_DEFAULT_UPDATE);
            y += 90 * Goblin::ScreenInfo::getGuiScale();
         }
      }
      break;
      case STATE_OPTIONS:
      {
         button[BUTTON_CANCEL]->setTargetPosition(
               Goblin::ScreenInfo::getHalfWindowWidth() + 
               37 * Goblin::ScreenInfo::getGuiScale(),
               buttonPosY[BUTTON_ACCEPT] * Goblin::ScreenInfo::getGuiScale() +
               60 * Goblin::ScreenInfo::getGuiScale(), 
               GUIINITIAL_DEFAULT_SLOW_UPDATE);
         button[BUTTON_ACCEPT]->setTargetPosition(
               Goblin::ScreenInfo::getHalfWindowWidth() + 
               buttonPosX[BUTTON_ACCEPT] *
               Goblin::ScreenInfo::getGuiScale(),
               buttonPosY[BUTTON_ACCEPT] * Goblin::ScreenInfo::getGuiScale() +
               60 * Goblin::ScreenInfo::getGuiScale(), 
               GUIINITIAL_DEFAULT_SLOW_UPDATE);
      }
      break;
      case STATE_QUIT:
         /* Remove the logo */
         btsoccerLogo->setTargetPosition(
               Goblin::ScreenInfo::getHalfWindowWidth() -
               128 * Goblin::ScreenInfo::getGuiScale(),
               -200 * Goblin::ScreenInfo::getGuiScale());
      break;
   }
}



/****************************************************************
 *                             hide                             *
 ****************************************************************/
void GuiInitial::hide()
{
   int i;
   for(i=0; i < 2; i++)
   {
      teamSel[i]->hide();
   }
   for(i=0; i < TOTAL_INITIAL_BUTTONS; i++)
   {
      button[i]->hide();
   }
   loadingBar->hide();
   ogreOverlay->hide();
}

/****************************************************************
 *                             show                             *
 ****************************************************************/
void GuiInitial::show()
{
   int i;
   for(i=0; i < 2; i++)
   {
      teamSel[i]->show();
   }
   for(i=0; i < TOTAL_INITIAL_BUTTONS; i++)
   {
      button[i]->show();
   }
   ogreOverlay->show();
   state = STATE_INITIAL;
   setButtons(true);
   btsoccerLogo->setTargetPosition(Goblin::ScreenInfo::getHalfWindowWidth() -
         128 * Goblin::ScreenInfo::getGuiScale(), 0, 
         GUIINITIAL_DEFAULT_SLOW_UPDATE);
}

/****************************************************************
 *                        setRandomTeamA                        *
 ****************************************************************/
void GuiInitial::setRandomTeamA()
{
   selTeamA = (int)(totalTeams * (rand() / (RAND_MAX + 1.0)));
}

/****************************************************************
 *                           getTeamA                           *
 ****************************************************************/
Ogre::String GuiInitial::getTeamA()
{
   if( (selTeamA >= 0) && (selTeamA < totalTeams) )
   {
      return(teams[selTeamA].info->fileName);
   }
   return("");
}

/****************************************************************
 *                           getTeamB                           *
 ****************************************************************/
Ogre::String GuiInitial::getTeamB()
{
   if( (selTeamB >= 0) && (selTeamB < totalTeams) )
   {
      return(teams[selTeamB].info->fileName);
   }
   return("");
}

