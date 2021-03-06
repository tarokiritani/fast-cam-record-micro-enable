IF (NOT GBELIB_FOUND)
	FILE(TO_CMAKE_PATH "$ENV{SISODIR5}" SISODIR5)

	IF (GBELIB_INCLUDE_DIR AND GBELIB_LIBRARY)
		SET(GBELIB_FIND_QUIETLY TRUE)
	ENDIF (GBELIB_INCLUDE_DIR AND GBELIB_LIBRARY)

	FIND_PATH(GBELIB_INCLUDE_DIR gbe.h
		PATHS
		$ENV{GBELIB}/include
		${CMAKE_INSTALL_PREFIX}/include
		${SISODIR5}/include
		"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Silicon Software GmbH\\Runtime5;Info]/include"
	)

	FIND_PATH(GENICAM_INCLUDE_DIR GenICam.h
		PATHS
		$ENV{GBELIB}/genicam/library/CPP/include
		${CMAKE_INSTALL_PREFIX}/genicam/library/CPP/include
		${SISODIR5}/genicam/library/CPP/include
		"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Silicon Software GmbH\\Runtime5;Info]/genicam/library/CPP/include"
	)
	
	IF (UNIX)
		SET(GENICAM_LINK_BASEDIR "${GENICAM_INCLUDE_DIR}/../../../bin")
	ELSE (UNIX)
		SET(GENICAM_LINK_BASEDIR "${GENICAM_INCLUDE_DIR}/../lib")
	ENDIF (UNIX)

	INCLUDE(SisoLibDir)
	SISO_LIB_DIR("${SISODIR5}/lib" LIB_DIRS COMPILER_LIB_DIR)

	IF (UNIX)
		SET(GBELIB_NAME libgbelib.a)
	ELSE (UNIX)
		SET(GBELIB_NAME gbelib)
	ENDIF (UNIX)

	FIND_LIBRARY(GBELIB_LIBRARY NAMES ${GBELIB_NAME}
		PATHS
		$ENV{GBELIB}/lib/${COMPILER_LIB_DIR}
		$ENV{GBELIB}/lib
		${CMAKE_INSTALL_PREFIX}/lib/${COMPILER_LIB_DIR}
		${CMAKE_INSTALL_PREFIX}/lib
		${LIB_DIRS}
		${SISODIR5}/lib/${COMPILER_LIB_DIR}
		"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Silicon Software GmbH\\Runtime5;Info]/lib/${COMPILER_LIB_DIR}"
		"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Silicon Software GmbH\\Runtime5;Info]/lib"
	)

	INCLUDE(FindPackageHandleStandardArgs)
	FIND_PACKAGE_HANDLE_STANDARD_ARGS(GBELIB DEFAULT_MSG GBELIB_LIBRARY GBELIB_INCLUDE_DIR GENICAM_INCLUDE_DIR)
ENDIF (NOT GBELIB_FOUND)

IF (GBELIB_FOUND)
	SET(GBELIB_LIBRARIES ${GBELIB_LIBRARY})
ENDIF (GBELIB_FOUND)
