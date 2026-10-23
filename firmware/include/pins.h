// EE05 pin map, raw ESP32-S3 GPIO numbers (docs/REQUIREMENTS.md §12).
// Raw numbers on purpose: Dn aliases differ between XIAO ESP32-S3 core variants.
#pragma once

// User keys: 10k external pull-up, active LOW, RTC-capable.
constexpr int PIN_KEY1 = 2;  // enclosed by the case; unused
constexpr int PIN_KEY_A = 3; // Key2: next
constexpr int PIN_KEY_B = 8; // Key3: previous

// Battery sense: VBAT / 2 on BAT_ADC, powered only while ADC_EN is high.
constexpr int PIN_BAT_ADC = 1;
constexpr int PIN_ADC_EN = 39; // D12; unverified (§10 item 10)

// E-paper (driven by Seeed_GFX2's Board_XIAO_ePaper_EE05; listed for reference).
constexpr int PIN_EPD_BUSY = 4;
constexpr int PIN_EPD_PWR_EN = 43;
constexpr int PIN_EPD_CS = 44;
constexpr int PIN_EPD_SCK = 7;
constexpr int PIN_EPD_MOSI = 9;
constexpr int PIN_EPD_RST = 38;
constexpr int PIN_EPD_DC = 10;
