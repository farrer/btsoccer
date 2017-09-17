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

#include "protocol.h"
#include "../btsoccer.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
   #include "gamecenternetwork.h"
#endif

/* Minimun version required to talk with current protocol (MAJOR.MINOR). */
#define MIN_MAJOR_SUPPORTED_VERSION  1
#define MIN_MINOR_SUPPORTED_VERSION  0

using namespace BtSoccer;

/***********************************************************************
 *                              Constructor                            *
 ***********************************************************************/
void Protocol::initProtocol(bool gameCenter, int fieldConstant)
{
   if(isInited)
   {
      finishProtocol();
   }
   isForTeamA = false;
   initSend = 0;
   endSend = 0;
   initReceived = 0;
   endReceived = 0;
   curQueueToSendInc = 0;
   curReceivedInc = 0;
   doubleSize = sizeof(double);
   messageWaitingForAck = NULL;
   pthread_mutex_init(&mutexSend, NULL);
   pthread_mutex_init(&mutexReceived, NULL);
   haveAckOrNackToSend = false;
   isInited = true;
   usingGameCenter = gameCenter;
   fieldSize = fieldConstant;
}

/***********************************************************************
 *                               Destructor                            *
 ***********************************************************************/
void Protocol::finishProtocol()
{
   pthread_mutex_destroy(&mutexSend);
   pthread_mutex_destroy(&mutexReceived);
   isInited = false;
}

/***********************************************************************
 *                                setTeam                              *
 ***********************************************************************/
void Protocol::setTeam(Ogre::String teamFileName)
{
   teamFile = teamFileName;
}

/***********************************************************************
 *                                getTeam                              *
 ***********************************************************************/
Ogre::String Protocol::getTeam()
{
   return teamFile;
}

/***********************************************************************
 *                                isTeamA                              *
 ***********************************************************************/
bool Protocol::isTeamA()
{
   return isForTeamA;
}

/***********************************************************************
 *                              setIsTeamA                             *
 ***********************************************************************/
void Protocol::setIsTeamA(bool teamA)
{
   isForTeamA = teamA;
}

/***********************************************************************
 *                          isUsingGameCenter                          *
 ***********************************************************************/
bool Protocol::isUsingGameCenter()
{
   return usingGameCenter;
}

/***********************************************************************
 *                            queueMessage                             *
 ***********************************************************************/
void Protocol::queueMessage(ProtocolMessage* msg)
{
   pthread_mutex_lock(&mutexSend);
   
   /* Queue the message using protocol queue */
   if( ((endSend + 1) % PROTOCOL_MAX_QUEUED_MESSAGES) == initSend )
   {
#ifdef BTSOCCER_NET_DEBUG
      printf("Messages to send queue overflow!\n");
#endif
      pthread_mutex_unlock(&mutexSend);
      return;
   }
      
   /* Insert the message on the queue */
   memcpy(&send[endSend], msg, sizeof(ProtocolMessage));
   endSend = (endSend+1) % PROTOCOL_MAX_QUEUED_MESSAGES;
   
   pthread_mutex_unlock(&mutexSend);
}

/***********************************************************************
 *                        getNextMessageToSend                         *
 ***********************************************************************/
