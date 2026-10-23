// Copy to secrets.h (gitignored) and fill in. See NW-1 in docs/REQUIREMENTS.md.
// Changing these requires reflashing every keychain over USB.
#pragma once

#define ECHAIN_WIFI_SSID     "your-home-ssid"
#define ECHAIN_WIFI_PASSWORD "your-wifi-password"

// Optional: password for ArduinoOTA / espota firmware uploads (FW-1).
// Leave empty to disable OTA authentication on the trusted home network.
#define ECHAIN_OTA_PASSWORD  ""
