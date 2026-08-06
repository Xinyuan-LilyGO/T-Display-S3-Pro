/**
 * PointShoot - T-Display-S3-Pro + OV5640 camera shield as a point-and-shoot.
 *
 *   btn1 (GPIO0)  WIFI  - bottom-left  - photo-transfer AP, gallery at 192.168.4.1
 *   btn2 (GPIO12) not populated on this board
 *   btn3 (GPIO16) SHOOT - bottom-right - full-res JPEG to SD if present, else flash
 *   touch          tap viewfinder = shutter, tap strip above it = torch
 *
 * Camera pin map and PMU setup lifted from the CameraShield example.
 */
#include <Arduino.h>
#include <esp_camera.h>
#include <TJpg_Decoder.h>
#include <TouchDrvCSTXXX.hpp>
#include <ESP32_OV5640_AF.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <XPowersLib.h>
#include <SPI.h>
#include <SD.h>
#include <FFat.h>
#include <Preferences.h>
#include <sys/time.h>
#include "utilities.h"

#define AP_SSID      "TDisplayCam"
#define AP_PASS      "photos1234"   // >=8 chars or softAP stays open

// How the sensor is mounted relative to the panel is a property of the shield,
// not something to guess at: orient is one element of the dihedral group,
// settable live by sending '0'-'7' over serial.
//   bits 0-1  quarter turns clockwise applied when displaying
//   bit  2    mirror horizontally first
// All 8 are reachable, so exactly one of them is correct. The sensor's own
// mirror/flip registers are forced off so this is the only thing acting.
// Measured on this shield: the sensor image needs a vertical flip. It first
// looked like a horizontal mirror, but that was judged against a display that
// was itself rotated 180; with the panel the right way up the correction is
// R180 composed with that mirror, which is exactly a vflip.
#define MOUNT_MIRROR 0
#define MOUNT_VFLIP  1

#define ORIENT_DEFAULT 0
static uint8_t orient = ORIENT_DEFAULT;

// Everything runs at 10MHz XCLK. Measured on this sensor: at 20MHz, full-res
// frames come out at 0.56-0.61x the preview's mean brightness at every
// resolution, because the full-res frame timing caps integration. At 10MHz the
// still matches the preview (ratio 1.01); 8MHz overshoots to 1.14.
#define CAM_XCLK_HZ    (10 * 1000000)
#define STILL_SIZE     FRAMESIZE_QSXGA   // 2560x1920
#define PREVIEW_SIZE   FRAMESIZE_QVGA    // 320x240
#define SETTLE_FRAMES  0                 // frames to drop after a size change

// The camera is initialised once, in JPEG mode at the largest frame size, and
// a shot is just a set_framesize() away. Tearing it down and re-initialising
// per shot cost ~2.5s of the ~2.9s shutter lag, and threw away the converged
// exposure and the uploaded autofocus firmware every time.
// Panel the right way up: text reads correctly and the SHOOT/WIFI labels land
// on the bottom edge, over btn1 (left) and btn3 (right).
#define DISPLAY_ROTATION 0

#define ORI_ROT    (orient & 3)
#define ORI_MIRROR ((orient >> 2) & 1)

// EXIF orientation for each mode, so stills come out upright in Photos without
// rotating 5M pixels on the ESP32. Index is (mirror << 2) | rot.
static const uint8_t EXIF_FOR_ORIENT[8] = {1, 6, 3, 8, 2, 7, 4, 5};
#define EXIF_ORIENT (EXIF_FOR_ORIENT[orient & 7])

#define SRC_W 320
#define SRC_H 240
#define VF_W  222                       // preview is fitted to the panel width
#define VF_H_MAX 296                    // 3:4 (quarter-turned), the taller case
static int vfH = VF_H_MAX;
static int vfY = (480 - VF_H_MAX) / 2;

TFT_eSPI    tft;
PowersSY6970 PMU;
WebServer   server(80);
DNSServer   dns;
Preferences prefs;

static fs::FS     *store    = nullptr;
static const char *storeName = "none";
static uint16_t   *vf       = nullptr;   // rotated viewfinder buffer (PSRAM)
static bool        wifiOn   = false;
static char        status[48] = "";
static uint16_t    battMv     = 0;
static char        sdInfo[48] = "not probed";
static uint16_t    photoCount = 0;
static uint32_t    photoBytes = 0;
static uint8_t     battPct    = 0;
static bool        battChg    = false;

// The SY6970 has no fuel gauge, so state of charge is inferred from voltage
// against a LiPo discharge curve. That sags under load - a capture pulls the
// reading down - so the voltage is smoothed before conversion.
static uint8_t battPercent(uint16_t mv)
{
    static const uint16_t curve[][2] = {
        {3300, 0}, {3600, 10}, {3700, 25}, {3750, 40},
        {3850, 60}, {3950, 75}, {4050, 90}, {4200, 100},
    };
    const int n = sizeof(curve) / sizeof(curve[0]);
    if (mv <= curve[0][0]) return 0;
    for (int i = 1; i < n; i++) {
        if (mv < curve[i][0]) {
            uint16_t v0 = curve[i - 1][0], p0 = curve[i - 1][1];
            return p0 + (uint32_t)(mv - v0) * (curve[i][1] - p0) / (curve[i][0] - v0);
        }
    }
    return 100;
}

