import QtQuick          2.3
import QtQuick.Controls 1.2

import QGroundControl.ScreenTools 1.0
import QGroundControl.Palette     1.0

Canvas {
    id:     root

    width:  _width
    height: _height

    signal clicked

    property string label                           ///< Label to show to the side of the index indicator
    property int    index:                  0       ///< Index to show in the indicator, 0 will show single char label instead, -1 first char of label in indicator full label to the side
    property bool   checked:                false
    property bool   small:                  false
    property bool   child:                  false
    property var    color:                  checked ? "green" : (child ? qgcPal.mapIndicatorChild : qgcPal.mapIndicator)
    property real   anchorPointX:           _height / 2
    property real   anchorPointY:           _height / 2
    property bool   specifiesCoordinate:    true
    property real   gimbalYaw
    property real   vehicleYaw
    property bool   showGimbalYaw:          false

    property real   _width:             showGimbalYaw ? Math.max(_gimbalYawWidth, labelControl.visible ? labelControl.width : indicator.width) : (labelControl.visible ? labelControl.width : indicator.width)
    property real   _height:            showGimbalYaw ? _gimbalYawWidth : (labelControl.visible ? labelControl.height : indicator.height)
    property real   _gimbalYawRadius:   ScreenTools.defaultFontPixelHeight
    property real   _gimbalYawWidth:    _gimbalYawRadius * 2
    property real   _indicatorRadius:   small ? (ScreenTools.defaultFontPixelHeight * ScreenTools.smallFontPointRatio * 1.25 / 2) : (ScreenTools.defaultFontPixelHeight * 0.66)
    property real   _gimbalRadians:     degreesToRadians(vehicleYaw + gimbalYaw - 90)
    property real   _labelMargin:       2
    property real   _labelRadius:       _indicatorRadius + _labelMargin
    property string _label:             label.length > 1 ? label : ""
    property string _index:             index === 0 || index === -1 ? label.charAt(0) : index

    onColorChanged:         requestPaint()
    onShowGimbalYawChanged: requestPaint()
    onGimbalYawChanged:     requestPaint()
    onVehicleYawChanged:    requestPaint()

    QGCPalette { id: qgcPal }

    function degreesToRadians(degrees) {
        return (Math.PI/180)*degrees
    }

    function paintGimbalYaw(context) {
        if (showGimbalYaw) {
            context.save()
            context.globalAlpha = 0.75
            context.beginPath()
            context.moveTo(anchorPointX, anchorPointY)
            context.arc(anchorPointX, anchorPointY, _gimbalYawRadius,  _gimbalRadians + degreesToRadians(45), _gimbalRadians + degreesToRadians(-45), true /* clockwise */)
            context.closePath()
            context.fillStyle = "white"
            context.fill()
            context.restore()
        }
    }

    onPaint: {
        var context = getContext("2d")
        context.clearRect(0, 0, width, height)
        paintGimbalYaw(context)
    }

    Rectangle {
        id:                     labelControl
        anchors.leftMargin:     -((_labelMargin * 2) + indicator.width)
        anchors.rightMargin:    -(_labelMargin * 2)
        anchors.fill:           labelControlLabel
        color:                  "white"
        opacity:                0.5
        radius:                 _labelRadius
        visible:                _label.length !== 0 && !small
    }

    QGCLabel {
        id:                     labelControlLabel
        anchors.topMargin:      -_labelMargin
        anchors.bottomMargin:   -_labelMargin
        anchors.leftMargin:     _labelMargin
        anchors.left:           indicator.right
        anchors.top:            indicator.top
        anchors.bottom:         indicator.bottom
        color:                  "white"
        text:                   _label
        verticalAlignment:      Text.AlignVCenter
        visible:                labelControl.visible
    }

    Rectangle {
        id:                             indicator
        anchors.horizontalCenter:       parent.left
        anchors.verticalCenter:         parent.top
        anchors.horizontalCenterOffset: anchorPointX
        anchors.verticalCenterOffset:   anchorPointY
        width:                          _indicatorRadius * 2
        height:                         width
        color:                          root.color
        radius:                         _indicatorRadius

        QGCLabel {
            anchors.fill:           parent
            horizontalAlignment:    Text.AlignHCenter
            verticalAlignment:      Text.AlignVCenter
            color:                  "white"
            font.pointSize:         ScreenTools.defaultFontPointSize
            fontSizeMode:           Text.Fit
            text:                   _index
        }
    }

    QGCMouseArea {
        fillItem:   parent
        onClicked: {
            focus = true
            parent.clicked()
        }
    }
}