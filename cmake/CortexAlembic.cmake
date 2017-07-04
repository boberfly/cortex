##-*****************************************************************************
##
## Copyright (c) 2009-2013,
##  Sony Pictures Imageworks Inc. and
##  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
##
## All rights reserved.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions are
## met:
## *       Redistributions of source code must retain the above copyright
## notice, this list of conditions and the following disclaimer.
## *       Redistributions in binary form must reproduce the above
## copyright notice, this list of conditions and the following disclaimer
## in the documentation and/or other materials provided with the
## distribution.
## *       Neither the name of Industrial Light & Magic nor the names of
## its contributors may be used to endorse or promote products derived
## from this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
## "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
## LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
## A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
## OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
## SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
## LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
## DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
## THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
## OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
##-*****************************************************************************


# We shall worry about windowsification later.

#-******************************************************************************
#-******************************************************************************
# FIRST, ALEMBIC STUFF
#-******************************************************************************
#-******************************************************************************

# If ALEMBIC_ROOT not set, use predefined paths
IF(NOT DEFINED ALEMBIC_ROOT)
    IF ( ${CMAKE_HOST_UNIX} )
        IF( ${DARWIN} )
          # TODO: set to default install path when shipping out
          SET( CORTEX_ALEMBIC_ROOT NOTFOUND )
        ELSE()
          # TODO: set to default install path when shipping out
          SET( CORTEX_ALEMBIC_ROOT "/usr/local/alembic" )
        ENDIF()
    ELSE()
        IF ( ${WINDOWS} )
          # TODO: set to 32-bit or 64-bit path
          SET( CORTEX_ALEMBIC_ROOT NOTFOUND )
        ELSE()
          SET( CORTEX_ALEMBIC_ROOT NOTFOUND )
        ENDIF()
    ENDIF()
ELSE()
    # Prefer ALEMBIC_ROOT set from the CMakeCache'd variable than default paths
    SET( CORTEX_ALEMBIC_ROOT ${ALEMBIC_ROOT})
ENDIF()

# Prefer ALEMBIC_ROOT set from the environment over the CMakeCache'd variable
IF(NOT $ENV{ALEMBIC_ROOT}x STREQUAL "x")
  SET( CORTEX_ALEMBIC_ROOT $ENV{ALEMBIC_ROOT})
ENDIF()


FIND_PATH( CORTEX_ALEMBIC_INCLUDE_DIRECTORY Alembic/Abc/All.h
           PATHS
           "${CORTEX_ALEMBIC_ROOT}/include"
           NO_DEFAULT_PATH
           NO_CMAKE_ENVIRONMENT_PATH
           NO_CMAKE_PATH
           NO_SYSTEM_ENVIRONMENT_PATH
           NO_CMAKE_SYSTEM_PATH
           DOC "The directory where alembic headers are" )

SET( CORTEX_ALEMBIC CORTEX_ALEMBIC-NOTFOUND )
FIND_LIBRARY( CORTEX_ALEMBIC Alembic
              PATHS
              "${CORTEX_ALEMBIC_ROOT}/lib/"
              NO_DEFAULT_PATH
              NO_CMAKE_ENVIRONMENT_PATH
              NO_CMAKE_PATH
              NO_SYSTEM_ENVIRONMENT_PATH
              NO_CMAKE_SYSTEM_PATH
              DOC "The Alembic library" )

IF ( ( ${CORTEX_ALEMBIC_INCLUDE_DIRECTORY} STREQUAL "CORTEX_ALEMBIC_INCLUDE_DIRECTORY-NOTFOUND" ) OR
     ( ${CORTEX_ALEMBIC} STREQUAL "CORTEX_ALEMBIC-NOTFOUND" ) )
  MESSAGE( STATUS "Alembic not found" )
  SET( CORTEX_ALEMBIC_FOUND FALSE )
ELSE()
  MESSAGE( STATUS "ALEMBIC INCLUDE PATH: ${CORTEX_ALEMBIC_INCLUDE_DIRECTORY}" )
  SET( CORTEX_ALEMBIC_FOUND TRUE )
  SET( CORTEX_ALEMBIC_LIBS ${CORTEX_ALEMBIC} )
ENDIF()
