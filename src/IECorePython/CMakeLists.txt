ADD_DEFINITIONS( -DIECOREPYTHON_EXPORTS )
# SimpleTypedDataBinding.cpp is large!
IF( MSVC )
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
ENDIF()

FILE( GLOB H_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "../../include/IECorePython/*.h" "../../include/IECorePython/*.inl" )

FILE( GLOB CXX_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp" )

SET( SOURCE_FILES ${CXX_FILES} ${H_FILES} )

SET( THIS_LIBS
     IECore
     ${Boost_LIBRARIES}
     ${TBB_LIBRARIES}
     ${ILMBASE_LIBRARIES}
     ${OPENEXR_LIBRARIES}
     ${ZLIB_LIBRARIES} )

IF( NOT EXISTS "${Boost_INCLUDE_DIRS}/boost/asio.hpp" )
  LIST( REMOVE_ITEM CXX_FILES
        ClientDisplayDriverBinding.cpp
        DisplayDriverBinding.cpp
        ImageDisplayDriverBinding.cpp
        DisplayDriverServerBinding.cpp
        OBJReader.cpp )
ENDIF()

IF( NOT EXISTS "${Boost_INCLUDE_DIRS}/boost/math/special_functions/factorials.hpp" )
  LIST( REMOVE_ITEM CXX_FILES
        EnvMapSHProjectorBinding.cpp
        ImageConvolveOpBinding.cpp )
ENDIF()

IF( NOT EXISTS "${OPENEXR_INCLUDE_DIR}/OpenEXR/ImfDeepFrameBuffer.h" )
  LIST( REMOVE_ITEM CXX_FILES
        EXRDeepImageReaderBinding.cpp
        EXRDeepImageWriterBinding.cpp )
ENDIF()

IF( NOT TIFF_FOUND )
  LIST( REMOVE_ITEM CXX_FILES
        TIFFImageWriterBinding.cpp
        TIFFImageReaderBinding.cpp )
ENDIF()

IF( NOT JPEG_FOUND )
  LIST( REMOVE_ITEM CXX_FILES
        JPEGImageWriterBinding.cpp
        JPEGImageReaderBinding.cpp )
ENDIF()

IF( NOT PNG_FOUND )
  LIST( REMOVE_ITEM CXX_FILES
        PNGImageReaderBinding.cpp )
ENDIF()

IF( NOT FREETYPE_FOUND )
  LIST( REMOVE_ITEM CXX_FILES
        FontBinding.cpp )
ENDIF()

IF( BUILD_STATIC_LIBS )
  ADD_LIBRARY( IECorePython STATIC ${SOURCE_FILES} )
ELSE()
  ADD_LIBRARY( IECorePython SHARED ${SOURCE_FILES} )
ENDIF()

TARGET_LINK_LIBRARIES( IECorePython ${THIS_LIBS} )

INSTALL( TARGETS IECorePython
         DESTINATION lib )

INSTALL( FILES ${H_FILES}
         DESTINATION include/IECorePython )
