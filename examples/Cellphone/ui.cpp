#include "ui.h"
#include "utilities.h"
#include "lvgl.h"
#include "stdio.h"

#define WIN 0

#if !WIN
#include "WiFi.h"
#endif

#include "ui_helpers.h"

extern "C"
{
#include "app_typedef.h"
#include "global_flags.h"
}

LV_FONT_DECLARE(alibaba_font);
LV_IMG_DECLARE(lilygo2_gif);
LV_IMG_DECLARE(ui_img_camera_png);
LV_IMG_DECLARE(ui_img_bg_png);
LV_FONT_DECLARE(ui_font_misans16);
LV_FONT_DECLARE(ui_font_symbols16);

LV_IMG_DECLARE(theme_bg_img0);
LV_IMG_DECLARE(grbybw_222480);

LV_IMG_DECLARE(app_facetime_img);
LV_IMG_DECLARE(app_mail_img);
LV_IMG_DECLARE(app_message_img);
LV_IMG_DECLARE(app_music_img);
LV_IMG_DECLARE(app_notes_img);
LV_IMG_DECLARE(app_Phone_img);
LV_IMG_DECLARE(app_photos_img);
LV_IMG_DECLARE(app_settings_img);
LV_IMG_DECLARE(app_store_img);
LV_IMG_DECLARE(app_tv_img);

LV_IMG_DECLARE(app_auto_brightne_img);

LV_IMG_DECLARE(app_contacts_img);
LV_IMG_DECLARE(app_files_img);
LV_IMG_DECLARE(app_find_my_img);
LV_IMG_DECLARE(app_wallet_img);

LV_IMG_DECLARE(app_books_img);
LV_IMG_DECLARE(app_calculator_img);
LV_IMG_DECLARE(app_camera_img);
LV_IMG_DECLARE(app_clock_img);
LV_IMG_DECLARE(app_safari_img);
LV_IMG_DECLARE(app_shortcuts_img);
LV_IMG_DECLARE(app_stocks_img);
LV_IMG_DECLARE(app_voice_memos_img);
LV_IMG_DECLARE(app_podcasts_img);

LV_IMG_DECLARE(ui_img_1103371374);
LV_IMG_DECLARE(ui_img_previous_png);
LV_IMG_DECLARE(ui_img_play_png);
LV_IMG_DECLARE(ui_img_next_png);

LV_IMG_DECLARE(ui_img_1139252462);
LV_IMG_DECLARE(ui_img_1596273026);
LV_IMG_DECLARE(ui_img_1837376705);
LV_IMG_DECLARE(ui_img_878056124);
LV_IMG_DECLARE(ui_img_1008068399);
LV_IMG_DECLARE(ui_img_2139845718);
LV_IMG_DECLARE(ui_img_352497594);
LV_IMG_DECLARE(ui_img_1614244405);
LV_IMG_DECLARE(ui_img_1983457674);
LV_IMG_DECLARE(ui_img_204030930);
LV_IMG_DECLARE(ui_img_350297007);
LV_IMG_DECLARE(ui_img_1918312013);

LV_IMG_DECLARE(twocirclingarrows1);
LV_IMG_DECLARE(twocirclingarrows2);


#if !WIN
QueueHandle_t led_setting_queue;
void setTime(int sc, int mn, int hr, int dy, int mt, int yr);
#endif

#if WIN
char wifi_name[30][30] = { "GL-MT1300-44e", "xinyuandianzi", "Xiaomi_F01D", "Xiaomi_33BB", "ysw-office",
                           "LilyGo-AABB", "ysw-office2", "aWiFi-2.4G", "ysw-office3", "ELF305",
                         };
#endif

int system_24hour = 1;

static lv_style_t style_frameless;
//void return_home_cd(lv_event_t* e);
void demo_animation(lv_obj_t *parent, lv_obj_t **obj, lv_coord_t x, lv_coord_t y);
void animation_stop(void);
void animation_sta(void);

static void show_wifiName(lv_obj_t *parent, int len);


void setTime(int sc, int mn, int hr, int dy, int mt, int yr)
{
    struct tm t = {0};        // Initalize to all 0's
    t.tm_year = yr - 1900;    // This is year-1900, so 121 = 2021
    t.tm_mon = mt - 1;
    t.tm_mday = dy;
    t.tm_hour = hr;
    t.tm_min = mn;
    t.tm_sec = sc;
    time_t timeSinceEpoch = mktime(&t);
    struct timeval now = { .tv_sec = timeSinceEpoch };
    settimeofday(&now, NULL);
}

void style_init()
{
    lv_style_init(&style_frameless);
    lv_style_set_radius(&style_frameless, 0);
    lv_style_set_border_width(&style_frameless, 0);
    lv_style_set_bg_color(&style_frameless, lv_color_white());
}

lv_obj_t *ui_home = NULL;
lv_obj_t *ui_header = NULL;
lv_obj_t *ui_body = NULL;
lv_obj_t *ui_home1 = NULL;
lv_obj_t *ui_home2 = NULL;
lv_obj_t *ui_home_electric_quantity_icon = NULL;
lv_obj_t *ui_home_electric_quantity_lable = NULL;
lv_obj_t *ui_Slider2 = NULL;
lv_obj_t *ui_home_wifi_icon = NULL;
lv_obj_t *ui_about_wifi_icon = NULL;
lv_obj_t *ui_datetimeui_wifi_icon = NULL;
lv_obj_t *ui_settings_wifi_icon = NULL;
lv_obj_t *ui_wifiset_wifi_icon = NULL;
lv_obj_t *ui_Image5 = NULL;

lv_obj_t *ui_wificonnect = NULL;
lv_obj_t *ui_wifiscan = NULL;
lv_obj_t *ui_wifilist = NULL;
lv_obj_t *ui_wlansetpannel = NULL;
lv_obj_t *ui_enterwifipass = NULL;
lv_obj_t *ui_wifinamelabel = NULL;
lv_obj_t *ui_wifipassinput = NULL;

lv_obj_t *ui_wifiset_wlanbatteryp = NULL;
lv_obj_t *wifiset_electric_quantity_icon = NULL;

lv_obj_t *ui_footer = NULL;
lv_obj_t *ui_wifirefresh = NULL;

lv_obj_t *ui_wifionlabel = NULL;
lv_obj_t *ui_menudown = NULL;
lv_obj_t *ui_menudowntop = NULL;
lv_obj_t *ui_wifibtn = NULL;
lv_obj_t *ui_cameratext = NULL;
lv_obj_t *ui_controlCenter = NULL;
lv_obj_t *ui_controlListener = NULL;
lv_obj_t *ui_setting = NULL;
lv_obj_t *ui_settings = NULL;
lv_obj_t *ui_wifiset = NULL;

lv_obj_t *ui_camera = NULL;
#if UI_CAMERA_CANVAS 
lv_obj_t *ui_camera_canvas = NULL;
#else
lv_obj_t *ui_camera_img = NULL;
lv_img_dsc_t img_canvas_src;
#endif
lv_timer_t *camera_timer = NULL;
bool camera_get_photo_flag = false;
bool camera_led_open_flag = false;

lv_obj_t *ui_photos = NULL;
lv_obj_t *ui_photos_list = NULL;
lv_obj_t *ui_photos_img_src = NULL;
lv_obj_t *ui_photos_img = NULL;
lv_obj_t *ui_photos_gif = NULL;
lv_obj_t *ui_photos_mbox = NULL;

lv_obj_t *ui_aboutui = NULL;
lv_obj_t *ui_datetimeui = NULL;

lv_obj_t *ui_setdate = NULL;
lv_obj_t *ui_settime = NULL;
lv_obj_t *ui_calendar = NULL;
lv_obj_t *ui_settime_roller = NULL;

lv_obj_t *ui_settime_roller_hour = NULL;
lv_obj_t *ui_settime_roller_min = NULL;

lv_obj_t *ui_time = NULL;
lv_obj_t *ui_settingtime = NULL;
lv_obj_t *ui_datetimeuitime = NULL;
lv_obj_t *ui_wifisettime = NULL;
lv_obj_t *ui_aboutuitime = NULL;
lv_obj_t *ui_time1 = NULL;
lv_obj_t *ui_date = NULL;

lv_obj_t *ui_usb_sats = NULL;
lv_obj_t *ui_battery_voltage = NULL;
lv_obj_t *ui_usb_voltage = NULL;

lv_obj_t *ui_datetimeui_electric_label = NULL;
lv_obj_t *datetimeui_electric_quantity_icon = NULL;

lv_obj_t *ui_aboutui_electric_label = NULL;
lv_obj_t *aboutui_electric_quantity_icon = NULL;

lv_obj_t *setting_electric_quantity_lable = NULL;
lv_obj_t *setting_electric_quantity_icon = NULL;

const char *opts_12h = "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11";
const char *opts_24h = "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23";
const char *opts_min = "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59";

#if !WIN
void updateTime(lv_timer_t *t)
{
    char buf[20] = { 0 };
    struct tm timeinfo;
    time_t now;
    time(&now);
    localtime_r(&now, &timeinfo);
    sprintf(buf, "%02d:%02d", system_24hour ? timeinfo.tm_hour : (timeinfo.tm_hour >= 12 ? timeinfo.tm_hour - 12 : timeinfo.tm_hour), timeinfo.tm_min);
    lv_label_set_text(ui_time, buf);
    lv_label_set_text(ui_settingtime, buf);
    lv_label_set_text(ui_datetimeuitime, buf);
    lv_label_set_text(ui_wifisettime, buf);
    lv_label_set_text(ui_aboutuitime, buf);
}
#endif



void set_usbAndBattery(uint16_t usb_sats, uint16_t usb_voltage, uint16_t battery)
{
    static int percentage_old = 0;
    lv_label_set_text(ui_usb_sats, (usb_sats ? "Connected" : "Disconnect"));
    char buf_usb_voltage[20] = { 0 };
    sprintf(buf_usb_voltage, "%dmV", usb_voltage);
    lv_label_set_text(ui_usb_voltage, (const char *)buf_usb_voltage);
    char buf_battery_voltage[20] = { 0 };
    sprintf(buf_battery_voltage, "%dmV", battery);
    lv_label_set_text(ui_battery_voltage, buf_battery_voltage);

    if (battery - 3400 >= 440) {
        lv_label_set_text(ui_home_electric_quantity_lable, "100%");
        lv_obj_t *boj = lv_obj_get_child(ui_home_electric_quantity_icon, 1);
        lv_obj_set_size(boj, 22 - 6, 12 - 6);

        lv_label_set_text(ui_wifiset_wlanbatteryp, "100%");
        boj = lv_obj_get_child(ui_home_electric_quantity_icon, 1);
        lv_obj_set_size(boj, 22 - 6, 12 - 6);

        lv_label_set_text(ui_datetimeui_electric_label, "100%");
        boj = lv_obj_get_child(datetimeui_electric_quantity_icon, 1);
        lv_obj_set_size(boj, 22 - 6, 12 - 6);

        lv_label_set_text(ui_aboutui_electric_label, "100%");
        boj = lv_obj_get_child(aboutui_electric_quantity_icon, 1);
        lv_obj_set_size(boj, 22 - 6, 12 - 6);

        lv_label_set_text(setting_electric_quantity_lable, "100%");
        boj = lv_obj_get_child(setting_electric_quantity_icon, 1);
        lv_obj_set_size(boj, 22 - 6, 12 - 6);
    } else {
        int percentage = (int)((float((float)(3840 - battery) / (float)440)) * 100);
        percentage = 100 - percentage;
        if (percentage < 0)
            percentage = 0;
        if (!usb_sats) {
            if (!percentage_old || !percentage) {
                percentage_old = percentage;
            } else if (percentage_old < percentage) {
                if (percentage - percentage_old < 10) {
                    return;
                }
            }
        } else {
            percentage_old = percentage;
        }
        char str[20] = { 0 };
        sprintf(str, "%02d%%", percentage);
        lv_label_set_text(ui_home_electric_quantity_lable, str);
        lv_obj_t *boj = lv_obj_get_child(ui_home_electric_quantity_icon, 1);
        lv_obj_set_size(boj, (int)(((float)16) * ((float)percentage / (float)100)), 6);

        lv_label_set_text(ui_wifiset_wlanbatteryp, str);
        boj = lv_obj_get_child(wifiset_electric_quantity_icon, 1);
        lv_obj_set_size(boj, (int)(((float)16) * ((float)percentage / (float)100)), 6);

        lv_label_set_text(ui_datetimeui_electric_label, str);
        boj = lv_obj_get_child(datetimeui_electric_quantity_icon, 1);
        lv_obj_set_size(boj, (int)(((float)16) * ((float)percentage / (float)100)), 6);

        lv_label_set_text(ui_aboutui_electric_label, str);
        boj = lv_obj_get_child(aboutui_electric_quantity_icon, 1);
        lv_obj_set_size(boj, (int)(((float)16) * ((float)percentage / (float)100)), 6);

        lv_label_set_text(setting_electric_quantity_lable, str);
        boj = lv_obj_get_child(setting_electric_quantity_icon, 1);
        lv_obj_set_size(boj, (int)(((float)16) * ((float)percentage / (float)100)), 6);

        percentage_old = percentage;
    }
}

static void backgBut_explain(lv_obj_t *parent, lv_obj_t **obj, char *showData, lv_event_cb_t event_cb, void *event_userData, const void *backg, lv_coord_t x, lv_coord_t y)
{
    int y_skewing = 0;
    lv_obj_t *img_obj = NULL;

    if (backg != NULL) {
        y_skewing = 56;

        if (obj == NULL)
            img_obj = lv_img_create(parent);
        else
            img_obj = *obj = lv_img_create(parent);
        lv_img_set_src(img_obj, backg);
        lv_obj_set_width(img_obj, 54);
        lv_obj_set_height(img_obj, 54);
        lv_obj_set_x(img_obj, x);
        lv_obj_set_y(img_obj, y);
        lv_obj_add_flag(img_obj, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_ADV_HITTEST);
        lv_obj_clear_flag(img_obj, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE |
                          LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM |
                          LV_OBJ_FLAG_SCROLL_CHAIN);
        if (event_cb != NULL) {
            lv_obj_add_event_cb(img_obj, event_cb, LV_EVENT_PRESSED, (void *)1);
        }
    }

    if (showData != NULL) {
        lv_obj_t *label_obj = lv_label_create(parent);
        lv_obj_set_width(label_obj, 68);
        lv_obj_set_height(label_obj, 54);
        lv_obj_set_x(label_obj, x);
        lv_obj_set_y(label_obj, y + y_skewing);
        if (img_obj != NULL) {
            lv_obj_align_to(label_obj, img_obj, LV_ALIGN_CENTER, 0, 54);
            lv_obj_set_style_text_align(label_obj, LV_TEXT_ALIGN_CENTER, 0);
        } else {
            lv_obj_set_x(label_obj, x);
            lv_obj_set_y(label_obj, y + y_skewing);
        }
        lv_label_set_text(label_obj, showData);
        lv_obj_clear_flag(label_obj, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SCROLLABLE |
                          LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    }
}

static void moveup_Animation(lv_obj_t *TargetObject, int delay)
{
    ui_anim_user_data_t *PropertyAnimation_0_user_data = (ui_anim_user_data_t *)lv_mem_alloc(sizeof(ui_anim_user_data_t));
    PropertyAnimation_0_user_data->target = TargetObject;
    PropertyAnimation_0_user_data->val = -1;
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 200);
    lv_anim_set_user_data(&PropertyAnimation_0, PropertyAnimation_0_user_data);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_y);
    lv_anim_set_values(&PropertyAnimation_0, 0, -480);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_deleted_cb(&PropertyAnimation_0, _ui_anim_callback_free_user_data);
    lv_anim_set_playback_time(&PropertyAnimation_0, 0);
    lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_count(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_0, &_ui_anim_callback_get_y);
    lv_anim_start(&PropertyAnimation_0);
}
static void movedown_Animation(lv_obj_t *TargetObject, int delay)
{
    ui_anim_user_data_t *PropertyAnimation_0_user_data = (ui_anim_user_data_t *)lv_mem_alloc(sizeof(ui_anim_user_data_t));
    PropertyAnimation_0_user_data->target = TargetObject;
    PropertyAnimation_0_user_data->val = -1;
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 200);
    lv_anim_set_user_data(&PropertyAnimation_0, PropertyAnimation_0_user_data);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_y);
    lv_anim_set_values(&PropertyAnimation_0, 0, 480);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_deleted_cb(&PropertyAnimation_0, _ui_anim_callback_free_user_data);
    lv_anim_set_playback_time(&PropertyAnimation_0, 0);
    lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_count(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_0, &_ui_anim_callback_get_y);
    lv_anim_start(&PropertyAnimation_0);
}
/*
static void ui_event_header(lv_event_t* e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_BOTTOM)
    {
        movedown_Animation(ui_menudown, 0);
    }
}*/

