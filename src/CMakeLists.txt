#SRC

include_directories ( ${CONVERTER_SOURCE_DIR}/include )
include_directories ( ${CONVERTER_SOURCE_DIR}/libs/lame-3.99.5/include )

find_package (Threads)

set (
  converter_src  
  main.cpp
  converter.cpp
  wav.c
)

if (WIN32)
set (
  lame_lib
  ${CONVERTER_SOURCE_DIR}/libs/lame-3.99.5/libmp3lame/libmp3lame.lib
)
else ()
set (
  lame_lib
  ${CONVERTER_SOURCE_DIR}/libs/lame-3.99.5/libmp3lame/libmp3lame.a
)
endif ()

add_executable ( converter ${converter_src} ) 

target_link_libraries ( 
  converter 
  ${converter_src_lib}
  ${lame_lib}
  ${CMAKE_THREAD_LIBS_INIT}
 )