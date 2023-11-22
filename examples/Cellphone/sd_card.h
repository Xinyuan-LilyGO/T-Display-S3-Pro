#pragma once

#ifndef SD_CARD_H
#define SD_CARD_H

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "lvgl.h"

#define SD_FD_DRI SD

typedef struct {
    uint16_t b : 5;
    uint16_t g : 6;
    uint16_t r : 5;
}rgb565_t;

typedef union{
	rgb565_t rgb;
	uint16_t word;
} color_rgb565_t;

// bmp 文件頭固定為54字節
typedef struct {        // 以小端序存放
    uint16_t type;      // 魔數，一般為 0x4d42，用於識別BMP文件
    uint32_t size;      // BMP文件大小，以字節為單位
    uint16_t reserved1; //
    uint16_t reserved2; //
    uint32_t offs_bits; // 位圖數據的起始地址
} __attribute__((packed)) bmp_file_head_t; // size=14byte

typedef struct { // 目前通用格式的 BITMAPINFOHEADER 版本
    uint32_t size;            // 該結構體的大小 40 byte
    uint32_t w;               // 圖像的寬度
    uint32_t h;               // 圖像的高度
    uint16_t planes;          // 色彩平面數，只有1為有效值
    uint16_t pixel_bit;       // 每個像素所佔大小，即色深（典型值：1,4,8,16,24,32）
    uint32_t compression;     // 使用的壓縮方法
    uint32_t img_size;        // 圖像的大小
    uint32_t hor_resolution;  // 圖像的橫向分辨率
    uint32_t ver_resolution;  // 圖像的縱向分辨率
    uint32_t pallet_cnt;      // 調色板顏色數，為0是默認為 2^(色深) 個
    uint32_t color_important; // 重要顏色數，為0是表示所有顏色都重要
}__attribute__((packed)) bmp_file_info_t; // size=40byte

void sd_card_init(void);
bool sd_card_get_init_flag(void);
bool sd_card_write(const char *path, uint8_t *buf, uint32_t bufLen);
bool sd_card_read(const char *path, uint8_t *buf, size_t size);
bool sd_card_remove(const char *path);
bool sd_card_bmp_img(const char * path, uint32_t w, uint32_t h, uint8_t *buf);
bool sd_card_bmp_lvgl(const char * path, uint32_t w, uint32_t h, lv_obj_t *canvas);
void listDir(const char *dirname, uint8_t levels);

#endif