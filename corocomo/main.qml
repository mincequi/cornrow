import QtQuick 2.8
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3
import Cornrow.EqChart 1.0
import Cornrow.Model 1.0

ApplicationWindow {
    id: appWindow
    visible: true
    width: 375
    height: 667

    Material.theme: Material.Dark
    Material.accent: Material.color(Material.Indigo)
    Material.primary: Material.color(Material.Indigo)


    CornrowModel {
        id: model

        onFilterCountChanged: {
            // Bug in QML? Need to call this before for loop
            //eqChart.setFilterCount(model.filterCount);
            for (var i = 0; filtersLayout.children.length; i++) {
                filtersLayout.children[i].enabled = i < model.filterCount;
            }
        }
        onFilterAdded: {
            eqChart.addFilter();
        }
        onFilterRemoved: {
            eqChart.removeFilter(i);
        }

        onCurrentFilterChanged: {
            eqChart.currentPlot = model.currentFilter
            filtersLayout.children[model.currentFilter].checked = true
        }

        onFilterChanged: {
            eqChart.setFilter(i, t, f, g, q);
        }

        onTypeChanged: {
            for (var i = 0; type.children.length; i++) {
                typesLayout.children[i].checked = i+1 == model.type;
            }
        }

        onFreqSliderChanged: {
            freqSlider.value = model.freqSlider
        }
        onGainChanged: {
            gainSlider.value = model.gain
        }
        onQSliderChanged: {
            qSlider.value = model.qSlider
        }
    }

    CornrowEqChart {
        id: eqChart
        currentPlotColor: Material.accent
        plotColor: Material.foreground
        sumPlotColor: Material.primary
        warningColor: "orange"
        criticalColor: "red" //"red"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: bandLabel.top
    }

    Label {
        id: bandLabel
        text: "Band"
        anchors.left: remBand.horizontalCenter
        anchors.bottom: remBand.top
    }

    ToolButton {
        id: remBand
        enabled: model.filterCount > 0
        text: qsTr("-")
        anchors.bottom: typeLabel.top
        anchors.left: parent.left

        onPressed: model.deleteFilter()
    }
    ToolButton {
        id: addBand
        enabled: model.filterCount < 4
        text: qsTr("+")
        anchors.bottom: typeLabel.top
        anchors.right: parent.right

        onPressed: model.addFilter()
    }

    ToolBar {
        anchors.left: remBand.right
        anchors.right: addBand.left
        anchors.bottom: addBand.bottom
        background: background

        RowLayout {
            id: filtersLayout
            ToolButton {
                text: qsTr("1")
                autoExclusive: true
                enabled: false

                onPressed: model.setCurrentFilter(0)
            }
            ToolButton {
                text: qsTr("2")
                autoExclusive: true
                enabled: false

                onPressed: model.setCurrentFilter(1)
            }
            ToolButton {
                text: qsTr("3")
                autoExclusive: true
                enabled: false

                onPressed: model.setCurrentFilter(2)
            }
            ToolButton {
                text: qsTr("4")
                autoExclusive: true
                enabled: false

                onPressed: model.setCurrentFilter(3)
            }
        }
    }

    Label {
        id: typeLabel
        text: "Type"
        anchors.left: freqLabel.left
        anchors.bottom: type.top
    }
    ToolBar {
        id: type
        anchors.bottom: freqLabel.top
        anchors.left: freqSlider.left
        anchors.right: parent.right
        background: background

        RowLayout {
            id: typesLayout
            ToolButton {
                text: qsTr("Peaking")
                autoExclusive: true
                checked: true

                onPressed: model.type = 1
            }
            ToolButton {
                text: qsTr("LowPass")
                autoExclusive: true

                onPressed: model.type = 2
            }
            ToolButton {
                text: qsTr("HighPass")
                autoExclusive: true

                onPressed: model.type = 3
            }
        }
    }

    Label {
        id: freqLabel
        text: "Frequency"
        anchors.left: decFreq.horizontalCenter
        anchors.bottom: freqSlider.top
    }
    Label {
        id: freqReadout
        text: model.freqReadout
        anchors.horizontalCenter: freqSlider.horizontalCenter
        anchors.bottom: freqSlider.top
    }
    ToolButton {
        id: decFreq
        text: qsTr("-")
        anchors.bottom: gainLabel.top
        anchors.left: parent.left

        onPressed: model.stepFreq(-1)
    }
    ToolButton {
        id: incFreq
        text: qsTr("+")
        anchors.bottom: gainLabel.top
        anchors.right: parent.right

        onPressed: model.stepFreq(1)
    }
    Slider {
        id: freqSlider
        anchors.bottom: gainLabel.top
        anchors.left: decFreq.right
        anchors.right: incFreq.left
        anchors.top: incFreq.top

        onValueChanged: model.freqSlider = value
    }

    Label {
        id: gainLabel
        text: "Gain"
        anchors.left: decGain.horizontalCenter
        anchors.bottom: gainSlider.top
    }
    Label {
        id: gainReadout
        text: model.gain.toFixed(1)
        anchors.horizontalCenter: gainSlider.horizontalCenter
        anchors.bottom: gainSlider.top
    }
    ToolButton {
        id: decGain
        text: qsTr("-")
        anchors.bottom: qLabel.top
        anchors.left: parent.left

        onPressed: model.gain -= 0.5
    }
    ToolButton {
        id: incGain
        text: qsTr("+")
        anchors.bottom: qLabel.top
        anchors.right: parent.right

        onPressed: model.gain += 0.5
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

        onValueChanged: model.gain = value
    }

    Label {
        id: qLabel
        text: "Q"
        anchors.left: decQ.horizontalCenter
        anchors.bottom: qSlider.top
    }
    Label {
        id: qReadout
        text: model.qReadout
        anchors.horizontalCenter: qSlider.horizontalCenter
        anchors.bottom: qSlider.top
    }
    ToolButton {
        id: decQ
        text: qsTr("-")
        anchors.bottom: parent.bottom
        anchors.left: parent.left

        onPressed: model.stepQ(-1)
    }
    ToolButton {
        id: incQ
        text: qsTr("+")
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        onPressed: model.stepQ(1)
    }
    Slider {
        id: qSlider
        anchors.bottom: decQ.bottom
        anchors.left: decQ.right
        anchors.right: incQ.left
        anchors.top: incQ.top

        onValueChanged: model.qSlider = value
    }
}
