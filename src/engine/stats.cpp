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

#include <kobold/i18n.h>
#include <goblin/screeninfo.h>

#include "stats.h"
#include "../gui/guiscore.h"
#include "../soundfiles.h"
using namespace BtSoccer;

#define STATS_Y_INC       48
#define STATS_Y_DEC  (STATS_Y_INC / 2) + 8
#define STATS_INITIAL_Y   262

#define STATS_MAX_DISPLAY_TIME 8000

Ogre::String statsTextTitles[]=
{
   "Ball %",
   "Fouls",
   "Goal Attempts",
   "Goal Kicks",
   "Throw-ins",
   "Corners",
   "Total Moves"
};

/***********************************************************************
 *                                 clear                               *
 ***********************************************************************/
void Stats::clear()
{
   int i;
   for(i=0; i < 2; i++)
   {
      fouls[i] = 0;
      goalShoots[i] = 0;
      corners[i] = 0;
      throwIns[i] = 0;
      goalKicks[i] = 0;
      penalties[i] = 0;
      totalMoves[i] = 0;
   }
   returnStatus = false;
}

/***********************************************************************
 *                              setTeams                               *
 ***********************************************************************/
void Stats::setTeams(Ogre::String teamA, Ogre::String teamB)
{
   if(teamLogoA)
   {
      delete(teamLogoA);
   }
   if(teamLogoB)
   {
      delete(teamLogoB);
   }

   teamLogoA = new Goblin::Image(ogreOverlay, teamA, "game");
   teamLogoA->setDimensions(100*Goblin::ScreenInfo::getGuiScale(), 
         100*Goblin::ScreenInfo::getGuiScale());
   teamLogoA->setPosition(1528*Goblin::ScreenInfo::getGuiScale(), 
         114*Goblin::ScreenInfo::getGuiScale());
   teamLogoA->hide();

   teamLogoB = new Goblin::Image(ogreOverlay, teamB, "game");
   teamLogoB->setDimensions(100*Goblin::ScreenInfo::getGuiScale(), 
         100*Goblin::ScreenInfo::getGuiScale());
   teamLogoB->setPosition(1868*Goblin::ScreenInfo::getGuiScale(), 
         114*Goblin::ScreenInfo::getGuiScale());
   teamLogoB->hide();
}


/***********************************************************************
 *                                 foul                                *
 ***********************************************************************/
void Stats::foul(bool teamA)
{
   fouls[((teamA)?0:1)]++;
}
int Stats::getFouls(bool teamA)
{
   return(fouls[((teamA)?0:1)]);
}

/***********************************************************************
 *                             goalShoot                               *
 ***********************************************************************/
void Stats::goalShoot(bool teamA)
{
   goalShoots[((teamA)?0:1)]++;
}
int Stats::getGoalShoots(bool teamA)
{
   return(goalShoots[((teamA)?0:1)]);
}

/***********************************************************************
 *                                 corner                              *
 ***********************************************************************/
void Stats::corner(bool teamA)
{
   corners[((teamA)?0:1)]++;
}
int Stats::getCorners(bool teamA)
{
   return(corners[((teamA)?0:1)]);
}

/***********************************************************************
 *                               throwIn                               *
 ***********************************************************************/
void Stats::throwIn(bool teamA)
{
   throwIns[((teamA)?0:1)]++;
}
int Stats::getThrows(bool teamA)
{
   return(throwIns[((teamA)?0:1)]);
}

/***********************************************************************
 *                                goalKick                             *
 ***********************************************************************/
void Stats::goalKick(bool teamA)
{
   goalKicks[((teamA)?0:1)]++;
}
int Stats::getGoalKicks(bool teamA)
{
   return(goalKicks[((teamA)?0:1)]);
}

/***********************************************************************
 *                               penalty                               *
 ***********************************************************************/
void Stats::penalty(bool teamA)
{
   penalties[((teamA)?0:1)]++;
}
int Stats::getPenalties(bool teamA)
{
   return(penalties[((teamA)?0:1)]);
}

/***********************************************************************
 *                                 moved                               *
 ***********************************************************************/
