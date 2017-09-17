
#include "rulestestcase.h"

using namespace BtSoccerTests;

/*********************************************************************
 *                           Constructor                             *
 *********************************************************************/
RulesTestCase::RulesTestCase() : TestCase(false)
{
}

/*********************************************************************
 *                            Destructor                             *
 *********************************************************************/
RulesTestCase::~RulesTestCase()
{
}

/*********************************************************************
 *                         beforeEachTest                            *
 *********************************************************************/
void RulesTestCase::beforeEachTest()
{
   BtSoccer::Rules::startHalf(true);
}

/*********************************************************************
 *                     collisionRelatedTests                         *
 *********************************************************************/
void RulesTestCase::collisionRelatedTests()
{
   ogreLog->logMessage("\tcollisionRelatedTests...");

   BtSoccer::Team* acting = BtSoccer::Rules::getActiveTeam();

   /* Note: at this test we are ignoring the remaining disk touches. */

   /* Just touch the ball and nothing more. */
   BtSoccer::TeamPlayer* actingDisk = acting->getDisk(0);
   BtSoccer::Rules::setDiskAct(actingDisk);
   BtSoccer::Rules::ballCollideDisk(acting);
   actionFinished();

   /* Check if acting remains to the acting team and NORMAL state. */
   assert(BtSoccer::Rules::getActiveTeam() == acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_NORMAL);

   /* Touch the ball before touching an enemy disk */
   BtSoccer::Rules::ballCollideDisk(acting);
   BtSoccer::Rules::diskCollideDisk(acting, BtSoccer::Rules::getOtherTeam(acting), 0.0f, 0.0f);
   actionFinished();
   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() == acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_NORMAL);

   /* Touch the ball before touching an own disk */
   BtSoccer::Rules::ballCollideDisk(acting);
   BtSoccer::Rules::diskCollideDisk(acting, acting, 0.0f, 0.0f);
   actionFinished();
   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() == acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_NORMAL);

   /* Touch an enemy disk before touching the ball */
   BtSoccer::Rules::diskCollideDisk(acting, BtSoccer::Rules::getOtherTeam(acting), 0.0f, 0.0f);
   BtSoccer::Rules::ballCollideDisk(acting);
   actionFinished();
   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() != acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_FREE_KICK);

   /* get new acting team */
   acting = BtSoccer::Rules::getActiveTeam();
   actingDisk = acting->getDisk(0);
   BtSoccer::Rules::setDiskAct(actingDisk);

   /* Touch another own player disk before touching the ball */
   BtSoccer::Rules::diskCollideDisk(acting, acting, 0.0f, 0.0f);
   BtSoccer::Rules::ballCollideDisk(acting);
   actionFinished();
   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() != acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_NORMAL);

   /* get new acting team */
   acting = BtSoccer::Rules::getActiveTeam();
   actingDisk = acting->getDisk(0);
   BtSoccer::Rules::setDiskAct(actingDisk);

   /* Neither touch the ball nor another disk */
   actionFinished();
   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() != acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_NORMAL);

   //TODO: Check penalties.
}

/*********************************************************************
 *                     remainingTouchesTests                         *
 *********************************************************************/
