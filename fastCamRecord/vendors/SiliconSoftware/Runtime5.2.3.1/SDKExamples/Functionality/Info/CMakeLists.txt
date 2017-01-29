PROJECT(Information)

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

ADD_EXECUTABLE(ImageTag
	ImageTag.cpp)
TARGET_LINK_LIBRARIES(ImageTag
	${FgLib5_LIBRARIES}
	${SISODISPLAYLIB_LIBRARIES}
)
SET_TARGET_PROPERTIES(ImageTag PROPERTIES FOLDER "Examples/Functionality/Information")

ADD_EXECUTABLE(TimeStamps
	TimeStamps.cpp)
TARGET_LINK_LIBRARIES(TimeStamps
	${FgLib5_LIBRARIES}
	${SISODISPLAYLIB_LIBRARIES}
)
SET_TARGET_PROPERTIES(TimeStamps PROPERTIES FOLDER "Examples/Functionality/Information")

ADD_EXECUTABLE(BoardInformation
	BoardInformation.c)
TARGET_LINK_LIBRARIES(BoardInformation
	${FgLib5_LIBRARIES}
)
SET_TARGET_PROPERTIES(BoardInformation PROPERTIES FOLDER "Examples/Functionality/Information")


ADD_EXECUTABLE(SystemInfo
	SystemInfo.cpp)
TARGET_LINK_LIBRARIES(SystemInfo
	${FgLib5_LIBRARIES}
)
SET_TARGET_PROPERTIES(SystemInfo PROPERTIES FOLDER "Examples/Functionality/Information")


INSTALL(TARGETS
	ImageTag
	TimeStamps
	BoardInformation
	DESTINATION bin/examples)
