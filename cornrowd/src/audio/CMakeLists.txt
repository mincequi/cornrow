add_library(audio STATIC
    Controller.cpp
    Converter.cpp
    FileDescriptorSource.cpp
    Pipeline.cpp
    CoroPipeline.cpp
)

target_link_libraries(audio
    common
    gstreamermm-dsp
    Qt5::Core
)
