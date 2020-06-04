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

#include "guiscore.h"
#include <OGRE/OgreRenderWindow.h>
#include <kobold/ogre3d/i18n.h>

using namespace BtSoccer;

#define GUISCORE_UPDATE_FRAMES 20
#define GUISCORE_UP_DOWN_TEAMS_Y_DELTA 40
#define GUISCORE_WAIT_TIME   1000

/***************************************************************
 *                           init                              *
 ***************************************************************/
void GuiScore::init()
{
   teamLogoA = NULL;
   teamLogoB = NULL;

   /* Create the ogre overlay */
   ogreOverlay = Ogre::OverlayManager::getSingletonPtr()->create("GuiScoreOvl");
   ogreOverlay->setZOrder(642);
   ogreOverlay->show();

   /* Create the "A x B" text */
   scoreText  = new Goblin::TextBox(128*Goblin::ScreenInfo::getGuiScale(), 0, 
         128*Goblin::ScreenInfo::getGuiScale(), 
         32*Goblin::ScreenInfo::getGuiScale(), 
         "0 x 0",  "GuiScoreText", ogreOverlay, "infoFont", 18);
   scoreText->setColor(1.0f, 1.0f, 1.0f, 1.0f);
   scoreText->setAlignment(Ogre::TextAreaOverlayElement::Center);
   scoreText->setPosition(Goblin::ScreenInfo::getWindowWidth() - 
         68*Goblin::ScreenInfo::getGuiScale(), 
         12*Goblin::ScreenInfo::getGuiScale());
   scoreText->hide();
   
   /* Create the "Turn:" text */
   turnText  = new Goblin::TextBox(128*Goblin::ScreenInfo::getGuiScale(), 0,
         128*Goblin::ScreenInfo::getGuiScale(), 
         32*Goblin::ScreenInfo::getGuiScale(),
         Kobold::i18n::translate("Turn:"), "GuiScoreTurnText", ogreOverlay, 
         "infoFont", 18);
   turnText->setColor(1.0f, 1.0f, 1.0f, 1.0f);
   turnText->setAlignment(Ogre::TextAreaOverlayElement::Center);
   turnText->setPosition(Goblin::ScreenInfo::getWindowWidth() - 
         80*Goblin::ScreenInfo::getGuiScale(), 
         60*Goblin::ScreenInfo::getGuiScale());
   turnText->hide();

   /* Create Team Background for Texts */
   teamBarA = new Goblin::Image(ogreOverlay, "initial/teamtext.png","gui");
   teamBarA->setDimensions(128*Goblin::ScreenInfo::getGuiScale(),
         64*Goblin::ScreenInfo::getGuiScale());
   teamBarB = new Goblin::Image(ogreOverlay, "initial/teamtext.png", "gui");
   teamBarB->setDimensions(128*Goblin::ScreenInfo::getGuiScale(),
         64*Goblin::ScreenInfo::getGuiScale());
   teamBarA->hide();
   teamBarB->hide();
   
   /* Create Team Texts */
   teamAText = new Goblin::TextBox(0, 0, 256*Goblin::ScreenInfo::getGuiScale(), 
         32*Goblin::ScreenInfo::getGuiScale(), "", "GuiScoreTeamAText",
         ogreOverlay, "freeSans24", 24);
   teamAText->setColor(0.0f, 0.0f, 0.0f, 1.0f);
   teamAText->setAlignment(Ogre::TextAreaOverlayElement::Center);
   teamAText->hide();
   teamBText = new Goblin::TextBox(0, 0, 256*Goblin::ScreenInfo::getGuiScale(), 
         32*Goblin::ScreenInfo::getGuiScale(), "", "GuiScoreTeamBText",
         ogreOverlay, "freeSans24", 24);
   teamBText->setColor(0.0f, 0.0f, 0.0f, 1.0f);
   teamBText->setAlignment(Ogre::TextAreaOverlayElement::Center);
   teamBText->hide();
}

/***************************************************************
 *                         isInited                            *
 ***************************************************************/
bool GuiScore::isInited()
{
   return ogreOverlay != NULL;
}

/***************************************************************
 *                          finish                             *
 ***************************************************************/
void GuiScore::finish()
{
   clearLogos();
   
   if(scoreText)
   {
      delete(scoreText);
   }
   if(turnText)
   {
      delete(turnText);
   }
   if(teamBarA)
   {
      delete teamBarA;
      teamBarA = NULL;
   }
   if(teamBarB)
   {
      delete teamBarB;
      teamBarB = NULL;
   }
   if(teamAText)
   {
      delete teamAText;
      teamAText = NULL;
   }
   if(teamBText)
   {
      delete teamBText;
      teamBText = NULL;
   }
   if(ogreOverlay)
   {
      Ogre::OverlayManager::getSingletonPtr()->destroy(ogreOverlay);
   }
}

