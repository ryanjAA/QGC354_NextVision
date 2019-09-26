/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


import QtQuick 2.3

import QGroundControl               1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Controls      1.0
import QGroundControl.Palette       1.0
import QGroundControl.Vehicle       1.0

Item {
    //property bool useLightColors - Must be passed in from loaded

    Timer {
        interval:   80  // 25Hz / 2, same as real joystick rate
        running:    QGroundControl.settingsManager.appSettings.virtualJoystick.value && _activeVehicle
        repeat:     true
        onTriggered: {
            if (_activeVehicle)
                _activeVehicle.virtualTabletJoystickValue(rightStick.xAxis, rightStick.yAxis, 0, zoomValue)
        }
    }


    QGCColoredImage {
        id:                     zoomInButton
        height:                 parent.height * 1.4
        width:                  parent.height * 1.4
        mipmap:                 true
        anchors.top:                parent.top
        anchors.topMargin:          height * -0.5
        source:             "/qmlimages/ZoomPlus.svg"
        fillMode:           Image.PreserveAspectFit
        sourceSize.height:  height
        color:              qgcPal.colorWhite
        MouseArea {
            width: 100; height: 100
            anchors.centerIn: parent
            onPressedChanged: {
                if ( pressed ) {
                    zoomValue = 1;
                    zoomInButton.color = "red";

                } else {
                    zoomValue = 0;
                    zoomInButton.color = "white";
                }
            }
        }
    }

    QGCColoredImage {
        id:                     zoomOutButton
        height:                 parent.height * 1.4
        width:                  parent.height * 1.4
        mipmap:                     true
        anchors.bottom:             parent.bottom
        anchors.bottomMargin:       height * -0.3
        anchors.left:           parent.left
        source:             "/qmlimages/ZoomMinus.svg"
        fillMode:           Image.PreserveAspectFit
        sourceSize.height:  height
        color:              qgcPal.colorWhite
        MouseArea {
            width: 100; height: 100
            anchors.centerIn: parent
            onPressedChanged: {
                if ( pressed ) {
                    zoomValue = 2;
                    zoomOutButton.color = "red"
                } else {
                    zoomValue = 0;
                    zoomOutButton.color = "white"
                }
            }
        }
    }

    JoystickThumbPad {
        id:                     rightStick
        anchors.rightMargin:    ScreenTools.defaultFontPixelHeight * 2
        anchors.bottomMargin:   0
        anchors.right:          parent.right
        anchors.bottom:         parent.bottom
        width:                  parent.height
        height:                 parent.height
        lightColors:            useLightColors
    }
}
