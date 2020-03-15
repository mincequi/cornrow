import QtQuick 2.0
import QtQuick.Controls.Material 2.3
import QtQuick.Shapes 1.14

import Cornrow.BusyIndicatorModel 1.0
import Cornrow.DeviceModel 1.0

Canvas {
    id: busyIndicator
    
    property alias strokeColor: myPath.strokeColor
    property alias fillColor: myPath.fillColor
    property alias strokeWidth: myPath.strokeWidth
    property int innerRadius: 36
    property int outerRadius: 48
    property real inactiveOpacity: 1.0
    
    width: 120
    height: 120
    contextType: "2d"
    anchors.horizontalCenter: parent.horizontalCenter
    
    CornrowBusyIndicatorModel {
        id: indicatorModel
        active: DeviceModel.status !== DeviceModel.Connected
        radius: (DeviceModel.status === DeviceModel.Discovering ||
                 DeviceModel.status === DeviceModel.Connecting) ? outerRadius : innerRadius
        numPoints: 7
        Behavior on radius { SmoothedAnimation { velocity: outerRadius*50 }}
    }
    
    Shape {
        opacity: (DeviceModel.status === DeviceModel.Discovering ||
                 DeviceModel.status === DeviceModel.Connecting) ? 1.0 : inactiveOpacity
        Behavior on opacity { SmoothedAnimation { velocity: 0.5 } }
        ShapePath {
            id: myPath
            
            strokeColor: "transparent"

            startX: indicatorModel.xCoords[0]; startY: indicatorModel.yCoords[0]
            // Only SmoothedAnimation gives nice animation here
            // Setting duration does not work well with update timer
            // So, we only use velocity here
            Behavior on startX { SmoothedAnimation { velocity: 10 }}
            Behavior on startY { SmoothedAnimation { velocity: 10 }}
            
            // @TODO(Qt): Repeater does not work here (and probably never will. We might move this to C++).
            PathCurve { x: indicatorModel.xCoords[1]; y: indicatorModel.yCoords[1]
                Behavior on x { SmoothedAnimation { velocity: 10 }}
                Behavior on y { SmoothedAnimation { velocity: 10 }}
            }
            PathCurve { x: indicatorModel.xCoords[2]; y: indicatorModel.yCoords[2]
                Behavior on x { SmoothedAnimation { velocity: 10 }}
                Behavior on y { SmoothedAnimation { velocity: 10 }}
            }
            PathCurve { x: indicatorModel.xCoords[3]; y: indicatorModel.yCoords[3]
                Behavior on x { SmoothedAnimation { velocity: 10 }}
                Behavior on y { SmoothedAnimation { velocity: 10 }}
            }
            PathCurve { x: indicatorModel.xCoords[4]; y: indicatorModel.yCoords[4]
                Behavior on x { SmoothedAnimation { velocity: 10 }}
                Behavior on y { SmoothedAnimation { velocity: 10 }}
            }
            PathCurve { x: indicatorModel.xCoords[5]; y: indicatorModel.yCoords[5]
                Behavior on x { SmoothedAnimation { velocity: 10 }}
                Behavior on y { SmoothedAnimation { velocity: 10 }}
            }
            PathCurve { x: indicatorModel.xCoords[6]; y: indicatorModel.yCoords[6]
                Behavior on x { SmoothedAnimation { velocity: 10 }}
                Behavior on y { SmoothedAnimation { velocity: 10 }}
            }
            PathCurve { x: indicatorModel.xCoords[0]; y: indicatorModel.yCoords[0]
                Behavior on x { SmoothedAnimation { velocity: 10 }}
                Behavior on y { SmoothedAnimation { velocity: 10 }}
            }
            
            fillGradient: RadialGradient {
                centerX: width/2; centerY: width/2
                centerRadius: indicatorModel.radius*1.1//1.25
                focalX: width/2; focalY: width/2
                // 0, 0.4, 0.8, 1.0
                GradientStop { id: grad1; position: 0; color: "#003F51B5" }
                GradientStop { id: grad2; position: 0.4; color: "#003F51B5" }
                GradientStop { position: 0.8; color: Material.color(Material.Indigo) }
                GradientStop { position: 1.0; color: Material.color(Material.Pink) }
                //GradientStop { position: 1.0; color: "#00E91E63" }
            }
        }
    } // Shape
} // Canvas
