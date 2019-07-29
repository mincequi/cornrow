macro(ble_tests)
    foreach(_testname ${ARGN})
        add_executable(${_testname} ${_testname}.cpp)
        target_link_libraries(${_testname} ble)
    endforeach(_testname)
endmacro()

ble_tests(
    centraltest
    #peripheraltest
)

find_package(Qt5 CONFIG REQUIRED Core DBus)
add_executable(peripheraltest peripheraltest.cpp)
target_link_libraries(peripheraltest
    ble
    KF5BluezQt
    Qt5::DBus
)
