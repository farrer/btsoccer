/*
  BtSoccer - button football (soccer) game
  Copyright (C) 2008-2015 DNTeam <btsoccer@dnteam.org>

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

#ifndef _btsoccer_cup_h
#define _btsoccer_cup_h

#include <goblin/image.h>
#include "teams.h"

namespace BtSoccer
{

#define CUP_TOTAL_TEAMS    16   /**< Total teams on cup */

/*! A single match at cup */
class CupMatch
{
   public:
      /*! Constructor */
      CupMatch(int teamA, int teamB);
      /*! Destructor */
      ~CupMatch();

      /* Simulate the match, to get an immediate result */
      void simulate();

      /* Get the victorious team */
      int getVictorious();

      int teams[2];          /**< index at teams list */
      int score[2];          /**< Goals of each team */
};

/*! The cup is a championship definition */
class Cup
{
   public:

      enum CupGuiReturnResults
      {
         RETURN_OTHER=0,
	 RETURN_START_MATCH,
	 RETURN_SAVE,
	 RETURN_LOAD,
	 RETURN_DONE
      };

      /*! Constructor 
       * \param teamPlayer -> fileName of team the player selected */
      Cup(Ogre::String teamPlayer);
      /*! Destructor */
      ~Cup();

      /*! Verify Button press at the Cup Gui controller
       * \return -> CupGuiReturnValues */
      int verifyEvents(int mouseX, int mouseY, bool leftButtonPressed);

      /* Simulate current matches (octaves, quarters, semi or finals) 
       * \note -> this function won't simulate matches where player is human */
      void simulateMatches();

   protected:

      /*! Randomize the teams on Cup
       * \param teamPlayer -> fileName of team the player selected */
      void defineTeams(Ogre::String teamPlayer);

      TeamInfo* teamList[CUP_TOTAL_TEAMS]; /**< List of teams on cup */
      Goblin::Image* teamLogos[CUP_TOTAL_TEAMS]; /**< Logo of each team */

      Goblin::Image* back;     /**< Background image of cup gui */

      CupMatch* octaves[8];     /**< Matches of the octaves */
      CupMatch* quarters[4];    /**< Quarter's matches */
      CupMatch* semiFinal[2];   /**< Semifinal matches */
      CupMatch* finalMatch;     /**< Final Match */

};


}

#endif

