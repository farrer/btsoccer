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

#include "field.h"

using namespace BtSoccer;

#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreSceneNode.h>
#include <kobold/defparser.h>
#include <goblin/camera.h>

/*********************************************************************
 *                            Constructor                            *
 *********************************************************************/
Field::Field()
{
   fieldFile = "";
   goals = NULL;
   ogreField = NULL;
   numberOfDisks = 10;
#if BTSOCCER_RENDER_DEBUG
   mLines = NULL;
#endif
}

/*********************************************************************
 *                             Destructor                            *
 *********************************************************************/
Field::~Field()
{
#if BTSOCCER_RENDER_DEBUG
   if(mLines != NULL)
   {
      pSceneManager->getRootSceneNode()->detachObject(mLines);
      delete mLines;
   }
#endif
}

/*********************************************************************
 *                             loadField                             *
 *********************************************************************/
void Field::loadField(Ogre::String fileName,
            Ogre::SceneManager* ogreSceneManager)
{
   Kobold::DefParser def;
   Ogre::String value, key;
   Ogre::Entity* fieldModel = NULL;
   float x=0.0f,y=0.0f,z=0.0f;
   
   if(!def.load(fileName))
   {
      return;
   }
   
   while(def.getNextTuple(key, value))
   {
      if(key == "model")
      {
         if(!fieldModel)
         {
            /* Load field model */
            fieldModel = ogreSceneManager->createEntity("Field", value, "game");
         }
      }
      else if(key == "scale")
      {
         sscanf(value.c_str(), "%f %f %f", &x, &y, &z);
         scale[0] = x;
         scale[1] = y;
         scale[2] = z;
      }
      else if(key == "halfSize")
      {
         sscanf(value.c_str(), "%f %f", &x, &z);
         halfSize[0] = x;
         halfSize[1] = z;
      }
      else if(key == "goalPosition")
      {
         sscanf(value.c_str(), "%f", &goalPosition);
      }
      else if(key == "sideDelta")
      {
         sscanf(value.c_str(), "%f %f", &x, &z);
         sideDelta[0] = x;
         sideDelta[1] = z;
      }
      else if(key == "border")
      {
         sscanf(value.c_str(), "%f %f %f", &x, &y, &z);
         border[0] = x;
         border[1] = y;
         border[2] = z;
      }
      else if(key == "borderDelta")
      {
         sscanf(value.c_str(), "%f %f", &x, &z);
         borderDelta[0] = x;
         borderDelta[1] = z;
      }
      else if(key == "littleAreaDelta")
      {
         sscanf(value.c_str(), "%f %f", &x, &z);
         littleAreaDelta[0] = x;
         littleAreaDelta[1] = z;
      }
      else if(key == "penaltyAreaDelta")
      {
         sscanf(value.c_str(), "%f %f", &x, &z);
         penaltyAreaDelta[0] = x;
         penaltyAreaDelta[1] = z;
      }
      else if(key == "penaltyMark")
      {
         sscanf(value.c_str(), "%f", &penaltyMark);
      }
      else if(key == "numberOfDisks")
      {
         sscanf(value.c_str(), "%d", &numberOfDisks);
      }
   }
   
#if BTSOCCER_RENDER_DEBUG
   if(mLines == NULL)
   {
      mLines = new Ogre::ManualObject("fielDebugLines");
      Ogre::String matName = "field_debug";
      
      ogreSceneManager->getRootSceneNode()->attachObject(mLines);
      
      mLines->begin( matName, Ogre::RenderOperation::OT_LINE_LIST );
      mLines->position( Ogre::Vector3::ZERO );
      mLines->colour( Ogre::ColourValue::Red );
      mLines->position( Ogre::Vector3::ZERO );
      mLines->colour( Ogre::ColourValue::Red );
      mLines->end();
   }
#endif
   
   /* Positionate field model at the static scenary */
   ogreField = ogreSceneManager->createStaticGeometry("Field");
   ogreField->addEntity(fieldModel, Ogre::Vector3(0.0f, 0.15f, -0.05f),
                        Ogre::Quaternion(0,0,0,0), scale);
   ogreField->build();
   
   /* Unload the entity (as no more needed) */
   pSceneManager->destroyEntity(fieldModel);
}

/*********************************************************************
 *                            createField                            *
 *********************************************************************/
