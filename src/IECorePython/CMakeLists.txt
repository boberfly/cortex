FILE( GLOB H_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "../../include/IECorePython/*.h" "../../include/IECorePython/*.inl" )

FILE( GLOB CXX_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp" )

SET( SOURCE_FILES ${CXX_FILES} ${H_FILES} )

SET( THIS_LIBS
     IECore
     ${Boost_SIGNALS_LIBRARY}
     ${Boost_IOSTREAMS_LIBRARY}
     ${Boost_FILESYSTEM_LIBRARY}
     ${Boost_DATE_TIME_LIBRARY}
     ${Boost_THREAD_LIBRARY}
     ${Boost_WAVE_LIBRARY}
     ${Boost_REGEX_LIBRARY}
     ${Boost_SYSTEM_LIBRARY}
     ${Boost_CHRONO_LIBRARY}
     ${Boost_PYTHON_LIBRARY}
     ${TBB_LIBRARIES}
     ${Ilmbase_HALF_LIBRARY}
     ${Ilmbase_IEX_LIBRARY}
     ${Ilmbase_IEXMATH_LIBRARY}
     ${Ilmbase_ILMTHREAD_LIBRARY}
     ${Ilmbase_IMATH_LIBRARY}
     ${Openexr_ILMIMF_LIBRARY}
     ${ZLIB_LIBRARIES} )

#IF( NOT EXISTS "${Boost_INCLUDE_DIRS}/boost/asio.hpp" )
#  LIST( REMOVE_ITEM CXX_FILES
#        ClientDisplayDriverBinding.cpp
#        DisplayDriverBinding.cpp
#        ImageDisplayDriverBinding.cpp
#        DisplayDriverServerBinding.cpp
#        OBJReader.cpp )
#ENDIF()

#IF( NOT EXISTS "${Boost_INCLUDE_DIRS}/boost/math/special_functions/factorials.hpp" )
#  LIST( REMOVE_ITEM CXX_FILES
#        EnvMapSHProjectorBinding.cpp
#        ImageConvolveOpBinding.cpp )
#ENDIF()

#IF( NOT EXISTS "${OPENEXR_INCLUDE_DIR}/OpenEXR/ImfDeepFrameBuffer.h" )
#  LIST( REMOVE_ITEM CXX_FILES
#        EXRDeepImageReaderBinding.cpp
#        EXRDeepImageWriterBinding.cpp )
#ENDIF()

#IF( NOT TIFF_FOUND )
#  LIST( REMOVE_ITEM CXX_FILES
#        TIFFImageWriterBinding.cpp
#        TIFFImageReaderBinding.cpp )
#ENDIF()

#IF( NOT JPEG_FOUND )
#  LIST( REMOVE_ITEM CXX_FILES
#        JPEGImageWriterBinding.cpp
#        JPEGImageReaderBinding.cpp )
#ENDIF()

#IF( NOT PNG_FOUND )
#  LIST( REMOVE_ITEM CXX_FILES
#        PNGImageReaderBinding.cpp )
#ENDIF()

#IF( NOT FREETYPE_FOUND )
#  LIST( REMOVE_ITEM CXX_FILES
#        FontBinding.cpp )
#ENDIF()

IF( BUILD_STATIC_LIBS )
  ADD_LIBRARY( IECorePython STATIC ${SOURCE_FILES} )
ELSE()
  ADD_LIBRARY( IECorePython SHARED ${SOURCE_FILES} )
ENDIF()

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif()

TARGET_LINK_LIBRARIES( IECorePython ${THIS_LIBS} )

INSTALL( TARGETS IECorePython
         DESTINATION lib
         ARCHIVE DESTINATION lib )

INSTALL( FILES ${H_FILES}
         DESTINATION include/IECorePython )
