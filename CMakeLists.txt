cmake_minimum_required (VERSION 2.6)

if ( UNIX )
enable_testing()
endif ()

set ( CMAKE_C_COMPILER gcc )
set ( CMAKE_CXX_COMPILER g++ )
set ( CMAKE_BUILD_TYPE Release )

project ( CONVERTER )

add_subdirectory ( src )

configure_file(${CONVERTER_SOURCE_DIR}/libs/lame-3.99.5/Makefile.unix
    		   ${CONVERTER_SOURCE_DIR}/libs/lame-3.99.5/Makefile COPYONLY)

add_custom_target(
  build_lame ALL
  COMMAND ${CMAKE_MAKE_PROGRAM}
  WORKING_DIRECTORY ${CONVERTER_SOURCE_DIR}/libs/lame-3.99.5/
  COMMENT "Build LAME"
)

if ( UNIX )
add_subdirectory ( ${CONVERTER_SOURCE_DIR}/libs/googletest/googletest/ )
add_subdirectory ( test )
add_test (NAME ut COMMAND run_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test)
endif ()