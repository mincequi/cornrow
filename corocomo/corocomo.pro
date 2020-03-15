QT += bluetooth quick svg
CONFIG += c++11
QMAKE_CXXFLAGS += -Wno-implicit-fallthrough

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(../cornrow.pri)

SOURCES += \
    src/BleCentralAdapter.cpp \
    src/BodePlotModel.cpp \
    src/BusyIndicatorModel.cpp \
    src/Config.cpp \
    src/DeviceModel.cpp \
    src/EqChart.cpp \
    src/FilterModel.cpp \
    src/IoModel.cpp \
    src/PhaseChart.cpp \
    src/Plot.cpp \
    src/PresetModel.cpp \
    src/SoftClipChart.cpp \
    src/main.cpp

HEADERS += \
    src/BleCentralAdapter.h \
    src/BodePlotModel.h \
    src/BusyIndicatorModel.h \
    src/Config.h \
    src/DeviceModel.h \
    src/EqChart.h \
    src/FilterModel.h \
    src/IoModel.h \
    src/PhaseChart.h \
    src/PresetModel.h \
    src/SoftClipChart.h

RESOURCES += qml.qrc

LIBS += -L$$OUT_PWD/../ble/ -lble -L$$OUT_PWD/../net/ -lnet
PRE_TARGETDEPS += $$OUT_PWD/../ble/libble.a $$OUT_PWD/../net/libnet.a

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

ios {
    QMAKE_INFO_PLIST = ios/Info.plist
    ios_icon.files = $$files($$PWD/ios/cornrow*.png)
    QMAKE_BUNDLE_DATA += ios_icon
}
