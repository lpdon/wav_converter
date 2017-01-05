cmake_minimum_required (VERSION 2.6)

if ( UNIX )
enable_testing()
endif ()

set ( CMAKE_C_COMPILER gcc )
set ( CMAKE_CXX_COMPILER g++ )
set ( CMAKE_BUILD_TYPE Release )

project ( CONVERTER )

add_subdirectory ( src )

if ( UNIX )
add_subdirectory ( test )
endif ()

#add_test (NAME ut COMMAND run_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test)