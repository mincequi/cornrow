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
        /*
        ToolButton {
            id: help
            text: "?"
            enabled: CornrowModel.status == CornrowModel.Connected
            anchors.right: parent.right
            anchors.top: parent.top
        }
        */

        TabBar {
            id: bandBar
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: typeBar.top
            background: background

            Repeater {
                model: CornrowModel.peqFilterCount
                FilterBandButton {
                    text: index+1
                    indicatorColor: Material.primary
                    indicatorVisible: CornrowModel.activeFilters[index]
                    checked: CornrowModel.currentBand == index
                    onPressed: CornrowModel.setCurrentBand(index)
                }
            } // Repeater
            FilterBandButton {
                text: "LN"
                indicatorColor: Material.primary
                indicatorVisible: CornrowModel.activeFilters[CornrowModel.peqFilterCount]
                visible: CornrowConfiguration.loudnessAvailable
                checked: CornrowModel.currentBand == CornrowModel.peqFilterCount
                onPressed: CornrowModel.setCurrentBand(CornrowModel.peqFilterCount)
            }
            FilterBandButton {
                text: "XO"
                indicatorColor: Material.primary
                indicatorVisible: CornrowModel.activeFilters[CornrowModel.peqFilterCount+1]
                visible: CornrowConfiguration.xoAvailable
                checked: CornrowModel.currentBand == CornrowModel.peqFilterCount+1
                onPressed: CornrowModel.setCurrentBand(CornrowModel.peqFilterCount+1)
            }
            FilterBandButton {
                text: "SW"
                indicatorColor: Material.primary
                indicatorVisible: CornrowModel.activeFilters[CornrowModel.peqFilterCount+2]
                visible: CornrowConfiguration.swAvailable
                checked: CornrowModel.currentBand == CornrowModel.peqFilterCount+2
                onPressed: CornrowModel.setCurrentBand(CornrowModel.peqFilterCount+2)
            }
        }

        // Type
        ToolBar {
            id: typeBar
            anchors.bottom: filterParameters.top
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
                        checked: CornrowModel.filterType === index
                        onPressed: CornrowModel.filterType = index
                    }
                }
            }
        }

        Column {
            id: filterParameters
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 16
            anchors.left: parent.left
            anchors.right: parent.right

            FilterParameter {
                label: "Frequency"
                unit: "Hz"
                opacity: CornrowModel.currentBand != CornrowModel.peqFilterCount
                enabled: CornrowModel.currentBand != CornrowModel.peqFilterCount &&
                         CornrowModel.filterType > 0
                readout: CornrowModel.freqReadout
                onStep: CornrowModel.stepFreq(i)
                value: CornrowModel.freqSlider
                onValueChanged: CornrowModel.freqSlider = value
            }
            FilterParameter {
                label: "Q"
                opacity: CornrowModel.currentBand < CornrowModel.peqFilterCount
                enabled: CornrowModel.currentBand < CornrowModel.peqFilterCount &&
                         CornrowModel.filterType > 0
                readout: CornrowModel.qReadout
                onStep: CornrowModel.stepQ(i)
                value: CornrowModel.qSlider
                onValueChanged: CornrowModel.qSlider = value
            }
            FilterParameter {
                label: "Gain"
                unit: CornrowModel.gainUnit
                opacity: CornrowModel.currentBand <= CornrowModel.peqFilterCount
                enabled: CornrowModel.currentBand <= CornrowModel.peqFilterCount &&
                         (CornrowModel.filterType === 1 ||
                          CornrowModel.filterType === 4 ||
                          CornrowModel.filterType === 5)
                readout: CornrowModel.gainStep < 1.0 ? CornrowModel.gain.toFixed(1) : CornrowModel.gain.toFixed(0)
                onStep: CornrowModel.stepGain(i)
                value: CornrowModel.gainSlider
                onValueChanged: CornrowModel.gainSlider = value
            }
        }
    } // Item
}
