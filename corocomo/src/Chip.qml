import QtQuick 2.10
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3

AbstractButton {
    id: control

    implicitWidth: Math.max(background ? background.implicitWidth : 0, contentItem.implicitWidth) + leftPadding + rightPadding
    implicitHeight: Math.max(background ? background.implicitHeight : 0, contentItem.implicitHeight) + topPadding + bottomPadding

    font.pixelSize: 13

    leftPadding: 12
    rightPadding: 12
    spacing: 8

    icon.width: 24
    icon.height: 24

    hoverEnabled: true

    Material.elevation: control.pressed ? 2 : 0
    Material.background: Material.color(Material.Grey, control.checked || control.hovered ? Material.Shade700 : Material.Shade300)

    background: Rectangle {
        implicitHeight: 32
        radius: 16
        color: control.Material.backgroundColor

        layer.enabled: control.Material.elevation > 0
    }

    contentItem: RowLayout {
        spacing: control.spacing

        Material.theme: control.hovered ? Material.Dark : Material.Light

        Label {
            id: bodyLabel

            Layout.alignment: Qt.AlignVCenter

            text: control.text
            font: control.font
            color: control.checked || control.hovered ? Material.primaryHighlightedTextColor : Material.primaryTextColor
            opacity: 0.87
        }
    }
}
