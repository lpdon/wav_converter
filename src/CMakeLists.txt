#SRC

include_directories ( ${CONVERTER_SOURCE_DIR}/include )
include_directories ( ${CONVERTER_SOURCE_DIR}/libs/lame-3.99.5/include )

find_package (Threads)

set (
  converter_src  
  audio_converter.cpp
  converter.cpp
  wav.c
)

set (
  lame_lib
  ${CONVERTER_SOURCE_DIR}/libs/lame-3.99.5/libmp3lame/libmp3lame.a
)

add_library ( 
  converter_src_lib 
  STATIC 
  IMPORTED
  ${CMAKE_THREAD_LIBS_INIT}
)

add_executable ( converter ${converter_src} ) 

target_link_libraries ( 
  converter 
  ${converter_src_lib}
  ${lame_lib}
  ${CMAKE_THREAD_LIBS_INIT}
 )