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


#ifndef _btsoccer_stats_h
#define _btsoccer_stats_h

#include <goblin/ibutton.h>
#include <goblin/textbox.h>
#include <kobold/timer.h>
#include <OGRE/OgreRenderWindow.h>

namespace BtSoccer
{

#define BTSOCCER_TOTAL_STATS        7
#define BTSOCCER_TOTAL_TEXT_STATS   BTSOCCER_TOTAL_STATS*2

/* The Stats class keep statistics about a Match, usually being show
 * at halftime and at match's end. */
class Stats
{
    public:
       /*! Create the Stas, loading images, etc. */
       static void init();
       /*! Destroy the Stats, unloading image, etc. */
       static void finish();

       /*! Verify Events at the GUI
        * \param mouseX -> current mouseX position 
        * \param mouseY -> current mouseY position
        * \param leftButtonPressed current press status
        * \param useTimer if should use a timer for the stats screen be active.
        * \return true when done with the Stats Screen */
       static bool verifyEvents(int mouseX, int mouseY, bool leftButtonPressed,
            bool useTimer);

       /*! Clear all current statistics */
       static void clear();

       /*! Set current match teams */
       static void setTeams(Ogre::String teamA, Ogre::String teamB);

       /*! Tell a foul occurred
        * \param teamA -> true if teamA was the infractor */
       static void foul(bool teamA);
       /*! Tell a goal shoot attempt occurred
        * \param teamA -> true if teamA was the shooter */
       static void goalShoot(bool teamA);
       /*! Tell a corner occurred
        * \param teamA -> true if teamA was the corner */
       static void corner(bool teamA);
       /*! Tell a throwIn occurred
        * \param teamA -> true if teamA will throw in */
       static void throwIn(bool teamA);
       /*! Tell a goalKick occurred
        * \param teamA -> true if teamA was the goalkeeper owner */
       static void goalKick(bool teamA);
       /*! Tell a penalty occurred
        * \param teamA -> true if teamA was the penalty shooter */
       static void penalty(bool teamA);
       /*! Tell a move happened
        * \param teamA -> true if teamA moved */
       static void moved(bool teamA);

       /*! Show statistics on screen */
       static void show();
       /*! hide the statistics screen */
       static void hide();

       /*! get number of fouls for team */
       static int getFouls(bool teamA);
       /*! get number of goal shoots for team */
       static int getGoalShoots(bool teamA);
       /*! get number of corners for team */
       static int getCorners(bool teamA);
       /*! get number of throw-ins for team */
       static int getThrows(bool teamA);
       /*! get number of goal kicks for team */
       static int getGoalKicks(bool teamA);
       /*! get number of penalties for team */
       static int getPenalties(bool teamA);
       /*! get total moves  for team */
       static int getTotalMoves(bool teamA);

    protected:

       /*! Set texts of stats to show */
       static void setTexts();
       /*! Set elements to target hide positions */
       static void hideTargets();

   private:
       /*! No allowed instances */
       Stats(){};

       static int fouls[2];        /**< total fouls*/
       static int goalShoots[2];   /**< total goal shoots */
       static int corners[2];      /**< total corners */
       static int throwIns[2];     /**< total throw-ins */
       static int goalKicks[2];    /**< total goal kicks */
       static int penalties[2];    /**< total penalty kicks */
       static int totalMoves[2];   /**< total moves */

       static Ogre::Overlay* ogreOverlay;   /**< Overlay for statistics */
       static Goblin::Ibutton* buttonClose; /**< Close Button */
       static Goblin::Image* backImage;     /**< Background image */
       static bool returnStatus;            /**< internal return */
       static Goblin::TextBox*
                  text[BTSOCCER_TOTAL_TEXT_STATS]; /**< Each stats text */
       static Goblin::TextBox*
                  title[BTSOCCER_TOTAL_STATS]; /**< Each stats name */
       static Goblin::TextBox* scoreText[2];/**< team score text */

       static Goblin::Image* teamLogoA; /**< Each team Logo */
       static Goblin::Image* teamLogoB; /**< Each team Logo */
       static Kobold::Timer timer; /**< timer for max display time */
};



}


#endif


