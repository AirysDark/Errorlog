#ifndef MENUFUNCTIONS_H
#define MENUFUNCTIONS_H

#include <lvgl.h>

// No-op stubs so “declared but never defined” warnings don’t fail build
static inline void ta_event_cb(lv_obj_t*, lv_event_t) {}
static inline void join_wifi_keyboard_event_cb(lv_obj_t*, lv_event_t) {}
static inline void add_ssid_keyboard_event_cb(lv_obj_t*, lv_event_t) {}
static inline void html_list_cb(lv_obj_t*, lv_event_t) {}
static inline void ap_list_cb(lv_obj_t*, lv_event_t) {}
static inline void ap_info_list_cb(lv_obj_t*, lv_event_t) {}
static inline void at_list_cb(lv_obj_t*, lv_event_t) {}
static inline void station_list_cb(lv_obj_t*, lv_event_t) {}
static inline void setting_dropdown_cb(lv_obj_t*, lv_event_t) {}

// Comment out unused buffers/objects to avoid “unused-variable” Werror
// static lv_color_t buf[LV_HOR_RES_MAX * 10];
// static lv_disp_buf_t disp_buf;
// static lv_obj_t *kb;
// static lv_obj_t *save_as_kb;

#endif // MENUFUNCTIONS_H