bool Protocol::getNextMessageToSend(ProtocolMessage* msg)
{
   bool hasMessage = false;
   pthread_mutex_lock(&mutexSend);
   
   if(haveAckOrNackToSend)
   {
      /* Have an ack or nack to send, must send it. */
#ifdef BTSOCCER_NET_DEBUG
      printf("I will send ack or nack: %d\n", ackNackToSend.type);
#endif
      memcpy(msg, &ackNackToSend, sizeof(ProtocolMessage));
      haveAckOrNackToSend = false;
      hasMessage = true;
   }
   else if(messageWaitingForAck != NULL)
   {
      /* Some message is waiting for ack, must re-send it, as ack
       * was not yet received. */
      if(waitingTimer.getMilliseconds() > PROTOCOL_TIME_TO_RESEND_MS)
      {
         waitingTimer.reset();
#ifdef BTSOCCER_NET_DEBUG
         printf("Will resend: %d\n", messageWaitingForAck->type);
#endif
         memcpy(msg, messageWaitingForAck, sizeof(ProtocolMessage));
         hasMessage = true;
      }
   }
   else if(initSend != endSend)
   {
      //printf("Init: %d End: %d\n", initSend, endSend);
      setSendIncValue(&send[initSend].inc[0]);
      memcpy(msg, &send[initSend], sizeof(ProtocolMessage));
#ifdef BTSOCCER_NET_DEBUG
      printf("Will send: %d\n", send[initSend].type);
#endif
      if(send[initSend].needAck != 0)
      {
#ifdef BTSOCCER_NET_DEBUG
         printf("Will wait for ack\n");
#endif
         waitingTimer.reset();
         /* Must mark we are waiting for it */
         messageWaitingForAck = &send[initSend];
      }
      else
      {
         initSend = (initSend+1)%PROTOCOL_MAX_QUEUED_MESSAGES;
      }
      hasMessage = true;
   }

   pthread_mutex_unlock(&mutexSend);
   return hasMessage;
}

/***********************************************************************
 *                        getLastMessageToSend                         *
 ***********************************************************************/
bool Protocol::getLastMessageToSend(ProtocolMessage* msg)
{
   bool hasMessage = false;
   pthread_mutex_lock(&mutexSend);

   if(initSend != endSend)
   {
      endSend--;
      if(endSend < 0)
      {
         endSend = PROTOCOL_MAX_QUEUED_MESSAGES-1;
      }
      memcpy(msg, &send[endSend], sizeof(ProtocolMessage));
      hasMessage = true;
   }

   pthread_mutex_unlock(&mutexSend);
   return hasMessage;

}

/***********************************************************************
 *                         queueParsedMessage                          *
 ***********************************************************************/
void Protocol::queueParsedMessage(ProtocolParsedMessage* msg)
{
   pthread_mutex_lock(&mutexReceived);
   if( (endReceived + 1) % PROTOCOL_MAX_QUEUED_MESSAGES == initReceived )
   {
#ifdef BTSOCCER_NET_DEBUG
      printf("Messages received queue overflow!\n");
#endif
      pthread_mutex_unlock(&mutexReceived);
      return;
   }
 
   /* Insert the message on the queue */
   received[endReceived].msgType = msg->msgType;
   received[endReceived].msgInfo = msg->msgInfo;
   received[endReceived].msgAditionalInfo = msg->msgAditionalInfo;
   received[endReceived].position = msg->position;
   received[endReceived].angles = msg->angles;
   received[endReceived].str = msg->str;
   endReceived = (endReceived+1) % PROTOCOL_MAX_QUEUED_MESSAGES;
   pthread_mutex_unlock(&mutexReceived);
}

/***********************************************************************
 *                       getNextReceivedMessage                        *
 ***********************************************************************/
bool Protocol::getNextReceivedMessage(ProtocolParsedMessage* msg)
{
   bool hasMessage = false;

   pthread_mutex_lock(&mutexReceived);

   if(initReceived != endReceived)
   {
      msg->msgType = received[initReceived].msgType;
      msg->msgInfo = received[initReceived].msgInfo;
      msg->msgAditionalInfo = received[initReceived].msgAditionalInfo;
      msg->position = received[initReceived].position;
      msg->angles = received[initReceived].angles;
      msg->str = received[initReceived].str;
      initReceived = (initReceived+1)%PROTOCOL_MAX_QUEUED_MESSAGES;
      hasMessage = true;
   }

   pthread_mutex_unlock(&mutexReceived);
   return hasMessage;
}

/***********************************************************************
 *                               queueHello                            *
 ***********************************************************************/
void Protocol::queueHello()
{
   ProtocolMessage msg;
   memset(msg.data, 0, PROTOCOL_DATA_SIZE);
   msg.type = MESSAGE_INIT_CONNECTION;
   msg.needAck = 1;
   msg.data[0] = BTSOCCER_VERSION_MAJOR; 
   msg.data[1] = BTSOCCER_VERSION_MINOR; 
   queueMessage(&msg);
}