static void ui_event_wifipanel(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(ui_wifiset, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0);
    }
}
/*
void closeControl_Animation(lv_obj_t* TargetObject, int delay)
{
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 500);
    lv_anim_set_user_data(&PropertyAnimation_0, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_y);
    lv_anim_set_values(&PropertyAnimation_0, 0, -20);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_playback_time(&PropertyAnimation_0, 0);
    lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_count(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_start(&PropertyAnimation_0);
    lv_anim_t PropertyAnimation_1;
    lv_anim_init(&PropertyAnimation_1);
    lv_anim_set_time(&PropertyAnimation_1, 100);
    lv_anim_set_user_data(&PropertyAnimation_1, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_1, _ui_anim_callback_set_y);
    lv_anim_set_values(&PropertyAnimation_1, -20, -480);
    lv_anim_set_path_cb(&PropertyAnimation_1, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_1, delay + 500);
    lv_anim_set_playback_time(&PropertyAnimation_1, 0);
    lv_anim_set_playback_delay(&PropertyAnimation_1, 0);
    lv_anim_set_repeat_count(&PropertyAnimation_1, 0);
    lv_anim_set_repeat_delay(&PropertyAnimation_1, 0);
    lv_anim_set_early_apply(&PropertyAnimation_1, false);
    lv_anim_start(&PropertyAnimation_1);

}
*/

static void ui_photos_img_src_event(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        lv_scr_load(ui_photos);
    }
}

static void delete_img_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    char *user_data = (char *)lv_event_get_user_data(e);
    char *operate = (char *)lv_msgbox_get_active_btn_text(obj);

    if(strcmp(operate, "confirm") == 0){
        sd_card_remove(user_data);
        lv_scr_load(ui_photos);
    }else if(strcmp(operate, "cancel") == 0){
        lv_scr_load(ui_photos);
    }
    lv_obj_del(obj);
}

static void ui_photos_list_event(lv_event_t *e)
{   
    static char path[32];
    static lv_event_code_t prev_code = LV_EVENT_ALL;
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    char *file_name = (char *)lv_list_get_btn_text(ui_photos_list, obj);
    uint16_t file_name_len = strlen(file_name);
    char *suffix = file_name + file_name_len - 3;
    
    
    if(code == LV_EVENT_CLICKED) {
        if(prev_code == LV_EVENT_LONG_PRESSED){
            prev_code = LV_EVENT_ALL;
            return;
        }

        lv_snprintf(path, 32, "S:/%s", file_name);
        int picture_type = -1;
        picture_type = strcmp(suffix, "jpg") == 0 ? 1 : picture_type;
        picture_type = strcmp(suffix, "png") == 0 ? 2 : picture_type;
        picture_type = strcmp(suffix, "bmp") == 0 ? 3 : picture_type;
        picture_type = strcmp(suffix, "gif") == 0 ? 4 : picture_type;

        lv_obj_clear_flag(ui_photos_gif, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui_photos_img, LV_OBJ_FLAG_HIDDEN);
    
        switch (picture_type) {
            case 1: // jpg
            case 2: // png
            case 3: // bmp
                lv_obj_set_style_transform_angle(ui_photos_img, 0, 0);
                lv_img_set_src(ui_photos_img, path);
                lv_obj_add_flag(ui_photos_gif, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(ui_photos_img, LV_OBJ_FLAG_HIDDEN);
                lv_scr_load(ui_photos_img_src);
                break;
            case 4: // gif
                lv_gif_set_src(ui_photos_gif, path);
                lv_obj_clear_flag(ui_photos_gif, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(ui_photos_img, LV_OBJ_FLAG_HIDDEN);
                lv_scr_load(ui_photos_img_src);
                break;
            default:
                break;
        }
        Serial.println(path);

    }else if(code == LV_EVENT_LONG_PRESSED) {
        static const char * btns[] = {"confirm", "cancel", ""};
        char prompt_buf[48];
        prev_code = LV_EVENT_LONG_PRESSED;
        lv_snprintf(prompt_buf, 48, "Do you want to delete %s", file_name);
        lv_snprintf(path, 32, "%s%s", TARGET_FOLDER, file_name);
        ui_photos_mbox = lv_msgbox_create(ui_photos_img_src, "Info", prompt_buf, btns, true);
        lv_obj_add_event_cb(ui_photos_mbox, delete_img_event_cb, LV_EVENT_VALUE_CHANGED, path);
        lv_obj_center(ui_photos_mbox);
        lv_scr_load(ui_photos_img_src);
        lv_obj_add_flag(ui_photos_gif, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_photos_img, LV_OBJ_FLAG_HIDDEN);
    }
}
static void ui_event_photos(lv_event_t *e)
{
#define FILE_INFO_MAX 18

    if(sd_card_get_init_flag() == false){
        Serial.println("No find SD card!");
        return;
    }

    lv_event_code_t event_code = lv_event_get_code(e);

    if(ui_photos_list != NULL){
        lv_obj_del(ui_photos_list);
        ui_photos_list = NULL;
    }
    ui_photos_list = lv_list_create(ui_photos);
    lv_obj_set_size(ui_photos_list, lv_pct(100), lv_pct(100));
    lv_obj_set_style_radius(ui_photos_list, 0, 0);
    lv_obj_set_style_pad_row(ui_photos_list, 10, 0);
    lv_obj_center(ui_photos_list);

    /*Add buttons to the list*/
    lv_obj_t *obj = lv_list_add_text(ui_photos_list, "File");
    lv_obj_set_height(obj, 40);
    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_pad_top(obj, 12, 0);
    
    if (event_code == LV_EVENT_PRESSED) {
        char buf[FILE_INFO_MAX];

        File root = SD_FD_DRI.open(TARGET_FOLDER);
        if(!root){
            Serial.println("Failed to open directory");
            return;
        }
        if(!root.isDirectory()){
            Serial.println("Not a directory");
            return;
        }

        File file = root.openNextFile();
        while(file){
            lv_obj_t *btn;

            lv_snprintf(buf, FILE_INFO_MAX, "%s", file.name());
            if(file.isDirectory()){
                btn = lv_list_add_btn(ui_photos_list, LV_SYMBOL_DIRECTORY, buf);
            }
            else 
                btn = lv_list_add_btn(ui_photos_list, LV_SYMBOL_FILE, buf);
            lv_obj_add_event_cb(btn, ui_photos_list_event, LV_EVENT_CLICKED, NULL);
            lv_obj_add_event_cb(btn, ui_photos_list_event, LV_EVENT_LONG_PRESSED, NULL);
            file = root.openNextFile();
        }
        // _ui_screen_change(ui_photos, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0);
        lv_scr_load(ui_photos);
    }
#undef FILE_INFO_MAX
}

static void ui_event_settings(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(ui_setting, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0);
    }

    if (event_code == LV_EVENT_PRESSED) {
        _ui_screen_change(ui_setting, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0);
    }
}


void cmaera_async_play(void *p)
{
}

static void px_swap(uint8_t *a, uint8_t *b)
{
    uint8_t c = *a;
    *a = *b;
    *b = c;
}

static void camera_video_play(lv_timer_t *t)
{
    // lv_async_call(cmaera_async_play, NULL);
    camera_fb_t *frame = esp_camera_fb_get();
    if (frame){
        sensor_t *s = esp_camera_sensor_get();
        ESP_LOGI("", "id=%d, w=%d, h=%d, len=%d", s->id.PID, frame->width, frame->height, frame->len);
#if UI_CAMERA_CANVAS 

        if(s->id.PID == GC0308_PID){
            for(int i = 0; i < frame->len / 2; i++){
                px_swap(&frame->buf[i], &frame->buf[frame->len-i]);
            }
        }
        
        lv_canvas_set_buffer(ui_camera_canvas, frame->buf, frame->height, frame->width, LV_IMG_CF_TRUE_COLOR);

#else
        img_canvas_src.header.cf = LV_IMG_CF_RAW;
        img_canvas_src.header.always_zero = 0;
        img_canvas_src.header.reserved = 0;
        img_canvas_src.header.w = frame->width;
        img_canvas_src.header.h = frame->height;
        img_canvas_src.data_size = frame->len;
        img_canvas_src.data = frame->buf;
        if(frame->len < 5000)
            lv_img_set_src(ui_camera_img, &img_canvas_src);
#endif
        if(camera_get_photo_flag){
            camera_get_photo_flag = false;
            if(camera_led_open_flag){
                ledcWrite(LEDC_WHITE_CH, 20);
                delay(400);
                ledcWrite(LEDC_WHITE_CH, 0);
            }
            
            if(sd_card_get_init_flag()){
                static uint16_t img_idx = 1000;
                char path[32];
                lv_snprintf(path,32, "%s%s%d%s", TARGET_FOLDER, "/img", img_idx++, ".bmp");
                // sd_card_write(path, frame->buf, frame->len);
                // sd_card_bmp_img(path, frame->width, frame->height, frame->buf);
                sd_card_bmp_lvgl(path, frame->width, frame->height, ui_camera_canvas);
            }else{
                Serial.println("No find SD card!");
            }
        }
        esp_camera_fb_return(frame);
    }
}

void open_camera_cb(lv_event_t *e)
{
    static bool click_verse = false;
    int data = (int)lv_event_get_user_data(e);

#define CAMERA_TAKE 1
#define CAMERA_LED 2
#define CAMERA_AF 3
#define SAVE_TO_SD_CARD 4

    if (data == CAMERA_TAKE){
        camera_get_photo_flag = true;
    }

    if(data == CAMERA_LED)
        camera_led_open_flag = !camera_led_open_flag;

    if(data == CAMERA_AF) {
        extern OV5640 ov5640;
        uint8_t rc = ov5640.getFWStatus();
        if (rc == -1) {
            Serial.println("Check your OV5640");
        } else if (rc == FW_STATUS_S_FOCUSED) {
            Serial.println("Focused!");
        } else if (rc == FW_STATUS_S_FOCUSING) {
            Serial.println("Focusing!");
        } else {
            Serial.println("Autofoucs succeed!");
        }
    }

    if(data == SAVE_TO_SD_CARD){
        if(sd_card_get_init_flag() == false){
            Serial.println("No find SD card!");
            return;
        }
    }
}

static void ui_event_camera(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_PRESSED) {
            Serial.println("camera icon is clicked");
        _ui_screen_change(ui_camera, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0);
        if(camera_timer)
            lv_timer_resume(camera_timer);
    }
}
/*
void showControl_Animation(lv_obj_t* TargetObject, int delay)
{
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 500);
    lv_anim_set_user_data(&PropertyAnimation_0, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_y);
    lv_anim_set_values(&PropertyAnimation_0, -20, 0);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 100);
    lv_anim_set_playback_time(&PropertyAnimation_0, 0);
    lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_count(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_start(&PropertyAnimation_0);
    lv_anim_t PropertyAnimation_1;
    lv_anim_init(&PropertyAnimation_1);
    lv_anim_set_time(&PropertyAnimation_1, 100);
    lv_anim_set_user_data(&PropertyAnimation_1, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_1, _ui_anim_callback_set_y);
    lv_anim_set_values(&PropertyAnimation_1, -480, -20);
    lv_anim_set_path_cb(&PropertyAnimation_1, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_1, delay + 0);
    lv_anim_set_playback_time(&PropertyAnimation_1, 0);
    lv_anim_set_playback_delay(&PropertyAnimation_1, 0);
    lv_anim_set_repeat_count(&PropertyAnimation_1, 0);
    lv_anim_set_repeat_delay(&PropertyAnimation_1, 0);
    lv_anim_set_early_apply(&PropertyAnimation_1, false);
    lv_anim_start(&PropertyAnimation_1);

}
*/
static void ui_event_controlListener(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_BOTTOM) {
        movedown_Animation(ui_controlCenter, 0);
    }
}
static void ui_event_controlCenter(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_TOP) {
        moveup_Animation(ui_controlCenter, 0);
    }
}
/*
void ui_event_menudown(lv_event_t* e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_TOP)
    {
        moveup_Animation(ui_menudown, 0);
    }
}*/

