
#-******************************************************************************
#-******************************************************************************
# FIND RMAN
#-******************************************************************************
#-******************************************************************************

# If RMAN_ROOT not set, use predefined paths
IF(NOT DEFINED RMAN_ROOT)
    IF ( ${CMAKE_HOST_UNIX} )
        IF( ${DARWIN} )
          # TODO: set to default install path when shipping out
          SET( CORTEX_RMAN_ROOT NOTFOUND )
        ELSE()
          # TODO: set to default install path when shipping out
          SET( CORTEX_RMAN_ROOT "/sww/tools/pixar/rman15" )
        ENDIF()
    ELSE()
        IF ( ${WINDOWS} )
          # TODO: set to 32-bit or 64-bit path
          SET( CORTEX_RMAN_ROOT NOTFOUND )
        ELSE()
          SET( CORTEX_RMAN_ROOT NOTFOUND )
        ENDIF()
    ENDIF()
ELSE()
    # Prefer RMAN_ROOT set from the CMakeCache'd variable than default paths
    SET( CORTEX_RMAN_ROOT ${RMAN_ROOT})
ENDIF()

# Prefer RMAN_ROOT set from the environment over the CMakeCache'd variable
IF(NOT $ENV{RMAN_ROOT}x STREQUAL "x")
  SET( CORTEX_RMAN_ROOT $ENV{RMAN_ROOT})
ENDIF()


FIND_PATH( CORTEX_RMAN_INCLUDE_PATH ri.h
           PATHS
           "${CORTEX_RMAN_ROOT}/include/"
           NO_DEFAULT_PATH
           NO_CMAKE_ENVIRONMENT_PATH
           NO_CMAKE_PATH
           NO_SYSTEM_ENVIRONMENT_PATH
           NO_CMAKE_SYSTEM_PATH
           DOC "The directory where ri.h resides" )

MESSAGE( STATUS ${CORTEX_RMAN_INCLUDE_PATH})
# Test to see if it's PRman or 3Delight
SET( RMAN_LIB_NAME "prman" )
SET( CORTEX_DELIGHT 0 )

IF( ${WINDOWS} )
  MESSAGE( STATUS "${CORTEX_RMAN_ROOT}lib/3DelightFor3dfm.lib" )
  IF ( EXISTS "${CORTEX_RMAN_ROOT}lib/3DelightFor3dfm.lib" )
    MESSAGE( STATUS "FOUNDDDD" )
    SET( RMAN_LIB_NAME "3DelightFor3dfm" )
    SET( CORTEX_DELIGHT 1 )
  ENDIF()
  IF ( EXISTS "${CORTEX_RMAN_ROOT}lib/3Delight.lib" )
    SET( RMAN_LIB_NAME "3Delight" )
    SET( CORTEX_DELIGHT 1 )
  ENDIF()
ELSEIF( ${DARWIN} )
  IF ( EXISTS "${CORTEX_RMAN_ROOT}/lib/lib3delight.dynlib" )
    SET( RMAN_LIB_NAME "lib3delight" )
    SET( CORTEX_DELIGHT 1 )
  ENDIF()
ELSEIF( ${LINUX} )
  IF ( EXISTS "${CORTEX_RMAN_ROOT}/lib/lib3delight.so" )
    SET( RMAN_LIB_NAME "lib3delight.so" )
    SET( CORTEX_DELIGHT 1 )
  ENDIF()
ENDIF()

IF ( ${CORTEX_DELIGHT} )
  MESSAGE( STATUS "3Delight found" )
ENDIF()

SET( CORTEX_RMAN_LIBRMAN CORTEX_RMAN_LIBRMAN-NOTFOUND )
#FIND_LIBRARY( CORTEX_RMAN_LIBRMAN ${RMAN_LIB_NAME}
FIND_LIBRARY( CORTEX_RMAN_LIBRMAN 3DelightFor3dfm
              PATHS
              "${CORTEX_RMAN_ROOT}/lib/"
              NO_DEFAULT_PATH
              NO_CMAKE_ENVIRONMENT_PATH
              NO_CMAKE_PATH
              NO_SYSTEM_ENVIRONMENT_PATH
              NO_CMAKE_SYSTEM_PATH
              DOC "The prman library" )


IF( ${WINDOWS} )
  SET( RMAN_COMPILE_FLAGS "/c /nologo /MT /TP /DWIN32" )
  SET( RMAN_LINK_FLAGS "/nologo /dll /LIBPATH:\"%RMANTREE%\\lib\" ${RMAN_LIB_NAME}.lib" )
ELSEIF( ${DARWIN} )
  SET( RMAN_COMPILE_FLAGS "-c" )
  SET( RMAN_LINK_FLAGS "-bundle -undefined dynamic_lookup" )
ELSEIF( ${LINUX} )
  SET( RMAN_COMPILE_FLAGS "-c -fPIC" )
  SET( RMAN_LINK_FLAGS "-shared" )
ENDIF()

IF ( ( ${CORTEX_RMAN_INCLUDE_PATH} STREQUAL "CORTEX_RMAN_INCLUDE_PATH-NOTFOUND" ) OR
     ( ${CORTEX_RMAN_LIBRMAN} STREQUAL "CORTEX_RMAN_LIBRMAN-NOTFOUND" ) )
  MESSAGE( STATUS "RMan not found" )
  SET( CORTEX_RMAN_FOUND FALSE )
ELSE()
  MESSAGE( STATUS "RMAN INCLUDE PATH: ${CORTEX_RMAN_INCLUDE_PATH}" )
  MESSAGE( STATUS "libprman: ${CORTEX_RMAN_LIBRMAN}" )
  SET( CORTEX_RMAN_FOUND TRUE )
  SET( CORTEX_RMAN_LIBS ${CORTEX_RMAN_LIBRMAN} )
ENDIF()

UNSET( RMAN_LIB_NAME )

##-*****************************************************************************
##-*****************************************************************************
# Macro for making rman plugins
##-*****************************************************************************
##-*****************************************************************************
MACRO(ADD_RMAN_CXX_PLUGIN PluginName SourceFile1 )

  IF( NOT ${CORTEX_RMAN_FOUND} )
    MESSAGE( FATAL_ERROR "PRMan is not found. :(" )
  ENDIF()

  GET_FILENAME_COMPONENT( PluginNameNoDirectory ${PluginName} NAME )
  GET_FILENAME_COMPONENT( PluginNameFullPath ${PluginName} ABSOLUTE )

  SET( TMP_SOURCES ${SourceFile1} ${ARGN} )
  SET( ${PluginName}_SOURCES ${TMP_SOURCES} )

  INCLUDE_DIRECTORIES( ${CORTEX_RMAN_INCLUDE_PATH} )

  ADD_LIBRARY( ${PluginName} MODULE ${TMP_SOURCES} )

  SET_TARGET_PROPERTIES( ${PluginName}
                         PROPERTIES
                         COMPILE_FLAGS ${RMAN_COMPILE_FLAGS}
                         LINK_FLAGS ${RMAN_LINK_FLAGS}
                         PREFIX "" )

  TARGET_LINK_LIBRARIES ( ${PluginName} ${CORTEX_RMAN_LIBRMAN} )

ENDMACRO(ADD_RMAN_CXX_PLUGIN)
