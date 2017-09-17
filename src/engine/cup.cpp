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

#include "cup.h"
#include "teams.h"

#include <OGRE/OgreMath.h>
using namespace BtSoccer;

/***********************************************************************
 *                       CupMatch Constructor                          *
 ***********************************************************************/
CupMatch::CupMatch(int teamA, int teamB)
{
   /* Set teams and clear score */
   teams[0] = teamA;
   teams[1] = teamB;
   score[0] = 0;
   score[1] = 0;
}

/***********************************************************************
 *                       CupMatch Destructor                           *
 ***********************************************************************/
CupMatch::~CupMatch()
{
}

/***********************************************************************
 *                        CupMatch Simulate                            *
 ***********************************************************************/
void CupMatch::simulate()
{
   /* FIXME: do a better simulation, using team values */
   score[0] = Ogre::Math::RangeRandom(0.0f, 
		   Ogre::Math::RangeRandom(1.0f, 12.0f));
   score[1] = Ogre::Math::RangeRandom(0.0f, 
		   Ogre::Math::RangeRandom(1.0f, 12.0f));
   
   /* FIXME: better ties resolver! */
   if(score[0] == score[1])
   {
      score[0]++;
   }
}

/***********************************************************************
 *                     CupMatch getVictorious                          *
 ***********************************************************************/
int CupMatch::getVictorious()
{
   if(score[0] > score[1])
   {
      return(teams[0]);
   }

   return(teams[1]);
}

/***********************************************************************
 *                           Cup Constructor                           *
 ***********************************************************************/
Cup::Cup(Ogre::String teamPlayer)
{
   int i;

   /* Define the Cup teams */
   defineTeams(teamPlayer);

   /* Create the logo images */

   /* Define Matches */
   for(i=0; i < 8; i++)
   {
      /* Define octaves matches */
      octaves[i] = new CupMatch(i*2, i*2+1);
   }
   for(i=0; i < 4; i++)
   {
      /* Clear quarters */
      quarters[i] = NULL;
   }
   semiFinal[0] = NULL;
   semiFinal[1] = NULL;
   finalMatch = NULL;

   /* FIXME: just for tests */
   simulateMatches();
   simulateMatches();
   simulateMatches();
   simulateMatches();

   printf("OCTAVES\n");
   for(i=0; i<8; i++)
   {
      printf("  %s (%d x %d) %s\n", 
         teamList[octaves[i]->teams[0]]->name.c_str(), octaves[i]->score[0],
         octaves[i]->score[1], teamList[octaves[i]->teams[1]]->name.c_str());
   }
   printf("\n");
   printf("QUARTERS\n");
   for(i=0; i<4; i++)
   {
      printf("  %s (%d x %d) %s\n", 
         teamList[quarters[i]->teams[0]]->name.c_str(), quarters[i]->score[0],
         quarters[i]->score[1], teamList[quarters[i]->teams[1]]->name.c_str());
   }
   printf("\n");
   printf("SEMI-FINALS\n");
   for(i=0; i<2; i++)
   {
      printf("  %s (%d x %d) %s\n", 
         teamList[semiFinal[i]->teams[0]]->name.c_str(), semiFinal[i]->score[0],
         semiFinal[i]->score[1],teamList[semiFinal[i]->teams[1]]->name.c_str());
   }
   printf("\n");

   printf("FINAL\n");
   printf("  %s (%d x %d) %s\n", 
      teamList[finalMatch->teams[0]]->name.c_str(), finalMatch->score[0],
      finalMatch->score[1], teamList[finalMatch->teams[1]]->name.c_str());

}

/***********************************************************************
 *                            Cup Destructor                           *
 ***********************************************************************/
Cup::~Cup()
{
   int i;
   /* TODO: delete images */

   /* Delete Matches */
   for(i = 0; i < 8; i++)
   {
      if(octaves[i])
      {
         delete octaves[i];
      }
      if((i < 4) && (quarters[i]))
      {
         delete quarters[i];
      }
      if((i < 2) && (semiFinal[i]))
      {
         delete semiFinal[i];
      }
   }
   if(finalMatch)
   {
      delete(finalMatch);
   }
}

/***********************************************************************
 *                             defineTeams                             *
 ***********************************************************************/
void Cup::defineTeams(Ogre::String teamPlayer)
{
   // FIXME: re-do.
#if 0
   Regions teams;
   int total = teams.getTotal();
   int i;
   bool* selected = new bool[total];
  
   /* Clear selected vector */
   for(i=0; i < total; i++)
   {
      selected[i] = false;
   }

   /* Mark the player's team as selected */
   teamList[0] = teams.getTeam(teamPlayer);
   if(!teamList[0])
   {
      printf("ERROR: couldn't get team %s\n", teamPlayer.c_str());
   }
   selected[teamList[0]->id] = true;

   /* Select all CPU ones */
   for(int t=1; t < CUP_TOTAL_TEAMS; t++)
   {
      /* Try to get a random team */
      int cur = (int)Ogre::Math::RangeRandom(0.0f, total-1);
      while(selected[cur])
      {
         /* Already selected, try the next one */
         cur = (cur+1) % total;
      }
      teamList[t] = teams[cur];
      selected[cur] = true;
      printf("Got: %d: %s\n", cur, teamList[t]->name.c_str());
   }

   /* Free the mallocs! */
   delete[] selected;
#endif
}

/***********************************************************************
 *                           verifyEvents                              *
 ***********************************************************************/
int Cup::verifyEvents(int mouseX, int mouseY, bool leftButtonPressed)
{
   /* TODO */
}

/***********************************************************************
 *                          simulateMatches                            *
 ***********************************************************************/
void Cup::simulateMatches()
{
   int i;
   
   if(finalMatch)
   {
      printf("Will simulate the final\n");
      /* Just simulate the final */
      finalMatch->simulate();
      return;
   }
   else if(semiFinal[0])
   {
      printf("Will simulate the semifinals\n");
      semiFinal[0]->simulate();
      semiFinal[1]->simulate();

      /* Create the Final */
      finalMatch = new CupMatch(semiFinal[0]->getVictorious(),
                                semiFinal[1]->getVictorious());
   }
   else if(quarters[0])
   {
      printf("Will simulate the quarters\n");
      for(i=0; i<2; i++)
      {
         /* Simulate two quarters */
         quarters[i*2]->simulate();
	 quarters[(i*2)+1]->simulate();
         
	 /* Create a semi final */
	 semiFinal[i] = new CupMatch(quarters[i*2]->getVictorious(),
			    quarters[(i*2)+1]->getVictorious());
      }
   }
   else if(octaves[0])
   {
      printf("Will simulate the octaves\n");
      for(i=0; i<4; i++)
      {
         /* Simulate two octaves */
         octaves[i*2]->simulate();
	 octaves[(i*2)+1]->simulate();
         
	 /* Create a quarter final */
	 quarters[i] = new CupMatch(octaves[i*2]->getVictorious(),
			    octaves[(i*2)+1]->getVictorious());
      }
   }
}


