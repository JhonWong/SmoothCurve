import QtQuick 2.0
import CustomGeometry 1.0

Item {
    width: 1920
    height: 1080

    //RandomPointsView {
    //    id: point_list
    //    anchors.fill: parent
    //    anchors.margins: 20
    //}

    SmoothCurve {
        id: line_smooth
        anchors.fill: parent
        anchors.margins: 20
    }
}