// Percentage is meaningless while the charger holds the terminal voltage up.
static const char *battPctStr()
{
    static char b[8];
    if (battChg) strcpy(b, "chg");
    else         snprintf(b, sizeof(b), "%u%%", battPct);
    return b;
}

static void pollBattery()
{
    static uint32_t next = 0;
    if (millis() < next) return;
    next = millis() + 2000;

    uint16_t mv = PMU.getBattVoltage();
    if (mv < 2500) return;                       // no battery / bad reading

    // While charging the terminal voltage is held up by the charger and is not
    // the cell's resting voltage, so the curve would read near-full whatever
    // the real charge. Only convert while discharging, and drop the smoothing
    // history when the state flips so the first reading off USB is honest.
    bool chg = PMU.isCharging() || PMU.isVbusIn();
    if (chg != battChg) { battChg = chg; battMv = 0; }

    battMv = battMv ? (battMv * 7 + mv) / 8 : mv;
    if (!battChg) battPct = battPercent(battMv);
}

// ---------------------------------------------------------------- camera ---

static camera_config_t baseConfig()
{
    camera_config_t c = {};
    c.ledc_channel  = LEDC_CHANNEL_0;
    c.ledc_timer    = LEDC_TIMER_0;
    c.pin_d0 = CAMERA_PIN_Y2;  c.pin_d1 = CAMERA_PIN_Y3;
    c.pin_d2 = CAMERA_PIN_Y4;  c.pin_d3 = CAMERA_PIN_Y5;
    c.pin_d4 = CAMERA_PIN_Y6;  c.pin_d5 = CAMERA_PIN_Y7;
    c.pin_d6 = CAMERA_PIN_Y8;  c.pin_d7 = CAMERA_PIN_Y9;
    c.pin_xclk      = CAMERA_PIN_XCLK;
    c.pin_pclk      = CAMERA_PIN_PCLK;
    c.pin_vsync     = CAMERA_PIN_VSYNC;
    c.pin_href      = CAMERA_PIN_HREF;
    // Touch, PMU and the camera's SCCB all sit on GPIO 5/6. Letting the camera
    // install its own I2C driver there breaks Wire, and touch.begin() fails.
    // pin_sccb_sda = -1 makes it reuse the bus Wire already configured.
    c.pin_sccb_sda  = -1;
    c.pin_sccb_scl  = CAMERA_PIN_SIOC;
    c.sccb_i2c_port = 0;
    c.pin_pwdn      = CAMERA_PIN_PWDN;
    c.pin_reset     = CAMERA_PIN_RESET;
    c.xclk_freq_hz  = XCLK_FREQ_HZ;
    c.fb_location   = CAMERA_FB_IN_PSRAM;
    c.grab_mode     = CAMERA_GRAB_WHEN_EMPTY;
    return c;
}

// The shield mounts the OV5640 mirrored. Correct that in the sensor rather
// than with an EXIF tag, so the stored JPEG is right for any viewer and not
// just EXIF-aware ones. Everything downstream then needs no transform at all.
static void applySensorOrient()
{
    sensor_t *s = esp_camera_sensor_get();
    if (!s) return;
    s->set_hmirror(s, MOUNT_MIRROR);
    s->set_vflip(s, MOUNT_VFLIP);
}

static OV5640 af;
static bool afReady = false;

#define FW_STATUS_S_CONSTANT 0x20   // undocumented in the lib: constant AF live

// btn2 is not populated, so the panel carries the torch. Tap the viewfinder to
// shoot, tap the strip above it to toggle the fill LED.
static TouchDrvCSTXXX touch;
static bool touchReady = false;
static bool torchOn    = false;

#define STILL_W    2560
#define PREVIEW_W  320
static bool setFramesize(framesize_t fs, int wantW);

// One init for the life of the session: JPEG at the largest size we will ever
// want, so set_framesize() can move between preview and stills without losing
// exposure or the autofocus firmware.
static bool startCamera()
{
    camera_config_t c = baseConfig();
    c.pixel_format = PIXFORMAT_JPEG;
    c.frame_size   = STILL_SIZE;
    c.xclk_freq_hz = CAM_XCLK_HZ;
    c.jpeg_quality = 10;
    c.fb_count     = 1;
    if (esp_camera_init(&c) != ESP_OK) return false;

    applySensorOrient();

    sensor_t *s = esp_camera_sensor_get();
    // The AF firmware is a blob uploaded over SCCB; it survives framesize
    // changes, so this only happens here.
    afReady = af.start(s) && af.focusInit() == 0 && af.autoFocusMode() == 0;

    // Internal flash writes at ~55KB/s because NOR sector erase costs ~40ms
    // per 4KB, so on flash the file size *is* the shutter lag. Trade some
    // quality back for it; an SD card has no such problem.
    s->set_quality(s, store == &SD ? 10 : 14);

    setFramesize(PREVIEW_SIZE, PREVIEW_W);
    return true;
}