/***********************************************************************
 *                           queueSetTeam                              *
 ***********************************************************************/
void Protocol::queueSetTeam(Ogre::String teamFile)
{
   ProtocolMessage msg;
   memset(msg.data, 0, PROTOCOL_DATA_SIZE);
   msg.type = MESSAGE_SET_TEAM;
   msg.needAck = 1;
   msg.data[0] = (isForTeamA)?UPDATE_TYPE_TEAM_A:UPDATE_TYPE_TEAM_B;
   sprintf(&msg.data[1], "%s", teamFile.c_str());

   queueMessage(&msg);
}

/***********************************************************************
 *                          queueSetField                              *
 ***********************************************************************/
void Protocol::queueSetField(int fieldConstant)
{
   ProtocolMessage msg;
   memset(msg.data, 0, PROTOCOL_DATA_SIZE);
   msg.type = MESSAGE_SET_FIELD;
   msg.needAck = 1;
   msg.data[0] = fieldConstant; 
   queueMessage(&msg);
}

/***********************************************************************
 *                       queueBallUpdateToSend                         *
 ***********************************************************************/
void Protocol::queueBallUpdateToSend(Ogre::Vector3 pos, 
      Ogre::Quaternion angles, bool isFinalPosition)
{
   ProtocolMessage msg;
   memset(msg.data, 0, PROTOCOL_DATA_SIZE);
   int i;
  
   /* Define header */ 
   msg.type = MESSAGE_UPDATE_POSITIONS;
   msg.needAck = (isFinalPosition)?1:0;
   msg.data[0] = UPDATE_TYPE_BALL;
   
   /* Copy positions as doubles */
   i = 1;
   setVector3(pos, &msg.data[i]);
   i += 3*doubleSize;

   /* Copy angles as doubles */
   setQuaternion(angles, &msg.data[i]);

   /* Finally, add message to queue */
   queueMessage(&msg);
}

/***********************************************************************
 *                    queueTeamPlayerUpdateToSend                      *
 ***********************************************************************/
void Protocol::queueTeamPlayerUpdateToSend(bool teamA, bool manualInput,
         int diskNumber, Ogre::Vector3 pos, Ogre::Quaternion angles,
         bool isFinalPosition)
{
   ProtocolMessage msg;
   memset(msg.data, 0, PROTOCOL_DATA_SIZE);
   bool useLast = false;
   int needAck = (isFinalPosition)?1:0;
   int desiredMessage;
   
   if(manualInput)
   {
      desiredMessage = (teamA)?
                          UPDATE_TYPE_MANUAL_TEAM_A_INPUT:
                          UPDATE_TYPE_MANUAL_TEAM_B_INPUT;
   }
   else
   {
      desiredMessage = (teamA)?UPDATE_TYPE_TEAM_A:UPDATE_TYPE_TEAM_B;
   }

   /* First, get last added message and see if it is
    * a teamUpdate message of the same team and have
    * some room to add more data to it. */
   if(getLastMessageToSend(&msg))
   {
      if( (msg.type == MESSAGE_UPDATE_POSITIONS) &&
          (msg.data[0] == desiredMessage) &&
          (msg.data[1] < MAX_POSITIONS_PER_MESSAGE) &&
          (msg.needAck == needAck))
      {
         /* Still have room for more positions. */
         useLast = true;
#ifdef BTSOCCER_NET_DEBUG
         printf("Will use last\n");
#endif
      }
      else
      {
         /* Message not of the same type/team or already full
          * Put message back at the queue */
         queueMessage(&msg);
      }
   }
   
   if(!useLast)
   {
#ifdef BTSOCCER_NET_DEBUG
      printf("Won't use last\n");
#endif
      /* define as a new message */
      msg.type = MESSAGE_UPDATE_POSITIONS;
      msg.needAck = needAck;
      msg.data[0] = desiredMessage;
      msg.data[1] = 0; // still without data
   }

   /* Finally, set the data */
   msg.data[1] += 1; //as setting new position.
   int i = (((int)msg.data[1])-1)*(7*doubleSize+1)+2; //3 pos + 4 angles + diskNumber
   msg.data[i] = diskNumber;
   i++;
   /* Set the position */
   setVector3(pos, &msg.data[i]);
   i += 3*doubleSize;
   /* Set the angles*/
   setQuaternion(angles, &msg.data[i]);
   i += 4*doubleSize;
   /*And add it to the queue */
   queueMessage(&msg);
}

