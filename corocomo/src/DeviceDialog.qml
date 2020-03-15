// Import this one first to not override other RadialGradient
import QtGraphicalEffects 1.14

import QtQuick 2.14
import QtQuick.Controls 2.2
import QtQuick.Controls.impl 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3

import Qt.labs.qmlmodels 1.0

import Cornrow.DeviceType 1.0
import Cornrow.DeviceModel 1.0
import Cornrow.IoModel 1.0
import Cornrow.FilterModel 1.0
import Cornrow.PresetModel 1.0

Dialog {
    id: myDialog
    visible: true
    background: Rectangle { color: "transparent" }
    anchors.centerIn: parent

    Component.onCompleted: {
        DeviceModel.startDiscovering()
        FilterModel.startDiscovering()
    }

    Column {
        id: statusScreen
        spacing: 16
        anchors.centerIn: parent
        width: myDialog.width
        CoroBusyIndicator {
            CoroBusyIndicator {
            strokeColor: Material.color(Material.Pink)
            fillColor: "transparent"
            innerRadius: 20
            outerRadius: 52
            strokeWidth: 1
            inactiveOpacity: 0.5
            }
        }
        
        Label {
            id: statusLabel
            text: DeviceModel.statusLabel;
            font.capitalization: Font.SmallCaps
            font.pixelSize: 20
            font.weight: Font.DemiBold
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            id: statusText
            horizontalAlignment: Text.AlignHCenter
            maximumLineCount: 2
            text: DeviceModel.statusText;
            width: 240
            height: 48
            clip: true
            wrapMode: Text.Wrap
            font.pixelSize: 16
            font.weight: Font.Light
        }

        ListView {
            clip: true
            width: parent.width
            height: 120
            model: DeviceModel.devices
            delegate: ListItemNew {
                icon.source: modelData.type === CornrowDeviceType.BluetoothLe ? "qrc:/icons/bluetooth.svg" : "qrc:/icons/wifi.svg"
                primaryText: modelData.name
                onClicked: modelData.onClicked
                //subText: ip
                //subTextFontSize: 12
                //showDivider: true
                /*
                rightItem: IconLabel {
                    icon.source: "qrc:/icons/chevron_right.svg"
                    icon.color: Material.foreground 
                    anchors.centerIn: rightItem
                }
                */
            }
        }
    } // Column

    footer: DialogButtonBox {
        opacity: DeviceModel.status != DeviceModel.Discovering &&
                 DeviceModel.status != DeviceModel.Connected

        Button {
            text: DeviceModel.status === DeviceModel.Connecting ? "Abort and rescan" : "Rescan"
            flat: true
            onPressed: {
                DeviceModel.startDiscovering()
                FilterModel.startDiscovering()
            }
        }
        Button {
            text: "Demo"
            flat: true
            visible: DeviceModel.status !== DeviceModel.Connecting
            onPressed: {
                FilterModel.startDemo()
                DeviceModel.startDemo()
                CornrowIoModel.startDemo()
                CornrowPresetModel.startDemo()
            }
        }
    }

    closePolicy: Popup.NoAutoClose
}
