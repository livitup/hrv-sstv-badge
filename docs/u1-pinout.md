# U1 Pinout — RP2350B (QFN-80)

Complete pin-to-net mapping for U1 as routed on the main badge PCB.
GPIOs are organized by physical side to minimize trace crossings.
See `docs/engineers-notebook/gpio-optimization.md` for rationale.

## Left Side (pins 1-20, top to bottom) — Display + Camera

| Pin | GPIO | Net | Function |
|-----|------|-----|----------|
| 1 | 4 | SPI_MISO | SPI0 data in (display, SD card) |
| 2 | 5 | DISP_CS | Display chip select |
| 3 | 6 | SPI_SCK | SPI0 clock (display, SD card) |
| 4 | 7 | SPI_MOSI | SPI0 data out (display, SD card) |
| 5 | — | +3V3 | IOVDD |
| 6 | 8 | CAM_D0 | Camera data bit 0 |
| 7 | 9 | CAM_D1 | Camera data bit 1 |
| 8 | 10 | CAM_D2 | Camera data bit 2 |
| 9 | 11 | CAM_D3 | Camera data bit 3 |
| 10 | — | DVDD | Internal regulator feedback |
| 11 | 12 | CAM_D4 | Camera data bit 4 |
| 12 | 13 | CAM_D5 | Camera data bit 5 |
| 13 | 14 | CAM_D6 | Camera data bit 6 |
| 14 | 15 | CAM_D7 | Camera data bit 7 |
| 15 | — | +3V3 | IOVDD |
| 16 | 16 | CAM_PCLK | Camera pixel clock |
| 17 | 17 | CAM_VSYNC | Camera vertical sync |
| 18 | 18 | CAM_HREF | Camera horizontal reference |
| 19 | 19 | DISP_DC | Display data/command select |
| 20 | 20 | CAM_XCLK | Camera master clock (~20MHz PWM) |

## Bottom Side (pins 21-40, left to right) — Controls + PWR LED

| Pin | GPIO | Net | Function |
|-----|------|-----|----------|
| 21 | 21 | DPAD_UP | D-pad up (active low) |
| 22 | 22 | DPAD_DOWN | D-pad down (active low) |
| 23 | 23 | DPAD_LEFT | D-pad left (active low) |
| 24 | — | +3V3 | IOVDD |
| 25 | 24 | DPAD_RIGHT | D-pad right (active low) |
| 26 | 25 | DPAD_CENTER | D-pad center / photo capture (active low) |
| 27 | 26 | AIRPLANE | Airplane mode slide switch |
| 28 | 27 | SAO1_GPIO1 | SAO connector 1, GPIO 1 (left edge) |
| 29 | — | +3V3 | IOVDD |
| 30 | — | XIN | 12 MHz crystal input |
| 31 | — | DVDD | Internal regulator |
| 32 | — | XOUT | 12 MHz crystal output |
| 33 | — | SWCLK | SWD clock (debug) |
| 34 | — | SWDIO | SWD data (debug) |
| 35 | 28 | SAO1_GPIO2 | SAO connector 1, GPIO 2 (left edge) |
| 36 | — | RUN | Reset (active low) |
| 37 | 29 | DISP_BL | Display backlight PWM |
| 38 | 30 | PWR_R | PWR LED red channel (PWM) |
| 39 | 31 | PWR_G | PWR LED green channel (PWM) |
| 40 | 32 | PWR_B | PWR LED blue channel (PWM) |

## Right Side (pins 41-60, bottom to top) — SA818 + I2C + SAO2

| Pin | GPIO | Net | Function |
|-----|------|-----|----------|
| 41 | — | +3V3 | IOVDD |
| 42 | 33 | SA818_PTT | Push-to-talk (active low) |
| 43 | 34 | SA818_PD | Power down / TPS22919 enable |
| 44 | 35 | SA818_HL | High/low power select |
| 45 | 36 | SA818_TX | UART1 TX → SA818 RXD |
| 46 | 37 | SA818_RX | UART1 RX ← SA818 TXD |
| 47 | 38 | SA818_SQ | Squelch detect input |
| 48 | 39 | SA818_ID | Band ID from carrier |
| 49 | 40 | RX_AUDIO | ADC input, RX audio from SA818 |
| 50 | — | +3V3 | IOVDD |
| 51 | — | DVDD | Internal regulator |
| 52 | 41 | SAO2_GPIO1 | SAO connector 2, GPIO 1 (right edge) |
| 53 | 42 | SAO2_GPIO2 | SAO connector 2, GPIO 2 (right edge) |
| 54 | 43 | DISP_RST | Display reset |
| 55 | 44 | I2C_SDA | I2C data (camera, fuel gauge, SAO) |
| 56 | 45 | I2C_SCL | I2C clock (camera, fuel gauge, SAO) |
| 57 | 46 | SD_CS | SD card chip select |
| 58 | 47 | — | Spare (unallocated) |
| 59 | — | ADC_AVDD | ADC analog power |
| 60 | — | +3V3 | IOVDD |

## Top Side (pins 61-80, right to left) — Audio + LED + Fixed

| Pin | GPIO | Net | Function |
|-----|------|-----|----------|
| 61 | 3 | I2S_BCK | I2S bit clock (PCM5102A) |
| 62 | 2 | I2S_DIN | I2S data out (PCM5102A) |
| 63 | 1 | I2S_LRCK | I2S L/R clock (PCM5102A) |
| 64 | 0 | LED_DATA | WS2812B chain data (26 LEDs) |
| 65 | — | +3V3 | IOVDD |
| 66 | — | ~QSPI_SS | Flash chip select (active low) |
| 67 | — | QSPI_SD2 | Flash data 2 |
| 68 | — | QSPI_SCLK | Flash clock |
| 69 | — | QSPI_SD0 | Flash data 0 |
| 70 | — | QSPI_SD3 | Flash data 3 |
| 71 | — | +3V3 | QSPI IOVDD |
| 72 | — | QSPI_SD1 | Flash data 1 |
| 73 | — | +3V3 | USB OTP VDD |
| 74 | — | USB_DP | USB D+ |
| 75 | — | USB_DM | USB D- |
| 76 | — | VREG_FB | Internal regulator feedback |
| 77 | — | VREG_VIN | Internal regulator input |
| 78 | — | VREG_LX | Internal regulator inductor |
| 79 | — | VREG_PGND | Internal regulator power ground |
| 80 | — | VREG_AVDD | Internal regulator analog power |

## Exposed Pad

| Pin | Net | Function |
|-----|-----|----------|
| 81 | GND | Exposed pad / thermal ground |
