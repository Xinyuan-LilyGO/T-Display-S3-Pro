
#include "ui.h"
#include "scr_mrg.h"

#define BOARD_I2C_SDA       5
#define BOARD_I2C_SCL       6

#define BOARD_SPI_MOSI      17
#define BOARD_SPI_SCK       18
#define BOARD_TFT_CS        39
#define BOARD_TFT_RST       47
#define BOARD_TFT_DC        9
#define BOARD_TFT_BL        48

#define BOARD_TOUCH_RST     13
#define BOARD_SENSOR_IRQ    21

#define TFT_WIDTH  222
#define TFT_HEIGHT 480

TFT_eSPI tft = TFT_eSPI();
TouchDrvCSTXXX touch;
int ui_rotation;

static void lv_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area,
                          lv_color_t *color_p)
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );
    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )&color_p->full, w * h, false );
    tft.endWrite();
    lv_disp_flush_ready( disp );
}

static void lv_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    static int16_t x[5], y[5];
    uint8_t touched = touch.getPoint(x, y, touch.getSupportTouchPoint());
    if (touched) {
        data->state = LV_INDEV_STATE_PR;
        if(ui_rotation == SCR_ROTATION_0){
            data->point.x = x[0];
            data->point.y = y[0];
        } 
        else if(ui_rotation == SCR_ROTATION_90){
            data->point.x = y[0];
            data->point.y = TFT_WIDTH - x[0];
        }
        else if(ui_rotation == SCR_ROTATION_180){
            data->point.x = TFT_WIDTH - x[0];
            data->point.y = TFT_HEIGHT - y[0];
        }
        else if(ui_rotation == SCR_ROTATION_270) {
            data->point.x = TFT_HEIGHT - y[0];
            data->point.y = x[0];
        }
        // Serial.print("x=");Serial.print(data->point.x);
        // Serial.print(", y=");Serial.println(data->point.y);
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

void ui_init(uint8_t r)
{
    static lv_color_t *frameBuffer;
    static lv_disp_drv_t disp_drv;
    static lv_disp_draw_buf_t draw_buf;

    ui_rotation = r;

    // Init Display
    tft.init();
    tft.fillScreen(TFT_BLACK);
    tft.setRotation(r);

    touch.setPins(BOARD_TOUCH_RST, BOARD_SENSOR_IRQ);
    bool hasTouch = touch.begin(Wire, CST226SE_SLAVE_ADDRESS, BOARD_I2C_SDA, BOARD_I2C_SCL);
    if (!hasTouch) {
        Serial.println("Failed to find Capacitive Touch !");
    } else {
        Serial.println("Find Capacitive Touch");
    }

    lv_init();

    size_t buffer_size = sizeof(lv_color_t) * tft.width() * tft.height();
    frameBuffer  = (lv_color_t *)ps_malloc(buffer_size);
    assert(frameBuffer);

    lv_disp_drv_init(&disp_drv);
    lv_disp_draw_buf_init(&draw_buf, frameBuffer, NULL, buffer_size);

    disp_drv.hor_res = tft.width();
    disp_drv.ver_res = tft.height();
    disp_drv.flush_cb = lv_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lv_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    pinMode(BOARD_TFT_BL, OUTPUT);
}
/***************************/
lv_obj_t * chart1;
lv_obj_t * chart2;
lv_obj_t * chart3;
lv_chart_series_t * ser1;
lv_chart_series_t * ser2;
lv_chart_series_t * ser3;
lv_obj_t * chart_label1;
lv_obj_t * chart_label2;
lv_obj_t * chart_label3;
lv_timer_t *sensor_timer;

void switch_scr_btn_cb(lv_event_t *e)
{
    int user_data = (int)e->user_data;
    switch (user_data)
    {
        case SCREEN_ID_0: scr_mgr_switch(SCREEN_ID_1, true); break;
        case SCREEN_ID_1: scr_mgr_switch(SCREEN_ID_2, true); break;
        case SCREEN_ID_2: scr_mgr_switch(SCREEN_ID_0, true); break;
        default:
            break;
    }
}

lv_obj_t * ui_chart_create(lv_obj_t *parent)
{
    lv_obj_t *obj = lv_chart_create(parent);
    lv_obj_set_size(obj, 350, 74);
    lv_obj_center(obj);
    lv_chart_set_type(obj, LV_CHART_TYPE_LINE);   /*Show lines and points too*/
    lv_obj_set_style_size(obj, 0, LV_PART_INDICATOR);
    lv_obj_set_style_radius(obj, 0, 0);
    lv_chart_set_point_count(obj, 200);
    lv_obj_set_style_line_width(obj, 1, LV_PART_ITEMS);
    lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN);
    lv_chart_set_div_line_count(obj, 0, 0);

    lv_chart_refresh(obj); /*Required after direct set*/
    return obj;
}
/*********************************************************************************
 *                              SCREEN1
 *********************************************************************************/