void Field::createField(Ogre::String fileName,
            Ogre::SceneManager* ogreSceneManager)
{
   if(fileName != fieldFile)
   {
      if(ogreField != NULL)
      {
         /* Delete current field */
         deleteField();
      }
      fieldFile = fileName;
      if(ogreSceneManager == NULL)
      {
         /* Use previously defined scene manager */
         ogreSceneManager = pSceneManager;
      }
      else
      {
         /* Define scene manager */
         pSceneManager = ogreSceneManager;
      }
   
      /* Load field definition */
      loadField(fileName, ogreSceneManager);

      /* Create the goals */
      goals = new Goals(ogreSceneManager, this);

      /* Create the correspondent bullet colliders */
      createCollisionShapes(true);
      
      /* And limit the camera to be somewat inner its dimensions */
      Ogre::Vector3 min = Ogre::Vector3(-halfSize[0], -20.0f, -halfSize[1]);
      Ogre::Vector3 max = Ogre::Vector3(halfSize[0], 20.0f, halfSize[1]);
      Goblin::Camera::limitCameraArea(min, max);
   }
}

/*********************************************************************
 *                            createField                            *
 *********************************************************************/
void Field::createFieldForTestCases(bool createSideShapes)
{
   /* Pre-defined values for test cases (same as professional) */
   scale[0] = 2.25f;
   scale[1] = 2.25f;
   scale[2] = 2.7f;
   halfSize[0] = 19.1f;
   halfSize[1] = 12.85f;
   goalPosition = 17.025f;
   sideDelta[0] = 2.054f;
   sideDelta[1] = 1.904f;
   border[0] = 0.5f;
   border[1] = 0.23f;
   border[2] = 0.35f;
   borderDelta[0] = 0.35f;
   borderDelta[1] = 0.35f;
   littleAreaDelta[0] = 4.325f;
   littleAreaDelta[1] = 3.1525f;
   penaltyAreaDelta[0] = 10.99f;
   penaltyAreaDelta[1] = 6.31f;
   penaltyMark = 6.225f;
   numberOfDisks = 10;

   /* define the shapes (without models) */
   createCollisionShapes(createSideShapes);
   goals = new Goals(this);
}

/*********************************************************************
 *                     createCollisionShapes                         *
 *********************************************************************/
void Field::createCollisionShapes(bool createSideShapes)
{
   /* Ground Field */
   groundShape = new btStaticPlaneShape(btVector3(0,1,0), 1);
   groundMotionState = new btDefaultMotionState(btTransform(
         btQuaternion(0,0,0,1), btVector3(0,-1,0)));
   btRigidBody::btRigidBodyConstructionInfo
      groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0,0,0));
   groundRigidBody = new btRigidBody(groundRigidBodyCI);
   groundRigidBody->setRestitution(FIELD_RESTITUTION);
   groundRigidBody->setFriction(FIELD_FRICTION);
   groundRigidBody->setRollingFriction(FIELD_ROLLING_FRICTION);
   groundRigidBody->setSpinningFriction(FIELD_SPINNING_FRICTION);
   BulletLink::addRigidBody(groundRigidBody);

   if(!createSideShapes)
   {
      /* Only the floor is necessary, so exit. */
      for(int i=0; i <4; i++)
      {
         sideRigidBody[i]  = NULL;
      }

      return;
   }

   /* Field Sides */
   float sizeX=halfSize.x;
   float sizeZ=border.z;
   for(int i=0; i < 4; i++)
   {
      sideShape[i] = new btBoxShape(btVector3(sizeX, border.y, sizeZ) 
            * OGRE_TO_BULLET_FACTOR);
      sideMotionState[i] = new btDefaultMotionState(btTransform(
               btQuaternion(0,0,0,1), btVector3(0,-1,0)));
      btRigidBody::btRigidBodyConstructionInfo
         sideRigidBodyCI(0,sideMotionState[i],sideShape[i],btVector3(0,0,0));
      sideRigidBody[i] = new btRigidBody(sideRigidBodyCI);
      sideRigidBody[i]->setRestitution(0.5f);
      sideRigidBody[i]->setFriction(0.2f);
      sideRigidBody[i]->setRollingFriction(0.2f);
      BulletLink::addRigidBody(sideRigidBody[i]);
      /* Redefine for next two sides */
      if(i==1)
      {
        sizeX = border.x;
        sizeZ = halfSize[1];
      }
   }

   /* Set side positions */
   float halfHeight = border.y / 2.0f;
   btTransform transform = sideRigidBody[0]->getCenterOfMassTransform();
   transform.setOrigin(btVector3(0, halfHeight, -halfSize[1]-borderDelta[1])
         * OGRE_TO_BULLET_FACTOR);
   sideRigidBody[0]->setCenterOfMassTransform(transform);
   
   transform = sideRigidBody[1]->getCenterOfMassTransform();
   transform.setOrigin(btVector3(0, halfHeight, halfSize[1]+borderDelta[1])
         * OGRE_TO_BULLET_FACTOR);
   sideRigidBody[1]->setCenterOfMassTransform(transform);

   transform = sideRigidBody[2]->getCenterOfMassTransform();
   transform.setOrigin(btVector3(-halfSize[0]-borderDelta[0], halfHeight, 0)
         * OGRE_TO_BULLET_FACTOR);
   sideRigidBody[2]->setCenterOfMassTransform(transform);

   transform = sideRigidBody[3]->getCenterOfMassTransform();
   transform.setOrigin(btVector3(halfSize[0]+borderDelta[0], halfHeight, 0)
         * OGRE_TO_BULLET_FACTOR);
   sideRigidBody[3]->setCenterOfMassTransform(transform);
}

