cmake_minimum_required(VERSION 3.7)

project(cornrow)

include(ExternalProject)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} \
    -Wall \
    -Wno-psabi \
    -Wno-narrowing \
    -Wno-deprecated-declarations \
    -Wno-zero-as-null-pointer-constant")
set(CMAKE_CXX_STANDARD 17)
#    -Werror \

find_package(Qt5 REQUIRED COMPONENTS Core Network)

# Add thirdparty libs
add_subdirectory(thirdparty/libcornrow)
#add_subdirectory(thirdparty/qtzeroprops)

# Add internal libs
add_subdirectory(common)
add_subdirectory(cornrowd)
#add_subdirectory(net)
#add_subdirectory(bluebricated)

# Mandatory debian variables
SET(CPACK_GENERATOR "DEB")
SET(CPACK_PACKAGE_CONTACT   "mincequi@web.de")
SET(CPACK_PACKAGE_VERSION   "0.9.0")
SET(CPACK_PACKAGE_DESCRIPTION "Bluetooth audio player")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "This daemon accepts and plays audio from any Bluetooth source and turns your linux PC into a Bluetooth speaker.")
#CPACK_SYSTEM_NAME -> CMAKE_SYSTEM_NAME
##CPACK_DEBIAN_PACKAGE_NAME -> CPACK_PACKAGE_NAME -> project name
#CPACK_DEBIAN_FILE_NAME -> CPACK_PACKAGE_FILE_NAME -> ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}
#CPACK_DEBIAN_PACKAGE_VERSION -> CPACK_PACKAGE_VERSION
#CPACK_DEBIAN_PACKAGE_RELEASE
#CPACK_DEBIAN_PACKAGE_ARCHITECTURE
#CPACK_DEBIAN_PACKAGE_MAINTAINER -> CPACK_PACKAGE_CONTACT
#CPACK_DEBIAN_PACKAGE_DESCRIPTION -> CPACK_PACKAGE_DESCRIPTION_SUMMARY
#CPACK_DEBIAN_COMPRESSION_TYPE
#CPACK_DEBIAN_PACKAGE_PRIORITY

# DEBIAN OPTIONS
#SET(CPACK_DEBIAN_PACKAGE_MAINTAINER  "Manuel Weichselbaumer <mincequi@web.de>")
#SET(CPACK_DEBIAN_PACKAGE_SECTION     "Sound")
#SET(CPACK_DEBIAN_PACKAGE_DEPENDS     "${OSG_PACKAGE_DEPENDS}")
#SET(CPACK_DEBIAN_PACKAGE_CONFLICTS   "${OSG_PACKAGE_CONFLICTS}")
#SET(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${OpenSceneGraph_SOURCE_DIR}/debian/postinst;")
#SET(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${OpenSceneGraph_SOURCE_DIR}/debian/conffiles;")