void Stats::moved(bool teamA)
{
   totalMoves[((teamA)?0:1)]++;
}
int Stats::getTotalMoves(bool teamA)
{
   return(totalMoves[((teamA)?0:1)]);
}



/***********************************************************************
 *                               create                                *
 ***********************************************************************/
void Stats::init()
{
   int i;

   /* Create the overlay */
   ogreOverlay = Ogre::OverlayManager::getSingletonPtr()->create("statsOvl");
   ogreOverlay->setZOrder(620);
   ogreOverlay->show();

   /* Create the back Image */
   backImage = new Goblin::Image(ogreOverlay, "main/stats.png", "gui");
   backImage->setPosition(1200*Goblin::ScreenInfo::getGuiScale(), 
         100*Goblin::ScreenInfo::getGuiScale());

   /* Create the button */
   buttonClose = new Goblin::Ibutton(ogreOverlay, "initial/accept.png", "gui",
         Kobold::i18n::translate("Accept"), "infoFontOut", 16);
   buttonClose->setPosition(-100*Goblin::ScreenInfo::getGuiScale(), 
         582*Goblin::ScreenInfo::getGuiScale());
   buttonClose->setDimensions(64*Goblin::ScreenInfo::getGuiScale(), 
         64*Goblin::ScreenInfo::getGuiScale());
   buttonClose->setMouseOverDimensions(65*Goblin::ScreenInfo::getGuiScale(), 
         65*Goblin::ScreenInfo::getGuiScale());
   buttonClose->setPressedSound(BTSOCCER_SOUND_GUI_CLICK);

   /* Create the score */
   scoreText[0] = new Goblin::TextBox(1605*Goblin::ScreenInfo::getGuiScale(), 
         142*Goblin::ScreenInfo::getGuiScale(), 128*Goblin::ScreenInfo::getGuiScale(), 
         128*Goblin::ScreenInfo::getGuiScale(), "0", "StatsScore0",
         ogreOverlay, "numberFont", 44);
   scoreText[0]->setColor(1.0f, 1.0f, 1.0f, 1.0f);
   scoreText[0]->setAlignment(Ogre::TextAreaOverlayElement::Center);
   scoreText[1] = new Goblin::TextBox(1805*Goblin::ScreenInfo::getGuiScale(), 
         142*Goblin::ScreenInfo::getGuiScale(), 128*Goblin::ScreenInfo::getGuiScale(), 
         128*Goblin::ScreenInfo::getGuiScale(), "0", "StatsScore1",
         ogreOverlay, "numberFont", 44);
   scoreText[1]->setColor(1.0f, 1.0f, 1.0f, 1.0f);
   scoreText[1]->setAlignment(Ogre::TextAreaOverlayElement::Center);

   /* Create each text box */
   int y = STATS_INITIAL_Y*Goblin::ScreenInfo::getGuiScale();
   char buf[32];
   Ogre::String str;
   for(i = 0; i < BTSOCCER_TOTAL_STATS; i++)
   {
      sprintf(buf, "st_%2d", i);
      str = Ogre::String("StatsBTSOCCER") + buf;
      text[i*2] = new Goblin::TextBox(1430*Goblin::ScreenInfo::getGuiScale(), 
            (y-STATS_Y_DEC)*Goblin::ScreenInfo::getGuiScale(),
            32*Goblin::ScreenInfo::getGuiScale(), 32*Goblin::ScreenInfo::getGuiScale(), "0", str,
            ogreOverlay, "infoFontOut", 16);
      text[i*2]->setColor(1.0f, 1.0f, 1.0f, 1.0f);
      text[i*2]->setAlignment(Ogre::TextAreaOverlayElement::Center);
      
      sprintf(buf, "st_%2d", i*2+1);
      str = Ogre::String("StatsBTSOCCER") + buf;
      text[i*2+1] = new Goblin::TextBox(1960*Goblin::ScreenInfo::getGuiScale(), 
            (y-STATS_Y_DEC)*Goblin::ScreenInfo::getGuiScale(),
            32*Goblin::ScreenInfo::getGuiScale(), 32*Goblin::ScreenInfo::getGuiScale(), "0", str,
            ogreOverlay, "infoFontOut", 16);
      text[i*2+1]->setColor(1.0f, 1.0f, 1.0f, 1.0f);
      text[i*2+1]->setAlignment(Ogre::TextAreaOverlayElement::Center);
      
      sprintf(buf, "st_title_%2d", i*2+1);
      title[i] = new Goblin::TextBox(1639*Goblin::ScreenInfo::getGuiScale(), 
            y*Goblin::ScreenInfo::getGuiScale(), 256*Goblin::ScreenInfo::getGuiScale(), 
            32*Goblin::ScreenInfo::getGuiScale(),
            Kobold::i18n::translate(statsTextTitles[i]),
            str, ogreOverlay, "infoFontOut", 16);
      title[i]->setColor(1.0f, 1.0f, 1.0f, 1.0f);
      title[i]->setAlignment(Ogre::TextAreaOverlayElement::Center);
    
      y += STATS_Y_INC*Goblin::ScreenInfo::getGuiScale();
   }

   /* Clear current statistics */
   clear();

   hide();
}

