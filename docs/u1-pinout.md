# U1 Pinout — RP2350B (QFN-80)

Complete pin-to-net mapping for U1 as routed on the main badge PCB.

| Pin | Net | Function |
|-----|-----|----------|
| 1 | SPI_MISO | SPI0 data in (display, SD card) |
| 2 | DISP_CS | Display chip select |
| 3 | DISP_DC | Display data/command |
| 4 | DISP_RST | Display reset |
| 5 | +3V3 | IOVDD |
| 6 | CAM_D0 | Camera data bit 0 |
| 7 | CAM_D1 | Camera data bit 1 |
| 8 | CAM_D2 | Camera data bit 2 |
| 9 | CAM_D3 | Camera data bit 3 |
| 10 | VREG_FB | Internal regulator feedback |
| 11 | CAM_D4 | Camera data bit 4 |
| 12 | CAM_D5 | Camera data bit 5 |
| 13 | CAM_D6 | Camera data bit 6 |
| 14 | CAM_D7 | Camera data bit 7 |
| 15 | +3V3 | IOVDD |
| 16 | CAM_PCLK | Camera pixel clock |
| 17 | CAM_VSYNC | Camera vertical sync |
| 18 | CAM_HREF | Camera horizontal reference |
| 19 | — | Unconnected (GPIO19) |
| 20 | SAO1_GPIO1 | SAO connector 1, GPIO 1 |
| 21 | SAO1_GPIO2 | SAO connector 1, GPIO 2 |
| 22 | DISP_BL | Display backlight PWM |
| 23 | SD_CS | SD card chip select |
| 24 | +3V3 | IOVDD |
| 25 | I2S_BCK | I2S bit clock (PCM5102A) |
| 26 | I2S_LRCK | I2S L/R clock (PCM5102A) |
| 27 | DPAD_RIGHT | D-pad right (active low) |
| 28 | SAO2_GPIO1 | SAO connector 2, GPIO 1 |
| 29 | +3V3 | IOVDD |
| 30 | XIN | 12 MHz crystal input |
| 31 | XOUT | 12 MHz crystal output |
| 32 | VREG_FB | Internal regulator feedback |
| 33 | SWCLK | SWD clock (debug) |
| 34 | SWDIO | SWD data (debug) |
| 35 | RUN | Reset (active low) |
| 36 | SAO2_GPIO2 | SAO connector 2, GPIO 2 |
| 37 | I2S_DIN | I2S data out (PCM5102A) |
| 38 | SA818_RX | UART1 RX ← SA818 TXD |
| 39 | SA818_TX | UART1 TX → SA818 RXD |
| 40 | SA818_PTT | Push-to-talk (active low) |
| 41 | +3V3 | IOVDD |
| 42 | SA818_PD | Power down / TPS22919 enable |
| 43 | SA818_HL | High/low power select |
| 44 | SA818_ID | Band ID from carrier |
| 45 | SA818_SQ | Squelch detect input |
| 46 | DPAD_UP | D-pad up (active low) |
| 47 | DPAD_DOWN | D-pad down (active low) |
| 48 | DPAD_LEFT | D-pad left (active low) |
| 49 | RX_AUDIO | ADC input, RX audio from SA818 |
| 50 | +3V3 | IOVDD |
| 51 | VREG_FB | Internal regulator feedback |
| 52 | DPAD_CENTER | D-pad center / photo capture (active low) |
| 53 | — | Unconnected (GPIO42/ADC2) |
| 54 | AIRPLANE | Airplane mode slide switch |
| 55 | PWR_R | PWR LED red channel (PWM) |
| 56 | PWR_G | PWR LED green channel (PWM) |
| 57 | PWR_B | PWR LED blue channel (PWM) |
| 58 | LED_DATA | WS2812B chain data (26 LEDs) |
| 59 | +3V3 | IOVDD |
| 60 | +3V3 | IOVDD |
| 61 | +3V3 | IOVDD |
| 62 | GND | Digital ground |
| 63 | VREG_LX | Internal regulator inductor |
| 64 | +3V3 | IOVDD |
| 65 | VREG_FB | Internal regulator feedback |
| 66 | USB_DM | USB D- |
| 67 | USB_DP | USB D+ |
| 68 | +3V3 | IOVDD |
| 69 | +3V3 | IOVDD |
| 70 | QSPI_SD3 | Flash data 3 |
| 71 | QSPI_SCLK | Flash clock |
| 72 | QSPI_SD0 | Flash data 0 |
| 73 | QSPI_SD2 | Flash data 2 |
| 74 | QSPI_SD1 | Flash data 1 |
| 75 | ~QSPI_SS | Flash chip select (active low) |
| 76 | +3V3 | IOVDD |
| 77 | I2C_SDA | I2C data (camera, fuel gauge, SAO) |
| 78 | I2C_SCL | I2C clock (camera, fuel gauge, SAO) |
| 79 | SPI_SCK | SPI0 clock (display, SD card) |
| 80 | SPI_MOSI | SPI0 data out (display, SD card) |
| 81 | GND | Exposed pad |
