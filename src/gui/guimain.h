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


#ifndef _btsoccer_main_gui_h
#define _btsoccer_main_gui_h

#include <goblin/ibar.h>
#include <goblin/ibutton.h>
#include <goblin/textbox.h>
#include <goblin/model3d.h>
#include <goblin/screeninfo.h>
#include <OGRE/OgreRenderWindow.h>

namespace BtSoccer
{

/*! The mainGui have all main interactive GUI needed by btsoccer */
class GuiMain
{
   public:

      enum
      {
         BUTTON_SHOOT=0,
         BUTTON_REPLAY,
         BUTTON_PAUSE,
         BUTTON_ACCEPT,
         TOTAL_BUTTONS,
         EVENT_NONE,
         EVENT_ON_PRESS
      };
   
      enum
      {
         STATE_VISIBLE,
         STATE_ACCEPT_BUTTON,
         STATE_HIDDEN
      };

      /*! Costructor */
      GuiMain(Ogre::Overlay* overlay, Ogre::SceneManager* sceneManager);
      /*! Destructor */
      ~GuiMain();

      /*! Verify Events at the main GUI
       * \param mouseButton -> current mouse button state
       * \param mouseX -> current mouseX position 
       * \param mouseY -> current mouseY position
       * \return button constant or NO_EVENTS */
      int verifyEvents(int mouseX, int mouseY, bool leftButtonPressed);
   
      /*! Get the event ocurred at last verifyEvents call
       * \return button constant or NO_EVENTS */
      int getLastEvent();

      /*! Hide the main gui */
      void hide();

      /*! Show the main gui */
      void show();
  
      /*! Restore the main GUI to a previous state (ie: the state before last
       * set on it). 
       * \note: usually called on game resume. */
      void restoreState();
   
      /*! Display the accept button */
      void showAcceptButton();
      /*! Hide the accept button */
      void hideAcceptButton();

      /*! If the main gui is active */
      bool isActive();

      /*! Update guiMain states (positions) */
      void update();
   
      /*! Set the current force fo show on the bar
       * \param curForce current force
       * \param maxForce max force
       * \param dX force vector X component
       * \param dZ force vector Z component
       * \param angle force vector angle
       * \param cX disk center X position
       * \param cZ disk center Z position
       * \param r disk radius */
      void setCurrentForce(float curForce, float maxForce,
               float dX, float dZ, Ogre::Degree angle,
               float cX, float cZ, float r);
      /*! Hide the force bar when not using it anymore */
      void hideForceBar();
   
      /*! Set current game as online */
      void setOnlineGame();
      /*! Set current game as local */
      void setLocalGame();

   protected:
      /*! Set button positions on hide state */
      void setHidePositions();
      /*! Set button positions on visible state */
      void setPositions();
   
      Goblin::Ibar* forceBar; /**< the bar for force input */
      Goblin::Model3d* forceDir; /**< the force direction */
   
      int lastEvent; /**< Last event occurred */

      Goblin::Ibutton* buttons[TOTAL_BUTTONS];  /**< The buttons */
      bool active;  /**< if the gui is active or not */
   
      bool onlineGame; /**< if current game is online or local */
   
      int curState; /**< Current state */
      int previousState; /**< Last state */
   
};

}

#endif

