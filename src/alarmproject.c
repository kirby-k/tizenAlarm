//Christian Lee

#include <app.h>
#include <app_alarm.h>
#include <efl_extension.h>
#include <dlog.h>
#include <system_settings.h>
#include <app_preference.h>
#include <bundle.h>
#include <widget_service.h>
#include <widget_errno.h>

//for media playback
#include <player.h>
#include <Elementary.h>
#include <glib.h>
#include <tone_player.h>

// for access to storage
#include <storage.h>

// other header files
#include "alarmproject.h"
#include "data.h"
#include "app_alarm.h"

// holds date input
Evas_Object* dateSelect;
// holds time input
Evas_Object* timeSelect;
// final time object
struct tm alarmTime;
// temp time object for transferring input date to alarmTime
struct tm tmpTime;
// list of alarms
struct tm alarmList[100];

// dummy ID given to alarm scheduler
int ALARMID = 0;
// actual ID that points to each alarm
int refALARMID = 0;
// ID's of tones currently active
int startToneId = 0;

// window object
Evas_Object *win;
// conform object is inside of window yet holds all info
Evas_Object *conform;

Evas_Object *box;
// allows scrolling over items
Evas_Object *scroll;
// each screen is represented as a naviFrame, which holds all data under it (like boxes and buttons), and can be
// pushed and popped from the viewable screen like a stack
Evas_Object *navi;

Elm_Object_Item *naviItem;

// build-in tizen functions use this
// my functions use global variables instead
typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *box;
	Evas_Object *scroll;
	Evas_Object *navi;
	Elm_Object_Item *naviItem;
} appdata_s;

// used by view.c for complete shutdown
void alarm_destroy_widget(void *user_data)
{
	struct genlist_item_data *gendata = NULL;
	bundle *b = NULL;
	int ret = 0;

	gendata = user_data;
	if (gendata == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to get gendata");
		return;
	}

	b = data_get_widget_data_bundle();
	if (b == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to get data bundle");
		return;
	}

	bundle_del(b, "Operation");
	bundle_add_str(b, "Operation", "Destroy");

	ret = widget_service_trigger_update(gendata->widget_id, gendata->instance_id, b, 0);
	if (ret != WIDGET_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to (). ret = %d", ret);
	}
}

// enables or disables widget
void alarm_set_widget_on_off(char *on_off, void *user_data)
{
	struct genlist_item_data *gendata = NULL;
	bundle *b = NULL;
	char alarm_id_str[BUF_LEN] = { 0, };
	int ret = 0;

	gendata = user_data;
	if (gendata == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to get gendata");
		return;
	}

	snprintf(alarm_id_str, sizeof(alarm_id_str), "%d", gendata->alarm_id);

	b = data_get_widget_data_bundle();
	if (b == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "widget data bundle is NULL");
		return;
	}

	bundle_del(b, "OnOff");
	if (!strcmp(on_off, "On")) {
		bundle_add_str(b, "OnOff", "On");
	} else {
		bundle_add_str(b, "OnOff", "Off");
	}

	bundle_del(b, "AlarmId");
	bundle_add_str(b, "AlarmId", alarm_id_str);

	bundle_del(b, "Operation");
	bundle_add_str(b, "Operation", "SetOnOff");

	dlog_print(DLOG_DEBUG, LOG_TAG, "%s[%d] widget_id(%s), instance_id(%s), alarm id(%s)",
			__func__, __LINE__, gendata->widget_id, gendata->instance_id, alarm_id_str);

	widget_service_trigger_update(gendata->widget_id, gendata->instance_id, b, 0);
	if (ret != WIDGET_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to widget_service_trigger_update(). ret = %d", ret);
	}
}

// required functions to allow back & home buttons to operate
void
service_app_control(app_control_h app_control, void *data)
{
    dlog_print(DLOG_INFO, LOG_TAG, "app_control called by Alarm API.");
}

static void
win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = data;
	/* Let window go to hide state. */
	elm_win_lower(win);
}


// every time a back button is pushed we pop the current naviFrame from the stack
// which deletes the current screen and makes the next one on the stack visible
void backButtonClick(void* data) {
	elm_naviframe_item_pop(data);
}

// stops all currently playing tones in case of alarms overlapping, and exits app
void dismissButtonClick(void* data, Evas_Object* obj, void* eventInfo) {
	for (int i = 0; i < startToneId; i++) {
		tone_player_stop(i);
	}
	ui_app_exit();    // exits app
}


