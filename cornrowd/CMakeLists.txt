project(cornrowd)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core DBus REQUIRED)

add_subdirectory(src/audio)
add_subdirectory(src/bluetooth)
add_subdirectory(src/config)
add_subdirectory(tests)

add_executable(${PROJECT_NAME}
    src/Controller.cpp
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME}
    audio
    bluetooth
    config
    qtzeroprops
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
