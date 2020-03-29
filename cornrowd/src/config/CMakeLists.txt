add_library(config STATIC
    Controller.cpp
    Persistence.cpp
)

target_link_libraries(config
    audio
    bluetooth
    common
    net
    Qt5::Core
)
