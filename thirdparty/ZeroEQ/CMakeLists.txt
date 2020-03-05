# Copyright (c) HBP 2014-2017 Daniel.Nachbaur@epfl.ch
#                             Stefan.Eilemann@epfl.ch

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(ZeroEQ VERSION 0.9.0)
set(ZeroEQ_VERSION_ABI 9)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/common)
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/CMake/common/Common.cmake)
  message(FATAL_ERROR "CMake/common missing, run: git submodule update --init")
endif()

set(ZEROEQ_DEB_DEPENDS
  libboost-test-dev
  libzmq3-dev
  # cppnetlib dependencies
  libboost-atomic-dev
  libboost-chrono-dev
  libboost-date-time-dev
  libboost-filesystem-dev
  libboost-program-options-dev
  libboost-regex-dev
  libboost-system-dev
  libboost-thread-dev
  libssl-dev
)

include(Common)

set(COMMON_PROJECT_DOMAIN eu.humanbrainproject)
set(ZEROEQ_DESCRIPTION "ZeroEQ - Zero Event Queue")
set(ZEROEQ_MAINTAINER "Human Brain Project <HBPVis@googlegroups.com>")
set(ZEROEQ_LICENSE LGPL)
set(ZEROEQ_DEPENDENT_LIBRARIES Servus)

if(NOT MSVC)
  option(CPP-NETLIB_BUILD_SHARED_LIBS "Build cpp-netlib as shared libraries." ON)
endif()
option(CPP-NETLIB_BUILD_TESTS "Build the cpp-netlib project tests." OFF)
option(CPP-NETLIB_BUILD_EXAMPLES "Build the cpp-netlib project examples." OFF)

common_find_package(Boost SYSTEM COMPONENTS unit_test_framework)
if(Boost_VERSION GREATER 105799)
  add_subproject(cppnetlib ${PROJECT_SOURCE_DIR}/cppnetlib) # from .gitmodules
  common_find_package(cppnetlib SYSTEM NO_DEFAULT_PATH)
endif()
if(TARGET libzmq)
  set(ZeroMQ_LIBRARY libzmq)
else()
  common_find_package(ZeroMQ) # new-ish CMake-based build
  if(NOT ZeroMQ_FOUND)
    common_find_package(libzmq 4 REQUIRED) # old-ish autopain/pkgconfig build
    set(ZeroMQ_LIBRARY ${libzmq_LIBRARIES})
  endif()
endif()
common_find_package(Servus REQUIRED)
common_find_package(Threads REQUIRED)
common_find_package_post()

set(LCOV_EXCLUDE "zeroeq/http/jsoncpp/*;cppnetlib/boost/network/*;cppnetlib/libs/network/src/uri/*")

add_subdirectory(zeroeq)
add_subdirectory(tests)

set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set(ZEROEQ_PACKAGE_DEB_DEPENDS ${ZEROEQ_DEB_DEPENDS})
include(CommonCPack)

set(DOXYGEN_MAINPAGE_MD README.md)
set(DOXYGEN_EXTRA_INPUT ${PROJECT_SOURCE_DIR}/README.md)
include(DoxygenRule)
