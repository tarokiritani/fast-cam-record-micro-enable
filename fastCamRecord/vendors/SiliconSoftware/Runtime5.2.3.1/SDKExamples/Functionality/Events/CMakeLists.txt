PROJECT(EventSDKExample)

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


ADD_EXECUTABLE(EventSubsystemSync
	EventSubsystemSync.cpp)
TARGET_LINK_LIBRARIES(EventSubsystemSync
	${FgLib5_LIBRARIES}
	${SISODISPLAYLIB_LIBRARIES}
)
SET_TARGET_PROPERTIES(EventSubsystemSync PROPERTIES FOLDER "Examples/Functionality/Events")


ADD_EXECUTABLE(EventSubsystemAsync
	EventSubsystemAsync.cpp)
TARGET_LINK_LIBRARIES(EventSubsystemAsync
	${FgLib5_LIBRARIES}
	${SISODISPLAYLIB_LIBRARIES}
)
SET_TARGET_PROPERTIES(EventSubsystemAsync PROPERTIES FOLDER "Examples/Functionality/Events")

INSTALL(TARGETS
	EventSubsystemAsync
	EventSubsystemSync
	DESTINATION bin/examples)
