macro(bluetooth_tests)
    foreach(_testname ${ARGN})
        add_executable(${_testname} ${_testname}.cpp)
        target_link_libraries(${_testname} bluetooth)
    endforeach(_testname)
endmacro()

bluetooth_tests(
    qtperipheraltest
)