/***********************************************************************
 *                        queueResultRules                             *
 ***********************************************************************/
void Protocol::queueRulesResult(int ruleState, bool ballWithTeamA)
{
   ProtocolMessage msg;
   memset(msg.data, 0, PROTOCOL_DATA_SIZE);
   msg.needAck = 1;
   msg.type = MESSAGE_RULES_RESULT;
   msg.data[0] = ruleState;
   msg.data[1] = (ballWithTeamA)?UPDATE_TYPE_TEAM_A:UPDATE_TYPE_TEAM_B;
   queueMessage(&msg);
}

/***********************************************************************
 *                            queueNack                                *
 ***********************************************************************/
void Protocol::queueNack(char nackReason, char extraInfo)
{
   pthread_mutex_lock(&mutexSend);
   
   haveAckOrNackToSend = true;
   memset(ackNackToSend.data, 0, PROTOCOL_DATA_SIZE);
   ackNackToSend.needAck = 0;
   ackNackToSend.type = MESSAGE_NACK;
   ackNackToSend.data[0] = NACK_REASON_EXPECTED_ANOTHER;
   ackNackToSend.data[1] = extraInfo;
   
   pthread_mutex_unlock(&mutexSend);
}

/***********************************************************************
 *                             queueAck                                *
 ***********************************************************************/
void Protocol::queueAck()
{
   pthread_mutex_lock(&mutexSend);
   
   memset(ackNackToSend.data, 0, PROTOCOL_DATA_SIZE);
   ackNackToSend.type = MESSAGE_ACK;
   ackNackToSend.needAck = 0;
   haveAckOrNackToSend = true;
   
   pthread_mutex_unlock(&mutexSend);
}

/***********************************************************************
 *                         queueWillShoot                              *
 ***********************************************************************/
void Protocol::queueWillShoot()
{
   ProtocolMessage msg;
   memset(msg.data, 0, PROTOCOL_DATA_SIZE);
   msg.type = MESSAGE_WILL_SHOOT;
   msg.needAck = 1;
   queueMessage(&msg);
}

/***********************************************************************
 *                      queueGoalKeeperDone                            *
 ***********************************************************************/
void Protocol::queueGoalKeeperDone()
{
   ProtocolMessage msg;
   memset(msg.data, 0, PROTOCOL_DATA_SIZE);
   msg.type = MESSAGE_GOAL_KEEPER_DONE;
   msg.needAck = 1;
   queueMessage(&msg);
}

/***********************************************************************
 *                           queueSoundEffect                          *
 ***********************************************************************/
void Protocol::queueSoundEffect(int soundType, Ogre::Vector3 pos)
{
   ProtocolMessage msg;
   memset(msg.data, 0, PROTOCOL_DATA_SIZE);
   
   /* Define header */
   msg.type = MESSAGE_PLAY_SOUND;
   msg.data[0] = soundType;
   msg.needAck = 0;
   
   /* Define the data as position */
   setVector3(pos, &msg.data[1]);
   
   /* Finally, add message to queue */
   queueMessage(&msg);
}

/***********************************************************************
 *                       queueGoalHappened                             *
 ***********************************************************************/
void Protocol::queueGoalHappened(bool teamAGoal)
{
   ProtocolMessage msg;
   memset(msg.data, 0, PROTOCOL_DATA_SIZE);
   
   /* Define header */
   msg.type = MESSAGE_GOAL;
   msg.needAck = 1;
   
   /* Define the data as position */
   msg.data[0] = (teamAGoal)?UPDATE_TYPE_TEAM_A:UPDATE_TYPE_TEAM_B;
   
   /* Finally, add message to queue */
   queueMessage(&msg);
}

/***********************************************************************
 *                            queuePause                               *
 ***********************************************************************/
