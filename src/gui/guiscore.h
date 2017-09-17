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



#ifndef _btsoccer_gui_score_h
#define _btsoccer_gui_score_h

#include <goblin/image.h>
#include <goblin/textbox.h>
#include <goblin/screeninfo.h>
#include <goblin/image.h>
#include <kobold/timer.h>

namespace BtSoccer
{

/*! The GuiScore Class will count each team goals and display it,
 * with team symbols, like a TV score on soccer games */
class GuiScore
{
   public:
      /*! Reset the GuiScore
       * \param logoA -> logo for teamA
       * \param logoB -> logo for teamB
       * \param teamAName -> name to show for teamA
       * \param teamBName -> name to show for teamB */
      static void reset(Ogre::String logoA, Ogre::String teamAName,
                 Ogre::String logoB, Ogre::String tamBName);
   
      /*! Show initial names of upper/dow teams.
       * \note: it will automatically hide. */
      static void showInitialUpperDownTeams();
   
      /*! Update gui positions (but not gui texts). */
      static void update();

      /*! Init the use of GuiScore */
      static void init();

      /*! Finish the use of GuiScore */
      static void finish();

      /*! Get total score for each teams
       * \param goalsA -> total goals done by teamA
       * \param goalsB -> total goals done by teamB */
      static void getTeamScore(int& goalsA, int& goalsB);

      /*! Called when Goal for teamA occurs */
      static void goalTeamA();

      /*! Called when Goal for teamB occurs */
      static void goalTeamB();

      /*! Set number of goals for teamA */
      static void setGoalsTeamA(int g);
      /*! Set number of goals for teamB */
      static void setGoalsTeamB(int g);

      /*! Get number of goals from teamA */
      static int goalsTeamA();
      /*! Get number of goals from teamB */
      static int goalsTeamB();

      /*! Show the guiScore */
      static void show();

      /*! Hide the guiScore */
      static void hide();
   
      /*! Tell that a new turn inited.
       * \param isTeamAActive -> true if is teamA turn, false if teamB */
      static void newTurn(bool isTeamAActive);

      /*! Get the overlay used */
      static Ogre::Overlay* getOverlay(){return(ogreOverlay);};

      /*! \return if score gui is inited and active */
      static bool isInited();

   protected:
      /* Set the score text */
      static void setText();
   
      /* Clear loaded logos */
      static void clearLogos();

   private:
      GuiScore(){};
  
      static Goblin::Image* teamBarA; /**< Background for team text  */
      static Goblin::Image* teamBarB; /**< Background for team text  */
      static Goblin::Image* teamLogoA;  /**< Current Logo for teamA */
      static Goblin::Image* teamLogoB;  /**< Current Logo for teamB */
   
      static Goblin::Image* turnTeamA;  /**< Logo for teamA turn */
      static Goblin::Image* turnTeamB;  /**< Logo for teamB turn */

      static Goblin::TextBox* scoreText; /**< The Score Text (A x B) */
      static Goblin::TextBox* turnText;  /**< The Turn Text */
   
      static Goblin::TextBox* teamAText; /**< Team that is "upper" */
      static Goblin::TextBox* teamBText; /**< Team that is "down" */

      static Ogre::Overlay* ogreOverlay;  /**< The overlay used */

      static Kobold::Timer timer; /**< a timer, used on animation. */

      static int teamGoalsA;    /**< Current Goals for teamA */
      static int teamGoalsB;    /**< Current Goals for teamB */
   
      static bool isTeamATurn;  /**< If current turn is of teamA */
};

}

#endif

