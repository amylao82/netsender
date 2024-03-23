cmake_minimum_required(VERSION 3.5)

set(PROJ_NAME netsender)
project(${PROJ_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB LIBNETSENDER_INC
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
	)

file(GLOB LIBNETSENDER_SRC
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
	)

if(WIN32)
    add_definitions(-DPLATFORM_WINDOWS)
endif()

ADD_LIBRARY(${PROJ_NAME} STATIC ${LIBNETSENDER_INC} ${LIBNETSENDER_SRC})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