void Protocol::queuePause()
{
   ProtocolMessage msg;
   memset(msg.data, 0, PROTOCOL_DATA_SIZE);
   msg.type = MESSAGE_PAUSE;
   msg.needAck = 1;
   queueMessage(&msg);
}

/***********************************************************************
 *                            queueResume                              *
 ***********************************************************************/
void Protocol::queueResume()
{
   ProtocolMessage msg;
   memset(msg.data, 0, PROTOCOL_DATA_SIZE);
   msg.type = MESSAGE_RESUME;
   msg.needAck = 1;
   queueMessage(&msg);
}

/***********************************************************************
 *                          queueBeginHalf                             *
 ***********************************************************************/
void Protocol::queueBeginHalf()
{
   ProtocolMessage msg;
   memset(msg.data, 0, PROTOCOL_DATA_SIZE);
   msg.type = MESSAGE_BEGIN_HALF;
   msg.needAck = 1;
   queueMessage(&msg);
}

/***********************************************************************
 *                           queueEndHalf                              *
 ***********************************************************************/
void Protocol::queueEndHalf()
{
   ProtocolMessage msg;
   memset(msg.data, 0, PROTOCOL_DATA_SIZE);
   msg.type = MESSAGE_END_HALF;
   msg.needAck = 1;
   queueMessage(&msg);
}

/***********************************************************************
 *                          defineGoodbye                              *
 ***********************************************************************/
void Protocol::defineGoodbye(ProtocolMessage* bye)
{
   bye->type = MESSAGE_GOODBYE;
   bye->needAck = 0;
   memset(bye->data, 0, PROTOCOL_DATA_SIZE);
}

/***********************************************************************
 *                       parseReceivedMessage                          *
 ***********************************************************************/
void Protocol::removeMessageWaitingForAck()
{
#ifdef BTSOCCER_NET_DEBUG
   printf("Removed ack wait\n");
#endif
   pthread_mutex_lock(&mutexSend);
   messageWaitingForAck = NULL;
   initSend = (initSend+1)%PROTOCOL_MAX_QUEUED_MESSAGES;
   pthread_mutex_unlock(&mutexSend);
}

/***********************************************************************
 *                         checkIfWaitingAck                           *
 ***********************************************************************/
bool Protocol::checkIfWaitingForAck()
{
   bool res = false;
   pthread_mutex_lock(&mutexSend);
   res = messageWaitingForAck != NULL;
   pthread_mutex_unlock(&mutexSend);
   
   return res;
}

/***********************************************************************
 *                         checkIfWaitingAck                           *
 ***********************************************************************/
bool Protocol::discardReceivedMessage(ProtocolMessage* msg)
{
   if( (msg->type == MESSAGE_ACK) || (msg->type == MESSAGE_NACK) )
   {
      /* Always accept ack or nack. */
      return false;
   }
   
   unsigned long received = 0;
   memcpy(&received, &msg->inc[0], PROTOCOL_INC_SIZE);
   if(received <= curReceivedInc)
   {
      /* Message too old, must discard */
#ifdef BTSOCCER_NET_DEBUG
      printf("Received too old message. Discarted\n");
#endif
      return true;
   }
   /* Message up-to-date, must accept */
   curReceivedInc = received;
   return false;
}

/***********************************************************************
 *                         checkIfWaitingAck                           *
 ***********************************************************************/
void Protocol::setSendIncValue(char* incValue)
{
   curQueueToSendInc++;
   memcpy(&incValue[0], &curQueueToSendInc, PROTOCOL_INC_SIZE);
}

/***********************************************************************
 *                       parseReceivedMessage                          *
 ***********************************************************************/
