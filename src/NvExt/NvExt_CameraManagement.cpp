#include "NvExt_CameraManagement.h"
#include "../Joystick/JoystickManager.h"
#include "../Terrain/TerrainQuery.h"
#include "QGCApplication.h"
#include "TerrainQuery.h"
#include "SettingsManager.h"


Q_GLOBAL_STATIC(TerrainTileManager, _terrainTileManager)

const char* CameraManagement::_nvExtSettingsGroup =                  "NvExtension";
const char* CameraManagement::_btnActionSetting =                    "BtnAction%1";

CameraManagement::CameraManagement(QObject *parent,MultiVehicleManager *multiVehicleManager, JoystickManager *joystickManager) : QObject(parent),_multiVehicleManager(NULL),_joystickManager(NULL),activeVehicle(NULL),activeJoystick(NULL)
{
    this->_multiVehicleManager = multiVehicleManager;
    this->_joystickManager = joystickManager;
    activeVehicle = _multiVehicleManager->activeVehicle();
    connect(_multiVehicleManager, &MultiVehicleManager::activeVehicleChanged, this, &CameraManagement::_activeVehicleChanged);

    /* connect the tile loaded signal to the cache worker */
    QGCMapEngine *map_engine = getQGCMapEngine();
    QGCCacheWorker *worker = &map_engine->_worker;
    connect(worker, &QGCCacheWorker::tileLoaded, this, &CameraManagement::addTileToCahce);
}

QStringList CameraManagement::cam_actions(void)
{
    QStringList list = {QT_TR_NOOP("Zoom In"),          QT_TR_NOOP("Zoom Out"),               QT_TR_NOOP("NUC"),
                        QT_TR_NOOP("Day/IR"),           QT_TR_NOOP("White Hot / Black Hot"),  QT_TR_NOOP("Stow"),
                        QT_TR_NOOP("Pilot"),            QT_TR_NOOP("Retract"),                QT_TR_NOOP("BIT"),
                        QT_TR_NOOP("Hold Coordinate"),  QT_TR_NOOP("Observation"),            QT_TR_NOOP("GRR"),
                        QT_TR_NOOP("Record"),           QT_TR_NOOP("Image Capture"),          QT_TR_NOOP("Single Yaw"),
                        QT_TR_NOOP("Follow")};
    return list;
}

QVariantList CameraManagement::buttonCamActions(void)
{
    QVariantList list;
    for (int button=0; button<activeJoystick->totalButtonCount(); button++) {
        list += QVariant::fromValue(_camButtonActionsMap[button]);
    }
    return list;
}

void CameraManagement::setButtonCamAction(int button, const QString& action)
{
    _camButtonActionsMap[button] = action;

    _saveSettings();
    emit buttonCamActionsChanged(buttonCamActions());
}

void CameraManagement::_activeVehicleChanged(Vehicle* activeVehicle)
{
    this->activeVehicle = activeVehicle;
    if(activeVehicle){
        float time = QDateTime::currentSecsSinceEpoch();
        /* Sending the system time to the vehicle */
        sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_SetSystemTime,time,0,0,0,0,0);

        /* load all elevation tiles to cache when the Vehicle is connected */
        QGCLoadElevationTileSetsTask* taskSave = new QGCLoadElevationTileSetsTask();
        getQGCMapEngine()->addTask(taskSave);
    }
}

void CameraManagement::_activeJoystickChanged(Joystick* joystick)
{
    if(!joystick)
        return;

    this->activeJoystick = joystick;

    /* load the camera joystick settings */
    _loadSettings();

    /* clear the button functions state var */
    memset(_camButtonFuncState,0,sizeof(_camButtonFuncState));
    /* clear the button values var */
    memset(_camButtonFuncValue,0,sizeof(_camButtonFuncValue));
}

void CameraManagement::manualCamControl(float cam_pitch, float cam_roll_yaw,quint16 buttons)
{
    if (!activeVehicle) {
        return;
    }

    if (!activeVehicle->priorityLink()) {
        return;
    }

    int zoomValue = 0;
    /* call the button functions for each button */
    for (int buttonIndex=0; buttonIndex<activeJoystick->totalButtonCount(); buttonIndex++)
    {
        bool button_value = (buttons & (1 << buttonIndex)) ? true :false;
        doCamAction(_camButtonActionsMap[buttonIndex],button_value,buttonIndex);

    }

    /* Calculating the zoom value */
    zoomValue = getZoomValue(buttons);

    /* send the gimbal command to the system */
    sendGimbalCommand(cam_roll_yaw,cam_pitch,zoomValue);

}


