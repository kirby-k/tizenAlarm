/*
 * Copyright (c) 2011 - 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef __TIZEN_APPFW_ALARM_H__
#define __TIZEN_APPFW_ALARM_H__

#include <tizen.h>
#include <time.h>
#include <app_control.h>
#include <notification.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file app_alarm.h
 */

/**
 * @addtogroup CAPI_ALARM_MODULE
 * @{
 */

/**
 * @brief Definition to app_control extra data : the ID of the alarm registered.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define APP_CONTROL_DATA_ALARM_ID "http://tizen.org/appcontrol/data/alarm_id"

/**
 * @brief Enumeration for Alarm Error.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum {
	ALARM_ERROR_NONE = TIZEN_ERROR_NONE,	/**< Successful */
	ALARM_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER,	/**< Invalid parameter */
	ALARM_ERROR_INVALID_TIME = TIZEN_ERROR_APPLICATION | 0x05,	/**< Invalid time */
	ALARM_ERROR_INVALID_DATE = TIZEN_ERROR_APPLICATION | 0x06,	/**< Invalid date */
	ALARM_ERROR_CONNECTION_FAIL = TIZEN_ERROR_APPLICATION | 0x07,	/**< The alarm service connection failed */
	ALARM_ERROR_NOT_PERMITTED_APP = TIZEN_ERROR_APPLICATION | 0x08, /**< Application is not permitted. (Since @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif) */
	ALARM_ERROR_OUT_OF_MEMORY = TIZEN_ERROR_OUT_OF_MEMORY,	/**< Out of memory */
	ALARM_ERROR_PERMISSION_DENIED = TIZEN_ERROR_PERMISSION_DENIED	/**< Permission denied */
} alarm_error_e;


/**
 * @brief Enumeration for Alarm Week Flag, the days of the week.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum {
	ALARM_WEEK_FLAG_SUNDAY = 0x01,	/**< Sunday */
	ALARM_WEEK_FLAG_MONDAY = 0x02,	/**< Monday */
	ALARM_WEEK_FLAG_TUESDAY = 0x04,	/**< Tuesday */
	ALARM_WEEK_FLAG_WEDNESDAY = 0x08,	/**< Wednesday */
	ALARM_WEEK_FLAG_THURSDAY = 0x10,	/**< Thursday */
	ALARM_WEEK_FLAG_FRIDAY = 0x20,	/**< Friday */
	ALARM_WEEK_FLAG_SATURDAY = 0x40	/**< Saturday */
} alarm_week_flag_e;


/**
 * @brief Called once for each scheduled alarm to get the alarm ID.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] alarm_id The alarm ID returned when the alarm is scheduled
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop,
 *         otherwise @c false to break out of the loop
 * @pre   alarm_foreach_registered_alarm() will invoke this callback to get all the registered alarm IDs.
 * @see   alarm_foreach_registered_alarm()
 */
typedef bool (*alarm_registered_alarm_cb)(int alarm_id, void *user_data);


/**
 * @brief Sets an alarm to be triggered after a specific time.
 * @details The alarm will first go off @a delay seconds later and then will go off every certain amount of time defined using @a period seconds. To cancel the alarm, call alarm_cancel() with @a alarm_id.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/alarm.set
 * @privilege %http://tizen.org/privilege/appmanager.launch
 * @remarks If the application is uninstalled after setting an alarm, the alarm is cancelled automatically.
 *          If the operation of @a app_control is not specified, #APP_CONTROL_OPERATION_DEFAULT is used for the launch request.
 *          If the operation of @a app_control is #APP_CONTROL_OPERATION_DEFAULT, the package information is mandatory to explicitly launch the application.
 *          Since @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif, this api does not support exact period and delay for minimizing the wakeups of the device. The system can adjust when the alarm expires.
 *          If you want to trigger an alarm exactly, see @see alarm_schedule_once_after_delay()
 *          Since @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif If @a app_control is specified with service-application, the application is only allowed to run on which has Background Category.
 *          Since @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif If the appid of @a app_control is not specified, this api is not allowed. In other words, the explicit @a app_control is only allowed.
 *          Since 4.0, %http://tizen.org/privilege/appmanager.launch privilege is additionally required.
 *
 * @param[in] app_control The destination app_control to perform a specific task when the alarm is triggered
 * @param[in] delay The amount of time before the first execution (in seconds). Since @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif, Although this is inexact, the alarm will not fire before this time
 * @param[in] period The amount of time between subsequent alarms (in seconds). Since @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif, This value does not guarantee the accuracy. The actual interval is calculated by the OS. The minimum value is 600sec
 * @param[out] alarm_id The alarm ID that uniquely identifies an alarm
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #ALARM_ERROR_NONE Successful
 * @retval #ALARM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #ALARM_ERROR_INVALID_TIME Triggered time is invalid
 * @retval #ALARM_ERROR_CONNECTION_FAIL Failed to connect to an alarm server
 * @retval #ALARM_ERROR_PERMISSION_DENIED Permission denied
 * @retval #ALARM_ERROR_NOT_PERMITTED_APP @a app_control is not permitted
 * @see alarm_cancel()
 * @see alarm_cancel_all()
 * @see alarm_get_scheduled_date()
 * @see alarm_get_scheduled_period()
 * @see alarm_schedule_once_after_delay()
 */
