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

```
              TOP SIDE (pins 61-80, right to left)
        Pin 80 ←──────────────────────────── Pin 61
        ┌────────────────────────────────────────┐
        │  VREG | USB | QSPI | GPIO0-3          │
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
- GPIO20: Camera XCLK (PWM ~20MHz master clock)

**Bottom side** (12 GPIOs: GPIO21-32, interrupted by crystal/SWD/RUN) — faces bottom edge:
- GPIO21-25: D-pad cluster (5 switches, all bottom-left, near physical D-pad)
- GPIO26: Airplane switch (near D-pad)
- GPIO27-28: SAO1 GPIOs (SAO1 is on left edge, reachable from bottom-left)
- GPIO29: Display backlight PWM
- GPIO30-32: PWR LED RGB (PWM, near bottom edge where LED sits)

**Right side** (15 GPIOs: GPIO33-47) — faces right edge, routes up to carrier:
- GPIO33-39: SA818 radio control (all 7 signals, matches J8 carrier header pin order)
- GPIO40: RX audio ADC input (must be GPIO40-47 for ADC)
- GPIO41-42: SAO2 GPIOs (SAO2 is on right edge)
- GPIO43: Display reset (infrequent toggle, can be anywhere)
- GPIO44-45: I2C0 bus (near fuel gauge U7 and SAO2)
- GPIO46: SD card chip select
- GPIO47: Spare

**Top side** (4 GPIOs: GPIO0-3) — faces upward toward carrier headers:
- GPIO0: WS2812B LED chain data (PIO, routes up to rabbit ear base)
- GPIO1-3: I2S audio (LRCK, DIN, BCK) — pin-order matched to PCM5102A placed directly above U1

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

U1's top side goes right-to-left: GPIO3=BCK(pin 61), GPIO2=DIN(pin 62), GPIO1=LRCK(pin 63).

With U8 placed directly above U1, these pins face each other in matching order. Zero crossings.

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
         |           U8(PCM5102A)      |
         |     U2(flash)               |
  SAO1 --+  +---------------------+    |
  (J5)   |  |     U1 (RP2350B)    |    +-- SAO2 (J6)
         |  |                     |    |
  J3 ----+  |  pin1 *            |    |
(display)|  +---------------------+    |
         |   Y1(xtal)  J9(SWD)  U7    |
         |                             |
         |  D-pad    SD(J10)   U5,U6   |
         |  (SW1-5)          (power)   |
         |                    J4       |
         |  SW7    U4(charger)(camera) |
         | (airplane)          D1(PWR) |
         |         J2(USB-C)           |
         +-----------------------------+
```

### Key Placement Notes

| Ref | Part | Position | Why |
|-----|------|----------|-----|
| U8 | PCM5102A | Above U1, right of center | I2S pins face U1 top-right; VOUTL faces J7 |
| U2 | W25Q128JVS | Above U1, left of center | QSPI traces < 5mm |
| J4 | OV2640 camera | Bottom-right | Data routes from U1 left side on B.Cu under U1 |
| J5 | SAO1 | Left edge | GPIO27/28 from bottom-left of U1 |
| J6 | SAO2 | Right edge | GPIO41/42 from right side of U1 |
| J8 | Carrier control | Top edge, right | Pin 1 nearest to GPIO33 (bottom of right side) |
| D1 | PWR LED | Bottom edge | GPIO30-32 route straight down from bottom side |

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
| 20 | SAO1_GPIO1 | CAM_XCLK | Camera clock on left side |
| 21-25 | various | DPAD cluster | D-pad consolidated bottom-left |
| 26 | DPAD_RIGHT | AIRPLANE | Switch near D-pad |
| 27-28 | SAO2 | SAO1 | SAO1 moved to left edge |
| 29 | I2S_DIN | DISP_BL | Backlight on bottom side |
| 30-32 | SA818 UART/PTT | PWR_R/G/B | PWR LED near bottom edge |
| 33-39 | SA818 (partial) | SA818 (all) | SA818 consolidated on right |
| 40 | RX_AUDIO | RX_AUDIO | Unchanged (ADC0) |
| 41-42 | DPAD/spare | SAO2 | SAO2 on right edge |
| 43 | AIRPLANE | DISP_RST | Reset can be anywhere |
| 44-45 | PWR_R/G | I2C0 SDA/SCL | I2C near fuel gauge + SAO |
| 46 | PWR_B | SD_CS | SD CS on right side |
| 47 | LED_DATA | spare | |

GPIO8-18 (camera data + control) and GPIO40 (ADC) are unchanged.
