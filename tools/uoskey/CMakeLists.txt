cmake_minimum_required(VERSION 3.5)

PROJECT(uoskey)

add_definitions(-std=c++14)

set(CMAKE_CXX_FLAGS_DEBUG   "-O0" )             # 调试包不优化
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG " )   # release包优化

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")




#MESSAGE("PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")

#set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_BUILD_TYPE Release)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
IF(CMAKE_BUILD_TYPE MATCHES Debug)
  set(CMAKE_CXX_FLAGS_DEBUG "-O0")
  message("Debug build.")
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
  set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")
  message("Release build.")
ELSE()
  message("Some other build type.")
ENDIF()


include_directories(${PROJECT_SOURCE_DIR})
include_directories(/usr/local/boost/include)

MESSAGE("project dir: ${PROJECT_SOURCE_DIR}")
MESSAGE("cmake_current: ${CMAKE_CURRENT_SOURCE_DIR}")


INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/fc/include)
add_subdirectory( fc )

link_directories( /usr/local/boost/lib)
ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK)


set(CXX_FLAGS
 -g
 -DCHECK_PTHREAD_RETURN_VALUE
 -D_FILE_OFFSET_BITS=64
 -Wall
 -Wextra
 -Wsign-compare
 -Wunused-variable
 -Wno-unused-parameter
 -Woverloaded-virtual
 -Wpointer-arith
 -Wwrite-strings
 -march=native
 -std=c++11
 -lrt 
 -rdynamic
 -pthread
 -lssl
 -lcrypto 
 -lboost_program_options
 -lboost_thread
 -lboost_chrono
 -lboost_date_time
 -lboost_timer
 -lboost_system
 -lboost_filesystem
 )

 if(CMAKE_BUILD_BITS EQUAL 32)
 list(APPEND CXX_FLAGS "-m32")
endif()

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")


set(IMPORTLIB  
   boost_log_setup
   boost_log
   boost_thread
   boost_system
   boost_filesystem
   boost_chrono
   boost_date_time
)



add_executable(uoskey  main.cpp)
target_link_libraries (uoskey  fc)
target_link_libraries(uoskey ${IMPORTLIB})