/***********************************************************************
 *                               destroy                               *
 ***********************************************************************/
void Stats::finish()
{
   int i;

   delete(backImage);
   delete(buttonClose);
  
   /* Delete texts */
   for(i=0; i < BTSOCCER_TOTAL_STATS; i++)
   {
      delete(title[i]);
      delete(text[i*2]);
      delete(text[i*2+1]);
   }

   /* Delete logos */
   if(teamLogoA)
   {
      delete(teamLogoA);
   }
   if(teamLogoB)
   {
      delete(teamLogoB);
   }

   Ogre::OverlayManager::getSingletonPtr()->destroy(ogreOverlay);
}

/***********************************************************************
 *                            verifyEvents                             *
 ***********************************************************************/
bool Stats::verifyEvents(int mouseX, int mouseY, bool leftButtonPressed, 
      bool useTimer)
{
   /* Update animations */
   int i;
   bool isUpdating = false;
   backImage->update();
   buttonClose->update();
   teamLogoA->update();
   teamLogoB->update();
   scoreText[0]->update();
   scoreText[1]->update();
   for(i = 0; i < BTSOCCER_TOTAL_STATS; i++)
   {
      text[i*2]->update();
      text[i*2+1]->update();
      title[i]->update();
      isUpdating |= text[i]->isUpdating();
   }
   isUpdating |= backImage->isUpdating();
   isUpdating |= buttonClose->isUpdating();
   isUpdating |= teamLogoA->isUpdating();
   isUpdating |= teamLogoB->isUpdating();
   isUpdating |= scoreText[0]->isUpdating();
   isUpdating |= scoreText[1]->isUpdating();
   
   unsigned long time = (useTimer) ? timer.getMilliseconds() : 0L;
   
   /* Verify Timeout of screen display and button press */
   if( (!returnStatus) &&
       ( (time >= STATS_MAX_DISPLAY_TIME) ||
         (buttonClose->verifyEvents(mouseX, mouseY, leftButtonPressed) ==
             IBUTTON_EVENT_PRESSED) ) )
   {
      /* Send everyone back to "hide" */
      hideTargets();
      returnStatus = true;

   }
   else if( (returnStatus) && (!isUpdating) )
   {
      /* Done with Animation */
      returnStatus = false;
      return(true);
   }
   else if(time < STATS_MAX_DISPLAY_TIME)
   {
      /* Print remaining time on button */
      char buf[16];
      sprintf(buf, " (%lu)", (STATS_MAX_DISPLAY_TIME - time) / 1000);
      buttonClose->setText(Kobold::i18n::translate("Accept") + buf);
   }

   return(false);
}

/***********************************************************************
 *                              setTexts                               *
 ***********************************************************************/
