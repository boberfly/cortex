
#-******************************************************************************
#-******************************************************************************
# CMake build script by Alex Fuller (boberfly@gmail.com)
# Based loosely on the Alembic and OIIO CMake scripts (Modules too)
#-******************************************************************************
#-******************************************************************************

# CMake version required
CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

# Declare a Cortex Project
PROJECT( Cortex )

IF (NOT DEFINED QUIET)
  SET(CMAKE_VERBOSE_MAKEFILE TRUE)
ENDIF()

# Declare the version
SET( PROJECT_VERSION_MAJOR  9 )
SET( PROJECT_VERSION_MINOR  21 )
SET( PROJECT_VERSION_PATCH  1 )
SET( PROJECT_VERSION_RELEASE_TYPE "" )
SET( PROJECT_VERSION
     ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-${PROJECT_VERSION_RELEASE_TYPE} )
SET( VERSION ${PROJECT_VERSION} )

ADD_DEFINITIONS( -DIE_CORE_MAJORVERSION=${PROJECT_VERSION_MAJOR}
                 -DIE_CORE_MINORVERSION=${PROJECT_VERSION_MINOR}
                 -DIE_CORE_PATCHVERSION=${PROJECT_VERSION_PATCH}
                 -DBOOST_FILESYSTEM_VERSION=3 )

OPTION( BUILD_STATIC_LIBS "Static library build" OFF )
OPTION( WITH_GL "OpenGL support" OFF )
OPTION( WITH_HOUDINI "Houdini support" OFF )
OPTION( WITH_MAYA "Maya support" OFF )
OPTION( WITH_NUKE "Nuke support" OFF )
OPTION( WITH_RMAN "Renderman support" OFF )
OPTION( WITH_TRUELIGHT "Truelight support" OFF )
OPTION( WITH_ARNOLD "Arnold support" OFF )
OPTION( WITH_ALEMBIC "Alembic support" OFF )
OPTION( WITH_APPLESEED "Appleseed support" OFF )
#IF( ${WITH_HOUDINI} )
  OPTION( WITH_MANTRA "Mantra support" OFF )
#ENDIF()
OPTION( WITH_TESTS "Compile tests" OFF )

MESSAGE(STATUS "CMAKE SYSTEM NAME = ${CMAKE_SYSTEM_NAME}" )

#-******************************************************************************
#-******************************************************************************
# PLATFORM SPECIFIC
#-******************************************************************************
#-******************************************************************************

# Windows flag
SET( WINDOWS FALSE )
IF( "${CMAKE_SYSTEM_NAME}" MATCHES "Windows" )
  SET( WINDOWS TRUE )
ENDIF()

SET( DARWIN FALSE )
IF( "${CMAKE_SYSTEM_NAME}" MATCHES "Darwin" )
  SET( DARWIN TRUE )
  IF( ${CMAKE_SYSTEM_VERSION} MATCHES "13." )
       ADD_DEFINITIONS( "-std=c++11" )
  ENDIF()
ENDIF()

SET( LINUX FALSE )
IF( "${CMAKE_SYSTEM_NAME}" MATCHES "Linux" )
  SET( LINUX TRUE )
ENDIF()

# Set some debug vs opt flags
IF ( "${CMAKE_BUILD_TYPE}" MATCHES "Debug" )
  ADD_DEFINITIONS( -DDEBUG=1 -UNDEBUG )
  IF ( NOT WINDOWS )
    ADD_DEFINITIONS( -pipe -Wall -O0)
  ENDIF()
ELSEIF ( "${CMAKE_BUILD_TYPE}" MATCHES "Release" )
  ADD_DEFINITIONS( -DNDEBUG=1 -UDEBUG )
  IF ( NOT WINDOWS )
    ADD_DEFINITIONS( -pipe -Wall -Werror -O3 -DNDEBUG -DBOOST_DISABLE_ASSERTS -Wno-unused-local-typedefs -Wno-strict-aliasing -Wno-maybe-uninitialized ) #-Wno-return-type )
  ENDIF()
ENDIF()

IF( ${WINDOWS} )
  ADD_DEFINITIONS ( -DOPENEXR_DLL -DNOMINMAX -D__PRETTY_FUNCTION__=__FUNCSIG__ -DBOOST_ALL_DYN_LINK )
  #ADD_DEFINITIONS( -DPLATFORM_WINDOWS -DPLATFORM=WINDOWS )
  set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} /MP )
