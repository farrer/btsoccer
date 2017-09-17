#ifndef _btsoccer_tests_rules_test_case_h
#define _btsoccer_tests_rules_test_case_h

#include "testcase.h"

#include "../engine/rules.h"

namespace BtSoccerTests
{

/*! Unit test case to stress our rules implementation. */
class RulesTestCase : public TestCase
{
   public:
      /*! Constructor */
      RulesTestCase();
      /*! Destructor */
      ~RulesTestCase();

   protected:
      void doSpecificScenarioCreation();
      void doRun();
      void doSpecificScenarioFinish();

   private:
      /*! Things to be cleared, recreated before each internal 
       * test execution  */
      void beforeEachTest();

      /*! Do all tests related to collision between ball and disks 
       * (with fouls, pocession change, etc). */
      void collisionRelatedTests();

      /*! Test the remaining touches functionality. */
      void remainingTouchesTests();

      /*! Tests where ball exit the field. */
      void ballExitTests();

      /*! Do the common things to set the turn action as finished. */
      void actionFinished();

};

}

#endif