void okButtonClick(void* data, Evas_Object* obj, void* eventInfo) {

	Evas_Object* okButton;	// creation of okButton object (still generalized)
	okButton = elm_button_add(navi);	// specializes Evas_Object to be a button
										// makes it a child of the current navi frame
	elm_object_text_set(okButton, "OK");	// sets text for button
	elm_object_style_set(okButton, "bottom");	// bottom style puts it in the bottom middle of its parent object
	evas_object_size_hint_align_set(okButton, .5,1);	// sets general alignment for object using x,y coords
														// but will only take effect if it can be placed in that area
	evas_object_smart_callback_add(okButton, "clicked", backButtonClick, navi);	//
	evas_object_show(okButton);


	app_control_h app_control;
    app_control_create(&app_control);
    app_control_set_operation(app_control, APP_CONTROL_OPERATION_ALARM_ONTIME);
    app_control_set_app_id(app_control, "org.example.alarmproject");

	alarm_get_current_time(&alarmTime);
	elm_datetime_value_get(timeSelect, &alarmTime);

	alarm_get_current_time(&tmpTime);
	elm_datetime_value_get(dateSelect, &tmpTime);

	alarmTime.tm_year = tmpTime.tm_year;
	alarmTime.tm_mon = tmpTime.tm_mon;
	alarmTime.tm_mday = tmpTime.tm_mday;
	alarmTime.tm_sec = 0;

	/*// rapid fire testing REMOVE
	struct tm test;
	alarm_get_current_time(&test);
	test.tm_sec += 5;
*/


	alarmList[refALARMID] = alarmTime;

	alarm_schedule_once_at_date(app_control, &alarmTime, &ALARMID);

	refALARMID++;
	ALARMID++;

	char log[50];
	int l;
	l = refALARMID;
	sprintf(log, "OKBUTTON refALARMID: %d", l);
	dlog_print(DLOG_INFO, "check", log);

	Elm_Object_Item* okScreen = elm_naviframe_item_push(navi, "Alarm set!", NULL, NULL, okButton, NULL);

}

