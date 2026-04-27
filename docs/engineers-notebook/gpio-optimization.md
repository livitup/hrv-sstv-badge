# GPIO Assignment Optimization for PCB Routing

**Date:** 2026-03-31
**Decision:** Complete reassignment of all 48 RP2350B GPIOs to minimize trace crossings and simplify PCB routing.

## Problem

The original GPIO assignment was done early in the electrical design phase, before component placement was finalized. Signals were assigned by functional grouping (all SA818 pins together, all camera pins together, etc.) without considering which physical pins on the QFN-80 package they mapped to. This caused:

1. **SA818 signals split across two sides of U1** — GPIO30-32 on the bottom side and GPIO33-36 on the right side, forcing traces to turn corners around U1
2. **D-pad switches scattered across three sides** — GPIO37-39 on the right, GPIO26 on the bottom, GPIO41 on the right (upper), creating a routing nightmare
3. **I2S audio on the bottom side** (GPIO24-25, 29) — far from the carrier headers at the top of the board where audio routes to the SA818
4. **PWR LED on the upper-right** (GPIO44-46) — far from the bottom edge where the LED is physically located
5. **~8-12 trace crossings** in the signal routing, each requiring layer changes

## Solution

Reassign all GPIOs so each peripheral group occupies ONE physical side of the QFN-80 package, with pin order matching the mating connector where possible.

### RP2350B QFN-80 Physical Layout

The QFN-80 has 20 pins per side, numbered counter-clockwise from pin 1 (top-left):

**IMPORTANT:** The KiCad symbol pin numbers do NOT match the RP2350B datasheet pin numbers.
The mapping below is verified from the actual PCB footprint pad positions.

```
              TOP SIDE (pins 80-61, left to right)
        Pin 80 ──────────────────────────→ Pin 61
        ┌────────────────────────────────────────┐
        │  GPIO0-3 | QSPI | USB | VREG          │
Pin 1   │                                        │ Pin 60
  │     │  GPIO4                       GPIO47    │   ↑
  │     │  GPIO5                       GPIO46    │   │
  │     │  GPIO6                       GPIO45    │   │
  │     │  GPIO7                       GPIO44    │   │
  │     │  IOVDD                       GPIO43    │   │
  │     │  GPIO8 (CAM_D0)     GPIO42             │   │
  │     │  GPIO9              GPIO41             │   │
  ↓     │  GPIO10             GPIO40 (ADC0)      │   │
LEFT    │  GPIO11             GPIO39             │ RIGHT
SIDE    │  DVDD               GPIO38             │ SIDE
(1-20)  │  GPIO12             GPIO37             │ (41-60)
  │     │  GPIO13             GPIO36             │   │
  │     │  GPIO14             GPIO35             │   │
  │     │  GPIO15             GPIO34             │   │
  │     │  IOVDD              GPIO33             │   │
  │     │  GPIO16             IOVDD              │   │
  │     │  GPIO17                                │   │
  │     │  GPIO18                                │   │
  ↓     │  GPIO19                                │   ↑
Pin 20  │  GPIO20                                │ Pin 41
        └────────────────────────────────────────┘
        Pin 21 ──────────────────────────→ Pin 40
              BOTTOM SIDE (pins 21-40, left to right)
              GPIO21-32 | Crystal | SWD | RUN
```

**Top side detail (left to right):**
Pin 80(GPIO3/I2S_BCK), 79(GPIO2/I2S_DIN), 78(GPIO1/I2S_LRCK), 77(GPIO0/LED_DATA),
76(IOVDD), 75(QSPI_SS), 74(QSPI_SD1), 73(QSPI_SD2), 72(QSPI_SD0), 71(QSPI_SCLK),
70(QSPI_SD3), 69(IOVDD), 68(IOVDD), 67(USB_DP), 66(USB_DM),
65(VREG_FB), 64(IOVDD), 63(VREG_LX), 62(GND), 61(IOVDD)

