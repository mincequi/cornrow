add_library(audio STATIC
    Controller.cpp
    Converter.cpp
    FileDescriptorSource.cpp
    CoroPipeline.cpp
)

target_link_libraries(audio
    common
    libcornrow
    Qt5::Core
)
