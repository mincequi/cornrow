import QtQml 2.2
import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3
import QtQuick.Shapes 1.11

import Cornrow.BusyIndicatorModel 1.0
import Cornrow.Configuration 1.0
import Cornrow.EqChart 1.0
import Cornrow.Model 1.0

ApplicationWindow {
    id: appWindow
    visible: true
    // iPhone SE
    width: 320
    height: 568
    // iPhone 6
    //width: 375
    //height: 667
    // Google Nexus 5, Samsung Galaxy S5, S6, S7
    //width: 360
    //height: 640
    // Samsung Galaxy S8
    //width: 360
    //height: 740
    // Google Pixel
    //width: 411
    //height: 731

    Material.theme: Material.Dark
    Material.accent: Material.color(Material.Indigo)
    Material.primary: Material.color(Material.Indigo)
    Material.background: "#FF212121" //#FF000030

    Component.onCompleted: {
        CornrowModel.startDiscovering()

    }

    Connections {
        target: CornrowModel
        onFilterChanged: {
            eqChart.setFilter(i, t, f, g, q); // @TODO(mawe): make struct
        }
    }

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
        enabled: CornrowModel.status != CornrowModel.Connected
        opacity: CornrowModel.status != CornrowModel.Connected ? 1.0 : 0.0
        Behavior on opacity { SmoothedAnimation { velocity: 2.0 }}
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

    Item {
        id: peq
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        enabled: CornrowModel.status == CornrowModel.Connected
        opacity: CornrowModel.status == CornrowModel.Connected ? 1.0 : 0.1
        Behavior on opacity { SmoothedAnimation { velocity: 2.0 }}

        CornrowEqChart {
            id: eqChart
            frequencyTable: CornrowConfiguration.freqTable
            currentFilter: CornrowModel.currentBand
            currentPlotColor: Material.accent
            plotColor: Material.foreground
            sumPlotColor: Material.primary
            warningColor: "orange" // unused
            criticalColor: Material.color(Material.Pink)
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: bandBar.top
        }

        ToolBar {
            id: bandBar
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: typeBar.top
            background: background

            Row {
                id: bandRow
                Repeater {
                    model: CornrowModel.peqFilterCount
                    ToolButton {
                        text: index+1
                        autoExclusive: true
                        checked: CornrowModel.currentBand == index
                        onPressed: CornrowModel.setCurrentBand(index)
                    }
                } // Repeater
                ToolButton {
                    text: "XO"
                    visible: CornrowConfiguration.xoAvailable
                    autoExclusive: true
                    checked: CornrowModel.currentBand == CornrowModel.peqFilterCount
                    onPressed: CornrowModel.setCurrentBand(CornrowModel.peqFilterCount)
                }
                ToolButton {
                    text: "SW"
                    visible: CornrowConfiguration.swAvailable
                    autoExclusive: true
                    checked: CornrowModel.currentBand == CornrowModel.peqFilterCount+1
                    onPressed: CornrowModel.setCurrentBand(CornrowModel.peqFilterCount+1)
                }
            }
        }

        // Type
        ToolBar {
            id: typeBar
            anchors.bottom: freqLabel.top
            anchors.left: parent.left
            anchors.right: parent.right
            background: background

            Row {
                id: typeRow
                Repeater {
                    model: CornrowModel.filterTypeNames
                    ToolButton {
                        text: CornrowModel.filterTypeNames[index]
                        autoExclusive: true
                        checked: CornrowModel.filterType == index
                        onPressed: CornrowModel.filterType = index
                    }
                }
            }
        }

        // Frequency
        Label {
            id: freqLabel
            text: "Frequency"
            anchors.left: freqDec.horizontalCenter
            anchors.bottom: freqSlider.top
        }
        Label {
            id: freqReadout
            text: CornrowModel.freqReadout
            anchors.horizontalCenter: freqSlider.horizontalCenter
            anchors.bottom: freqSlider.top
        }
        ToolButton {
            id: freqDec
            text: qsTr("-")
            anchors.bottom: qLabel.top
            anchors.horizontalCenter: gainDec.horizontalCenter
            onPressed: CornrowModel.stepFreq(-1)
        }
        ToolButton {
            id: freqInc
            text: qsTr("+")
            anchors.bottom: qLabel.top
            anchors.horizontalCenter: gainInc.horizontalCenter
            onPressed: CornrowModel.stepFreq(1)
        }
        Slider {
            id: freqSlider
            anchors.bottom: qLabel.top
            anchors.top: freqInc.top
            anchors.left: gainSlider.anchors.left
            anchors.leftMargin: 4
            anchors.right: gainSlider.anchors.right
            anchors.rightMargin: 4
            value: CornrowModel.freqSlider
            onValueChanged: CornrowModel.freqSlider = value
        }

        // Q
        Label {
            id: qLabel
            text: "Q"
            anchors.left: qDec.horizontalCenter
            anchors.bottom: qSlider.top
        }
        Label {
            id: qReadout
            text: CornrowModel.qReadout
            anchors.horizontalCenter: qSlider.horizontalCenter
            anchors.bottom: qSlider.top
        }
        ToolButton {
            id: qDec
            text: qsTr("-")
            anchors.bottom: gainLabel.top
            anchors.horizontalCenter: gainDec.horizontalCenter
            onPressed: CornrowModel.stepQ(-1)
        }
        ToolButton {
            id: qInc
            text: qsTr("+")
            anchors.bottom: gainLabel.top
            anchors.horizontalCenter: gainInc.horizontalCenter
            onPressed: CornrowModel.stepQ(1)
        }
        Slider {
            id: qSlider
            anchors.bottom: gainLabel.top
            anchors.top: qInc.top
            anchors.left: gainSlider.anchors.left
            anchors.leftMargin: 4
            anchors.right: gainSlider.anchors.right
            anchors.rightMargin: 4
            value: CornrowModel.qSlider // @TODO(mawe) fix binding loop
            onValueChanged: CornrowModel.qSlider = value
        }

        // Gain
        Label {
            id: gainLabel
            text: "Gain"
            anchors.left: gainDec.horizontalCenter
            anchors.bottom: gainSlider.top
        }
        Label {
            id: gainReadout
            text: CornrowConfiguration.gainStep < 1.0 ? CornrowModel.gain.toFixed(1) : CornrowModel.gain.toFixed(0)
            anchors.horizontalCenter: gainSlider.horizontalCenter
            anchors.bottom: gainSlider.top
        }
        ToolButton {
            id: gainDec
            text: qsTr("-")
            anchors.verticalCenter: gainSlider.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 4
            onPressed: CornrowModel.stepGain(-1)
        }
        ToolButton {
            id: gainInc
            text: qsTr("+")
            anchors.verticalCenter: gainSlider.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 4
            onPressed: CornrowModel.stepGain(1)
        }
        Slider {
            id: gainSlider
            stepSize: CornrowConfiguration.gainStep
            from: CornrowConfiguration.gainMin
            to: CornrowConfiguration.gainMax
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 16
            anchors.left: gainDec.right
            anchors.leftMargin: 4
            anchors.right: gainInc.left
            anchors.rightMargin: 4
            value: CornrowModel.gain
            onValueChanged: CornrowModel.gain = value
        }
    } // Item
}
