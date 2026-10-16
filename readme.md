# MiniFace for SiWG917 BRD2605A

MiniFace is a VCOM-native interactive board workspace for the Silicon Labs
SiWG917 BRD2605A development kit. It renders a responsive text-mode desktop in
an ANSI terminal, so a computer can become the board's display, keyboard, and
mouse without an external screen.

The project is designed as a resource-aware SiWG917 interface rather than a
general-purpose desktop. Services start only when an app needs them: Wi-Fi is
created for Network actions and released when Network closes; the microphone
and audio model are started only from an audio app.

## MiniFace Apps

- **Board Console**: live uptime, CPU frequency, VCOM link, and heap telemetry.
- **Network**: scan 2.4 GHz Wi-Fi, join with SSID/password, and display DHCP IP.
- **Audio Intelligence**: optional keyword-model status and inference telemetry.
- **Signal Analyzer**: live PCM waveform, level, peak, baseline, and sample count.
- **LED Control**: direct LED control alongside voice-command operation.
- **System Monitor**: FreeRTOS heap, task, frame, and VCOM throughput diagnostics.
- **Counter**: a small interaction test for mouse, keyboard, widgets, and timers.

## Hardware And Software

- Silicon Labs SiWG917 BRD2605A
- Simplicity SDK 2026.6.1
- WiSeConnect 3 SDK 4.1.1
- VCOM terminal: `921600`, `8-N-1`, no flow control

## Run It

1. Open the project in Simplicity Studio.
2. Build the `miniface_siwg917` target and flash the generated `.rps` image.
3. Open the J-Link CDC UART VCOM port at `921600` baud.
4. Reset the board. MiniFace draws its desktop directly in the terminal.

Use a terminal with ANSI colour, mouse reporting, and a viewport of at least
`100 x 30`. PuTTY and the MiniFace web terminal are suitable choices.

## Resource Model

MiniFace uses an 80 KB FreeRTOS heap in the BRD2605A external PSRAM region.
The base desktop keeps VCOM input and rendering alive. Network and audio are
opt-in services so unused peripherals do not reserve a task or radio stack at
boot.

## Project Status

This is an experimental community project. It is intended for development and
demonstration on the BRD2605A, not production deployment.

## License And Attribution

MiniFace project additions are MIT licensed. The compact windowing and terminal
engine derives from TinyDesk and remains available under its required MIT
notice at [LICENSES/TINYDESK-MIT.txt](LICENSES/TINYDESK-MIT.txt).

Created by [Tharun Babu](https://github.com/TharunBabu-05).