bool Protocol::parseReceivedMessage(ProtocolMessage* msg)
{
#ifdef BTSOCCER_NET_DEBUG
   printf("Received: %d\n", msg->type);
#endif
   if( (checkIfWaitingForAck()) && (msg->type != MESSAGE_ACK))
   {
#ifdef BTSOCCER_NET_DEBUG
      printf("Message unexpected: %d -> %d\n", msg->type, MESSAGE_ACK);
#endif
      /* Unexpected message received */
      if(msg->needAck)
      {
         if(discardReceivedMessage(msg))
         {
            /* Message was already received, must resend the ack */
            queueAck();
         }
         else
         {
            /* Nack, as unexpected and new! */
            queueNack(NACK_REASON_EXPECTED_ANOTHER, MESSAGE_ACK);
         }
         return true;
      }
   }
   else
   {
      /* Send the ack, if needed. */
      if(msg->needAck)
      {
         queueAck();
      }
      
      if(discardReceivedMessage(msg))
      {
         /* Message is too old to accept. */
         return true;
      }
      
      /* Let's parse the message got */
      switch(msg->type)
      {
         case MESSAGE_ACK:
#ifdef BTSOCCER_NET_DEBUG
            printf("Received ack\n");
#endif
            /* If was waiting for ack, can remove the waiting for ack msg. */
            removeMessageWaitingForAck();
         break;
         case MESSAGE_NACK:
            /* Received an NACK. TODO: must threat it. */
#ifdef BTSOCCER_NET_DEBUG
            printf("Received nack, reason: %d\n", msg->data[0]);
#endif
         break;
         case MESSAGE_INIT_CONNECTION:
         {
#ifdef BTSOCCER_NET_DEBUG
            printf("Received hello\n");
            printf("Client version: %d.%d\n", msg->data[0], msg->data[1]);
#endif
            /* Must check if at last minimum version supported. */
            if( (msg->data[0] >= MIN_MAJOR_SUPPORTED_VERSION) &&
                (msg->data[1] >= MIN_MINOR_SUPPORTED_VERSION) )
            {
               /* must send back the field defined at the user server */
               queueSetField(fieldSize);
               /* must send back the team the user with the server will use. */
               queueSetTeam(teamFile);
               return true;
            }
            else
            {
               /* Version unsupported. TODO: close connection and 
                * set message for user. */
               queueNack(NACK_REASON_DIFFERENT_VERSIONS);
               return true;
            }
         }
         break;
         case MESSAGE_SET_FIELD:
         {
            /* Received set game field. */
            queueReceivedSetField(msg);
         }
         break;
         case MESSAGE_SET_TEAM:
         {
            /* Received the team that will play */
            queueReceivedSetTeam(msg);
         }
         break;
         case MESSAGE_UPDATE_POSITIONS:
         {
            switch(msg->data[0])
            {
               case UPDATE_TYPE_BALL:
               {
                  queueReceivedBallUpdate(msg);
               }
               break;
               case UPDATE_TYPE_TEAM_A:
               case UPDATE_TYPE_TEAM_B:
               case UPDATE_TYPE_MANUAL_TEAM_A_INPUT:
               case UPDATE_TYPE_MANUAL_TEAM_B_INPUT:
               {
                  queueReceivedTeamPlayerUpdate(msg);
               }
               break;
               default:
                  //printf("Unknow update type!\n");
               break;
            }
         }
         break;
         case MESSAGE_PLAY_SOUND:
         {
            queueReceivedSoundEffect(msg);
         }
         break;
         case MESSAGE_GOAL:
         {
            queueReceivedGoal(msg);
         }
         break;
         case MESSAGE_WILL_SHOOT:
         case MESSAGE_GOAL_KEEPER_DONE:
         case MESSAGE_PAUSE:
         case MESSAGE_RESUME:
         case MESSAGE_BEGIN_HALF:
         case MESSAGE_END_HALF:
         {
            ProtocolParsedMessage parsed;
            parsed.msgType = msg->type;
            queueParsedMessage(&parsed);
         }
         break;
         case MESSAGE_RULES_RESULT:
         {
            queueReceivedRulesResult(msg);
         }
         break;
         case MESSAGE_GOODBYE:
            return false;
         break;
         default:
         {
            /* Unknow message or message not supported! */
#ifdef BTSOCCER_NET_DEBUG
            printf("Received unknow or not implemented message: %d\n",
                   msg->type);
#endif
            if(msg->needAck)
            {
               queueNack(NACK_REASON_UNKNOW_MESSAGE);
               return true;
            }
         }
         break;
      }
   }

   return true;
}