// set_framesize() takes effect a few frames later, and frames captured in the
// meantime still carry the old geometry (and can overflow the DMA buffers).
// Drain until one actually arrives at the requested width, then drop a couple
// more so exposure has caught up.
static bool setFramesize(framesize_t fs, int wantW)
{
    sensor_t *s = esp_camera_sensor_get();
    if (!s) return false;
    if (s->set_framesize(s, fs) != 0) return false;
    delay(100);

    for (int i = 0; i < 12; i++) {
        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) { delay(20); continue; }
        bool match = fb->width == wantW;
        esp_camera_fb_return(fb);
        if (match) {
            for (int j = 0; j < SETTLE_FRAMES; j++) {
                camera_fb_t *k = esp_camera_fb_get();
                if (k) esp_camera_fb_return(k);
            }
            return true;
        }
    }
    return false;
}

// --------------------------------------------------------------- storage ---

static void initStorage()
{
    // Probe SD before the display so a failed probe cannot leave the shared
    // SPI bus half-configured under TFT_eSPI. The display is on the same bus
    // and its CS is still floating at this point, so it can answer and corrupt
    // the card's replies - hold it deasserted.
    pinMode(BOARD_TFT_CS, OUTPUT);
    digitalWrite(BOARD_TFT_CS, HIGH);
    pinMode(BOARD_SD_CS, OUTPUT);
    digitalWrite(BOARD_SD_CS, HIGH);

    SPI.begin(BOARD_SPI_SCK, BOARD_SPI_MISO, BOARD_SPI_MOSI);
    // SD cards must be initialised at <=400kHz; some refuse faster probes.
    for (uint32_t hz : {20000000u, 4000000u, 1000000u, 400000u}) {
        if (SD.begin(BOARD_SD_CS, SPI, hz)) {
            store = &SD;
            storeName = "SD";
            snprintf(sdInfo, sizeof(sdInfo), "ok %luHz %lluMB",
                     (unsigned long)hz, (unsigned long long)(SD.cardSize() >> 20));
            return;
        }
        SD.end();
    }

    // Do NOT pass format_if_empty here: arduino-esp32's format path double-frees
    // (assert in multi_heap_free) and panics into a boot loop. The card must be
    // formatted FAT32 externally; exFAT, which cards ship with, is unreadable
    // by this library.
    snprintf(sdInfo, sizeof(sdInfo), "no FAT32");
    if (FFat.begin(true)) {
        store = &FFat;
        storeName = "flash";
    }
}

// Free space means little on a camera; shots taken and shots left mean a lot.
// Capacity is estimated from the average size of what has actually been
// stored, so it tracks the scene and quality setting rather than a guess.
static void scanPhotos()
{
    photoCount = 0;
    photoBytes = 0;
    if (!store) return;
    File dir = store->open("/");
    for (File f = dir.openNextFile(); f; f = dir.openNextFile()) {
        String n = f.name();
        if (n.endsWith(".JPG")) { photoCount++; photoBytes += f.size(); }
        f.close();
    }
}

static uint64_t freeBytes()
{
    if (store == &SD)  return SD.totalBytes() - SD.usedBytes();
    if (store == &FFat) return FFat.freeBytes();
    return 0;
}

// EXIF APP1 carrying nothing but an Orientation tag, so Photos rotates the
// 5MP image on display instead of us rotating 5M pixels on an ESP32.
static uint8_t EXIF_APP1[] = {
    0xFF, 0xE1, 0x00, 0x22, 'E', 'x', 'i', 'f', 0x00, 0x00,
    0x49, 0x49, 0x2A, 0x00, 0x08, 0x00, 0x00, 0x00,   // little-endian TIFF
    0x01, 0x00,                                        // 1 IFD entry
    0x12, 0x01, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00,    // tag 0x0112, SHORT, 1
    0x06, 0x00, 0x00, 0x00,                            // [28] orientation value
    0x00, 0x00, 0x00, 0x00                             // no next IFD
};
#define EXIF_ORIENT_BYTE 28

static bool saveJpeg(const uint8_t *buf, size_t len, char *nameOut, size_t nameLen)
{
    uint32_t n = prefs.getUInt("n", 0) + 1;
    snprintf(nameOut, nameLen, "/IMG_%04u.JPG", n);

    File f = store->open(nameOut, FILE_WRITE);
    if (!f) return false;

    EXIF_APP1[EXIF_ORIENT_BYTE] = EXIF_ORIENT;

    bool ok = true;
    size_t off = 0;
    if (len > 2 && buf[0] == 0xFF && buf[1] == 0xD8) {
        ok &= f.write(buf, 2) == 2;                                   // SOI
        ok &= f.write(EXIF_APP1, sizeof(EXIF_APP1)) == sizeof(EXIF_APP1);
        off = 2;
    }

    // The frame lives in PSRAM, which the flash write path cannot DMA from;
    // staging through an internal-RAM buffer in sector-sized chunks is much
    // faster than handing it one large PSRAM-backed write.
    static uint8_t chunk[4096];
    while (ok && off < len) {
        size_t n = len - off;
        if (n > sizeof(chunk)) n = sizeof(chunk);
        memcpy(chunk, buf + off, n);
        ok &= f.write(chunk, n) == n;
        off += n;
    }
    f.close();

    if (ok) {
        prefs.putUInt("n", n);
        photoCount++;
        photoBytes += len;
    } else {
        store->remove(nameOut);
    }
    return ok;
}

