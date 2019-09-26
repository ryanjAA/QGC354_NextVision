#pragma once
// MESSAGE MAVLINK NvExt Line Of Sight Report

#define MAVLINK_MSG_NVEXT_LOS_REPORT 248

MAVPACKED(
typedef struct __mavlink_nvext_los_report_t {
    uint16_t            report_type;
    float               los_x;
    float               los_y;
    float               los_z;
    float               los_upper_left_corner_lat;
    float               los_upper_left_corner_lon;
    float               los_upper_right_corner_lat;
    float               los_upper_right_corner_lon;
    float               los_lower_right_corner_lat;
    float               los_lower_right_corner_lon;
    float               los_lower_left_corner_lat;
    float               los_lower_left_corner_lon;
    float               los_elevation;
    float               los_azimuth;
}) mavlink_nvext_los_report_t;

#define MAVLINK_MSG_NVEXT_LOS_REPORT_LEN 54
#define MAVLINK_MSG_NVEXT_LOS_REPORT_MIN_LEN 54
#define MAVLINK_MSG_NVEXT_LOS_REPORT_CRC 8

#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_NVEXT_LOS_REPORT { \
    248, \
    "V2_EXTENSION", \
    5, \
    {  { "target_network", NULL, MAVLINK_TYPE_UINT8_T, 0, 2, offsetof(mavlink_v2_extension_t, target_network) }, \
         { "target_system", NULL, MAVLINK_TYPE_UINT8_T, 0, 3, offsetof(mavlink_v2_extension_t, target_system) }, \
         { "target_component", NULL, MAVLINK_TYPE_UINT8_T, 0, 4, offsetof(mavlink_v2_extension_t, target_component) }, \
         { "message_type", NULL, MAVLINK_TYPE_UINT16_T, 0, 0, offsetof(mavlink_v2_extension_t, message_type) }, \
         { "payload", NULL, MAVLINK_TYPE_UINT8_T, 249, 5, offsetof(mavlink_v2_extension_t, payload) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_NVEXT_LOS_REPORT { \
    "NVEXT_LOS_REPORT", \
    14, \
    {  { "report_type", NULL, MAVLINK_TYPE_UINT16_T, 0, 0, offsetof(mavlink_nvext_los_report_t, report_type) }, \
         { "los_x", NULL, MAVLINK_TYPE_FLOAT, 0, 2, offsetof(mavlink_nvext_los_report_t, los_x) }, \
         { "los_y", NULL, MAVLINK_TYPE_FLOAT, 0, 6, offsetof(mavlink_nvext_los_report_t, los_y) }, \
         { "los_z", NULL, MAVLINK_TYPE_FLOAT, 0, 10, offsetof(mavlink_nvext_los_report_t, los_z) }, \
         { "los_upper_left_corner_lat", NULL, MAVLINK_TYPE_FLOAT, 0, 14, offsetof(mavlink_nvext_los_report_t, los_upper_left_corner_lat) }, \
         { "los_upper_left_corner_lon", NULL, MAVLINK_TYPE_FLOAT, 0, 18, offsetof(mavlink_nvext_los_report_t, los_upper_left_corner_lon) }, \
         { "los_upper_right_corner_lat", NULL, MAVLINK_TYPE_FLOAT, 0, 22, offsetof(mavlink_nvext_los_report_t, los_upper_right_corner_lat) }, \
         { "los_upper_right_corner_lon", NULL, MAVLINK_TYPE_FLOAT, 0, 26, offsetof(mavlink_nvext_los_report_t, los_upper_right_corner_lon) }, \
         { "los_lower_right_corner_lat", NULL, MAVLINK_TYPE_FLOAT, 0, 30, offsetof(mavlink_nvext_los_report_t, los_lower_right_corner_lat) }, \
         { "los_lower_right_corner_lon", NULL, MAVLINK_TYPE_FLOAT, 0, 34, offsetof(mavlink_nvext_los_report_t, los_lower_right_corner_lon) }, \
         { "los_lower_left_corner_lat", NULL, MAVLINK_TYPE_FLOAT, 0, 38, offsetof(mavlink_nvext_los_report_t, los_lower_left_corner_lat) }, \
         { "los_lower_left_corner_lon", NULL, MAVLINK_TYPE_FLOAT, 0, 42, offsetof(mavlink_nvext_los_report_t, los_lower_left_corner_lon) }, \
         { "los_elevation", NULL, MAVLINK_TYPE_FLOAT, 0, 46, offsetof(mavlink_nvext_los_report_t, los_elevation) }, \
         { "los_azimuth", NULL, MAVLINK_TYPE_FLOAT, 0, 50, offsetof(mavlink_nvext_los_report_t, los_azimuth) }, \
         } \
}
#endif


// MESSAGE MAVLINK_MSG_NVEXT_LOS_REPORT UNPACKING


/**
 * @brief Get field report_type from mavlink_nvext_los_report message
 *
 * @return Report type
 */
static inline uint16_t mavlink_nvext_los_report_get_report_type(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint16_t(msg,  0);
}

/**
 * @brief Get field los_x from mavlink_nvext_los_report message
 *
 * @return Los_x
 */
static inline float mavlink_nvext_los_report_get_los_x(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  2);
}

/**
 * @brief Get field los_y from mavlink_nvext_los_report message
 *
 * @return Los_ y
 */
static inline float mavlink_nvext_los_report_get_los_y(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  6);
}

