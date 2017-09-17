
#include "bulletdebugdraw.h"
#include "../btsoccer.h"
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTimer.h>
#include <OGRE/OgreLogManager.h>

using namespace Ogre;

BulletDebugDraw::BulletDebugDraw( SceneManager *scm )
{
   mContactPoints = &mContactPoints1;
   mLines = new ManualObject("physics lines");
   mTriangles = new ManualObject("physics triangles");
   mLines->setDynamic(true);
   mTriangles->setDynamic(true);
   //mLines->estimateVertexCount( 100000 );
   //mLines->estimateIndexCount( 0 );

   scm->getRootSceneNode()->attachObject( mLines );
   scm->getRootSceneNode()->attachObject( mTriangles );

   /*static const char * matName = "OgreBulletCollisionsDebugDefault";
   MaterialPtr mtl = MaterialManager::getSingleton().getDefaultSettings()->clone(matName);
   mtl->setReceiveShadows(false);
   mtl->setSceneBlending( SBT_TRANSPARENT_ALPHA );
   mtl->setDepthBias( 0.1, 0 );
   TextureUnitState * tu = mtl->getTechnique(0)->getPass(0)->createTextureUnitState();
   tu->setColourOperationEx( LBX_SOURCE1, LBS_DIFFUSE );
   mtl->getTechnique(0)->setLightingEnabled(false);
   //mtl->getTechnique(0)->setSelfIllumination( ColourValue::White ); */

   String matName = "field_debug";

   mLines->begin( matName, RenderOperation::OT_LINE_LIST );
   mLines->position( Vector3::ZERO );
   mLines->colour( ColourValue::Blue );
   mLines->position( Vector3::ZERO );
   mLines->colour( ColourValue::Blue );
   mLines->end();

   mTriangles->begin( matName, RenderOperation::OT_TRIANGLE_LIST );
   mTriangles->position( Vector3::ZERO );
   mTriangles->colour( ColourValue::Blue );
   mTriangles->position( Vector3::ZERO );
   mTriangles->colour( ColourValue::Blue );
   mTriangles->position( Vector3::ZERO );
   mTriangles->colour( ColourValue::Blue );
   mTriangles->end();

   mDebugModes = (DebugDrawModes) DBG_DrawWireframe;
  
   /*linesNode = scm->getRootSceneNode()->createChildSceneNode();
   linesNode->attachObject(mLines);
   trisNode = scm->getRootSceneNode()->createChildSceneNode();
   trisNode->attachObject(mTriangles);*/
   //Root::getSingleton().addFrameListener(this);
}

BulletDebugDraw::~BulletDebugDraw()
{
   //Root::getSingleton().removeFrameListener(this);
   delete mLines;
   delete mTriangles;
}

void BulletDebugDraw::drawRay(const Ogre::Vector3& origin, 
      const Ogre::Vector3& dir, const float size, const Ogre::Vector3& color)
{
   Ogre::Vector3 fin = origin + dir * size;
   drawLine(btVector3(origin.x, origin.y, origin.z) * OGRE_TO_BULLET_FACTOR,
         btVector3(fin.x, fin.y, fin.z) * OGRE_TO_BULLET_FACTOR,
         btVector3(color.x, color.y, color.z));
}


void BulletDebugDraw::drawLine( const Ogre::Vector3 &from, 
      const Ogre::Vector3 &to, const Ogre::Vector3 &color)
{
   drawLine(btVector3(from.x, from.y, from.z) * OGRE_TO_BULLET_FACTOR, 
            btVector3(to.x, to.y, to.z) * OGRE_TO_BULLET_FACTOR,
            btVector3(color.x, color.y, color.z));
}

void BulletDebugDraw::drawLine( const btVector3 &from, const btVector3 &to, const btVector3 &color )
{
   int i = lines.size();
   lines.resize( lines.size() + 1 );
   lines[i].from = cvt( from );
   lines[i].to = cvt( to );
   lines[i].color.r = color.x();
   lines[i].color.g = color.y();
   lines[i].color.b = color.z();
}

void BulletDebugDraw::drawTriangle( const btVector3 &v0, const btVector3 &v1, const btVector3 &v2, const btVector3 &color, btScalar alpha )
{
   int i = tris.size();
   tris.resize( tris.size() + 1 );
   ColourValue c( color.getX(), color.getY(), color.getZ(), alpha );  
   c.saturate();
   tris[i].color = c;
   tris[i].v0 = cvt(v0);
   tris[i].v1 = cvt(v1);
   tris[i].v2 = cvt(v2);
}

void BulletDebugDraw::drawContactPoint( const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color )
{
   mContactPoints->resize( mContactPoints->size() + 1 );
   ContactPoint p = mContactPoints->back();
   p.from = cvt( PointOnB );
   p.to = p.from + cvt( normalOnB ) * distance;
   p.dieTime = Root::getSingleton().getTimer()->getMilliseconds() + lifeTime;
   p.color.r = color.x();
   p.color.g = color.y();
   p.color.b = color.z();
}

void BulletDebugDraw::update()
{

#if 0
  std::vector<ContactPoint>* newCP = 
      mContactPoints == &mContactPoints1 ? &mContactPoints2 : &mContactPoints1;
   
   for(std::vector<ContactPoint>::iterator i = mContactPoints->begin(); 
       i < mContactPoints->end(); i++ )
   {
      ContactPoint &cp = *i;
      mLines->position( cp.from );
      mLines->colour( cp.color );
      mLines->position( cp.to );
      /*if ( now <= cp.dieTime  )
         newCP->push_back( cp ); */
   }
   mContactPoints->clear();
   mContactPoints = newCP;
#endif

   /*mLines->position(Vector3::ZERO);
   mLines->position(Vector3(40.0f, 40.0f, 40.0f));*/

   mLines->beginUpdate(0);
   for(unsigned int i = 0; 
       i < lines.size(); i++ )
   {
      mLines->position( lines[i].from * BULLET_TO_OGRE_FACTOR );
      mLines->colour( lines[i].color );
      mLines->position( lines[i].to * BULLET_TO_OGRE_FACTOR );
      mLines->colour( lines[i].color );
      /*if ( now <= cp.dieTime  )
         newCP->push_back( cp ); */
   }
   mLines->end();
   lines.clear();
   
   mTriangles->beginUpdate(0);
   for(std::vector<Triangle>::iterator i = tris.begin(); 
       i < tris.end(); i++ )
   {
      Triangle &t = *i;
      mTriangles->position( t.v0 * BULLET_TO_OGRE_FACTOR );
      mTriangles->colour( t.color );
      mTriangles->position( t.v1 * BULLET_TO_OGRE_FACTOR );
      mTriangles->position( t.v2 * BULLET_TO_OGRE_FACTOR );
      /*if ( now <= cp.dieTime  )
         newCP->push_back( cp ); */
   } 
   mTriangles->end();
   tris.clear();
}

void BulletDebugDraw::reportErrorWarning( const char *warningString )
{
   LogManager::getSingleton().getDefaultLog()->logMessage( warningString );
}

void BulletDebugDraw::draw3dText( const btVector3 &location, const char *textString )
{

}

void BulletDebugDraw::setDebugMode( int debugMode )
{
   mDebugModes = (DebugDrawModes) debugMode;
}

int BulletDebugDraw::getDebugMode() const
{
   return mDebugModes;
}

