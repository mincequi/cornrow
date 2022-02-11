set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLOGURU_WITH_STREAMS")

add_library(audio STATIC
    AudioManager.cpp
    Converter.cpp
    CoroPipeline.cpp
)

target_link_libraries(audio
PUBLIC
    common
    cornrow
    Qt5::Core
)
