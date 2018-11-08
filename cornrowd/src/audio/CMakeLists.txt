find_package(Qt5GStreamer REQUIRED)

add_library(audio STATIC
    Controller.cpp
    Pipeline.cpp
)

target_link_libraries(audio
    common
    gstreamermm-dsp
    Qt5::Core
    Qt5GStreamer
)
