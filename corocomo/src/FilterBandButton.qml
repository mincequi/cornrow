import QtQuick 2.2
import QtQuick.Controls 2.0

TabButton {
    autoExclusive: true
    property alias indicatorColor: indicator.color
    property alias indicatorVisible: indicator.visible
    width: implicitWidth
    indicator: Rectangle {
        id: indicator
        width: parent.height*0.15
        height: width
        anchors.right: parent.right
        anchors.rightMargin: width
        anchors.top: parent.top
        anchors.topMargin: width*1.5
        radius: width*0.5
    }
}
