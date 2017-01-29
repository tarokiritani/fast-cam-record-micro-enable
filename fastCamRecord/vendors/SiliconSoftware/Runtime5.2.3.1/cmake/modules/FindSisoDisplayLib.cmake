IF (NOT SISODISPLAYLIB_FOUND)
	FILE(TO_CMAKE_PATH "$ENV{SISODIR5}" SISODIR5)
	FILE(TO_CMAKE_PATH "$ENV{SISODIR4}" SISODIR4)
	FILE(TO_CMAKE_PATH "$ENV{SISODIRME4}" SISODIRME4)
	FILE(TO_CMAKE_PATH "$ENV{SISODIR}" SISODIR)

	FIND_PATH(SISODISPLAYLIB_INCLUDE_DIR NAMES SisoDisplay.h fgrab_prototyp.h
		PATHS
		${CMAKE_INSTALL_PREFIX}/include
		${SISODIR5}/include
		${SISODIR}/include
		${SISODIR4}/include
		${SISODIRME4}/include
		"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Silicon Software GmbH\\Runtime5;Info]/include"
	)

	INCLUDE(SisoLibDir)
	SISO_LIB_DIR("${SISODIR5}/lib" LIB_DIRS COMPILER_LIB_DIR)
	SISO_LIB_DIR("${SISODIR4}/lib" LIB_DIRS)
	SISO_LIB_DIR("${SISODIRME4}/lib" LIB_DIRS)
	SISO_LIB_DIR("${SISODIR}/lib" LIB_DIRS)

	FIND_LIBRARY(SISODISPLAYLIB_LIBRARY NAMES display display_lib
		PATHS
		${CMAKE_INSTALL_PREFIX}/lib/${COMPILER_LIB_DIR}
		${CMAKE_INSTALL_PREFIX}/lib
		${LIB_DIRS}
		"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Silicon Software GmbH\\Runtime5;Info]/lib/${COMPILER_LIB_DIR}"
		"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Silicon Software GmbH\\Runtime5;Info]/lib"
	)

	INCLUDE(FindPackageHandleStandardArgs)
	FIND_PACKAGE_HANDLE_STANDARD_ARGS(SISODISPLAYLIB DEFAULT_MSG SISODISPLAYLIB_LIBRARY SISODISPLAYLIB_INCLUDE_DIR)
ENDIF (NOT SISODISPLAYLIB_FOUND)

IF (SISODISPLAYLIB_FOUND)
	SET(SISODISPLAYLIB_LIBRARIES ${SISODISPLAYLIB_LIBRARY})
ENDIF (SISODISPLAYLIB_FOUND)
