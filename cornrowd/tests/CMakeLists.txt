add_executable(fdtest
    fdtest.cpp
)

target_link_libraries(fdtest
    gstreamermm-dsp
    pthread
    Qt5::Core
)
