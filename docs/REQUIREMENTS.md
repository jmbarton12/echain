# eChain — Requirements

Status: **Draft v0.2** (2026-10-23): hardware open questions researched; see §10 and §12.

A small 4-color e-paper keychain that cycles through photos and short text
quotes. Three units will be built as gifts for children. The design is loosely
inspired by Ameya Angadi's
[eChain on Hackster](https://www.hackster.io/Ameya_Angadi/echain-a-fun-digital-key-chain-ce8f8d),
but this repo is a **clean-room rewrite**: no code is copied from the original
(which is CC BY-NC 4.0). The only external design reused is the original 3D-printed case.

Priority keys: **M** = must (v1), **S** = should (v1 if cheap), **C** = could / later.

---

## 1. Hardware

| ID | Requirement | Pri |
|----|-------------|-----|
| HW-1 | Controller: Seeed Studio XIAO ePaper Display Board **EE05** (XIAO ESP32-S3 Plus, 24-pin FPC). | M |
| HW-2 | Display: Seeed 2.9" quadruple-color e-paper (SKU 104990855), **128×296**, black / white / red / yellow, SPI. | M |
| HW-3 | Power: single-cell 3.7 V LiPo (~1000 mAh, **JST 2.0 mm (PH)** plug, polarity verified) via the EE05 battery connector and slide switch; charge via USB-C. | M |
| HW-4 | Input: the EE05 edge has four side-mounted buttons and a slide switch. Three buttons are user keys (Key1/Key2/Key3, active-low, readable by firmware). The fourth is **Reset**, wired to EN and not readable by firmware. The slide switch is battery on/off. **In the case, only Key2 and Key3 (and the slide switch) are reachable.** Key1, Reset and the XIAO BOOT button are enclosed, so firmware uses **A** = Key2 and **B** = Key3 and ignores Key1. | M |
| HW-5 | Enclosure: the original eChain 3D-printed case (Fusion design from the guide), with keyring loop. | M |
| HW-6 | No buzzer. | M |
| HW-7 | Battery voltage is sensed on BAT_ADC through a 10k/10k (÷2) divider behind a load switch gated by ADC_EN. ADC_EN is driven high only for the duration of a reading (§12). | M |
| HW-9 | Display power is switched off (PWR_EN low) before deep sleep. | M |
| HW-8 | Wi-Fi via the FPC antenna attached to the ESP32-S3 module. | M |

## 2. Content & display

| ID | Requirement | Pri |
|----|-------------|-----|
| CT-1 | Device shows a **playlist** of items. Each item is either an **image** or a **text quote** at the source, but **every item reaches the device as a pre-rendered frame**: the firmware only ever displays frames and has no fonts or text layout. | M |
| CT-2 | Images are stored pre-dithered in the controller's native format: 2 bpp, 4 pixels per byte MSB-first, row-major in the controller's portrait 128×296 RAM, 9,472 bytes per frame. Codes are `00` black, `01` white, `10` yellow, `11` red (§12). Landscape images are rotated into portrait by the CLI. No image decoding on the device. | M |
| CT-3 | Each image is either **portrait (128×296)** or **landscape (296×128)**; orientation is stored per item and chosen automatically by the CLI from the source aspect ratio. | M |
| CT-4 | Each item may be flipped 180° (per-item flag) so it reads correctly however the keychain hangs. | S |
| CT-5 | Text quotes come from a **user-entered list** per device. A quote has text, an optional attribution line and an optional accent color (black/red/yellow). | M |
| CT-6 | Text quotes are **auto-fit by the CLI** (CLI-4): the largest font size that fits, word-wrapped, centered, attribution in smaller type, then rendered to a frame. | M |
| CT-7 | Content is stored on a flash filesystem partition, separate from firmware. Adding or removing content **never** needs a firmware reflash. | M |
| CT-8 | Device holds at least 100 items (≈1 MB). | M |
| CT-9 | Display state persists across sleep/reset: device resumes at the last shown item, with its shuffle order and history. | M |
| CT-11 | **Shuffle playback**: items play in random order with no repeats until every item has been shown, then reshuffle. A new cycle never starts with the item that just ended the last one. The shuffle order is regenerated whenever a sync changes the playlist. | M |
| CT-10 | Empty playlist shows a built-in "No pictures yet" screen with the device name. | S |

## 3. Interaction

| ID | Requirement | Pri |
|----|-------------|-----|
| UX-1 | **A** = next item in the shuffle order. | M |
| UX-2 | **B** = previous item: goes back through the shuffle history, not the folder order. | M |
| UX-3 | **A + B together** = enter **sync mode** (see §4). This is the **only** way into sync mode; there is no automatic sync on charge (§11). | M |
| UX-4 | Items rotate automatically on a timer. Default **1 hour**, configurable per device, **minimum 5 minutes**. | M |
| UX-5 | A button press resets the auto-rotate timer. | S |
| UX-6 | Presses during a refresh are ignored, or queued as at most one pending action. A full 4-color refresh is expected to take about 16–20 s (§10). | M |
| UX-7 | **Low-battery icon** drawn in a corner of the current item when the battery is below ~15%. | M |
| UX-8 | Below a critical threshold (~5%), show a "Please charge me" screen and stop timer wakes. Buttons may still wake it. | S |

## 4. Connectivity & sync mode

| ID | Requirement | Pri |
|----|-------------|-----|
| NW-1 | **Station mode only**: device joins the home Wi-Fi. Credentials are compiled in from a gitignored `secrets.h`; a committed `secrets.example.h` documents the format. If the Wi-Fi password changes, each unit is reflashed over USB (accepted). | M |
| NW-2 | Wi-Fi is **off** except in sync mode. | M |
| NW-3 | In sync mode the device advertises itself over mDNS as `echain-<name>.local` (e.g. `echain-alice.local`). | M |
| NW-4 | Sync-mode screen shows: device name, hostname, IP address, battery %, firmware version. | M |
| NW-5 | Sync mode ends after **5 minutes with no requests**, or when A+B is pressed again. The device then redraws the current item and sleeps. | M |
| NW-6 | If Wi-Fi fails to connect (~20 s timeout), show a short error screen, then return to normal operation. | M |
| NW-7 | No authentication on the LAN API (trusted home network). | M |
| NW-8 | Fallback access point to re-enter Wi-Fi credentials without USB. | C |
| NW-9 | Device-hosted web page (status page or phone-based editing). | C (v2) |

## 5. Firmware updates

| ID | Requirement | Pri |
|----|-------------|-----|
| FW-1 | Firmware is updatable **over the air** while in sync mode, from PlatformIO (`espota` / ArduinoOTA-compatible) or the CLI. | M |
| FW-2 | Flash layout uses dual OTA app slots plus a filesystem partition. Content survives firmware updates. | M |
| FW-3 | A failed or interrupted OTA must not brick the device: it rolls back to the previous image. | M |
| FW-4 | USB flashing remains available for first flash and recovery (see FW-7). | M |
| FW-5 | Firmware version (git describe) is embedded at build time and reported by the API and sync screen. | M |
| FW-6 | **No reachable Reset button**, so the firmware must recover by itself. A watchdog covers every awake phase (refresh, Wi-Fi, OTA). Every code path ends in deep sleep. A BUSY timeout aborts the refresh and sleeps, rather than hanging. The slide switch (on battery, USB unplugged) is the only manual power-cycle. | M |
| FW-7 | USB flashing must work without the BOOT button. Keep the native USB-Serial/JTAG enabled so esptool can auto-reset into the bootloader. Opening the case is the last-resort recovery. | M |

## 6. Device API (LAN, sync mode only)

A small HTTP/JSON API. Both the CLI and any future web page talk to the device only through it.

| ID | Requirement | Pri |
|----|-------------|-----|
| API-1 | `GET /status`: name, firmware version, battery % and mV, free and used storage, item count, settings. | M |
| API-2 | `GET /items`: the playlist (id, source type, orientation, flip, content hash). | M |
| API-3 | Add item: upload a raw 9,472-byte frame with metadata. Quotes arrive as frames too. | M |
| API-4 | Delete item; set playlist order; update item flags. | M |
| API-5 | Get or set settings (rotate interval). | M |
| API-6 | Writes are atomic: a dropped connection never leaves a corrupt playlist. | M |
| API-7 | `GET /items/<id>/frame`: download a frame so the CLI can verify it or show previews. | C |

## 7. Host tooling (CLI)

A Python CLI, `echain`, run from this repo on the developer's PC.

| ID | Requirement | Pri |
|----|-------------|-----|
| CLI-1 | **Folder sync is the source of truth**: `echain sync <device>` mirrors `content/<device>/` to the device, adding new items, removing missing ones and applying order. Unchanged items are skipped by content hash. | M |
| CLI-2 | The folder's contents define the set of items. Order doesn't matter for playback (CT-11). An optional `playlist.yaml` can carry per-item overrides (flip, crop focus, dither settings). | M |
| CLI-3 | Image conversion on the PC: accepts JPG/PNG/HEIC, auto-orients (EXIF plus aspect → portrait/landscape), crops to fill, and dithers to the 4-color palette (Floyd–Steinberg or Atkinson, with tunable gamma and saturation). | M |
| CLI-4 | Quotes are defined as text files or entries in `quotes.yaml`, with optional attribution, accent color and orientation. The CLI renders each quote to a frame with a bundled, permissively licensed TTF font (CT-6). Each quote's frame hash includes the font and layout version, so changing them re-syncs the affected quotes. | M |
| CLI-5 | `echain preview <device>` renders all items to PNGs or a contact sheet locally, without a device, so dithering can be checked before syncing. | S |
| CLI-6 | `echain status <device>` and `echain ota <device>` (firmware upload). | S |
| CLI-7 | `--dry-run` on sync shows planned adds and removes. | S |
| CLI-8 | Device registry `devices.yaml` (committed): each child's first name as the device name (→ `echain-<name>.local`), timer setting, flip default. Kids' first names are public in the repo (accepted). | M |

## 8. Power

| ID | Requirement | Pri |
|----|-------------|-----|
| PW-1 | Between refreshes the device is in deep sleep with the display controller powered down. Target **≤ 30 µA** sleep current, to be measured. | M |
| PW-2 | Wake sources: RTC timer (auto-rotate) and A/B via EXT1 `ANY_LOW` on GPIO3 and GPIO8. Both are RTC-capable with external 10k pull-ups. Key1 (GPIO2) is not a wake source. | M |
| PW-3 | Battery-life target with a 1000 mAh cell, a 1-hour timer and a few button presses a day: **≥ 4 weeks**. Validate by measurement. | S |
| PW-4 | Battery % mapping is calibrated per device (scale factor stored in NVS or `devices.yaml`). | S |

## 9. Repository & project

| ID | Requirement | Pri |
|----|-------------|-----|
| RP-1 | Public GitHub repo, **MIT license**, with a clean-room note crediting the original eChain as inspiration and the case design as the original author's. | M |
| RP-2 | Firmware is **C++17 on the Arduino-ESP32 framework**, built with **PlatformIO**, library versions pinned. Code is plain and C-like (functions and structs, no class hierarchies). ESP-IDF APIs are called directly where Arduino lacks them (deep sleep, OTA rollback, watchdog). MicroPython was rejected: interpreter boot on every wake costs battery, and there's no proven driver for this panel. | M |
| RP-7 | Host CLI is **Python 3.12+** (Pillow for decoding and dithering, `pillow-heif` for HEIC). All image decoding happens on the PC; the device never decodes JPG or PNG. | M |
| RP-3 | Layout (proposed): `firmware/`, `tools/` (CLI), `content/` (gitignored, one folder per child), `hardware/` (case notes and links, BOM), `docs/`. | M |
| RP-4 | `content/` and `secrets.h` are gitignored. Kids' photos are never committed. | M |
| RP-5 | CI: build firmware and run CLI unit tests (dithering and packing) on push. | S |
| RP-6 | Docs: build guide (assembly, first flash), and a "how to update content" guide. | S |

---

## 10. Open questions & technical risks

### Resolved by research (2026-10-23)

1. **Panel driver: resolved, one discrepancy left.** Seeed_GFX2 (MIT, v1.0.0, `github.com/Seeed-Studio/Seeed_GFX2`) has a ready-made EE05 board definition and a 2.9" BWRY panel config (`Board_XIAO_ePaper_EE05` + `Config_Seeed_ePaper_2inch9_BWRY_JD79667`), with an example for exactly this panel. **Decision: use Seeed_GFX2 for bring-up.** Quotes are now rendered on the PC, so the firmware only pushes pre-packed frames and needs no graphics or fonts. The driver surface is just init, write frame (cmd `0x10`), refresh (`0x12`) and sleep (`0x02`, `0x07 A5`). That makes a **minimal driver of about 200 lines the likely production choice**, with Seeed_GFX2's init sequence as the reference. GxEPD2 is not needed.
   - *Discrepancy:* the panel datasheet names the controller **JD79661**, but Seeed_GFX2 drives it with a class named **JD79667**. Both are JD796xx 4-color parts with the same command set and color codes. Trust Seeed's verified example, and confirm with the HelloWorld sketch on real hardware.
2. **EE05 pin map: resolved** from the EE05 V1.0 schematic and Seeed_GFX2. See §12. All three keys are on RTC-capable GPIOs (2, 3, 8), so deep-sleep wake on button press works. Remaining item: ADC_EN is on XIAO pad **D12**. Its GPIO is **39** per Seeed's Chinese Plus pin table, but English pages list D12 as GPIO41 for the non-Plus board. Confirm on hardware.
3. **Refresh time: partly resolved.** No figure is published for this exact panel. Sibling JD79661 4-color panels from GoodDisplay list a **16 s full refresh**. A reseller lists 20 s for a JD79667 2.9" part. There is **no partial refresh** in 4-color mode. Plan for **about 16–20 s** per change, and still measure it, along with refresh current (datasheet: 9.9 mW typical while refreshing, 0.003 mW standby).
4. **Flash size: resolved (16 MB flash, 8 MB OPI PSRAM, ESP32-S3R8)** per Plus retail listings. Verify on the first board with `esptool.py flash_id`. PSRAM must be enabled in the build (Seeed's wiki calls it required for Seeed_GFX2).

### Decided in review

5. **Where quotes are rendered: on the PC.** The CLI renders them (CT-1, CT-6, CLI-4), and the firmware is frames-only.
6. **Wi-Fi password changes: USB reflash is acceptable.** NW-8 stays a "could".

7. **Buttons:** the case exposes only Key2 and Key3, which become A and B. Sync mode is the A+B chord. Key1 is unused.
8. **Cold weather: ignored.** The panel is rated 0–40 °C, but the firmware does nothing special when cold. If ghosting shows up in winter, revisit.
9. **Sync trigger: A+B only.** The EE05 has no VBUS-sense GPIO, so detecting charging would need a hardware mod (a VBUS divider to a spare RTC GPIO) or an unreliable voltage heuristic. Dropped.

### Still open: verify on first hardware

10. **ADC_EN GPIO:** 39 expected (§12).
11. **Controller:** JD79661 vs JD79667 naming. Run Seeed's HelloWorld unchanged and confirm colors.
12. **Refresh timing and current:** measure (16–20 s expected), plus deep-sleep current against the ≤ 30 µA target.
13. **Flash ID:** confirm 16 MB with `esptool.py flash_id`.

### Notes

- **Panel longevity:** no manufacturer minimum refresh interval was found. Keep UX-4's 5-minute minimum.
- **Charge time:** the Plus charges at about 100 mA, so a 1000 mAh cell takes about 10 h from empty. That's fine overnight. Mention it in the kids' usage notes.

## 11. Out of scope (v1)

- Automatic sync while charging (no VBUS detect without a hardware mod; see §10 item 9).
- On-device text rendering and fonts.
- Buzzer, find-my-keys, BLE.
- Phone app or device-hosted web UI (NW-9).
- Contact / business-card mode.
- Cloud or internet services. The device never needs internet access.

## 12. Hardware reference (EE05 + 2.9" BWRY)

Sources: EE05 V1.0 schematic (KiCad, 2025-12-17, sheet 03 XIAO), Seeed_GFX2 `XIAO_ePaper_Board_Configs.h` and the EE05 2.9" BWRY examples, and the EE05 wiki. "GPIO" is the ESP32-S3 pin number to use in firmware. Use raw GPIO numbers, not `Dn` aliases: Seeed notes that D11/D16 aliases differ between XIAO ESP32-S3 board-package variants.

| Function | Net (schematic) | XIAO pad | GPIO | Notes |
|---|---|---|---|---|
| Battery sense | `BAT_ADC` | D0 / A0 | 1 | ADC1. VBAT ÷ 2 (R22/R23 10k/10k), behind load switch U9. |
| Battery sense enable | `ADC_EN` | D12 | 39 *(verify)* | Active-high EN of TPS22916 U9. 1 MΩ pull-down, so it's off by default. |
| Key1 (unused, enclosed) | `BOTTON1` | D1 | 2 | 10k pull-up to 3V3, active-low, RTC GPIO. |
| Key2 → **A** | `BOTTON2` | D2 | 3 | Same. Also an ESP32-S3 strapping pin (JTAG select); harmless with the pull-up. |
| Key3 → **B** | `BOTTON3` | D9 | 8 | Same. |
| E-paper BUSY | `EDP_BUSY` | D3 | 4 | Input. Library waits for high = ready, 30 s timeout. |
| E-paper power enable | `PWR_EN` | D6 | 43 | Active-high EN of TPS22916 U4 (panel 3V3). 1 MΩ pull-down. Also UART0 TX. Don't use Serial on UART0; use USB-CDC. |
| E-paper CS | `SPI0_CS` | D7 | 44 | Also UART0 RX. |
| E-paper SCK | `SPI0_SCL` | D8 | 7 | |
| E-paper MOSI | `SPI0_MOSI` | D10 | 9 | No MISO. Write-only bus, 10 MHz, mode 0, HSPI host. |
| E-paper RST | `EDP_RES` | D11 | 38 | |
| E-paper DC | `EDP_DC` | D16 | 10 | |
| Expansion I2C | `GPIO05_D4…SDA` / `GPIO06_D5…SCL` | D4 / D5 | 5 / 6 | On 7-pin expansion header J2 together with PWR_EN, EDP_RES, ADC_EN, D13. |
| Reset | `RST` | EN | — | Side-mounted reset key. Enclosed by the case, so not usable (FW-6). |

**Panel / controller**
- Controller RAM is portrait 128 (W) × 296 (H). The glass is physically landscape; Seeed's example uses `setRotation(3)` for 296×128.
- Pixel format: 2 bits per pixel, 4 pixels per byte, first pixel in bits 7–6. Codes: `00` black, `01` white, `10` yellow, `11` red. Frame size is 128 × 296 / 4 = **9,472 bytes**, written with command `0x10`.
- Refresh: command `0x12`, then wait for BUSY. Sleep: `0x02` (power off), wait for BUSY, then `0x07 0xA5` (deep sleep). After that, PWR_EN can be dropped.
- Electrical: 3.3 V. 9.9 mW typical during refresh, 0.003 mW standby. Operating temperature 0–40 °C. Active area 66.9 × 29.1 mm, outline 79 × 36.7 × 1.2 mm.

**MCU module (XIAO ESP32-S3 Plus)**
- ESP32-S3R8: 240 MHz dual-core, 8 MB OPI PSRAM, 16 MB flash, Wi-Fi 2.4 GHz plus BLE 5. Uses an external FPC antenna.
- Deep sleep is about 14 µA for the module, per Seeed. Board-level target is ≤ 30 µA (PW-1).
- On-module LiPo charger: about 100 mA charge current.

**Power path**
- The battery goes through the slide switch SW6 and P-MOSFET Q3 (Q4 gates it from VBUS) into the XIAO VBAT pad. The XIAO handles charging from USB-C.
