#SRC

include_directories ( 
  ${CONVERTER_SOURCE_DIR}/include
  ${CONVERTER_SOURCE_DIR}/libs/lame-3.99.5/include 
  ${CONVERTER_SOURCE_DIR}/libs/googletest/googletest/include 
)

find_package (Threads)

set (
  lame_lib
  ${CONVERTER_SOURCE_DIR}/libs/lame-3.99.5/libmp3lame/libmp3lame.a
)

add_executable ( 
  converter  
  main.cpp
) 

set (
  converter_src_lib
  converter.cpp
  wav.c
)

add_library (
  converter_lib
  ${converter_src_lib}
)

target_link_libraries (
  converter_lib
  ${lame_lib}
  ${CMAKE_THREAD_LIBS_INIT}
) 

target_link_libraries ( 
  converter 
  converter_lib
 )