// ---------------------------------------------------------------- capture ---

static void measureSharpness();
static bool inFocus();
extern uint16_t *decoded;

// Let a hunt finish before the shutter fires, judged on the image itself -
// the AF firmware's status register reports success unconditionally on this
// module. Never block: a scene it cannot resolve would hang the shutter.
static bool waitForFocus(uint32_t timeoutMs)
{
    uint32_t t0 = millis();
    while (millis() - t0 < timeoutMs) {
        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) { delay(20); continue; }
        bool ok = decoded && TJpgDec.drawJpg(0, 0, fb->buf, fb->len) == JDR_OK;
        esp_camera_fb_return(fb);
        if (ok) {
            measureSharpness();
            if (inFocus()) return true;
        }
    }
    return false;
}

// Switching to full res and then grabbing are the same operation: the frame
// that proves the new size took effect is itself the photo. Waiting for one
// and then fetching another costs an extra full-res readout, ~0.9s.
static camera_fb_t *grabStill()
{
    sensor_t *sn = esp_camera_sensor_get();
    if (!sn || sn->set_framesize(sn, STILL_SIZE) != 0) return nullptr;
    delay(100);
    for (int i = 0; i < 12; i++) {
        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) { delay(20); continue; }
        if (fb->width == STILL_W) return fb;
        esp_camera_fb_return(fb);
    }
    return nullptr;
}

static void capture()
{
    strcpy(status, "AF wait");
    bool locked = waitForFocus(1200);
    strcpy(status, "capturing...");


    camera_fb_t *fb = grabStill();
    if (!fb) {
        strcpy(status, "capture FAILED");
        setFramesize(PREVIEW_SIZE, PREVIEW_W);
        return;
    }

    // Copy the frame out and hand the camera back to small frames before
    // writing: at full res the sensor keeps streaming into PSRAM for the whole
    // write, and that contention dominates the shutter lag.
    size_t len = fb->len;
    uint8_t *jpg = (uint8_t *)ps_malloc(len);
    if (jpg) memcpy(jpg, fb->buf, len);
    esp_camera_fb_return(fb);

    setFramesize(PREVIEW_SIZE, PREVIEW_W);

    if (!jpg) {
        strcpy(status, "out of memory");
        return;
    }
    char name[24];
    if (saveJpeg(jpg, len, name, sizeof(name)))
        snprintf(status, sizeof(status), "%.8s %uKB%s", name + 1,
                 (unsigned)(len / 1024), locked ? "" : " ~");   // ~ = soft
    else
        strcpy(status, "write FAILED");
    free(jpg);
}

// ---------------------------------------------------------------- gallery ---

static void handleRoot()
{
    // The board has no RTC and no internet in AP mode, so photo timestamps
    // would all read 1970. The browser knows the time - take it from there on
    // page load, which makes the time-based deletes below meaningful.
    String p = F("<!doctype html><meta name=viewport content='width=device-width,initial-scale=1'>"
                 "<style>body{font:17px -apple-system;margin:16px}"
                 "a.f{display:block;padding:12px 0;border-bottom:1px solid #ccc;"
                 "text-decoration:none}"
                 ".b{display:inline-block;margin:4px 8px 12px 0;padding:8px 12px;"
                 "border:1px solid #c00;border-radius:6px;color:#c00;"
                 "text-decoration:none;font-size:15px}</style>"
                 "<script>fetch('/time?t='+Math.floor(Date.now()/1000));</script>"
                 "<h2>Photos</h2>"
                 "<a class=b href='/delh?h=1' onclick=\"return confirm('Delete photos from the last hour?')\">Last hour</a>"
                 "<a class=b href='/delh?h=24' onclick=\"return confirm('Delete photos from the last 24 hours?')\">Last 24h</a>"
                 "<a class=b href='/delall' onclick=\"return confirm('Delete ALL photos? This cannot be undone.')\">Delete all</a>");
    File dir = store->open("/");
    int n = 0;
    for (File f = dir.openNextFile(); f; f = dir.openNextFile()) {
        String name = f.name();
        if (!name.startsWith("/")) name = "/" + name;
        if (name.endsWith(".JPG")) {
            p += "<a class=f href='/img?f=" + name + "'>" + name.substring(1) +
                 "  <small>" + String(f.size() / 1024) + "KB</small>"
                 "<a href='/del?f=" + name + "' style='float:right;color:#c00'>"
                 "delete</a></a>";
            n++;
        }
        f.close();
    }
    if (!n) p += F("<p>No photos yet.</p>");
    p += F("<p><small>Tap a photo, then long-press it and choose "
           "<b>Add to Photos</b>.<br>If saving is unavailable, this is the "
           "captive sign-in window - open <b>http://192.168.4.1</b> in Safari "
           "instead.</small></p>");
    server.send(200, "text/html", p);
}

static void handleImg()
{
    String f = server.arg("f");
    if (!f.startsWith("/") || f.indexOf("..") >= 0) { server.send(400, "text/plain", "bad name"); return; }
    File file = store->open(f);
    if (!file) { server.send(404, "text/plain", "not found"); return; }
    server.streamFile(file, "image/jpeg");
    file.close();
}

