macro(ble_tests)
    foreach(_testname ${ARGN})
        add_executable(${_testname} ${_testname}.cpp)
        target_link_libraries(${_testname} ble)
    endforeach(_testname)
endmacro()

ble_tests(
    centraltest
    peripheraltest
)
