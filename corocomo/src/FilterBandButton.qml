import QtQuick 2.2
import QtQuick.Controls 2.0

ToolButton {
    autoExclusive: true
    property alias indicatorColor: indicator.color
    property alias indicatorVisible: indicator.visible
    indicator: Rectangle {
        id: indicator
        width: parent.height*0.20
        height: width
        anchors.right: parent.right
        anchors.rightMargin: width
        anchors.top: parent.top
        anchors.topMargin: width
        radius: width*0.5
    }
}
