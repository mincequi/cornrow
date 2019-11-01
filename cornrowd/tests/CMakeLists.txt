add_executable(fdtest
    fdtest.cpp
)

target_link_libraries(fdtest
    gstreamermm-dsp
    pthread
    Qt5::Core
)

macro(coro_tests)
    foreach(_testname ${ARGN})
        add_executable(${_testname} ${_testname}.cpp)
        target_link_libraries(${_testname} gstreamermm-dsp)
    endforeach(_testname)
endmacro()

coro_tests(
    bletest
)