bool CameraManagement::doBtnFuncToggle(bool pressed, int buttonIndex)
{
    switch ( _camButtonFuncState[buttonIndex] )
    {
        case JoyBtnReleased:
        {
            if ( pressed )
            {
                _camButtonFuncState[buttonIndex] = JoyBtnPressed;
            }
        }
        break;
        case JoyBtnPressed:
        {
            if ( !pressed )
            {
                _camButtonFuncValue[buttonIndex] ^= 1;
                _camButtonFuncState[buttonIndex] = JoyBtnReleased;
                return true;
            }
        }
        break;
    }
    return false;
}

void CameraManagement::doCamAction(QString buttonAction, bool pressed, int buttonIndex)
{
    bool doAction = doBtnFuncToggle(pressed,buttonIndex);

    if ( buttonAction.isEmpty() )
        return;

    if (!activeVehicle || !activeVehicle->joystickEnabled()) {
        return;
    }

    if (buttonAction == "Day/IR")
    {
        /* Day/IR toggle */
        if (doAction)
            sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_SetSensor,_camButtonFuncValue[buttonIndex],0,0,0,0,0);
    } else if (buttonAction == "White Hot / Black Hot")
    {
        /* Polarity Toggle */
        if (doAction)
            sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_SetIrPolarity,_camButtonFuncValue[buttonIndex],0,0,0,0,0);
    }else if(buttonAction == "Image Capture"){
        /* Image Capture */
        if (doAction)
            sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_TakeSnapShot,0,0,0,0,0,0);
    }else if(buttonAction == "Single Yaw"){
        /* Single Yaw */
        if (doAction)
            sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_SetSingleYawMode,_camButtonFuncValue[buttonIndex],0,0,0,0,0);
    }else if(buttonAction == "BIT"){
        /* Do BIT */
        if (doAction)
            sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_DoBIT,0,0,0,0,0,0);
    }else if(buttonAction == "Follow"){
        /* Follow Target */
        if (doAction)
            sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_SetFollowMode,_camButtonFuncValue[buttonIndex],0,0,0,0,0);
    }else if(buttonAction == "GRR"){
        /* GRR */
        if (doAction)
            sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_SetSystemMode,MavExtCmdArg_GRR,0,0,0,0,0);
    }else if(buttonAction =="NUC"){
        /* NUC */
        if (doAction)
            sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_DoNUC,0,0,0,0,0,0);
    }else if(buttonAction =="Stow"){
        /* Stow */
        if (doAction)
            sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_SetSystemMode,MavExtCmdArg_Stow,0,0,0,0,0);
    }else if(buttonAction =="Pilot"){
        /* Pilot */
        if (doAction)
            sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_SetSystemMode,MavExtCmdArg_Pilot,0,0,0,0,0);
    }else if(buttonAction =="Retract"){
         /* Retract */
        if (doAction)
            sendMavCommandLong(MAV_CMD_DO_MOUNT_CONTROL,_camButtonFuncValue[buttonIndex],0,0,0,0,0,0);
    }else if(buttonAction =="Hold Coordinate"){
        /* Hold Coordinate */
        if (doAction)
            sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_SetSystemMode,MavExtCmdArg_Hold,0,0,0,0,0);
    }else if(buttonAction =="Observation"){
        /* Observation */
        if (doAction)
            sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_SetSystemMode,MavExtCmdArg_Observation,0,0,0,0,0);
    }else if(buttonAction =="Record"){
        /* Record */
        if (doAction)
            sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_SetRecordState,_camButtonFuncValue[buttonIndex],0,0,0,0,0);
    }
}

void CameraManagement::_saveSettings(void)
{
    QSettings settings;
    settings.beginGroup(_nvExtSettingsGroup);
    settings.beginGroup(activeJoystick->name());

    for (int button=0; button<activeJoystick->totalButtonCount(); button++)
        settings.setValue(QString(_btnActionSetting).arg(button), _camButtonActionsMap[button]);
}

void CameraManagement::_loadSettings(void)
{
    QSettings   settings;
    settings.beginGroup(_nvExtSettingsGroup);
    settings.beginGroup(activeJoystick->name());

    _camButtonActionsMap.clear();
    for (int button=0; button<activeJoystick->totalButtonCount(); button++)
        _camButtonActionsMap << settings.value(QString(_btnActionSetting).arg(button), QString()).toString();
}

