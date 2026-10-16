# Contributing To MiniFace

MiniFace targets the Silicon Labs SiWG917 BRD2605A. Keep changes focused on
the board interface and avoid adding background tasks for optional features.

Before opening a pull request, build the `miniface_siwg917` target in
Simplicity Studio and test the VCOM interface at 921600 baud. Do not commit
generated firmware images, local SDK state, Wi-Fi credentials, or build output.
