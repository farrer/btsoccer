LOCAL_PATH := $(call my-dir)

NDK_DIR := /home/farrer/Android/Ndk
NDK_USER_DIR := $(NDK_DIR)/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/user
NDK_LIB_DIR := $(NDK_USER_DIR)/lib
NDK_INCLUDE_DIR := $(NDK_USER_DIR)/include
OGRE_PLUGIN_DIR := $(NDK_LIB_DIR)/OGRE

OGREDEPS_BUILD_DIR := /projetos/ogredeps/build

DNTEAM_DIR := /projetos/dnteam

# Specify prebuilt static libraries
# Plugin_OctreeSceneManagerStatic
	include $(CLEAR_VARS)
	LOCAL_MODULE    := Plugin_OctreeSceneManagerStatic
	LOCAL_SRC_FILES := $(OGRE_PLUGIN_DIR)/libPlugin_OctreeSceneManagerStatic.a
	include $(PREBUILT_STATIC_LIBRARY)

# Plugin_ParticleFXStatic
	include $(CLEAR_VARS)
	LOCAL_MODULE    := Plugin_ParticleFXStatic
	LOCAL_SRC_FILES := $(OGRE_PLUGIN_DIR)/libPlugin_ParticleFXStatic.a
	include $(PREBUILT_STATIC_LIBRARY)

# OgreVolumeStatic
	include $(CLEAR_VARS)
	LOCAL_MODULE    := OgreVolumeStatic
	LOCAL_SRC_FILES := $(NDK_LIB_DIR)/libOgreVolumeStatic.a
	include $(PREBUILT_STATIC_LIBRARY)

# OgrePagingStatic
	include $(CLEAR_VARS)
	LOCAL_MODULE    := OgrePagingStatic
	LOCAL_SRC_FILES := $(NDK_LIB_DIR)/libOgrePagingStatic.a
	include $(PREBUILT_STATIC_LIBRARY)

# OgreOverlayStatic
	include $(CLEAR_VARS)
	LOCAL_MODULE    := OgreOverlayStatic
	LOCAL_SRC_FILES := $(NDK_LIB_DIR)/libOgreOverlayStatic.a
	include $(PREBUILT_STATIC_LIBRARY)

# OgreMeshLodGeneratorStatic
	include $(CLEAR_VARS)
	LOCAL_MODULE    := OgreMeshLodGeneratorStatic
	LOCAL_SRC_FILES := $(NDK_LIB_DIR)/libOgreMeshLodGeneratorStatic.a
	include $(PREBUILT_STATIC_LIBRARY)

# OgreRTShaderSystemStatic
	include $(CLEAR_VARS)
	LOCAL_MODULE    := OgreRTShaderSystemStatic
	LOCAL_SRC_FILES := $(NDK_LIB_DIR)/libOgreRTShaderSystemStatic.a
	include $(PREBUILT_STATIC_LIBRARY)

# OgreTerrainStatic
	include $(CLEAR_VARS)
	LOCAL_MODULE    := OgreTerrainStatic
	LOCAL_SRC_FILES := $(NDK_LIB_DIR)/libOgreTerrainStatic.a
	include $(PREBUILT_STATIC_LIBRARY)

# OgreGLSupportStatic 
	include $(CLEAR_VARS)
	LOCAL_MODULE    := OgreGLSupportStatic
	LOCAL_SRC_FILES := $(NDK_LIB_DIR)/libOgreGLSupportStatic.a
	include $(PREBUILT_STATIC_LIBRARY)

# RenderSystem_GLES2Static
	include $(CLEAR_VARS)
	LOCAL_MODULE    := RenderSystem_GLES2Static
	LOCAL_SRC_FILES := $(OGRE_PLUGIN_DIR)/libRenderSystem_GLES2Static.a
	include $(PREBUILT_STATIC_LIBRARY)

# OgreMainStatic
	include $(CLEAR_VARS)
	LOCAL_MODULE    := OgreMainStatic
	LOCAL_SRC_FILES := $(NDK_LIB_DIR)/libOgreMainStatic.a
	include $(PREBUILT_STATIC_LIBRARY)

