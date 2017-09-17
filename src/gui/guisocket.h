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

#ifndef btsoccer_gui_socket_h
#define btsoccer_gui_socket_h

#include <goblin/ibutton.h>
#include <goblin/textbox.h>
#include <goblin/screeninfo.h>
#include <OGRE/OgreRenderWindow.h>

namespace BtSoccer
{

/*! Screen for server/client creation, where user will set the port and,
 * if needed, address to connect to. */
class GuiSocket
{
   public:
   
      enum
      {
         /*! Nothing happened */
         RETURN_NOTHING,
         /*! Other internal action */
         RETURN_OTHER,
         /*! Clicked to create the server */
         RETURN_CREATE,
         /*! Clicked to create the client and connect to a server */
         RETURN_CONNECT,
         /*! Clicked to cancel creation/connection proccess. */
         RETURN_CANCEL
      };
   
      /*! Create the GuiSocket screen.
       * @param server if creating gui for server creation. False for client. */
      GuiSocket(bool server);
      /*! Destructor */
      ~GuiSocket();
   
      /*! Verify Events at the GUI
       * \param mouseButton -> current mouse button state
       * \param mouseX -> current mouseX position
       * \param mouseY -> current mouseY position
       * \return return constant */
      int verifyEvents(int mouseX, int mouseY, bool leftButtonPressed);
   
      /*! Get the selected address. */
      Ogre::String getAddress();
      /*! Get the selected port */
      unsigned short int getPort();
   
      /*! Make all elements visible */
      void show();
   
   protected:
   
      /*! Hide [all] elements */
      void hide(bool all);
      /*! True when no gui element is updating. */
      bool isStable();
   
      bool isServer; /**< if is server or not */
   
      /*! Convert value to String. */
      Ogre::String getValue(int value);
   
      /*! Update value to text box */
      void updateTextValue(int i, int value);
   
      Goblin::Ibutton* buttonInc[5]; /**< increment buttons */
      int ipPart[4]; /**< each current ip part (xxx.xxx.xxx.xxx) */
      unsigned short int port; /**< current port */
      Goblin::Ibutton* buttonDec[5]; /**< decrement buttons */
   
      Goblin::TextBox* textValues[5]; /**< Each values */
   
      Goblin::Ibutton* buttonOk;  /**< Ok (create/connect) buttons */
      Goblin::Ibutton* buttonCancel; /**< Cancel buttons */
   
      Goblin::Image* helpImage; /**< Image used for help */
      Goblin::TextBox* helpText; /**< Text used for help */
   
      Ogre::Overlay* ogreOverlay; /**< Current overlay */
   
      int incValue; /**< Increment value */
      int delayBeforeRepeat; /** Delay variable to use before repeat inc/dec */
   
      int returnStatus;             /**< Potential return value after stable */
};
   
   
}

#endif