int alarm_schedule_after_delay(app_control_h app_control, int delay, int period, int *alarm_id);


/**
 * @deprecated Deprecated since @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif. [Use alarm_schedule_once_at_date() instead]
 * @brief Sets an alarm to be triggered at a specific time.
 * @details The @a date describes the time of the first occurrence.
 *          If @a period is bigger than @c 0, the alarm will be scheduled after the @a period time.
 *          If @a period is set to @c 0, the alarm will go off just once without repetition.
 *          To cancel the alarm, call alarm_cancel() with @a alarm_id.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/alarm.set
 * @remarks If application is uninstalled after setting an alarm, the alarm is cancelled automatically.
 *          If the operation of @a app_control is not specified, #APP_CONTROL_OPERATION_DEFAULT is used for the launch request.
 *          If the operation of @a app_control is #APP_CONTROL_OPERATION_DEFAULT, the package information is mandatory to explicitly launch the application.
 *
 * @param[in] app_control The destination app_control to perform specific work when the alarm is triggered
 * @param[in] date The first active alarm time
 * @param[in] period The amount of time between subsequent alarms(in second)
 * @param[out] alarm_id The alarm ID that uniquely identifies an alarm
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #ALARM_ERROR_NONE Successful
 * @retval #ALARM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #ALARM_ERROR_INVALID_DATE Triggered date is invalid
 * @retval #ALARM_ERROR_CONNECTION_FAIL Failed to connect to an alarm server
 * @retval #ALARM_ERROR_PERMISSION_DENIED Permission denied
 * @see alarm_cancel()
 * @see alarm_cancel_all()
 * @see alarm_get_scheduled_date()
 * @see alarm_get_scheduled_period()
 */
int alarm_schedule_at_date(app_control_h app_control, struct tm *date, int period, int *alarm_id) TIZEN_DEPRECATED_API;

/**
 * @brief Sets an alarm to be triggered after a specific time.
 * @details The alarm will go off @a delay seconds later. To cancel the alarm, call alarm_cancel() with @a alarm_id.
 * @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/alarm.set
 * @privilege %http://tizen.org/privilege/appmanager.launch
 * @remarks If the application is uninstalled after setting an alarm, the alarm is cancelled automatically.
 *          If the operation of @a app_control is not specified, #APP_CONTROL_OPERATION_DEFAULT is used for the launch request.
 *          If the operation of @a app_control is #APP_CONTROL_OPERATION_DEFAULT, the package information is mandatory to explicitly launch the application.
 *          If the appid of @a app_control is not specified, this api is not allowed. In other words, the explicit @a app_control is only allowed.
 *          The @a app_control only supports UI application with this api. If @a app_control is not UI application, #ALARM_ERROR_NOT_PERMITTED_APP returned.
 *          When the alarm is expired, Alarm Manager will turn on LCD to prohibit background jobs.
 *          Since 4.0, %http://tizen.org/privilege/appmanager.launch privilege is additionally required.
 *
 * @param[in] app_control The destination app_control to perform a specific task when the alarm is triggered
 * @param[in] delay The amount of time before the execution (in seconds)
 * @param[out] alarm_id The alarm ID that uniquely identifies an alarm
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #ALARM_ERROR_NONE Successful
 * @retval #ALARM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #ALARM_ERROR_INVALID_TIME Triggered time is invalid
 * @retval #ALARM_ERROR_CONNECTION_FAIL Failed to connect to an alarm server
 * @retval #ALARM_ERROR_PERMISSION_DENIED Permission denied
 * @retval #ALARM_ERROR_NOT_PERMITTED_APP @a app_control is not permitted. @a app_control for UI application is only permitted.
 * @see alarm_cancel()
 * @see alarm_cancel_all()
 * @see alarm_get_scheduled_date()
 */
