PROJECT(main)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
AUX_SOURCE_DIRECTORY(. DIR_SRCS)
ADD_EXECUTABLE(main ${DIR_SRCS})

SET(CMAKE_C_COMPILER g++)
SET(CMAKE_CXX_STANDARD 11)

#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -Wall -g -ggdb ")

#SET(CMAKE_CXX_FLAGS_RELEASE "${ENV{CXXFLAGS}  -Wall")



find_package(Boost REQUIRED COMPONENTS system thread)

target_link_libraries(main ${Boost_LIBRARIES})
