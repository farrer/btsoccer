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

#ifndef _btsoccer_gui_saves_h
#define _btsoccer_gui_saves_h

#include <goblin/ibutton.h>
#include <goblin/textbox.h>
#include <goblin/screeninfo.h>
#include <OGRE/OgreRenderWindow.h>

namespace BtSoccer
{

#define SAVE_SLOTS    12

class GuiSaves
{
   public:
      
      /*! Constructor
       * \param listFile -> list file to load
       * \param sufix -> sufix used (.qms for quick matches, 
       *                              .cps for cups ) */
      GuiSaves(Ogre::String listFile, Ogre::String sufix);
      /*! Destructor */
      ~GuiSaves();

      enum GuiSavesActions
      {
         ACTION_NONE=0,
         ACTION_CANCEL,
         ACTION_LOAD,
         ACTION_SAVE
      };

      /*! Verify Events at the GUI
       * \param mouseButton -> current mouse button state
       * \param mouseX -> current mouseX position 
       * \param mouseY -> current mouseY position
       * \return GuiSavesActions constant */
      int verifyEvents(int mouseX, int mouseY, bool leftButtonPressed);

      /*! Hide the gui */
      void hide();

      /*! Show the gui 
       * \param save -> true if will use to save a game. false if will load
       * \note: on load mode, will describe saved games info. */
      void show(bool save);

      /*! If is at save mode or load mode
       * \return true if at save mode. */
      bool isSaveMode();

      /*! Get the selected filename
       * \return filename selected to load/save or "" if no file selected. */
      Ogre::String getSelectedFileName();

      /*! Save current filled slots filenames to list */
      void saveCurrentSlotFiles();

   protected:

      /*! Load current filled slots filenames from list */
      void loadCurrentSlotFiles();

      Goblin::Ibutton* slotButton[SAVE_SLOTS]; /**< Each save slot button */
      Goblin::TextBox* slotDate[SAVE_SLOTS];   /**< Each date text box */
      Goblin::TextBox* slotTime[SAVE_SLOTS];   /**< Each time text box */
      Goblin::Ibutton* cancelButton;           /**< Cancel button */
      Ogre::Overlay* ogreOverlay; /**< Overlay used */

      Ogre::String curSelectedFile; /**< Current selected file */
      Ogre::String curSufix;        /**< Current sufix */
      Ogre::String curListFile;     /**< Current list file*/


      Ogre::String curSlotFile[SAVE_SLOTS];    /**< Current slot file */
      Ogre::String curSlotDate[SAVE_SLOTS];    /**< Current slot date */
      Ogre::String curSlotTime[SAVE_SLOTS];    /**< Current slot time */
      Ogre::String curSlotInfo[SAVE_SLOTS];    /**< Current slot info */

      bool saving; /**< If will save or load */
      int returnStatus; /**< Internal return status */
};

}

#endif