void Stats::setTexts()
{
   char buf[16];

   /* Set score text */
   sprintf(buf, "%d", GuiScore::goalsTeamA());
   scoreText[0]->setText(buf);
   sprintf(buf, "%d", GuiScore::goalsTeamB());
   scoreText[1]->setText(buf);

   /* Calculate ball possession */
   int ballPossession = 50;
   if( (totalMoves[0] != 0) || (totalMoves[1] != 0) )
   {
      float poss = (totalMoves[0]/(float)(totalMoves[0]+totalMoves[1]))*100.0f;
      ballPossession = (int)poss;
   }

   /* Set ball possession texts */
   sprintf(buf, "%d", ballPossession);
   text[0]->setText(buf);
   sprintf(buf, "%d", 100-ballPossession);
   text[1]->setText(buf);

   /* Set Fouls texts */
   sprintf(buf, "%d", fouls[0]);
   text[2]->setText(buf);
   sprintf(buf, "%d", fouls[1]);
   text[3]->setText(buf);

   /* Set Goal Attempts texts */
   sprintf(buf, "%d", goalShoots[0]);
   text[4]->setText(buf);
   sprintf(buf, "%d", goalShoots[1]);
   text[5]->setText(buf);

   /* Set Goal Kicks texts */
   sprintf(buf, "%d", goalKicks[0]);
   text[6]->setText(buf);
   sprintf(buf, "%d", goalKicks[1]);
   text[7]->setText(buf);

   /* Set Throw-ins texts */
   sprintf(buf, "%d", throwIns[0]);
   text[8]->setText(buf);
   sprintf(buf, "%d", throwIns[1]);
   text[9]->setText(buf);

   /* Set Corners texts */
   sprintf(buf, "%d", corners[0]);
   text[10]->setText(buf);
   sprintf(buf, "%d", corners[1]);
   text[11]->setText(buf);

   /* Set Moves texts */
   sprintf(buf, "%d", totalMoves[0]);
   text[12]->setText(buf);
   sprintf(buf, "%d", totalMoves[1]);
   text[13]->setText(buf);
}

/***********************************************************************
 *                                 show                                *
 ***********************************************************************/
void Stats::show()
{
   /* Update stats texts */
   setTexts();

   /* Show them all */
   backImage->show();
   buttonClose->show();
   ogreOverlay->show();
   scoreText[0]->show();
   scoreText[1]->show();
   if( (teamLogoA) && (teamLogoB) )
   {
      teamLogoA->show();
      teamLogoB->show();
   }


   /* Set target positions */
   backImage->setTargetPosition(0, 100*Goblin::ScreenInfo::getGuiScale(), 50);

   if(Goblin::ScreenInfo::getWindowHeight() >= 760)
   {
      /* Usually, bellow stats display */
      buttonClose->setTargetPosition(480*Goblin::ScreenInfo::getGuiScale(), 
         582*Goblin::ScreenInfo::getGuiScale(), 50);
   }
   else 
   {
      /* On small screens, at screen side */
      buttonClose->setTargetPosition(Ogre::Real(Goblin::ScreenInfo::getWindowWidth()) -
            Ogre::Real(80*Goblin::ScreenInfo::getGuiScale()),
            Goblin::ScreenInfo::getWindowHeight()-160*Goblin::ScreenInfo::getGuiScale(), 50);
   }

   /* Target logos */
   teamLogoA->setTargetPosition(252*Goblin::ScreenInfo::getGuiScale(), 
         114*Goblin::ScreenInfo::getGuiScale(), 50);
   teamLogoB->setTargetPosition(668*Goblin::ScreenInfo::getGuiScale(), 
         114*Goblin::ScreenInfo::getGuiScale(), 50);

   /* Target scoreText */
   scoreText[0]->setTargetPosition(415*Goblin::ScreenInfo::getGuiScale(), 
         142*Goblin::ScreenInfo::getGuiScale(), 50);
   scoreText[1]->setTargetPosition(605*Goblin::ScreenInfo::getGuiScale(), 
         142*Goblin::ScreenInfo::getGuiScale(), 50);

   /* Target text stats */
   int i;
   int y= STATS_INITIAL_Y*Goblin::ScreenInfo::getGuiScale();
   for(i = 0; i < BTSOCCER_TOTAL_STATS; i++)
   {
      title[i]->show();
      title[i]->setTargetPosition(510*Goblin::ScreenInfo::getGuiScale(), 
            y*Goblin::ScreenInfo::getGuiScale(), 50);
      text[i*2]->show();
      text[i*2]->setTargetPosition(315*Goblin::ScreenInfo::getGuiScale(), 
            (y-STATS_Y_DEC)*Goblin::ScreenInfo::getGuiScale(), 50);
      text[i*2+1]->show();
      text[i*2+1]->setTargetPosition(695*Goblin::ScreenInfo::getGuiScale(), 
            (y-STATS_Y_DEC)*Goblin::ScreenInfo::getGuiScale(), 50);
      y += STATS_Y_INC*Goblin::ScreenInfo::getGuiScale();
   }
   
   timer.reset();
}

