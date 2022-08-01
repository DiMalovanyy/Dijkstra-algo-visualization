cmake_minimum_required(VERSION 3.5)
project(ShortPath LANGUAGES C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

set(target_name shortPath)
find_package(wxWidgets REQUIRED)
if(NOT ${wxWidgets_FOUND})
	message(FATAL_ERROR "Could not found wxWidgets GUI lib")
else()
	message(STATUS "wxWidgets found ${wxWidgets_VERSION}")
	include(${wxWidgets_USE_FILE})
endif()

find_package(Boost REQUIRED)
if(NOT ${Boost_FOUND})
	message(FATAL_ERROR "Could not found Boost library")
else()
	message(STATUS "Boost found ${Boost_VERSION}")
	include_directories(${Boost_INCLUDE_DIR})
endif()

set(UTILITY
	utility/utility.cpp)

set(COMPUTE
	computation/graph.cpp)

set(GUI 
	gui/application.cpp
	gui/mainFrame.cpp
	gui/pathGrid.cpp
	gui/GraphThread.cpp)

add_executable(${target_name} ${GUI} ${COMPUTE} ${UTILITY})

target_link_libraries(${target_name} ${wxWidgets_LIBRARIES})
target_link_libraries(${target_name} ${Boost_LIBRARIES})

target_include_directories(${target_name} PUBLIC ${CMAKE_SOURCE_DIR}/computation)
target_include_directories(${target_name} PUBLIC ${CMAKE_SOURCE_DIR}/utility)
