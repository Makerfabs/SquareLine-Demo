// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.2.3
// LVGL version: 8.3.4
// Project name: 43inch

#ifndef _43INCH_UI_H
#define _43INCH_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined __has_include
#if __has_include("lvgl.h")
#include "lvgl.h"
#elif __has_include("lvgl/lvgl.h")
#include "lvgl/lvgl.h"
#else
#include "lvgl.h"
#endif
#else
#include "lvgl.h"
#endif

#include "ui_events.h"
void Logo_a_Animation(lv_obj_t * TargetObject, int delay);
extern lv_obj_t * ui_Screen1;
extern lv_obj_t * ui_Label2;
void ui_event_Button2(lv_event_t * e);
extern lv_obj_t * ui_Button2;
extern lv_obj_t * ui_Label6;
void ui_event_Image2(lv_event_t * e);
extern lv_obj_t * ui_Image2;
extern lv_obj_t * ui_Label3;
extern lv_obj_t * ui_Screen2;
void ui_event_TextArea2(lv_event_t * e);
extern lv_obj_t * ui_TextArea2;
void ui_event_TextArea1(lv_event_t * e);
extern lv_obj_t * ui_TextArea1;
void ui_event_Button1(lv_event_t * e);
extern lv_obj_t * ui_Button1;
extern lv_obj_t * ui_Label7;
extern lv_obj_t * ui_Keyboard2;
extern lv_obj_t * ui_Label4;
extern lv_obj_t * ui_Screen3;
extern lv_obj_t * ui_Bar2;
extern lv_obj_t * ui_Bar1;
extern lv_obj_t * ui_Bar4;
extern lv_obj_t * ui_Bar3;
extern lv_obj_t * ui_Calendar2;
extern lv_obj_t * ui_Label15;
extern lv_obj_t * ui_Label14;
extern lv_obj_t * ui_Label11;
extern lv_obj_t * ui_Label12;
extern lv_obj_t * ui_Label13;
extern lv_obj_t * ui_Label1;
extern lv_obj_t * ui_Image3;
extern lv_obj_t * ui____initial_actions0;


LV_IMG_DECLARE(ui_img_bg_png);    // assets\bg.png
LV_IMG_DECLARE(ui_img_logo_png);    // assets\logo.png
LV_IMG_DECLARE(ui_img_weather2_png);    // assets\weather2.png
LV_IMG_DECLARE(ui_img_1980645537);    // assets\logo-250x80.png
LV_IMG_DECLARE(ui_img_weather_png);    // assets\weather.png




void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif