cmake_minimum_required(VERSION 2.8)

project(pcap CXX)

include(FindPkgConfig)

# gtest
enable_testing()
find_package(GTest REQUIRED)

# dependencies libpcap
# include(FindPCAP.cmake)
find_library(PCAP_LIBRARY NAMES pcap)

# dependency DLT
IF (DEFINED DLT_ENABLED)
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
    ${CMAKE_SOURCE_DIR}/test/util
)

# link libraries
set(LIBRARIES
    ${PCAP_LIBRARY}
)
# dependency DLT
IF (DEFINED DLT_ENABLED)
    set(LIBRARIES
        ${LIBRARIES}
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

# pcap src
set(SRC_PCAP ${CMAKE_SOURCE_DIR}/src/pcap.cc)

# build examples
add_executable(example01
    ./example/example01.cc
    ${SRC_UTIL}
    ${SRC_PCAP}
)
target_link_libraries(example01 ${LIBRARIES})
set_target_properties(example01 PROPERTIES LINKER_LANGUAGE CXX)

add_executable(example02
    ./example/example02.cc
    ${SRC_UTIL}
    ${SRC_PCAP}
)
target_link_libraries(example02 ${LIBRARIES})
set_target_properties(example02 PROPERTIES LINKER_LANGUAGE CXX)

# build tests
add_executable(pcap_test
    ./test/pcap_test.cc
    ${SRC_PCAP}
    ${SRC_UTIL}
)
target_link_libraries(pcap_test GTest::GTest ${LIBRARIES})
set_target_properties(pcap_test PROPERTIES LINKER_LANGUAGE CXX)