void alarmMenu(void* data, Evas_Object* obj, void* eventInfo) {

	Elm_Object_Item* alarmScreen;
	Evas_Object* parentBox;
	Evas_Object* buttonBox;
    Evas_Object* alarmBox;
    Evas_Object* alarmScroll;

    alarmScroll = elm_scroller_add(navi);
	evas_object_show(alarmScroll);

	parentBox = elm_box_add(alarmScroll);
	evas_object_size_hint_weight_set(parentBox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(parentBox);
	elm_object_content_set(alarmScroll, parentBox);
	elm_box_padding_set(parentBox, 0,10);

    alarmBox = elm_box_add(parentBox);
	evas_object_size_hint_weight_set(alarmBox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(alarmBox);
	elm_box_padding_set(alarmBox, 0,50);


    buttonBox = elm_box_add(parentBox);
	evas_object_size_hint_weight_set(buttonBox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(buttonBox);
	elm_box_padding_set(buttonBox, 0,30);


	dateSelect = elm_datetime_add(alarmBox);

	//sets hour and minute as visible, and others as invisible
	elm_datetime_field_visible_set(dateSelect, ELM_DATETIME_DATE, EINA_TRUE);
	elm_datetime_field_visible_set(dateSelect, ELM_DATETIME_MONTH, EINA_TRUE);
	elm_datetime_field_visible_set(dateSelect, ELM_DATETIME_YEAR, EINA_TRUE);

	elm_datetime_format_set(dateSelect, "%b, %d, %Y");
	elm_object_style_set(dateSelect, "datepicker_layout");

	evas_object_show(dateSelect);

    timeSelect = elm_datetime_add(alarmBox);

    //set elements you don't want as invisible

    elm_datetime_field_visible_set(timeSelect, ELM_DATETIME_DATE, EINA_FALSE);
    elm_datetime_field_visible_set(timeSelect, ELM_DATETIME_YEAR, EINA_FALSE);
    elm_datetime_field_visible_set(timeSelect, ELM_DATETIME_MONTH, EINA_FALSE);
    elm_datetime_field_visible_set(timeSelect, ELM_DATETIME_AMPM, EINA_FALSE);


    elm_object_style_set(timeSelect, "timepicker/circle");
    elm_datetime_format_set(timeSelect, "%R");

    evas_object_size_hint_align_set(timeSelect, -5,0);
    evas_object_show(timeSelect);

    // OK button
    Evas_Object* okButton;
	okButton = elm_button_add(buttonBox);
	elm_object_text_set(okButton, "OK");
	elm_object_style_set(okButton, "bottom");
	evas_object_smart_callback_add(okButton, "clicked", okButtonClick, navi);
	evas_object_show(okButton);

    // back button
    Evas_Object* backButton;
	backButton = elm_button_add(buttonBox);
	elm_object_text_set(backButton, "Back");
	elm_object_style_set(backButton, "bottom");
	evas_object_smart_callback_add(backButton, "clicked", backButtonClick, navi);
	evas_object_show(backButton);


	elm_box_pack_end(alarmBox, dateSelect);
	elm_box_pack_end(alarmBox, timeSelect);
	elm_box_pack_end(buttonBox, okButton);
	elm_box_pack_end(buttonBox, backButton);
	elm_box_pack_end(parentBox, alarmBox);
	elm_box_pack_end(parentBox, buttonBox);


    //push new alarm screen onto stack (make visible)
	alarmScreen = elm_naviframe_item_push(navi, "Alarm", NULL, NULL, alarmScroll, NULL);
}

void listItemRemove(void* data, Evas_Object* obj, void* eventInfo) {

}

void alarmListMenu(void* data, Evas_Object* obj, void* eventInfo) {
	Evas_Object* backButton;
	Elm_Object_Item* alarmListScreen;

	Evas_Object* alarmScroll;
	Evas_Object* parentBox;
	Evas_Object* aList;

	alarmScroll = elm_scroller_add(navi);
	evas_object_show(alarmScroll);

	parentBox = elm_box_add(alarmScroll);
	evas_object_size_hint_weight_set(parentBox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(parentBox);
	elm_object_content_set(alarmScroll, parentBox);
	//elm_box_padding_set(parentBox, 0,10);

	aList = elm_list_add(parentBox);
	evas_object_size_hint_weight_set(aList, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(aList, EVAS_HINT_FILL, EVAS_HINT_FILL);

	for (int i = 0; i < refALARMID; i++) {
		char log[50];
		int l;
		l = refALARMID;
		sprintf(log, "LIST refALARMID: %d", l);
		dlog_print(DLOG_INFO, "check", log);

		char c [50];
		int year = alarmList[i].tm_year + 1900;
		int month = alarmList[i].tm_mon;
		int day = alarmList[i].tm_mday;
		int hr = alarmList[i].tm_hour;
		int min = alarmList[i].tm_min;

		sprintf(c, "%d/%d/%d %d:%d", month, day, year, hr, min);
		elm_list_item_append(aList, c, NULL, NULL, NULL, NULL);
	}
	evas_object_show(aList);

	backButton = elm_button_add(parentBox);
	elm_object_text_set(backButton, "Back");
	elm_object_style_set(backButton, "bottom");
	evas_object_smart_callback_add(backButton, "clicked", backButtonClick, navi);
	evas_object_show(backButton);

	elm_box_pack_end(parentBox, aList);
	elm_box_pack_end(parentBox, backButton);



	alarmListScreen = elm_naviframe_item_push(navi, "Alarm List", NULL, NULL, alarmScroll, NULL);
}

void nudgeMenu(void* data, Evas_Object* obj, void* eventInfo) {
	Evas_Object* backButton;
	Elm_Object_Item* nudgeScreen;

	backButton = elm_button_add(navi);
	elm_object_text_set(backButton, "Back");
	elm_object_style_set(backButton, "bottom");
	evas_object_smart_callback_add(backButton, "clicked", backButtonClick, navi);

	nudgeScreen = elm_naviframe_item_push(navi, "Nudge", NULL, NULL, backButton, NULL);
}

static void
create_base_gui(appdata_s *ad)
{

	/* Window */
	/* Create and initialize elm_win.
	   elm_win is mandatory to manipulate window. */
	win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(win, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(win, "delete,request", win_delete_request_cb, NULL);
	eext_object_event_callback_add(win, EEXT_CALLBACK_BACK, win_back_cb, ad);

	/* Conformant */
	/* Create and initialize elm_conformant.
	   elm_conformant is mandatory for base gui to have proper size
	   when indicator or virtual keypad is visible. */
	conform = elm_conformant_add(win);
	elm_win_indicator_mode_set(win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(win, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(win, conform);
	evas_object_show(conform);

	navi = elm_naviframe_add(conform);
	evas_object_show(navi);
	elm_object_content_set(conform, navi);

	box = elm_box_add(navi);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
//	evas_object_show(box);
	elm_object_content_set(navi, box);

	Evas_Object* alarmButton;
	Evas_Object* alarmListButton;
	Evas_Object* nudgeButton;

	alarmButton = elm_button_add(box);
	elm_object_text_set(alarmButton, "Alarm");
	elm_object_style_set(alarmButton, "bottom");
	evas_object_smart_callback_add(alarmButton, "clicked", alarmMenu, navi);

	alarmListButton = elm_button_add(box);
	elm_object_text_set(alarmListButton, "Alarm List");
	elm_object_style_set(alarmListButton, "bottom");
	evas_object_smart_callback_add(alarmListButton, "clicked", alarmListMenu, navi);

	nudgeButton = elm_button_add(box);
	elm_object_text_set(nudgeButton, "Nudge");
	elm_object_style_set(nudgeButton, "bottom");
	evas_object_smart_callback_add(nudgeButton, "clicked", nudgeMenu, navi);

	evas_object_show(alarmButton);
	evas_object_show(alarmListButton);
	evas_object_show(nudgeButton);
	elm_box_pack_end(box, alarmButton);
	elm_box_pack_end(box, alarmListButton);
	elm_box_pack_end(box, nudgeButton);

	scroll = elm_scroller_add(navi);
	elm_object_content_set(scroll, box);
	evas_object_show(scroll);


	naviItem = elm_naviframe_item_push(navi, "Select Feature", NULL, NULL, scroll, NULL);


	/* Show window after base gui is set up */
	evas_object_show(win);
}

static bool
app_create(void *data)
{
	/* Hook to take necessary actions before main event loop starts
		Initialize UI resources and application's data
		If this function returns true, the main loop of application starts
		If this function returns false, the application is terminated */
	appdata_s *ad = data;

	create_base_gui(ad);

	return true;
}

static void
app_control(app_control_h app_control, void *data)
{
	dlog_print(DLOG_ERROR, "check", "here, but not really [cringe]");

	char* op;
	app_control_get_operation(app_control, &op);
	dlog_print(DLOG_INFO, LOG_TAG, "operation = %s", op);

	if (!strncmp(APP_CONTROL_OPERATION_ALARM_ONTIME, op, strlen(APP_CONTROL_OPERATION_ALARM_ONTIME))) {
		dlog_print(DLOG_ERROR, "check", "right where I want to be [cringe]");


		Evas_Object* box;
		box = elm_box_add(navi);
		elm_object_content_set(navi, box);


		Evas_Object* label;
		label = elm_label_add(box);
		evas_object_size_hint_align_set(label, .5,.8);

		char* c = "";
		//sprintf(c, "%d", alarmTime.tm_min);
		elm_object_text_set(label, c);
		evas_object_show(label);

		Evas_Object* dismissButton;
		dismissButton = elm_button_add(box);
		elm_object_text_set(dismissButton, "Dismiss");
		elm_object_style_set(dismissButton, "bottom");
		evas_object_size_hint_align_set(dismissButton, .5,1);
		evas_object_smart_callback_add(dismissButton, "clicked", dismissButtonClick, navi);

		/*
		Evas_Object* okButton;
		okButton = elm_button_add(data);
		elm_object_text_set(okButton, "OK");
		elm_object_style_set(okButton, "bottom");
		evas_object_size_hint_align_set(okButton, .5,1);
		evas_object_smart_callback_add(okButton, "clicked", backButtonClick, data);
		evas_object_show(okButton);
*/

		evas_object_show(dismissButton);

		elm_box_pack_end(box, label);
		elm_box_pack_end(box, dismissButton);


		player_h player;
		player_create(&player);
		sound_stream_info_h stream_info;
	    sound_manager_create_stream_information(SOUND_STREAM_TYPE_MEDIA, NULL, NULL, &stream_info);

	    //japan default ringing
	    tone_player_start_new(TONE_TYPE_CDMA_NETWORK_USA_RINGBACK, stream_info, -1, &startToneId);
	    startToneId++;

		Elm_Object_Item* dismissScreen = elm_naviframe_item_push(navi, "Alarm!", NULL, NULL, box, NULL);
	}
}

static void
app_pause(void *data)
{
	/* Take necessary actions when application becomes invisible. */
}

static void
app_resume(void *data)
{
	/* Take necessary actions when application becomes visible. */
}

static void
app_terminate(void *data)
{
	/* Release all resources. */
}

static void
ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void
ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
	return;
}

static void
ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void
ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_BATTERY*/
}

static void
ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_MEMORY*/
}

int
main(int argc, char *argv[])
{
	appdata_s ad = {0,};
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}