void RulesTestCase::remainingTouchesTests()
{
   Ogre::LogManager::getSingleton().getDefaultLog()->logMessage(
         "\tremainingTouchesTests...");

   /* Make sure we are testing at game's begin */
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_MIDDLE);

   BtSoccer::Team* acting = BtSoccer::Rules::getActiveTeam();
   BtSoccer::TeamPlayer* actingDisk = acting->getDisk(0);

   /* Check if we can select the disk */
   assert(BtSoccer::Rules::setDiskAct(actingDisk));

   /* Act */
   BtSoccer::Rules::ballCollideDisk(acting);
   actionFinished();

   /* Check no more moves for current disk */
   assert(!BtSoccer::Rules::setDiskAct(actingDisk));

   /* Select another disk */
   actingDisk = acting->getDisk(1);

   /* Do three consecutive acts with it */
   for(int i=0; i < 3; i++)
   {
      assert(BtSoccer::Rules::setDiskAct(actingDisk));
      BtSoccer::Rules::ballCollideDisk(acting);
      actionFinished();
   }
   /* Verify if the disk can't act again. */
   assert(!BtSoccer::Rules::setDiskAct(actingDisk));

   /* Do remaining acts for the team */
   for(int i=0; i < 8; i++)
   {
      assert(acting == BtSoccer::Rules::getActiveTeam());
      /* Act with alternating disks */
      assert(BtSoccer::Rules::setDiskAct(acting->getDisk(i % 2)));
      BtSoccer::Rules::ballCollideDisk(acting);
      actionFinished();
   }

   /* And see if acting team changes. */
   assert(BtSoccer::Rules::getActiveTeam() != acting);
}

/*********************************************************************
 *                            ballExitTests                          *
 *********************************************************************/
void RulesTestCase::ballExitTests()
{
   Ogre::LogManager::getSingleton().getDefaultLog()->logMessage(
         "\tballExitTests...");

   BtSoccer::Team* acting = BtSoccer::Rules::getActiveTeam();
   BtSoccer::TeamPlayer* actingDisk = acting->getDisk(0);

   /***************
    * Side checks * 
    ***************/

   /* Send a ball to some side*/  
   BtSoccer::Rules::setDiskAct(actingDisk);
   BtSoccer::Rules::ballCollideDisk(acting);
   BtSoccer::Rules::ballExitAtSide(0.0f, 0.0f);
   actionFinished();

   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() != acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_THROW_IN);

   /* Get new acting */
   acting = BtSoccer::Rules::getActiveTeam();
   actingDisk = acting->getDisk(0);

   /* Send a ball to some side after touching enemy */
   BtSoccer::Rules::setDiskAct(actingDisk);
   BtSoccer::Rules::ballCollideDisk(acting);
   BtSoccer::Rules::ballCollideDisk(BtSoccer::Rules::getOtherTeam(acting));
   BtSoccer::Rules::ballExitAtSide(0.0f, 0.0f);
   actionFinished();

   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() == acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_THROW_IN);

   actingDisk = acting->getDisk(1);

   /* Send a ball to some side and touch the ball with an enemy after */
   BtSoccer::Rules::setDiskAct(actingDisk);
   BtSoccer::Rules::ballCollideDisk(acting);
   BtSoccer::Rules::ballExitAtSide(0.0f, 0.0f);
   BtSoccer::Rules::ballCollideDisk(BtSoccer::Rules::getOtherTeam(acting));
   actionFinished();

   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() != acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_THROW_IN);

   acting = BtSoccer::Rules::getActiveTeam();
   actingDisk = acting->getDisk(0);

   /* Send a ball to side, and do a disk collision */
   BtSoccer::Rules::setDiskAct(actingDisk);
   BtSoccer::Rules::ballCollideDisk(acting);
   BtSoccer::Rules::ballExitAtSide(0.0f, 0.0f);
   BtSoccer::Rules::diskCollideDisk(acting, BtSoccer::Rules::getOtherTeam(acting), 1.0f, 1.0f);
   actionFinished();

   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() != acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_THROW_IN);
   assert(BtSoccer::Rules::getBall()->getPosition().x == 0.0f);

   acting = BtSoccer::Rules::getActiveTeam();
   actingDisk = acting->getDisk(0);

   /* Touch own disk, touch ball and exit it at side. */
   BtSoccer::Rules::setDiskAct(actingDisk);
   BtSoccer::Rules::diskCollideDisk(acting, acting, 1.0f, 1.0f);
   BtSoccer::Rules::ballCollideDisk(acting);
   BtSoccer::Rules::ballExitAtSide(0.0f, 0.0f);
   actionFinished();

   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() != acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_THROW_IN);
   assert(BtSoccer::Rules::getBall()->getPosition().x == 0.0f);

   acting = BtSoccer::Rules::getActiveTeam();
   actingDisk = acting->getDisk(0);

   /* Touch enemy disk, touch ball and exit it at side. */
   BtSoccer::Rules::setDiskAct(actingDisk);
   BtSoccer::Rules::diskCollideDisk(acting, BtSoccer::Rules::getOtherTeam(acting), 1.0f, 1.0f);
   BtSoccer::Rules::ballCollideDisk(acting);
   BtSoccer::Rules::ballExitAtSide(0.0f, 0.0f);
   actionFinished();

   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() != acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_FREE_KICK);
   assert(BtSoccer::Rules::getBall()->getPosition().x == 1.0f);

   acting = BtSoccer::Rules::getActiveTeam();
   actingDisk = acting->getDisk(0);

   /* Touch enemy disk, touch ball with enemy and exit it at side. */
   BtSoccer::Rules::setDiskAct(actingDisk);
   BtSoccer::Rules::diskCollideDisk(acting, BtSoccer::Rules::getOtherTeam(acting), 1.0f, 1.0f);
   BtSoccer::Rules::ballCollideDisk(BtSoccer::Rules::getOtherTeam(acting));
   BtSoccer::Rules::ballExitAtSide(0.0f, 0.0f);
   actionFinished();

   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() != acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_FREE_KICK);
   assert(BtSoccer::Rules::getBall()->getPosition().x == 1.0f);

   /*****************
    * Byline checks * 
    *****************/

   acting = BtSoccer::Rules::getActiveTeam();
   actingDisk = acting->getDisk(0);

   /* Send a ball to own byline */  
   BtSoccer::Rules::setDiskAct(actingDisk);
   BtSoccer::Rules::ballCollideDisk(acting);
   BtSoccer::Rules::ballExitAtByline(acting == BtSoccer::Rules::getUpperTeam(), 0.0f);
   actionFinished();

   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() != acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_CORNER_KICK);

   acting = BtSoccer::Rules::getActiveTeam();
   actingDisk = acting->getDisk(0);

   /* Send a ball to enemy's byline */  
   BtSoccer::Rules::setDiskAct(actingDisk);
   BtSoccer::Rules::ballCollideDisk(acting);
   BtSoccer::Rules::ballExitAtByline(acting != BtSoccer::Rules::getUpperTeam(), 0.0f);
   actionFinished();

   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() != acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_GOAL_KICK);

   acting = BtSoccer::Rules::getActiveTeam();
   actingDisk = acting->getDisk(0);

   /* Send the ball to own byline after it touches the enemy */
   BtSoccer::Rules::setDiskAct(actingDisk);
   BtSoccer::Rules::ballCollideDisk(acting);
   BtSoccer::Rules::ballCollideDisk(BtSoccer::Rules::getOtherTeam(acting));
   BtSoccer::Rules::ballExitAtByline(BtSoccer::Rules::getUpperTeam() == acting, 0.0f);
   actionFinished();

   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() == acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_GOAL_KICK);

   /* Send the ball to enemy's byline after it touches the enemy */
   BtSoccer::Rules::setDiskAct(actingDisk);
   BtSoccer::Rules::ballCollideDisk(acting);
   BtSoccer::Rules::ballCollideDisk(BtSoccer::Rules::getOtherTeam(acting));
   BtSoccer::Rules::ballExitAtByline(BtSoccer::Rules::getUpperTeam() != acting, 0.0f);
   actionFinished();

   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() == acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_CORNER_KICK);

   actingDisk = acting->getDisk(1);

   /* Send the ball to own byline and touch the ball with an enemy after */
   BtSoccer::Rules::setDiskAct(actingDisk);
   BtSoccer::Rules::ballCollideDisk(acting);
   BtSoccer::Rules::ballExitAtByline(acting == BtSoccer::Rules::getUpperTeam(), 0.0f);
   BtSoccer::Rules::ballCollideDisk(BtSoccer::Rules::getOtherTeam(acting));
   actionFinished();

   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() != acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_CORNER_KICK);

   acting = BtSoccer::Rules::getActiveTeam();
   actingDisk = acting->getDisk(0);

   /* Send the ball to own byline and touch the ball with an enemy after */
   BtSoccer::Rules::setDiskAct(actingDisk);
   BtSoccer::Rules::ballCollideDisk(acting);
   BtSoccer::Rules::ballExitAtByline(acting != BtSoccer::Rules::getUpperTeam(), 0.0f);
   BtSoccer::Rules::ballCollideDisk(BtSoccer::Rules::getOtherTeam(acting));
   actionFinished();

   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() != acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_GOAL_KICK);

   acting = BtSoccer::Rules::getActiveTeam();
   actingDisk = acting->getDisk(0);

   /* Send a ball to own byline, and do a disk collision with enemy */
   BtSoccer::Rules::setDiskAct(actingDisk);
   BtSoccer::Rules::ballCollideDisk(acting);
   BtSoccer::Rules::ballExitAtByline(acting == BtSoccer::Rules::getUpperTeam(), 0.0f);
   BtSoccer::Rules::diskCollideDisk(acting, BtSoccer::Rules::getOtherTeam(acting), 1.0f, 1.0f);
   actionFinished();

   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() != acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_CORNER_KICK);
   assert(BtSoccer::Rules::getBall()->getPosition().z != 1.0f);

   acting = BtSoccer::Rules::getActiveTeam();
   actingDisk = acting->getDisk(0);

   /* Send a ball to own byline, and do a disk collision with enemy */
   BtSoccer::Rules::setDiskAct(actingDisk);
   BtSoccer::Rules::ballCollideDisk(acting);
   BtSoccer::Rules::ballExitAtByline(acting != BtSoccer::Rules::getUpperTeam(), 0.0f);
   BtSoccer::Rules::diskCollideDisk(acting, BtSoccer::Rules::getOtherTeam(acting), 1.0f, 1.0f);
   actionFinished();

   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() != acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_GOAL_KICK);
   assert(BtSoccer::Rules::getBall()->getPosition().z != 1.0f);

   acting = BtSoccer::Rules::getActiveTeam();
   actingDisk = acting->getDisk(0);

   /* Touch own disk, touch ball and exit it at own byline. */
   BtSoccer::Rules::setDiskAct(actingDisk);
   BtSoccer::Rules::diskCollideDisk(acting, acting, 1.0f, 1.0f);
   BtSoccer::Rules::ballCollideDisk(acting);
   BtSoccer::Rules::ballExitAtByline(acting == BtSoccer::Rules::getUpperTeam(), 0.0f);
   actionFinished();

   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() != acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_CORNER_KICK);
   assert(BtSoccer::Rules::getBall()->getPosition().z != 1.0f);


   acting = BtSoccer::Rules::getActiveTeam();
   actingDisk = acting->getDisk(0);

   /* Touch own disk, touch ball and exit it at enemy's byline. */
   BtSoccer::Rules::setDiskAct(actingDisk);
   BtSoccer::Rules::diskCollideDisk(acting, acting, 1.0f, 1.0f);
   BtSoccer::Rules::ballCollideDisk(acting);
   BtSoccer::Rules::ballExitAtByline(acting != BtSoccer::Rules::getUpperTeam(), 0.0f);
   actionFinished();

   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() != acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_GOAL_KICK);
   assert(BtSoccer::Rules::getBall()->getPosition().z != 1.0f);

   acting = BtSoccer::Rules::getActiveTeam();
   actingDisk = acting->getDisk(0);

   /* Touch enemy disk, touch ball and exit it at own byline. */
   BtSoccer::Rules::setDiskAct(actingDisk);
   BtSoccer::Rules::diskCollideDisk(acting, BtSoccer::Rules::getOtherTeam(acting), 1.0f, 1.0f);
   BtSoccer::Rules::ballCollideDisk(acting);
   BtSoccer::Rules::ballExitAtByline(acting == BtSoccer::Rules::getUpperTeam(), 0.0f);
   actionFinished();

   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() != acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_FREE_KICK);
   assert(BtSoccer::Rules::getBall()->getPosition().x == 1.0f);

   acting = BtSoccer::Rules::getActiveTeam();
   actingDisk = acting->getDisk(0);

   /* Touch enemy disk, touch ball and exit it at enemy's byline. */
   BtSoccer::Rules::setDiskAct(actingDisk);
   BtSoccer::Rules::diskCollideDisk(acting, BtSoccer::Rules::getOtherTeam(acting), 1.0f, 1.0f);
   BtSoccer::Rules::ballCollideDisk(acting);
   BtSoccer::Rules::ballExitAtByline(acting != BtSoccer::Rules::getUpperTeam(), 0.0f);
   actionFinished();

   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() != acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_FREE_KICK);
   assert(BtSoccer::Rules::getBall()->getPosition().x == 1.0f);

   acting = BtSoccer::Rules::getActiveTeam();
   actingDisk = acting->getDisk(0);

   /* Touch enemy disk, touch ball with enemy and exit it at own byline. */
   BtSoccer::Rules::setDiskAct(actingDisk);
   BtSoccer::Rules::diskCollideDisk(acting, BtSoccer::Rules::getOtherTeam(acting), 1.0f, 1.0f);
   BtSoccer::Rules::ballCollideDisk(BtSoccer::Rules::getOtherTeam(acting));
   BtSoccer::Rules::ballExitAtByline(BtSoccer::Rules::getUpperTeam() == acting, 0.0f);
   actionFinished();

   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() != acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_FREE_KICK);
   assert(BtSoccer::Rules::getBall()->getPosition().x == 1.0f);

   acting = BtSoccer::Rules::getActiveTeam();
   actingDisk = acting->getDisk(0);

   /* Touch enemy disk, touch ball with enemy and exit it at enemy's byline. */
   BtSoccer::Rules::setDiskAct(actingDisk);
   BtSoccer::Rules::diskCollideDisk(acting, BtSoccer::Rules::getOtherTeam(acting), 1.0f, 1.0f);
   BtSoccer::Rules::ballCollideDisk(BtSoccer::Rules::getOtherTeam(acting));
   BtSoccer::Rules::ballExitAtByline(BtSoccer::Rules::getUpperTeam() != acting, 0.0f);
   actionFinished();

   /* Check it */
   assert(BtSoccer::Rules::getActiveTeam() != acting);
   assert(BtSoccer::Rules::getState() == BtSoccer::Rules::STATE_FREE_KICK);
   assert(BtSoccer::Rules::getBall()->getPosition().x == 1.0f);
}

/*********************************************************************
 *                         actionFinished                            *
 *********************************************************************/
void RulesTestCase::actionFinished()
{
   BtSoccer::Rules::ballAtFinalPosition(false);
   BtSoccer::Rules::setPositions();
   BtSoccer::Rules::newTurn();
}


/*********************************************************************
 *                   doSpecificScenarioCreation                      *
 *********************************************************************/
void RulesTestCase::doSpecificScenarioCreation()
{
   /* Nothing needed here. */
}

/*********************************************************************
 *                               doRun                               *
 *********************************************************************/
void RulesTestCase::doRun()
{
   beforeEachTest();
   collisionRelatedTests();

   beforeEachTest();
   remainingTouchesTests();

   beforeEachTest();
   ballExitTests();
}

/*********************************************************************
 *                   doSpecificScenarioFinish                        *
 *********************************************************************/
void RulesTestCase::doSpecificScenarioFinish()
{
   /* nothing created, nothing to finish. */
}


