/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


import QtQuick                          2.11
import QtQuick.Controls                 2.4

import QGroundControl                   1.0
import QGroundControl.FlightDisplay     1.0
import QGroundControl.FlightMap         1.0
import QGroundControl.ScreenTools       1.0
import QGroundControl.Controls          1.0
import QGroundControl.Palette           1.0
import QGroundControl.Vehicle           1.0
import QGroundControl.Controllers       1.0

Item {
    id: root
    property double _ar:                QGroundControl.videoManager.aspectRatio
    property bool   _showGrid:          QGroundControl.settingsManager.videoSettings.gridLines.rawValue > 0
    property var    _videoReceiver:     QGroundControl.videoManager.videoReceiver
    property var    _activeVehicle:     QGroundControl.multiVehicleManager.activeVehicle
    property var    _dynamicCameras:    _activeVehicle ? _activeVehicle.dynamicCameras : null
    property bool   _connected:         _activeVehicle ? !_activeVehicle.connectionLost : false
    property int    _curCameraIndex:    _dynamicCameras ? _dynamicCameras.currentCamera : 0
    property bool   _isCamera:          _dynamicCameras ? _dynamicCameras.cameras.count > 0 : false
    property var    _camera:            _isCamera ? _dynamicCameras.cameras.get(_curCameraIndex) : null
    property bool   _hasZoom:           _camera && _camera.hasZoom
    property int    _fitMode:           QGroundControl.settingsManager.videoSettings.videoFit.rawValue
    Rectangle {
        id:             noVideo
        anchors.fill:   parent
        color:          Qt.rgba(0,0,0,0.75)
        visible:        !(_videoReceiver && _videoReceiver.videoRunning)
        QGCLabel {
            text:               QGroundControl.settingsManager.videoSettings.streamEnabled.rawValue ? qsTr("WAITING FOR VIDEO") : qsTr("VIDEO DISABLED")
            font.family:        ScreenTools.demiboldFontFamily
            color:              "white"
            font.pointSize:     _mainIsMap ? ScreenTools.smallFontPointSize : ScreenTools.largeFontPointSize
            anchors.centerIn:   parent
        }
        MouseArea {
            anchors.fill: parent
            onDoubleClicked: {
                QGroundControl.videoManager.fullScreen = !QGroundControl.videoManager.fullScreen
            }
        }
    }
    Rectangle {
        anchors.fill:   parent
        color:          "black"
        visible:        _videoReceiver && _videoReceiver.videoRunning
        function getWidth() {
            //-- Fit Width or Stretch
            if(_fitMode === 0 || _fitMode === 2) {
                return parent.width
            }
            //-- Fit Height
            return _ar != 0.0 ? parent.height * _ar : parent.width
        }
        function getHeight() {
            //-- Fit Height or Stretch
            if(_fitMode === 1 || _fitMode === 2) {
                return parent.height
            }
            //-- Fit Width
            return _ar != 0.0 ? parent.width * (1 / _ar) : parent.height
        }
        QGCVideoBackground {
            id:             videoContent
            height:         parent.getHeight()
            width:          parent.getWidth()
            anchors.centerIn: parent
            receiver:       _videoReceiver
            display:        _videoReceiver && _videoReceiver.videoSurface
            visible:        _videoReceiver && _videoReceiver.videoRunning
            Connections {
                target:         _videoReceiver
                onImageFileChanged: {
                    videoContent.grabToImage(function(result) {
                        if (!result.saveToFile(_videoReceiver.imageFile)) {
                            console.error('Error capturing video frame');
                        }
                    });
                }
            }
            Rectangle {
                color:  Qt.rgba(1,1,1,0.5)
                height: parent.height
                width:  1
                x:      parent.width * 0.33
                visible: _showGrid && !QGroundControl.videoManager.fullScreen
            }
            Rectangle {
                color:  Qt.rgba(1,1,1,0.5)
                height: parent.height
                width:  1
                x:      parent.width * 0.66
                visible: _showGrid && !QGroundControl.videoManager.fullScreen
            }
            Rectangle {
                color:  Qt.rgba(1,1,1,0.5)
                width:  parent.width
                height: 1
                y:      parent.height * 0.33
                visible: _showGrid && !QGroundControl.videoManager.fullScreen
            }
            Rectangle {
                color:  Qt.rgba(1,1,1,0.5)
                width:  parent.width
                height: 1
                y:      parent.height * 0.66
                visible: _showGrid && !QGroundControl.videoManager.fullScreen
            }
        }

        PinchArea {
            anchors.fill: parent
            pinch.minimumRotation: -360
            pinch.maximumRotation: 360
            pinch.minimumScale: 0.1
            pinch.maximumScale: 10
            pinch.dragAxis: Pinch.XAndYAxis

            MouseArea {
                anchors.fill: parent
                onDoubleClicked: {
                    QGroundControl.videoManager.fullScreen = !QGroundControl.videoManager.fullScreen
                }
                onClicked: {
                    /* Calculating the position to track on */
                    var videoWidth
                    var videoHeight
                    var videoMargin
                    var xPos = mouseX
                    videoHeight = height
                    videoWidth = (videoHeight * 16.0 ) / 9.0
                    videoMargin = (width - videoWidth) / 2.0
                    if(mouseX < (videoMargin + 16))
                        xPos = videoMargin + 16
                    else if(mouseX > (videoMargin + videoWidth - 16) )
                        xPos = (videoMargin + videoWidth - 16)
                    xPos -= videoMargin
                    var xScaled = (1280.0 * xPos) / videoWidth
                    var yScaled = (720.0 * mouseY) / videoHeight
                    /* Sending the Track On Position command to the TRIP */
                    joystickManager.cameraManagement.trackOnPosition(xScaled,yScaled);
                }
            }

            onPinchUpdated: {
                if(pinch.scale > 1)
                    zoomValue = 1
                else
                    zoomValue = 2
            }
            onPinchFinished: {
                zoomValue = 0
            }
	    property int zoom: 0
        }
    }
}
