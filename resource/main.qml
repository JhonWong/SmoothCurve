import QtQuick 2.2
import SceneGraphRendering 1.0

Item {
	id:root
    width: 1920
    height: 1080
	visible: true
	
    Image {
        id: circle
        width: 200
        height: 200
        anchors.centerIn: parent
        source: "qrc:/circle.jpg"
        fillMode: Image.PreserveAspectFit

        visible: false
        layer.enabled: true
    }
	
    SmoothCurve{
        anchors.horizontalCenter: parent.horizontalCenter
        y: root.height * 0.05;
        width: parent.width
        height: parent.height
        source: circle
    }
}
