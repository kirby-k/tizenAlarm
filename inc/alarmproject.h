#ifndef __alarmproject_H__
#define __alarmproject_H__

#include <app.h>
#include <Elementary.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "alarmproject"

#if !defined(PACKAGE)
#define PACKAGE "org.example.alarmproject"
#endif

#define BUF_LEN 1024

void alarm_destroy_widget(void *user_data);
void alarm_set_widget_on_off(char *on_off, void *user_data);

#endif /* __alarmproject_H__ */