/***************************************************************
 *                        clearLogos                           *
 ***************************************************************/
void GuiScore::clearLogos()
{
   if(teamLogoA)
   {
      delete teamLogoA;
      teamLogoA = NULL;
   }
   if(teamLogoB)
   {
      delete teamLogoB;
      teamLogoB = NULL;
   }
   if(turnTeamA)
   {
      delete turnTeamA;
      turnTeamA = NULL;
   }
   if(turnTeamB)
   {
      delete turnTeamB;
      turnTeamB = NULL;
   }
}

/***************************************************************
 *                  showInitialUpperDownTeams                  *
 ***************************************************************/
void GuiScore::showInitialUpperDownTeams()
{
   int quarterY = Goblin::ScreenInfo::getQuarterWindowHeight() -
      GUISCORE_UP_DOWN_TEAMS_Y_DELTA*Goblin::ScreenInfo::getGuiScale();
   
   teamBarA->setPosition(-364*Goblin::ScreenInfo::getGuiScale(),
            Goblin::ScreenInfo::getHalfWindowHeight() + quarterY - 
            20 * Goblin::ScreenInfo::getGuiScale());
   teamAText->setPosition(-300*Goblin::ScreenInfo::getGuiScale(), 
         Goblin::ScreenInfo::getHalfWindowHeight() + quarterY);
   teamBarA->setTargetPosition(Goblin::ScreenInfo::getHalfWindowWidth() - 
            64 * Goblin::ScreenInfo::getGuiScale(),
            Goblin::ScreenInfo::getHalfWindowHeight() + quarterY - 
            20 * Goblin::ScreenInfo::getGuiScale(),
            GUISCORE_UPDATE_FRAMES);
   teamAText->setTargetPosition(Goblin::ScreenInfo::getHalfWindowWidth(),
         Goblin::ScreenInfo::getHalfWindowHeight()+quarterY,
         GUISCORE_UPDATE_FRAMES);
   teamAText->show();
   teamBarA->show();

   teamBarB->setPosition(-364*Goblin::ScreenInfo::getGuiScale(),
            quarterY - 20 * Goblin::ScreenInfo::getGuiScale());
   teamBText->setPosition(-300*Goblin::ScreenInfo::getGuiScale(), quarterY);
   teamBarB->setTargetPosition(Goblin::ScreenInfo::getHalfWindowWidth() - 
            64 * Goblin::ScreenInfo::getGuiScale(),
            quarterY - 20 * Goblin::ScreenInfo::getGuiScale(),
            GUISCORE_UPDATE_FRAMES);
   teamBText->setTargetPosition(Goblin::ScreenInfo::getHalfWindowWidth(),
         quarterY, GUISCORE_UPDATE_FRAMES);
   teamBText->show();
   teamBarB->show();
   
   teamLogoA->setDimensions(128*Goblin::ScreenInfo::getGuiScale(), 
         128*Goblin::ScreenInfo::getGuiScale());
   teamLogoA->setPosition(-364*Goblin::ScreenInfo::getGuiScale(), 
         Goblin::ScreenInfo::getHalfWindowHeight()+
         quarterY+32*Goblin::ScreenInfo::getGuiScale());
   teamLogoA->setTargetPosition(Goblin::ScreenInfo::getHalfWindowWidth() - 
         64*Goblin::ScreenInfo::getGuiScale(),
         Goblin::ScreenInfo::getHalfWindowHeight()+
         quarterY+32*Goblin::ScreenInfo::getGuiScale(),
         GUISCORE_UPDATE_FRAMES);
   teamLogoB->setDimensions(128*Goblin::ScreenInfo::getGuiScale(), 
         128*Goblin::ScreenInfo::getGuiScale());
   teamLogoB->setPosition(-364*Goblin::ScreenInfo::getGuiScale(), 
         quarterY-128*Goblin::ScreenInfo::getGuiScale());
   teamLogoB->setTargetPosition(Goblin::ScreenInfo::getHalfWindowWidth() - 
         64*Goblin::ScreenInfo::getGuiScale(), 
         quarterY - 128*Goblin::ScreenInfo::getGuiScale(),
         GUISCORE_UPDATE_FRAMES);
   
   timer.reset();
   timer.pause();
}