int alarm_schedule_once_after_delay(app_control_h app_control, int delay, int *alarm_id);


/**
 * @brief Sets an alarm to be triggered at a specific time.
 * @details The @a date describes the time of the first occurrence. To cancel the alarm, call alarm_cancel() with @a alarm_id.
 * @since_tizen @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/alarm.set
 * @privilege %http://tizen.org/privilege/appmanager.launch
 * @remarks If application is uninstalled after setting an alarm, the alarm is cancelled automatically.
 *          If the operation of @a app_control is not specified, #APP_CONTROL_OPERATION_DEFAULT is used for the launch request.
 *          If the operation of @a app_control is #APP_CONTROL_OPERATION_DEFAULT, the package information is mandatory to explicitly launch the application.
 *          If the appid of @a app_control is not specified, this api is not allowed. In other words, the explicit @a app_control is only allowed.
 *          The @a app_control only supports UI application with this api. If @a app_control is not UI application, #ALARM_ERROR_NOT_PERMITTED_APP returned.
 *          When the alarm is expired, Alarm Manager will turn on LCD to prohibit background jobs.
 *          Since 4.0, %http://tizen.org/privilege/appmanager.launch privilege is additionally required.
 *
 * @param[in] app_control The destination app_control to perform specific work when the alarm is triggered
 * @param[in] date The first active alarm time
 * @param[out] alarm_id The alarm ID that uniquely identifies an alarm
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #ALARM_ERROR_NONE   Successful
 * @retval #ALARM_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #ALARM_ERROR_INVALID_DATE Triggered date is invalid
 * @retval #ALARM_ERROR_CONNECTION_FAIL Failed to connect to an alarm server
 * @retval #ALARM_ERROR_PERMISSION_DENIED Permission denied
 * @retval #ALARM_ERROR_NOT_PERMITTED_APP @a app_control is not permitted. @a app_control for UI application is only permitted.
 * @see alarm_cancel()
 * @see alarm_cancel_all()
 * @see alarm_get_scheduled_date()
 */
int alarm_schedule_once_at_date(app_control_h app_control, struct tm *date, int *alarm_id);


/**
 * @brief Sets an alarm to be triggered periodically, starting at a specific time.
 * @details The @a date describes the time of the first occurrence.
 *          @a week_flag is the repeat value of the days of the week.
 *          If @a week_flag is #ALARM_WEEK_FLAG_TUESDAY, the alarm will repeat every Tuesday at a specific time.
 *          To cancel the alarm, call alarm_cancel() with @a alarm_id.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/alarm.set
 * @privilege %http://tizen.org/privilege/appmanager.launch
 * @remarks If the application is uninstalled after setting an alarm, the alarm is cancelled automatically.
 *          If the operation of @a app_control is not specified, #APP_CONTROL_OPERATION_DEFAULT is used for the launch request.
 *          If the operation of @a app_control is #APP_CONTROL_OPERATION_DEFAULT, the package information is mandatory to explicitly launch the application.
 *          Since @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif, The @a app_control only supports UI application with this api. If @a app_control is not UI application, #ALARM_ERROR_NOT_PERMITTED_APP returned.
 *          When the alarm is expired, Alarm Manager will turn on LCD to prohibit background jobs.
 *          Since @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif, If the appid of @a app_control is not specified, this api is not allowed. In other words, the explicit @a app_control is only allowed.
 *          Since 4.0, %http://tizen.org/privilege/appmanager.launch privilege is additionally required.
 *
 * @param[in] app_control The destination app_control to perform specific work when the alarm is triggered
 * @param[in] date The first active alarm time
 * @param[in] week_flag The day of the week, @a week_flag may be a combination of days, like #ALARM_WEEK_FLAG_TUESDAY | #ALARM_WEEK_FLAG_FRIDAY
 * @param[out] alarm_id The alarm ID that uniquely identifies an alarm
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #ALARM_ERROR_NONE Successful
 * @retval #ALARM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #ALARM_ERROR_INVALID_DATE Triggered date is invalid
 * @retval #ALARM_ERROR_CONNECTION_FAIL Failed to connect to an alarm server
 * @retval #ALARM_ERROR_PERMISSION_DENIED Permission denied
 * @retval #ALARM_ERROR_NOT_PERMITTED_APP @a app_control is not permitted. @a app_control for UI application is only permitted.
 * @see alarm_cancel()
 * @see alarm_cancel_all()
 * @see alarm_get_scheduled_recurrence_week_flag()
 * @see alarm_get_scheduled_date()
 * @see #alarm_week_flag_e
 */
