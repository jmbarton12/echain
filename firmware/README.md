# eChain firmware

C++17 / Arduino-ESP32 (core 3.3.x) on PlatformIO, for the XIAO ePaper Display
Board EE05 with the 2.9" BWRY panel. Pin map: [docs/REQUIREMENTS.md §12](../docs/REQUIREMENTS.md#12-hardware-reference-ee05--29-bwry).

## Build & flash

```bash
pio run
```

```bash
pio run -t upload
```

```bash
pio device monitor
```

The first build downloads Seeed_GFX2 at a pinned commit into `.pio/vendor/`
(see `scripts/fetch_seeed_gfx2.py` for why it isn't a plain `lib_deps`).

For later Wi-Fi features, copy `include/secrets.example.h` to `include/secrets.h`.
The bring-up build doesn't need it.

## Bring-up checklist

The current `src/main.cpp` is bring-up firmware for the "verify on first
hardware" items in REQUIREMENTS §10. Connect the panel and a battery (slide
switch ON), flash, open the monitor, and check:

| Check | Expect in serial log / on screen |
|---|---|
| Flash (item 13) | `[flash] physical size: ... (16 MB)` |
| ADC_EN (item 10) | `[battery] ... low: ~0 mV, high: ~3700-4200 mV` → "looks correct" |
| Panel colors (item 11) | Four swatches labelled BLACK / WHITE / RED / YELLOW match their labels; red border |
| Refresh time (item 12) | `[display] full refresh took ... ms` (expect ~16-20 s) |
| Keys | Pressing Key2 logs `Key2 = A`, Key3 logs `Key3 = B` |

Record the results in REQUIREMENTS §10. Deep-sleep current (also item 12) needs a
meter in series with the battery and comes with the sleep firmware.