/***********************************************************************
 *                             setVector3                              *
 ***********************************************************************/
void Protocol::setVector3(Ogre::Vector3 v, char* data)
{
   double d;
   d = v[0];
   int i = 0;
   memcpy(&data[i], &d, doubleSize);
   i += doubleSize;
   d = v[1];
   memcpy(&data[i], &d, doubleSize);
   i += doubleSize;
   d = v[2];
   memcpy(&data[i], &d, doubleSize);
}

/***********************************************************************
 *                            parseVector3                             *
 ***********************************************************************/
void Protocol::parseVector3(double* vec, char* data)
{
   int i = 0;
   memcpy(&vec[0], &data[i], doubleSize);
   i += doubleSize;
   memcpy(&vec[1], &data[i], doubleSize);
   i += doubleSize;
   memcpy(&vec[2], &data[i], doubleSize);
}

/***********************************************************************
 *                            setQuaternion                            *
 ***********************************************************************/
void Protocol::setQuaternion(Ogre::Quaternion q, char* data)
{
   double d;
   d = q.w;
   int i = 0;
   memcpy(&data[i], &d, doubleSize);
   i += doubleSize;
   d = q.x;
   memcpy(&data[i], &d, doubleSize);
   i += doubleSize;
   d = q.y;
   memcpy(&data[i], &d, doubleSize);
   i += doubleSize;
   d = q.z;
   memcpy(&data[i], &d, doubleSize);
}

/***********************************************************************
 *                          parseQuaternion                            *
 ***********************************************************************/
void Protocol::parseQuaternion(double* vec, char* data)
{
   int i = 0;
   memcpy(&vec[0], &data[i], doubleSize);
   i += doubleSize;
   memcpy(&vec[1], &data[i], doubleSize);
   i += doubleSize;
   memcpy(&vec[2], &data[i], doubleSize);
   i += doubleSize;
   memcpy(&vec[3], &data[i], doubleSize);
}

/***********************************************************************
 *                      queueReceivedSetField                          *
 ***********************************************************************/
void Protocol::queueReceivedSetField(ProtocolMessage* msg)
{
   ProtocolParsedMessage parsed;
   
   parsed.msgType = msg->type;
   parsed.msgInfo = msg->data[0];

#ifdef BTSOCCER_NET_DEBUG
   printf("Received: field: %d\n", parsed.msgInfo);
#endif
   queueParsedMessage(&parsed);
}

/***********************************************************************
 *                     queueReceivedRulesResult                         *
 ***********************************************************************/
void Protocol::queueReceivedRulesResult(ProtocolMessage* msg)
{
#ifdef BTSOCCER_NET_DEBUG
   printf("Received rules result: %d %d\n", msg->data[0], msg->data[1]);
#endif
   ProtocolParsedMessage parsed;
   parsed.msgType = msg->type;
   parsed.msgInfo = msg->data[0];
   parsed.msgAditionalInfo = msg->data[1];
   queueParsedMessage(&parsed);
} 

/***********************************************************************
 *                       queueReceivedSetTeam                          *
 ***********************************************************************/
void Protocol::queueReceivedSetTeam(ProtocolMessage* msg)
{
   ProtocolParsedMessage parsed;
   
   parsed.msgType = msg->type;
   parsed.msgInfo = msg->data[0];
   parsed.str = &msg->data[1];
#ifdef BTSOCCER_NET_DEBUG
   printf("Received: team: %d : %s\n", parsed.msgInfo, parsed.str.c_str());
#endif
   queueParsedMessage(&parsed);
}

/***********************************************************************
 *                      queueReceivedBallUpdate                        *
 ***********************************************************************/