ELSEIF( ${DARWIN} )
  #ADD_DEFINITIONS( -DPLATFORM_DARWIN -DPLATFORM=DARWIN "-arch x86_64" )
  #ADD_DEFINITIONS( -DPLATFORM_DARWIN -DPLATFORM=DARWIN )
ELSE()
  #ADD_DEFINITIONS( -DPLATFORM_LINUX -DPLATFORM=LINUX )
ENDIF()

ADD_DEFINITIONS( -DBOOST_PYTHON_MAX_ARITY=20 )

#-******************************************************************************
#-******************************************************************************
# INSTALLATION
#-******************************************************************************
#-******************************************************************************

# Tell me what my install location would be
MESSAGE(STATUS "The install dir is ${CMAKE_INSTALL_PREFIX}" )

# check in the source directory
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake )

#-******************************************************************************
#-******************************************************************************
# GLOBAL PACKAGE & INCLUDE/LINK DEPENDENCY
#-******************************************************************************
#-******************************************************************************

FIND_PACKAGE( Threads REQUIRED )
FIND_PACKAGE( ZLIB REQUIRED )
FIND_PACKAGE( TBB REQUIRED )
FIND_PACKAGE( PythonLibs 2.7 REQUIRED )
FIND_PACKAGE( Boost REQUIRED COMPONENTS signals iostreams filesystem date_time wave regex system chrono python )
FIND_PACKAGE( IlmBase REQUIRED )
FIND_PACKAGE( OpenEXR REQUIRED )
FIND_PACKAGE( TIFF )
FIND_PACKAGE( JPEG )
FIND_PACKAGE( PNG )
FIND_PACKAGE( Freetype )

INCLUDE_DIRECTORIES( ${TBB_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES( ${ILMBASE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${OPENEXR_INCLUDE_DIRS} )

LINK_DIRECTORIES( ${TBB_LIBRARY_DIRS} )
LINK_DIRECTORIES( ${Boost_LIBRARY_DIRS} )

#-******************************************************************************
#-******************************************************************************
# OPENGL
#-******************************************************************************
#-******************************************************************************
IF ( ${WITH_GL} )
  FIND_PACKAGE( OpenGL REQUIRED )
  FIND_PACKAGE( GLEW REQUIRED )
  INCLUDE_DIRECTORIES( ${GLEW_INCLUDE_DIR} )
ENDIF ()

IF ( ${WINDOWS} )
  # Do a windows OpenGL thing, whatever that may be
  SET( CORTEX_GL_LIBS glew32 ${GLUT_LIBRARY} ${OPENGL_LIBRARIES} )
  INCLUDE_DIRECTORIES ( ${GLUT_INCLUDE_DIR} )
  LINK_DIRECTORIES ( ${Glew_LIBRARY_DIR} )
ELSEIF( ${DARWIN} )
  # Do a darwin OpenGL thing.
  SET( CORTEX_GL_LIBS ${GLUT_LIBRARY} "-framework OpenGL" )
ELSE()
  SET( CORTEX_GL_LIBS GLEW ${GLUT_LIBRARY} ${OPENGL_LIBRARIES} )
ENDIF()

MESSAGE( STATUS "OpenGL Libraries: ${CORTEX_GL_LIBS}" )

#-******************************************************************************
#-******************************************************************************
# CORTEX
#-******************************************************************************
#-******************************************************************************

# Includes
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/include )

# Test what can be compiled
IF( EXISTS "${Boost_INCLUDE_DIRS}/boost/asio.hpp" )
  ADD_DEFINITIONS( -DIECORE_WITH_ASIO )
ELSE()
  MESSAGE(STATUS "WARNING: boost/asio.hpp not found, some functionality will be disabled.")
ENDIF()

ADD_DEFINITIONS( -DIECORE_WITH_SIGNALS )
#IF( ${Boost_SIGNALS_FOUND} )
#  ADD_DEFINITIONS( -DIECORE_WITH_SIGNALS )
#ELSE()
#  MESSAGE(STATUS "ERROR : unable to find boost signal library - some functionality will be disabled.")
#ENDIF()

IF( EXISTS "${Boost_INCLUDE_DIRS}/boost/math/special_functions/factorials.hpp" )
  ADD_DEFINITIONS( -DIECORE_WITH_BOOSTFACTORIAL )
ELSE()
  MESSAGE(STATUS "WARNING: boost/math/special_functions/factorials.hpp not found, some functionality will be disabled.")
ENDIF()

IF( EXISTS "${OPENEXR_INCLUDE_DIRS}/OpenEXR/ImfDeepFrameBuffer.h" )
  ADD_DEFINITIONS( -DIECORE_WITH_DEEPEXR )
ENDIF()

IF( ${TIFF_FOUND} )
  ADD_DEFINITIONS( -DIECORE_WITH_TIFF )
  INCLUDE_DIRECTORIES( ${TIFF_INCLUDE_DIR} )
ELSE()
  MESSAGE(STATUS "WARNING: no TIFF library found, no TIFF support, check TIFF_INCLUDE_PATH and TIFF_LIB_PATH.")
ENDIF()

IF( ${JPEG_FOUND} )
  ADD_DEFINITIONS( -DIECORE_WITH_JPEG )
  INCLUDE_DIRECTORIES( ${JPEG_INCLUDE_DIR} )
ELSE()
  MESSAGE(STATUS "WARNING: no JPEG library found, no JPEG support, check JPEG_INCLUDE_PATH and JPEG_LIB_PATH.")
ENDIF()

IF( ${PNG_FOUND} )
  ADD_DEFINITIONS( -DIECORE_WITH_PNG )
  INCLUDE_DIRECTORIES( ${PNG_INCLUDE_DIR} )
ELSE()
  MESSAGE(STATUS "WARNING: no PNG library found, no PNG support, check PNG_INCLUDE_PATH and PNG_LIB_PATH.")
ENDIF()

IF( ${FREETYPE_FOUND} )
  ADD_DEFINITIONS( -DIECORE_WITH_FREETYPE )
  INCLUDE_DIRECTORIES( ${FREETYPE_INCLUDE_DIRS} )
ELSE()
  MESSAGE(STATUS "WARNING: no FreeType library found, no font support, check FREETYPE_INCLUDE_PATH and FREETYPE_LIB_PATH.")
ENDIF()


# Add subdirectories, dependencies
#ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/include )
ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/src/IECore )
ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/src/IECorePython )
ADD_DEPENDENCIES( IECorePython IECore )
ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/src/IECorePythonModule )
# Python Module
ADD_DEPENDENCIES( _IECore IECore IECorePython )

