cmake_minimum_required(VERSION 3.7)

project(cornrow)

include(cmake/CPM.cmake)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} \
    -Wall \
    -Wno-psabi \
    -Wno-narrowing \
    -Wno-deprecated-declarations \
    -Wno-zero-as-null-pointer-constant")
set(CMAKE_CXX_STANDARD 17)

find_package(Qt5 REQUIRED COMPONENTS Core Network)

# Add internal libs
add_subdirectory(common)
add_subdirectory(cornrowd)

install(TARGETS cornrowd DESTINATION bin)
install(FILES ${PROJECT_SOURCE_DIR}/debian/cornrowd.service DESTINATION /usr/lib/systemd/system)

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Manuel Weichselbaumer <mincequi@web.de>") #required
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_DEPENDS adduser)
SET(CPACK_PACKAGE_DESCRIPTION "Bluetooth audio player")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "This daemon accepts and plays audio from any Bluetooth source and turns your linux PC into a Bluetooth speaker.")
SET(CPACK_PACKAGE_VERSION   "1.0.0")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${PROJECT_SOURCE_DIR}/debian/postinst;${PROJECT_SOURCE_DIR}/debian/postrm;${PROJECT_SOURCE_DIR}/debian/install")
include(CPack)
