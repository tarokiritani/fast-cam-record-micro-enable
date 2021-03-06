IF (NOT SISOIOLIB_FOUND)
	FILE(TO_CMAKE_PATH "$ENV{SISODIR5}" SISODIR5)
	FILE(TO_CMAKE_PATH "$ENV{SISODIR4}" SISODIR4)
	FILE(TO_CMAKE_PATH "$ENV{SISODIRME4}" SISODIRME4)
	FILE(TO_CMAKE_PATH "$ENV{SISODIR}" SISODIR)

	IF (SISOIOLIB_INCLUDE_DIR AND SISOIOLIB_LIBRARY)
		SET(SISOIOLIB_FIND_QUIETLY TRUE)
	ENDIF (SISOIOLIB_INCLUDE_DIR AND SISOIOLIB_LIBRARY)

	FIND_PATH(SISOIOLIB_INCLUDE_DIR sisoIo.h
		PATHS
		$ENV{SISOIOLIB}/include
		${CMAKE_INSTALL_PREFIX}/include
		$ENV{SISODIR5}/include
		$ENV{SISODIR4}/include
		$ENV{SISODIRME4}/include
		$ENV{SISODIR}/include
		"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Silicon Software GmbH\\Runtime5;Info]/include"
		"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Silicon Software GmbH\\microEnableIV\\version3.0\\info]/include"
	)

	if (WIN32)
		SET(SISOIOLIB_NAME iolibrt iolib4 iolib)
	else (WIN32)
		SET(SISOIOLIB_NAME sisoiolibrt sisoiolib4 sisoiolib)
	endif (WIN32)

	INCLUDE(SisoLibDir)
	SISO_LIB_DIR("${SISODIR5}/lib" LIB_DIRS COMPILER_LIB_DIR)
	SISO_LIB_DIR("${SISODIR4}/lib" LIB_DIRS)
	SISO_LIB_DIR("${SISODIRME4}/lib" LIB_DIRS)
	SISO_LIB_DIR("${SISODIR}/lib" LIB_DIRS)

	FIND_LIBRARY(SISOIOLIB_LIBRARY NAMES ${SISOIOLIB_NAME}
		PATHS
		$ENV{SISOIOLIB}/lib
		${CMAKE_INSTALL_PREFIX}/lib
		${LIB_DIRS}
		"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Silicon Software GmbH\\Runtime5;Info]/lib/${COMPILER_LIB_DIR}"
		"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Silicon Software GmbH\\Runtime5;Info]/lib"
		"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Silicon Software GmbH\\microEnableIV\\version3.0\\info]/lib/${COMPILER_LIB_DIR}"
		"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Silicon Software GmbH\\microEnableIV\\version3.0\\info]/lib"
	)

	INCLUDE(FindPackageHandleStandardArgs)
	FIND_PACKAGE_HANDLE_STANDARD_ARGS(SISOIOLIB DEFAULT_MSG SISOIOLIB_LIBRARY SISOIOLIB_INCLUDE_DIR)
ENDIF (NOT SISOIOLIB_FOUND)

IF (SISOIOLIB_FOUND)
	SET(SISOIOLIB_LIBRARIES ${SISOIOLIB_LIBRARY})
ENDIF (SISOIOLIB_FOUND)