int alarm_schedule_with_recurrence_week_flag(app_control_h app_control, struct tm *date, int week_flag, int *alarm_id);


/**
 * @brief Gets the recurrence days of the week.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/alarm.get
 * @remarks If the given @a alarm_id is not obtained by using the alarm_schedule_with_recurrence_week_flag() function,
 *          an error (error code #ALARM_ERROR_INVALID_PARAMETER) will occur because this alarm is scheduled with no recurrence.
 *          If the operation of @a app_control is not specified, #APP_CONTROL_OPERATION_DEFAULT is used for the launch request.
 *          If the operation of @a app_control is #APP_CONTROL_OPERATION_DEFAULT, the package information is mandatory to explicitly launch the application.
 * @param[in] alarm_id The alarm ID returned when the alarm is scheduled
 * @param[out] week_flag The recurrence days of the week, @a week_flag may be a combination of days, like #ALARM_WEEK_FLAG_TUESDAY | #ALARM_WEEK_FLAG_FRIDAY
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #ALARM_ERROR_NONE Successful
 * @retval #ALARM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #ALARM_ERROR_PERMISSION_DENIED Permission denied
 * @see alarm_schedule_with_recurrence_week_flag()
 * @see #alarm_week_flag_e
 */
int alarm_get_scheduled_recurrence_week_flag(int alarm_id, int *week_flag);


/**
 * @brief Cancels the alarm with the specific alarm ID.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/alarm.set
 * @param[in] alarm_id The alarm ID that is cancelled
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #ALARM_ERROR_NONE Successful
 * @retval #ALARM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #ALARM_ERROR_CONNECTION_FAIL Failed to connect to an alarm server
 * @retval #ALARM_ERROR_PERMISSION_DENIED Permission denied
 * @see alarm_schedule_once_at_date()
 * @see alarm_schedule_after_delay()
 * @see alarm_schedule_with_recurrence_week_flag()
 * @see alarm_cancel_all()
 */
int alarm_cancel(int alarm_id);


/**
 * @brief Cancels all scheduled alarms that are registered by the application that calls this API.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/alarm.set
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #ALARM_ERROR_NONE Successful
 * @retval #ALARM_ERROR_CONNECTION_FAIL Failed to connect to an alarm server
 * @retval #ALARM_ERROR_PERMISSION_DENIED Permission denied
 * @see alarm_schedule_once_at_date()
 * @see alarm_schedule_after_delay()
 * @see alarm_schedule_with_recurrence_week_flag()
 * @see alarm_cancel()
 */
int alarm_cancel_all(void);


/**
 * @brief Retrieves the IDs of all registered alarms by invoking a callback once for each scheduled alarm.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/alarm.get
 * @param[in] callback The callback function to invoke
 * @param[in] user_data The user data to be passed to the callback function
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #ALARM_ERROR_NONE Successful
 * @retval #ALARM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #ALARM_ERROR_CONNECTION_FAIL Failed to connect to an alarm server
 * @retval #ALARM_ERROR_PERMISSION_DENIED Permission denied
 * @post This function invokes alarm_registered_alarm_cb() repeatedly for each registered alarm.
 * @see alarm_registered_alarm_cb()
 */
