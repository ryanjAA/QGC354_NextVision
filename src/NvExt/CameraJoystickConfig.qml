/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


import QtQuick          2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs  1.2

import QGroundControl               1.0
import QGroundControl.Palette       1.0
import QGroundControl.Controls      1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Controllers   1.0
import QGroundControl.FactSystem    1.0
import QGroundControl.FactControls  1.0

/// Joystick Config
SetupPage {
    id:                 joystickPage
    pageComponent:      pageComponent
    pageName:           qsTr("Camera Joystick")
    pageDescription:    qsTr("Camera Joystick Setup is used to configure the camera joystick.")

    readonly property real _maxButtons: 16

    Connections {
        target: joystickManager
        onAvailableJoysticksChanged: {
            if( joystickManager.joysticks.length == 0 ) {
                summaryButton.checked = true
                setupView.showSummaryPanel()
            }
        }
        onActiveCamJoystickChanged:{
            _activeJoystick:   joystickManager.activeCamJoystick
        }
    }

    Component {
        id: pageComponent

        Item {
            width:  availableWidth
            height: leftColumn.height

            readonly property real labelToMonitorMargin: defaultTextWidth * 3

            property var _activeVehicle:    QGroundControl.multiVehicleManager.activeVehicle
            property var _activeJoystick:   joystickManager.activeCamJoystick

            // Live axis monitor control component
            Component {
                id: axisMonitorDisplayComponent

                Item {
                    property int axisValue: 0
                    property int deadbandValue: 0
                    property bool narrowIndicator: false
                    property color deadbandColor: "#8c161a"

                    property color          __barColor:             qgcPal.windowShade

                    // Bar
                    Rectangle {
                        id:                     bar
                        anchors.verticalCenter: parent.verticalCenter
                        width:                  parent.width
                        height:                 parent.height / 2
                        color:                  __barColor
                    }

                    // Deadband
                    Rectangle {
                        id:                     deadbandBar
                        anchors.verticalCenter: parent.verticalCenter
                        x:                      _deadbandPosition
                        width:                  _deadbandWidth
                        height:                 parent.height / 2
                        color:                  deadbandColor
                        visible:                true//.deadbandToggle

                        property real _percentDeadband:    ((2 * deadbandValue) / (32768.0 * 2))
                        property real _deadbandWidth:   parent.width * _percentDeadband
                        property real _deadbandPosition:   (parent.width - _deadbandWidth) / 2
                    }

                    // Center point
                    Rectangle {
                        anchors.horizontalCenter:   parent.horizontalCenter
                        width:                      defaultTextWidth / 2
                        height:                     parent.height
                        color:                      qgcPal.window
                    }

                    // Indicator
                    Rectangle {
                        anchors.verticalCenter: parent.verticalCenter
                        width:                  parent.narrowIndicator ?  height/6 : height
                        height:                 parent.height * 0.75
                        x:                      (false ? (parent.width - _indicatorPosition) : _indicatorPosition) - (width / 2)
                        radius:                 width / 2
                        color:                  qgcPal.text
                        visible:                true

                        property real _percentAxisValue:    ((axisValue + 32768.0) / (32768.0 * 2))
                        property real _indicatorPosition:   parent.width * _percentAxisValue
                    }

                    ColorAnimation {
                        id:         barAnimation
                        target:     bar
                        property:   "color"
                        from:       "yellow"
                        to:         __barColor
                        duration:   1500
                    }
                }
            } // Component - axisMonitorDisplayComponent

            // Main view Qml starts here

            // Left side column
            Column {
                id:                     leftColumn
                anchors.rightMargin:    ScreenTools.defaultFontPixelWidth
                anchors.left:           parent.left
                anchors.right:          parent.right
                spacing:                10

                // Attitude Controls
                Column {
                    width:      parent.width
                    spacing:    5

                    QGCLabel { text: qsTr("Attitude Controls") }

                    Item {
                        width:  parent.width
                        height: defaultTextHeight * 2

                        QGCLabel {
                            id:     rollLabel
                            width:  defaultTextWidth * 10
                            text:   qsTr("Roll/Yaw")
                        }

                        Loader {
                            id:                 rollLoader
                            anchors.left:       rollLabel.right
                            anchors.right:      parent.right
                            height:             ScreenTools.defaultFontPixelHeight
                            width:              100
                            sourceComponent:    axisMonitorDisplayComponent

                            property real defaultTextWidth: ScreenTools.defaultFontPixelWidth
                        }

                        Connections {
                            target: _activeJoystick

                            onManualCamControl: rollLoader.item.axisValue = cam_roll_yaw*32768.0
                            }
                    }

                    Item {
                        width:  parent.width
                        height: defaultTextHeight * 2

                        QGCLabel {
                            id:     pitchLabel
                            width:  defaultTextWidth * 10
                            text:   qsTr("Pitch")
                        }

                        Loader {
                            id:                 pitchLoader
                            anchors.left:       pitchLabel.right
                            anchors.right:      parent.right
                            height:             ScreenTools.defaultFontPixelHeight
                            width:              100
                            sourceComponent:    axisMonitorDisplayComponent

                            property real defaultTextWidth: ScreenTools.defaultFontPixelWidth
                        }

                        Connections {
                            target: _activeJoystick

                            onManualCamControl: pitchLoader.item.axisValue = cam_pitch*32768.0
                        }
                    }
                } // Column - Attitude Control labels

                Rectangle {
                    width:          parent.width
                    height:         1
                    border.color:   qgcPal.text
                    border.width:   1
                }

                // Settings
                Row {
                    width:      parent.width
                    spacing:    ScreenTools.defaultFontPixelWidth

                    // Left column settings
                    Column {
                        width:      parent.width / 2
                        spacing:    ScreenTools.defaultFontPixelHeight

                        QGCLabel { text: qsTr("Additional Joystick settings:") }

                        Column {
                            width:      parent.width
                            spacing:    ScreenTools.defaultFontPixelHeight


                            QGCCheckBox {
                                id:         enabledCheckBox
                                checked:    _activeVehicle.joystickCamEnabled
                                text:       qsTr("Enable Camera joystick input")
                                onClicked:  _activeVehicle.joystickCamEnabled = checked
                                Component.onCompleted: checked = _activeVehicle.joystickCamEnabled

                                Connections {
                                    target: _activeVehicle

                                    onJoystickCamEnabledChanged: {
                                        enabledCheckBox.checked = _activeVehicle.joystickCamEnabled
                                    }
                                }
                            }

                            Row {
                                width:      parent.width
                                spacing:    ScreenTools.defaultFontPixelWidth

                                QGCLabel {
                                    id:                 activeJoystickLabel
                                    anchors.baseline:   joystickCombo.baseline
                                    text:               qsTr("Active Camera joystick:")
                                }

                                QGCComboBox {
                                    id:                 joystickCombo
                                    width:              parent.width - activeJoystickLabel.width - parent.spacing
                                    model:              joystickManager.joystickNames

                                    onActivated: joystickManager.activeCamJoystickName = textAt(index)

                                    Component.onCompleted: {
                                        var index = joystickCombo.find(joystickManager.activeCamJoystickName)
                                        if (index == -1) {
                                            console.warn(qsTr("Active joystick name not in combo"), joystickManager.activeCamJoystickName)
                                        } else {
                                            joystickCombo.currentIndex = index
                                        }
                                    }

                                    Connections {
                                        target: joystickManager
                                        onAvailableJoysticksChanged: {
                                            var index = joystickCombo.find(joystickManager.activeCamJoystickName)
                                            if (index >= 0) {
                                                joystickCombo.currentIndex = index
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    } // Column - left column

                    // Right column settings
                    Column {
                        width:      parent.width / 2
                        spacing:    ScreenTools.defaultFontPixelHeight

                        Connections {
                            target: _activeJoystick

                            onRawButtonPressedChanged: {
                                if (buttonActionRepeater.itemAt(index)) {
                                    buttonActionRepeater.itemAt(index).pressed = pressed
                                }                               
                            }
                        }

                        QGCLabel { text: qsTr("Button actions:") }

                        Column {
                            width:      parent.width
                            spacing:    ScreenTools.defaultFontPixelHeight / 3

                            Repeater {
                                id:     buttonActionRepeater
                                model:  _activeJoystick ? Math.min(_activeJoystick.totalButtonCount - 4, _maxButtons) : 0

                                Row {
                                    spacing: ScreenTools.defaultFontPixelWidth
                                    visible: (_activeVehicle.manualControlReservedButtonCount == -1 ? false : modelData >= _activeVehicle.manualControlReservedButtonCount) && !_activeVehicle.supportsJSButton

                                    property bool pressed

                                    QGCCheckBox {
                                        anchors.verticalCenter:     parent.verticalCenter
                                        checked:                    joystickManager.cameraManagement ? joystickManager.cameraManagement.buttonCamActions[modelData] != "" : false

                                        onClicked: joystickManager.cameraManagement.setButtonCamAction(modelData, checked ? buttonActionCombo.textAt(buttonActionCombo.currentIndex) : "")
                                    }

                                    Rectangle {
                                        anchors.verticalCenter:     parent.verticalCenter
                                        width:                      ScreenTools.defaultFontPixelHeight * 1.5
                                        height:                     width
                                        border.width:               1
                                        border.color:               qgcPal.text
                                        color:                      pressed ? qgcPal.buttonHighlight : qgcPal.button

                                        QGCLabel {
                                            anchors.fill:           parent
                                            color:                  pressed ? qgcPal.buttonHighlightText : qgcPal.buttonText
                                            horizontalAlignment:    Text.AlignHCenter
                                            verticalAlignment:      Text.AlignVCenter
                                            text:                   modelData
                                        }
                                    }

                                    QGCComboBox {
                                        id:             buttonActionCombo
                                        width:          ScreenTools.defaultFontPixelWidth * 20
                                        model:          joystickManager.cameraManagement ? joystickManager.cameraManagement.cam_actions : 0

                                        onActivated:            joystickManager.cameraManagement.setButtonCamAction(modelData, textAt(index))
                                        Component.onCompleted:  currentIndex = find(joystickManager.cameraManagement.buttonCamActions[modelData])
                                    }
                                }
                            } // Repeater                        
                        } // Column
                    } // Column - right setting column
                } // Row - Settings
            } // Column - Left Main Column
        } // Item
    } // Component - pageComponent
} // SetupPage


