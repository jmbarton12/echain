# eChain

A 4-color e-paper keychain that shuffles through photos and quotes, built on the
Seeed Studio XIAO ePaper Display Board EE05 (ESP32-S3) and a 2.9" 128×296
black/white/red/yellow panel.

Content is managed from a PC: drop photos and quotes into `content/<device>/`,
press **A+B** on the keychain to enter sync mode, then run `echain sync <device>`
over home Wi-Fi. Firmware updates go over the air the same way.

> Status: requirements stage. See [docs/REQUIREMENTS.md](docs/REQUIREMENTS.md).

## Layout

| Path | Contents |
|---|---|
| `firmware/` | C++17 / Arduino-ESP32 firmware (PlatformIO) |
| `tools/` | `echain` Python CLI: image conversion, quote rendering, sync, OTA |
| `content/` | Per-device photos and quotes (gitignored) |
| `hardware/` | Case notes, BOM, wiring |
| `docs/` | Requirements and guides |

## Credits

Inspired by [eChain: A Fun Digital Key Chain](https://www.hackster.io/Ameya_Angadi/echain-a-fun-digital-key-chain-ce8f8d)
by Ameya Angadi. This repository is an independent, clean-room implementation;
no code from the original project is used. The 3D-printed case design is the
original author's work and is not redistributed here. Get it from the original
project page.

## License

[MIT](LICENSE)