int alarm_foreach_registered_alarm(alarm_registered_alarm_cb callback, void *user_data);


/**
 * @brief Gets the scheduled time from the given alarm ID in C standard time struct.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/alarm.get
 * @remarks @a date is not needed by the function after the call.
 * @param[in] alarm_id The alarm ID returned when the alarm is scheduled
 * @param[out] date The time value of the next alarm event
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #ALARM_ERROR_NONE Successful
 * @retval #ALARM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #ALARM_ERROR_CONNECTION_FAIL Failed to connect to an alarm server
 * @retval #ALARM_ERROR_PERMISSION_DENIED Permission denied
 * @see alarm_schedule_once_at_date()
 * @see alarm_schedule_after_delay()
 * @see alarm_schedule_with_recurrence_week_flag()
 */
int alarm_get_scheduled_date(int alarm_id, struct tm *date);


/**
 * @brief Gets the period of time between the recurrent alarms.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/alarm.get
 * @remarks If the given @a alarm_id is not obtained by using the alarm_schedule_once_at_date() or alarm_schedule_after_delay() function,
 *          an error (error code #ALARM_ERROR_INVALID_PARAMETER) will occur.
 * @param[in] alarm_id The alarm ID returned when the alarm is scheduled
 * @param[out] period The period of time between recurrent alarms in seconds
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #ALARM_ERROR_NONE Successful
 * @retval #ALARM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #ALARM_ERROR_CONNECTION_FAIL Failed to connect to an alarm server
 * @retval #ALARM_ERROR_PERMISSION_DENIED Permission denied
 * @see alarm_schedule_once_at_date()
 * @see alarm_schedule_after_delay()
 * @see alarm_schedule_with_recurrence_week_flag()
 */
int alarm_get_scheduled_period(int alarm_id, int *period);


/**
 * @brief Gets the current system time using C standard time struct.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[out] date The current system time
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #ALARM_ERROR_NONE Successful
 * @retval #ALARM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #ALARM_ERROR_PERMISSION_DENIED Permission denied
 */
int alarm_get_current_time(struct tm *date);


/**
 * @brief Gets the app_control to be invoked when the alarm is triggered.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/alarm.get
 * @remarks The @a app_control must be released using app_control_destroy().
 * @param[in] alarm_id The alarm ID uniquely identifies an alarm
 * @param[out] app_control The app_control handle to launch when the alarm is triggered
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #ALARM_ERROR_NONE Successful
 * @retval #ALARM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #ALARM_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #ALARM_ERROR_PERMISSION_DENIED Permission denied
 * @see alarm_schedule_once_at_date()
 * @see alarm_schedule_after_delay()
 * @see alarm_schedule_with_recurrence_week_flag()
 */
int alarm_get_app_control(int alarm_id, app_control_h *app_control);


/**
 * @brief Sets global flag in the alarm.
 * @details Sets @a global flag to set/unset alarm globally.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/alarm.set
 * @remarks The @a alarm_id must be id of alarm which will launch global application.
 *          The function returns an error (error code #ALARM_ERROR_NOT_PERMITTED_APP) if it is not.
 *          Note that the application which is launched by global alarm runs in the user session that is logged in currently.
 *          It doesn't run in the session for the user who registers the global alarm.
 *          So, if the application uses application's private data for each user to handling the app_control for the alarm, it might not show proper data to the user.
 *          Therefore, We recommend to contain all the data for the alarm to the app_control handle for the global alarm.
 *          Then, the launched application would be able to use the data to show proper alarm UX to the user.
 * @param[in] alarm_id The alarm ID uniquely identifies an alarm
 * @param[in] global The global flag to set/unset alarm globally
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #ALARM_ERROR_NONE Successful
 * @retval #ALARM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #ALARM_ERROR_NOT_PERMITTED_APP @a alarm_id is not permitted
 * @retval #ALARM_ERROR_PERMISSION_DENIED Permission denied
 * @see alarm_schedule_once_at_date()
 * @see alarm_schedule_after_delay()
 * @see alarm_schedule_with_recurrence_week_flag()
 */
int alarm_set_global(int alarm_id, bool global);


