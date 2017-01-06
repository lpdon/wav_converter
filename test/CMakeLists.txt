#TEST

include_directories (
  ${CONVERTER_SOURCE_DIR}/include
  ${CONVERTER_SOURCE_DIR}/libs/googletest/googletest/include 
)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test.dbc
#    		   ${CMAKE_CURRENT_BINARY_DIR}/test.dbc COPYONLY)
    		   
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test.dbc
#    		   ${CMAKE_CURRENT_BINARY_DIR}/../test.dbc COPYONLY)

set (CMAKE_BUILD_TYPE Debug)
add_definitions( -Wall )

set (
  converter_test
  test.cpp
)

add_executable (run_test ${converter_test})
target_link_libraries (run_test converter_lib gtest gtest_main )