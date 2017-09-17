# This is the CMake definition file for BtSoccer usage.
#
# Usually you should just call:
#
#    cmake .
#
# To create the needed makefiles for BtSoccer compilation.
# 
# You could also set some debug options with:
#

cmake_minimum_required(VERSION 2.8)

set(PACKAGE btsoccer)

# Set Version number
set(VERSION_MAJOR 0)
set(VERSION_MINOR 4)
set(VERSION_PATCH 0)
if(${VERSION_PATCH})
   set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})
else(${VERSION_PATCH})
   set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR})
endif(${VERSION_PATCH})

# Set directories
set(DATADIR ${CMAKE_INSTALL_PREFIX}/share)
set(LOCALEDIR ${CMAKE_INSTALL_PREFIX}/share/locale)
set(LOCALE_DIR share/locale)
set(INSTALL_BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin)

# Some options
option(DEBUG_AI "Show AI debug messages" OFF)
option(RENDER_DEBUG "Render debug primitives" OFF)

set(BIN_DIR "${CMAKE_SOURCE_DIR}/bin")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ./CMakeFiles)

configure_file("src/btsoccerconfig.h.in" "btsoccerconfig.h")

project(btsoccer)

# Some compiler options
if(UNIX)
   add_definitions(-Wall -g)
endif(UNIX)


# First, try to find required libraries

FIND_PACKAGE(OGRE REQUIRED)
include_directories(${OGRE_INCLUDE_DIR})

FIND_PACKAGE(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

FIND_PACKAGE(Bullet REQUIRED)
include_directories(${BULLET_INCLUDE_DIRS})

FIND_PACKAGE(OpenAL REQUIRED)
include_directories(${OPENAL_INCLUDE_DIR})

FIND_PACKAGE(Ogg REQUIRED)
include_directories(${OGG_INCLUDE_DIR})

FIND_PACKAGE(Vorbis REQUIRED)
include_directories(${VORBIS_INCLUDE_DIR})

FIND_PACKAGE(Vorbisfile REQUIRED)
include_directories(${VORBISFILE_INCLUDE_DIR})

FIND_PACKAGE(Kobold REQUIRED)
include_directories(${KOBOLD_INCLUDE_DIR})

FIND_PACKAGE(Kosound REQUIRED)
include_directories(${KOSOUND_INCLUDE_DIR})

FIND_PACKAGE(Goblin REQUIRED)
include_directories(${GOBLIN_INCLUDE_DIR})

# include the ./src as default too
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${CMAKE_BINARY_DIR}/)

# Include the sources
include(sources.cmake)

# Make Libraries
add_library(btsoccerlib ${BTSOCCER_LIB_SOURCES} ${BTSOCCER_LIB_HEADERS} )

# Make Binaries
add_executable(btsoccer WIN32 ${BTSOCCER_SOURCES} ${BTSOCCER_HEADERS} )
target_link_libraries(btsoccer btsoccerlib 
   ${GOBLIN_LIBRARY}
   ${KOSOUND_LIBRARY}
   ${KOBOLD_LIBRARY}
   ${OGRE_LIBRARIES} 
   ${OGRE_Overlay_LIBRARIES} 
   ${OGRE_RTShaderSystem_LIBRARIES}
   ${SDL2_LIBRARY}
   ${OPENAL_LIBRARY} 
   ${BULLET_LIBRARIES} 
   ${VORBISFILE_LIBRARY} ${VORBIS_LIBRARY}
   ${OGG_LIBRARY} m
   ${LIBINTL_LIBRARIES} pthread)

# Make Test Binaries
add_executable(run_btsoccer_tests WIN32 ${BTSOCCER_TEST} )
target_link_libraries(run_btsoccer_tests btsoccerlib
   ${GOBLIN_LIBRARY}
   ${KOSOUND_LIBRARY}
   ${KOBOLD_LIBRARY}
   ${OGRE_LIBRARIES} 
   ${OGRE_Overlay_LIBRARIES} 
   ${OGRE_RTShaderSystem_LIBRARIES}
   ${SDL2_LIBRARY} 
   ${OPENAL_LIBRARY} 
   ${BULLET_LIBRARIES} 
   ${VORBISFILE_LIBRARY} ${VORBIS_LIBRARY}
   ${OGG_LIBRARY} m
   ${LIBINTL_LIBRARIES} pthread)