/**
 * @brief Gets whether the alarm will launch global application or not.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/alarm.get
 * @param[in] alarm_id The alarm ID uniquely identifies an alarm
 * @param[out] global Whether the alarm will launch global application or not
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #ALARM_ERROR_NONE Successful
 * @retval #ALARM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #ALARM_ERROR_PERMISSION_DENIED Permission denied
 * @see alarm_set_global()
 */
int alarm_get_global(int alarm_id, bool *global);


/**
 * @brief Sets a notification alarm to be triggered at a specific time.
 * @details The @a date describes the time of the alarm occurrence.
 *          To cancel the alarm, call alarm_cancel() with @a alarm_id.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/alarm.set
 * @privilege %http://tizen.org/privilege/notification
 * @remarks If application is uninstalled after setting an alarm, the alarm is cancelled automatically.
 *          When the alarm goes off, Alarm Manager will turn on LCD to prohibit background jobs.
 *
 * @param[in] noti The notification to be posted when the alarm is triggered
 * @param[in] date The active alarm time
 * @param[out] alarm_id The ID which uniquely identifies the scheduled alarm
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval  #ALARM_ERROR_NONE   Successful
 * @retval  #ALARM_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ALARM_ERROR_INVALID_DATE Triggered date is invalid
 * @retval  #ALARM_ERROR_CONNECTION_FAIL Failed to connect to an alarm server
 * @retval  #ALARM_ERROR_PERMISSION_DENIED Permission denied
 * @see alarm_cancel()
 * @see alarm_cancel_all()
 * @see alarm_get_scheduled_date()
 */
int alarm_schedule_noti_once_at_date(notification_h noti, struct tm *date, int *alarm_id);

/**
 * @brief Sets a notification alarm to be triggered after a specific delay.
 * @details The alarm will first go off after @a delay seconds.
 *          The alarm will then go off every period seconds until canceled.
 *          To cancel the alarm, call alarm_cancel() with @a alarm_id.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/alarm.set
 * @privilege %http://tizen.org/privilege/notification
 * @remarks If the application is uninstalled after setting an alarm, the alarm is cancelled automatically.
 *          This function is a minimally intrusive way to trigger alarms when precision is not important.
 *          The system will adjust the @a delay and @a period requests to suit internal needs; the requests
 *          will be treated as minimum values. Note that @a period cannot be less than 600 seconds, if
 *          a smaller request is supplied it will be silently adjusted to a request of 600.
 *          When the alarm goes off, Alarm Manager will turn on LCD to prohibit background jobs.
 *
 * @param[in] noti The notification to be posted when the alarm is triggered
 * @param[in] delay The amount of time before the first execution (in seconds).
 * @param[in] period The amount of time between subsequent alarms (in seconds).
 * @param[out] alarm_id The ID which uniquely identifies the scheduled alarm
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval  #ALARM_ERROR_NONE Successful
 * @retval  #ALARM_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ALARM_ERROR_INVALID_TIME Triggered time is invalid
 * @retval  #ALARM_ERROR_CONNECTION_FAIL Failed to connect to an alarm server
 * @retval  #ALARM_ERROR_PERMISSION_DENIED Permission denied
 * @see alarm_cancel()
 * @see alarm_cancel_all()
 * @see alarm_get_scheduled_date()
 * @see alarm_get_scheduled_period()
 * @see alarm_schedule_noti_once_after_delay()
 */
int alarm_schedule_noti_after_delay(notification_h noti, int delay, int period, int *alarm_id);

/**
 * @brief Sets a notification alarm to be triggered after a specific delay.
 * @details The alarm will go off @a delay seconds later.
 *          To cancel the alarm, call alarm_cancel() with @a alarm_id.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/alarm.set
 * @privilege %http://tizen.org/privilege/notification
 * @remarks If the application is uninstalled after setting an alarm, the alarm is cancelled automatically.
 *          When the alarm goes off, Alarm Manager will turn on LCD to prohibit background jobs.
 *
 * @param[in] noti The notification to be posted when the alarm is triggered
 * @param[in] delay The amount of time before the execution (in seconds)
 * @param[out] alarm_id The ID which uniquely identifies the scheduled alarm
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval  #ALARM_ERROR_NONE Successful
 * @retval  #ALARM_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval  #ALARM_ERROR_INVALID_TIME Triggered time is invalid
 * @retval  #ALARM_ERROR_CONNECTION_FAIL Failed to connect to an alarm server
 * @retval  #ALARM_ERROR_PERMISSION_DENIED Permission denied
 * @see alarm_cancel()
 * @see alarm_cancel_all()
 * @see alarm_get_scheduled_date()
 */
