cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(qn_tools_tes)
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED)
list(APPEND CMAKE_PREFIX_PATH $ENV{QnTools})
find_package(QnTools REQUIRED)

include_directories(
        QnDrawer/include
        ${CMAKE_SOURCE_DIR}
        ${ROOT_INCLUDE_DIRS}
        ${Base_INCLUDE_DIR}
)

add_executable(test test.cc)
target_link_libraries(test QnTools::Base ${ROOT_LIBRARIES})