# FreeImage
	include $(CLEAR_VARS)
	LOCAL_MODULE    := FreeImage
	LOCAL_SRC_FILES := $(NDK_LIB_DIR)/libFreeImage.a
	include $(PREBUILT_STATIC_LIBRARY)

# freetype
	include $(CLEAR_VARS)
	LOCAL_MODULE    := freetype
	LOCAL_SRC_FILES := $(NDK_LIB_DIR)/libfreetype.a
	include $(PREBUILT_STATIC_LIBRARY)

# zziplib
	include $(CLEAR_VARS)
	LOCAL_MODULE    := zziplib
	LOCAL_SRC_FILES := $(NDK_LIB_DIR)/libzziplib.a
	include $(PREBUILT_STATIC_LIBRARY)

# openal-soft
	include $(CLEAR_VARS)
	LOCAL_MODULE    := openal
	LOCAL_SRC_FILES := $(NDK_LIB_DIR)/libopenal.so
	include $(PREBUILT_SHARED_LIBRARY)

# ogg
    include $(CLEAR_VARS)
    LOCAL_MODULE := ogg
    LOCAL_SRC_FILES := $(NDK_LIB_DIR)/libogg.a
    include $(PREBUILT_STATIC_LIBRARY)

# tremor
    include $(CLEAR_VARS)
    LOCAL_MODULE := tremor
    LOCAL_SRC_FILES := $(NDK_LIB_DIR)/libtremor.a
    include $(PREBUILT_STATIC_LIBRARY)

# KOBOLD
    include $(CLEAR_VARS)
    LOCAL_MODULE := kobold
    LOCAL_SRC_FILES := $(NDK_LIB_DIR)/libkobold.a
    include $(PREBUILT_STATIC_LIBRARY)

# KOSOUND
    include $(CLEAR_VARS)
    LOCAL_MODULE := kosound
    LOCAL_SRC_FILES := $(NDK_LIB_DIR)/libkosound.a
    include $(PREBUILT_STATIC_LIBRARY)

# GOBLIN
    include $(CLEAR_VARS)
    LOCAL_MODULE := goblin
    LOCAL_SRC_FILES := $(NDK_LIB_DIR)/libgoblin.a
    include $(PREBUILT_STATIC_LIBRARY)

# Bullet
    include $(CLEAR_VARS)
    LOCAL_MODULE := bullet
    LOCAL_SRC_FILES := $(NDK_LIB_DIR)/libbullet.a
    include $(PREBUILT_STATIC_LIBRARY)

    ALL_C_INCLUDES := $(NDK_INCLUDE_DIR)
    ALL_C_INCLUDES += $(NDK_INCLUDE_DIR)/bullet
    ALL_C_INCLUDES += $(NDK_INCLUDE_DIR)/tremor
    ALL_C_INCLUDES += $(NDK_INCLUDE_DIR)/ogg
    ALL_C_INCLUDES += $(NDK_INCLUDE_DIR)/kobold
    ALL_C_INCLUDES += $(NDK_INCLUDE_DIR)/kosound
    ALL_C_INCLUDES += $(NDK_INCLUDE_DIR)/goblin
    ALL_C_INCLUDES += $(NDK_INCLUDE_DIR)/OGRE
    ALL_C_INCLUDES += $(NDK_INCLUDE_DIR)/OGRE/RenderSystems/GLES2/
    ALL_C_INCLUDES += $(NDK_INCLUDE_DIR)/OGRE/RenderSystems/GLES2/Android/
    ALL_C_INCLUDES += $(NDK_INCLUDE_DIR)/OGRE/Overlay
    ALL_C_INCLUDES += $(NDK_DIR)/sources/cpufeatures 
    ALL_C_INCLUDES += $(OGREDEPS_BUILD_DIR)/include 
    ALL_C_INCLUDES += $(OGREDEPS_BUILD_DIR)/include/OIS 
    ALL_C_INCLUDES += $(DNTEAM_DIR)/btsoccer/
    ALL_C_INCLUDES += $(DNTEAM_DIR)/btsoccer/src/
    ALL_C_INCLUDES += $(DNTEAM_DIR)/bullet/src/

