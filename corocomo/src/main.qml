import QtQml 2.2
import QtQuick 2.9
import QtQuick.Controls 2.4
import QtQuick.Controls.Material 2.4
import QtQuick.Layouts 1.3
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

    Material.theme: Material.Dark
    Material.accent: Material.color(Material.Indigo)
    Material.primary: Material.color(Material.Indigo)

    Connections {
        target: CornrowModel
        onFilterChanged: {
            eqChart.setFilter(i, t, f, g, q);
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

    Button {
        text: "Retry"
        font.capitalization: Font.AllUppercase
        visible: CornrowModel.status != CornrowModel.Discovering && CornrowModel.status != CornrowModel.Connected
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 48
        anchors.horizontalCenter: parent.horizontalCenter
        onPressed: CornrowModel.startDiscovering()
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
            plotCount: CornrowModel.filterCount
            currentPlot: CornrowModel.currentBand
            currentPlotColor: Material.accent
            plotColor: Material.foreground
            sumPlotColor: Material.primary
            warningColor: "orange"
            criticalColor: "red" //"red"
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

    ToolButton {
        id: remBand
        enabled: CornrowModel.filterCount > 0
        text: qsTr("-")
        anchors.bottom: typeLabel.top
        anchors.left: parent.left

        onPressed: CornrowModel.deleteFilter()
    }
    ToolButton {
        id: addBand
        enabled: CornrowModel.filterCount < 4
        text: qsTr("+")
        anchors.bottom: typeLabel.top
        anchors.right: parent.right

        onPressed: CornrowModel.addFilter()
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
        /*
        Label {
            id: typeLabel
            text: "Type"
            anchors.left: freqLabel.left
            anchors.bottom: type.top
        }
        */
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
            stepSize: 1
            from: 0
            to: 120
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
            text: CornrowModel.gain.toFixed(1)
            anchors.horizontalCenter: gainSlider.horizontalCenter
            anchors.bottom: gainSlider.top
        }
        ToolButton {
            id: decGain
            text: qsTr("-")
            anchors.bottom: qLabel.top
            anchors.left: parent.left

            onPressed: CornrowModel.gain -= 0.5
        }
        ToolButton {
            id: incGain
            text: qsTr("+")
            anchors.bottom: qLabel.top
            anchors.right: parent.right

            onPressed: CornrowModel.gain += 0.5
        }
        Slider {
            id: gainSlider
            stepSize: 0.5
            to: 6
            from: -24
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
            anchors.left: parent.left
            onPressed: CornrowModel.stepQ(-1)
        }
        ToolButton {
            id: incQ
            text: qsTr("+")
            anchors.bottom: parent.bottom
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