/***********************************************************************
 *                             hideTargets                             *
 ***********************************************************************/
void Stats::hideTargets()
{
   backImage->setTargetPosition(1200*Goblin::ScreenInfo::getGuiScale(), 
         100*Goblin::ScreenInfo::getGuiScale(), 50);
   buttonClose->setTargetPosition(-100*Goblin::ScreenInfo::getGuiScale(), 
         582*Goblin::ScreenInfo::getGuiScale(), 50);
   teamLogoA->setTargetPosition(1528*Goblin::ScreenInfo::getGuiScale(), 
         114*Goblin::ScreenInfo::getGuiScale(), 50);
   teamLogoB->setTargetPosition(1868*Goblin::ScreenInfo::getGuiScale(), 
         114*Goblin::ScreenInfo::getGuiScale(), 50);
   scoreText[0]->setTargetPosition(1605*Goblin::ScreenInfo::getGuiScale(), 
         142*Goblin::ScreenInfo::getGuiScale(), 50);
   scoreText[1]->setTargetPosition(1805*Goblin::ScreenInfo::getGuiScale(), 
         142*Goblin::ScreenInfo::getGuiScale(), 50);
   int i;
   int y=STATS_INITIAL_Y*Goblin::ScreenInfo::getGuiScale();
   for(i = 0; i < BTSOCCER_TOTAL_STATS; i++)
   {
      title[i]->setTargetPosition(1639*Goblin::ScreenInfo::getGuiScale(), 
            y*Goblin::ScreenInfo::getGuiScale(), 50);
      text[i*2]->setTargetPosition(1430*Goblin::ScreenInfo::getGuiScale(), 
            y*Goblin::ScreenInfo::getGuiScale(), 50);
      text[i*2+1]->setTargetPosition(1960*Goblin::ScreenInfo::getGuiScale(), 
            y*Goblin::ScreenInfo::getGuiScale(), 50);
      y += STATS_Y_INC*Goblin::ScreenInfo::getGuiScale();
   }
}

/***********************************************************************
 *                                 hide                                *
 ***********************************************************************/
void Stats::hide()
{
   int i;

   backImage->hide();
   buttonClose->hide();
   scoreText[0]->hide();
   scoreText[1]->hide();

   if(teamLogoA)
   {
      teamLogoA->hide();
   }
   if(teamLogoB)
   {
      teamLogoB->hide();
   }

   for(i = 0; i < BTSOCCER_TOTAL_STATS; i++)
   {
      title[i]->hide();
      text[i*2]->hide();
      text[i*2+1]->hide();
   }

   ogreOverlay->hide();
}




int Stats::fouls[2];
int Stats::goalShoots[2];
int Stats::corners[2];
int Stats::throwIns[2];
int Stats::goalKicks[2];
int Stats::penalties[2];
int Stats::totalMoves[2];
Ogre::Overlay* Stats::ogreOverlay=NULL;
Goblin::Ibutton* Stats::buttonClose=NULL;
Goblin::Image* Stats::backImage=NULL;
Goblin::TextBox* Stats::text[BTSOCCER_TOTAL_TEXT_STATS];
Goblin::TextBox* Stats::title[BTSOCCER_TOTAL_STATS];
bool Stats::returnStatus = false;
Goblin::Image* Stats::teamLogoA = NULL;
Goblin::Image* Stats::teamLogoB = NULL;
Goblin::TextBox* Stats::scoreText[2];
Kobold::Timer Stats::timer;

