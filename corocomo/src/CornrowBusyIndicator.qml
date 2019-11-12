import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3
import QtQuick.Shapes 1.11

import Cornrow.BusyIndicatorModel 1.0
import Cornrow.Model 1.0

Item {
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom

    CornrowBusyIndicatorModel {
        id: model
        active: CornrowModel.status != CornrowModel.Connected
        radius: (CornrowModel.status == CornrowModel.Discovering ||
                 CornrowModel.status == CornrowModel.Connecting) ? 48 : 36
        numPoints: 11
        Behavior on radius { SmoothedAnimation { velocity: 1000 }}
    }

    Item {
        id: statusScreen
        anchors.centerIn: parent
        Canvas {
            id: busyIndicator
            width: 120; height: 120
            contextType: "2d"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.verticalCenter
            anchors.bottomMargin: 48

            Shape {
                ShapePath {
                    id: myPath
                    strokeColor: "transparent"

                    startX: model.xCoords[0]; startY: model.yCoords[0]
                    Behavior on startX { SmoothedAnimation { velocity: 10; duration: 5000 }}
                    Behavior on startY { SmoothedAnimation { velocity: 10; duration: 5000 }}

                    // @TODO(Qt): Repeater does not work here (and probably never will. We might move this to C++).
                    PathCurve { x: model.xCoords[1]; y: model.yCoords[1]
                        Behavior on x { SmoothedAnimation { velocity: 10; duration: 5000 }}
                        Behavior on y { SmoothedAnimation { velocity: 10; duration: 5000 }}
                    }
                    PathCurve { x: model.xCoords[2]; y: model.yCoords[2]
                        Behavior on x { SmoothedAnimation { velocity: 10; duration: 5000 }}
                        Behavior on y { SmoothedAnimation { velocity: 10; duration: 5000 }}
                    }
                    PathCurve { x: model.xCoords[3]; y: model.yCoords[3]
                        Behavior on x { SmoothedAnimation { velocity: 10; duration: 5000 }}
                        Behavior on y { SmoothedAnimation { velocity: 10; duration: 5000 }}
                    }
                    PathCurve { x: model.xCoords[4]; y: model.yCoords[4]
                        Behavior on x { SmoothedAnimation { velocity: 10; duration: 5000 }}
                        Behavior on y { SmoothedAnimation { velocity: 10; duration: 5000 }}
                    }
                    PathCurve { x: model.xCoords[5]; y: model.yCoords[5]
                        Behavior on x { SmoothedAnimation { velocity: 10; duration: 5000 }}
                        Behavior on y { SmoothedAnimation { velocity: 10; duration: 5000 }}
                    }
                    PathCurve { x: model.xCoords[6]; y: model.yCoords[6]
                        Behavior on x { SmoothedAnimation { velocity: 10; duration: 5000 }}
                        Behavior on y { SmoothedAnimation { velocity: 10; duration: 5000 }}
                    }
                    PathCurve { x: model.xCoords[7]; y: model.yCoords[7]
                        Behavior on x { SmoothedAnimation { velocity: 10; duration: 5000 }}
                        Behavior on y { SmoothedAnimation { velocity: 10; duration: 5000 }}
                    }
                    PathCurve { x: model.xCoords[8]; y: model.yCoords[8]
                        Behavior on x { SmoothedAnimation { velocity: 10; duration: 5000 }}
                        Behavior on y { SmoothedAnimation { velocity: 10; duration: 5000 }}
                    }
                    PathCurve { x: model.xCoords[9]; y: model.yCoords[9]
                        Behavior on x { SmoothedAnimation { velocity: 10; duration: 5000 }}
                        Behavior on y { SmoothedAnimation { velocity: 10; duration: 5000 }}
                    }
                    PathCurve { x: model.xCoords[10]; y: model.yCoords[10]
                        Behavior on x { SmoothedAnimation { velocity: 10; duration: 5000 }}
                        Behavior on y { SmoothedAnimation { velocity: 10; duration: 5000 }}
                    }
                    PathCurve { x: model.xCoords[0]; y: model.yCoords[0]
                        Behavior on x { SmoothedAnimation { velocity: 10; duration: 5000 }}
                        Behavior on y { SmoothedAnimation { velocity: 10; duration: 5000 }}
                    }

                    fillGradient: RadialGradient {
                        centerX: 60; centerY: 60
                        // 45
                        centerRadius: model.radius*1.1//1.25
                        focalX: 60; focalY: 60
                        // 0, 0.4, 0.8, 1.0
                        GradientStop { id: grad1; position: 0; color: "#003F51B5" }
                        GradientStop { id: grad2; position: 0.4; color: "#003F51B5" }
                        GradientStop { position: 0.8; color: Material.color(Material.Indigo) }
                        GradientStop { position: 1.0; color: Material.color(Material.Pink) }
                        //GradientStop { position: 1.0; color: "#00E91E63" }
                    }
                }
            }
        }

        Label {
            id: statusReadout
            text: CornrowModel.statusLabel;
            font.capitalization: Font.SmallCaps
            font.pixelSize: 20
            font.weight: Font.DemiBold

            anchors.top: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            id: errorReadout
            horizontalAlignment: Text.AlignHCenter
            //verticalAlignment: Text.AlignBottom
            maximumLineCount: 3
            text: CornrowModel.statusText;
            width: 240
            height: 48
            clip: true
            wrapMode: Text.Wrap
            font.pixelSize: 16
            font.weight: Font.Light

            anchors.top: statusReadout.bottom
            anchors.topMargin: 8
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    ToolBar {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 48
        visible: CornrowModel.status != CornrowModel.Discovering &&
                 CornrowModel.status != CornrowModel.Connecting &&
                 CornrowModel.status != CornrowModel.Connected
        background: background
        RowLayout {
            id: column
            anchors.horizontalCenter: parent.horizontalCenter

            ToolButton {
                text: "Retry"
                onPressed: CornrowModel.startDiscovering()
            }
            ToolButton {
                text: "Demo"
                onPressed: CornrowModel.startDemoMode()
            }
        }
    }

    ToolButton {
        text: "Abort and retry"
        onPressed: CornrowModel.startDiscovering()
        background: background
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 48
        visible: CornrowModel.status == CornrowModel.Connecting
    }
}
