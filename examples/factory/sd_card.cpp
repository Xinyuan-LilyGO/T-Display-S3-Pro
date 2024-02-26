

#include "sd_card.h"
#include "utilities.h"

static bool sd_init_flag = false;


void sd_card_init(void)
{
    if (!SD.begin(BOARD_SD_CS)) {
        Serial.println("Card Mount Failed");
        return;
    }

    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC) {
        Serial.println("MMC");
    } else if (cardType == CARD_SD) {
        Serial.println("SDSC");
    } else if (cardType == CARD_SDHC) {
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    sd_init_flag = true;
}

bool sd_card_get_init_flag(void)
{
    return sd_init_flag;
}

bool sd_card_write(const char * path, uint8_t *buf, uint32_t bufLen)
{
    File f = SD_FD_DRI.open(path, FILE_WRITE);
    if(!f){
        Serial.println("Failed to open directory");
        return false;
    }
    if(f.write(buf, bufLen) == 0){
        Serial.println("Failed to open file for writing");
    }
    f.close();
    return true;
}

bool sd_card_read(const char * path, uint8_t* buf, size_t size)
{
    File f = SD_FD_DRI.open(path, FILE_WRITE);
    if(!f){
        Serial.println("Failed to open directory");
        return false;
    }
    f.read(buf, size);
    f.close();
    return true;
}

bool sd_card_remove(const char * path)
{
    Serial.printf("Deleting file: %s\n", path);
    if(SD_FD_DRI.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
        return false;
    }
    return true;
}

bool sd_card_bmp_img(const char * path, uint32_t w, uint32_t h, uint8_t *buf)
{
    uint32_t img_size = w * h * 2;

    bmp_file_head_t head;
    memset(&head, 0, sizeof(head));
    head.type = 0x4d42;
    head.size = 54 + img_size;
    head.reserved1 = 0;
    head.reserved2 = 0;
    head.offs_bits = 54;

    bmp_file_info_t info;
    memset(&info, 0, sizeof(info));
    info.size = 40;
    info.w = w;
    info.h = h;
    info.planes = 1;
    info.pixel_bit = 16;
    info.compression = 0;
    info.img_size = img_size;
    info.hor_resolution = 0x0b13;
    info.ver_resolution = 0x0b13;
    info.pallet_cnt = 0;
    info.color_important = 0;

    File f = SD_FD_DRI.open(path, FILE_APPEND);
    f.write((uint8_t*)&head, (unsigned int)sizeof(head));
    f.write((uint8_t*)&info, (unsigned int)sizeof(info));
    // f.write((uint8_t *)buf, img_size/2);
    // f.write((uint8_t *)&buf[img_size/2], img_size/2);

    color_rgb565_t color;
    for(int i = 0; i < h; i++)
        for(int j = 0; j < w; j+=2){
            uint8_t c1, c2;     
            c1 = buf[i * w + j + 0];
            c2 = buf[i * w + j + 1];
            color.word = c1 << 8 | c2;
            f.write((uint8_t *)&color.word, 2);
        }
    
    for(int i = 0; i < h; i++)
        for(int j = 0; j < w; j+=2){
            uint8_t c1, c2;     
            uint8_t tmp_buf[2];
            c1 = buf[img_size/2 + i * w + j + 0];
            c2 = buf[img_size/2 + i * w + j + 1];
            color.word = c1 << 8 | c2;
            f.write((uint8_t *)&color.word, 2);
        }
    f.close();
    return true;
}

bool sd_card_bmp_lvgl(const char * path, uint32_t w, uint32_t h, lv_obj_t *canvas)
{
    uint32_t img_size = w * h * 2;

    bmp_file_head_t head;
    memset(&head, 0, sizeof(head));
    head.type = 0x4d42;
    head.size = 54 + img_size + 16;
    head.reserved1 = 0;
    head.reserved2 = 0;
    head.offs_bits = 54 + 16;

    bmp_file_info_t info;
    memset(&info, 0, sizeof(info));
    info.size = 40 + 16;
    info.w = w;
    info.h = h;
    info.planes = 1;
    info.pixel_bit = 16;
    info.compression = 3;
    info.img_size = img_size;
    info.hor_resolution = 2835;
    info.ver_resolution = 2835;
    info.pallet_cnt = 0;
    info.color_important = 0;

    uint32_t r = 0x0000F800;
    uint32_t g = 0x000007E0;
    uint32_t b = 0x0000001F;
    uint32_t reserved = 0;

    File f = SD_FD_DRI.open(path, FILE_APPEND);
    f.write((uint8_t*)&head, (unsigned int)sizeof(head));
    f.write((uint8_t*)&info, (unsigned int)sizeof(info));
    f.write((uint8_t*)&r, sizeof(r));
    f.write((uint8_t*)&g, sizeof(g));
    f.write((uint8_t*)&b, sizeof(b));
    f.write((uint8_t*)&reserved, sizeof(reserved));

    color_rgb565_t color;
    for(int i = 0; i < h; i++)
        for(int j = 0; j < w; j++){
            lv_color_t lv_color;
            lv_color = lv_canvas_get_px(canvas, j, h - i); 
            color.rgb.r = lv_color.ch.red;
            color.rgb.g = (lv_color.ch.green_h << 3 | lv_color.ch.green_l) & 0x3f;
            color.rgb.b = lv_color.ch.blue;
            f.write((uint8_t *)&color.word, 2);
        }
    f.close();
    return true;
}

void listDir( const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = SD_FD_DRI.open(dirname);
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
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}