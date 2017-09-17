#ifndef _btsoccer_tests_fobject_test_case_h
#define _btsoccer_tests_fobject_test_case_h

#include "testcase.h"

#include "../engine/fobject.h"

namespace BtSoccerTests
{

/*! Unit test case to stress FieldObject related functions. */
class FieldObjectTestCase : public TestCase
{
   public:
      /*! Constructor */
      FieldObjectTestCase();
      /*! Destructor */
      ~FieldObjectTestCase();

   protected:
      void doSpecificScenarioCreation();
      void doRun();
      void doSpecificScenarioFinish();

   private:
      /*! Test the function "hasFreeWayTo" */
      void testHasFreeWayTo();

      BtSoccer::TeamPlayer* diskA;
      BtSoccer::TeamPlayer* diskB;
};

}

#endif