# Add tests
IF( WITH_TESTS )
  MESSAGE( STATUS "CMake build scripts for IECore tests are currently not implemented!" )
  #ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/test/IECore )
ENDIF ()

# Inlcude GL
IF( DEFINED WITH_GL AND NOT WITH_GL )
  MESSAGE(STATUS "Skipping GL support")
ELSE()
  MESSAGE(STATUS "Building GL support" )
  ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/src/IECoreGL )
  ADD_DEPENDENCIES( IECoreGL IECore )
  # Python Module
  ADD_DEPENDENCIES( _IECoreGL IECore IECorePython IECoreGL )
  IF( WITH_TESTS )
    MESSAGE( STATUS "CMake build scripts for IECoreGL tests are currently not implemented!" )
    #ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/test/IECoreGL )
  ENDIF()
ENDIF()

# Include Houdini
IF( DEFINED WITH_HOUDINI AND NOT WITH_HOUDINI )
  MESSAGE(STATUS "Skipping Houdini support")
ELSE()
  MESSAGE( STATUS "About to include CortexHoudini.cmake" )
  INCLUDE( "${CMAKE_SOURCE_DIR}/cmake/CortexHoudini.cmake" )
  IF( CORTEX_HOUDINI_FOUND )
    MESSAGE(STATUS "Building Houdini support" )
    ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/src/IECoreHoudini )
    ADD_DEPENDENCIES( IECoreHoudini IECore )
    # Python Module
    ADD_DEPENDENCIES( _IECoreHoudini IECore IECorePython IECoreHoudini )
    # Plugin
    ADD_DEPENDENCIES( IECoreHoudiniPlugin IECore IECorePython IECoreHoudini )
    IF( WITH_TESTS )
      MESSAGE( STATUS "CMake build scripts for IECoreHoudini tests are currently not implemented!" )
      #ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/test/IECoreHoudini )
    ENDIF()
  ELSE()
    MESSAGE(FATAL_ERROR "For some reason, CORTEX_HOUDINI_FOUND is False?")
  ENDIF()
ENDIF()

# Include Maya
IF( DEFINED WITH_MAYA AND NOT WITH_MAYA )
  MESSAGE(STATUS "Skipping Maya support")