/*********************************************************************
 *                            deleteField                            *
 *********************************************************************/
void Field::deleteField()
{
   /* Unload Field model */
   if(ogreField != NULL)
   {
      pSceneManager->destroyStaticGeometry(ogreField);
   }

   /* Unload goals */
   if(goals != NULL)
   {
      delete goals;
   }

   /* Unload collision shapes */
   BulletLink::removeRigidBody(groundRigidBody);
   delete groundRigidBody;
   delete groundShape;
   delete groundMotionState;

   for(int i=0; i <4; i++)
   {
      if(sideRigidBody[i] != NULL)
      {
         BulletLink::removeRigidBody(sideRigidBody[i]);
         delete sideRigidBody[i];
         delete sideShape[i];
         delete sideMotionState[i];
      }
   }

}

/*********************************************************************
 *                             getByline                             *
 *********************************************************************/
Ogre::Real Field::getByline(bool isUpper)
{
   if(isUpper)
   {
      return (halfSize[0] - sideDelta[0]);
   }
   else 
   {
      return (-halfSize[0] + sideDelta[0]);
   }
}

/*********************************************************************
 *                            getHalfSize                            *
 *********************************************************************/
Ogre::Vector2 Field::getHalfSize()
{
   return halfSize;
}

/*********************************************************************
 *                          getGoalPosition                          *
 *********************************************************************/
Ogre::Real Field::getGoalPosition()
{
   return goalPosition;
}

/*********************************************************************
 *                            getSideDelta                           *
 *********************************************************************/
Ogre::Vector2 Field::getSideDelta()
{
   return sideDelta;
}

/*********************************************************************
 *                              getBorder                            *
 *********************************************************************/
Ogre::Vector3 Field::getBorder()
{
   return border;
}

/*********************************************************************
 *                           getBorderDelta                          *
 *********************************************************************/
Ogre::Vector2 Field::getBorderDelta()
{
   return borderDelta;
}

/*********************************************************************
 *                         getLittleAreaDelta                        *
 *********************************************************************/
Ogre::Vector2 Field::getLittleAreaDelta()
{
   return littleAreaDelta;
}

/*********************************************************************
 *                        getPenaltyAreaDelta                        *
 *********************************************************************/
Ogre::Vector2 Field::getPenaltyAreaDelta()
{
   return penaltyAreaDelta;
}

/*********************************************************************
 *                           getPenaltyMark                          *
 *********************************************************************/
Ogre::Real Field::getPenaltyMark()
{
   return penaltyMark;
}

/*********************************************************************
 *                          getNumberOfDisks                         *
 *********************************************************************/
int Field::getNumberOfDisks()
{
   return numberOfDisks;
}

/*********************************************************************
 *                          setNumberOfDisks                         *
 *********************************************************************/
void Field::setNumberOfDisks(int n)
{
   numberOfDisks = n;
}

/*********************************************************************
 *                              getGoals                             *
 *********************************************************************/
BtSoccer::Goals* Field::getGoals()
{
   return goals;
}

/*********************************************************************
 *                            getFileName                            *
 *********************************************************************/
Ogre::String Field::getFileName()
{
   return fieldFile;
}

/*********************************************************************
 *                            getUpGoalBox                           *
 *********************************************************************/
Ogre::AxisAlignedBox Field::getUpGoalBox()
{
   return(goals->getUpGoalBox());
}

/*********************************************************************
 *                           getDownGoalBox                          *
 *********************************************************************/
Ogre::AxisAlignedBox Field::getDownGoalBox()
{
   return(goals->getDownGoalBox());
}

/*********************************************************************
 *                         getNearGoalFactor                         *
 *********************************************************************/
float Field::getNearGoalFactor(bool upperTeam, float posX, float posZ)
{
   return(goals->getNearGoalFactor(upperTeam, posX, posZ));
}

/*********************************************************************
 *                         isInnerLittleArea                         *
 *********************************************************************/
