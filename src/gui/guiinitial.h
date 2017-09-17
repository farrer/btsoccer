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

#ifndef _btsoccer_gui_initial_h
#define _btsoccer_gui_initial_h

#include <goblin/ibar.h>
#include <goblin/ibutton.h>
#include <goblin/textbox.h>
#include <goblin/screeninfo.h>
#include <OGRE/OgreRenderWindow.h>

#include "../engine/teams.h"
#include "../engine/field.h"

#include "guioptions.h"

namespace BtSoccer
{

/*! The GuiInitialTeamInfo reatains team descriptions to show */
class GuiInitialTeamInfo
{
   public:

      /*! Constructor */
      GuiInitialTeamInfo();
      /*! Destructor */
      ~GuiInitialTeamInfo();

      Goblin::Image* logo;    /**< Logo */
      TeamInfo* info;         /**< The real info */
};

/*! The GuiInitial class controlls all initial selctions made by users
 * before starting the game itself. Here it selects the game mode,
 * load games, select teams, change options, etc. */
class GuiInitial
{
   public:

      enum GuiReturnValues
      {
         RETURN_OTHER=0,
         RETURN_SINGLE_QUICK_MATCH,
         RETURN_VERSUS_QUICK_MATCH,
         RETURN_LOAD_QUICK_MATCH,
         RETURN_NEW_CUP,
         RETURN_LOAD_CUP,
         RETURN_CREATE_SERVER,
         RETURN_CONNECT_SERVER,
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
         RETURN_CONNECT_GAME_CENTER,
#endif
         RETURN_OPTIONS,
         RETURN_TUTORIAL_CAMERA,
         RETURN_TUTORIAL_GAME,
         RETURN_QUIT
      };
      
      enum GuiState
      {
         STATE_INITIAL,
         STATE_QUICK_MATCH,
         STATE_CUP,
         STATE_SELECTOR,
         STATE_ONLINE,
         STATE_OPTIONS,
         STATE_TUTORIAL,
         STATE_QUIT
      };

      enum InitialButtons
      {
         BUTTON_NONE=-1,
         BUTTON_TUTORIAL,
         BUTTON_QUICK_MATCH,
#ifdef BTSOCCER_HAS_CUP
         BUTTON_CUP,
#endif
         BUTTON_ONLINE,
         BUTTON_OPTIONS,
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS &&\
    OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
         BUTTON_QUIT,
#endif
         BUTTON_CUP_NEW,
#ifdef BTSOCCER_HAS_AI
         BUTTON_QM_NEW,
#endif
         BUTTON_QM_VERSUS,
         BUTTON_CANCEL,
         BUTTON_LOAD,
         BUTTON_SAVE,
         BUTTON_NEXT,
         BUTTON_PREVIOUS,
         BUTTON_ACCEPT,
         BUTTON_TUTORIAL_CAMERA,
         BUTTON_TUTORIAL_GAME,
         BUTTON_CREATE_SERVER,
         BUTTON_CONNECT_SERVER,
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
         BUTTON_GAME_CENTER,
#endif
         TOTAL_INITIAL_BUTTONS
      };

      /*! Constructor */
      GuiInitial();
      
      /*! Destructor */
      ~GuiInitial();

      /*! Verify Button press at the controller
       * \return -> GuiReturnValues constant, with button selected, if any */
      int verifyEvents(int mouseX, int mouseY, bool leftButtonPressed,
                       Field* field);

      /*! Hide */
      void hide();

      /*! Display */
      void show();

      /*! Get first team filename */
      Ogre::String getTeamA();
      /*! Get second team filename */
      Ogre::String getTeamB();
   
      /*! Set a random team to play with. */
      void setRandomTeamA();
   
      /*! Set the current loading percentual at load action. */
      void setLoadingPercentual(float perc);

   protected:

      /*! Verify if some button is updating position/size */
      bool isUpdating();
      /*! Set buttons target positions, according to current state */
      void setButtons(bool setLogos);

      /*! hide prevTeam logo and show curTeam logo on selector screen */
      void changeTeam(int prevTeam);
      /*! set active region icon to be active, and inactivate previous one
       * \param prevTeam -> previous team ID or -1 if none */
      void setRegion(int prevTeam=-1);

      Goblin::Image* btsoccerLogo;       /**< Image with BTSOCCER logo */

      Goblin::Ibutton* button[TOTAL_INITIAL_BUTTONS];/**< Buttons used*/
      Ogre::Overlay* ogreOverlay;   /**< The overlay */
   
      BtSoccer::GuiOptions* guiOptions;      /**< The options GUI */

      int state;                    /**< Current GuiInitial state  */
      int returnStatus;             /**< Potential return value after stable */

      Goblin::Ibar* loadingBar;     /**< the bar for force input */
      Goblin::Ibutton** regions;    /**< Regions Buttons */
      Goblin::Image* teamNameImage; /**< Team name image */
      Goblin::TextBox* teamName;    /**< Team name text box */
      Goblin::Image* curSel;        /**< Current selected background image */
      Goblin::Image* teamSel[2];    /**< Selected teams icons */
      Goblin::Image* vsImage;       /**< The "VS" image */
      bool startedSelectedAnimation;/**< Flag for when already started final
                                         selection animation. */
      GuiInitialTeamInfo* teams;    /**< Teams */
      int totalRegions;             /**< Total regions on game */
      int totalTeams;               /**< Total teams loaded */
      int curTeam;                  /**< Current team */
      int teamsToSelect;            /**< Number of teams to select */
      int selTeamA;                 /**< Current selected teamA */
      int selTeamB;                 /**< Current selected teamB */
      int modeSelected;             /**< Current selected mode */
};

}

#endif