**Right side detail (bottom to top):**
Pin 41(IOVDD), 42(GPIO33/SA818_PTT), 43(GPIO34/SA818_PD), 44(GPIO35/SA818_HL),
45(GPIO36/SA818_TX), 46(GPIO37/SA818_RX), 47(GPIO38/SA818_SQ), 48(GPIO39/SA818_ID),
49(GPIO40/RX_AUDIO), 50(IOVDD), 51(DVDD), 52(GPIO41/SAO2_GPIO1), 53(GPIO42/SAO2_GPIO2),
54(GPIO43/DISP_RST), 55(GPIO44/I2C_SDA), 56(GPIO45/I2C_SCL), 57(GPIO46/SD_CS),
58(GPIO47/spare), 59(ADC_AVDD), 60(IOVDD)

### Badge Physical Layout

U1 sits on the right side of the board:
- **Left of U1**: Display module (center of board), camera module (bottom-right)
- **Above U1**: Carrier board headers J7 (audio), J8 (control)
- **Below U1**: USB-C connector, BOOTSEL, PWR LED
- **Right of U1**: Right edge of board, SAO2 connector

### Assignment Strategy

**Left side** (17 GPIOs: GPIO4-20) — faces the display and camera area:
- GPIO4-7: SPI0 bus + display chip select (display connector above-left)
- GPIO8-15: Camera 8-bit parallel data bus (MUST be 8 consecutive GPIOs for PIO)
- GPIO16-18: Camera control (PCLK, VSYNC, HREF)
- GPIO19: Display DC (data/command, toggles every SPI transaction)
- GPIO20: Display RST (infrequent toggle, on left side near display)

**Bottom side** (12 GPIOs: GPIO21-32, interrupted by crystal/SWD/RUN) — faces bottom edge:
- GPIO21-25: D-pad cluster (5 switches, all bottom-left, near physical D-pad)
- GPIO26: SD card chip select (near SPI bus, bottom-left)
- GPIO27-28: SAO1 GPIOs (SAO1 is on left edge, reachable from bottom-left)
- GPIO29: Display backlight PWM
- GPIO30-32: PWR LED RGB (PWM, near bottom edge where LED sits)