ELSE()
  MESSAGE( STATUS "About to include CortexMaya.cmake" )
  INCLUDE( "${CMAKE_SOURCE_DIR}/cmake/CortexMaya.cmake" )
  IF( CORTEX_MAYA_FOUND )
    #ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/mel )
    ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/src/IECoreMaya )
    IF( WITH_RMAN )
      ADD_DEPENDENCIES( IECoreMaya IECore IECoreRI )
    ELSE()
      ADD_DEPENDENCIES( IECoreMaya IECore )
    ENDIF()
    # Python Module
    ADD_DEPENDENCIES( _IECoreMaya IECore IECoreMaya )
    # Plugin
    ADD_DEPENDENCIES( IECoreMayaPlugin IECore IECoreMaya )
    IF( WITH_TESTS )
      MESSAGE( STATUS "CMake build scripts for IECoreMaya tests are currently not implemented!" )
      #ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/test/IECoreMaya )
    ENDIF()
  ELSE()
    MESSAGE(FATAL_ERROR "For some reason, CORTEX_MAYA_FOUND is False?")
  ENDIF()
ENDIF()

# Include Nuke
IF( DEFINED WITH_NUKE AND NOT WITH_NUKE )
  MESSAGE(STATUS "Skipping Nuke support")
ELSE()
  MESSAGE( STATUS "About to include CortexNuke.cmake" )
  INCLUDE( "${CMAKE_SOURCE_DIR}/cmake/CortexNuke.cmake" )
  IF( CORTEX_NUKE_FOUND )
    MESSAGE(STATUS "Building Nuke support" )
    ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/src/IECoreNuke )
    ADD_DEPENDENCIES( IECoreNuke IECore )
    # Python Module
    ADD_DEPENDENCIES( _IECoreNuke IECore IECorePython IECoreNuke )
    # Plugin
    ADD_DEPENDENCIES( IECoreNukePlugin IECore IECorePython IECoreNuke )
    IF( WITH_TESTS )
      MESSAGE( STATUS "CMake build scripts for IECoreNuke tests are currently not implemented!" )
      #ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/test/IECoreNuke )
    ENDIF()
  ELSE()
    MESSAGE(FATAL_ERROR "For some reason, CORTEX_NUKE_FOUND is False?")
  ENDIF()
ENDIF()

# Include Rman (PRman or 3Delight)
IF( DEFINED WITH_RMAN AND NOT WITH_RMAN )
  MESSAGE(STATUS "Skipping Rman support")
ELSE()
  MESSAGE( STATUS "About to include CortexRMan.cmake" )
  INCLUDE( "${CMAKE_SOURCE_DIR}/cmake/CortexRMan.cmake" )
  IF( CORTEX_RMAN_FOUND )
    MESSAGE(STATUS "Building RMan support" )
    ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/src/IECoreRI )
    ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/src/rmanDisplays/ieDisplay )
    ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/src/rmanProcedurals/python )
    ADD_DEPENDENCIES( IECoreRI IECore )
    # Python Module
    ADD_DEPENDENCIES( _IECoreRI IECore IECorePython IECoreRI )
    # Display driver
    ADD_DEPENDENCIES( rmanDisplay IECore IECoreRI )
    # Procedural
    ADD_DEPENDENCIES( rmanProcedural IECore IECoreRI )
    IF( WITH_TESTS )
      MESSAGE( STATUS "CMake build scripts for IECoreRI tests are currently not implemented!" )
      #ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/test/IECoreRI )
    ENDIF()
  ELSE()
    MESSAGE(FATAL_ERROR "For some reason, CORTEX_RMAN_FOUND is False?")
  ENDIF()
ENDIF()

# Include Truelight
IF( DEFINED WITH_TRUELIGHT AND NOT WITH_TRUELIGHT )
  MESSAGE(STATUS "Skipping Truelight support")
ELSE()
  MESSAGE( STATUS "About to include CortexTruelight.cmake" )
  INCLUDE( "${CMAKE_SOURCE_DIR}/cmake/CortexTruelight.cmake" )
  IF( CORTEX_TRUELIGHT_FOUND )
    MESSAGE(STATUS "Building Truelight support" )
    ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/src/IECoreTruelight )
    ADD_DEPENDENCIES( IECoreTruelight IECore )
    # Python Module
    ADD_DEPENDENCIES( _IECoreTruelight IECore IECorePython IECoreTruelight )
    IF( WITH_TESTS )
      MESSAGE( STATUS "CMake build scripts for IECoreTruelight tests are currently not implemented!" )
      #ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/test/IECoreTruelight )
    ENDIF()
  ELSE()
    MESSAGE(FATAL_ERROR "For some reason, CORTEX_TRUELIGHT_FOUND is False?")
  ENDIF()
ENDIF()

