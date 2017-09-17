#include "testcase.h"
using namespace BtSoccerTests;

#include "../physics/bulletlink.h"


/*********************************************************************
 *                              Constructor                          *
 *********************************************************************/
TestCase::TestCase(bool createFieldBorders)
{
   this->createFieldBorders = createFieldBorders;
   ogreLog = Ogre::LogManager::getSingleton().getDefaultLog();
}

/*********************************************************************
 *                             createScenario                        *
 *********************************************************************/
void TestCase::createScenario()
{
   /* Create the scenario */
   teamA = new BtSoccer::Team("TeamA");
   teamB = new BtSoccer::Team("TeamB");

   field = new BtSoccer::Field();
   field->createFieldForTestCases(createFieldBorders);

   ball = new BtSoccer::Ball();

   /* Set pointers */
   BtSoccer::BulletLink::setPointers(teamA, teamB, ball, field, false);

   BtSoccer::Rules::setTeamA(teamA);
   BtSoccer::Rules::setTeamB(teamB);
   BtSoccer::Rules::setBall(ball);
   BtSoccer::Rules::setField(field);

   BtSoccer::Rules::startHalf(true);

   doSpecificScenarioCreation();
}

/*********************************************************************
 *                                  run                              *
 *********************************************************************/
void TestCase::run()
{
   createScenario();
   doRun();
   finishScenario();
}

/*********************************************************************
 *                             deleteScenario                        *
 *********************************************************************/
void TestCase::finishScenario()
{
   doSpecificScenarioFinish();

   delete teamA;

   field->deleteField();
   delete field;
}

/*********************************************************************
 *                                 Destructor                        *
 *********************************************************************/
TestCase::~TestCase()
{
}