int alarm_schedule_noti_once_after_delay(notification_h noti, int delay, int *alarm_id);

/**
 * @brief Sets a notification to be triggered periodically, starting at a specific time.
 * @details The @a date describes the time of the first occurrence.
 *              @a week_flag describes the day(s) of the week when the notification recurs.
 *              If @a week_flag is #ALARM_WEEK_FLAG_TUESDAY, the alarm will repeat every Tuesday at a specific time.
 *              If @a week_flag is less than or equal to zero, the alarm is not repeated.
 *              To cancel the alarm, call alarm_cancel() with @a alarm_id.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/alarm.set
 * @privilege %http://tizen.org/privilege/notification
 * @remarks If the application is uninstalled after setting an alarm, the alarm is cancelled automatically.
 *
 * @param[in] noti The notification to be posted when the alarm is triggered
 * @param[in] date The first active alarm time
 * @param[in] week_flag The day of the week the notification recurs. @a week_flag may be a combination of days, like #ALARM_WEEK_FLAG_TUESDAY | #ALARM_WEEK_FLAG_FRIDAY
 * @param[out] alarm_id The ID which uniquely identifies the scheduled alarm
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #ALARM_ERROR_NONE Successful
 * @retval #ALARM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #ALARM_ERROR_INVALID_DATE Triggered date is invalid
 * @retval #ALARM_ERROR_CONNECTION_FAIL Failed to connect to an alarm server
 * @retval #ALARM_ERROR_PERMISSION_DENIED Permission denied
 * @see alarm_cancel()
 * @see alarm_cancel_all()
 * @see alarm_get_scheduled_recurrence_week_flag()
 * @see alarm_get_scheduled_date()
 * @see #alarm_week_flag_e
 */
int alarm_schedule_noti_with_recurrence_week_flag(notification_h noti, struct tm *date, int week_flag, int *alarm_id);

/**
 * @brief Gets the notification to be posted when an alarm is triggered.
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/alarm.get
 * @remarks @a noti must be released using notification_free().
 *
 * @param[in] alarm_id The ID which uniquely identifies a scheduled alarm
 * @param[out] noti The notification to be posted when the alarm is triggered
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #ALARM_ERROR_NONE Successful
 * @retval #ALARM_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #ALARM_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #ALARM_ERROR_PERMISSION_DENIED Permission denied
 * @see alarm_schedule_noti_once_at_date()
 * @see alarm_schedule_noti_after_delay()
 * @see alarm_schedule_noti_once_after_delay()
 * @see alarm_schedule_noti_with_recurrence_week_flag()
 */
int alarm_get_notification(int alarm_id, notification_h *noti);

/**
 * @brief Updates the delay of the registered alarm.
 * @details The @a delay is the time (in seconds) before the alarm's first setting off.
 *          The delay is an exact value, see alarm_schedule_once_after_delay() for details.
 *          This function can be called for any alarm.
 *          If the delay was set before, it will be overwritten. If it was not, it will be set.
 * @since_tizen 4.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/alarm.set
 * @remarks This function can be called only for alarms whose app control launches an UI application.
 *          If this condition is not met, #ALARM_ERROR_NOT_PERMITTED_APP is returned.
 * @param[in] alarm_id  The ID which uniquely identifies the scheduled alarm
 * @param[in] delay     The amount of time before the first execution (in seconds).
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #ALARM_ERROR_NONE               Successful
 * @retval #ALARM_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #ALARM_ERROR_CONNECTION_FAIL    Failed to connect to an alarm server
 * @retval #ALARM_ERROR_OUT_OF_MEMORY      Out of memory
 * @retval #ALARM_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #ALARM_ERROR_NOT_PERMITTED_APP  App control does not launch an UI application
 */
int alarm_update_delay(int alarm_id, int delay);