/* Returning the zoom value according to the buttons pressed */
CameraManagement::MavlinkExtSetGimbalArgs CameraManagement::getZoomValue(quint16 buttons)
{
    int zoomInVal = 0;
    int zoomOutVal = 0;

    /* call the button functions for each button */
    for (int buttonIndex=0; buttonIndex<activeJoystick->totalButtonCount(); buttonIndex++)
    {
        bool button_value = (buttons & (1 << buttonIndex)) ? true :false;
        if((_camButtonActionsMap[buttonIndex] == "Zoom In") && (button_value == 1))
            zoomInVal = 1;
        else if((_camButtonActionsMap[buttonIndex] == "Zoom Out") && (button_value == 1))
             zoomOutVal = 1;
    }

    if( (zoomInVal == 0 && zoomOutVal == 0) || (zoomInVal == 1 && zoomOutVal == 1) )
        return MavExtCmdArg_ZoomStop;
    else if(zoomInVal == 1)
        return MavExtCmdArg_ZoomIn;
    else
        return MavExtCmdArg_ZoomOut;
}

/* Sending gimbal Command Messages */
void CameraManagement::sendGimbalCommand(float cam_roll_yaw,float cam_pitch,int zoom_value)
{
    if(!activeVehicle)
        return;

    if (!activeVehicle->priorityLink())
        return;

    /* check if virtual joystick is enabled */
    if ( qgcApp()->toolbox()->settingsManager()->appSettings()->virtualJoystick()->rawValue().toBool() == true )
    {
        mavlink_message_t message;

        mavlink_msg_command_long_pack_chan(1,
                                       0,
                                       activeVehicle->priorityLink()->mavlinkChannel(),
                                       &message,1,0,MAV_CMD_DO_DIGICAM_CONTROL,0,MavExtCmd_SetGimbal,cam_roll_yaw,cam_pitch,zoom_value,this->gndCrsAltitude,0,0);
        activeVehicle->sendMessageOnLink(activeVehicle->priorityLink(), message);
    }
}

/* Sending Mavlink Command Long Messages */
void CameraManagement::sendMavCommandLong(MAV_CMD command,  float param1,   float param2,   float param3,
                                          float param4,     float param5,   float param6,   float param7)
{
    if(!activeVehicle)
        return;

    if (!activeVehicle->priorityLink())
        return;

    activeVehicle->sendMavCommand(activeVehicle->defaultComponentId(),
                   command,
                   true,
                   param1, param2, param3, param4, param5, param6, param7);
}



void CameraManagement::addTileToCahce(QString tile_hash, QByteArray tile_data)
{
    _terrainTileManager->addTileToCahce(tile_data,tile_hash);
}

void CameraManagement::getAltAtCoord(float lat,float lon)
{
    double terrainAltitude;
    QGeoCoordinate coord;

    coord.setLatitude(lat);
    coord.setLongitude(lon);

    /* check if we have this data cached */
    if( _terrainTileManager->requestCahcedData(coord,terrainAltitude) )
        this->gndCrsAltitude = terrainAltitude;     /* save the value, will be transmitted to the TRIP2 in the next Gimbal or GndAlt message */

    /* when the virtual joystick is disabled set gnd crs alt here */
    if ( qgcApp()->toolbox()->settingsManager()->appSettings()->virtualJoystick()->rawValue().toBool() == false )
    {
        mavlink_message_t message;

        if(!activeVehicle)
            return;

        if (!activeVehicle->priorityLink())
            return;

        /* when the virtual joystick is disabled send the ground altitude from here instead */
        mavlink_msg_command_long_pack_chan(1,
                                           0,
                                           activeVehicle->priorityLink()->mavlinkChannel(),
                                           &message,1,0,MAV_CMD_DO_DIGICAM_CONTROL,0,MavExtCmd_SetGroundCrossingAlt,(float)this->gndCrsAltitude,0,0,0,0,0);
        activeVehicle->sendMessageOnLink(activeVehicle->priorityLink(), message);
    }
}

