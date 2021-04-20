//Christian Lee
#include "test5point5.h"

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *box;
	Evas_Object *scroll;
	Evas_Object *navi;
	Elm_Object_Item *naviItem;
} appdata_s;

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
	elm_win_lower(ad->win);
}

void backButtonClick(void* data) {
	Evas_Object* currentScreen = data;
	elm_naviframe_item_pop(currentScreen);
}

void okButtonClick(void* data, Evas_Object* obj, void* eventInfo) {

	Evas_Object* okButton;
	okButton = elm_button_add(data);
	elm_object_text_set(okButton, "OK");
	elm_object_style_set(okButton, "bottom");
	evas_object_size_hint_align_set(okButton, .5,1);
	evas_object_smart_callback_add(okButton, "clicked", backButtonClick, data);
	evas_object_show(okButton);

	Elm_Object_Item* okScreen = elm_naviframe_item_push(data, "Alarm set!", NULL, NULL, okButton, NULL);

}


void alarmMenu(void* data, Evas_Object* obj, void* eventInfo) {

	Evas_Object* nf = data;
	Elm_Object_Item* alarmScreen;
	Evas_Object* parentBox;
	Evas_Object* buttonBox;
    Evas_Object* alarmBox;
    Evas_Object* alarmScroll;

    alarmScroll = elm_scroller_add(nf);
	evas_object_show(alarmScroll);

	parentBox = elm_box_add(alarmScroll);
	evas_object_size_hint_weight_set(parentBox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(parentBox);
	elm_object_content_set(alarmScroll, parentBox);
	elm_box_padding_set(parentBox, 0,100);

    alarmBox = elm_box_add(parentBox);
	evas_object_size_hint_weight_set(alarmBox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(alarmBox);
	elm_box_padding_set(alarmBox, 0,90);


    buttonBox = elm_box_add(parentBox);
	evas_object_size_hint_weight_set(buttonBox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(buttonBox);
	elm_box_padding_set(buttonBox, 0,0);


//	elm_object_content_set(nf, alarmBox);		breaks everything

	Evas_Object* dateSelect;
	dateSelect = elm_datetime_add(alarmBox);

	//sets hour and minute as visible, and others as invisible
	elm_datetime_field_visible_set(dateSelect, ELM_DATETIME_DATE, EINA_TRUE);
	elm_datetime_field_visible_set(dateSelect, ELM_DATETIME_MONTH, EINA_TRUE);
	elm_datetime_field_visible_set(dateSelect, ELM_DATETIME_YEAR, EINA_TRUE);

	elm_datetime_format_set(dateSelect, "%b, %d, %Y");
	elm_object_style_set(dateSelect, "datepicker_layout");

	evas_object_size_hint_min_set(dateSelect, 100,100);
	evas_object_size_hint_align_set(dateSelect, .5,.5);
	evas_object_show(dateSelect);


	Evas_Object* timeSelect;
    timeSelect = elm_datetime_add(alarmBox);

    //set elements you don't want as invisible

    elm_datetime_field_visible_set(timeSelect, ELM_DATETIME_DATE, EINA_FALSE);
    elm_datetime_field_visible_set(timeSelect, ELM_DATETIME_YEAR, EINA_FALSE);
    elm_datetime_field_visible_set(timeSelect, ELM_DATETIME_MONTH, EINA_FALSE);
    elm_datetime_field_visible_set(timeSelect, ELM_DATETIME_AMPM, EINA_FALSE);

    elm_datetime_field_limit_set(timeSelect, ELM_DATETIME_HOUR, 0, 60);

    elm_object_style_set(timeSelect, "timepicker_layout");
    elm_datetime_format_set(timeSelect, "%R");

    evas_object_size_hint_align_set(timeSelect, .7,.5);
    evas_object_show(timeSelect);

    // OK button
    Evas_Object* okButton;
	okButton = elm_button_add(buttonBox);
	elm_object_text_set(okButton, "OK");
	elm_object_style_set(okButton, "bottom");
	evas_object_size_hint_align_set(okButton, .5,0);
	evas_object_smart_callback_add(okButton, "clicked", okButtonClick, nf);
	evas_object_show(okButton);

    // back button
    Evas_Object* backButton;
	backButton = elm_button_add(buttonBox);
	elm_object_text_set(backButton, "Back");
	elm_object_style_set(backButton, "bottom");
    evas_object_size_hint_align_set(backButton, .5,0);
	evas_object_smart_callback_add(backButton, "clicked", backButtonClick, nf);
	evas_object_show(backButton);


	elm_box_pack_end(alarmBox, dateSelect);
	elm_box_pack_end(alarmBox, timeSelect);
	elm_box_pack_end(buttonBox, okButton);
	elm_box_pack_end(buttonBox, backButton);
	elm_box_pack_end(parentBox, alarmBox);
	elm_box_pack_end(parentBox, buttonBox);


    //push new alarm screen onto stack (make visible)
	alarmScreen = elm_naviframe_item_push(nf, "Alarm", NULL, NULL, alarmScroll, NULL);
}

void reminderMenu(void* data, Evas_Object* obj, void* eventInfo) {
	Evas_Object* backButton;
	Evas_Object* nf = data;
	Elm_Object_Item* reminderScreen;

	backButton = elm_button_add(nf);
	elm_object_text_set(backButton, "Back");
	elm_object_style_set(backButton, "bottom");
	evas_object_smart_callback_add(backButton, "clicked", backButtonClick, nf);

	reminderScreen = elm_naviframe_item_push(nf, "Reminder", NULL, NULL, backButton, NULL);
}

void nudgeMenu(void* data, Evas_Object* obj, void* eventInfo) {
	Evas_Object* backButton;
	Evas_Object* nf = data;
	Elm_Object_Item* nudgeScreen;

	backButton = elm_button_add(nf);
	elm_object_text_set(backButton, "Back");
	elm_object_style_set(backButton, "bottom");
	evas_object_smart_callback_add(backButton, "clicked", backButtonClick, nf);

	nudgeScreen = elm_naviframe_item_push(nf, "Nudge", NULL, NULL, backButton, NULL);
}

static void
create_base_gui(appdata_s *ad)
{

	/* Window */
	/* Create and initialize elm_win.
	   elm_win is mandatory to manipulate window. */
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);
	eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb, ad);

	/* Conformant */
	/* Create and initialize elm_conformant.
	   elm_conformant is mandatory for base gui to have proper size
	   when indicator or virtual keypad is visible. */
	ad->conform = elm_conformant_add(ad->win);
	elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	ad->navi = elm_naviframe_add(ad->conform);
	evas_object_show(ad->navi);
	elm_object_content_set(ad->conform, ad->navi);

	ad->box = elm_box_add(ad->navi);
	evas_object_size_hint_weight_set(ad->box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
//	evas_object_show(ad->box);
	elm_object_content_set(ad->navi, ad->box);

	Evas_Object* alarmButton;
	Evas_Object* reminderButton;
	Evas_Object* nudgeButton;

	alarmButton = elm_button_add(ad->box);
	elm_object_text_set(alarmButton, "Alarm");
	elm_object_style_set(alarmButton, "bottom");
	evas_object_smart_callback_add(alarmButton, "clicked", alarmMenu, ad->navi);

	reminderButton = elm_button_add(ad->box);
	elm_object_text_set(reminderButton, "Reminder");
	elm_object_style_set(reminderButton, "bottom");
	evas_object_smart_callback_add(reminderButton, "clicked", reminderMenu, ad->navi);

	nudgeButton = elm_button_add(ad->box);
	elm_object_text_set(nudgeButton, "Nudge");
	elm_object_style_set(nudgeButton, "bottom");
	evas_object_smart_callback_add(nudgeButton, "clicked", nudgeMenu, ad->navi);

	evas_object_show(alarmButton);
	evas_object_show(reminderButton);
	evas_object_show(nudgeButton);
	elm_box_pack_end(ad->box, alarmButton);
	elm_box_pack_end(ad->box, reminderButton);
	elm_box_pack_end(ad->box, nudgeButton);

	ad->scroll = elm_scroller_add(ad->navi);
	elm_object_content_set(ad->scroll, ad->box);
	evas_object_show(ad->scroll);


	ad->naviItem = elm_naviframe_item_push(ad->navi, "Select Feature", NULL, NULL, ad->scroll, NULL);


	/* Show window after base gui is set up */
	evas_object_show(ad->win);
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
	/* Handle the launch request. */
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