**Right side** (15 GPIOs: GPIO33-47) — faces right edge, routes up to carrier:
- GPIO33-39: SA818 radio control (all 7 signals, matches J8 carrier header pin order)
- GPIO40: RX audio ADC input (must be GPIO40-47 for ADC)
- GPIO41-42: SAO2 GPIOs (SAO2 is on right edge)
- GPIO43: Airplane mode switch (slow toggle, location doesn't matter)
- GPIO44-45: I2C0 bus (near fuel gauge U7 and SAO2)
- GPIO46-47: Spare

**Top side** (4 GPIOs: GPIO0-3) — faces upward toward carrier headers:
- GPIO3-1: I2S audio (BCK, DIN, LRCK) at far LEFT of top side (pins 80-78) — pin-order matched to PCM5102A placed above-left of U1
- GPIO0: WS2812B LED chain data (pin 77, just right of I2S group)

## Peripheral Mux Verification

Every assignment was verified against the RP2350B GPIO function multiplexing table:

| Peripheral       | GPIO  | Mux Function     | Verified                        |
| ---------------- | ----- | ---------------- | ------------------------------- |
| SPI0 SCK         | 6     | F0               | Valid: 2, **6**, 18, 22, 34, 38 |
| SPI0 TX (MOSI)   | 7     | F0               | Valid: 3, **7**, 19, 23, 35, 39 |
| SPI0 RX (MISO)   | 4     | F0               | Valid: 0, **4**, 16, 20, 32, 36 |
| I2C0 SDA         | 44    | F2               | Valid: 0, 4, 8, ..., 40, **44** |
| I2C0 SCL         | 45    | F2               | Valid: 1, 5, 9, ..., 41, **45** |
| UART1 TX         | 36    | F1               | Valid: 4, 8, 20, 24, **36**, 40 |
| UART1 RX         | 37    | F1               | Valid: 5, 9, 21, 25, **37**, 41 |
| ADC0             | 40    | ADC              | Valid: GPIO40-47 only           |
| PIO (camera DVP) | 8-15  | F6/F7/F8         | Valid: all GPIOs support PIO    |
| PIO (I2S)        | 1-3   | F6/F7/F8         | Valid: all GPIOs support PIO    |
| PIO (WS2812B)    | 0     | F6/F7/F8         | Valid: all GPIOs support PIO    |
| PWM (CAM_XCLK)   | 20    | F4 (PWM2A)       | Valid: all GPIOs support PWM    |
| PWM (DISP_BL)    | 29    | F4 (PWM6B)       | Valid                           |
| PWM (PWR LED)    | 30-32 | F4 (PWM7A/7B/8A) | Valid                           |

## SA818 Carrier Header Matching

The carrier board header J8 has a FIXED pinout (carrier PCBs already fabricated):

| J8 Pin | Signal | GPIO | U1 Pin |
|--------|--------|------|--------|
| 1 | PTT | 33 | 42 |
| 2 | PD | 34 | 43 |
| 3 | HL | 35 | 44 |
| 4 | SQ | 38 | 47 |
| 5 | SA818 TXD (MCU RX) | 37 | 46 |
| 6 | SA818 RXD (MCU TX) | 36 | 45 |
| 7 | ID | 39 | 48 |
| 8 | GND | — | — |

GPIO33-35 (PTT/PD/HL) route directly to J8 pins 1-3 with zero crossings. GPIO36-37 (UART TX/RX) route to J8 pins 5-6. GPIO38 (SQ) needs to cross the UART pair to reach J8 pin 4 — route on B.Cu with 2 vias. This is the ONLY crossing in the SA818 group.

## I2S Pin-Order Matching to PCM5102A

The PCM5102A (TSSOP-20) has I2S pins on its left side going top-to-bottom: BCK(4), DIN(5), LRCK(6).

U1's top side has GPIO3=BCK(pin 80, far left), GPIO2=DIN(pin 79), GPIO1=LRCK(pin 78).

With U8 placed above-left of U1, these pins face each other in matching order. Zero crossings.

## I2C Bus Topology

I2C moved from GPIO0/1 (top side) to GPIO44/45 (right side) to be near the fuel gauge (U7) and SAO2 connector on the right edge. The bus topology:

- Short stubs: U7 (fuel gauge), SAO2 (J6), both on right side of board
- Medium run: SAO1 (J5) on left edge, via B.Cu across bottom of board
- Long run: Camera (J4) on bottom-right, via B.Cu

I2C at 100-400kHz tolerates long traces with no issues. 4.7k pullups stay near U7.

## Crossing Inventory

| Signal Group | Crossings | Resolution |
|-------------|-----------|------------|
| SPI to display | 0 | All on upper-left side, parallel route left |
| Camera D0-D7 | 0 | Sequential on mid-left side |
| Camera VSYNC/HREF | 0 | Route on B.Cu under data bus to J4 |
| I2S to PCM5102A | 0 | Pin-order matched |
| SA818 to J8 | 1 | SQ crosses UART pair; route SQ on B.Cu |
| D-pad | 0 | All grouped on bottom-left |
| PWR LED | 0 | Three adjacent pins, parallel route down |
| I2C bus | 0 | Shared bus, stubs from right side |
| LED_DATA | 0 | Single wire from top side to ear base |

**Total: 1 crossing** (down from ~8-12 in the original design).

## Component Placement Recommendations

```
                    J7(audio)  J8(control)
                        |          |
         +--------------+----------+---+
         |  U8(PCM5102A)               |
         |     U2(flash)               |
  SAO1 --+  +---------------------+    |
  (J5)   |  |  pin80    U1    pin61|   +-- SAO2 (J6)
         |  |  (I2S)  (RP2350B)   |    |
  J3 ----+  |  pin1 *         pin60|   |
(display)|  +---------------------+    |
         |   Y1(xtal)  J9(SWD)  U7    |
         |                     R1,R2   |
         |  D-pad    SD(J10)   U5,U6   |
         |  (SW1-5)          (power)   |
         |                    J4       |
         |  SW7    U4(charger)(camera) |
         | (airplane)          D1(PWR) |
         |         J2(USB-C)           |
         +-----------------------------+
```

### Key Placement Notes

**Verified from actual PCB footprint pad positions (not datasheet pin numbers).**

Power components (U4, U5, U6, U7, TH1) are on the **back** of the board, clustered in the bottom-left. They stay together — their GPIO connections (I2C for U7, SA818_PD for U6) are low-speed and reach U1 via vias.

**Front side components that move:**

| Ref | Part | Position | Why |
|-----|------|----------|-----|
| U8 | PCM5102A | **Above-LEFT** of U1 | I2S pins (78-80) are at **top-left** of U1; VOUTL routes up to J7 |
| D1 + R14-R16 | PWR LED + resistors | Bottom edge, **right of center** | GPIO30-32 (pins 38-40) at bottom-right of bottom side |
| R1, R2 | I2C pullups | Near U1 right side, **upper area** (pins 55/56) | I2C moved to GPIO44/45 |
| SW1-5 | D-pad | **Left edge**, centered vertically | D-pad GPIOs (pins 21-26) are on bottom-left of U1; traces route via vias across board to left edge. Slow signals, no routing concern. |

**Front side components that stay put:**

| Ref | Part | Position | Why |
|-----|------|----------|-----|
| U2 | W25Q128JVS | Above U1, center | QSPI traces < 5mm |
| J3 | Display | Center of board, left of U1 | SPI from top of left side |
| J4 | OV2640 camera | Bottom-right | Data routes from U1 left side on B.Cu under U1 |
| J5 | SAO1 | Left edge | GPIO27/28 from bottom-left of U1 |
| J6 | SAO2 | Right edge, **upper area** | GPIO41/42 (pins 52/53) toward top of right side |
| J8 | Carrier control | Top edge | SA818 signals route up from bottom of right side |

**Back side components (stay clustered bottom-left):**

| Ref | Part | Why stays on back |
|-----|------|-------------------|
| U4 | MCP73871 (charger) | No GPIOs, near battery/USB |
| U5 | TPS63001 (buck-boost) | No GPIOs, keep with L1 inductor |
| U6 | TPS22919 (load switch) | SA818_PD toggles infrequently, via to U1 right side |
| U7 | LC709203F (fuel gauge) | I2C at 100-400kHz tolerates via + trace to U1 |
| TH1 | NTC thermistor | Shared between U4 and U7 |

## What Changed

| GPIO | Old Signal | New Signal | Reason |
|------|-----------|------------|--------|
| 0 | I2C_SDA | LED_DATA | LED chain starts at ear (top side) |
| 1 | I2C_SCL | I2S_LRCK | Audio near carrier headers |
| 2 | SPI_SCK | I2S_DIN | Audio near carrier headers |
| 3 | SPI_MOSI | I2S_BCK | Audio near carrier headers |
| 6 | DISP_DC | SPI0_SCK | SPI moved to left side |
| 7 | DISP_RST | SPI0_MOSI | SPI moved to left side |
| 19 | NC | DISP_DC | DC on left side near display |
| 20 | SAO1_GPIO1 | DISP_RST | Display reset on left side near display |
| 21-25 | various | DPAD cluster | D-pad consolidated bottom-left |
| 26 | DPAD_RIGHT | SD_CS | SD card CS on bottom-left, near SPI |
| 27-28 | SAO2 | SAO1 | SAO1 moved to left edge |
| 29 | I2S_DIN | DISP_BL | Backlight on bottom side |
| 30-32 | SA818 UART/PTT | PWR_R/G/B | PWR LED near bottom edge |
| 33-39 | SA818 (partial) | SA818 (all) | SA818 consolidated on right |
| 40 | RX_AUDIO | RX_AUDIO | Unchanged (ADC0) |
| 41-42 | DPAD/spare | SAO2 | SAO2 on right edge |
| 43 | AIRPLANE | AIRPLANE | Stays as airplane, moved from GPIO26 to right side |
| 44-45 | PWR_R/G | I2C0 SDA/SCL | I2C near fuel gauge + SAO |
| 46 | PWR_B | spare | Unallocated |
| 47 | LED_DATA | spare | |

GPIO8-18 (camera data + control) and GPIO40 (ADC) are unchanged.
