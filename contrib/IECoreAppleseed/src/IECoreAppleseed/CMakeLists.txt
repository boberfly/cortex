ADD_SUBDIRECTORY( bindings )
ADD_SUBDIRECTORY( outputDriver )

FILE( GLOB H_FILES_PRIVATE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "../../include/IECoreAppleseed/private/*.h" "../../include/IECoreAppleseed/private/*.inl" )

FILE( GLOB H_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "../../include/IECoreAppleseed/*.h" "../../include/IECoreAppleseed/*.inl" )

FILE( GLOB CXX_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp" )

SET( SOURCE_FILES ${CXX_FILES} ${H_FILES} ${H_FILES_PRIVATE} )

SET( THIS_LIBS
     IECore
     ${CORTEX_APPLESEED_LIBS}
     ${Boost_LIBRARIES}
     ${TBB_LIBRARIES}
     ${Ilmbase_HALF_LIBRARY}
     ${Ilmbase_IEX_LIBRARY}
     ${Ilmbase_IEXMATH_LIBRARY}
     ${Ilmbase_ILMTHREAD_LIBRARY}
     ${Ilmbase_IMATH_LIBRARY}
     ${Openexr_ILMIMF_LIBRARY}
     ${ZLIB_LIBRARIES} )

IF( BUILD_STATIC_LIBS )
  ADD_LIBRARY( IECoreAppleseed STATIC ${SOURCE_FILES} )
ELSE()
  ADD_LIBRARY( IECoreAppleseed SHARED ${SOURCE_FILES} )
ENDIF()

TARGET_LINK_LIBRARIES( IECoreAppleseed ${THIS_LIBS} )

INSTALL( TARGETS IECoreAppleseed
         DESTINATION lib
         ARCHIVE DESTINATION lib )

INSTALL( FILES ${H_FILES}
         DESTINATION include/IECoreAppleseed )
