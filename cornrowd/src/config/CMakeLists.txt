add_library(config STATIC
    ConfigManager.cpp
    Persistence.cpp
)

target_link_libraries(config
    audio
    bluetooth
    common
    #qtzeroprops
)