/***************************************************************
 *                           update                            *
 ***************************************************************/
void GuiScore::update()
{
   if((teamLogoA == NULL) || (teamLogoB == NULL))
   {
      return;
   }
   int quarterY = Goblin::ScreenInfo::getQuarterWindowHeight() -
      GUISCORE_UP_DOWN_TEAMS_Y_DELTA*Goblin::ScreenInfo::getGuiScale();
   
   teamBarA->update();
   teamBarB->update();
   teamAText->update();
   teamBText->update();
   teamLogoA->update();
   teamLogoB->update();
   if(!teamAText->isUpdating())
   {
      /* It's static, lets start the timer if needed. */
      if(timer.isPaused())
      {
         if(teamAText->getPosX() == Goblin::ScreenInfo::getHalfWindowWidth())
         {
            /* Lets wait some time */
            timer.resume();
         }
      }
      else if(timer.getMilliseconds() >= GUISCORE_WAIT_TIME)
      {
         /* Let's send the text outer */
         timer.pause();
         teamBarA->setTargetPosition(Goblin::ScreenInfo::getWindowWidth() +
               364 * Goblin::ScreenInfo::getGuiScale(),
               Goblin::ScreenInfo::getHalfWindowHeight() + quarterY - 20,
               GUISCORE_UPDATE_FRAMES);
         teamAText->setTargetPosition(Goblin::ScreenInfo::getWindowWidth() + 
               300*Goblin::ScreenInfo::getGuiScale(),
               Goblin::ScreenInfo::getHalfWindowHeight()+quarterY,
               GUISCORE_UPDATE_FRAMES);
         teamBarB->setTargetPosition(Goblin::ScreenInfo::getWindowWidth() + 
               364 * Goblin::ScreenInfo::getGuiScale(),
               quarterY - 20, GUISCORE_UPDATE_FRAMES);
         teamBText->setTargetPosition(Goblin::ScreenInfo::getWindowWidth() + 
               300 * Goblin::ScreenInfo::getGuiScale(), quarterY, 
               GUISCORE_UPDATE_FRAMES);
         /* And images to the score position */
         teamLogoA->setTarget(Goblin::ScreenInfo::getWindowWidth() - 132 * 
               Goblin::ScreenInfo::getGuiScale(), 
               4*Goblin::ScreenInfo::getGuiScale(), 
               32*Goblin::ScreenInfo::getGuiScale(), 
               32*Goblin::ScreenInfo::getGuiScale(),
               GUISCORE_UPDATE_FRAMES);
         teamLogoB->setTarget(Goblin::ScreenInfo::getWindowWidth() - 36 * 
               Goblin::ScreenInfo::getGuiScale(), 
               4*Goblin::ScreenInfo::getGuiScale(), 
               32*Goblin::ScreenInfo::getGuiScale(), 
               32*Goblin::ScreenInfo::getGuiScale(),
               GUISCORE_UPDATE_FRAMES);
      }
   }
}

/***************************************************************
 *                           newTurn                           *
 ***************************************************************/
void GuiScore::newTurn(bool isTeamAActive)
{
   if(teamLogoA != NULL)
   {
      isTeamATurn = isTeamAActive;
      hide();
      show();
   }
}

/***************************************************************
 *                            reset                            *
 ***************************************************************/
void GuiScore::reset(Ogre::String logoA, Ogre::String teamAName,
                     Ogre::String logoB, Ogre::String teamBName)
{
   /* Clean Logos, if any */
   clearLogos();
   
   /* Get Team Logos */
   teamLogoA = new Goblin::Image(ogreOverlay, logoA, "game");
   teamLogoA->hide();
   teamLogoB = new Goblin::Image(ogreOverlay, logoB, "game");
   teamLogoB->hide();
   turnTeamA = new Goblin::Image(ogreOverlay, logoA, "game");
   turnTeamA->hide();
   turnTeamB = new Goblin::Image(ogreOverlay, logoB, "game");
   turnTeamB->hide();
   
   /* Set team texts */
   teamAText->setText(teamAName);
   teamBText->setText(teamBName);
   
   
   /* Reset team goals */
   teamGoalsA = 0;
   teamGoalsB = 0;

   /* Set new text */
   scoreText->setText("0 x 0");

   int size = 32*Goblin::ScreenInfo::getGuiScale();

   /* Define image positions */
   teamLogoA->setDimensions(size, size);
   teamLogoA->setPosition(Goblin::ScreenInfo::getWindowWidth() - 132 * 
         Goblin::ScreenInfo::getGuiScale(), 
         4 * Goblin::ScreenInfo::getGuiScale());
   teamLogoB->setDimensions(size, size);
   teamLogoB->setPosition(Goblin::ScreenInfo::getWindowWidth() - 36 * 
         Goblin::ScreenInfo::getGuiScale(), 
         4 * Goblin::ScreenInfo::getGuiScale());
   
   /* Set turn positions */
   turnTeamA->setDimensions(size, size);
   turnTeamA->setPosition(Goblin::ScreenInfo::getWindowWidth()-52 * 
         Goblin::ScreenInfo::getGuiScale(), 
         51 * Goblin::ScreenInfo::getGuiScale());
   turnTeamB->setDimensions(size, size);
   turnTeamB->setPosition(Goblin::ScreenInfo::getWindowWidth()-52 * 
         Goblin::ScreenInfo::getGuiScale(), 
         51 * Goblin::ScreenInfo::getGuiScale());

   /* Show them */
   show();
}