void Protocol::queueReceivedBallUpdate(ProtocolMessage* msg)
{
   ProtocolParsedMessage parsed;

   /* Parse the message */
   double pos[3];
   double angle[4];
   int i = 1;
   parseVector3(pos, &msg->data[i]);
   i += 3*doubleSize;
   parseQuaternion(angle, &msg->data[i]);
   
#ifdef BTSOCCER_NET_DEBUG
   printf("Updated Ball\n");
   printf("   position: %.3f %.3f %.3f\n",
         pos[0], pos[1], pos[2]);
   printf("   angles: %.3f %.3f %.3f %.3f\n",
         angle[0], angle[1], angle[2], angle[3]);
#endif

   /* Define the parsed message */
   parsed.msgType = msg->type;
   parsed.msgInfo = UPDATE_TYPE_BALL;
   parsed.position = Ogre::Vector3(pos[0], pos[1], pos[2]);
   parsed.angles = Ogre::Quaternion(angle[0], angle[1], angle[2], angle[3]);
   queueParsedMessage(&parsed);
}

/***********************************************************************
 *                     queueReceivedDiskCollision                      *
 ***********************************************************************/
void Protocol::queueReceivedSoundEffect(ProtocolMessage* msg)
{
   ProtocolParsedMessage parsed;
   
   /* Parse the message */
   double pos[3];
   parseVector3(pos, &msg->data[1]);
   
   /* Define the parsed message */
   parsed.msgType = msg->type;
   parsed.msgInfo = msg->data[0];
   parsed.position = Ogre::Vector3(pos[0], pos[1], pos[2]);
   queueParsedMessage(&parsed);
}

/***********************************************************************
 *                          queueReceivedGoal                          *
 ***********************************************************************/
void Protocol::queueReceivedGoal(ProtocolMessage* msg)
{
   ProtocolParsedMessage parsed;
   
   parsed.msgType = msg->type;
   parsed.msgInfo = msg->data[0];
   
   queueParsedMessage(&parsed);
}

/***********************************************************************
 *                   queueReceivedTeamPlayerUpdate                     *
 ***********************************************************************/
void Protocol::queueReceivedTeamPlayerUpdate(ProtocolMessage* msg)
{
   double pos[3];
   double angle[4];
   int i, j;

   ProtocolParsedMessage parsed;
   parsed.msgType = msg->type;
   parsed.msgInfo = msg->data[0];
#ifdef BTSOCCER_NET_DEBUG
   printf("Updated Team %d\n", parsed.msgInfo);
#endif

   for(i=0; i<msg->data[1]; i++)
   {
      j = i*(7*doubleSize+1)+2;
      parsed.msgAditionalInfo = msg->data[j];
      j++;
      parseVector3(pos, &msg->data[j]);
      parsed.position = Ogre::Vector3(pos[0], pos[1], pos[2]);
      j += 3*doubleSize;
      parseQuaternion(angle, &msg->data[j]);
      parsed.angles = Ogre::Quaternion(angle[0], angle[1], angle[2], angle[3]);
      queueParsedMessage(&parsed);
#ifdef BTSOCCER_NET_DEBUG
      printf("   disk: %d - pos(%.3f, %.3f, %.3f) a(%.3f, %.3f, %.3f, %.3f)\n",
            parsed.msgAditionalInfo, parsed.position[0], parsed.position[1],
            parsed.position[2], parsed.angles[0], parsed.angles[1],
            parsed.angles[2], parsed.angles[3]);
#endif
   }

}

ProtocolMessage Protocol::send[PROTOCOL_MAX_QUEUED_MESSAGES];
int Protocol::endSend;
int Protocol::initSend;
ProtocolParsedMessage Protocol::received[PROTOCOL_MAX_QUEUED_MESSAGES];
int Protocol::initReceived;
int Protocol::endReceived;
unsigned long int Protocol::doubleSize;
bool Protocol::isForTeamA;
bool Protocol::isInited;
bool Protocol::usingGameCenter;
pthread_mutex_t Protocol::mutexSend;
pthread_mutex_t Protocol::mutexReceived;
Ogre::String Protocol::teamFile;
int Protocol::fieldSize;
ProtocolMessage* Protocol::messageWaitingForAck;
unsigned long Protocol::curQueueToSendInc;
unsigned long Protocol::curReceivedInc;
ProtocolMessage Protocol::ackNackToSend;
bool Protocol::haveAckOrNackToSend;
Kobold::Timer Protocol::waitingTimer;

