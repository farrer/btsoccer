
#include "../../src/engine/core.h"

Goblin::BaseApp* createApplication()
{
   Goblin::CameraConfig cameraConfig;
   cameraConfig.angularVelocity = 4.0f;

   return new BtSoccer::Core(cameraConfig);
}