static void electric_quantity_icon_create(lv_obj_t **ui_obj, lv_obj_t *parent, lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h)
{
    *ui_obj = lv_obj_create(parent);
    lv_obj_set_style_pad_all(*ui_obj, 0, 0);
    lv_obj_set_size(*ui_obj, w + 6, h + 5);
    lv_obj_set_pos(*ui_obj, x, y);
    lv_obj_set_style_bg_opa(*ui_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(*ui_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *border = lv_obj_create(*ui_obj);
    lv_obj_set_style_border_width(border, 2, 0);
    lv_obj_set_style_radius(border, 2, 0);
    lv_obj_clear_flag(border, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(border, w, h);
    lv_obj_set_pos(border, 0, 0);
    lv_obj_set_style_bg_opa(border, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(border, lv_color_hex(0xffffff), 0);
    lv_obj_t *pad = lv_obj_create(*ui_obj);
    lv_obj_set_style_outline_width(pad, 0, 0);
    lv_obj_set_style_outline_pad(pad, 0, 0);
    lv_obj_set_style_border_width(pad, 0, 0);
    lv_obj_set_style_bg_color(pad, lv_color_hex(0xffffff), 0);
    lv_obj_set_size(pad, w - 6, h - 6);
    lv_obj_set_style_border_width(pad, 0, 0);
    lv_obj_set_style_radius(pad, 0, 0);
    lv_obj_align(pad, LV_ALIGN_LEFT_MID, 3, 0);
    lv_obj_clear_flag(pad, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *line = lv_obj_create(*ui_obj);
    lv_obj_set_style_radius(line, 0, 0);
    lv_obj_set_size(line, 2, h - 6);
    lv_obj_align(line, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_border_color(line, lv_color_hex(0xffffff), 0);
}

static void ui_label_create(lv_obj_t **ui_label, lv_obj_t *parent, char *showData, lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h)
{
    *ui_label = lv_label_create(parent);
    lv_obj_set_width(*ui_label, w);
    lv_obj_set_height(*ui_label, h);
    lv_obj_set_x(*ui_label, x);
    lv_obj_set_y(*ui_label, y);
    lv_label_set_text(*ui_label, showData);
    lv_obj_set_style_text_align(*ui_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(*ui_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(*ui_label, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SCROLLABLE |
                      LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
}

static void ui_img_create(lv_obj_t **ui_obj, lv_obj_t *parent, const void *src, lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h)
{
    *ui_obj = lv_img_create(parent);
    lv_img_set_src(*ui_obj, src);
    lv_obj_set_width(*ui_obj, w);
    lv_obj_set_height(*ui_obj, h);
    lv_obj_set_x(*ui_obj, x);
    lv_obj_set_y(*ui_obj, y);
    lv_obj_add_flag(*ui_obj, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(*ui_obj, LV_OBJ_FLAG_SCROLLABLE);
}

static void ui_img_bgColour_create(lv_obj_t **ui_obj, lv_obj_t *parent, const void *src, lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h)
{
    *ui_obj = lv_img_create(parent);
    lv_img_set_src(*ui_obj, src);
    lv_obj_set_width(*ui_obj, w);
    lv_obj_set_height(*ui_obj, h);
    lv_obj_set_x(*ui_obj, x);
    lv_obj_set_y(*ui_obj, y);
    lv_obj_add_flag(*ui_obj, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(*ui_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(*ui_obj, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(*ui_obj, lv_color_hex(0x323232), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(*ui_obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(*ui_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_color(*ui_obj, lv_color_hex(0x323232), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_opa(*ui_obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(*ui_obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_pad(*ui_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(*ui_obj, 16, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(*ui_obj, lv_color_hex(0xC9A520), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(*ui_obj, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_color(*ui_obj, lv_color_hex(0xC9A520), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_opa(*ui_obj, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_width(*ui_obj, 10, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_pad(*ui_obj, 0, LV_PART_MAIN | LV_STATE_CHECKED);
}

static void lv_obj_t_create(lv_obj_t **ui_obj, lv_obj_t *parent, lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h)
{
    *ui_obj = lv_obj_create(parent);
    lv_obj_set_width(*ui_obj, w);
    lv_obj_set_height(*ui_obj, h);
    lv_obj_set_x(*ui_obj, x);
    lv_obj_set_y(*ui_obj, y);
    lv_obj_set_style_pad_all(*ui_obj, 0, 0);
    lv_obj_set_style_radius(*ui_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(*ui_obj, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(*ui_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(*ui_obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
}


int wifi_scanNetworks = 0;
static void ui_event_wlanswitch(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_VALUE_CHANGED && lv_obj_has_state(target, LV_STATE_CHECKED)) {
        _ui_flag_modify(ui_wificonnect, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        _ui_flag_modify(ui_wifiscan, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        lv_label_set_text(ui_wifionlabel, "Open");
#if !WIN
        if (wifi_scanNetworks <= 0) {
            WiFi.mode(WIFI_STA);
            WiFi.disconnect();
            wifi_scanNetworks = WiFi.scanNetworks();
            printf("!WIN %d\n", wifi_scanNetworks);
            show_wifiName(ui_wifilist, wifi_scanNetworks);
            printf("lv_obj_has_state\n");
        }
#else
        if (wifi_scanNetworks <= 0) {
            wifi_scanNetworks = 10;
            show_wifiName(ui_wifilist, wifi_scanNetworks);
            printf("lv_obj_has_state\n");
        }
#endif
    }
    if (event_code == LV_EVENT_VALUE_CHANGED && !lv_obj_has_state(target, LV_STATE_CHECKED)) {
        _ui_flag_modify(ui_wificonnect, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        _ui_flag_modify(ui_wifiscan, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        lv_obj_add_flag(ui_home_wifi_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_wifiset_wifi_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_settings_wifi_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_about_wifi_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_datetimeui_wifi_icon, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(ui_wifionlabel, "Closed");
        lv_obj_set_style_bg_color(ui_Image5, lv_color_hex(0x323232), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_outline_color(ui_Image5, lv_color_hex(0x323232), LV_PART_MAIN | LV_STATE_DEFAULT);

        while (wifi_scanNetworks >= 0) {
            lv_obj_t *label = lv_obj_get_child(ui_wifilist, wifi_scanNetworks);
            if (label != NULL) {
                lv_obj_del(label);
            }
            wifi_scanNetworks--;
        }
    }
}

void ui_event_headerbackico(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(ui_setting, LV_SCR_LOAD_ANIM_OVER_RIGHT, 200, 0);
    }
}

void ui_event_wifilistname(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED) {
        lv_obj_t *label = lv_obj_get_child(target, 1);
        char *text_t = NULL;
        if (label != NULL) {
            text_t = lv_label_get_text(label);
            //printf("ui_event_wifilistname:%s\n", text_t);
        }
        lv_label_set_text(ui_wifinamelabel, text_t);
        moveup_Animation(ui_enterwifipass, 0);
    }
}

static void show_wifiName(lv_obj_t *parent, int len)
{
    int i = 0, x = 0, y = 0;
    for (i = 0; i < len; i++) {
        lv_obj_t *ui_wifilist = lv_obj_create(parent);
        lv_obj_set_width(ui_wifilist, 200);
        lv_obj_set_height(ui_wifilist, 40);
        lv_obj_set_pos(ui_wifilist, x, y);

        lv_obj_set_style_radius(ui_wifilist, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(ui_wifilist, lv_color_hex(0xE6E6E6), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(ui_wifilist, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(ui_wifilist, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_all(ui_wifilist, 0, 0);
        lv_obj_add_flag(ui_wifilist, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CHECKABLE);

        lv_obj_t *ui_wifisymbol1 = lv_label_create(ui_wifilist);
        lv_obj_set_width(ui_wifisymbol1, LV_SIZE_CONTENT);
        lv_obj_set_height(ui_wifisymbol1, LV_SIZE_CONTENT);
        lv_obj_set_align(ui_wifisymbol1, LV_ALIGN_LEFT_MID);
        lv_label_set_text(ui_wifisymbol1, "");
        lv_obj_set_style_text_color(ui_wifisymbol1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(ui_wifisymbol1, &ui_font_symbols16, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t *ui_wifilistname = lv_label_create(ui_wifilist);
        lv_obj_set_width(ui_wifilistname, 170);
        lv_obj_set_height(ui_wifilistname, LV_SIZE_CONTENT);
        lv_obj_set_x(ui_wifilistname, 30);
        lv_obj_set_y(ui_wifilistname, 0);
        lv_obj_set_style_text_color(ui_wifilistname, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_align(ui_wifilistname, LV_ALIGN_LEFT_MID);
        lv_label_set_long_mode(ui_wifilistname, LV_LABEL_LONG_DOT);

#if WIN
        printf("%s\n", &wifi_name[i][0]);
        lv_label_set_text(ui_wifilistname, (const char *)&wifi_name[i][0]);
#else
        //printf("%s\n", WiFi.SSID(i));
        char c[31] = { 0 };
        strncpy(c, WiFi.SSID(i).c_str(), 30);
        //const char* c = WiFi.SSID(i).c_str();
        lv_label_set_text(ui_wifilistname, (const char *)c);
        //printf("c:%s\n", c);
#endif
        lv_obj_t *ui_wifilock = lv_label_create(ui_wifilist);
        lv_obj_set_width(ui_wifilock, LV_SIZE_CONTENT);
        lv_obj_set_height(ui_wifilock, LV_SIZE_CONTENT);
        lv_obj_set_x(ui_wifilock, -20);
        lv_obj_set_y(ui_wifilock, 0);
        lv_obj_set_align(ui_wifilock, LV_ALIGN_RIGHT_MID);
        lv_obj_set_style_text_color(ui_wifilock, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(ui_wifilock, "");
        lv_obj_set_style_text_font(ui_wifilock, &ui_font_symbols16, LV_PART_MAIN | LV_STATE_DEFAULT);

        lv_obj_t *ui_wifilistright = lv_label_create(ui_wifilist);
        lv_obj_set_width(ui_wifilistright, LV_SIZE_CONTENT);
        lv_obj_set_height(ui_wifilistright, LV_SIZE_CONTENT);
        lv_obj_set_align(ui_wifilistright, LV_ALIGN_RIGHT_MID);
        lv_label_set_text(ui_wifilistright, "");
        lv_obj_set_style_text_color(ui_wifilistright, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(ui_wifilistright, &ui_font_symbols16, LV_PART_MAIN | LV_STATE_DEFAULT);
        y += 40;
        lv_obj_add_event_cb(ui_wifilist, ui_event_wifilistname, LV_EVENT_ALL, NULL);
    }
}

void wifi_connect_cd(void)
{
#if !WIN
    lv_obj_t *connection_status = lv_obj_get_child(ui_wificonnect, 2);
    if (WiFi.status() == WL_CONNECTED) {
        printf("wifi ip: %s\n", WiFi.localIP().toString().c_str());
        if (connection_status != NULL)
            lv_label_set_text(connection_status, "connect");
        lv_obj_set_style_bg_color(ui_Image5, lv_color_hex(0x0000ff), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_outline_color(ui_Image5, lv_color_hex(0x0000ff), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_clear_flag(ui_home_wifi_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui_about_wifi_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui_settings_wifi_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui_wifiset_wifi_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(ui_datetimeui_wifi_icon, LV_OBJ_FLAG_HIDDEN);
        animation_stop();
    } else {
        lv_obj_add_flag(ui_home_wifi_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_about_wifi_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_settings_wifi_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_wifiset_wifi_icon, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_datetimeui_wifi_icon, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(connection_status, "connect failed!!!");
        animation_stop();
    }
#endif
}

void ui_event_connectbtn(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        movedown_Animation(ui_enterwifipass, 0);

        lv_obj_t *wifi_name = lv_obj_get_child(ui_wificonnect, 1);

        char *wifiname_t = NULL;
        char *wifipass_t = NULL;
        if (wifi_name != NULL) {
            wifiname_t = (char *)lv_label_get_text(ui_wifinamelabel);
            printf("wifi name:%s\n", wifiname_t);
            wifipass_t = (char *)lv_textarea_get_text(ui_wifipassinput);
            printf("wifi pass:%s\n", wifipass_t);

#if WIN
            lv_obj_t *connection_status = lv_obj_get_child(ui_wificonnect, 2);
            lv_label_set_text(wifi_name, wifiname_t);
            lv_label_set_text(connection_status, "connect");
            lv_obj_clear_flag(ui_home_wifi_icon, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui_settings_wifi_icon, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui_wifiset_wifi_icon, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui_about_wifi_icon, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui_datetimeui_wifi_icon, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_style_bg_color(ui_Image5, lv_color_hex(0x0000ff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_outline_color(ui_Image5, lv_color_hex(0x0000ff), LV_PART_MAIN | LV_STATE_DEFAULT);
            animation_sta();
#else
            WiFi.disconnect();
            lv_label_set_text(wifi_name, wifiname_t);
            WiFi.begin(wifiname_t, wifipass_t);
            animation_sta();
#endif
        }
    }
}
void ui_event_canceltbtn(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        movedown_Animation(ui_enterwifipass, 0);
    }
}

void ui_event_wifiset(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT) {
        _ui_screen_change(ui_setting, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0);
    }
}

void ui_event_wifipassinput(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_DEFOCUSED) {
        movedown_Animation(ui_enterwifipass, 0);
    }
}

void ui_wifiset_screen_init(void)
{
    ui_wifiset = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_wifiset, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_wifiset, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_wifiset, lv_color_hex(0xE6E6E6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_wifiset, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_wifiset, &ui_font_misans16, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_wlanheader = lv_obj_create(ui_wifiset);
    lv_obj_set_width(ui_wlanheader, 222);
    lv_obj_set_height(ui_wlanheader, 60);
    lv_obj_set_align(ui_wlanheader, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_wlanheader, LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_wlanheader, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_wlanheader, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_wlanheader, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_wlanheader, lv_color_hex(0xB4B4B4), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_wlanheader, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_wlanheader, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_wlanheader, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_wlanheader, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_wlanheader, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_wlanheader, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_wlanheader, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_wlanheader, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_wlanheader, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_wlanheader, &ui_font_misans16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_wifiset_wifi_icon = lv_label_create(ui_wlanheader);
    lv_obj_set_width(ui_wifiset_wifi_icon, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_wifiset_wifi_icon, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_wifiset_wifi_icon, 5);
    lv_obj_set_y(ui_wifiset_wifi_icon, 5);
    lv_label_set_text(ui_wifiset_wifi_icon, "");
    lv_obj_add_flag(ui_wifiset_wifi_icon, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_text_font(ui_wifiset_wifi_icon, &ui_font_symbols16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_wifisettime = NULL;
    ui_label_create(&ui_wifisettime, ui_wlanheader, (char *)"12:12", 0, 7, 60, 30);
    lv_obj_set_style_text_color(ui_wifisettime, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_align(ui_wifisettime, LV_ALIGN_TOP_MID);

    ui_wifiset_wlanbatteryp = NULL;
    ui_label_create(&ui_wifiset_wlanbatteryp, ui_wlanheader, (char *)"100%", 155, 5, 40, 30);
    lv_obj_set_style_text_color(ui_wifiset_wlanbatteryp, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    wifiset_electric_quantity_icon = NULL;
    electric_quantity_icon_create(&wifiset_electric_quantity_icon, ui_wlanheader, 193, 7, 22, 12);
    lv_obj_t *obj_child = lv_obj_get_child(wifiset_electric_quantity_icon, 0);
    lv_obj_set_style_border_color(obj_child, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_color(obj_child, lv_color_hex(0x000000), 0);
    lv_obj_t *obj_child1 = lv_obj_get_child(wifiset_electric_quantity_icon, 1);
    lv_obj_set_style_border_color(obj_child1, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_color(obj_child1, lv_color_hex(0x000000), 0);
    lv_obj_t *obj_child2 = lv_obj_get_child(wifiset_electric_quantity_icon, 2);
    lv_obj_set_style_border_color(obj_child2, lv_color_hex(0x000000), 0);

    lv_obj_t *ui_wlantitle = lv_label_create(ui_wlanheader);
    lv_obj_set_width(ui_wlantitle, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_wlantitle, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_wlantitle, 30);
    lv_obj_set_y(ui_wlantitle, -2);
    lv_obj_set_style_text_color(ui_wlantitle, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_align(ui_wlantitle, LV_ALIGN_BOTTOM_LEFT);
    lv_label_set_text(ui_wlantitle, "WLAN");

    lv_obj_t *ui_headerbackico = lv_label_create(ui_wlanheader);
    lv_obj_set_width(ui_headerbackico, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_headerbackico, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_headerbackico, 10);
    lv_obj_set_y(ui_headerbackico, -3);
    lv_obj_set_align(ui_headerbackico, LV_ALIGN_BOTTOM_LEFT);
    lv_label_set_text(ui_headerbackico, "");
    lv_obj_add_flag(ui_headerbackico, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_text_font(ui_headerbackico, &ui_font_symbols16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_wlansetpannel = lv_obj_create(ui_wifiset);
    lv_obj_set_width(ui_wlansetpannel, 200);
    lv_obj_set_height(ui_wlansetpannel, 420);
    lv_obj_set_x(ui_wlansetpannel, 0);
    lv_obj_set_y(ui_wlansetpannel, 60);
    lv_obj_set_align(ui_wlansetpannel, LV_ALIGN_TOP_MID);
    lv_obj_set_flex_flow(ui_wlansetpannel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_wlansetpannel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(ui_wlansetpannel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_wlansetpannel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_wlansetpannel, lv_color_hex(0xE6E6E6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_wlansetpannel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_wlansetpannel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_wlanswitchpanel = lv_obj_create(ui_wlansetpannel);
    lv_obj_set_width(ui_wlanswitchpanel, 200);
    lv_obj_set_height(ui_wlanswitchpanel, 40);
    lv_obj_set_align(ui_wlanswitchpanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_wlanswitchpanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_wlanswitchpanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_wlanswitchpanel, lv_color_hex(0xE6E6E6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_wlanswitchpanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_wlanswitchpanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_wlanswitchpanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_wlanswitchpanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_wlanswitchpanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_wlanswitchpanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_wlansetlabel = lv_label_create(ui_wlanswitchpanel);
    lv_obj_set_width(ui_wlansetlabel, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_wlansetlabel, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_wlansetlabel, 0);
    lv_obj_set_y(ui_wlansetlabel, 4);
    lv_obj_set_style_text_color(ui_wlansetlabel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(ui_wlansetlabel, "WLAN");

    lv_obj_t *ui_wlanswitch = lv_switch_create(ui_wlanswitchpanel);
    lv_obj_set_width(ui_wlanswitch, 45);
    lv_obj_set_height(ui_wlanswitch, 24);
    lv_obj_set_align(ui_wlanswitch, LV_ALIGN_TOP_RIGHT);

    lv_obj_set_style_bg_color(ui_wlanswitch, lv_color_hex(0xAAAAAA), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_wlanswitch, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_wlanswitch, lv_color_hex(0x00D200), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui_wlanswitch, 255, LV_PART_INDICATOR | LV_STATE_CHECKED);

    ui_wificonnect = lv_obj_create(ui_wlansetpannel);
    lv_obj_set_width(ui_wificonnect, 200);
    lv_obj_set_height(ui_wificonnect, 70);
    lv_obj_set_x(ui_wificonnect, 0);
    lv_obj_set_y(ui_wificonnect, 45);
    lv_obj_set_align(ui_wificonnect, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_wificonnect, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_wificonnect, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_wificonnect, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_wificonnect, lv_color_hex(0x0078F5), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_wificonnect, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_wificonnect, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_wifisymbol = lv_label_create(ui_wificonnect);
    lv_obj_set_width(ui_wifisymbol, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_wifisymbol, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_wifisymbol, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_wifisymbol, "");
    lv_obj_set_style_text_color(ui_wifisymbol, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_wifisymbol, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_wifisymbol, &ui_font_symbols16, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_wifiname = lv_label_create(ui_wificonnect);
    lv_obj_set_width(ui_wifiname, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_wifiname, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_wifiname, 35);
    lv_obj_set_y(ui_wifiname, 0);
    lv_label_set_text(ui_wifiname, "LilyGo");
    lv_obj_set_style_text_color(ui_wifiname, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_wifiname, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_wifistats = lv_label_create(ui_wificonnect);
    lv_obj_set_width(ui_wifistats, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_wifistats, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_wifistats, 35);
    lv_obj_set_y(ui_wifistats, 0);
    lv_obj_set_align(ui_wifistats, LV_ALIGN_BOTTOM_LEFT);
    lv_label_set_text(ui_wifistats, "ununited");
    lv_obj_set_style_text_color(ui_wifistats, lv_color_hex(0xB4B4B4), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_wifistats, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_wificlock = lv_label_create(ui_wificonnect);
    lv_obj_set_width(ui_wificlock, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_wificlock, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_wificlock, -20);
    lv_obj_set_y(ui_wificlock, 0);
    lv_obj_set_align(ui_wificlock, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_wificlock, "");
    lv_obj_set_style_text_color(ui_wificlock, lv_color_hex(0xDFDFDF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_wificlock, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_wificlock, &ui_font_symbols16, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_wificonright = lv_label_create(ui_wificonnect);
    lv_obj_set_width(ui_wificonright, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_wificonright, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_wificonright, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_wificonright, "");
    lv_obj_set_style_text_color(ui_wificonright, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_wificonright, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_wificonright, &ui_font_symbols16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_wifiscan = lv_obj_create(ui_wlansetpannel);
    lv_obj_set_width(ui_wifiscan, 200);
    lv_obj_set_height(ui_wifiscan, 30);
    lv_obj_set_x(ui_wifiscan, 0);
    lv_obj_set_y(ui_wifiscan, 125);
    lv_obj_set_align(ui_wifiscan, LV_ALIGN_LEFT_MID);
    lv_obj_set_flex_flow(ui_wifiscan, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_wifiscan, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_add_flag(ui_wifiscan, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_wifiscan, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_wifiscan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_wifiscan, lv_color_hex(0xE6E6E6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_wifiscan, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_wifiscan, lv_color_hex(0xB4B4B4), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_wifiscan, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_wifiscan, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_wifiscan, LV_BORDER_SIDE_TOP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_wifiscan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_wifiscan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_wifiscan, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_wifiscan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_wifiselecttitle = lv_label_create(ui_wifiscan);
    lv_obj_set_height(ui_wifiselecttitle, LV_SIZE_CONTENT);
    lv_obj_set_flex_grow(ui_wifiselecttitle, 1);
    lv_obj_set_align(ui_wifiselecttitle, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_wifiselecttitle, "Select nearby WLAN");
    lv_obj_set_style_text_color(ui_wifiselecttitle, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_wifiselecttitle, &ui_font_misans16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_wifiselecttitle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_wifirefresh = NULL;
    demo_animation(ui_wifiscan, &ui_wifirefresh, 180, 125);

    ui_wifilist = lv_obj_create(ui_wlansetpannel);
    lv_obj_set_width(ui_wifilist, 200);
    lv_obj_set_height(ui_wifilist, 220);

    lv_obj_set_style_radius(ui_wifilist, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_wifilist, lv_color_hex(0xE6E6E6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_wifilist, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_wifilist, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui_wifilist, 0, 0);
    lv_obj_set_pos(ui_wifilist, 0, 280);

    ui_enterwifipass = lv_obj_create(ui_wifiset);
    lv_obj_set_width(ui_enterwifipass, 222);
    lv_obj_set_height(ui_enterwifipass, 350);
    lv_obj_set_x(ui_enterwifipass, 0);
    lv_obj_set_y(ui_enterwifipass, 610);
    lv_obj_set_align(ui_enterwifipass, LV_ALIGN_TOP_MID);
    lv_obj_set_flex_flow(ui_enterwifipass, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_enterwifipass, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(ui_enterwifipass, LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_enterwifipass, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_enterwifipass, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_enterwifipass, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_enterwifipass, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_passpannel = lv_obj_create(ui_enterwifipass);
    lv_obj_set_width(ui_passpannel, 222);
    lv_obj_set_height(ui_passpannel, 170);
    lv_obj_set_align(ui_passpannel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_passpannel, LV_OBJ_FLAG_SCROLLABLE);

    ui_wifinamelabel = lv_label_create(ui_passpannel);
    lv_obj_set_width(ui_wifinamelabel, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_wifinamelabel, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_wifinamelabel, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_wifinamelabel, "LilyGo");
    lv_obj_set_style_text_font(ui_wifinamelabel, &ui_font_misans16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_wifipassinput = lv_textarea_create(ui_passpannel);
    lv_obj_set_width(ui_wifipassinput, 200);
    lv_obj_set_height(ui_wifipassinput, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_wifipassinput, 0);
    lv_obj_set_y(ui_wifipassinput, -10);
    lv_obj_set_align(ui_wifipassinput, LV_ALIGN_CENTER);
    lv_textarea_set_max_length(ui_wifipassinput, 64);
    lv_textarea_set_placeholder_text(ui_wifipassinput, "Password");
    lv_textarea_set_one_line(ui_wifipassinput, true);

    lv_textarea_set_password_mode(ui_wifipassinput, false);
    lv_obj_clear_flag(ui_wifipassinput, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM |
                      LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_border_color(ui_wifipassinput, lv_color_hex(0x0079F6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_wifipassinput, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_wifipassinput, 1, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_connectbtn = lv_btn_create(ui_passpannel);
    lv_obj_set_width(ui_connectbtn, 80);
    lv_obj_set_height(ui_connectbtn, 40);
    lv_obj_set_x(ui_connectbtn, 10);
    lv_obj_set_y(ui_connectbtn, 0);
    lv_obj_set_align(ui_connectbtn, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_add_flag(ui_connectbtn, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_connectbtn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_connectbtn, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_connectbtn, lv_color_hex(0xE6E6E6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_connectbtn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_connectbtn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_connectbtn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_connectbtn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_connectbtn, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_connectbtn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_connectbtn, 50, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(ui_connectbtn, lv_color_hex(0x1D87F5), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_connectbtn, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_connectbtn, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_connectbtn, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_radius(ui_connectbtn, 0, LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(ui_connectbtn, lv_color_hex(0x1D87F5), LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_connectbtn, 255, LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_connectbtn, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_connectbtn, 255, LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);

    lv_obj_t *ui_connecttext = lv_label_create(ui_connectbtn);
    lv_obj_set_width(ui_connecttext, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_connecttext, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_connecttext, LV_ALIGN_CENTER);
    lv_label_set_text(ui_connecttext, "connect");

    lv_obj_t *ui_canceltbtn = lv_btn_create(ui_passpannel);
    lv_obj_set_width(ui_canceltbtn, 80);
    lv_obj_set_height(ui_canceltbtn, 40);
    lv_obj_set_x(ui_canceltbtn, -10);
    lv_obj_set_y(ui_canceltbtn, 0);
    lv_obj_set_align(ui_canceltbtn, LV_ALIGN_BOTTOM_RIGHT);
    lv_obj_add_flag(ui_canceltbtn, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_canceltbtn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_canceltbtn, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_canceltbtn, lv_color_hex(0xE6E6E6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_canceltbtn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_canceltbtn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_canceltbtn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_canceltbtn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_canceltbtn, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_canceltbtn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_canceltbtn, 50, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(ui_canceltbtn, lv_color_hex(0x1D87F5), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_canceltbtn, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_canceltbtn, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_canceltbtn, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_radius(ui_canceltbtn, 0, LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(ui_canceltbtn, lv_color_hex(0x1D87F5), LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(ui_canceltbtn, 255, LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui_canceltbtn, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui_canceltbtn, 255, LV_PART_MAIN | LV_STATE_CHECKED | LV_STATE_PRESSED);

    lv_obj_t *ui_canceltext = lv_label_create(ui_canceltbtn);
    lv_obj_set_width(ui_canceltext, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_canceltext, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_canceltext, LV_ALIGN_CENTER);
    lv_label_set_text(ui_canceltext, "cancel");

    lv_obj_t *ui_keypannel = lv_obj_create(ui_enterwifipass);
    lv_obj_set_width(ui_keypannel, 222);
    lv_obj_set_height(ui_keypannel, 160);
    lv_obj_set_align(ui_keypannel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_keypannel, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *ui_Keyboard = lv_keyboard_create(ui_keypannel);
    lv_obj_set_width(ui_Keyboard, 222);
    lv_obj_set_height(ui_Keyboard, 160);
    lv_obj_set_align(ui_Keyboard, LV_ALIGN_CENTER);

    lv_obj_set_style_text_font(ui_Keyboard, &lv_font_montserrat_16, LV_PART_ITEMS | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_headerbackico, ui_event_headerbackico, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_wlanswitch, ui_event_wlanswitch, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_connectbtn, ui_event_connectbtn, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_canceltbtn, ui_event_canceltbtn, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_wifiset, ui_event_wifiset, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_wifipassinput, ui_event_wifipassinput, LV_EVENT_ALL, NULL);
    lv_keyboard_set_textarea(ui_Keyboard, ui_wifipassinput);

}


void ui_event_autodatetimeswitch(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_VALUE_CHANGED && !lv_obj_has_state(target, LV_STATE_CHECKED)) {
#if !WIN
        char buf[43] = { 0 };
        char buf1[20] = { 0 };
        struct tm timeinfo;
        time_t now;
        time(&now);
        localtime_r(&now, &timeinfo);
        sprintf(buf, "Date  %04d-%02d-%02d", timeinfo.tm_year + 1900, (timeinfo.tm_mon < 12 ? timeinfo.tm_mon + 1 : 0), timeinfo.tm_mday);
        lv_label_set_text(ui_date, buf);
        sprintf(buf1, "Time  %02d:%02d", system_24hour ? timeinfo.tm_hour : (timeinfo.tm_hour >= 12 ? timeinfo.tm_hour - 12 : timeinfo.tm_hour), timeinfo.tm_min);
        lv_label_set_text(ui_time1, buf1);
#endif
        _ui_flag_modify(ui_setdate, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        _ui_flag_modify(ui_settime, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
    }
    if (event_code == LV_EVENT_VALUE_CHANGED && lv_obj_has_state(target, LV_STATE_CHECKED)) {
        _ui_flag_modify(ui_setdate, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
        _ui_flag_modify(ui_settime, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    }
}


void ui_event_autodatetimeswitch1(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_VALUE_CHANGED) {
        system_24hour = system_24hour ? 0 : 1;
    }
}

void ui_event_setdate(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
#if !WIN
        struct tm timeinfo;
        time_t now;
        time(&now);
        localtime_r(&now, &timeinfo);
        lv_calendar_set_today_date(ui_calendar, timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
        lv_calendar_set_showed_date(ui_calendar, timeinfo.tm_year + 1900, timeinfo.tm_mon + 1);
#endif
        _ui_flag_modify(ui_calendar, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_TOGGLE);
    }
}

void ui_event_settime(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
#if !WIN
        struct tm timeinfo;
        time_t now;
        time(&now);
        localtime_r(&now, &timeinfo);
        lv_roller_set_options(ui_settime_roller_hour, system_24hour ? opts_24h : opts_12h, LV_ROLLER_MODE_INFINITE);
        lv_roller_set_selected(ui_settime_roller_min, timeinfo.tm_min, LV_ANIM_ON);
        lv_roller_set_selected(ui_settime_roller_hour, timeinfo.tm_hour, LV_ANIM_ON);
#endif
        _ui_flag_modify(ui_settime_roller, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_TOGGLE);
    }
}

void ui_event_calendar(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_current_target(e);
    char buf[24];
    if (event_code == LV_EVENT_VALUE_CHANGED) {
        lv_calendar_date_t date;
        if (lv_calendar_get_pressed_date(target, &date)) {
            lv_snprintf(buf, 23, "Date  %d-%02d-%02d", date.year, date.month, date.day);
#if !WIN
            struct tm timeinfo;
            time_t now;
            time(&now);
            localtime_r(&now, &timeinfo);
            setTime(timeinfo.tm_sec, timeinfo.tm_min, timeinfo.tm_hour, date.day, date.month, date.year);
#endif
        }
        _ui_checked_set_text_value(ui_date, target, (char *)"", buf);
    }
}

void ui_event_datetimeui(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT) {
        _ui_screen_change(ui_setting, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0);
    }
}

void ui_settime_hour_min(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t *target = lv_event_get_target(e);
    if (event_code == LV_EVENT_VALUE_CHANGED) {
        char buf[37] = { 0 }, hour_buf[15] = { 0 }, min_buf[15] = { 0 };
        lv_roller_get_selected_str(ui_settime_roller_hour, hour_buf, sizeof(hour_buf));
        lv_roller_get_selected_str(ui_settime_roller_min, min_buf, sizeof(min_buf));

        lv_snprintf(buf, sizeof(buf), "Time  %s:%s", hour_buf, min_buf);
#if !WIN
        int min = lv_roller_get_selected(ui_settime_roller_min);
        int hour = lv_roller_get_selected(ui_settime_roller_hour);
        struct tm timeinfo;
        time_t now;
        time(&now);
        localtime_r(&now, &timeinfo);
        setTime(timeinfo.tm_sec, min, hour, timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
#endif
        _ui_checked_set_text_value(ui_time1, target, (char *)"", buf);
    }
}

static void ui_datetimeui_screen_init(void)
{
    ui_datetimeui = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_datetimeui, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_datetimeui, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_datetimeui, lv_color_hex(0xE6E6E6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_datetimeui, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_datetimeui, &ui_font_misans16, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_datetimeheader = lv_obj_create(ui_datetimeui);
    lv_obj_set_width(ui_datetimeheader, 222);
    lv_obj_set_height(ui_datetimeheader, 60);
    lv_obj_set_align(ui_datetimeheader, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_datetimeheader, LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_datetimeheader, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_datetimeheader, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_datetimeheader, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_datetimeheader, lv_color_hex(0xB4B4B4), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_datetimeheader, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_datetimeheader, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_datetimeheader, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_datetimeheader, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_datetimeheader, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_datetimeheader, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_datetimeheader, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_datetimeheader, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_datetimeheader, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_datetimeheader, &ui_font_misans16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_label_create(&ui_datetimeui_wifi_icon, ui_datetimeheader, (char *)"", 5, 5, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_add_flag(ui_datetimeui_wifi_icon, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_text_color(ui_datetimeui_wifi_icon, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_datetimeui_wifi_icon, &ui_font_symbols16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_datetimeuitime = NULL;
    ui_label_create(&ui_datetimeuitime, ui_datetimeheader, (char *)"12:12", 0, 7, 60, 30);
    lv_obj_set_style_text_color(ui_datetimeuitime, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_align(ui_datetimeuitime, LV_ALIGN_TOP_MID);

    ui_datetimeui_electric_label = NULL;
    ui_label_create(&ui_datetimeui_electric_label, ui_datetimeheader, (char *)"100%", 155, 5, 40, 30);
    lv_obj_set_style_text_color(ui_datetimeui_electric_label, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    datetimeui_electric_quantity_icon = NULL;
    electric_quantity_icon_create(&datetimeui_electric_quantity_icon, ui_datetimeheader, 193, 7, 22, 12);
    lv_obj_t *obj_child = lv_obj_get_child(datetimeui_electric_quantity_icon, 0);
    lv_obj_set_style_border_color(obj_child, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_color(obj_child, lv_color_hex(0x000000), 0);
    lv_obj_t *obj_child1 = lv_obj_get_child(datetimeui_electric_quantity_icon, 1);
    lv_obj_set_style_border_color(obj_child1, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_color(obj_child1, lv_color_hex(0x000000), 0);
    lv_obj_t *obj_child2 = lv_obj_get_child(datetimeui_electric_quantity_icon, 2);
    lv_obj_set_style_border_color(obj_child2, lv_color_hex(0x000000), 0);

    lv_obj_t *ui_datetimetitle = lv_label_create(ui_datetimeheader);
    lv_obj_set_width(ui_datetimetitle, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_datetimetitle, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_datetimetitle, 0);
    lv_obj_set_y(ui_datetimetitle, -2);
    lv_obj_set_align(ui_datetimetitle, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(ui_datetimetitle, "Date and time");

    lv_obj_t *ui_datetimebody = lv_obj_create(ui_datetimeui);
    lv_obj_set_width(ui_datetimebody, 222);
    lv_obj_set_height(ui_datetimebody, 420);
    lv_obj_set_x(ui_datetimebody, 0);
    lv_obj_set_y(ui_datetimebody, 60);
    lv_obj_set_align(ui_datetimebody, LV_ALIGN_TOP_MID);
    lv_obj_set_flex_flow(ui_datetimebody, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_datetimebody, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_scrollbar_mode(ui_datetimebody, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(ui_datetimebody, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_datetimebody, lv_color_hex(0xE6E6E6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_datetimebody, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_datetimebody, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui_datetimebody, 0, 0);
    lv_obj_set_style_pad_row(ui_datetimebody, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_datetimebody, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_datetimepanel = lv_obj_create(ui_datetimebody);
    lv_obj_set_width(ui_datetimepanel, 222);
    lv_obj_set_height(ui_datetimepanel, 40);
    lv_obj_set_align(ui_datetimepanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_datetimepanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_datetimepanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_datetimepanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_datetimepanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_datetimepanel, lv_color_hex(0xB4B4B4), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_datetimepanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_datetimepanel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_datetimepanel, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_autodatetimelabel1 = lv_label_create(ui_datetimepanel);
    lv_obj_set_width(ui_autodatetimelabel1, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_autodatetimelabel1, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_autodatetimelabel1, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_autodatetimelabel1, "24 Hourly system");
    lv_obj_set_style_text_color(ui_autodatetimelabel1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_autodatetimeswitch1 = lv_switch_create(ui_datetimepanel);
    lv_obj_set_width(ui_autodatetimeswitch1, 45);
    lv_obj_set_height(ui_autodatetimeswitch1, 24);
    lv_obj_set_align(ui_autodatetimeswitch1, LV_ALIGN_RIGHT_MID);
    lv_obj_add_state(ui_autodatetimeswitch1, LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui_autodatetimeswitch1, lv_color_hex(0xAAAAAA), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_autodatetimeswitch1, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_autodatetimeswitch1, lv_color_hex(0x00D200), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui_autodatetimeswitch1, 255, LV_PART_INDICATOR | LV_STATE_CHECKED);

    lv_obj_t *ui_datetimepanel1 = lv_obj_create(ui_datetimebody);
    lv_obj_set_width(ui_datetimepanel1, 222);
    lv_obj_set_height(ui_datetimepanel1, 40);
    lv_obj_set_align(ui_datetimepanel1, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_datetimepanel1, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_datetimepanel1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_datetimepanel1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_datetimepanel1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_datetimepanel1, lv_color_hex(0xB4B4B4), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_datetimepanel1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_datetimepanel1, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_datetimepanel1, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_autodatetimelabel = lv_label_create(ui_datetimepanel1);
    lv_obj_set_width(ui_autodatetimelabel, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_autodatetimelabel, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_autodatetimelabel, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_autodatetimelabel, "Auto set time");
    lv_obj_set_style_text_color(ui_autodatetimelabel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);


    lv_obj_t *ui_autodatetimeswitch = lv_switch_create(ui_datetimepanel1);
    lv_obj_set_width(ui_autodatetimeswitch, 45);
    lv_obj_set_height(ui_autodatetimeswitch, 24);
    lv_obj_set_align(ui_autodatetimeswitch, LV_ALIGN_RIGHT_MID);
    lv_obj_add_state(ui_autodatetimeswitch, LV_STATE_CHECKED);

    lv_obj_set_style_bg_color(ui_autodatetimeswitch, lv_color_hex(0xAAAAAA), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_autodatetimeswitch, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_autodatetimeswitch, lv_color_hex(0x00D200), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui_autodatetimeswitch, 255, LV_PART_INDICATOR | LV_STATE_CHECKED);

    ui_setdate = lv_obj_create(ui_datetimebody);
    lv_obj_set_width(ui_setdate, 222);
    lv_obj_set_height(ui_setdate, 40);
    lv_obj_set_align(ui_setdate, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_setdate, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_setdate, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_setdate, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_setdate, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_setdate, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_setdate, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_setdate, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_date = lv_label_create(ui_setdate);
    lv_obj_set_width(ui_date, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_date, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_date, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_date, "Date  2023-8-8");
    lv_obj_set_style_text_color(ui_date, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_dateright = lv_label_create(ui_setdate);
    lv_obj_set_width(ui_dateright, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_dateright, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_dateright, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_dateright, "");
    lv_obj_set_style_text_color(ui_dateright, lv_color_hex(0x787878), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_dateright, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_dateright, &ui_font_symbols16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_settime = lv_obj_create(ui_datetimebody);
    lv_obj_set_width(ui_settime, 222);
    lv_obj_set_height(ui_settime, 40);
    lv_obj_set_align(ui_settime, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_settime, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_settime, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_settime, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_settime, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_settime, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_settime, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_settime, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_time1 = lv_label_create(ui_settime);
    lv_obj_set_width(ui_time1, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_time1, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_time1, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_time1, "Time  12:12");
    lv_obj_set_style_text_color(ui_time1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_timeright = lv_label_create(ui_settime);
    lv_obj_set_width(ui_timeright, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_timeright, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_timeright, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_timeright, "");
    lv_obj_set_style_text_color(ui_timeright, lv_color_hex(0x787878), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_timeright, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_timeright, &ui_font_symbols16, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_datetimecountry = lv_obj_create(ui_datetimebody);
    lv_obj_set_width(ui_datetimecountry, 222);
    lv_obj_set_height(ui_datetimecountry, 40);
    lv_obj_set_align(ui_datetimecountry, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_datetimecountry, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_datetimecountry, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_datetimecountry, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_datetimecountry, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_datetimecountry, lv_color_hex(0xB4B4B4), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_datetimecountry, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_datetimecountry, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_datetimecountry, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_datetimecountrylabel = lv_label_create(ui_datetimecountry);
    lv_obj_set_width(ui_datetimecountrylabel, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_datetimecountrylabel, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_datetimecountrylabel, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_datetimecountrylabel, "Timezone");
    lv_obj_set_style_text_color(ui_datetimecountrylabel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_datetimelabel1 = lv_label_create(ui_datetimecountry);
    lv_obj_set_width(ui_datetimelabel1, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_datetimelabel1, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_datetimelabel1, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_datetimelabel1, "Beijing");
    lv_obj_set_style_text_color(ui_datetimelabel1, lv_color_hex(0x787878), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_datetimelabel1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_calendar = lv_calendar_create(ui_datetimebody);
    lv_calendar_set_today_date(ui_calendar, 2023, 8, 8);
    lv_calendar_set_showed_date(ui_calendar, 2023, 8);
    lv_obj_t *ui_calendar_header = lv_calendar_header_arrow_create(ui_calendar);
    if (ui_calendar_header != NULL) {

    }
    lv_obj_set_width(ui_calendar, 222);
    lv_obj_set_height(ui_calendar, 220);
    lv_obj_set_align(ui_calendar, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_calendar, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_radius(ui_calendar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_calendar, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    static lv_style_t Time_Setting_style_sel;
    lv_style_init(&Time_Setting_style_sel);
    lv_style_set_text_font(&Time_Setting_style_sel, &lv_font_montserrat_32);
    lv_style_set_text_color(&Time_Setting_style_sel, lv_color_hex(0xd9d9d9));
    lv_style_set_pad_all(&Time_Setting_style_sel, 0);
    lv_style_set_bg_opa(&Time_Setting_style_sel, 0);
    lv_style_set_border_width(&Time_Setting_style_sel, 0);

    lv_obj_t_create(&ui_settime_roller, ui_datetimebody, 0, 0, 222, 220);
    lv_obj_set_style_bg_opa(ui_settime_roller, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(ui_settime_roller, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_bg_color(ui_settime_roller, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_settime_roller_hour = lv_roller_create(ui_settime_roller);
    lv_roller_set_options(ui_settime_roller_hour, opts_12h, LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count(ui_settime_roller_hour, 0);
    lv_obj_set_style_bg_opa(ui_settime_roller_hour, 0, LV_PART_SELECTED);
    lv_obj_set_style_text_color(ui_settime_roller_hour, lv_color_hex(0x000000), LV_PART_SELECTED);
    lv_obj_set_width(ui_settime_roller_hour, 45);
    lv_obj_set_height(ui_settime_roller_hour, 165);
    lv_obj_set_style_text_line_space(ui_settime_roller_hour, 0, 0);
    lv_obj_add_style(ui_settime_roller_hour, &Time_Setting_style_sel, 0);
    lv_obj_align(ui_settime_roller_hour, LV_ALIGN_LEFT_MID, 50, 0);

    lv_obj_t *ui_Label = NULL;
    ui_label_create(&ui_Label, ui_settime_roller, (char *)"hour", 0, 0, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_text_color(ui_Label, lv_color_hex(0x000000), 0);
    lv_obj_align(ui_Label, LV_ALIGN_LEFT_MID, 18, 0);

    ui_settime_roller_min = lv_roller_create(ui_settime_roller);
    lv_roller_set_options(ui_settime_roller_min, opts_min, LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count(ui_settime_roller_min, 0);
    lv_obj_set_style_bg_opa(ui_settime_roller_min, 0, LV_PART_SELECTED);
    lv_obj_set_style_text_color(ui_settime_roller_min, lv_color_hex(0x000000), LV_PART_SELECTED);
    lv_obj_set_width(ui_settime_roller_min, 45);
    lv_obj_set_height(ui_settime_roller_min, 165);
    lv_obj_set_style_text_line_space(ui_settime_roller_min, 0, 0);
    lv_obj_add_style(ui_settime_roller_min, &Time_Setting_style_sel, 0);
    lv_obj_align(ui_settime_roller_min, LV_ALIGN_RIGHT_MID, -50, 0);

    lv_obj_t *ui_Labe2 = NULL;
    ui_label_create(&ui_Labe2, ui_settime_roller, (char *)"min", 0, 0, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_text_color(ui_Labe2, lv_color_hex(0x000000), 0);
    lv_obj_align(ui_Labe2, LV_ALIGN_RIGHT_MID, -22, 0);

    lv_obj_add_event_cb(ui_autodatetimeswitch, ui_event_autodatetimeswitch, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_autodatetimeswitch1, ui_event_autodatetimeswitch1, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_setdate, ui_event_setdate, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_settime, ui_event_settime, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_calendar, ui_event_calendar, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_datetimeui, ui_event_datetimeui, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(ui_settime_roller_hour, ui_settime_hour_min, LV_EVENT_ALL, (void *)1);
    lv_obj_add_event_cb(ui_settime_roller_min, ui_settime_hour_min, LV_EVENT_ALL, (void *)2);
}

void ui_event_aboutui(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT) {
        _ui_screen_change(ui_setting, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0);
    }
}

void aboutui_screen_label_create(lv_obj_t *parent, const char *left_data, const char *right_data)
{
    lv_obj_t *ui_aboutpane = lv_obj_create(parent);
    lv_obj_set_width(ui_aboutpane, 222);
    lv_obj_set_height(ui_aboutpane, 40);
    lv_obj_set_align(ui_aboutpane, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_aboutpane, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_aboutpane, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_aboutpane, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_aboutpane, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_aboutpane, lv_color_hex(0xB4B4B4), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_aboutpane, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_aboutpane, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_aboutpane, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_aboutlabel5 = lv_label_create(ui_aboutpane);
    lv_obj_set_width(ui_aboutlabel5, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_aboutlabel5, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_aboutlabel5, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_aboutlabel5, left_data);
    lv_obj_set_style_text_color(ui_aboutlabel5, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_aboutlabe = lv_label_create(ui_aboutpane);
    lv_obj_set_width(ui_aboutlabe, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_aboutlabe, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_aboutlabe, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_aboutlabe, right_data);
    lv_obj_set_style_text_color(ui_aboutlabe, lv_color_hex(0x787878), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_aboutlabe, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void aboutui_screen_label_create_return(lv_obj_t **ui_label, lv_obj_t *parent, const char *left_data, const char *right_data)
{
    lv_obj_t *ui_aboutpane = lv_obj_create(parent);
    lv_obj_set_width(ui_aboutpane, 222);
    lv_obj_set_height(ui_aboutpane, 40);
    lv_obj_set_align(ui_aboutpane, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_aboutpane, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_aboutpane, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_aboutpane, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_aboutpane, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_aboutpane, lv_color_hex(0xB4B4B4), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_aboutpane, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_aboutpane, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_aboutpane, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_aboutlabel5 = lv_label_create(ui_aboutpane);
    lv_obj_set_width(ui_aboutlabel5, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_aboutlabel5, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_aboutlabel5, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_aboutlabel5, left_data);
    lv_obj_set_style_text_color(ui_aboutlabel5, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);

    *ui_label = lv_label_create(ui_aboutpane);
    lv_obj_set_width(*ui_label, LV_SIZE_CONTENT);
    lv_obj_set_height(*ui_label, LV_SIZE_CONTENT);
    lv_obj_set_align(*ui_label, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(*ui_label, right_data);
    lv_obj_set_style_text_color(*ui_label, lv_color_hex(0x787878), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(*ui_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void ui_aboutui_screen_init(void)
{
    ui_aboutui = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_aboutui, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_aboutui, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_aboutui, lv_color_hex(0xE6E6E6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_aboutui, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_aboutui, &ui_font_misans16, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_aboutheader = lv_obj_create(ui_aboutui);
    lv_obj_set_width(ui_aboutheader, 222);
    lv_obj_set_height(ui_aboutheader, 60);
    lv_obj_set_align(ui_aboutheader, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_aboutheader, LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_aboutheader, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_aboutheader, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_aboutheader, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_aboutheader, lv_color_hex(0xB4B4B4), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_aboutheader, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_aboutheader, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_aboutheader, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_aboutheader, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_aboutheader, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_aboutheader, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_aboutheader, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_aboutheader, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_aboutheader, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_aboutheader, &ui_font_misans16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_label_create(&ui_about_wifi_icon, ui_aboutheader, (char *)"", 5, 5, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_add_flag(ui_about_wifi_icon, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_text_color(ui_about_wifi_icon, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_about_wifi_icon, &ui_font_symbols16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_aboutuitime = NULL;
    ui_label_create(&ui_aboutuitime, ui_aboutheader, (char *)"12:12", 0, 7, 60, 30);
    lv_obj_set_style_text_color(ui_aboutuitime, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_align(ui_aboutuitime, LV_ALIGN_TOP_MID);

    ui_aboutui_electric_label = NULL;
    ui_label_create(&ui_aboutui_electric_label, ui_aboutheader, (char *)"100%", 155, 5, 40, 30);
    lv_obj_set_style_text_color(ui_aboutui_electric_label, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    aboutui_electric_quantity_icon = NULL;
    electric_quantity_icon_create(&aboutui_electric_quantity_icon, ui_aboutheader, 193, 7, 22, 12);
    lv_obj_t *obj_child = lv_obj_get_child(aboutui_electric_quantity_icon, 0);
    lv_obj_set_style_border_color(obj_child, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_color(obj_child, lv_color_hex(0x000000), 0);
    lv_obj_t *obj_child1 = lv_obj_get_child(aboutui_electric_quantity_icon, 1);
    lv_obj_set_style_border_color(obj_child1, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_color(obj_child1, lv_color_hex(0x000000), 0);
    lv_obj_t *obj_child2 = lv_obj_get_child(aboutui_electric_quantity_icon, 2);
    lv_obj_set_style_border_color(obj_child2, lv_color_hex(0x000000), 0);

    lv_obj_t *ui_abouttitle = NULL;
    ui_label_create(&ui_abouttitle, ui_aboutheader, (char *)"About this machine", 0, -2, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_text_color(ui_abouttitle, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_align(ui_abouttitle, LV_ALIGN_BOTTOM_MID);

    lv_obj_t *ui_aboutbody = lv_obj_create(ui_aboutui);

    lv_obj_set_width(ui_aboutbody, 222);
    lv_obj_set_height(ui_aboutbody, 390);
    lv_obj_set_x(ui_aboutbody, 0);
    lv_obj_set_y(ui_aboutbody, 60);
    lv_obj_set_align(ui_aboutbody, LV_ALIGN_TOP_MID);
    lv_obj_set_flex_flow(ui_aboutbody, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_aboutbody, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(ui_aboutbody, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_aboutbody, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_aboutbody, lv_color_hex(0xE6E6E6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_aboutbody, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_aboutbody, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui_aboutbody, 0, 0);
    lv_obj_set_style_pad_row(ui_aboutbody, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_aboutbody, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    aboutui_screen_label_create(ui_aboutbody, (const char *)"Name", (const char *)"LILYGO 1.0.0");

#if WIN
    aboutui_screen_label_create(ui_aboutbody, (const char *)"IDF versions", (const char *)"IDF4.4.1");
    aboutui_screen_label_create(ui_aboutbody, (const char *)"CPU model", (const char *)"ESP32S3");
    aboutui_screen_label_create(ui_aboutbody, (const char *)"CPU core data", (const char *)"Two");
    aboutui_screen_label_create(ui_aboutbody, (const char *)"Memory capacity", (const char *)"8MB");
#else
    char buf[20] = { 0 };
    sprintf(buf, "IDF %s", ESP.getSdkVersion());
    aboutui_screen_label_create(ui_aboutbody, (const char *)"IDF versions", (const char *)buf);
    aboutui_screen_label_create(ui_aboutbody, (const char *)"CPU model", (const char *)ESP.getChipModel());
    aboutui_screen_label_create(ui_aboutbody, (const char *)"CPU core data", (const char *)"Two");
    char buf1[20] = { 0 };
    sprintf(buf1, "%dMB", ESP.getFlashChipSize() / 1024 / 1024);
    aboutui_screen_label_create(ui_aboutbody, (const char *)"Memory capacity", (const char *)buf1);
#endif
    aboutui_screen_label_create(ui_aboutbody, (const char *)"Screen zize", (const char *)"2.33");

    aboutui_screen_label_create_return(&ui_usb_sats, ui_aboutbody, (const char *)"USB", (const char *)"Disconnect");
    aboutui_screen_label_create_return(&ui_usb_voltage, ui_aboutbody, (const char *)"USB Voltage", (const char *)"0mV");
    aboutui_screen_label_create_return(&ui_battery_voltage, ui_aboutbody, (const char *)"Battery Voltage", (const char *)"0mV");

    lv_obj_add_event_cb(ui_aboutui, ui_event_aboutui, LV_EVENT_ALL, NULL);
}

void ui_event_setting(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_RIGHT) {
        _ui_screen_change(ui_home, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0);
    }
}

void ui_event_aboutpannel(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(ui_aboutui, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0);
    }
}

void ui_event_aboutpannel1(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_CLICKED) {
        _ui_screen_change(ui_datetimeui, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0);
    }
}

void ui_setting_screen_init(void)
{
    ui_setting = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_setting, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_setting, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_setting, lv_color_hex(0xE6E6E6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_setting, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_setting, &ui_font_misans16, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_settingheader = lv_obj_create(ui_setting);
    lv_obj_set_width(ui_settingheader, 222);
    lv_obj_set_height(ui_settingheader, 60);
    lv_obj_set_align(ui_settingheader, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_settingheader, LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_settingheader, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_settingheader, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_settingheader, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_settingheader, lv_color_hex(0xB4B4B4), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_settingheader, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_settingheader, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_settingheader, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_settingheader, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_settingheader, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_settingheader, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_settingheader, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_settingheader, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_settingheader, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_settingheader, &ui_font_misans16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_settings_wifi_icon = lv_label_create(ui_settingheader);
    lv_obj_set_width(ui_settings_wifi_icon, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_settings_wifi_icon, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_settings_wifi_icon, 5);
    lv_obj_set_y(ui_settings_wifi_icon, 5);
    lv_label_set_text(ui_settings_wifi_icon, "");
    lv_obj_add_flag(ui_settings_wifi_icon, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_text_font(ui_settings_wifi_icon, &ui_font_symbols16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_settingtime = NULL;
    ui_label_create(&ui_settingtime, ui_settingheader, (char *)"12:12", 0, 7, 60, 30);
    lv_obj_set_style_text_color(ui_settingtime, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_align(ui_settingtime, LV_ALIGN_TOP_MID);

    setting_electric_quantity_lable = NULL;
    ui_label_create(&setting_electric_quantity_lable, ui_settingheader, (char *)"100%", 155, 5, 40, 30);
    lv_obj_set_style_text_color(setting_electric_quantity_lable, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);

    setting_electric_quantity_icon = NULL;
    electric_quantity_icon_create(&setting_electric_quantity_icon, ui_settingheader, 193, 7, 22, 12);
    lv_obj_t *obj_child = lv_obj_get_child(setting_electric_quantity_icon, 0);
    lv_obj_set_style_border_color(obj_child, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_color(obj_child, lv_color_hex(0x000000), 0);
    lv_obj_t *obj_child1 = lv_obj_get_child(setting_electric_quantity_icon, 1);
    lv_obj_set_style_border_color(obj_child1, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_color(obj_child1, lv_color_hex(0x000000), 0);
    lv_obj_t *obj_child2 = lv_obj_get_child(setting_electric_quantity_icon, 2);
    lv_obj_set_style_border_color(obj_child2, lv_color_hex(0x000000), 0);

    lv_obj_t *ui_settingtext = NULL;
    ui_label_create(&ui_settingtext, ui_settingheader, (char *)"setting", 0, 0, 60, 20);
    lv_obj_set_align(ui_settingtext, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_style_text_color(ui_settingtext, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_setpannel = lv_obj_create(ui_setting);
    lv_obj_set_style_pad_all(ui_setpannel, 0, 0);
    lv_obj_set_width(ui_setpannel, 222);
    lv_obj_set_height(ui_setpannel, 420);
    lv_obj_set_x(ui_setpannel, 0);
    lv_obj_set_y(ui_setpannel, 60);
    lv_obj_set_align(ui_setpannel, LV_ALIGN_TOP_MID);
    lv_obj_set_flex_flow(ui_setpannel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_setpannel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(ui_setpannel, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                      LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_setpannel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_setpannel, lv_color_hex(0xE6E6E6), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_setpannel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_setpannel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_setpannel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_setpannel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_setpannel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_setpannel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_setpannel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_aboutpannel = lv_obj_create(ui_setpannel);
    lv_obj_set_width(ui_aboutpannel, 222);
    lv_obj_set_height(ui_aboutpannel, 40);
    lv_obj_set_align(ui_aboutpannel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_aboutpannel, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_SCROLLABLE |
                      LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_radius(ui_aboutpannel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_aboutpannel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_aboutpannel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_aboutpannel, lv_color_hex(0xB4B4B4), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_aboutpannel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_aboutpannel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_aboutpannel, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_aboutlabel = lv_label_create(ui_aboutpannel);
    lv_obj_set_width(ui_aboutlabel, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_aboutlabel, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_aboutlabel, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_aboutlabel, "About this machine");
    lv_obj_set_style_text_color(ui_aboutlabel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(ui_aboutlabel, LV_OBJ_FLAG_PRESS_LOCK);

    lv_obj_t *ui_aboutlabel2 = lv_label_create(ui_aboutpannel);
    lv_obj_set_width(ui_aboutlabel2, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_aboutlabel2, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_aboutlabel2, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_aboutlabel2, "");
    lv_obj_clear_flag(ui_aboutlabel2, LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_set_style_text_color(ui_aboutlabel2, lv_color_hex(0x787878), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_aboutlabel2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_aboutlabel2, &ui_font_symbols16, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_aboutpannel1 = lv_obj_create(ui_setpannel);
    lv_obj_set_width(ui_aboutpannel1, 222);
    lv_obj_set_height(ui_aboutpannel1, 40);
    lv_obj_set_x(ui_aboutpannel1, 166);
    lv_obj_set_y(ui_aboutpannel1, 2);
    lv_obj_set_align(ui_aboutpannel1, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_aboutpannel1, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_SCROLLABLE |
                      LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_radius(ui_aboutpannel1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_aboutpannel1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_aboutpannel1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_aboutpannel1, lv_color_hex(0xB4B4B4), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_aboutpannel1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_aboutpannel1, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_aboutpannel1, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_aboutlabel19 = lv_label_create(ui_aboutpannel1);
    lv_obj_set_width(ui_aboutlabel19, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_aboutlabel19, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_aboutlabel19, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_aboutlabel19, "Date and time");
    lv_obj_set_style_text_color(ui_aboutlabel19, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(ui_aboutlabel19, LV_OBJ_FLAG_PRESS_LOCK);

    lv_obj_t *ui_aboutlabel21 = lv_label_create(ui_aboutpannel1);
    lv_obj_set_width(ui_aboutlabel21, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_aboutlabel21, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_aboutlabel21, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_aboutlabel21, "");
    lv_obj_clear_flag(ui_aboutlabel21, LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_set_style_text_color(ui_aboutlabel21, lv_color_hex(0x787878), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_aboutlabel21, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_aboutlabel21, &ui_font_symbols16, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_wifipanel = lv_obj_create(ui_setpannel);
    lv_obj_set_width(ui_wifipanel, 222);
    lv_obj_set_height(ui_wifipanel, 40);
    lv_obj_set_align(ui_wifipanel, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_wifipanel, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_wifipanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_wifipanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_wifipanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_wifipanel, lv_color_hex(0xB4B4B4), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_wifipanel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_wifipanel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_wifipanel, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_wifitext = lv_label_create(ui_wifipanel);
    lv_obj_set_width(ui_wifitext, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_wifitext, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_wifitext, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_wifitext, "WIFI");
    lv_obj_set_style_text_color(ui_wifitext, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(ui_wifitext, LV_OBJ_FLAG_PRESS_LOCK);

    ui_wifionlabel = lv_label_create(ui_wifipanel);
    lv_obj_set_width(ui_wifionlabel, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_wifionlabel, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_wifionlabel, -20);
    lv_obj_set_y(ui_wifionlabel, 0);
    lv_obj_set_align(ui_wifionlabel, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_wifionlabel, "Closed");
    lv_obj_clear_flag(ui_wifionlabel, LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_set_style_text_color(ui_wifionlabel, lv_color_hex(0x787878), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_wifionlabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_wifionlabel1 = lv_label_create(ui_wifipanel);
    lv_obj_set_width(ui_wifionlabel1, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_wifionlabel1, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_wifionlabel1, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_wifionlabel1, "");
    lv_obj_clear_flag(ui_wifionlabel1, LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_set_style_text_color(ui_wifionlabel1, lv_color_hex(0x787878), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_wifionlabel1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_wifionlabel1, &ui_font_symbols16, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_wifipanel1 = lv_obj_create(ui_setpannel);
    lv_obj_set_width(ui_wifipanel1, 222);
    lv_obj_set_height(ui_wifipanel1, 40);
    lv_obj_set_align(ui_wifipanel1, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_wifipanel1, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_wifipanel1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_wifipanel1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_wifipanel1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_wifipanel1, lv_color_hex(0xB4B4B4), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_wifipanel1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_wifipanel1, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_wifipanel1, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_wifitext1 = lv_label_create(ui_wifipanel1);
    lv_obj_set_width(ui_wifitext1, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_wifitext1, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_wifitext1, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_wifitext1, "Bluetooth");
    lv_obj_set_style_text_color(ui_wifitext1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(ui_wifitext1, LV_OBJ_FLAG_PRESS_LOCK);

    lv_obj_t *ui_wifionlabel2 = lv_label_create(ui_wifipanel1);
    lv_obj_set_width(ui_wifionlabel2, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_wifionlabel2, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_wifionlabel2, -20);
    lv_obj_set_y(ui_wifionlabel2, 0);
    lv_obj_set_align(ui_wifionlabel2, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_wifionlabel2, "Closed");
    lv_obj_clear_flag(ui_wifionlabel2, LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_set_style_text_color(ui_wifionlabel2, lv_color_hex(0x787878), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_wifionlabel2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_wifionlabel3 = lv_label_create(ui_wifipanel1);
    lv_obj_set_width(ui_wifionlabel3, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_wifionlabel3, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_wifionlabel3, LV_ALIGN_RIGHT_MID);
    lv_label_set_text(ui_wifionlabel3, "");
    lv_obj_clear_flag(ui_wifionlabel3, LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_set_style_text_color(ui_wifionlabel3, lv_color_hex(0x787878), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_wifionlabel3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_wifionlabel3, &ui_font_symbols16, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_aboutpannel, ui_event_aboutpannel, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_aboutpannel1, ui_event_aboutpannel1, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_wifipanel, ui_event_wifipanel, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_setting, ui_event_setting, LV_EVENT_ALL, NULL);
}

void ui_camera_screen_init(void)
{
    ui_camera = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_camera, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_camera, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_camera, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_camera, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * btn = lv_btn_create(ui_camera);
    lv_obj_set_size(btn, 140, 80);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_event_cb(btn, open_camera_cb, LV_EVENT_CLICKED, (void *)1);
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, "camera");
    lv_obj_center(label);

    lv_obj_t *btn1 = lv_btn_create(ui_camera);
    lv_obj_set_size(btn1, 60, 60);
    lv_obj_add_flag(btn1, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_align(btn1, LV_ALIGN_LEFT_MID, 20, 90);
    lv_obj_add_event_cb(btn1, open_camera_cb, LV_EVENT_CLICKED, (void *)2);
    label = lv_label_create(btn1);
    lv_label_set_text(label, "FL");
    lv_obj_center(label);

    lv_obj_t *btn2 = lv_btn_create(ui_camera);
    lv_obj_set_size(btn2, 60, 60);
    lv_obj_align_to(btn2, btn1, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    lv_obj_add_event_cb(btn2, open_camera_cb, LV_EVENT_CLICKED, (void *)3);
    label = lv_label_create(btn2);
    lv_label_set_text(label, "AF");
    lv_obj_center(label);

    lv_obj_t *btn3 = lv_btn_create(ui_camera);
    lv_obj_set_size(btn3, 60, 60);
    lv_obj_align_to(btn3, btn2, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    lv_obj_add_event_cb(btn3, open_camera_cb, LV_EVENT_CLICKED, (void *)4);
    label = lv_label_create(btn3);
    lv_label_set_text(label, "SAVE");
    lv_obj_center(label);
#if UI_CAMERA_CANVAS  
    ui_camera_canvas = lv_canvas_create(ui_camera);
    lv_obj_align(ui_camera_canvas, LV_ALIGN_CENTER, 0, -80);
#else
    ui_camera_img = lv_img_create(ui_camera);
    lv_obj_align(ui_camera_img, LV_ALIGN_CENTER, 0, -80);
#endif
    camera_timer = lv_timer_create(camera_video_play, 50, NULL);
    lv_timer_ready(camera_timer);
    lv_timer_pause(camera_timer);
}

void ui_photos_screen_init(void)
{
    ui_photos = lv_obj_create(NULL);
    lv_obj_set_size(ui_photos, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(ui_photos, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_pad_all(ui_photos, 0, 0);
    lv_obj_set_style_radius(ui_photos, 0, 0);
    lv_obj_clear_flag(ui_photos, LV_OBJ_FLAG_SCROLLABLE);

    ui_photos_img_src = lv_obj_create(NULL);
    lv_obj_set_size(ui_photos_img_src, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(ui_photos_img_src, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_pad_all(ui_photos_img_src, 0, 0);
    lv_obj_set_style_radius(ui_photos_img_src, 0, 0);
    lv_obj_clear_flag(ui_photos_img_src, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(ui_photos_img_src, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(ui_photos_img_src, ui_photos_img_src_event, LV_EVENT_CLICKED, NULL);

    ui_photos_img = lv_img_create(ui_photos_img_src);
    lv_obj_center(ui_photos_img);

    ui_photos_gif = lv_gif_create(ui_photos_img_src);
    lv_obj_center(ui_photos_gif);
}


void ui_home_create(lv_obj_t **ui_home, lv_obj_t *parent)
{
    *ui_home = lv_obj_create(parent);

    lv_obj_clear_flag(*ui_home, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_GESTURE_BUBBLE |
                      LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(*ui_home, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(*ui_home, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(*ui_home, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(*ui_home, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(*ui_home, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(*ui_home, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(*ui_home, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(*ui_home, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(*ui_home, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(*ui_home, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
}

#if !WIN
void set_ui_Slider2_value(int32_t value)
{
    if (ui_Slider2 != NULL) {
        lv_slider_set_value(ui_Slider2, value >= BRIGHTNESS_MAX_LEVEL ? BRIGHTNESS_MAX_LEVEL : value, LV_ANIM_OFF);
    }
}
#endif

#if !WIN
extern void setBrightness(uint8_t value);
#endif
void ui_event_lightslider(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_VALUE_CHANGED) {
        //printf("%d\n", lv_slider_get_value(ui_Slider2));
#if !WIN
        setBrightness(lv_slider_get_value(ui_Slider2));
#endif
    }
}

void wifi_button_cd(lv_event_t *e)
{
    _ui_screen_change(ui_wifiset, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0);
}

void return_home_cd(void *)
{
    if(camera_timer)
        lv_timer_pause(camera_timer);
    _ui_screen_change(ui_home, LV_SCR_LOAD_ANIM_NONE, 200, 0);
}

#if !WIN
extern void lv_auto_brightne_cd(void);
#endif
static void auto_brightne_button_cd(lv_event_t *e)
{
#if !WIN
    lv_auto_brightne_cd();
#endif
}

void camera_open_btn_cb(lv_event_t *e)
{
    _ui_screen_change(ui_camera, LV_SCR_LOAD_ANIM_NONE, 0, 0);
}

void flashlight_btn_cb(lv_event_t *e)
{
    int n = (int)lv_event_get_user_data(e);
    static bool light_flag = false;

    if(!light_flag)
        ledcWrite(LEDC_WHITE_CH, 20);
    else
        ledcWrite(LEDC_WHITE_CH, 0);

    light_flag = !light_flag;

    Serial.println(n);
    // Serial.println("flashlight");
}

static void slider_event_cb(lv_event_t * e)
{
    lv_obj_t *label = (lv_obj_t *)lv_event_get_user_data(e);
    lv_obj_t * slider = lv_event_get_target(e);
    char buf[8];

    if(label != NULL){
        int val = (int)lv_slider_get_value(slider);
        lv_snprintf(buf, sizeof(buf), "%d%%", val);
        lv_label_set_text(label, buf);
        lv_obj_align_to(label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
        analogWrite(VIBRATING_MOTOR, lv_map(val, 0, 100, 0, 255));
        if(val < 10)
            digitalWrite(VIBRATING_MOTOR, LOW);
    }
}


static const lv_img_dsc_t *imgs[4] = {
    &twocirclingarrows1,
    &twocirclingarrows2,
};

static void demo_init(lv_obj_t *parent, lv_img_dsc_t **dsc, uint8_t num, uint32_t duration, lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h)
{
    lv_obj_t *animimg0 = lv_animimg_create(parent);
    lv_obj_center(animimg0);

#if WIN
    lv_animimg_set_src(animimg0, dsc, num);
#else
    lv_animimg_set_src(animimg0, (const void **)dsc, num);
#endif
    lv_animimg_set_duration(animimg0, duration);
    lv_animimg_set_repeat_count(animimg0, 0x00);

    lv_obj_set_style_pad_all(animimg0, 0, 0);

    lv_obj_set_size(animimg0, w, h);
    lv_animimg_start(animimg0);
}

void animation_sta(void)
{
    lv_obj_t *animimg0 = lv_obj_get_child(ui_wifirefresh, 0);
    lv_animimg_set_repeat_count(animimg0, 0xffff);
    lv_animimg_start(animimg0);
}

void animation_stop(void)
{
    lv_obj_t *animimg0 = lv_obj_get_child(ui_wifirefresh, 0);
    lv_animimg_set_repeat_count(animimg0, 0x0);
    lv_animimg_start(animimg0);
}

void demo_animation(lv_obj_t *parent, lv_obj_t **obj, lv_coord_t x, lv_coord_t y)
{
    *obj = lv_btn_create(parent);
    lv_obj_set_style_pad_all(*obj, 0, 0);
    lv_obj_set_pos(*obj, x, y);
    lv_obj_set_size(*obj, 16, 16);
    lv_obj_set_style_bg_opa(*obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(*obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(*obj, LV_OPA_0, 0);
    demo_init(*obj, (lv_img_dsc_t **)imgs, 2, 400, 0, 0, 16, 16);
}

void ui_init(void)
{
    ui_home = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_home, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(ui_home, 0, 0);
    lv_obj_set_style_bg_img_src(ui_home, &theme_bg_img0, LV_PART_MAIN);

    ui_header = lv_obj_create(ui_home);
    lv_obj_set_style_pad_all(ui_header, 0, 0);
    lv_obj_set_width(ui_header, 222);
    lv_obj_set_height(ui_header, 30);
    lv_obj_set_align(ui_header, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_header, LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_header, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_header, lv_color_hex(0x141414), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_header, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_header, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_blend_mode(ui_header, LV_BLEND_MODE_NORMAL, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_header, 20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_body = lv_obj_create(ui_home);
    lv_obj_set_style_pad_all(ui_body, 0, 0);
    lv_obj_set_width(ui_body, 222);
    lv_obj_set_height(ui_body, 360);
    lv_obj_set_x(ui_body, 0);
    lv_obj_set_y(ui_body, -30);
    lv_obj_set_align(ui_body, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_body, LV_OBJ_FLAG_SCROLL_ONE);
    lv_obj_clear_flag(ui_body, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_scrollbar_mode(ui_body, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_snap_x(ui_body, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scroll_dir(ui_body, LV_DIR_HOR);
    lv_obj_set_style_radius(ui_body, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_body, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_body, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_body, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_body, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_body, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_body, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_body, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_body, 0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);

    ui_home_create(&ui_home1, ui_body);
    lv_obj_set_width(ui_home1, 222);
    lv_obj_set_height(ui_home1, 360);
    lv_obj_set_align(ui_home1, LV_ALIGN_TOP_LEFT);
    backgBut_explain(ui_home1, NULL, (char *)"facetime", NULL, (void *)1, &app_facetime_img, -2, 0);
    backgBut_explain(ui_home1, NULL, (char *)"mail", NULL, (void *)1, &app_mail_img, 64, 0);
    backgBut_explain(ui_home1, NULL, (char *)"music", NULL, (void *)1, &app_music_img, 130, 0);
    backgBut_explain(ui_home1, NULL, (char *)"notes", NULL, (void *)1, &app_notes_img, -2, 90);
    backgBut_explain(ui_home1, NULL, (char *)"photos", ui_event_photos, (void *)1, &app_photos_img, 64, 90);
    backgBut_explain(ui_home1, NULL, (char *)"settings", ui_event_settings, (void *)1, &app_settings_img, 130, 90);
    backgBut_explain(ui_home1, NULL, (char *)"books", NULL, (void *)1, &app_books_img, -2, 180);
    //backgBut_explain(ui_home1, NULL, (char*)"calculator", NULL, (void*)1, &app_calculator_img, 64, 180);
    backgBut_explain(ui_home1, NULL, (char *)"counter", NULL, (void *)1, &app_calculator_img, 64, 180);
    backgBut_explain(ui_home1, NULL, (char *)"contacts", NULL, (void *)1, &app_contacts_img, 130, 180);
    //backgBut_explain(ui_home1, NULL, (char*)"camera", NULL, (void*)1, &app_camera_img, 130, 180);

    ui_home_create(&ui_home2, ui_body);
    lv_obj_set_width(ui_home2, 222);
    lv_obj_set_height(ui_home2, 360);
    lv_obj_set_x(ui_home2, 222);
    lv_obj_set_y(ui_home2, 0);
    lv_obj_set_align(ui_home2, LV_ALIGN_CENTER);
    backgBut_explain(ui_home2, NULL, (char *)"store", NULL, (void *)1, &app_store_img, -2, 0);
    //backgBut_explain(ui_home2, NULL, (char*)"apple tv", NULL, (void*)1, &app_tv_img, 64, 0);
    backgBut_explain(ui_home2, NULL, (char *)"find_my", NULL, (void *)1, &app_find_my_img, 64, 0);
    backgBut_explain(ui_home2, NULL, (char *)"clock", NULL, (void *)1, &app_clock_img, 130, 0);
    backgBut_explain(ui_home2, NULL, (char *)"podcasts", NULL, (void *)1, &app_podcasts_img, -2, 90);
    backgBut_explain(ui_home2, NULL, (char *)"safari", NULL, (void *)1, &app_safari_img, 64, 90);
    backgBut_explain(ui_home2, NULL, (char *)"shortcuts", NULL, (void *)1, &app_shortcuts_img, 130, 90);
    backgBut_explain(ui_home2, NULL, (char *)"stocks", NULL, (void *)1, &app_stocks_img, -2, 180);
    //backgBut_explain(ui_home2, NULL, (char*)"voice memos", NULL, (void*)1, &app_voice_memos_img, 64, 180);
    backgBut_explain(ui_home2, NULL, (char *)"files", NULL, (void *)1, &app_files_img, 64, 180);

    ui_footer = lv_obj_create(ui_home);
    lv_obj_set_width(ui_footer, 222);
    lv_obj_set_height(ui_footer, 90);
    lv_obj_set_align(ui_footer, LV_ALIGN_BOTTOM_MID);
    lv_obj_clear_flag(ui_footer, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM |
                      LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_radius(ui_footer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_footer, lv_color_hex(0x141414), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_footer, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_footer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_blend_mode(ui_footer, LV_BLEND_MODE_NORMAL, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(ui_footer, 20, LV_PART_MAIN | LV_STATE_DEFAULT);

    //backgBut_explain(ui_home, NULL, (char*)"Phone", NULL, (void *)1, &app_Phone_img, 18, 400);
    //backgBut_explain(ui_home, NULL, (char*)"message", NULL, (void *)1, &app_message_img, 86, 400);
    backgBut_explain(ui_home, NULL, NULL, NULL, (void *)1, &app_Phone_img, 18, 410);
    backgBut_explain(ui_home, NULL, NULL, NULL, (void *)1, &app_message_img, 86, 410);
    backgBut_explain(ui_home, NULL, NULL, ui_event_camera, (void *)1, &app_camera_img, 156, 410);

    //backgBut_explain(ui_home, NULL, (char*)"", NULL, (void *)1, NULL, 200, 7);
    //backgBut_explain(ui_home, NULL, (char*)"12:12", NULL, (void *)1, NULL, 100, 7);
    ui_label_create(&ui_time, ui_home, (char *)"12:12", 0, 7, 60, 30);
    lv_obj_set_align(ui_time, LV_ALIGN_TOP_MID);
    //ui_label_create(&ui_home_electric_quantity_icon, ui_home, (char*)"", 195, 5, 24, 12);
    //lv_obj_set_style_text_font(ui_home_electric_quantity_icon, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
    electric_quantity_icon_create(&ui_home_electric_quantity_icon, ui_home, 193, 7, 22, 12);
    ui_label_create(&ui_home_electric_quantity_lable, ui_home, (char *)"100%", 158, 7, 40, 30);

    ui_label_create(&ui_home_wifi_icon, ui_home, (char *)"", 5, 5, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_add_flag(ui_home_wifi_icon, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_text_font(ui_home_wifi_icon, &ui_font_symbols16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_controlListener = lv_obj_create(ui_home);
    lv_obj_set_style_pad_all(ui_controlListener, 0, 0);
    lv_obj_set_width(ui_controlListener, 222);
    lv_obj_set_height(ui_controlListener, 50);
    lv_obj_set_align(ui_controlListener, LV_ALIGN_TOP_RIGHT);
    lv_obj_clear_flag(ui_controlListener, LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_controlListener, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_controlListener, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_controlListener, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_controlListener, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_controlCenter = lv_obj_create(ui_home);
    lv_obj_set_style_pad_all(ui_controlCenter, 0, 0);
    lv_obj_set_width(ui_controlCenter, 222);
    lv_obj_set_height(ui_controlCenter, 480);
    lv_obj_set_x(ui_controlCenter, 0);
    lv_obj_set_y(ui_controlCenter, -480);
    lv_obj_set_align(ui_controlCenter, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_controlCenter, LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE |
                      LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_controlCenter, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_controlCenter, lv_color_hex(0x323232), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_controlCenter, 245, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_Panel10 = NULL;
    lv_obj_t_create(&ui_Panel10, ui_controlCenter, 56, 30, 110, 110);
    lv_obj_set_align(ui_Panel10, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_Panel10, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_Panel10, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Panel10, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Panel10, 150, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_Label4 = NULL;
    ui_label_create(&ui_Label4, ui_Panel10, (char *)"Anti-Hero", 0, 20, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_Label4, LV_ALIGN_TOP_MID);

    lv_obj_t *ui_Label3 = NULL;
    ui_label_create(&ui_Label3, ui_Panel10, (char *)"Taylor Swift", 0, 40, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_Label3, LV_ALIGN_TOP_MID);
    lv_obj_set_style_text_color(ui_Label3, lv_color_hex(0xBCBCBC), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_Label3, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_Image2 = NULL;
    ui_img_create(&ui_Image2, ui_Panel10, &ui_img_previous_png, -35, -10, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_Image2, LV_ALIGN_BOTTOM_MID);
    lv_img_set_zoom(ui_Image2, 150);

    lv_obj_t *ui_Image1 = NULL;
    ui_img_create(&ui_Image1, ui_Panel10, &ui_img_play_png, 0, -10, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_Image1, LV_ALIGN_BOTTOM_MID);
    lv_img_set_zoom(ui_Image1, 150);

    lv_obj_t *ui_Image3 = NULL;
    ui_img_create(&ui_Image3, ui_Panel10, &ui_img_next_png, 35, -10, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_Image3, LV_ALIGN_BOTTOM_MID);
    lv_img_set_zoom(ui_Image3, 180);

    lv_obj_t *ui_Panel1 = NULL;
    lv_obj_t_create(&ui_Panel1, ui_controlCenter, -56, 30, 110, 110);
    lv_obj_set_align(ui_Panel1, LV_ALIGN_TOP_MID);
    lv_obj_set_style_radius(ui_Panel1, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Panel1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Panel1, 150, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_Image6 = NULL;
    ui_img_bgColour_create(&ui_Image6, ui_Panel1, &ui_img_1139252462, -26, -26, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_Image6, LV_ALIGN_CENTER);

    lv_obj_t *ui_Image4 = NULL;
    ui_img_bgColour_create(&ui_Image4, ui_Panel1, &ui_img_1596273026, 26, -26, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_Image4, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_color(ui_Image4, lv_color_hex(0x1ED439), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_color(ui_Image4, lv_color_hex(0x1ED439), LV_PART_MAIN | LV_STATE_CHECKED);

    ui_Image5 = lv_img_create(ui_Panel1);
    lv_img_set_src(ui_Image5, &ui_img_1837376705);
    lv_obj_set_width(ui_Image5, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Image5, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_Image5, -26);
    lv_obj_set_y(ui_Image5, 26);
    lv_obj_set_align(ui_Image5, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image5, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_Image5, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_Image5, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Image5, lv_color_hex(0x323232), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Image5, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Image5, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_color(ui_Image5, lv_color_hex(0x323232), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_opa(ui_Image5, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui_Image5, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_pad(ui_Image5, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(ui_Image5, wifi_button_cd, LV_EVENT_CLICKED, (void *)2);

    lv_obj_t *ui_Image7 = NULL;
    ui_img_bgColour_create(&ui_Image7, ui_Panel1, &ui_img_878056124, 26, 26, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_Image7, LV_ALIGN_CENTER);
    lv_obj_set_style_bg_color(ui_Image7, lv_color_hex(0x3D0EC9), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_color(ui_Image7, lv_color_hex(0x3D0EC9), LV_PART_MAIN | LV_STATE_CHECKED);

    ui_Slider2 = lv_slider_create(ui_controlCenter);
    lv_slider_set_range(ui_Slider2, 1, BRIGHTNESS_MAX_LEVEL);
    lv_slider_set_value(ui_Slider2, 255, LV_ANIM_OFF);
    if (lv_slider_get_mode(ui_Slider2) == LV_SLIDER_MODE_RANGE) lv_slider_set_left_value(ui_Slider2, 0, LV_ANIM_OFF);
    lv_obj_set_width(ui_Slider2, 50);
    lv_obj_set_height(ui_Slider2, 120);
    lv_obj_set_x(ui_Slider2, 27);
    lv_obj_set_y(ui_Slider2, 150);
    lv_obj_set_align(ui_Slider2, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_Slider2, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_radius(ui_Slider2, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Slider2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Slider2, 150, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_radius(ui_Slider2, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Slider2, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Slider2, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_Slider2, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Slider2, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    lv_obj_t *ui_Image11 = lv_img_create(ui_Slider2);
    lv_img_set_src(ui_Image11, &ui_img_1008068399);
    lv_obj_set_width(ui_Image11, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Image11, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_Image11, 0);
    lv_obj_set_y(ui_Image11, -10);
    lv_obj_set_align(ui_Image11, LV_ALIGN_BOTTOM_MID);
    lv_obj_add_flag(ui_Image11, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_Image11, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_img_recolor(ui_Image11, lv_color_hex(0x313031), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_Image11, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_Slider1 = lv_slider_create(ui_controlCenter);
    lv_slider_set_value(ui_Slider1, 50, LV_ANIM_OFF);
    if (lv_slider_get_mode(ui_Slider1) == LV_SLIDER_MODE_RANGE) lv_slider_set_left_value(ui_Slider1, 0, LV_ANIM_OFF);
    lv_obj_set_width(ui_Slider1, 50);
    lv_obj_set_height(ui_Slider1, 120);
    lv_obj_set_x(ui_Slider1, 85);
    lv_obj_set_y(ui_Slider1, 150);
    lv_obj_set_align(ui_Slider1, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_Slider1, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_radius(ui_Slider1, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Slider1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Slider1, 150, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_radius(ui_Slider1, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Slider1, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Slider1, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_Slider1, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Slider1, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    lv_obj_t *ui_Image15 = lv_img_create(ui_Slider1);
    lv_img_set_src(ui_Image15, &ui_img_2139845718);
    lv_obj_set_width(ui_Image15, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Image15, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_Image15, 0);
    lv_obj_set_y(ui_Image15, -10);
    lv_obj_set_align(ui_Image15, LV_ALIGN_BOTTOM_MID);
    lv_obj_add_flag(ui_Image15, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_Image15, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_img_recolor(ui_Image15, lv_color_hex(0x313031), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_Image15, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_Image9 = lv_img_create(ui_controlCenter);
    lv_img_set_src(ui_Image9, &ui_img_352497594);
    lv_obj_set_width(ui_Image9, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Image9, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_Image9, -85);
    lv_obj_set_y(ui_Image9, 160);
    lv_obj_set_align(ui_Image9, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_Image9, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_Image9, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_Image9, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Image9, lv_color_hex(0x101010), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Image9, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Image9, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_color(ui_Image9, lv_color_hex(0x101010), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_opa(ui_Image9, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui_Image9, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_pad(ui_Image9, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_Image9, 5, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui_Image9, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui_Image9, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_color(ui_Image9, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_opa(ui_Image9, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_width(ui_Image9, 10, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_pad(ui_Image9, 0, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_img_recolor(ui_Image9, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_img_recolor_opa(ui_Image9, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_add_event_cb(ui_Image9, flashlight_btn_cb, LV_EVENT_CLICKED, (void *)9);

    lv_obj_t *ui_Image10 = lv_img_create(ui_controlCenter);
    lv_img_set_src(ui_Image10, &app_auto_brightne_img);
    lv_obj_set_width(ui_Image10, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Image10, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_Image10, -30);
    lv_obj_set_y(ui_Image10, 160);
    lv_obj_set_align(ui_Image10, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_Image10, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_Image10, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_Image10, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Image10, lv_color_hex(0x101010), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Image10, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Image10, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_color(ui_Image10, lv_color_hex(0x101010), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_opa(ui_Image10, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui_Image10, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_pad(ui_Image10, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_Image10, 5, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui_Image10, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui_Image10, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_color(ui_Image10, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_opa(ui_Image10, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_width(ui_Image10, 10, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_pad(ui_Image10, 0, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_img_recolor(ui_Image10, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_img_recolor_opa(ui_Image10, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_add_event_cb(ui_Image10, auto_brightne_button_cd, LV_EVENT_CLICKED, (void *)2);

    lv_obj_t *ui_Panel3 = lv_obj_create(ui_controlCenter);
    lv_obj_set_width(ui_Panel3, 105);
    lv_obj_set_height(ui_Panel3, 50);
    lv_obj_set_x(ui_Panel3, -57);
    lv_obj_set_y(ui_Panel3, 220);
    lv_obj_set_align(ui_Panel3, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_Panel3, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_Panel3, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Panel3, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Panel3, 150, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Panel3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *ui_Image8 = lv_img_create(ui_Panel3);
    lv_img_set_src(ui_Image8, &ui_img_1103371374);
    lv_obj_set_width(ui_Image8, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Image8, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_Image8, -27);
    lv_obj_set_y(ui_Image8, 0);
    lv_obj_set_align(ui_Image8, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image8, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_Image8, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_Image8, 16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Image8, lv_color_hex(0x323232), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Image8, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Image8, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_color(ui_Image8, lv_color_hex(0x323232), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_opa(ui_Image8, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui_Image8, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_pad(ui_Image8, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_Image8, 16, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui_Image8, lv_color_hex(0x280EA8), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui_Image8, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_color(ui_Image8, lv_color_hex(0x280EA8), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_opa(ui_Image8, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_width(ui_Image8, 5, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_pad(ui_Image8, 0, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_add_event_cb(ui_Image8, flashlight_btn_cb, LV_EVENT_CLICKED, (void *)8);

    lv_obj_t *ui_Label5 = lv_label_create(ui_Panel3);
    lv_obj_set_width(ui_Label5, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Label5, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_Label5, 45);
    lv_obj_set_y(ui_Label5, 0);
    lv_obj_set_align(ui_Label5, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_Label5, "Focus");

    lv_obj_t *ui_Image16 = lv_img_create(ui_controlCenter);
    lv_img_set_src(ui_Image16, &ui_img_1983457674);
    lv_obj_set_width(ui_Image16, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Image16, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_Image16, -83);
    lv_obj_set_y(ui_Image16, 290);
    lv_obj_set_align(ui_Image16, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_Image16, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_Image16, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_Image16, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Image16, lv_color_hex(0x101010), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Image16, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Image16, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_color(ui_Image16, lv_color_hex(0x101010), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_opa(ui_Image16, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui_Image16, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_pad(ui_Image16, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_Image16, 5, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui_Image16, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui_Image16, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_color(ui_Image16, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_opa(ui_Image16, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_width(ui_Image16, 10, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_pad(ui_Image16, 0, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_img_recolor(ui_Image16, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_img_recolor_opa(ui_Image16, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_add_event_cb(ui_Image16, flashlight_btn_cb, LV_EVENT_CLICKED, (void *)16);

    lv_obj_t *ui_Image17 = lv_img_create(ui_controlCenter);
    lv_img_set_src(ui_Image17, &ui_img_204030930);
    lv_obj_set_width(ui_Image17, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Image17, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_Image17, -28);
    lv_obj_set_y(ui_Image17, 290);
    lv_obj_set_align(ui_Image17, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_Image17, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_Image17, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_Image17, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Image17, lv_color_hex(0x101010), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Image17, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Image17, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_color(ui_Image17, lv_color_hex(0x101010), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_opa(ui_Image17, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui_Image17, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_pad(ui_Image17, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_Image17, 5, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui_Image17, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui_Image17, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_color(ui_Image17, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_opa(ui_Image17, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_width(ui_Image17, 10, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_pad(ui_Image17, 0, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_img_recolor(ui_Image17, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_img_recolor_opa(ui_Image17, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_add_event_cb(ui_Image17, flashlight_btn_cb, LV_EVENT_CLICKED, (void *)17);

    lv_obj_t *ui_Image18 = lv_img_create(ui_controlCenter);
    lv_img_set_src(ui_Image18, &ui_img_350297007);
    lv_obj_set_width(ui_Image18, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Image18, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_Image18, 28);
    lv_obj_set_y(ui_Image18, 290);
    lv_obj_set_align(ui_Image18, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_Image18, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_Image18, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_Image18, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Image18, lv_color_hex(0x101010), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Image18, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Image18, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_color(ui_Image18, lv_color_hex(0x101010), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_opa(ui_Image18, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui_Image18, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_pad(ui_Image18, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_Image18, 5, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui_Image18, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui_Image18, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_color(ui_Image18, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_opa(ui_Image18, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_width(ui_Image18, 10, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_pad(ui_Image18, 0, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_img_recolor(ui_Image18, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_img_recolor_opa(ui_Image18, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_add_event_cb(ui_Image18, flashlight_btn_cb, LV_EVENT_CLICKED, (void *)18);

    lv_obj_t *ui_Image19 = lv_img_create(ui_controlCenter);
    lv_img_set_src(ui_Image19, &ui_img_1918312013);
    lv_obj_set_width(ui_Image19, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Image19, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_Image19, 83);
    lv_obj_set_y(ui_Image19, 290);
    lv_obj_set_align(ui_Image19, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_Image19, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CHECKABLE | LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_Image19, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_Image19, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Image19, lv_color_hex(0x101010), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Image19, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Image19, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_color(ui_Image19, lv_color_hex(0x101010), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_opa(ui_Image19, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui_Image19, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_pad(ui_Image19, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_Image19, 5, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui_Image19, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui_Image19, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_color(ui_Image19, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_opa(ui_Image19, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_width(ui_Image19, 10, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_outline_pad(ui_Image19, 0, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_img_recolor(ui_Image19, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_img_recolor_opa(ui_Image19, 255, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_add_event_cb(ui_Image19, camera_open_btn_cb, LV_EVENT_CLICKED, (void *)19);

    /*Create a slider in the center of the display*/
    lv_obj_t * slider = lv_slider_create(ui_controlCenter);
    lv_obj_set_size(slider, 180, 15);
    lv_obj_align_to(slider, ui_controlCenter, LV_ALIGN_BOTTOM_MID, 0, -100);
    lv_obj_t * slider_label = lv_label_create(ui_controlCenter);
    lv_label_set_text(slider_label, "0%");
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, slider_label);

    lv_obj_add_event_cb(ui_controlListener, ui_event_controlListener, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_controlCenter, ui_event_controlCenter, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Slider2, ui_event_lightslider, LV_EVENT_ALL, NULL);

    ui_setting_screen_init();
    ui_wifiset_screen_init();
    ui_aboutui_screen_init();
    ui_datetimeui_screen_init();
    ui_camera_screen_init();
    ui_photos_screen_init();

    _ui_screen_change(ui_home, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0);


    lv_timer_create(updateTime, 1000, NULL);
}