#-******************************************************************************
#-******************************************************************************
# CORTEX CONTRIB
#-******************************************************************************
#-******************************************************************************

# Include Arnold
IF( DEFINED WITH_ARNOLD AND NOT WITH_ARNOLD )
  MESSAGE(STATUS "Skipping Arnold support")
ELSE()
  MESSAGE( STATUS "About to include CortexArnold.cmake" )
  INCLUDE( "${CMAKE_SOURCE_DIR}/cmake/CortexArnold.cmake" )
  IF( CORTEX_ARNOLD_FOUND )
    MESSAGE(STATUS "Building Arnold support" )
    ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/contrib/IECoreArnold )
    ADD_DEPENDENCIES( IECoreArnold IECore )
    # Python Module
    ADD_DEPENDENCIES( _IECoreArnold IECore IECorePython IECoreArnold )
    # Display driver
    ADD_DEPENDENCIES( arnoldOutputDriver IECore IECoreArnold )
    # Procedural
    ADD_DEPENDENCIES( arnoldProcedural IECore IECoreArnold )
  ELSE()
    MESSAGE(FATAL_ERROR "For some reason, CORTEX_ARNOLD_FOUND is False?")
  ENDIF()
ENDIF()

# Include Alembic
IF( DEFINED WITH_ALEMBIC AND NOT WITH_ALEMBIC )
  MESSAGE(STATUS "Skipping Alembic support")
ELSE()
  MESSAGE( STATUS "About to include CortexAlembic.cmake" )
  INCLUDE( "${CMAKE_SOURCE_DIR}/cmake/CortexAlembic.cmake" )
  FIND_PACKAGE( HDF5 REQUIRED )
  IF( CORTEX_ALEMBIC_FOUND AND HDF5_FOUND )
    MESSAGE(STATUS "Building Alembic support" )
    ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/contrib/IECoreAlembic )
    ADD_DEPENDENCIES( IECoreAlembic IECore )
    # Python Module
    ADD_DEPENDENCIES( _IECoreAlembic IECore IECorePython IECoreAlembic )
  ELSE()
    IF( NOT CORTEX_ALEMBIC_FOUND )
      MESSAGE(FATAL_ERROR "For some reason, CORTEX_ALEMBIC_FOUND is False?")
    ELIF( NOT HDF5_FOUND )
      MESSAGE(FATAL_ERROR "For some reason, HDF5_FOUND is False?")
    ELSE()
      MESSAGE(FATAL_ERROR "Cannot find Alembic or HDF5")
    ENDIF()
  ENDIF()
ENDIF()

# Include Mantra
IF( DEFINED WITH_MANTRA AND NOT WITH_MANTRA )
  MESSAGE(STATUS "Skipping Mantra support")
ELSE()
  MESSAGE( STATUS "About to include CortexMantra.cmake" )
  INCLUDE( "${CMAKE_SOURCE_DIR}/cmake/CortexMantra.cmake" )
  IF( CORTEX_MANTRA_FOUND )
    MESSAGE(STATUS "Building Mantra support" )
    ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/contrib/IECoreMantra )
    ADD_DEPENDENCIES( IECoreMantra IECore IECoreHoudini )
    # Python Module
    ADD_DEPENDENCIES( _IECoreMantra IECore IECorePython IECoreMantra IECoreHoudini )
    # Procedural
    ADD_DEPENDENCIES( ieProceduralMantra IECore IECoreMantra IECoreHoudini )
  ELSE()
    MESSAGE(FATAL_ERROR "For some reason, CORTEX_MANTRA_FOUND is False?")
  ENDIF()
ENDIF()

# Include Appleseed
IF( DEFINED WITH_APPLESEED AND NOT WITH_APPLESEED )
  MESSAGE(STATUS "Skipping Appleseed support")
ELSE()
  MESSAGE( STATUS "About to include CortexAppleseed.cmake" )
  INCLUDE( "${CMAKE_SOURCE_DIR}/cmake/CortexAppleseed.cmake" )
  IF( CORTEX_APPLESEED_FOUND )
    MESSAGE(STATUS "Building Appleseed support" )
    ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/contrib/IECoreAppleseed )
    ADD_DEPENDENCIES( IECoreAppleseed IECore )
    # Python Module
    ADD_DEPENDENCIES( _IECoreAppleseed IECore IECorePython IECoreAppleseed )
  ELSE()
    MESSAGE(FATAL_ERROR "For some reason, CORTEX_APPLESEED_FOUND is False?")
  ENDIF()
ENDIF()