import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3

//import Cornrow.PresetsModel 1.0

Item {
    height: io.height + 36

    Io {
        id: io
        anchors.left: parent.left
        anchors.right: parent.right
    }

    /*
    ChipGroup {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        id: presets
        name: "Presets"
        model: presetsModel
    }

    Io {
        anchors.top: presets.bottom
        anchors.left: parent.left
        anchors.right: parent.right
    }
    */


    ListModel {
        id: presetsModel
        ListElement {
            name: "Preset 1"
        }
        ListElement {
            name: "Wohnzimmer"
        }
        ListElement {
            name: "Esszimmer"
        }
        ListElement {
            name: "Fullrange"
        }
        ListElement {
            name: "Preset 5"
        }
        ListElement {
            name: "Two way"
        }
        ListElement {
            //     123456789012345678901
            name: "Max Twentyone chars!!"
        }
        ListElement {
            name: "These are even more..."
        }
        ListElement {
            name: "Stereo and LFE"
        }
        ListElement {
            name: "Preset 10"
        }
        ListElement {
            name: "Keller"
        }
        ListElement {
            name: "Partyraum"
        }
        ListElement {
            name: "Wohnzimmer"
        }
        ListElement {
            name: "Esszimmer"
        }
        ListElement {
            name: "Preset 15"
        }
        ListElement {
            name: "The last preset"
        }
    }

    /*
    ListView {
        id: listView
        anchors.leftMargin: 12
        width: parent.width
        height: 60
        model: presetsModel
        delegate: Text {
            text: name
        }
        highlight: Rectangle { color: Material.accent }
        focus: true
    }
    */
} // Item
