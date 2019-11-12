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

    icon.width: 18
    icon.height: 18

    hoverEnabled: true

    Material.elevation: control.pressed ? 2 : 0

    background: Rectangle {
        implicitHeight: 24
        radius: 12
        color: control.checked || control.hovered ? Material.accent : Material.color(Material.Grey)

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
            //opacity: 0.87
        }
    }
}
