
#include "fieldobjecttestcase.h"
#include "../engine/teamplayer.h"
using namespace BtSoccerTests;


FieldObjectTestCase::FieldObjectTestCase() : TestCase(false)
{
}

FieldObjectTestCase::~FieldObjectTestCase()
{
}

void FieldObjectTestCase::doSpecificScenarioCreation()
{
   /* Retrieve disk pointers */
   diskA = teamA->getDisk(0);
   diskB = teamB->getDisk(0);

   /* Put all disks very away from us. */
   int i = 0;
   while(teamA->getDisk(i) != NULL)
   {
      teamA->getDisk(i)->setPosition(-1000, 0.0f, -200 + i * 16);
      teamB->getDisk(i)->setPosition(1000, 0.0f, -200 + i * 16);
      i++;
   }
}

void FieldObjectTestCase::doRun()
{
   testHasFreeWayTo();
}

void FieldObjectTestCase::testHasFreeWayTo()
{
   ogreLog->logMessage("\ttestHasFreeWayTo...");

   /* Put diskA at its down position */
   diskA->setPosition(-100, 0.0f, 0.0f);

   /* let's test with diskB at 'up' position */
   diskB->setPosition(100, 0.0f, 0.0f);

   //TODO: will not test, as no ogre on test mode. :(
}

void FieldObjectTestCase::doSpecificScenarioFinish()
{
}

