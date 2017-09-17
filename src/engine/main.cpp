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

#include "core.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
   #include "windows.h"
#endif
 

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS &&\
    OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
/***********************************************************************
 *                                main                                 *
 ***********************************************************************/
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
      LPSTR lpCmdLine, int nCmdShow) 
#else
int main (int argc, char *argv[])
#endif
{
   /* Create and run the game */
   Goblin::CameraConfig cameraConfig;
   cameraConfig.angularVelocity = 4.0f;

   BtSoccer::Core* btsoccerGame = new BtSoccer::Core(cameraConfig);
   btsoccerGame->run();
   delete btsoccerGame;
#if 0
  if(argc == 1)
  {
     /* Act as client */
     BtSoccer::TcpClient* tcpClient = new BtSoccer::TcpClient("chile");
     if(tcpClient->connect(7089, "127.0.0.1"))
     {
        tcpClient->createThread();
     }

     tcpClient->queueRulesResult(5, UPDATE_TYPE_TEAM_A);
     tcpClient->queueBallUpdateToSend(Ogre::Vector3(1.0f, 2.0f, 3.0f),
           Ogre::Quaternion(4.0f, 5.0f, 7.0f, 20.0f));
     for(int i=0; i<300; i++)
     {
        tcpClient->queueTeamPlayerUpdateToSend(false, i,
           Ogre::Vector3(i*3, i*3+1, i*3+2),
           Ogre::Quaternion(i*6, i*6+1, i*6+2, 10));
     }
     tcpClient->queueRulesResult(5, UPDATE_TYPE_TEAM_A);
  }
  else if(argc == 2)
  {
     /* Act as server */
     BtSoccer::TcpServer* tcpServer = new BtSoccer::TcpServer(7089, "argentina",
           2);
     tcpServer->init();
     tcpServer->createThread();
  }
  while(true);
#endif
}
#endif
