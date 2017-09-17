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

#ifndef _btsoccer_tutorial_h
#define _btsoccer_tutorial_h

#include "btsoccer.h"
#include "../physics/bulletlink.h"
#include <goblin/ibar.h>
#include <goblin/image.h>
#include <goblin/ibutton.h>
#include <goblin/textbox.h>
#include <kobold/timer.h>

namespace BtSoccer
{

#define TUTORIAL_TOTAL_IMAGES   13
   
enum
{
   TUTORIAL_CAMERA,
   TUTORIAL_CAMERA_MOVE,
   TUTORIAL_CAMERA_MOVE_ACT,
   TUTORIAL_CAMERA_ZOOM_IN,
   TUTORIAL_CAMERA_ZOOM_IN_ACT,
   TUTORIAL_CAMERA_ZOOM_OUT,
   TUTORIAL_CAMERA_ZOOM_OUT_ACT,
   TUTORIAL_CAMERA_ROTATE_LEFT,
   TUTORIAL_CAMERA_ROTATE_LEFT_ACT,
   TUTORIAL_CAMERA_ROTATE_RIGHT_ACT,
   TUTORIAL_CAMERA_ROTATE_UP,
   TUTORIAL_CAMERA_ROTATE_UP_ACT,
   TUTORIAL_CAMERA_ROTATE_DOWN_ACT,
   TUTORIAL_GAME,
   TUTORIAL_DISK_MOVE_SEL,
   TUTORIAL_DISK_MOVE_DIREC,
   TUTORIAL_DISK_MOVE_FORCE,
   TUTORIAL_DISK_MOVE_ACT,
   TUTORIAL_DISK_BALL,
   TUTORIAL_DISK_BALL_ACT,
   TUTORIAL_DISK_FOUL,
   TUTORIAL_DISK_FOUL_WAIT,
   TUTORIAL_DISK_FOUL_ACT,
   TUTORIAL_GOAL_SHOOTS,
   TUTORIAL_GOAL_KEEPER_MOVE,
   TUTORIAL_GOAL_KEEPER_ACT,
   TUTORIAL_GOAL_KEEPER_ACT_WAIT,
   TUTORIAL_BALL_INNER_OWN_AREA,
   TUTORIAL_BALL_INNER_OWN_AREA_WAIT,
   TUTORIAL_BALL_INNER_OWN_AREA_ACT,
   TOTAL_TUTORIALS
};
   
/*! The tutorial class implements an interface to teach users
 * some useful informations about how to play the game.
 *   Tutorial types:
 *
 *   1) Camera
 *      A) Move Camera on field.
 *      B) Zoom Camera
 *      C) Rotate Camera.
 *
 *   2) Gameplay
 *      A) Move disk
 *      B) Fouls (tell it also happens for throw-ins goals, and 
           corner kicks):
 *         -> positionate disk
 *         -> act -> just one act to disk
 *      C) Consecutive moves (3), team moves (10);
 *      D) Goal Shoots
 *      E) Goal Keeper positionate;
 *      F) Ball inner own area; */
class Tutorial
{
   public:
      /*! Constructor */
      Tutorial(Core* core, BtSoccer::Team** teamA,
            BtSoccer::Team** teamB, BulletDebugDraw* debugDraw);
      /*! Destructor */
      ~Tutorial();
   
      /*! Set the tutorial mode
       * \param tutorial id of tutorial to be active */
      void set(int tutorial);
   
      /*!  Update current tutorial.
       * \return false when the tutorial is all over or canceled. */
      bool update(int mouseX, int mouseY, bool leftButtonPressed);
   
      /*! If disk input is enabled */
      bool isDiskInputEnabled();
      /*! If disk position input is enabled */
      bool isDiskPositionInputEnabled();
      /*! If goal keeper position input is enabled */
      bool isGoalKeeperPositionInputEnabled();

   protected:
   
      /*! Load things at current load status */
      void doLoading();
   
      /*! Called when tutorial is over or canceled, to
       * "make things work as usual" again. */
      void finish();
   
      /*! Update the gui elements. */
      void updateGuiElements();
   
      /*! Set tutorial texts and titles, displaying them. */
      void setTutorialText(Ogre::String title, Ogre::String textA,
                           Ogre::String textB);
   
      /*! Hide all lesson images */
      void hideAllLessonImages();
      /*! Show a lesson image */
      void showLessonImage(int i);
   
      /*! Show GUI of lesson description */
      void showLessonDescription();
      /*! Show GUI of tutorial action */
      void showTutorialActionGui();
   
      /*! Delete current existent teams */
      void deleteCurrentTeams();
   
      /*! Verify if clicked the accept button */
      bool verifyDescriptionAccept(int mouseX, int mouseY,
               bool leftButtonPressed);
   
      /*! Verify if ball entered up or down goal.
       * @param up true to up goal, false for down 
       * \return true if entered. */
      bool verifyBallEnteredGoal(bool up);
   
      Core* gameCore;   /**< Pointer to used game core */
      int curTutorial;  /**< Current tutorial */
      int curTutorialInit; /**< Initial 'state' of current tutorial */
      //Ogre::Vector3 pos;  /**< Position related to each tutorial */
      //Ogre::Vector3 angle; /**< Angle related to each tutorial */
      Ogre::Real value;   /**< A value related to each tutorial */
      int prevNumberOfDisks;  /**< Previous number of disks per team */
      BtSoccer::Team** teams[2]; /**< pointers to teams pointers */
      bool diskInput;  /**< If disk input is enabled */
      bool diskPositionInput; /**< If disk position input is enabled */
      bool goalKeeperPositionInput; /**< if gk input is enabled */
   
      Goblin::Image* back;          /**< background lesson image */
      Goblin::TextBox* lessonTitle; /**< lesson title text */
      Goblin::TextBox* lessonText[2]; /**< lesson texts */
   
      Goblin::Image* lessonImage[TUTORIAL_TOTAL_IMAGES]; /**< images */
   
      Goblin::Ibutton* btAccept;  /**< The accept button  */
      Goblin::Ibutton* btRestart; /**< Restart tutorial button */
      Goblin::Ibutton* btQuit;  /**< Quit tutorial button */
   
      Goblin::Ibar* progressBar; /**< The loading progress bar */
      int loadingState;          /**< Current Loading state. -1 done. */
   
      Ogre::Overlay* overlay; /**< Overlay used in tutorial */
   
      Kobold::Timer timer; /**< A timer to wait a few. */
   
      bool definedDesc; /**< If lesson description were defined */

      BulletDebugDraw* debugDraw; /**< For debug renders */
   
};
   
}

#endif
