#define LV_CONF_INCLUDE_SIMPLE // Use the lv_conf.h included in this project

#include <lvgl.h> // Install "lvgl" with the Library Manager
#include "PINS_ESP32-C6-LCD-1_47.h" // Install "GFX Library for Arduino" with the Library Manager
#include "Arduino.h"

#define SCREEN_BRIGHTNESS 255 // Screen brightness
#define UPDATE_INTERVAL 1000  // Update UI every second

// LVGL Display global variabel
lv_display_t *disp;

// UI-object
static lv_obj_t *tempBox;
static lv_obj_t *tempTitle;
static lv_obj_t *tempLbl;
static lv_obj_t *lblFooter;

// Update UI with the new value
void updateUI()
{
    float currentTemp = temperatureRead();
    Serial.printf("[TEMP] CPU Temperature: %.2f °C\n", currentTemp);

    // Update text in the middle of the display
    lv_label_set_text_fmt(tempLbl, "%.1f °C", currentTemp);

    // Update text in the footer of the display
    lv_label_set_text_fmt(lblFooter,
                          "RAM %u KB",
                          heap_caps_get_free_size(MALLOC_CAP_8BIT) / 1024);

    if (currentTemp >= 45.0) {
        // If hotter than 45°C or more -> red color
        lv_obj_set_style_bg_color(tempBox, lv_color_mix(lv_palette_main(LV_PALETTE_RED), lv_color_black(), 160), 0);
    }
        // If chip is in working temperature (between 35°C and 45°C) -> green/yellow color
    else if (currentTemp >= 35.0 && currentTemp < 45.0) {
        lv_obj_set_style_bg_color(tempBox, lv_color_mix(lv_palette_main(LV_PALETTE_AMBER), lv_color_black(), 180), 0);
    }
    else {
        // If the chip is cool (under 35°C) -> blue/grey color
        lv_obj_set_style_bg_color(tempBox, lv_color_mix(lv_palette_main(LV_PALETTE_BLUE), lv_color_black(), 180), 0);
    }
}

// Build UI using only font 16
static void buildUI()
{
    lv_obj_clean(lv_scr_act());

    // Create a nice-looking container/card in the middle for temperature
    tempBox = lv_obj_create(lv_scr_act());
    lv_obj_set_size(tempBox, LV_PCT(90), 100);
    lv_obj_align(tempBox, LV_ALIGN_CENTER, 0, -10);
    lv_obj_set_style_radius(tempBox, 12, 0);
    lv_obj_set_style_pad_all(tempBox, 12, 0);
    
    // Mix discret dark blue/gray background
    lv_obj_set_style_bg_color(tempBox, lv_color_mix(lv_palette_main(LV_PALETTE_GREEN), lv_color_black(), 180), 0);

    // Title in card
    tempTitle = lv_label_create(tempBox);
    lv_obj_set_style_text_font(tempTitle, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(tempTitle, lv_palette_lighten(LV_PALETTE_YELLOW, 2), 0);
    lv_label_set_text(tempTitle, "CPU TEMP");
    lv_obj_align(tempTitle, LV_ALIGN_TOP_MID, 0, 0);

    // Temperature-text
    tempLbl = lv_label_create(tempBox);
    lv_obj_set_style_text_font(tempLbl, &lv_font_montserrat_16, 0); 
    lv_obj_set_style_text_color(tempLbl, lv_color_white(), 0);
    lv_label_set_text(tempLbl, "-- °C");
    lv_obj_align(tempLbl, LV_ALIGN_CENTER, 0, 15);

    // Footer for system-info
    lblFooter = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(lblFooter, &lv_font_montserrat_16, 0);
    lv_obj_align(lblFooter, LV_ALIGN_BOTTOM_MID, 0, -6);
    lv_label_set_text(lblFooter, "RAM -- KB");
    lv_obj_set_style_text_color(lblFooter, lv_palette_main(LV_PALETTE_GREY), 0);
}

// LVGL-function for screen and time sync
void my_print(lv_log_level_t level, const char *buf) {
    LV_UNUSED(level);
    Serial.println(buf);
    Serial.flush();
}

uint32_t millis_cb(void) {
    return millis();
}

void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    uint32_t w = lv_area_get_width(area);
    uint32_t h = lv_area_get_height(area);
    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);
    lv_disp_flush_ready(disp);
}

void setup()
{
    Serial.begin(115200);
    DEV_DEVICE_INIT();
    delay(2000); 

    Serial.println("ESP32-C6 Internal Temp App");

    // Initiate Display using GFX
    if (!gfx->begin()) {
        Serial.println("gfx->begin() failed!");
        while (true) {}
    }
    gfx->setRotation(0);
    gfx->fillScreen(RGB565_BLACK);
    setDisplayBrigthness();

    // Initiate LVGL
    lv_init();
    lv_tick_set_cb(millis_cb);

#if LV_USE_LOG != 0
    lv_log_register_print_cb(my_print);
#endif

    uint32_t screenWidth = gfx->width();
    uint32_t screenHeight = gfx->height();
    uint32_t bufSize = screenWidth * 40;

    lv_color_t *disp_draw_buf = (lv_color_t *)heap_caps_malloc(bufSize * 2, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (!disp_draw_buf) {
        disp_draw_buf = (lv_color_t *)heap_caps_malloc(bufSize * 2, MALLOC_CAP_8BIT);
    }
    if (!disp_draw_buf) {
        Serial.println("LVGL disp_draw_buf allocate failed!");
        while (true) {}
    }

    disp = lv_display_create(screenWidth, screenHeight);
    lv_display_set_flush_cb(disp, my_disp_flush);
    lv_display_set_buffers(disp, disp_draw_buf, NULL, bufSize * 2, LV_DISPLAY_RENDER_MODE_PARTIAL);

    // Build interface
    buildUI();
    updateUI();

    // LVGL-timer running every second to get temperature and update display
    lv_timer_create([](lv_timer_t *) { 
        updateUI(); 
    }, UPDATE_INTERVAL, nullptr);

    Serial.println("Setup done!");
}

void loop()
{
    lv_task_handler(); /* Let LVGL do the rendering */
    delay(5);
}

void setDisplayBrigthness()
{
    ledcAttachChannel(GFX_BL, 1000, 8, 1);
    ledcWrite(GFX_BL, SCREEN_BRIGHTNESS);
}