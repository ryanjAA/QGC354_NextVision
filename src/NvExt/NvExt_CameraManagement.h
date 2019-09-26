#ifndef CAMERAMANAGEMENT_H
#define CAMERAMANAGEMENT_H

#include <QObject>
#include <QWidget>
#include <QVector>

#include "Joystick.h"
#include "QGCMapEngineManager.h"
#include "QGCMapEngine.h"
#include "QGeoMapReplyQGC.h"
#include "QGeoTileFetcherQGC.h"

class CameraManagement : public QObject
{
    Q_OBJECT

    /* enums */

    typedef enum
    {
        JoyBtnReleased,
        JoyBtnPressed
    }JoyBtnState;

    /* Mavlink Extension Arguments for Set System Mode Command */
    typedef enum
    {
        MavExtCmdArg_Stow = 0,
        MavExtCmdArg_Pilot,
        MavExtCmdArg_Hold,
        MavExtCmdArg_Observation,
        MavExtCmdArg_LocalPosition,
        MavExtCmdArg_GlobalPosition,
        MavExtCmdArg_GRR,
        MavExtCmdArg_TrackOnPosition,
    }MavlinkExtSetSystemModeArgs;

    /* Mavlink Extension Arguments for Enable/Disable */
    typedef enum
    {
        MavExtCmdArg_Disable = 0,
        MavExtCmdArg_Enable,
        MavExtCmdArg_Toggle
    }MavlinkExtEnDisArgs;

    /* Mavlink Extension Arguments for Set Sensor Command */
    typedef enum
    {
        MavExtCmdArg_DaySensor = 0,
        MavExtCmdArg_IrSensor,
        MavExtCmdArg_ToggleSensor
    }MavlinkExtSetSensorArgs;

    /* Mavlink Extension Arguments for Set Sharpness Command */
    typedef enum
    {
        MavExtCmdArg_NoSharpnessBoost = 0,
        MavExtCmdArg_LowSharpnessBoost,
        MavExtCmdArg_HighSharpnessBoost
    }MavlinkExtSetSharpnessArgs;

    /* Mavlink Extension Arguments for Set Gimbal Command */
    typedef enum
    {
        MavExtCmdArg_ZoomStop = 0,
        MavExtCmdArg_ZoomIn,
        MavExtCmdArg_ZoomOut
    }MavlinkExtSetGimbalArgs;

    /* Mavlink Extension Arguments for Set Polarity Command */
    typedef enum
    {
        MavExtCmdArg_WhiteHot = 0,
        MavExtCmdArg_BlackHot,
        MavExtCmdArg_TogglePolarity,
    }MavlinkExtSetPolarityArgs;
private:

    void _terrainDataReceived(bool success, QList<double> heights);
    QGeoCoordinate _coord;
    double gndCrsAltitude = 0.0;

public:

    /* Mavlink Extension Commands */
    typedef enum
    {
        MavExtCmd_SetSystemMode = 0,
        MavExtCmd_TakeSnapShot,
        MavExtCmd_SetRecordState,
        MavExtCmd_SetSensor,
        MavExtCmd_SetFOV,
        MavExtCmd_SetSharpness,
        MavExtCmd_SetGimbal,
        MavExtCmd_DoBIT,
        MavExtCmd_SetIrPolarity,
        MavExtCmd_SetSingleYawMode,
        MavExtCmd_SetFollowMode,
        MavExtCmd_DoNUC,
        MavExtCmd_SetReportInterval,
        MavExtCmd_ClearRetractLock,
        MavExtCmd_SetSystemTime,
        MavExtCmd_SetIrColor,
        MavExtCmd_SetJoystickMode,
        MavExtCmd_SetGroundCrossingAlt,
        MavExtCmd_SetRollDerot,
        MavExtCmd_SetLaser,
        MavExtCmd_Reboot,
        MavExtCmd_ReconfVideo,
        MavExtCmd_SetTrackerIcon,
        MavExtCmd_SetZoom,
        MavExtCmd_FreezeVideo,
        MavExtCmd_SetColibExtMode,
        MavExtCmd_PilotView,
        MavExtCmd_RvtPostion,
    }MavlinkExtCmd;

    explicit CameraManagement(QObject *parent = nullptr,MultiVehicleManager *multiVehicleManager = nullptr, JoystickManager *joystickManager = nullptr);
    QStringList cam_actions(void);
    void sendGimbalCommand(float cam_roll_yaw,float cam_pitch,int zoom_value);

    QVariantList buttonCamActions(void);
    QStringList _camButtonActionsMap;
    JoyBtnState _camButtonFuncState[32];
    int         _camButtonFuncValue[32];

    Q_INVOKABLE void setButtonCamAction(int button, const QString& action);
    Q_PROPERTY(QVariantList buttonCamActions READ buttonCamActions NOTIFY buttonCamActionsChanged)
    Q_PROPERTY(QStringList cam_actions READ cam_actions CONSTANT)
    void addTileToCahce(QString tile_hash, QByteArray tile_data);
    void getAltAtCoord(float lat,float lon);

    Q_INVOKABLE void pointToCoordinate(float lat,float lon);
    Q_INVOKABLE void trackOnPosition(float posX,float posY);
    Q_INVOKABLE void setSysModeObsCommand(void);
    Q_INVOKABLE void setSysModeGrrCommand(void);
    Q_INVOKABLE void setSysModeHoldCommand(void);
    Q_INVOKABLE void setSysModePilotCommand(void);
    Q_INVOKABLE void setSysModeStowCommand(void);
    Q_INVOKABLE void setSysModeRetractCommand(void);
    Q_INVOKABLE void setSysModeRetractUnlockCommand(void);
    Q_INVOKABLE void setSysSensorToggleCommand(void);
    Q_INVOKABLE void setSysSensorDayCommand(void);
    Q_INVOKABLE void setSysSensorIrCommand(void);
    Q_INVOKABLE void setSysIrPolarityToggleCommand(void);
    Q_INVOKABLE void setSysIrNUCCommand(void);
    Q_INVOKABLE void setSysRecToggleCommand(void);
    Q_INVOKABLE void setSysSnapshotCommand(void);

protected:
    MultiVehicleManager*    _multiVehicleManager;
    Vehicle*                activeVehicle;
    JoystickManager*        _joystickManager;
    Joystick*               activeJoystick;
    void    _saveSettings(void);
    void    _loadSettings(void);

private:
    static const char* _nvExtSettingsGroup;
    static const char* _btnActionSetting;
    void doCamAction(QString buttonAction, bool pressed, int buttonIndex);
    bool doBtnFuncToggle(bool pressed, int buttonIndex);
    MavlinkExtSetGimbalArgs  getZoomValue(quint16 buttons);
    void sendMavCommandLong(MAV_CMD command,  float param1,   float param2,   float param3,
                            float param4, float param5,   float param6,   float param7);

signals:
    void buttonCamActionsChanged(QVariantList actions);

public slots:
    void _activeVehicleChanged(Vehicle* activeVehicle);
    void _activeJoystickChanged(Joystick* joystick);
    void manualCamControl(float cam_pitch, float cam_roll_yaw,quint16 buttons);

public slots:
};

#endif // CAMERAMANAGEMENT_H
