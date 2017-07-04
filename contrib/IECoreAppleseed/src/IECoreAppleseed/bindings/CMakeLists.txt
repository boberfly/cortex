FILE( GLOB H_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "../../../include/IECoreAppleseed/bindings/*.h" )

FILE( GLOB CXX_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp" )

FILE( GLOB PY_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "../../../python/IECoreAppleseed/*.py" )

SET( SOURCE_FILES ${CXX_FILES} ${H_FILES} )

SET( THIS_LIBS
     IECore
     IECorePython
     IECoreAppleseed
     ${Boost_LIBRARIES}
     ${Ilmbase_HALF_LIBRARY}
     ${Ilmbase_IEX_LIBRARY}
     ${Ilmbase_IEXMATH_LIBRARY}
     ${Ilmbase_ILMTHREAD_LIBRARY}
     ${Ilmbase_IMATH_LIBRARY}
     ${Openexr_ILMIMF_LIBRARY}
     ${TBB_LIBRARIES}
     ${ZLIB_LIBRARIES} )

ADD_LIBRARY( _IECoreAppleseed MODULE ${SOURCE_FILES} )

IF ( WINDOWS )
  SET_TARGET_PROPERTIES( _IECoreAppleseed PROPERTIES SUFFIX ".pyd" )
ELSE ()
  SET_TARGET_PROPERTIES( _IECoreAppleseed PROPERTIES PREFIX "" )
ENDIF ()

TARGET_LINK_LIBRARIES( _IECoreAppleseed ${THIS_LIBS} )

INSTALL( TARGETS _IECoreAppleseed
         LIBRARY DESTINATION python/IECoreAppleseed )

INSTALL( FILES ${PY_FILES}
         DESTINATION python/IECoreAppleseed )
