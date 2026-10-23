// eChain bring-up firmware.
//
// Exercises the hardware checks in docs/REQUIREMENTS.md §10 ("verify on first
// hardware") and reports results on USB serial (115200):
//   - item 13: physical flash size
//   - item 10: ADC_EN pin (battery reading should jump when it is driven high)
//   - item 11: panel colors via Seeed_GFX2's JD79667 driver
//   - item 12: full-refresh duration
// Afterwards it logs key presses so the A/B mapping can be checked by hand.

#include <Arduino.h>
#include <esp_flash.h>

#include <Seeed_GFX.h>
#include "board/boards/XIAO_ePaper_Boards.h"
#include "driver/epaper/Driver_JD79667.h"
#include "panel/Panel_EPaper.h"
#include "panel/configs/Seeed_Panel_Configs.h"

#include "pins.h"

#ifndef ECHAIN_VERSION
#define ECHAIN_VERSION "unknown"
#endif

static Seeed_GFX display;

static uint32_t flash_size_bytes() {
    uint32_t size = 0;
    if (esp_flash_get_physical_size(esp_flash_default_chip, &size) != ESP_OK) return 0;
    return size;
}

// Battery voltage in mV (VBAT, i.e. BAT_ADC x 2), sampled with ADC_EN at `en_level`.
static uint32_t read_battery_mv(int en_level) {
    pinMode(PIN_ADC_EN, OUTPUT);
    digitalWrite(PIN_ADC_EN, en_level);
    delay(5); // let the load switch and divider settle

    uint32_t sum = 0;
    const int samples = 16;
    for (int i = 0; i < samples; i++) sum += analogReadMilliVolts(PIN_BAT_ADC);

    digitalWrite(PIN_ADC_EN, LOW);
    return (sum / samples) * 2;
}

static void draw_test_screen(uint32_t flash_mb, uint32_t bat_mv) {
    display.setRotation(3); // 296 x 128 landscape
    display.fillScreen(TFT_WHITE);

    const int16_t w = display.width();
    const int16_t h = display.height();

    display.setTextColor(TFT_BLACK);
    display.setTextSize(2);
    display.drawString("eChain bring-up", 6, 4);

    display.setTextSize(1);
    char line[64];
    snprintf(line, sizeof(line), "fw %s  flash %lu MB  bat %lu mV", ECHAIN_VERSION,
             (unsigned long)flash_mb, (unsigned long)bat_mv);
    display.drawString(line, 6, 24);

    // One swatch per panel color; labels let you confirm each maps correctly.
    const uint16_t colors[4] = {TFT_BLACK, TFT_WHITE, TFT_RED, TFT_YELLOW};
    const char *names[4] = {"BLACK", "WHITE", "RED", "YELLOW"};
    const int16_t sw = 64, sh = 64, gap = 6;
    const int16_t x0 = (w - (sw * 4 + gap * 3)) / 2;
    const int16_t y0 = 40;
    for (int i = 0; i < 4; i++) {
        const int16_t x = x0 + i * (sw + gap);
        display.fillRect(x, y0, sw, sh, colors[i]);
        display.drawRect(x, y0, sw, sh, TFT_BLACK);
        display.drawCentreString(names[i], x + sw / 2, y0 + sh + 4, 1);
    }

    display.drawRect(0, 0, w, h, TFT_RED);
}

void setup() {
    Serial.begin(115200);
    const uint32_t start = millis();
    while (!Serial && millis() - start < 3000) delay(10);

    Serial.printf("\n== eChain bring-up %s ==\n", ECHAIN_VERSION);

    const uint32_t flash = flash_size_bytes();
    Serial.printf("[flash] physical size: %lu bytes (%lu MB); expected 16 MB\n",
                  (unsigned long)flash, (unsigned long)(flash >> 20));
    Serial.printf("[psram] %lu bytes\n", (unsigned long)ESP.getPsramSize());

    const uint32_t bat_off = read_battery_mv(LOW);
    const uint32_t bat_on = read_battery_mv(HIGH);
    Serial.printf("[battery] ADC_EN=GPIO%d low: %lu mV, high: %lu mV\n", PIN_ADC_EN,
                  (unsigned long)bat_off, (unsigned long)bat_on);
    Serial.println(bat_on > bat_off + 1000
                       ? "[battery] ADC_EN pin looks correct"
                       : "[battery] no jump: ADC_EN pin wrong, or no battery connected");

    pinMode(PIN_KEY1, INPUT);
    pinMode(PIN_KEY_A, INPUT);
    pinMode(PIN_KEY_B, INPUT);

    if (!display.begin<Board_XIAO_ePaper_EE05, Config_Seeed_ePaper_2inch9_BWRY_JD79667>()) {
        Serial.printf("[display] begin failed: %s\n", display.lastResult().message);
        return;
    }

    draw_test_screen(flash >> 20, bat_on);
    const uint32_t t0 = millis();
    display.update();
    Serial.printf("[display] full refresh took %lu ms\n", (unsigned long)(millis() - t0));
    Serial.println("[keys] press keys; Key2 should be A (next), Key3 B (previous)");
}

void loop() {
    static int last[3] = {HIGH, HIGH, HIGH};
    const int pins[3] = {PIN_KEY1, PIN_KEY_A, PIN_KEY_B};
    const char *names[3] = {"Key1 (unused)", "Key2 = A", "Key3 = B"};

    for (int i = 0; i < 3; i++) {
        const int now = digitalRead(pins[i]);
        if (now != last[i]) {
            if (now == LOW) Serial.printf("[keys] %s pressed (GPIO%d)\n", names[i], pins[i]);
            last[i] = now;
        }
    }
    delay(20);
}
