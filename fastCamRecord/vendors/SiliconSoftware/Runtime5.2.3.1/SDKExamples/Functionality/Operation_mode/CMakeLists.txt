PROJECT(Trigger)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

#where to find local cmake modules
FILE(TO_CMAKE_PATH "$ENV{SISODIR5}" SISODIR5)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_INSTALL_PREFIX}/cmake/modules" "${SISODIR5}/cmake/modules")

INCLUDE(SisoPlatform)
FIND_PACKAGE(FgLib5 REQUIRED)
FIND_PACKAGE(SisoDisplayLib REQUIRED)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

INCLUDE_DIRECTORIES(
	${FgLib5_INCLUDE_DIR}
	${SISODISPLAYLIB_INCLUDE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

ADD_EXECUTABLE(AreaExternal
	AreaExternal.cpp)
TARGET_LINK_LIBRARIES(AreaExternal
	${FgLib5_LIBRARIES}
	${SISODISPLAYLIB_LIBRARIES}
)
SET_TARGET_PROPERTIES(AreaExternal PROPERTIES FOLDER "Examples/Functionality/Trigger")

ADD_EXECUTABLE(AreaFreeRun
	AreaFreeRun.cpp)
TARGET_LINK_LIBRARIES(AreaFreeRun
	${FgLib5_LIBRARIES}
	${SISODISPLAYLIB_LIBRARIES}
)
SET_TARGET_PROPERTIES(AreaFreeRun PROPERTIES FOLDER "Examples/Functionality/Trigger")

ADD_EXECUTABLE(AreaGrabberControl
	AreaGrabberControl.cpp)
TARGET_LINK_LIBRARIES(AreaGrabberControl
	${FgLib5_LIBRARIES}
	${SISODISPLAYLIB_LIBRARIES}
)
SET_TARGET_PROPERTIES(AreaGrabberControl PROPERTIES FOLDER "Examples/Functionality/Trigger")

ADD_EXECUTABLE(AreaSoftware
	AreaSoftware.cpp)
TARGET_LINK_LIBRARIES(AreaSoftware
	${FgLib5_LIBRARIES}
	${SISODISPLAYLIB_LIBRARIES}
)
SET_TARGET_PROPERTIES(AreaSoftware PROPERTIES FOLDER "Examples/Functionality/Trigger")

ADD_EXECUTABLE(LineFreeRunImg
	LineFreeRunImg.cpp)
TARGET_LINK_LIBRARIES(LineFreeRunImg
	${FgLib5_LIBRARIES}
	${SISODISPLAYLIB_LIBRARIES}
)
SET_TARGET_PROPERTIES(LineFreeRunImg PROPERTIES FOLDER "Examples/Functionality/Trigger")

ADD_EXECUTABLE(LineGatedAsyncTriggerLine
	LineGatedAsyncTriggerLine.cpp)
TARGET_LINK_LIBRARIES(LineGatedAsyncTriggerLine
	${FgLib5_LIBRARIES}
	${SISODISPLAYLIB_LIBRARIES}
)
SET_TARGET_PROPERTIES(LineGatedAsyncTriggerLine PROPERTIES FOLDER "Examples/Functionality/Trigger")

ADD_EXECUTABLE(LineGatedGrabberControlledLine
	LineGatedGrabberControlledLine.cpp)
TARGET_LINK_LIBRARIES(LineGatedGrabberControlledLine
	${FgLib5_LIBRARIES}
	${SISODISPLAYLIB_LIBRARIES}
)
SET_TARGET_PROPERTIES(LineGatedGrabberControlledLine PROPERTIES FOLDER "Examples/Functionality/Trigger")

ADD_EXECUTABLE(LineGrabberControlledLine
	LineGrabberControlledLine.cpp)
TARGET_LINK_LIBRARIES(LineGrabberControlledLine
	${FgLib5_LIBRARIES}
	${SISODISPLAYLIB_LIBRARIES}
)
SET_TARGET_PROPERTIES(LineGrabberControlledLine PROPERTIES FOLDER "Examples/Functionality/Trigger")

ADD_EXECUTABLE(LineStartTriggerImg
	LineStartTriggerImg.cpp)
TARGET_LINK_LIBRARIES(LineStartTriggerImg
	${FgLib5_LIBRARIES}
	${SISODISPLAYLIB_LIBRARIES}
)
SET_TARGET_PROPERTIES(LineStartTriggerImg PROPERTIES FOLDER "Examples/Functionality/Trigger")

ADD_EXECUTABLE(LineStartTriggerLine
	LineStartTriggerLine.cpp)
TARGET_LINK_LIBRARIES(LineStartTriggerLine
	${FgLib5_LIBRARIES}
	${SISODISPLAYLIB_LIBRARIES}
)
SET_TARGET_PROPERTIES(LineStartTriggerLine PROPERTIES FOLDER "Examples/Functionality/Trigger")

ADD_EXECUTABLE(LineTriggerGatedImg
	LineTriggerGatedImg.cpp)
TARGET_LINK_LIBRARIES(LineTriggerGatedImg
	${FgLib5_LIBRARIES}
	${SISODISPLAYLIB_LIBRARIES}
)
SET_TARGET_PROPERTIES(LineTriggerGatedImg PROPERTIES FOLDER "Examples/Functionality/Trigger")

ADD_EXECUTABLE(LineTriggerGatedMultiBufferImg
	LineTriggerGatedMultiBufferImg.cpp)
TARGET_LINK_LIBRARIES(LineTriggerGatedMultiBufferImg
	${FgLib5_LIBRARIES}
	${SISODISPLAYLIB_LIBRARIES}
)
SET_TARGET_PROPERTIES(LineTriggerGatedMultiBufferImg PROPERTIES FOLDER "Examples/Functionality/Trigger")

INSTALL(TARGETS AreaExternal
	AreaFreeRun
	AreaGrabberControl
	AreaSoftware
	LineFreeRunImg
	LineGatedAsyncTriggerLine
	LineGatedGrabberControlledLine
	LineGrabberControlledLine
	LineStartTriggerImg
	LineStartTriggerLine
	LineTriggerGatedImg
	LineTriggerGatedMultiBufferImg
	DESTINATION bin/examples)
