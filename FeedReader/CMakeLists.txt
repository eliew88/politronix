cmake_minimum_required(VERSION 3.5)
project(FeedReader)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(twitcurl)
include_directories("../twitcurl/libtwitcurl")
# TODO(eli): We're supposed to do the following but can't make it work. fix.
# include_directories(${twitcurl_INCLUDE_DIRS})


# if (twitcurl_FOUND)
#     message(STATUS "found")
# endif()

set(SOURCE_FILES main.cpp)
add_executable(FeedReader ${SOURCE_FILES})

target_link_libraries(FeedReader ${twitcurl_LIBRARIES})