static void handleDel()
{
    String f = server.arg("f");
    if (f.startsWith("/") && f.indexOf("..") < 0) { store->remove(f); scanPhotos(); }
    server.sendHeader("Location", "/");
    server.send(303);
}

// Delete every photo, or only those newer than `since`. A zero `since` means
// no time filter. Files written before the clock was set carry a 1970 stamp
// and so never match a time window - they are only removed by "delete all",
// which is the safe way round.
static int deletePhotos(time_t since)
{
    if (!store) return 0;
    int n = 0;
    File dir = store->open("/");
    String doomed[64];
    int count = 0;
    for (File f = dir.openNextFile(); f && count < 64; f = dir.openNextFile()) {
        String name = f.name();
        if (!name.startsWith("/")) name = "/" + name;
        if (name.endsWith(".JPG") && (since == 0 || f.getLastWrite() >= since))
            doomed[count++] = name;
        f.close();
    }
    dir.close();
    for (int i = 0; i < count; i++)
        if (store->remove(doomed[i])) n++;
    scanPhotos();
    return n;
}

static void handleDelAll()
{
    int n = deletePhotos(0);
    snprintf(status, sizeof(status), "deleted %d", n);
    server.sendHeader("Location", "/");
    server.send(303);
}

static void handleDelHours()
{
    long h = server.arg("h").toInt();
    time_t now = time(nullptr);
    // Refuse a time-based delete until the clock has been set, or "recent"
    // would be meaningless and could match everything or nothing.
    if (h <= 0 || now < 1600000000) {
        server.send(409, "text/plain", "clock not set - reload the gallery first");
        return;
    }
    int n = deletePhotos(now - (time_t)h * 3600);
    snprintf(status, sizeof(status), "deleted %d", n);
    server.sendHeader("Location", "/");
    server.send(303);
}

// Phones probe a known URL to decide whether a network has internet. Serving
// anything other than the expected response makes them declare a captive
// portal and pop the sign-in sheet, which is where the gallery appears.
static void handleCaptive()
{
    server.sendHeader("Location", "http://192.168.4.1/", true);
    server.send(302, "text/plain", "");
}

static void handleTime()
{
    long t = server.arg("t").toInt();
    if (t > 1600000000) {
        struct timeval tv = { .tv_sec = (time_t)t, .tv_usec = 0 };
        settimeofday(&tv, nullptr);
    }
    server.send(200, "text/plain", "ok");
}

static void toggleWifi()
{
    wifiOn = !wifiOn;
    if (wifiOn) {
        // The camera stays initialised. Tearing it down here used to be needed
        // when the preview held two RGB565 buffers, but it now shares Wire for
        // SCCB, and esp_camera_deinit() takes the I2C driver with it - which
        // kills touch and the PMU the moment the loop next polls them.
        // Preview simply stops being drawn while the AP is up.
        WiFi.mode(WIFI_AP);
        if (!WiFi.softAP(AP_SSID, AP_PASS)) {
            strcpy(status, "AP FAILED");
            wifiOn = false;
            return;
        }
        server.on("/", handleRoot);
        server.on("/img", handleImg);
        server.on("/del", handleDel);
        server.on("/delall", handleDelAll);
        server.on("/delh", handleDelHours);
        server.on("/time", handleTime);
        server.onNotFound(handleCaptive);
        dns.setTTL(0);
        dns.start(53, "*", WiFi.softAPIP());
        server.begin();
        strcpy(status, "AP up");
        tft.fillRect(0, vfY, VF_W, vfH, TFT_BLACK);
    } else {
        dns.stop();
        server.stop();
        WiFi.softAPdisconnect(true);
        WiFi.mode(WIFI_OFF);
        strcpy(status, "ready");
    }
}

// ------------------------------------------------------------------- draw ---

// A quarter turn swaps the aspect, so the fitted height depends on the mode.
static void updateViewfinderRect()
{
    vfH = (ORI_ROT & 1) ? (VF_W * SRC_W) / SRC_H    // 222 * 4/3 = 296
                        : (VF_W * SRC_H) / SRC_W;   // 222 * 3/4 = 166
    vfY = (480 - vfH) / 2;
    tft.fillScreen(TFT_BLACK);
}

// TJpg_Decoder hands back MCU blocks; collect them into a full frame so the
// orientation/scale pass below stays a simple array read.
uint16_t *decoded = nullptr;

static bool jpegBlock(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bmp)
{
    if (!decoded) return false;
    for (uint16_t r = 0; r < h; r++) {
        int dy = y + r;
        if (dy < 0 || dy >= SRC_H) continue;
        for (uint16_t col = 0; col < w; col++) {
            int dx = x + col;
            if (dx >= 0 && dx < SRC_W) decoded[dy * SRC_W + dx] = bmp[r * w + col];
        }
    }
    return true;
}

