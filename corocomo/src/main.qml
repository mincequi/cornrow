import QtQml 2.2
import QtQuick 2.9
import QtQuick.Controls 2.4
import QtQuick.Controls.Material 2.4
import QtQuick.Layouts 1.3
import Cornrow.EqChart 1.0
import Cornrow.Model 1.0
import Cornrow.ModelConfiguration 1.0

ApplicationWindow {
    id: appWindow
    visible: true
    // iPhone SE
    //width: 320
    //height: 568
    // iPhone 6
    //width: 375
    //height: 667
    // Google Nexus 5, Samsung Galaxy S5, S6, S7
    width: 360
    height: 640
    // Samsung Galaxy S8
    //width: 360
    //height: 740
    // Google Pixel
    //width: 411
    //height: 731

    Material.theme: Material.Dark
    Material.accent: Material.color(Material.Indigo)
    Material.primary: Material.color(Material.Indigo)

    Connections {
        target: CornrowModel
        onFilterChanged: {
            eqChart.setFilter(i, t, f, g, q); // @TODO(mawe): make struct
        }
    }

    BusyIndicator {
        id: busyIndicator
        visible: CornrowModel.status == CornrowModel.Discovering
        anchors.centerIn: parent
    }

    Label {
        id: statusReadout
        text: CornrowModel.statusReadout;
        visible: CornrowModel.status != CornrowModel.Connected
        font.capitalization: Font.SmallCaps
        font.pixelSize: 20
        font.weight: Font.DemiBold

        anchors.top: busyIndicator.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Label {
        id: errorReadout
        horizontalAlignment: Text.AlignHCenter
        maximumLineCount: 2
        text: CornrowModel.errorReadout;
        visible: CornrowModel.status != CornrowModel.Connected
        width: 240
        wrapMode: Text.Wrap
        font.pixelSize: 16
        font.weight: Font.Light

        anchors.top: statusReadout.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }

    // Button
    ToolBar {
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 48
        anchors.horizontalCenter: parent.horizontalCenter
        visible: CornrowModel.status != CornrowModel.Discovering && CornrowModel.status != CornrowModel.Connected

        Row {
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

    Item {
        id: peq
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        enabled: CornrowModel.status == CornrowModel.Connected
        opacity: CornrowModel.status == CornrowModel.Connected ? 1.0 : 0.1

        CornrowEqChart {
            id: eqChart
            frequencyTable: CornrowModel.configuration.freqTable
            plotCount: CornrowModel.filterCount
            currentPlot: CornrowModel.currentBand
            currentPlotColor: Material.accent
            plotColor: Material.foreground
            sumPlotColor: Material.primary
            warningColor: "orange" // unused
            criticalColor: "red"
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: bandBar.top
        }

        /*
        Label {
            id: bandLabel
            text: "Band"
            anchors.left: typeLabel.left
            anchors.bottom: bandBar.top
        }
        */

        ToolBar {
            id: bandBar
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: typeBar.top
            background: background

            Row {
                id: bandRow
                ToolButton {
                    text: qsTr("1")
                    autoExclusive: true
                    checked: CornrowModel.currentBand == 0
                    onPressed: CornrowModel.setCurrentBand(0)
                }
                ToolButton {
                    text: qsTr("2")
                    autoExclusive: true
                    checked: CornrowModel.currentBand == 1
                    onPressed: CornrowModel.setCurrentBand(1)
                }
                ToolButton {
                    text: qsTr("3")
                    autoExclusive: true
                    checked: CornrowModel.currentBand == 2
                    onPressed: CornrowModel.setCurrentBand(2)
                }
                ToolButton {
                    text: qsTr("4")
                    autoExclusive: true
                    checked: CornrowModel.currentBand == 3
                    onPressed: CornrowModel.setCurrentBand(3)
                }
                ToolButton {
                    text: qsTr("5")
                    autoExclusive: true
                    checked: CornrowModel.currentBand == 4
                    onPressed: CornrowModel.setCurrentBand(4)
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
                ToolButton {
                    text: qsTr("Off")
                    autoExclusive: true
                    checked: CornrowModel.type == 0
                    onPressed: CornrowModel.type = 0
                }
                ToolButton {
                    text: qsTr("Peaking")
                    autoExclusive: true
                    checked: CornrowModel.type == 1
                    onPressed: CornrowModel.type = 1
                }
                ToolButton {
                    text: qsTr("LowPass")
                    autoExclusive: true
                    checked: CornrowModel.type == 2
                    onPressed: CornrowModel.type = 2
                }
                ToolButton {
                    text: qsTr("HighPass")
                    autoExclusive: true
                    checked: CornrowModel.type == 3
                    onPressed: CornrowModel.type = 3
                }
            }
        }

        // Frequency
        Label {
            id: freqLabel
            text: "Frequency"
            anchors.left: decFreq.horizontalCenter
            anchors.bottom: freqSlider.top
        }
        Label {
            id: freqReadout
            text: CornrowModel.freqReadout
            anchors.horizontalCenter: freqSlider.horizontalCenter
            anchors.bottom: freqSlider.top
        }
        ToolButton {
            id: decFreq
            text: qsTr("-")
            anchors.bottom: gainLabel.top
            anchors.left: parent.left

            onPressed: CornrowModel.stepFreq(-1)
        }
        ToolButton {
            id: incFreq
            text: qsTr("+")
            anchors.bottom: gainLabel.top
            anchors.right: parent.right

            onPressed: CornrowModel.stepFreq(1)
        }
        Slider {
            id: freqSlider
            anchors.bottom: gainLabel.top
            anchors.left: decFreq.right
            anchors.right: incFreq.left
            anchors.top: incFreq.top

            value: CornrowModel.freqSlider
            onValueChanged: CornrowModel.freqSlider = value
        }

        // Gain
        Label {
            id: gainLabel
            text: "Gain"
            anchors.left: decGain.horizontalCenter
            anchors.bottom: gainSlider.top
        }
        Label {
            id: gainReadout
            text: CornrowModel.configuration.gainStep < 1.0 ? CornrowModel.gain.toFixed(1) : CornrowModel.gain.toFixed(0)
            anchors.horizontalCenter: gainSlider.horizontalCenter
            anchors.bottom: gainSlider.top
        }
        ToolButton {
            id: decGain
            text: qsTr("-")
            anchors.bottom: qLabel.top
            anchors.left: parent.left
            onPressed: CornrowModel.stepGain(-1)
        }
        ToolButton {
            id: incGain
            text: qsTr("+")
            anchors.bottom: qLabel.top
            anchors.right: parent.right
            onPressed: CornrowModel.stepGain(1)
        }
        Slider {
            id: gainSlider
            stepSize: CornrowModel.configuration.gainStep
            from: CornrowModel.configuration.gainMin
            to: CornrowModel.configuration.gainMax
            anchors.bottom: qLabel.top
            anchors.left: decGain.right
            anchors.right: incGain.left
            anchors.top: incGain.top

            value: CornrowModel.gain
            onValueChanged: CornrowModel.gain = value
        }

        // Q
        Label {
            id: qLabel
            text: "Q"
            anchors.left: decQ.horizontalCenter
            anchors.bottom: qSlider.top
        }
        Label {
            id: qReadout
            text: CornrowModel.qReadout
            anchors.horizontalCenter: qSlider.horizontalCenter
            anchors.bottom: qSlider.top
        }
        ToolButton {
            id: decQ
            text: qsTr("-")
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 16
            anchors.left: parent.left
            onPressed: CornrowModel.stepQ(-1)
        }
        ToolButton {
            id: incQ
            text: qsTr("+")
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 16
            anchors.right: parent.right
            onPressed: CornrowModel.stepQ(1)
        }
        Slider {
            id: qSlider
            anchors.bottom: decQ.bottom
            anchors.left: decQ.right
            anchors.right: incQ.left
            anchors.top: incQ.top

            value: CornrowModel.qSlider
            onValueChanged: CornrowModel.qSlider = value
        }
    } // Item
}
