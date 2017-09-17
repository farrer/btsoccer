
#include "baseaitestcase.h" 
#include "rulestestcase.h"
#include "fieldobjecttestcase.h"
#include "../physics/bulletlink.h"
#include "../physics/disttable.h"

#include <OGRE/OgreLogManager.h>

#include <iostream>
using namespace std;

using namespace BtSoccerTests;

int main(int argc, char* argv[])
{
   Ogre::LogManager* ogreLogManager = new Ogre::LogManager();
   Ogre::Log* log = ogreLogManager->createLog("unit_tests.log", true);

   log->logMessage("Creating bullet world...");
   BtSoccer::BulletLink::createBulletWorld();

   log->logMessage("Calculating BtSoccer::DistTable::..");
   BtSoccer::DistTable::init(false);

   log->logMessage("Running RulesTestCase... ");
   RulesTestCase* rulesTestCase = new RulesTestCase();
   rulesTestCase->run();
   delete rulesTestCase;

   log->logMessage("Running FieldObjectTestCase..");
   FieldObjectTestCase* fieldObjectTestCase = new FieldObjectTestCase();
   fieldObjectTestCase->run();
   delete fieldObjectTestCase;

   log->logMessage("Running BaseAITestCase... ");
   BtSoccerTests::BaseAITestCase* baseAiTest = 
      new BtSoccerTests::BaseAITestCase();
   baseAiTest->run();
   delete baseAiTest;

   log->logMessage("Cleaning up...");
   BtSoccer::DistTable::finish();
   BtSoccer::BulletLink::deleteBulletWorld();
   delete ogreLogManager;
}

