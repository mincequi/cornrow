import QtQuick 2.10
import QtQuick.Controls 2.3

Item {
    property alias name: label.text
    property alias model: repeater.model
    height: label.height + inputs.height

    Label {
        id: label
        x: 16
        anchors.bottomMargin: 12
    }

    ButtonGroup {
        id: chipGroup
    }

    Flow {
        id: inputs
        spacing: 8
        anchors.top: label.bottom
        anchors.topMargin: 12
        anchors.left: parent.left
        anchors.right: parent.right

        Repeater {
            id: repeater
            model: CornrowIoModel.inputNames
            Chip {
                checkable: true
                //checked: CornrowIoModel.activeInput == index
                text: modelData
                ButtonGroup.group: chipGroup
                //onPressed: CornrowIoModel.setActiveInput(index)
            }
        }
    }
} // Item
