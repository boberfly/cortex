ADD_DEFINITIONS( -DIECORE_EXPORTS )

FILE( GLOB H_FILES_PRIVATE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "../../include/IECore/private/*.h" )

FILE( GLOB H_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "../../include/IECore/*.h" "../../include/IECore/*.inl" )

FILE( GLOB CXX_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp" )

IF( NOT EXISTS "${Boost_INCLUDE_DIRS}/boost/asio.hpp" )
  LIST( REMOVE_ITEM CXX_FILES 
        ClientDisplayDriver.cpp
        DisplayDriver.cpp
        ImageDisplayDriver.cpp
        DisplayDriverServer.cpp
        OBJReaderBinding.cpp )
ENDIF()

IF( NOT EXISTS "${Boost_INCLUDE_DIRS}/boost/math/special_functions/factorials.hpp" )
  LIST( REMOVE_ITEM CXX_FILES
        AssociatedLegendre.cpp
        src/IECore/SphericalHarmonics.cpp
        SphericalHarmonicsTensor.cpp
        SphericalHarmonicsRotationMatrix.cpp
        SphericalHarmonicsTransferMatrix.cpp
        SphericalHarmonicsProjector.cpp
        src/IECore/EnvMapSHProjector.cpp
        src/IECore/ImageConvolveOp.cpp )
ENDIF()

IF( NOT EXISTS "${OPENEXR_INCLUDE_DIR}/OpenEXR/ImfDeepFrameBuffer.h" )
  LIST( REMOVE_ITEM CXX_FILES
        EXRDeepImageReader.cpp
        EXRDeepImageWriter.cpp )
ENDIF()

SET( THIS_LIBS
     ${Boost_LIBRARIES}
     ${TBB_LIBRARIES}
     ${ILMBASE_LIBRARIES}
     ${OPENEXR_LIBRARIES}
     ${ZLIB_LIBRARIES} )

IF(NOT TIFF_FOUND )
  LIST( REMOVE_ITEM CXX_FILES
        TIFFImageWriter.cpp
        TIFFImageReader.cpp
        ScopedTIFFErrorHandler.cpp )
ELSE()
  set( THIS_LIBS ${THIS_LIBS}
        ${TIFF_LIBRARIES} )
ENDIF()

IF(NOT JPEG_FOUND )
  LIST( REMOVE_ITEM CXX_FILES
        JPEGImageWriter.cpp
        JPEGImageReader.cpp )
ELSE()
  set( THIS_LIBS ${THIS_LIBS}
        ${JPEG_LIBRARIES} )
ENDIF()

IF(NOT PNG_FOUND )
  LIST( REMOVE_ITEM CXX_FILES
        PNGImageReader.cpp )
ELSE()
  set( THIS_LIBS ${THIS_LIBS}
        ${PNG_LIBRARIES} )
ENDIF()

IF(NOT FREETYPE_FOUND )
  LIST( REMOVE_ITEM CXX_FILES
        Font.cpp )
ELSE()
  set( THIS_LIBS ${THIS_LIBS}
        ${FREETYPE_LIBRARIES} )
ENDIF()

SET( SOURCE_FILES ${CXX_FILES} ${H_FILES} ${H_FILES_PRIVATE} )

IF( BUILD_STATIC_LIBS )
  ADD_LIBRARY( IECore STATIC ${SOURCE_FILES} )
ELSE()
  ADD_LIBRARY( IECore SHARED ${SOURCE_FILES} )
ENDIF()

TARGET_LINK_LIBRARIES( IECore ${THIS_LIBS} )

INSTALL( TARGETS IECore
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib/static )

INSTALL( FILES ${H_FILES}
         DESTINATION include/IECore )