static void drawViewfinder()
{
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) return;
    bool ok = decoded && vf &&
              TJpgDec.drawJpg(0, 0, fb->buf, fb->len) == JDR_OK;
    esp_camera_fb_return(fb);
    if (ok) {
        const uint16_t *src = decoded;
        const int rot = ORI_ROT, mir = ORI_MIRROR;

        for (int oy = 0; oy < vfH; oy++) {
            uint16_t *dst = vf + oy * VF_W;
            // b runs down the displayed image, a across it, both 0..4095.
            const int b = (oy * 4095) / (vfH - 1);
            for (int ox = 0; ox < VF_W; ox++) {
                const int a = (ox * 4095) / (VF_W - 1);
                int p, q;                       // position within the source
                switch (rot) {
                case 0:  p = a;        q = b;        break;
                case 1:  p = b;        q = 4095 - a; break;   // 90 CW
                case 2:  p = 4095 - a; q = 4095 - b; break;   // 180
                default: p = 4095 - b; q = a;        break;   // 270 CW
                }
                if (mir) p = 4095 - p;
                dst[ox] = src[((q * (SRC_H - 1)) / 4095) * SRC_W +
                              ((p * (SRC_W - 1)) / 4095)];
            }
        }
        tft.setAddrWindow(0, vfY, VF_W, vfH);
        tft.pushColors(vf, VF_W * vfH, false);
    }
}

// Continuous AF hunts constantly, so this is live state, not a one-off: green
// once the firmware reports focus, red while it is still hunting. Polled on a
// timer rather than every frame - it is an I2C read on the bus the camera and
// touch also share.
// The AF firmware's status register is useless as a focus signal on this
// module: it reports 0x20 (constant AF running) whether or not anything is in
// focus, and a single-focus trigger never reaches 0x10. So measure the image
// instead - mean absolute horizontal gradient over the centre of the preview,
// using the green channel as a cheap luma proxy. Out of focus, or flat against
// a surface, there is no high-frequency detail and this collapses.
static uint32_t sharpness = 0, sharpPeak = 16;

static void measureSharpness()
{
    if (!decoded) return;
    uint64_t sum = 0;
    for (int y = SRC_H / 4; y < SRC_H * 3 / 4; y++) {
        const uint16_t *row = decoded + y * SRC_W;
        for (int x = SRC_W / 4; x < SRC_W * 3 / 4 - 1; x++) {
            int d = (int)((row[x + 1] >> 5) & 0x3F) - (int)((row[x] >> 5) & 0x3F);
            sum += (uint32_t)(d * d);     // squared: edges dominate noise
        }
    }
    uint32_t now = (uint32_t)(sum / ((SRC_H / 2) * (SRC_W / 2 - 1)));
    sharpness = (sharpness * 3 + now) / 4;

    // Judge against a slowly decaying peak rather than a fixed threshold, so
    // the meaning of "sharp" adapts to the scene's own contrast.
    // Rise immediately, fall slowly (~17s half-life): a brief blur must not
    // drag the reference down to meet it, or everything reads as focused.
    static uint32_t nextDecay = 0;
    if (sharpness > sharpPeak) {
        sharpPeak = sharpness;
    } else if (millis() > nextDecay) {
        nextDecay = millis() + 500;
        if (sharpPeak > 16) sharpPeak -= (sharpPeak / 50) + 1;
    }
}

static bool inFocus()
{
    // Relative to the recent best, and above an absolute floor so a flat,
    // detail-free frame cannot qualify however long it is held.
    return sharpness > 24 && sharpness * 10 > sharpPeak * 6;
}

static void drawStatus()
{
    static char shown[48] = "\1";
    static bool shownWifi = !wifiOn;
    static int shownFocus = -1;
    static int shownBatt = -1;
    if (!strcmp(shown, status) && shownWifi == wifiOn &&
        shownFocus == (int)inFocus() && shownBatt == (int)(battChg ? -2 : battPct))
        return;
    strcpy(shown, status);
    shownWifi = wifiOn;
    shownFocus = (int)inFocus();
    shownBatt  = (int)(battChg ? -2 : battPct);

    tft.fillRect(0, 0, VF_W, vfY, TFT_BLACK);
    tft.fillRect(0, vfY + vfH, VF_W, 480 - vfY - vfH, TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);         // 12x16 per char, so 18 chars across the panel
    const int lh = 18;

    uint32_t avg = photoCount ? photoBytes / photoCount : 300000;
    uint64_t remain = freeBytes() / (avg ? avg : 300000);
    if (remain > 9999) remain = 9999;          // keep the field readable
    tft.setCursor(4, 6);
    tft.printf("%u/%u pics", photoCount, (unsigned)(photoCount + remain));

    // No percentage on USB: it would be the charger's voltage, not the cell's.
    char b[8];
    if (battChg) strcpy(b, "USB");
    else         snprintf(b, sizeof(b), "%u%%", battPct);
    tft.setTextColor(battChg ? TFT_CYAN :
                     battPct > 40 ? TFT_GREEN : battPct > 15 ? TFT_YELLOW : TFT_RED,
                     TFT_BLACK);
    tft.setCursor(VF_W - 4 - (int)strlen(b) * 12, 6);
    tft.print(b);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(4, 6 + lh);
    tft.print(status);

    int y = vfY + vfH + 6;
    if (wifiOn) {
        tft.setCursor(4, y);            tft.print(AP_SSID);
        tft.setCursor(4, y + lh);       tft.print(AP_PASS);
        tft.setCursor(4, y + lh * 2);   tft.print("192.168.4.1");
    } else {
        tft.setCursor(4, y);
        if (!afReady) {
            tft.setTextColor(TFT_RED, TFT_BLACK);
            tft.print("fixed focus");
        } else {
            // 0x10 focus achieved, 0x20 constant AF tracking, else hunting.
            // Only 0x10 is evidence of convergence; 0x20 just means constant
            // AF is running, which is true even pressed against a surface.
            bool ok = inFocus();
            tft.setTextColor(ok ? TFT_GREEN : TFT_RED, TFT_BLACK);
            tft.print(ok ? "focus lock" : "focusing");
        }
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
    }

    // Labels sit over the physical buttons: btn1 bottom-left, btn3 bottom-right.
    const char *left  = wifiOn ? "BACK" : "WIFI";
    const char *right = wifiOn ? "BACK" : "SHOOT";
    tft.setCursor(4, 480 - 20);
    tft.print(left);
    tft.setCursor(222 - 4 - (int)strlen(right) * 12, 480 - 20);
    tft.print(right);
}

