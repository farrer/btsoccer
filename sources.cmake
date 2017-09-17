# This file is the list of sources needed for dnt, dntMapEditor and 
# dntPartEditor executables

########################################################################
# Files related to the kobold library
########################################################################
set(KOBOLD_SOURCES
kobold/cafstream.cpp
kobold/defparser.cpp
kobold/i18n.cpp
kobold/list.cpp
kobold/network.cpp
kobold/oggstream.cpp
kobold/parallelprocess.cpp
kobold/sndfx.cpp
kobold/sound.cpp
kobold/soundstream.cpp
kobold/timer.cpp
kobold/userinfo.cpp
)
set(KOBOLD_HEADERS
kobold/cafstream.h
kobold/config.h
kobold/defparser.h
kobold/i18n.h
kobold/list.h
kobold/network.h
kobold/oggstream.h
kobold/parallelprocess.h
kobold/sndfx.h
kobold/sound.h
kobold/soundstream.h
kobold/timer.h
kobold/userinfo.h
)

########################################################################
# Files related to the kobold library
########################################################################
set(GOBLIN_SOURCES
goblin/baseapp.cpp
goblin/camera.cpp
goblin/cursor.cpp
goblin/fpsdisplay.cpp
goblin/guiobject.cpp
goblin/ibar.cpp
goblin/ibutton.cpp
goblin/image.cpp
goblin/keyboard.cpp
goblin/materiallistener.cpp
goblin/model3d.cpp
goblin/multitouchcontroller.cpp
goblin/screeninfo.cpp
goblin/target.cpp
goblin/textbox.cpp
goblin/texttitle.cpp
)

set(GOBLIN_HEADERS
goblin/baseapp.h
goblin/camera.h
goblin/config.h
goblin/cursor.h
goblin/fpsdisplay.h
goblin/guiobject.h
goblin/ibar.h
goblin/ibutton.h
goblin/image.h
goblin/keyboard.h
goblin/keycodes.h
goblin/materiallistener.h
goblin/model3d.h
goblin/multitouchcontroller.h
goblin/screeninfo.h
goblin/target.h
goblin/textbox.h
goblin/texttitle.h
goblin/touchview.h
)

########################################################################
# Files related to the core engine
########################################################################
set(CORE_SOURCES
src/engine/ball.cpp
src/engine/cup.cpp
src/engine/goals.cpp
src/engine/field.cpp
src/engine/fobject.cpp
src/engine/goalkeeper.cpp
src/engine/options.cpp
src/engine/core.cpp
src/engine/replay.cpp
src/engine/rules.cpp
src/engine/savefile.cpp
src/engine/team.cpp
src/engine/teams.cpp
src/engine/teamplayer.cpp
src/engine/tutorial.cpp
src/engine/stats.cpp
)
set(CORE_HEADERS
src/engine/ball.h
src/engine/cup.h
src/engine/goals.h
src/engine/field.h
src/engine/fobject.h
src/engine/goalkeeper.h
src/engine/options.h
src/engine/core.h
src/engine/replay.h
src/engine/rules.h
src/engine/savefile.h
src/engine/team.h
src/engine/teams.h
src/engine/teamplayer.h
src/engine/tutorial.h
src/engine/stats.h
)

set(GUI_SOURCES
src/gui/guiinitial.cpp
src/gui/guimain.cpp
src/gui/guimessage.cpp
src/gui/guioptions.cpp
src/gui/guipause.cpp
src/gui/guireplay.cpp
src/gui/guisaves.cpp
src/gui/guiscore.cpp
src/gui/guisocket.cpp
)
set(GUI_HEADERS
src/gui/guiinitial.h
src/gui/guimain.h
src/gui/guimessage.h
src/gui/guioptions.h
src/gui/guipause.h
src/gui/guireplay.h
src/gui/guiscore.h
src/gui/guisaves.h
src/gui/guisocket.h
)

set(PHYSICS_SOURCES
src/physics/bulletlink.cpp
src/physics/collision.cpp
src/physics/disttable.cpp
src/physics/forceio.cpp
src/physics/ogremotionstate.cpp
)
set(PHYSICS_HEADERS
src/physics/bulletlink.h
src/physics/collision.h
src/physics/disttable.h
src/physics/forceio.h
src/physics/ogremotionstate.h
)
set(NET_SOURCES
src/net/protocol.cpp
src/net/tcpnetwork.cpp
)
set(NET_HEADERS
src/net/protocol.h
src/net/tcpnetwork.h
)
set(AI_SOURCES
src/ai/baseai.cpp
src/ai/decourtai.cpp
src/ai/dummyai.cpp
src/ai/fuzzyai.cpp
)
set(AI_HEADERS
src/ai/baseai.h
src/ai/decourtai.h
src/ai/dummyai.h
src/ai/fuzzyai.h
)

set(DEBUG_HEADERS
src/debug/bulletdebugdraw.h
)

set(DEBUG_SOURCES
src/debug/bulletdebugdraw.cpp
)

IF(${APPLE})
   #set(APPLE_CONFIG_SOURCES
   #    src/config_mac.cpp
   #    macosx/SDLMain.m)
   #set(APPLE_CONFIG_HEADERS
   #    macosx/SDLMain.h)
ENDIF(${APPLE})

IF(${WIN32} OR ${MINGW})
   set(WIN_SOURCES
       windows/resource.rc)
ENDIF(${WIN32} OR ${MINGW})

set(BTSOCCER_LIB_HEADERS
${CORE_HEADERS}
${GUI_HEADERS}
${PHYSICS_HEADERS}
${NET_HEADERS}
${AI_HEADERS}
${DEBUG_HEADERS}
${APPLE_CONFIG_HEADERS}
)

set(BTSOCCER_LIB_SOURCES
${CORE_SOURCES}
${GUI_SOURCES}
${PHYSICS_SOURCES}
${NET_SOURCES}
${AI_SOURCES}
${DEBUG_SOURCES}
${APPLE_CONFIG_SOURCES}
)

set(BTSOCCER_SOURCES
src/engine/main.cpp
${WIN_SOURCES}
)
set(BTSOCCER_HEADERS
)

set(BTSOCCER_DISTCALC
src/distcalc.cpp
${WIN_SOURCES}
)

set(BTSOCCER_TEST
src/unit_tests/testcase.h
src/unit_tests/testcase.cpp
src/unit_tests/baseaitestcase.h
src/unit_tests/baseaitestcase.cpp
src/unit_tests/fieldobjecttestcase.h
src/unit_tests/fieldobjecttestcase.cpp
src/unit_tests/rulestestcase.h
src/unit_tests/rulestestcase.cpp
src/unit_tests/runall.cpp
${WIN_SOURCES}
)

