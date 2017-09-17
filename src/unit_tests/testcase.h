#ifndef _btsoccer_tests_test_case_h
#define _btsoccer_tests_test_case_h

#include <assert.h>
#include <OGRE/OgreLogManager.h>

#include "../engine/team.h"
#include "../engine/field.h"
#include "../engine/ball.h"
#include "../engine/rules.h"
#include "../physics/bulletlink.h"

namespace BtSoccerTests
{

/*! The base class to implement all BtSoccer test cases */
class TestCase
{
   public:
      /*! Constructor 
       * \param createFieldBorders if is needed to create field borders,
       *                           or if just the floor/ground is necessary
       *                           for the specific test implementation. */
      TestCase(bool createFieldBorders);

      /*! Destructor */
      virtual ~TestCase();

      /*! Run the test. */
      void run();

   protected:

      /*! Create the scenario related to a match test.
       * Override if not a related match test (TeamA x TeamB) */
      virtual void createScenario();

      /*! Do any specific scenario creation for the implemented test */
      virtual void doSpecificScenarioCreation()=0;

      /*! Run the specific tests, using assertions */
      virtual void doRun()=0;

      /*! Finish with the scenario created at #createScenario.
       * Override if changed the creation function. */
      virtual void finishScenario();
      
      /*! Finish with any specific scenario created */
      virtual void doSpecificScenarioFinish()=0;

      BtSoccer::Team* teamA; /**< First match team */
      BtSoccer::Team* teamB; /**< Second match team */
      BtSoccer::Field* field; /**< The field */
      BtSoccer::Ball* ball;  /**< The ball */

      Ogre::Log* ogreLog; /**< Log to use */

      bool createFieldBorders; /**< If needed to create field borders. */
      
};

}

#endif


