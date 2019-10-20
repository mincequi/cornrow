import QtQuick 2.10
import QtQuick.Controls 2.3

import Cornrow.IoModel 1.0

Item {
    anchors.leftMargin: 12
    anchors.rightMargin: 12

    Label {
        id: inputLabel
        text: qsTr("Inputs")
        x: 16
        anchors.bottomMargin: 12
    }

    ButtonGroup {
        id: inputsGroup
    }

    Flow {
        id: inputs
        spacing: 8
        anchors.top: inputLabel.bottom
        anchors.topMargin: 12
        anchors.left: parent.left
        anchors.right: parent.right

        Repeater {
            model: CornrowIoModel.inputNames
            Chip {
                checkable: true
                text: CornrowIoModel.inputNames[index]
                ButtonGroup.group: inputsGroup
            }
        }
    }

    Label {
        id: outputLabel
        text: qsTr("Outputs")
        x: 16
        anchors.topMargin: 12
        anchors.top: inputs.bottom
    }

    ButtonGroup {
        id: outputsGroup
    }

    Flow {
        id: outputs
        spacing: 8
        anchors.top: outputLabel.bottom
        anchors.topMargin: 12
        anchors.left: parent.left
        anchors.right: parent.right

        Repeater {
            model: CornrowIoModel.outputNames
            Chip {
                checkable: true
                text: CornrowIoModel.outputNames[index]
                ButtonGroup.group: outputsGroup
            }
        }
    }
} // Item
