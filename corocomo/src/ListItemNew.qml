import QtQuick 2.10
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import QtQuick.Controls.impl 2.3
import QtQuick.Controls.Material 2.3

ItemDelegate {
    width: parent.width
    property alias primaryText: label.text
    
    Label {
        id: label
        anchors.left: parent.left
        anchors.leftMargin: icon.width+2*spacing
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width-anchors.leftMargin-rightButton.width
        elide: Text.ElideRight
    }
    
    ToolButton {
        id: rightButton
        anchors.right: parent.right
        icon.source: "qrc:/icons/chevron_right.svg"
        icon.color: Material.accent
    }
}