/**
 * @brief Get field los_z from mavlink_nvext_los_report message
 *
 * @return los_z
 */
static inline float mavlink_nvext_los_report_get_los_z(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  10);
}


/**
 * @brief Get field los_upper_left_corner_lat from mavlink_nvext_los_report message
 *
 * @return los_upper_left_corner_lat
 */
static inline float mavlink_nvext_los_report_get_los_upper_left_corner_lat(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  14);
}

/**
 * @brief Get field los_upper_left_corner_lon from mavlink_nvext_los_report message
 *
 * @return los_upper_left_corner_lon
 */
static inline float mavlink_nvext_los_report_get_los_upper_left_corner_lon(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  18);
}

/**
 * @brief Get field los_upper_right_corner_lat from mavlink_nvext_los_report message
 *
 * @return los_upper_right_corner_lat
 */
static inline float mavlink_nvext_los_report_get_los_upper_right_corner_lat(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  22);
}

/**
 * @brief Get field los_upper_right_corner_lon from mavlink_nvext_los_report message
 *
 * @return los_upper_right_corner_lon
 */
static inline float mavlink_nvext_los_report_get_los_upper_right_corner_lon(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  26);
}

/**
 * @brief Get field los_lower_right_corner_lat from mavlink_nvext_los_report message
 *
 * @return los_lower_right_corner_lat
 */
static inline float mavlink_nvext_los_report_get_los_lower_right_corner_lat(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  30);
}

/**
 * @brief Get field los_lower_right_corner_lon from mavlink_nvext_los_report message
 *
 * @return los_lower_right_corner_lon
 */
static inline float mavlink_nvext_los_report_get_los_lower_right_corner_lon(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg, 34);
}

/**
 * @brief Get field los_lower_left_corner_lat from mavlink_nvext_los_report message
 *
 * @return los_lower_left_corner_lat
 */
static inline float mavlink_nvext_los_report_get_los_lower_left_corner_lat(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  38);
}

/**
 * @brief Get field los_lower_left_corner_lon from mavlink_nvext_los_report message
 *
 * @return los_lower_left_corner_lon
 */
static inline float mavlink_nvext_los_report_get_los_lower_left_corner_lon(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg, 42);
}

/**
 * @brief Get field los_elevation from mavlink_nvext_los_report message
 *
 * @return los_elevation
 */
static inline float mavlink_nvext_los_report_get_los_elevation(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  46);
}

/**
 * @brief Get field los_azimuth from mavlink_nvext_los_report message
 *
 * @return los_azimuth
 */
static inline float mavlink_nvext_los_report_get_los_azimuth(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  50);
}


/**
 * @brief Decode a v2_extension message into a struct
 *
 * @param msg The message to decode
 * @param nvext_los_report C-struct to decode the message contents into
 */
static inline void mavlink_nvext_los_report_decode(const mavlink_message_t* msg, mavlink_nvext_los_report_t* nvext_los_report)
{
    nvext_los_report->report_type = mavlink_nvext_los_report_get_report_type(msg);
    nvext_los_report->los_x = mavlink_nvext_los_report_get_los_x(msg);
    nvext_los_report->los_y = mavlink_nvext_los_report_get_los_y(msg);
    nvext_los_report->los_z = mavlink_nvext_los_report_get_los_z(msg);
    nvext_los_report->los_upper_left_corner_lat = mavlink_nvext_los_report_get_los_upper_left_corner_lat(msg);
    nvext_los_report->los_upper_left_corner_lon = mavlink_nvext_los_report_get_los_upper_left_corner_lon(msg);
    nvext_los_report->los_upper_right_corner_lat = mavlink_nvext_los_report_get_los_upper_right_corner_lat(msg);
    nvext_los_report->los_upper_right_corner_lon = mavlink_nvext_los_report_get_los_upper_right_corner_lon(msg);
    nvext_los_report->los_lower_right_corner_lat = mavlink_nvext_los_report_get_los_lower_right_corner_lat(msg);
    nvext_los_report->los_lower_right_corner_lon = mavlink_nvext_los_report_get_los_lower_right_corner_lon(msg);
    nvext_los_report->los_lower_left_corner_lat = mavlink_nvext_los_report_get_los_lower_left_corner_lat(msg);
    nvext_los_report->los_lower_left_corner_lon = mavlink_nvext_los_report_get_los_lower_left_corner_lon(msg);
    nvext_los_report->los_elevation = mavlink_nvext_los_report_get_los_elevation(msg);
    nvext_los_report->los_azimuth = mavlink_nvext_los_report_get_los_azimuth(msg);
}