# BtSoccer CORE
    include $(CLEAR_VARS)
    LOCAL_MODULE := core
    LOCAL_CFLAGS := -DGL_GLEXT_PROTOTYPES=1 
    LOCAL_CFLAGS += -fexceptions -frtti -x c++ -D___ANDROID___ -DANDROID -DZZIP_OMIT_CONFIG_H -DINCLUDE_RTSHADER_SYSTEM
    LOCAL_C_INCLUDES := $(ALL_C_INCLUDES)
    LOCAL_PATH := $(DNTEAM_DIR)/btsoccer/src
    LOCAL_SRC_FILES := ai/baseai.cpp ai/decourtai.cpp ai/dummyai.cpp 
    LOCAL_SRC_FILES += ai/fuzzyai.cpp debug/bulletdebugdraw.cpp
    LOCAL_SRC_FILES += engine/ball.cpp engine/core.cpp engine/cup.cpp
    LOCAL_SRC_FILES += engine/field.cpp engine/fobject.cpp engine/goalkeeper.cpp
    LOCAL_SRC_FILES += engine/goals.cpp engine/options.cpp engine/replay.cpp
    LOCAL_SRC_FILES += engine/rules.cpp engine/savefile.cpp engine/stats.cpp
    LOCAL_SRC_FILES += engine/team.cpp engine/teamplayer.cpp engine/teams.cpp
    LOCAL_SRC_FILES += engine/tutorial.cpp
    LOCAL_SRC_FILES += gui/guiinitial.cpp gui/guimain.cpp gui/guimessage.cpp
    LOCAL_SRC_FILES += gui/guioptions.cpp gui/guipause.cpp gui/guireplay.cpp 
    LOCAL_SRC_FILES += gui/guisaves.cpp gui/guiscore.cpp gui/guisocket.cpp 
    LOCAL_SRC_FILES += net/protocol.cpp net/tcpnetwork.cpp
    LOCAL_SRC_FILES += physics/bulletlink.cpp physics/collision.cpp
    LOCAL_SRC_FILES += physics/disttable.cpp physics/forceio.cpp 
    LOCAL_SRC_FILES += physics/ogremotionstate.cpp 
    include $(BUILD_STATIC_LIBRARY) 

# The shared library JNI ITSELF
    include $(CLEAR_VARS)
    LOCAL_MODULE    := GoblinJNI
    LOCAL_LDLIBS	:= -landroid -lc -lm -ldl -lz -llog -lEGL -lGLESv2 -Wl,-s
    LOCAL_LDLIBS	+= -L$(NDK_LIB_DIR)

    LOCAL_WHOLE_STATIC_LIBRARIES := core goblin kosound kobold tremor ogg bullet
    LOCAL_WHOLE_STATIC_LIBRARIES +=  Plugin_OctreeSceneManagerStatic OgreOverlayStatic OgreMeshLodGeneratorStatic OgreRTShaderSystemStatic RenderSystem_GLES2Static OgreGLSupportStatic OgreMainStatic 
    LOCAL_STATIC_LIBRARIES	:=  FreeImage
    LOCAL_WHOLE_STATIC_LIBRARIES	+=  freetype 
    LOCAL_WHOLE_STATIC_LIBRARIES += boostsystem boostchrono boostdatetime
    LOCAL_WHOLE_STATIC_LIBRARIES += boostthread boost atomic
    LOCAL_STATIC_LIBRARIES += zziplib
    LOCAL_STATIC_LIBRARIES	+= android_native_app_glue cpufeatures
    LOCAL_SHARED_LIBRARIES := openal
    LOCAL_CFLAGS := -DGL_GLEXT_PROTOTYPES=1 
    LOCAL_CFLAGS += -fexceptions -frtti -x c++ -D___ANDROID___ -DANDROID -DZZIP_OMIT_CONFIG_H -DINCLUDE_RTSHADER_SYSTEM
    
    LOCAL_C_INCLUDES := $(ALL_C_INCLUDES)
    LOCAL_PATH := $(DNTEAM_DIR)/btsoccer/android/src
    LOCAL_SRC_FILES := glue.cpp 
    include $(BUILD_SHARED_LIBRARY) 
         
    $(call import-module,android/cpufeatures) 
    $(call import-module,android/native_app_glue) 
