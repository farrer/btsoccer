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


#ifndef _btsoccer_tcp_network_h
#define _btsoccer_tcp_network_h

#include <kobold/network.h>
#include <kobold/parallelprocess.h>
#include <OGRE/OgreString.h>
#include "protocol.h"

namespace BtSoccer
{

/*! The TCP protocol server implementation for BtSoccer */
class TcpServer: public Kobold::NetServer, public Kobold::ParallelProcess,
                 public Protocol
{
   public:
      /*! Server construction.
       * \param port port to listen.
       * \param teamFileName filename of the team used by the server's user
       * \param fieldSize field constant of the field defined. */
      TcpServer(unsigned short int port, Ogre::String teamFileName,
         int fieldSize);
      /*! Destructor */
      ~TcpServer();

      /*! Called when received message from the connected client.
       * \param clientSocket socket where received the message */
      void receivedMessage(unsigned short int clientSocket);

      /*! Called when receives a new client connection. */
      void receivedNewConnection(unsigned short int clientSocket);

      /*! Do the server step */
      bool step();
   
      unsigned int getExecutionFrequency();

   protected:

      /*! Send a goodbye message to the client. It's called just before
       * closing the connection with it. */
      void sendByeMessage(unsigned short int clientSocket);
};

/*! The TCP client implementation */
class TcpClient : protected Kobold::NetClient, public Kobold::ParallelProcess,
                  public Protocol
{
   public:
      /*! Contructor
       * \param teamFileName filename of the team used by the client
       * \param fieldConstant current size constant. */
      TcpClient(Ogre::String teamFileName, int fieldConstant);
      /*! Destructor. */
      ~TcpClient();

      /*! Connect to a server throught a port */
      bool connect(unsigned short int port, string serverAddr);

      /*! Try to receive messages from server */
      bool receiveMessages();

      /*! Do the client single step, sending queue and receiving messages */
      bool step();
     
      /*! Send all queued messages to the server */ 
      void sendQueue();
   
      unsigned int getExecutionFrequency();

   protected:
      fd_set masterSet;            /**< Master fd list for listen */
      fd_set readSet;              /**< Read fd list for select() */
   
};


}

#endif

