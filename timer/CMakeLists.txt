cmake_minimum_required(VERSION 2.8)

project(timer CXX)

include(FindPkgConfig)

# gtest
enable_testing()
find_package(GTest REQUIRED)

# dependency DLT
IF(${DLT_ENABLED} MATCHES "TRUE")
    ADD_DEFINITIONS(-DDLT_ENABLED)
    pkg_check_modules(DLT REQUIRED automotive-dlt)
    include_directories(${DLT_INCLUDE_DIRS})
    link_directories(${DLT_LIBRARY_DIRS})
ENDIF()

# include headers
include_directories(
    ${GTEST_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/util
)

# link libraries
find_library(PTHREAD_LIBRARY NAMES pthread)
set(LIBRARIES
    ${PTHREAD_LIBRARY}
)

# dependency DLT
IF(${DLT_ENABLED} MATCHES "TRUE")
    set(LIBRARIES
        ${DLT_LDFLAGS}
    )
ENDIF()

# compiler options depending on build type
IF (${CMAKE_BUILD_TYPE} MATCHES "[Rr]elease")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++1z -O0")
ELSE()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -std=c++1z -O0")
    ADD_DEFINITIONS(-DLOG_ENABLED)
ENDIF()

# util src
set(SRC_UTIL ${CMAKE_SOURCE_DIR}/util/logger.cc)

# build examples
add_executable(example01
    ./example/example01.cc
    ${SRC_UTIL}
)
target_link_libraries(example01 ${LIBRARIES})
set_target_properties(example01 PROPERTIES LINKER_LANGUAGE CXX)

add_executable(example02
    ./example/example02.cc
    ${SRC_UTIL}
)
target_link_libraries(example02 ${LIBRARIES})
set_target_properties(example02 PROPERTIES LINKER_LANGUAGE CXX)

add_executable(example03
    ./example/example03.cc
    ${SRC_UTIL}
)
target_link_libraries(example03 ${LIBRARIES})
set_target_properties(example03 PROPERTIES LINKER_LANGUAGE CXX)

add_executable(example04
    ./example/example04.cc
    ${SRC_UTIL}
)
target_link_libraries(example04 ${LIBRARIES})
set_target_properties(example04 PROPERTIES LINKER_LANGUAGE CXX)

# build tests
add_executable(timer_test
    ./test/timer_test.cc
    ${SRC_UTIL}
)
target_link_libraries(timer_test GTest::GTest ${LIBRARIES})
set_target_properties(timer_test PROPERTIES LINKER_LANGUAGE CXX)