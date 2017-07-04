# If APPLESEED_ROOT not set, use predefined paths
IF(NOT DEFINED APPLESEED_ROOT)
    IF ( ${CMAKE_HOST_UNIX} )
        IF( ${DARWIN} )
          # TODO: set to default install path when shipping out
          SET( CORTEX_APPLESEED_ROOT NOTFOUND )
        ELSE()
          # TODO: set to default install path when shipping out
          SET( CORTEX_APPLESEED_ROOT "/usr/local/appleseed" )
        ENDIF()
    ELSE()
        IF ( ${WINDOWS} )
          # TODO: set to 32-bit or 64-bit path
          SET( CORTEX_APPLESEED_ROOT NOTFOUND )
        ELSE()
          SET( CORTEX_APPLESEED_ROOT NOTFOUND )
        ENDIF()
    ENDIF()
ELSE()
    # Prefer ALEMBIC_ROOT set from the CMakeCache'd variable than default paths
    SET( CORTEX_APPLESEED_ROOT ${APPLESEED_ROOT})
ENDIF()

# Prefer APPLESEED_ROOT set from the environment over the CMakeCache'd variable
IF(NOT $ENV{APPLESEED_ROOT}x STREQUAL "x")
  SET( CORTEX_APPLESEED_ROOT $ENV{APPLESEED_ROOT})
ENDIF()


FIND_PATH( CORTEX_APPLESEED_INCLUDE_DIRECTORY foundation/core/appleseed.h
           PATHS
           "${CORTEX_APPLESEED_ROOT}/include"
           NO_DEFAULT_PATH
           NO_CMAKE_ENVIRONMENT_PATH
           NO_CMAKE_PATH
           NO_SYSTEM_ENVIRONMENT_PATH
           NO_CMAKE_SYSTEM_PATH
           DOC "The directory where appleseed headers are" )

SET( CORTEX_APPLESEED CORTEX_APPLESEED-NOTFOUND )
FIND_LIBRARY( CORTEX_APPLESEED appleseed
              PATHS
              "${CORTEX_APPLESEED_ROOT}/lib/"
              NO_DEFAULT_PATH
              NO_CMAKE_ENVIRONMENT_PATH
              NO_CMAKE_PATH
              NO_SYSTEM_ENVIRONMENT_PATH
              NO_CMAKE_SYSTEM_PATH
              DOC "The Appleseed library" )

IF ( ( ${CORTEX_APPLESEED_INCLUDE_DIRECTORY} STREQUAL "CORTEX_APPLESEED_INCLUDE_DIRECTORY-NOTFOUND" ) OR
     ( ${CORTEX_APPLESEED} STREQUAL "CORTEX_APPLESEED-NOTFOUND" ) )
  MESSAGE( STATUS "Appleseed not found" )
  SET( CORTEX_APPLESEED_FOUND FALSE )
ELSE()
  MESSAGE( STATUS "APPLESEED INCLUDE PATH: ${CORTEX_APPLESEED_INCLUDE_DIRECTORY}" )
  SET( CORTEX_APPLESEED_FOUND TRUE )
  SET( CORTEX_APPLESEED_LIBS ${CORTEX_APPLESEED} )
ENDIF()
