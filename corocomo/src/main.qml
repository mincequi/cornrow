import QtQuick 2.12
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3
import QtQuick.Window 2.12

import QtGraphicalEffects 1.0

import Cornrow.BodePlotModel 1.0
import Cornrow.Configuration 1.0
import Cornrow.DeviceModel 1.0
import Cornrow.EqChart 1.0
import Cornrow.FilterModel 1.0
import Cornrow.IoModel 1.0
import Cornrow.PhaseChart 1.0
import Cornrow.SoftClipChart 1.0

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

    Connections {
        target: FilterModel
        onFilterChanged: {
            CornrowBodePlotModel.setFilter(i, t, f, g, q)
            eqChart.update()
            phaseChart.update()
        }
        onCrossoverChanged: {
            CornrowBodePlotModel.setFilter(i, t, f, g, q)
            eqChart.update()
            phaseChart.update()
        }
        onGainChanged: {
            if (FilterModel.currentBand === FilterModel.peqFilterCount) {
                var t = FilterModel.gain < 1.0 ? 0 : 1
                if (t !== FilterModel.filterType) FilterModel.filterType = t
            }
        }
    }
    
    Connections {
        target: DeviceModel
        onStatusChanged: {
            if (DeviceModel.status != DeviceModel.Connected) drawer.close()
        }
    }

    DeviceDialog {
        width: appWindow.width
        height: appWindow.height
        enabled: DeviceModel.status !== DeviceModel.Connected
        opacity: DeviceModel.status != DeviceModel.Connected ? 1.0 : 0.0
        Behavior on opacity { SmoothedAnimation { velocity: 1.5 }}
        z: 10
    }

    ToolButton {
        id: menuButton
        visible: CornrowConfiguration.ioAvailable
        transform: Translate {
           y: drawer.position * menu.height
        }
        icon.source: drawer.opened ? "qrc:/icons/expand_less.svg" : "qrc:/icons/expand_more.svg"
        enabled: DeviceModel.status == DeviceModel.Connected
        z: 12
        onPressed: {
            drawer.visible = !drawer.visible
        }
    }

    Drawer {
        id: drawer
        width: parent.width
        height: menu.height
        edge: Qt.TopEdge
        interactive: false
        modal: false

        Presets {
            id: menu
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 12
            z: 1
        }
    }

    Item {
        id: peq
        anchors.fill: parent
        enabled: DeviceModel.status == DeviceModel.Connected
        opacity: DeviceModel.status == DeviceModel.Connected ? 1.0 : 0.0
        //opacity: 0.0
        Behavior on opacity { SmoothedAnimation { velocity: 1.5 }}
        transform: Translate {
           y: drawer.position * menu.height
        }

        SwipeView {
            id: bodeView
            currentIndex: pageIndicator.currentIndex
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: bandBar.top

            CornrowEqChart {
                id: eqChart
                bodePlot: CornrowBodePlotModel
                currentFilter: FilterModel.currentBand
                currentPlotColor: Material.accent
                plotColor: Material.foreground
                sumPlotColor: Material.accent
                warningColor: "orange" // unused
                criticalColor: Material.color(Material.Pink)
                Item {
                    anchors.right: parent.right
                    anchors.verticalCenter: eqChart.verticalCenter
                    anchors.rightMargin: axisLabel.height/2 + 4
                    Label {
                        id: axisLabel
                        text: "Magnitude (dB)"
                        font.pointSize: font.pointSize.valueOf() - 2
                        opacity: 0.5
                        rotation: -90
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }

            CornrowPhaseChart {
                id: phaseChart
                bodePlot: CornrowBodePlotModel
                currentFilter: FilterModel.currentBand
                currentPlotColor: Material.accent
                plotColor: Material.foreground
                sumPlotColor: Material.accent
                criticalColor: Material.color(Material.Pink)
                Item {
                    anchors.right: parent.right
                    anchors.verticalCenter: phaseChart.verticalCenter
                    anchors.rightMargin: phaseAxisLabel.height/2 + 4
                    Label {
                        id: phaseAxisLabel
                        text: "Phase (rad)"
                        font.pointSize: font.pointSize.valueOf()-2
                        opacity: 0.5
                        rotation: -90
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }

            /*
            CornrowSoftClipChart {
                id: softClipChart
                currentFilter: FilterModel.currentBand
                currentPlotColor: Material.accent
                plotColor: Material.foreground
                sumPlotColor: Material.accent
                criticalColor: Material.color(Material.Pink)
            }
            */
        }

        PageIndicator {
            id: pageIndicator
            interactive: true
            count: bodeView.count
            currentIndex: bodeView.currentIndex

            anchors.bottom: bandBar.top
            anchors.horizontalCenter: parent.horizontalCenter
        }

        /*
        ToolButton {
            id: help
            text: "?"
            enabled: DeviceModel.status == DeviceModel.Connected
            anchors.right: parent.right
            anchors.top: parent.top
        }
        */

        ToolSeparator {
            orientation: Qt.Horizontal
            anchors.top: bandBar.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            horizontalPadding: 0
            verticalPadding: 0
        }

        TabBar {
            id: bandBar
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: typeBar.top

            Repeater {
                model: FilterModel.peqFilterCount
                FilterBandButton {
                    text: index+1
                    indicatorVisible: FilterModel.activeFilters[index]
                    checked: FilterModel.currentBand === index
                    onPressed: FilterModel.setCurrentBand(index)
                }
            } // Repeater
            FilterBandButton {
                text: "LN"
                indicatorVisible: FilterModel.activeFilters[FilterModel.peqFilterCount]
                visible: CornrowConfiguration.loudnessAvailable
                checked: FilterModel.currentBand === FilterModel.peqFilterCount
                onPressed: FilterModel.setCurrentBand(FilterModel.peqFilterCount)
            }
            FilterBandButton {
                text: "XO"
                indicatorVisible: FilterModel.activeFilters[FilterModel.peqFilterCount+1]
                visible: CornrowConfiguration.xoAvailable
                enabled: CornrowIoModel.multiChannelAvailable
                checked: FilterModel.currentBand === FilterModel.peqFilterCount+1
                onPressed: FilterModel.setCurrentBand(FilterModel.peqFilterCount+1)
            }
            /*
            FilterBandButton {
                text: "SC"
                indicatorVisible: FilterModel.activeFilters[FilterModel.peqFilterCount+2]
                //visible: CornrowConfiguration.swAvailable
                checked: FilterModel.currentBand == FilterModel.peqFilterCount+2
                onPressed: FilterModel.setCurrentBand(FilterModel.peqFilterCount+2)
            }
            */
            /*
            FilterBandButton {
                text: "SW"
                indicatorVisible: FilterModel.activeFilters[FilterModel.peqFilterCount+2]
                visible: CornrowConfiguration.swAvailable
                enabled: CornrowIoModel.multiChannelAvailable
                checked: FilterModel.currentBand == FilterModel.peqFilterCount+2
                onPressed: FilterModel.setCurrentBand(FilterModel.peqFilterCount+2)
            }
            */
        }

        // Type
        ToolBar {
            id: typeBar
            anchors.bottom: filterParameters.top
            anchors.left: parent.left
            anchors.right: parent.right
            background: background
            enabled: FilterModel.currentBand !== FilterModel.peqFilterCount
            opacity: FilterModel.currentBand !== FilterModel.peqFilterCount
            visible: FilterModel.currentBand !== -1

            Row {
                id: typeRow
                Repeater {
                    model: FilterModel.filterTypeNames
                    TabButton {
                        text: FilterModel.filterTypeNames[index]
                        autoExclusive: true
                        checked: FilterModel.filterType === index
                        onPressed: FilterModel.filterType = index
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
            opacity: FilterModel.currentBand !== FilterModel.peqFilterCount &&
                     FilterModel.currentBand !== -1

            FilterParameter {
                label: "Frequency (Hz)"
                // prefix: "Hz"
                opacity: FilterModel.currentBand < FilterModel.peqFilterCount
                enabled: FilterModel.currentBand < FilterModel.peqFilterCount &&
                         FilterModel.filterType > 0
                readout: FilterModel.freqReadout
                onStep: FilterModel.stepFreq(i)
                value: FilterModel.freqSlider
                onValueChanged: FilterModel.freqSlider = value
            }
            FilterParameter {
                label: "Q"
                opacity: FilterModel.currentBand < FilterModel.peqFilterCount
                enabled: FilterModel.currentBand < FilterModel.peqFilterCount &&
                         FilterModel.filterType > 0
                readout: FilterModel.qReadout
                onStep: FilterModel.stepQ(i)
                value: FilterModel.qSlider
                onValueChanged: FilterModel.qSlider = value
            }
            FilterParameter {
                label: "Gain (dB)"
                // prefix: "dB"
                opacity: FilterModel.currentBand < FilterModel.peqFilterCount
                enabled: FilterModel.currentBand <= FilterModel.peqFilterCount &&
                         (FilterModel.filterType === 1 ||
                          FilterModel.filterType === 4 ||
                          FilterModel.filterType === 5)
                readout: CornrowConfiguration.gainStep < 1.0 ? FilterModel.gain.toFixed(1) : FilterModel.gain.toFixed(0)
                onStep: FilterModel.stepGain(i)
                value: FilterModel.gainSlider
                onValueChanged: FilterModel.gainSlider = value
            }
        } // Column

        FilterParameter {
            anchors.top: filterParameters.top
            anchors.left: parent.left
            anchors.right: parent.right
            label: "Loudness (phon)"
            // prefix: "phon"
            opacity: FilterModel.currentBand === FilterModel.peqFilterCount
            enabled: FilterModel.currentBand === FilterModel.peqFilterCount
            readout: FilterModel.gain.toFixed(0)
            onStep: FilterModel.stepGain(i)
            value: FilterModel.gainSlider
            onValueChanged: FilterModel.gainSlider = value
        }

        Column {
            id: crossover
            anchors.top: filterParameters.top
            anchors.left: parent.left
            anchors.right: parent.right
            visible: FilterModel.currentBand === FilterModel.peqFilterCount+1

            FilterParameter {
                label: "Frequency (Hz)"
                // prefix: "Hz"
                enabled: FilterModel.filterType > 0
                readout: FilterModel.freqReadout
                onStep: FilterModel.stepFreq(i)
                value: FilterModel.freqSlider
                onValueChanged: FilterModel.freqSlider = value
            }

            FilterParameter {
                label: "Gain (dB)"
                prefix: FilterModel.gain < 0 ? "High:" : FilterModel.gain === 0 ? "" : "Low:"
                enabled: FilterModel.filterType > 0
                readout: (-1*Math.abs(FilterModel.gain)).toFixed(1)
                onStep: FilterModel.stepGain(i)
                value: FilterModel.gainSlider
                onValueChanged: FilterModel.gainSlider = value
            }

            /*
            // Subwoofer Type
            ToolBar {
                id: swTypeBar
                anchors.left: parent.left
                anchors.right: parent.right
                background: background

                Row {
                    id: swTypeRow
                    Repeater {
                        model: FilterModel.filterTypeNames
                        TabButton {
                            text: FilterModel.filterTypeNames[index]
                            autoExclusive: true
                            //checked: FilterModel.filterType === index
                            //onPressed: FilterModel.filterType = index
                        }
                    }
                }
            }

            FilterParameter {
                label: "LFE Frequency (Hz)"
                unit: "Hz"
                visible: false
                //readout: FilterModel.qReadout
                //onStep: FilterModel.stepQ(i)
                //value: FilterModel.qSlider
                //onValueChanged: FilterModel.qSlider = value
            }
            */
        }

        /*
        Column {
            id: softClip
            anchors.top: filterParameters.top
            anchors.left: parent.left
            anchors.right: parent.right
            visible: FilterModel.currentBand == FilterModel.peqFilterCount+2
            FilterParameter {
                label: "Clipping"
                onValueChanged: softClipChart.setClipping(value)
            }
            FilterParameter {
                label: "InputRange"
                onValueChanged: softClipChart.setInputRange(value*2.0)
            }
        }
        */

    } // Item peq

    FastBlur {
        anchors.fill: parent
        source: peq
        radius: 32
        opacity: DeviceModel.status == DeviceModel.Connected ? 0.0 : 0.125
        Behavior on opacity { SmoothedAnimation { velocity: 0.5 }}
        transform: Translate {
           y: drawer.position * menu.height
        }
    }
}