void chart_acceleration_timer(lv_timer_t *t)
{
    myIMU.readSensor();
    xyzFloat accRaw = myIMU.getAccRawValues();
    xyzFloat corrAccRaw = myIMU.getCorrectedAccRawValues();
    xyzFloat gVal = myIMU.getGValues();
    float resultantG = myIMU.getResultantG(gVal);

    int x = (gVal.x * 100) + 100;
    int y = (gVal.y * 100) + 100;
    int z = (gVal.z * 100);

    lv_chart_set_next_value(chart1, ser1, lv_map(x, 0, 200, 0, 100));
    lv_chart_set_next_value(chart2, ser2, lv_map(y, 0, 200, 0, 100));
    lv_chart_set_next_value(chart3, ser3, lv_map(z, 0, 300, 0, 100));

    lv_label_set_text_fmt(chart_label1, "#ff0000 acc_x :# %0.2f", gVal.x);
    lv_label_set_text_fmt(chart_label2, "#00ff00 acc_y :# %0.2f", gVal.y);
    lv_label_set_text_fmt(chart_label3, "#0000ff acc_z :# %0.2f", gVal.z);
}

void create1(lv_obj_t *parent) {
    chart1 = ui_chart_create(parent);
    ser1 = lv_chart_add_series(chart1, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    lv_obj_align(chart1, LV_ALIGN_TOP_LEFT, 0, 0);

    chart2 = ui_chart_create(parent);
    ser2 = lv_chart_add_series(chart2, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);
    lv_obj_align(chart2, LV_ALIGN_LEFT_MID, 0, 0);

    chart3 = ui_chart_create(parent);
    ser3 = lv_chart_add_series(chart3, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);
    lv_obj_align(chart3, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, "Acceleration");
    lv_obj_align(label, LV_ALIGN_TOP_RIGHT, -15, 10);

    chart_label1 = lv_label_create(parent);
    lv_label_set_recolor(chart_label1, true);  
    lv_label_set_text(chart_label1, "#ff0000 acc_x :# --");
    lv_obj_align_to(chart_label1, label, LV_ALIGN_OUT_BOTTOM_MID, -10, 20);

    chart_label2 = lv_label_create(parent);
    lv_label_set_recolor(chart_label2, true);  
    lv_label_set_text(chart_label2, "#00ff00 acc_y :# --");
    lv_obj_align_to(chart_label2, chart_label1, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);

    chart_label3 = lv_label_create(parent);
    lv_label_set_recolor(chart_label3, true);  
    lv_label_set_text(chart_label3, "#0000ff acc_z :# --");
    lv_obj_align_to(chart_label3, chart_label2, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);

    lv_obj_t *next_btn = lv_btn_create(parent);
    lv_obj_set_size(next_btn, 80, 40);
    lv_obj_align(next_btn, LV_ALIGN_BOTTOM_RIGHT, -25, -10);
    lv_obj_add_event_cb(next_btn, switch_scr_btn_cb, LV_EVENT_CLICKED, (void *)SCREEN_ID_0);

    label = lv_label_create(next_btn);
    lv_label_set_text(label, "next");
    lv_obj_center(label);

    sensor_timer = lv_timer_create(chart_acceleration_timer, 50, NULL);
    printf("create1\n");
}

void entry1(void) {   }
void exit1(void) {    }
void destroy1(void) { lv_timer_del(sensor_timer); };

scr_lifecycle_t screen1 = {
    .create = create1,
    .entry = entry1,
    .exit  = exit1,
    .destroy = destroy1,
};
/*********************************************************************************
 *                              SCREEN2
 *********************************************************************************/
void chart_gyroscope_timer(lv_timer_t *t)
{
    myIMU.readSensor();
    xyzFloat gyrRaw = myIMU.getCorrectedGyrRawValues();
    xyzFloat gyr = myIMU.getGyrValues();

    int x = gyr.x + 200;
    int y = gyr.y + 200;
    int z = gyr.z + 200;

    lv_chart_set_next_value(chart1, ser1, lv_map(x, 0, 400, 0, 100));
    lv_chart_set_next_value(chart2, ser2, lv_map(y, 0, 400, 0, 100));
    lv_chart_set_next_value(chart3, ser3, lv_map(z, 0, 400, 0, 100));

    lv_label_set_text_fmt(chart_label1, "#ff0000 gyr_x :# %0.2f", gyr.x);
    lv_label_set_text_fmt(chart_label2, "#00ff00 gyr_y :# %0.2f", gyr.y);
    lv_label_set_text_fmt(chart_label3, "#0000ff gyr_z :# %0.2f", gyr.z);
}

void create2(lv_obj_t *parent)
{
    chart1 = ui_chart_create(parent);
    ser1 = lv_chart_add_series(chart1, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    lv_obj_align(chart1, LV_ALIGN_TOP_LEFT, 0, 0);

    chart2 = ui_chart_create(parent);
    ser2 = lv_chart_add_series(chart2, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);
    lv_obj_align(chart2, LV_ALIGN_LEFT_MID, 0, 0);

    chart3 = ui_chart_create(parent);
    ser3 = lv_chart_add_series(chart3, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);
    lv_obj_align(chart3, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, "Gyroscope");
    lv_obj_align(label, LV_ALIGN_TOP_RIGHT, -25, 10);

    chart_label1 = lv_label_create(parent);
    lv_label_set_recolor(chart_label1, true);  
    lv_label_set_text(chart_label1, "#ff0000 gyr_x :# --");
    lv_obj_align_to(chart_label1, label, LV_ALIGN_OUT_BOTTOM_MID, -10, 20);

    chart_label2 = lv_label_create(parent);
    lv_label_set_recolor(chart_label2, true);  
    lv_label_set_text(chart_label2, "#00ff00 gyr_y :# --");
    lv_obj_align_to(chart_label2, chart_label1, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);

    chart_label3 = lv_label_create(parent);
    lv_label_set_recolor(chart_label3, true);  
    lv_label_set_text(chart_label3, "#0000ff gyr_z :# --");
    lv_obj_align_to(chart_label3, chart_label2, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);

    lv_obj_t *next_btn = lv_btn_create(parent);
    lv_obj_set_size(next_btn, 80, 40);
    lv_obj_align(next_btn, LV_ALIGN_BOTTOM_RIGHT, -25, -10);
    lv_obj_add_event_cb(next_btn, switch_scr_btn_cb, LV_EVENT_CLICKED, (void *)SCREEN_ID_1);

    label = lv_label_create(next_btn);
    lv_label_set_text(label, "next");
    lv_obj_center(label);

    sensor_timer = lv_timer_create(chart_gyroscope_timer, 50, NULL);
}
void entry2(void) {   }
void exit2(void) {    }
void destroy2(void) { lv_timer_del(sensor_timer); };

scr_lifecycle_t screen2 = {
    .create = create2,
    .entry = entry2,
    .exit  = exit2,
    .destroy = destroy2,
};
/*********************************************************************************
 *                              SCREEN3
 *********************************************************************************/
void chart_magnetometer_timer(lv_timer_t *t)
{
    myIMU.readSensor();
    xyzFloat magValue = myIMU.getMagValues(); // returns magnetic flux density [µT]

    int x = magValue.x + 400;
    int y = magValue.y + 400;
    int z = magValue.z + 400;

    lv_chart_set_next_value(chart1, ser1, lv_map(x, 0, 800, 0, 100));
    lv_chart_set_next_value(chart2, ser2, lv_map(y, 0, 800, 0, 100));
    lv_chart_set_next_value(chart3, ser3, lv_map(z, 0, 800, 0, 100));

    lv_label_set_text_fmt(chart_label1, "#ff0000 mag_x :# %0.2f", magValue.x);
    lv_label_set_text_fmt(chart_label2, "#00ff00 mag_y :# %0.2f", magValue.y);
    lv_label_set_text_fmt(chart_label3, "#0000ff mag_z :# %0.2f", magValue.z);
}

void create3(lv_obj_t *parent)
{
    chart1 = ui_chart_create(parent);
    ser1 = lv_chart_add_series(chart1, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    lv_obj_align(chart1, LV_ALIGN_TOP_LEFT, 0, 0);

    chart2 = ui_chart_create(parent);
    ser2 = lv_chart_add_series(chart2, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);
    lv_obj_align(chart2, LV_ALIGN_LEFT_MID, 0, 0);

    chart3 = ui_chart_create(parent);
    ser3 = lv_chart_add_series(chart3, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);
    lv_obj_align(chart3, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, "Magnetometer");
    lv_obj_align(label, LV_ALIGN_TOP_RIGHT, -10, 10);

    chart_label1 = lv_label_create(parent);
    lv_label_set_recolor(chart_label1, true);  
    lv_label_set_text(chart_label1, "#ff0000 mag_x :# --");
    lv_obj_align_to(chart_label1, label, LV_ALIGN_OUT_BOTTOM_MID, -10, 20);

    chart_label2 = lv_label_create(parent);
    lv_label_set_recolor(chart_label2, true);  
    lv_label_set_text(chart_label2, "#00ff00 mag_y :# --");
    lv_obj_align_to(chart_label2, chart_label1, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);

    chart_label3 = lv_label_create(parent);
    lv_label_set_recolor(chart_label3, true);  
    lv_label_set_text(chart_label3, "#0000ff mag_z :# --");
    lv_obj_align_to(chart_label3, chart_label2, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);

    lv_obj_t *next_btn = lv_btn_create(parent);
    lv_obj_set_size(next_btn, 80, 40);
    lv_obj_align(next_btn, LV_ALIGN_BOTTOM_RIGHT, -25, -10);
    lv_obj_add_event_cb(next_btn, switch_scr_btn_cb, LV_EVENT_CLICKED, (void *)SCREEN_ID_2);

    label = lv_label_create(next_btn);
    lv_label_set_text(label, "next");
    lv_obj_center(label);

    sensor_timer = lv_timer_create(chart_magnetometer_timer, 50, NULL);
}
void entry3(void) {   }
void exit3(void) {    }
void destroy3(void) { lv_timer_del(sensor_timer); };

scr_lifecycle_t screen3 = {
    .create = create3,
    .entry = entry3,
    .exit  = exit3,
    .destroy = destroy3,
};


void ui_enter(void)
{
    scr_mgr_init();

    scr_mgr_register(SCREEN_ID_0, &screen1);
    scr_mgr_register(SCREEN_ID_1, &screen2);
    scr_mgr_register(SCREEN_ID_2, &screen3);

    scr_mgr_switch(SCREEN_ID_0, false);
}