// ------------------------------------------------------------------ touch ---

static void initTouch()
{
    touch.setPins(BOARD_TOUCH_RST, BOARD_SENSOR_IRQ);
    touchReady = touch.begin(Wire, CST226SE_SLAVE_ADDRESS,
                             BOARD_I2C_SDA, BOARD_I2C_SCL);
    if (!touchReady) return;
    touch.setMaxCoordinates(BOARD_TFT_WIDTH, BOARD_TFT_HEIHT);
    touch.setSwapXY(false);
    touch.setMirrorXY(false, false);
}

// Returns true on a fresh press, with x/y already in the same coordinates the
// display uses (DISPLAY_ROTATION 2 means the panel is read upside down).
static bool touchTap(int &x, int &y)
{
    static uint32_t last = 0;
    static bool down = false;
    int16_t px[5], py[5];

    if (!touchReady) return false;
    uint8_t n = touch.getPoint(px, py, 1);
    if (!n) { down = false; return false; }
    if (down || millis() - last < 400) return false;
    down = true;
    last = millis();

#if DISPLAY_ROTATION == 2
    x = BOARD_TFT_WIDTH  - 1 - px[0];
    y = BOARD_TFT_HEIHT  - 1 - py[0];
#else
    x = px[0];
    y = py[0];
#endif
    Serial.printf("tap raw=%d,%d -> %d,%d\n", px[0], py[0], x, y);
    return true;
}

static void setTorch(bool on)
{
    torchOn = on;
    // The LED is on the 5V rail. Running from battery there is no VBUS, so the
    // PMU has to boost for it. Only when needed: OTG while USB is attached
    // stops the battery charging.
    if (on && !PMU.isVbusIn()) PMU.enableOTG();
    else if (!on)              PMU.disableOTG();
    ledcWrite(LEDC_WHITE_CH, on ? 20 : 0);
}

// ------------------------------------------------------------------ setup ---

void setup()
{
    Serial.begin(115200);
    // With ARDUINO_USB_CDC_ON_BOOT and no host attached (running on battery),
    // every write waits out the default 100ms TX timeout. Shorten it, but not
    // to zero: at zero, writes are dropped outright when the buffer is busy.
    Serial.setTxTimeoutMs(10);

    // PMU first: if the card slot is fed from a PMU-controlled rail, probing
    // storage before the PMU is up would find an unpowered card.
    if (PMU.init(Wire, BOARD_I2C_SDA, BOARD_I2C_SCL, SY6970_SLAVE_ADDRESS)) {
        PMU.setChargeTargetVoltage(3856);
        PMU.setPrechargeCurr(64);
        // The board sets its input limit by resistor: ILIM = 360R/680R =
        // 0.53A. Charging at the stock 320mA leaves under 220mA for everything
        // else, and a full-res capture with sustained flash writes is the peak
        // draw. 128mA keeps the margin (and matches the README's <200mA note);
        // it just charges more slowly.
        PMU.setChargerConstantCurr(128);
        PMU.enableADCMeasure();
        PMU.disableStatLed();

        // The board's own PMU example warns that with no battery attached the
        // charger limits how much current the chip will pass. A full-res
        // capture is the peak draw - sensor readout plus sustained flash
        // program/erase - so a limited rail browns out exactly there.
        // isBatteryConnect() is unimplemented in the library, so infer it from
        // the measured voltage and only disable charging when there is nothing
        // to charge.
        delay(120);                       // let the ADC produce a reading
        battMv = PMU.getBattVoltage();
        if (battMv < 3000) PMU.disableCharge();
    }


    delay(50);
    initStorage();

    tft.begin();
    tft.setRotation(DISPLAY_ROTATION);   // 222x480 portrait
    tft.fillScreen(TFT_BLACK);

    initTouch();

    vf      = (uint16_t *)ps_malloc(VF_W * VF_H_MAX * sizeof(uint16_t));
    decoded = (uint16_t *)ps_malloc(SRC_W * SRC_H * sizeof(uint16_t));

    TJpgDec.setJpgScale(1);
    TJpgDec.setSwapBytes(true);
    TJpgDec.setCallback(jpegBlock);

    if (!startCamera()) {
        tft.setCursor(4, 100);
        tft.setTextColor(TFT_RED);
        tft.print("camera init failed");
    }

    ledcSetup(LEDC_WHITE_CH, 1000, 8);
    ledcAttachPin(CAMERA_WHITH_LED, LEDC_WHITE_CH);
    ledcWrite(LEDC_WHITE_CH, 0);

    // btn2 (GPIO12) is not populated on this board.
    pinMode(0, INPUT_PULLUP);
    pinMode(16, INPUT_PULLUP);

    prefs.begin("cam", false);
    scanPhotos();
    updateViewfinderRect();

    snprintf(status, sizeof(status), "ready");

    Serial.printf("storage=%s free=%llu autofocus=%d\n", storeName,
                  (unsigned long long)freeBytes(), afReady);
}

