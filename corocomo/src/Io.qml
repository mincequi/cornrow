import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3

Column {
    anchors.centerIn: parent
    spacing: 16

    Column {
        spacing: 16

        TitleLabel {
            text: qsTr("Color Chips")
        }

        ButtonGroup {
            id: colorChipsGroup
        }

        Row {
            spacing: 8

            Chip {
                id: colorChip1
                text: qsTr("Blue 500")
                checkable: true
                ButtonGroup.group: colorChipsGroup
            }

            Chip {
                id: colorChip2
                text: qsTr("Green 500")
                checkable: true
                ButtonGroup.group: colorChipsGroup
            }

            Chip {
                id: colorChip3
                text: qsTr("Red 500")
                checkable: true
                ButtonGroup.group: colorChipsGroup
            }
        }
    }

    Column {
        spacing: 16

        TitleLabel {
            text: qsTr("Choice Chips")
        }

        ButtonGroup {
            id: choiceChipsGroup
        }

        Row {
            id: choiceChips
            spacing: 8

            Chip {
                checkable: true
                checked: true
                text: qsTr("Extra Soft")
                ButtonGroup.group: choiceChipsGroup
            }

            Chip {
                checkable: true
                text: qsTr("Soft")
                ButtonGroup.group: choiceChipsGroup
            }

            Chip {
                checkable: true
                text: qsTr("Medium")
                ButtonGroup.group: choiceChipsGroup
            }

            Chip {
                checkable: true
                text: qsTr("Hard")
                ButtonGroup.group: choiceChipsGroup
            }
        }
    }
}
