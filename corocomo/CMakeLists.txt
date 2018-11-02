find_package(Qt5 COMPONENTS Quick QuickCompiler REQUIRED)

qtquick_compiler_add_resources(RESOURCES
    src/qml.qrc
)

add_executable(corocomo
    src/BleCentralAdapter.cpp
    src/EqChart.cpp
    src/Model.cpp
    src/main.cpp
    ${RESOURCES}
)

target_link_libraries(corocomo
    ble
)

qt5_use_modules(corocomo
    Quick
)
