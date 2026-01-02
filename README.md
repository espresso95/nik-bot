# nik-bot

## Build, upload, and monitor (PlatformIO)

1) Build: `pio run`

2) Upload to Arduino Uno R3 (USB): `pio run -t upload`

3) Open serial monitor at 9600 baud: `pio device monitor -b 9600`

If the upload succeeds, the onboard LED should blink and the monitor will print heartbeat messages.