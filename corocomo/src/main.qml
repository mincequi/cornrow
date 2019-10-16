import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3

import Cornrow.BodePlotModel 1.0
import Cornrow.Configuration 1.0
import Cornrow.EqChart 1.0
import Cornrow.Model 1.0
import Cornrow.PhaseChart 1.0

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
            CornrowBodePlotModel.setFilter(i, t, f, g, q)
            eqChart.update()
            phaseChart.update()
        }
        onGainChanged: {
            if (CornrowModel.currentBand == CornrowModel.peqFilterCount) {
                var t = CornrowModel.gain < 1.0 ? 0 : 1
                if (t !== CornrowModel.filterType) CornrowModel.filterType = t
            }
        }
    }

    CornrowBusyIndicator {
        anchors.fill: parent
    }

    Item {
        id: peq
        anchors.fill: parent
        enabled: CornrowModel.status == CornrowModel.Connected
        opacity: CornrowModel.status == CornrowModel.Connected ? 1.0 : 0.1
        Behavior on opacity { SmoothedAnimation { velocity: 2.0 }}

        SwipeView {
            id: bodeView
            currentIndex: pageIndicator.currentIndex
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: bandBar.top

            CornrowEqChart {
                id: eqChart
                frequencyTable: CornrowConfiguration.freqTable
                bodePlot: CornrowBodePlotModel
                currentFilter: CornrowModel.currentBand
                currentPlotColor: Material.accent
                plotColor: Material.foreground
                sumPlotColor: Material.accent
                warningColor: "orange" // unused
                criticalColor: Material.color(Material.Pink)
            }

            CornrowPhaseChart {
                id: phaseChart
                frequencyTable: CornrowConfiguration.freqTable
                bodePlot: CornrowBodePlotModel
                currentFilter: CornrowModel.currentBand
                currentPlotColor: Material.accent
                plotColor: Material.foreground
                sumPlotColor: Material.accent
                criticalColor: Material.color(Material.Pink)
            }
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
            enabled: CornrowModel.currentBand != CornrowModel.peqFilterCount
            opacity: CornrowModel.currentBand != CornrowModel.peqFilterCount

            Row {
                id: typeRow
                Repeater {
                    model: CornrowModel.filterTypeNames
                    TabButton {
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
            opacity: CornrowModel.currentBand != CornrowModel.peqFilterCount

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
        } // Column

        FilterParameter {
            anchors.top: filterParameters.top
            anchors.left: parent.left
            anchors.right: parent.right
            label: "Loudness"
            unit: "phon"
            opacity: CornrowModel.currentBand == CornrowModel.peqFilterCount
            enabled: CornrowModel.currentBand == CornrowModel.peqFilterCount
            readout: CornrowModel.gain.toFixed(0)
            onStep: CornrowModel.stepGain(i)
            value: CornrowModel.gainSlider
            onValueChanged: CornrowModel.gainSlider = value
        }
    } // Item
}