/**
 * @brief Updates the date of the registered alarm.
 * @details The @a date describes the date on which the alarm is set off for the first time.
 *          This function can be called for any alarm.
 *          If the date was set before, it will be overwritten. If it was not, it will be set.
 * @since_tizen 4.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/alarm.set
 * @remarks This function can be called only for alarms whose app control launches an UI application.
 *          If this condition is not met, #ALARM_ERROR_NOT_PERMITTED_APP is returned.
 * @param[in] alarm_id  The ID which uniquely identifies the scheduled alarm
 * @param[in] date      The time value of the next alarm event
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #ALARM_ERROR_NONE               Successful
 * @retval #ALARM_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #ALARM_ERROR_INVALID_TIME       Triggered time is invalid
 * @retval #ALARM_ERROR_INVALID_DATE       Triggered time is invalid
 * @retval #ALARM_ERROR_CONNECTION_FAIL    Failed to connect to an alarm server
 * @retval #ALARM_ERROR_OUT_OF_MEMORY      Out of memory
 * @retval #ALARM_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #ALARM_ERROR_NOT_PERMITTED_APP  App control does not launch an UI application
 */
int alarm_update_date(int alarm_id, struct tm *date);

/**
 * @brief Updates the period of the registered alarm.
 * @details The alarm will then go off every @a period seconds until canceled.
 *          This function can be called for any alarm.
 *          If the week recurrence flag was set before, it will be removed and the period will be set.
 *          If the period was set before, it will be overwritten. If it was not, it will be set.
 *          If the @a period argument is 0 and the period was previously set,
 *          the period attribute will be cleared and the alarm will be changed to one-time.
 *          If the @a period argument is 0 and the period was not set,
 *          or the week recurrence flag was set, the alarm will be unchanged.
 * @since_tizen 4.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/alarm.set
 * @param[in] alarm_id  The ID which uniquely identifies the scheduled alarm
 * @param[in] period    The amount of time between subsequent alarms (in seconds).
 *                      Since @if MOBILE 2.4 @elseif WEARABLE 3.0 @endif, this
 *                      value does not guarantee the accuracy. The actual interval
 *                      is calculated by the OS. The minimum value is 600sec.
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #ALARM_ERROR_NONE               Successful
 * @retval #ALARM_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #ALARM_ERROR_CONNECTION_FAIL    Failed to connect to an alarm server
 * @retval #ALARM_ERROR_OUT_OF_MEMORY      Out of memory
 * @retval #ALARM_ERROR_PERMISSION_DENIED  Permission denied
 */
int alarm_update_period(int alarm_id, int period);

/**
 * @brief Updates the week recurrence flag of the registered alarm.
 * @details @a week_flag is the repeat value of the days of the week.
 *          For example, if @a week_flag is #ALARM_WEEK_FLAG_TUESDAY, the alarm will repeat every
 *          Tuesday at a specific time.
 *          This function can be called for any alarm.
 *          If the period was set before, it will be removed and the week recurrence flag will be set.
 *          If the week recurrence flag was set before, it will be overwritten. If it was not, it will be set.
 *          If the @a week_flag argument is 0 and the flag was previously set,
 *          the flag attribute will be cleared and the alarm will be changed to one-time.
 *          If the @a week_flag argument is 0 and the flag was not set,
 *          or the period was set, the alarm will be unchanged.
 * @since_tizen 4.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/alarm.set
 * @remarks This function can be called only for alarms whose app control launches an UI application.
 *          If this condition is not met, #ALARM_ERROR_NOT_PERMITTED_APP is returned.
 * @param[in] alarm_id   The ID which uniquely identifies the scheduled alarm
 * @param[in] week_flag  The day of the week, @a week_flag may be a combination
 *                       of days, like #ALARM_WEEK_FLAG_TUESDAY | #ALARM_WEEK_FLAG_FRIDAY
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #ALARM_ERROR_NONE               Successful
 * @retval #ALARM_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #ALARM_ERROR_CONNECTION_FAIL    Failed to connect to an alarm server
 * @retval #ALARM_ERROR_OUT_OF_MEMORY      Out of memory
 * @retval #ALARM_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #ALARM_ERROR_NOT_PERMITTED_APP  App control does not launch an UI application
 */
int alarm_update_week_flag(int alarm_id, int week_flag);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_APPFW_ALARM_H__ */

