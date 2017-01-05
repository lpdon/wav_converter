cmake_minimum_required (VERSION 2.6)

#enable_testing()

#set ( CMAKE_C_COMPILER gcc )
#set ( CMAKE_CXX_COMPILER g++ )
#set(CMAKE_CXX_FLAGS "-std=c++11 -g3")
set ( CMAKE_BUILD_TYPE Debug )
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

add_definitions( -std=c++11 )

project ( CONVERTER )

add_subdirectory ( src )
#add_subdirectory ( test )

#add_test (NAME ut COMMAND run_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test)