void CameraManagement::pointToCoordinate(float lat,float lon)
{
    double terrainAltitude;

    _coord.setLatitude(lat);
    _coord.setLongitude(lon);

    /* first check if we have this data cached */
    if ( _terrainTileManager->requestCahcedData(_coord,terrainAltitude) == false )
    {
        TerrainAtCoordinateQuery* terrain = new TerrainAtCoordinateQuery(this);
        connect(terrain, &TerrainAtCoordinateQuery::terrainDataReceived, this, &CameraManagement::_terrainDataReceived);
        QList<QGeoCoordinate> rgCoord;
        rgCoord.append(_coord);
        terrain->requestData(rgCoord);
    }
    else
    {
        sendMavCommandLong(MAV_CMD_DO_SET_ROI_LOCATION,0.0,0.0,0.0,0.0,_coord.latitude(),_coord.longitude(),terrainAltitude);
        //qDebug() << "PTC On lat= " << (int)(_coord.latitude() * 10000000.0) << " lon = " << (int)(_coord.longitude() * 10000000.0 )<< " alt = " << _terrainAltitude;
    }
}

void CameraManagement::_terrainDataReceived(bool success, QList<double> heights)
{
    double _terrainAltitude = success ? heights[0] : 0;
    sendMavCommandLong(MAV_CMD_DO_SET_ROI_LOCATION,0.0,0.0,0.0,0.0,_coord.latitude(),_coord.longitude(),_terrainAltitude);
    //qDebug() << "PTC On lat= " << (int)(_coord.latitude() * 10000000.0) << " lon = " << (int)(_coord.longitude() * 10000000.0 )<< " alt = " << _terrainAltitude;
    //sender()->deleteLater();
}

void CameraManagement::trackOnPosition(float posX,float posY)
{
    /* Sending the track on position command */
    sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_SetSystemMode,MavExtCmdArg_TrackOnPosition,posX,posY,0,0,0);
}

void CameraManagement::setSysModeGrrCommand()
{
    /* Sending the OBS command */
    sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_SetSystemMode,MavExtCmdArg_GRR,0,0,0,0,0);
}

void CameraManagement::setSysModeObsCommand()
{
    /* Sending the GRR command */
    sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_SetSystemMode,MavExtCmdArg_Observation,0,0,0,0,0);
}

void CameraManagement::setSysModeHoldCommand()
{
    /* Sending the Hold command */
    sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_SetSystemMode,MavExtCmdArg_Hold,0,0,0,0,0);
}

void CameraManagement::setSysModePilotCommand()
{
    /* Sending the Pilot command */
    sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_SetSystemMode,MavExtCmdArg_Pilot,0,0,0,0,0);
}

void CameraManagement::setSysModeStowCommand()
{
    /* Sending the Stow command */
    sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_SetSystemMode,MavExtCmdArg_Stow,0,0,0,0,0);
}

void CameraManagement::setSysModeRetractCommand()
{
    /* Sending the Retract command */
    sendMavCommandLong(MAV_CMD_DO_MOUNT_CONTROL,0,0,0,0,0,0,0);
}

void CameraManagement::setSysModeRetractUnlockCommand()
{
    /* Sending retract release command */
    sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_ClearRetractLock,0,0,0,0,0,0);
}

void CameraManagement::setSysSensorToggleCommand()
{
    /* Toggle the system sensor */
    sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_SetSensor,MavExtCmdArg_ToggleSensor,0,0,0,0,0);
}

void CameraManagement::setSysSensorDayCommand(void)
{
    /* Set the system sensor */
    sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_SetSensor,MavExtCmdArg_DaySensor,0,0,0,0,0);

}

void CameraManagement::setSysSensorIrCommand(void)
{
    /* Set the system sensor */
    sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_SetSensor,MavExtCmdArg_IrSensor,0,0,0,0,0);

}

void CameraManagement::setSysIrPolarityToggleCommand(void)
{
    /* Set the system sensor */
    sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_SetIrPolarity,MavExtCmdArg_TogglePolarity,0,0,0,0,0);

}
void CameraManagement::setSysIrNUCCommand(void)
{
    /* Set the system sensor */
    sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_DoNUC,0,0,0,0,0,0);

}
void CameraManagement::setSysRecToggleCommand(void)
{
    /* Set the system sensor */
    sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_SetRecordState,MavExtCmdArg_Toggle,0,0,0,0,0);

}
void CameraManagement::setSysSnapshotCommand(void)
{
    /* Set the system sensor */
    sendMavCommandLong(MAV_CMD_DO_DIGICAM_CONTROL,MavExtCmd_TakeSnapShot,0,0,0,0,0,0);

}
