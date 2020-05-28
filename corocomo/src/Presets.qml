import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3

import Cornrow.Configuration 1.0
import Cornrow.DeviceModel 1.0
import Cornrow.PresetModel 1.0

Item {
    height: io.height /*+ presets.height*/ + 12

    Io {
        id: io
        visible: CornrowConfiguration.ioAvailable
        anchors.top: deviceLabel.bottom
        anchors.topMargin: 12
        anchors.left: parent.left
        anchors.right: parent.right
    }
    
    /*
    ChipGroup {
        anchors.topMargin: 16
        anchors.top: io.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        id: presets
        name: "Presets"
        model: CornrowPresetModel.presetNames
    }
    */
} // Item
