
#ifndef _dnteam_btsoccer_debug_draw_h
#define _dnteam_btsoccer_debug_draw_h

#include <btBulletDynamicsCommon.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreRoot.h>

inline btVector3 cvt(const Ogre::Vector3 &V){
   return btVector3(V.x, V.y, V.z);
}

inline Ogre::Vector3 cvt(const btVector3&V){
   return Ogre::Vector3(V.x(), V.y(), V.z());
}

inline btQuaternion cvt(const Ogre::Quaternion &Q)
{
   return btQuaternion(Q.x, Q.y, Q.z, Q.w);
};

inline Ogre::Quaternion cvt(const btQuaternion &Q)
{
   return Ogre::Quaternion(Q.w(), Q.x(), Q.y(), Q.z());
};

class BulletDebugDraw: public btIDebugDraw{
   public:
      BulletDebugDraw( Ogre::SceneManager *scm );
      ~BulletDebugDraw ();
      virtual void     drawLine (const btVector3 &from, const btVector3 &to, const btVector3 &color);
      virtual void     drawTriangle (const btVector3 &v0, const btVector3 &v1, const btVector3 &v2, const btVector3 &color, btScalar);
      virtual void     drawContactPoint (const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color);
      virtual void     reportErrorWarning (const char *warningString);
      virtual void     draw3dText (const btVector3 &location, const char *textString);
      virtual void     setDebugMode (int debugMode);
      virtual int     getDebugMode () const;

      void drawRay(const Ogre::Vector3& origin, const Ogre::Vector3& dir,
                   const float size, const Ogre::Vector3& color);
      void drawLine( const Ogre::Vector3 &from, const Ogre::Vector3 &to, 
            const Ogre::Vector3 &color);

      void update();
   private:
      struct ContactPoint{
        Ogre::Vector3 from;
        Ogre::Vector3 to;
        Ogre::ColourValue   color;
        size_t        dieTime;
      };

      struct Line
      {
        Ogre::Vector3 from;
        Ogre::Vector3 to;
        Ogre::ColourValue   color;
      };

      struct Triangle
      {
        Ogre::Vector3 v0;
        Ogre::Vector3 v1;
        Ogre::Vector3 v2;
        Ogre::ColourValue   color;
      };

      DebugDrawModes               mDebugModes;
      Ogre::ManualObject          *mLines;
      Ogre::ManualObject          *mTriangles;
      Ogre::SceneNode* linesNode;
      Ogre::SceneNode* trisNode;
      std::vector< ContactPoint > *mContactPoints;
      std::vector< ContactPoint >  mContactPoints1;
      std::vector< ContactPoint >  mContactPoints2;

      std::vector<Line> lines;
      std::vector<Triangle> tris;
};


#endif


