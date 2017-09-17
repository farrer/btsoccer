#ifndef _btsoccer_gamecenter_network_h
#define _btsoccer_gamecenter_network_h

#include "protocol.h"
#include <kobold/parallelprocess.h>

namespace BtSoccer
{

/*! A game center controller implementation: talk iwith objective-c
 * game center delegate, and also create a thread to send Protocol messages. */
class GameCenterController : public Kobold::ParallelProcess
{
   public:
      /*! Constructor */
      GameCenterController();
      /*! Destructor */
      ~GameCenterController();

      /**! Procedure that will authenticate the user in the game
       * center network. */
      void authenticate();

      /*! \return if user is with the game center view opened, 
       * authenticating. */
      bool isAuthenticating();

      /*! \return if user is authenticated at the game center. */
      bool isEnabled();

      /*! \return if user is canceled the game center. */
      bool isCanceled();

      /*! Open Match subview to host a match. */
      void hostMatch();

      /*! \return true if the host match creation was canceled. */
      bool isMatchCanceled();

      /*! \return true if failed host match creation */
      bool matchCreationFailed();

      /*! \return if a game center match is ready to start */
      bool isMatchReady();

      /*! Verify if match is defined (but not if is ready).
       * \return true if match is defined. */
      bool isMatchDefined();

      /*! \return if the current side is teamA or teamB. */
      bool isTeamA();
   
      /*! Do the step of gamecenter */
      bool step();
   
      /*! Get time to sleep after each step */
      unsigned int getSleepTime();

      /*! Queue a protocol message to send via gamecenter
       * \param msg message to send
       * \param message size
       * \return true if success. */
      bool queueMessage(BtSoccer::ProtocolMessage* msg, unsigned long size);

      /*! Disconnect from the current match */
      void finishMatch();
   
   private:
      /*! Protocol to use. */
      Protocol protocol;
};
   
}

#endif