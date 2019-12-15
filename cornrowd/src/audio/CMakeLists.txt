add_library(audio STATIC
    Controller.cpp
    Converter.cpp
    FileDescriptorSource.cpp
    CoroPipeline.cpp
)

target_link_libraries(audio
    common
    cornrow
    Qt5::Core
)
