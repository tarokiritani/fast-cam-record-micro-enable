INCLUDE(SisoLibDir)

SET(COMPILER_LIB_DIR "")

SISO_LIB_DIR("" LIB_DIRS_IGNORED COMPILER_LIB_DIR)

INCLUDE(SisoPlatform)

IF (WIN32)

	SET(SISOPLATFORM_DEFINITIONS -D_WIN32 -D_CRT_SECURE_NO_DEPRECATE)

ELSEIF (${CMAKE_SYSTEM_NAME} STREQUAL "QNX")
	SET(SISOPLATFORM_DEFINITIONS -DQNX)
	SET(QNX 1)

ELSEIF (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")

	SET(SISOPLATFORM_DEFINITIONS -DLINUX)
	SET(LINUX 1)

ELSEIF (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")

	SET(SISOPLATFORM_DEFINITIONS -DDARWIN)
	SET(DARWIN 1)

ELSE (WIN32)

	MESSAGE(FATAL_ERROR "Unsupported build system ${CMAKE_SYSTEM_NAME}")

ENDIF (WIN32)