bool Field::isInnerLittleArea(Ogre::Real x, Ogre::Real z)
{
   Ogre::Real upBottom = halfSize[0]-littleAreaDelta[0];
   Ogre::Real upTop = halfSize[0]-sideDelta[0];
   
   Ogre::Real left = -littleAreaDelta[1];
   Ogre::Real right = littleAreaDelta[1];
   
   Ogre::Real downBottom = -halfSize[0]+sideDelta[0];
   Ogre::Real downTop = -halfSize[0]+littleAreaDelta[0];
   
   if( (z >= left) && (z <= right) )
   {
      if( (x >= upBottom) && (x <= upTop) )
      {
         return true;
      }
      
      if( (x >= downBottom) && (x <= downTop) )
      {
         return true;
      }
   }
   
   return false;
}

/*********************************************************************
 *                         isInnerPenaltyArea                        *
 *********************************************************************/
bool Field::isInnerPenaltyArea(Ogre::Real x, Ogre::Real z,
                               bool testUp, bool testDown)
{
   Ogre::Real upBottom = penaltyAreaDelta[0];
   Ogre::Real upTop = halfSize[0]-sideDelta[0];
   
   Ogre::Real left = -penaltyAreaDelta[1];
   Ogre::Real right = penaltyAreaDelta[1];
   
   Ogre::Real downBottom = -halfSize[0]+sideDelta[0];
   Ogre::Real downTop = -penaltyAreaDelta[0];
   
   if( (z >= left) && (z <= right) )
   {
      if( (testUp) && (x >= upBottom) && (x <= upTop) )
      {
         return true;
      }
      
      if( (testDown) && (x >= downBottom) && (x <= downTop) )
      {
         return true;
      }
   }

   return false;
}

/*********************************************************************
 *                     getNearetPointInLittleArea                    *
 *********************************************************************/
void Field::getNearestPointInLittleArea(Ogre::Real& x, Ogre::Real& z)
{
   Ogre::Real upBottom = halfSize[0]-littleAreaDelta[0];
   Ogre::Real upTop = halfSize[0]-sideDelta[0];
   
   Ogre::Real left = -littleAreaDelta[1];
   Ogre::Real right = littleAreaDelta[1];
   
   Ogre::Real downBottom = -halfSize[0]+sideDelta[0];
   Ogre::Real downTop = -halfSize[0]+littleAreaDelta[0];
   
   Ogre::Real top;
   Ogre::Real bottom;
   
   if(!isInnerLittleArea(x, z))
   {
      /* Check z limits */
      if(z < left)
      {
         z  = left;
      }
      else if(z > right)
      {
         z = right;
      }
      
      /* Determine which goal X to check  */
      if(x > 0)
      {
         /* Up goal check */
         top = upTop;
         bottom = upBottom;
      }
      else
      {
         /* Down goal check */
         top = downTop;
         bottom = downBottom;
      }
      
      /* Check x limits */
      if(x < bottom)
      {
         x = bottom;
      }
      else if(x > top)
      {
         x = top;
      }
      
   }
}

/*********************************************************************
 *                    getNearestPointInPlayableArea                  *
 *********************************************************************/
bool Field::getNearestPointInPlayableArea(Ogre::Real&x, Ogre::Real& z)
{
   bool changed = false;
   
   if(x < -halfSize[0] + sideDelta[0])
   {
      x = sideDelta[0] - halfSize[0];
      changed = true;
   }
   else if(x > halfSize[0] - sideDelta[0])
   {
      x = halfSize[0] - sideDelta[0];
      changed = true;
   }
   
   if(z < -halfSize[1] + sideDelta[1])
   {
      z = sideDelta[1] - halfSize[1];
      changed = true;
   }
   else if(z > halfSize[1] - sideDelta[1])
   {
      z = halfSize[1] - sideDelta[1];
      changed = true;
   }
   
   return changed;
}

#if BTSOCCER_RENDER_DEBUG
/*********************************************************************
 *                              debugDraw                            *
 *********************************************************************/