/***************************************************************
 *                             show                            *
 ***************************************************************/
void GuiScore::show()
{
   scoreText->show();
   turnText->show();
   if(teamLogoA)
   {
      teamLogoA->show();
   }
   if(teamLogoB)
   {
      teamLogoB->show();
   }
   if( (turnTeamA) && (isTeamATurn) )
   {
      turnTeamA->show();
   }
   if( (turnTeamB) && (!isTeamATurn) )
   {
      turnTeamB->show();
   }
}


/***************************************************************
 *                             hide                            *
 ***************************************************************/
void GuiScore::hide()
{
   scoreText->hide();
   turnText->hide();
   if(teamLogoA)
   {
      teamLogoA->hide();
   }
   if(teamLogoB)
   {
      teamLogoB->hide();
   }
   if(turnTeamA)
   {
      turnTeamA->hide();
   }
   if(turnTeamB)
   {
      turnTeamB->hide();
   }
   if(teamAText)
   {
      teamAText->hide();
   }
   if(teamBText)
   {
      teamBText->hide();
   }
}

/***************************************************************
 *                         getTeamScore                        *
 ***************************************************************/
void GuiScore::getTeamScore(int& goalsA, int& goalsB)
{
   goalsA = teamGoalsA;
   goalsB = teamGoalsB;
}

/***************************************************************
 *                          goalTeamA                          *
 ***************************************************************/
void GuiScore::goalTeamA()
{
   teamGoalsA++;
   setText();
}

/***************************************************************
 *                          goalTeamB                          *
 ***************************************************************/
void GuiScore::goalTeamB()
{
   teamGoalsB++;
   setText();
}

/***************************************************************
 *                       setGoalsTeamA                         *
 ***************************************************************/
void GuiScore::setGoalsTeamA(int g)
{
   teamGoalsA = g;
   setText();
}

/***************************************************************
 *                       setGoalsTeamB                         *
 ***************************************************************/
void GuiScore::setGoalsTeamB(int g)
{
   teamGoalsB = g;
   setText();
}

/***************************************************************
 *                         goalsTeamA                          *
 ***************************************************************/
int GuiScore::goalsTeamA()
{
   return(teamGoalsA);
}

/***************************************************************
 *                         goalsTeamB                          *
 ***************************************************************/
int GuiScore::goalsTeamB()
{
   return(teamGoalsB);
}

/***************************************************************
 *                           setText                           *
 ***************************************************************/
void GuiScore::setText()
{
   scoreText->setText(Ogre::StringConverter::toString(teamGoalsA) +
                      Ogre::String(" x ") +
                      Ogre::StringConverter::toString(teamGoalsB) );
}

/***************************************************************
 *                       Static Members                        *
 ***************************************************************/
Goblin::Image* GuiScore::teamBarA = NULL;
Goblin::Image* GuiScore::teamBarB = NULL;
Goblin::Image* GuiScore::teamLogoA = NULL;
Goblin::Image* GuiScore::teamLogoB = NULL;
Goblin::Image* GuiScore::turnTeamA = NULL;
Goblin::Image* GuiScore::turnTeamB = NULL;
Goblin::TextBox* GuiScore::teamAText = NULL;
Goblin::TextBox* GuiScore::teamBText = NULL;
int GuiScore::teamGoalsA = 0;
int GuiScore::teamGoalsB = 0;
Goblin::TextBox* GuiScore::scoreText = NULL;
Goblin::TextBox* GuiScore::turnText = NULL;
Ogre::Overlay* GuiScore::ogreOverlay = NULL;
bool GuiScore::isTeamATurn = true;
Kobold::Timer GuiScore::timer;


