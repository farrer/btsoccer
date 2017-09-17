/*
   BtSoccer - button football (soccer) game
   Copyright (C) 2008-2015 DNTeam <btsoccer@dnteam.org>

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


#include "tcpnetwork.h"
#include "../btsoccer.h"

using namespace BtSoccer;

///////////////////////////////////////////////////////////////////////////
//                                                                       //
//                               TcpServer                               //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

void* serverThreadProc(void* arg)
{
   TcpServer* tcpServer = (TcpServer*) arg;

   while(tcpServer->isRunning())
   {
      tcpServer->step();
   }

   return NULL;
}

/***********************************************************************
 *                              Constructor                            *
 ***********************************************************************/
TcpServer::TcpServer(unsigned short int port, Ogre::String teamFileName,
                     int fieldSize) :
     Kobold::NetServer(port)
{
   Protocol::teamFile = teamFileName;
   initProtocol(false, fieldSize);
   setIsTeamA(true);
}

/***********************************************************************
 *                               Destructor                            *
 ***********************************************************************/
TcpServer::~TcpServer()
{
   if(isRunning())
   {
      endThread();
   }
   finish();
   finishProtocol();
}

/***********************************************************************
 *                        receivedNewConnection                        *
 ***********************************************************************/
void TcpServer::receivedNewConnection(unsigned short int clientSocket)
{
   if(getTotal() > 1)
   {
      /* Already have a client connected. Refuse it. */
      sendByeMessage(clientSocket);
      closeConnection(clientSocket);
   }
}

/***********************************************************************
 *                           receivedMessage                           *
 ***********************************************************************/
void TcpServer::receivedMessage(unsigned short int clientSocket)
{
   ProtocolMessage msg;

#ifdef BTSOCCER_NET_DEBUG
   printf("Server: received message\n");
#endif
   
   int n = receiveData(clientSocket, &msg, sizeof(ProtocolMessage));
#ifdef BTSOCCER_NET_DEBUG
   printf("n: %d\n", n);
#endif
   if(n > 0)
   {
      if(!parseReceivedMessage(&msg))
      {
         /* Received goodbye from client */
         closeConnection(clientSocket, false);
      }
   }
   else if(n == 0)
   {
      /* Received client disconnected! */
      closeConnection(clientSocket, false);
      return;
   }
}

/***********************************************************************
 *                             sendByeMessage                          *
 ***********************************************************************/
void TcpServer::sendByeMessage(unsigned short int clientSocket)
{
   ProtocolMessage msg;
   defineGoodbye(&msg);
   sendData(clientSocket, &msg, sizeof(ProtocolMessage));
}

/***********************************************************************
 *                            getSleepTime                             *
 ***********************************************************************/
unsigned int TcpServer::getExecutionFrequency()
{
   /* At TCP server, sleep is with recv */
   return 0;
}

/***********************************************************************
 *                                 doStep                              *
 ***********************************************************************/
bool TcpServer::step()
{
   /*! Receive messages and connections */
   if(!doStep())
   {
      return false;
   }

   /* Send pending messages*/
   ProtocolMessage msg;

   //FIXME: error control.
   while(getNextMessageToSend(&msg))
   {
#ifdef BTSOCCER_NET_DEBUG
      printf("Will send data msg->type: %d\n", msg.type);
#endif
      /* Send message to the only client (but at a loop for compatibility) */
      broadcast(&msg, sizeof(ProtocolMessage));
   }
   return true;
}

///////////////////////////////////////////////////////////////////////////
//                                                                       //
//                               TcpClient                               //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

/***********************************************************************
 *                              Constructor                            *
 ***********************************************************************/
TcpClient::TcpClient(Ogre::String teamFileName, int fieldConstant)
{
    /* Clear structs */
    FD_ZERO(&masterSet);
    FD_ZERO(&readSet);

    Protocol::teamFile = teamFileName;
    initProtocol(false, fieldConstant);
    setIsTeamA(false);
}

/***********************************************************************
 *                               Destructor                            *
 ***********************************************************************/
TcpClient::~TcpClient()
{
   if(isRunning())
   {
      endThread();
   }
   finishProtocol();
}

/***********************************************************************
 *                               connect                               *
 ***********************************************************************/
bool TcpClient::connect(unsigned short int port, string serverAddr)
{
   if(createConnection(port, serverAddr))
   {
      FD_SET(usedSocket, &masterSet);
      queueHello();
      return true;
   }
   return false;
}

/***********************************************************************
 *                              sendQueue                              *
 ***********************************************************************/
void TcpClient::sendQueue()
{
   ProtocolMessage msg;

   //FIXME: error control.
   while(getNextMessageToSend(&msg))
   {
#ifdef BTSOCCER_NET_DEBUG
      printf("Will send data msg->type: %d\n", msg.type);
#endif
      sendData(usedSocket, &msg, sizeof(ProtocolMessage));
   }
}


/***********************************************************************
 *                           receiveMessages                           *
 ***********************************************************************/
bool TcpClient::receiveMessages()
{
   if(!connected)
   {
#ifdef BTSOCCER_NET_DEBUG
      printf("Not connnected\n");
#endif
      return false;
   }

   struct timeval timeout;
   timeout.tv_sec = 0;
   timeout.tv_usec = 1000;

   readSet = masterSet;
   if(select(usedSocket+1, &readSet, NULL, NULL, &timeout) == -1)
   {
      error("Error on select!");
      return false;
   }

   if(FD_ISSET(usedSocket, &readSet))
   {
#ifdef BTSOCCER_NET_DEBUG
      printf("Client: received message\n");
#endif
      ProtocolMessage msg;

      int n = receiveData(usedSocket, &msg, sizeof(ProtocolMessage));
      if(n > 0)
      {
         if(!parseReceivedMessage(&msg))
         {
            /* Received goodbye from server */
            closeConnection();
            return false;
         }
      }
      else if(n == 0)
      {
         closeConnection();
         return false;
      }
   }

   return true;
}

/***********************************************************************
 *                            getSleepTime                             *
 ***********************************************************************/
unsigned int TcpClient::getExecutionFrequency()
{
   /* At TCP client, sleep is with select */
   return 0;
}

/***********************************************************************
 *                                step                                 *
 ***********************************************************************/
bool TcpClient::step()
{
   if(!isConnected())
   {
      return false;
   }
   
   if(!receiveMessages())
   {
#ifdef BTSOCCER_NET_DEBUG
      printf("Probably disconnected!\n");
#endif
      return false;
   }
   sendQueue();

   return true;
}