// ponytail: 250ms lockout is the whole debounce story for three buttons.
static bool pressed(uint8_t pin)
{
    static uint32_t last[40] = {0};
    if (digitalRead(pin) == LOW && millis() - last[pin] > 250) {
        last[pin] = millis();
        return true;
    }
    return false;
}

void loop()
{
    // ponytail: bring-up only - send '0'-'7' to try an orientation live.
    while (Serial.available()) {
        int c = Serial.read();
        if (c >= '0' && c <= '7') {
            orient = c - '0';
            updateViewfinderRect();
            snprintf(status, sizeof(status), "or%u r%d m%d e%d",
                     orient, ORI_ROT * 90, ORI_MIRROR, EXIF_ORIENT);
            Serial.println(status);
        } else if (c == 'c' && !wifiOn) {
            capture();
            Serial.println(status);
        } else if (c == 'w') {
            toggleWifi();
            Serial.printf("%s ip=%s\n", status, WiFi.softAPIP().toString().c_str());
        } else if (c == 'p') {
            // Same wire format as 'd', but the live preview frame - lets the
            // preview and the still be compared on one scene.
            camera_fb_t *fb = esp_camera_fb_get();
            if (!fb) { Serial.println("BEGIN 0\nEND"); continue; }
            Serial.printf("BEGIN %u\n", (unsigned)fb->len);   // already JPEG
            Serial.write(fb->buf, fb->len);
            esp_camera_fb_return(fb);
            Serial.println("\nEND");
        } else if (c == 'd') {
            // Raw dump of the newest JPEG: "BEGIN <size>\n", bytes, "\nEND\n".
            File dir = store->open("/");
            String last;
            for (File f = dir.openNextFile(); f; f = dir.openNextFile()) {
                String n = f.name();
                if (n.endsWith(".JPG") && n > last) last = n;
                f.close();
            }
            if (!last.startsWith("/")) last = "/" + last;
            File f = store->open(last);
            if (!f) { Serial.println("BEGIN 0\nEND"); continue; }
            Serial.printf("BEGIN %u\n", (unsigned)f.size());
            uint8_t chunk[512];
            while (f.available()) Serial.write(chunk, f.read(chunk, sizeof(chunk)));
            f.close();
            Serial.println("\nEND");
        } else if (c == 'i') {
            Serial.printf("storage=%s free=%llu autofocus=%d touch=%d wifi=%d torch=%d "
                          "reset=%d heap=%u psram=%u batt=%umV(%s) chg=%d ichg=%umA ilim=%umA "
                          "sd[%s] sharp=%u peak=%u foc=%d\n",
                          storeName, (unsigned long long)freeBytes(), afReady, touchReady,
                          wifiOn, torchOn, (int)esp_reset_reason(),
                          (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getFreePsram(),
                          (unsigned)battMv, battPctStr(), PMU.isCharging(),
                          (unsigned)PMU.getChargerConstantCurr(),
                          (unsigned)PMU.getInputCurrentLimit(), sdInfo,
                          (unsigned)sharpness, (unsigned)sharpPeak, inFocus());
        } else if (c == 'l') {
            File dir = store->open("/");
            for (File f = dir.openNextFile(); f; f = dir.openNextFile()) {
                Serial.printf("  %s %u\n", f.name(), (unsigned)f.size());
                f.close();
            }
            Serial.printf("free=%u\n", (unsigned)freeBytes());
        }
    }

    // Shutter is btn3, not btn1: btn1 is GPIO0, the BOOT pin, so any reset
    // while it is held brings the board up in ROM download mode - a blank
    // screen that only a power cycle clears. Keeping the most-pressed button
    // off GPIO0 makes a reset merely a reset. It also puts the shutter on the
    // right, which is where a right hand wants it.
    bool p1 = pressed(0), p3 = pressed(16);
    if (wifiOn) {
        if (p1 || p3) toggleWifi();     // both labels read BACK
    } else {
        if (p3) capture();
        if (p1) toggleWifi();
    }

    int tx, ty;
    if (touchTap(tx, ty) && !wifiOn) {
        if (ty >= vfY && ty < vfY + vfH) capture();          // viewfinder
        else if (ty < vfY)               setTorch(!torchOn); // strip above it
    }

    if (wifiOn) { dns.processNextRequest(); server.handleClient(); }
    else      { drawViewfinder(); measureSharpness(); }

    pollBattery();
    drawStatus();
}
