# Copyright (c) HBP 2014-2016 Daniel.Nachbaur@epfl.ch
#                             Stefan.Eilemann@epfl.ch
# Change this number when adding tests to force a CMake run: 5

if(NOT BOOST_FOUND)
  return()
endif()

set(TEST_LIBRARIES ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} ZeroEQ
  ${CMAKE_THREAD_LIBS_INIT})

set(EXCLUDE_FROM_TESTS)
if(TARGET ZeroEQHTTP)
  list(APPEND TEST_LIBRARIES ZeroEQHTTP ${CPPNETLIB_LIBRARIES})
else()
  list(APPEND EXCLUDE_FROM_TESTS http/server.cpp)
endif()

include(CommonCTest)