void Field::debugDraw()
{
   if(mLines != NULL)
   {
      mLines->beginUpdate(0);

      float height = 0.05f;
      
      /* Sides */
      mLines->colour(0.8f, 0.1f, 0.1f);
      mLines->position(-halfSize[0]+sideDelta[0], height,
            -halfSize[1]+sideDelta[1]);
      mLines->position(halfSize[0]-sideDelta[0], height,
                       -halfSize[1]+sideDelta[1]);
      
      mLines->position(-halfSize[0]+sideDelta[0], height,
                       halfSize[1]-sideDelta[1]);
      mLines->position(halfSize[0]-sideDelta[0], height,
                       halfSize[1]-sideDelta[1]);
      
      mLines->position(-halfSize[0]+sideDelta[0], height,
                       -halfSize[1]+sideDelta[1]);
      mLines->position(-halfSize[0]+sideDelta[0], height,
                       halfSize[1]-sideDelta[1]);
      
      mLines->position(halfSize[0]-sideDelta[0], height,
                       -halfSize[1]+sideDelta[1]);
      mLines->position(halfSize[0]-sideDelta[0], height,
                       halfSize[1]-sideDelta[1]);
      
      /* Upper Little area */
      mLines->position(halfSize[0]-sideDelta[0], height, -littleAreaDelta[1]);
      mLines->position(halfSize[0]-littleAreaDelta[0], height,
            -littleAreaDelta[1]);
      
      mLines->position(halfSize[0]-sideDelta[0], height, littleAreaDelta[1]);
      mLines->position(halfSize[0]-littleAreaDelta[0], height, 
            littleAreaDelta[1]);
      
      mLines->position(halfSize[0]-littleAreaDelta[0], height,
            -littleAreaDelta[1]);
      mLines->position(halfSize[0]-littleAreaDelta[0], height,
            littleAreaDelta[1]);

      
      /* Bottom Little area */
      mLines->position(-halfSize[0]+sideDelta[0], height, -littleAreaDelta[1]);
      mLines->position(-halfSize[0]+littleAreaDelta[0], height,
            -littleAreaDelta[1]);
      
      mLines->position(-halfSize[0]+sideDelta[0], height, littleAreaDelta[1]);
      mLines->position(-halfSize[0]+littleAreaDelta[0], height,
            littleAreaDelta[1]);
      
      mLines->position(-halfSize[0]+littleAreaDelta[0], height, 
            -littleAreaDelta[1]);
      mLines->position(-halfSize[0]+littleAreaDelta[0], height,
            littleAreaDelta[1]);
      
      /* Upper penalty Area */
      mLines->position(halfSize[0]-sideDelta[0], height, -penaltyAreaDelta[1]);
      mLines->position(penaltyAreaDelta[0], height, -penaltyAreaDelta[1]);
      
      mLines->position(halfSize[0]-sideDelta[0], height, penaltyAreaDelta[1]);
      mLines->position(penaltyAreaDelta[0], height, penaltyAreaDelta[1]);
      
      mLines->position(penaltyAreaDelta[0], height, -penaltyAreaDelta[1]);
      mLines->position(penaltyAreaDelta[0], height, penaltyAreaDelta[1]);
      
      /* Bottom penaltyArea */
      mLines->position(-halfSize[0]+sideDelta[0], height, -penaltyAreaDelta[1]);
      mLines->position(-penaltyAreaDelta[0], height, -penaltyAreaDelta[1]);
      
      mLines->position(-halfSize[0]+sideDelta[0], height, penaltyAreaDelta[1]);
      mLines->position(-penaltyAreaDelta[0], height, penaltyAreaDelta[1]);
      
      mLines->position(-penaltyAreaDelta[0], height, -penaltyAreaDelta[1]);
      mLines->position(-penaltyAreaDelta[0], height, penaltyAreaDelta[1]);
      
      /* Upper Penalty Mark */
      mLines->position(halfSize[0] - penaltyMark-0.1f, height, 0.0f);
      mLines->position(halfSize[0] - penaltyMark+0.1f, height, 0.0f);
      mLines->position(halfSize[0] - penaltyMark, height, -0.1f);
      mLines->position(halfSize[0] - penaltyMark, height, 0.1f);
      
      /* Bottom Penalty Mark */
      mLines->position(-halfSize[0] + penaltyMark-0.1f, height, 0.0f);
      mLines->position(-halfSize[0] + penaltyMark+0.1f, height, 0.0f);
      mLines->position(-halfSize[0] + penaltyMark, height, -0.1f);
      mLines->position(-halfSize[0] + penaltyMark, height, 0.1f);
      
      /* Upper Goal */
      mLines->position(goalPosition-0.1f, height, 0.0f);
      mLines->position(goalPosition+0.1f, height, 0.0f);
      mLines->position(goalPosition, height, -0.1f);
      mLines->position(goalPosition, height, 0.1f);
      
      /* Bottom Goal */
      mLines->position(-goalPosition-0.1f, height, 0.0f);
      mLines->position(-goalPosition+0.1f, height, 0.0f);
      mLines->position(-goalPosition, height, -0.1f);
      mLines->position(-goalPosition, height, 0.1f);

      
      mLines->end();
   }
}
#endif


