add_library(audio STATIC
    Controller.cpp
    Converter.cpp
    CoroPipeline.cpp
)

target_link_libraries(audio
    common
    cornrow
    Qt5::Core
)
