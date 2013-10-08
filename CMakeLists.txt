project(fantasy-core)
cmake_minimum_required(VERSION 2.6)

#Find Pthreads and gtk3
find_package (Threads)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)


#include gtk3 liraries
include_directories(${GTK3_INCLUDE_DIRS} include)
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

#set the binary output location to bin
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

#Build the game
add_executable(fantasy-core 	src/fantasIA.c   
				src/fantasy-gtk.c  
				src/fantasy-lib.c  
				src/fantasy-loadsave.c  
				src/fantasy-main.c)

#Link the libraries
target_link_libraries(fantasy-core ${GTK3_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}) 

