PROJECT(GbeFeatures)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

#where to find local cmake modules
FILE(TO_CMAKE_PATH "$ENV{SISODIR5}" SISODIR5)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_INSTALL_PREFIX}/cmake/modules" "${SISODIR5}/cmake/modules")
INCLUDE(SisoPlatform)
FIND_PACKAGE(FgLib5 REQUIRED)
FIND_PACKAGE(SisoDisplayLib REQUIRED)
FIND_PACKAGE(GbeLib REQUIRED)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

INCLUDE_DIRECTORIES(
	${FgLib5_INCLUDE_DIR}
	${SISODISPLAYLIB_INCLUDE_DIR}
	${GBELIB_INCLUDE_DIR}
	${GENICAM_INCLUDE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/../include
)

ADD_EXECUTABLE(GbeForceIp GbeForceIp.cpp GbeFeaturesCommon.h)
TARGET_LINK_LIBRARIES(GbeForceIp ${FgLib5_LIBRARIES} ${GBELIB_LIBRARIES})
SET_TARGET_PROPERTIES(GbeForceIp PROPERTIES FOLDER "Examples/CameraInterfaces/GbE/GbeFeatures")
INSTALL(TARGETS GbeForceIp DESTINATION bin/examples)

ADD_EXECUTABLE(GbeEvents GbeEvents.cpp GbeFeaturesCommon.h)
TARGET_LINK_LIBRARIES(GbeEvents ${FgLib5_LIBRARIES} ${GBELIB_LIBRARIES} ${SISODISPLAYLIB_LIBRARIES})
SET_TARGET_PROPERTIES(GbeEvents PROPERTIES FOLDER "Examples/CameraInterfaces/GbE/GbeFeatures")
INSTALL(TARGETS GbeEvents DESTINATION bin/examples)

ADD_EXECUTABLE(GbeActionCommands GbeActionCommands.cpp GbeFeaturesCommon.h)
TARGET_LINK_LIBRARIES(GbeActionCommands ${FgLib5_LIBRARIES} ${GBELIB_LIBRARIES} ${SISODISPLAYLIB_LIBRARIES})
SET_TARGET_PROPERTIES(GbeActionCommands PROPERTIES FOLDER "Examples/CameraInterfaces/GbE/GbeFeatures")
INSTALL(TARGETS GbeActionCommands DESTINATION bin/examples)

IF (WIN32)
	IF (${CMAKE_CL_64})
		LINK_DIRECTORIES(${GENICAM_LINK_BASEDIR}/Win64_x64)
	ELSE (${CMAKE_CL_64})
		LINK_DIRECTORIES(${GENICAM_LINK_BASEDIR}/Win32_i86)
	ENDIF (${CMAKE_CL_64})
ELSE (WIN32)
	IF (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
		LINK_DIRECTORIES(${GENICAM_LINK_BASEDIR}/Linux64_x64)
	ELSE (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
		LINK_DIRECTORIES(${GENICAM_LINK_BASEDIR}/Linux32_i86)
	ENDIF (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
ENDIF (WIN32)

ADD_EXECUTABLE(GbeInternalGenICam GbeInternalGenICam.cpp GbeFeaturesCommon.h)
TARGET_LINK_LIBRARIES(GbeInternalGenICam ${FgLib5_LIBRARIES} ${GBELIB_LIBRARIES})
IF (WIN32)
	SET_TARGET_PROPERTIES(GbeInternalGenICam PROPERTIES LINK_FLAGS "/DELAYLOAD:GCBase_MD_VC80_v2_3.dll /DELAYLOAD:GenApi_MD_VC80_v2_3.dll")
	TARGET_LINK_LIBRARIES(GbeInternalGenICam delayimp.lib)
ELSE (WIN32)
	TARGET_LINK_LIBRARIES(GbeInternalGenICam GCBase_gcc40_v2_3 GenApi_gcc40_v2_3 MathParser_gcc40_v2_3 log4cpp_gcc40_v2_3 Log_gcc40_v2_3)
ENDIF (WIN32)
SET_TARGET_PROPERTIES(GbeInternalGenICam PROPERTIES FOLDER "Examples/CameraInterfaces/GbE/GbeFeatures")
INSTALL(TARGETS GbeInternalGenICam DESTINATION bin/examples)
