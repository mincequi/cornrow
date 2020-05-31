import QtQuick 2.9
import QtQuick.Controls 2.2

Item {
    id: parameter
    //property alias param: param.text
    //property alias unit: unit.text
    property string label
    property alias prefix: prefix.text
    property alias readout: readout.text
    property alias value: slider.value
    signal step(int i)
    width: parent.width
    height: param.height + slider.height
    //Behavior on enabled { NumberAnimation { duration: 200 } }
    //Behavior on opacity { NumberAnimation { duration: 200 } }

    Label {
        id: param
        text: label
        anchors.left: dec.horizontalCenter
        anchors.bottom: slider.top
    }
    Label {
        id: readout
        anchors.bottom: slider.top
        anchors.right: slider.right
    }

    Label {
        id: prefix
        anchors.right: slider.right
        anchors.rightMargin: 40
        anchors.bottom: slider.top
    }

    ToolButton {
        id: dec
        icon.source: "qrc:/icons/remove.svg"
        //text: "-"
        anchors.verticalCenter: slider.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 4
        onPressed: parameter.step(-1)
    }
    ToolButton {
        id: inc
        icon.source: "qrc:/icons/add.svg"
        //text: "+"
        anchors.verticalCenter: slider.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 4
        onPressed: parameter.step(1)
    }
    Slider {
        id: slider
        anchors.bottom: parent.bottom
        anchors.left: dec.right
        //anchors.leftMargin: 4
        anchors.right: inc.left
        //anchors.rightMargin: 4
        onValueChanged: parameter.valueChanged(value)
    }
}
