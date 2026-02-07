# DEFCON SSTV Badge — Bill of Materials

**Version:** 2.0
**Last Updated:** 2025-02
**Source:** Consolidated from `docs/electrical-design.md`

This is the consolidated BOM for the DEFCON SSTV badge. Components are organized by subsystem and assembly method.

---

## Quick Summary

| Category | Est. Cost | Notes |
|----------|-----------|-------|
| Power System | ~$4.00 | TPS63001 buck-boost, TPS22919 load switch |
| USB-C Interface | ~$1.20 | Connector + ESD protection |
| Battery System | ~$11.50 | MCP73871, LC709203F, 2000mAh LiPo |
| Audio | ~$2.10 | PCM5102A DAC + filter passives |
| Display | ~$10.00 | ILI9341 module + backlight circuit |
| Camera | ~$6.00 | OV2640 DVP module + resistors |
| User Controls | ~$1.50 | Switches, PWR LED, resistors |
| Blinky LEDs | ~$2.40 | 26× WS2812B + bypass caps |
| MCU + Support | ~$3.00 | RP2350B, crystal, flash, passives |
| Connectors | ~$2.00 | SAO, carrier sockets, debug |
| **Main Badge Total** | **~$44** | Without carrier |
| SA818 Carrier | ~$17.00 | SA818, SMA, antenna, headers |
| **Complete Kit** | **~$61** | Main + one carrier |

*PCB fabrication adds ~$3-5/board at quantity.*

---

## Assembly Legend

| Code | Meaning |
|------|---------|
| **Fab** | Factory assembled (requires reflow/hot air) |
| **User** | User-solderable at conference (through-hole or large pads) |
| **Module** | Pre-assembled breakout, user installs |

---

## Power System

### Buck-Boost Regulator (TPS63001)

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| U_REG | 1 | TPS63001DRCR | QFN-10 (3×3mm) | Fab | Fixed 3.3V output |
| L1 | 1 | 1µH | 3×3mm | Fab | Coilcraft XAL3030-102 or equiv |
| C_IN1, C_IN2 | 2 | 10µF | 0805 | Fab | X5R/X7R, 10V, input |
| C_OUT1, C_OUT2 | 2 | 10µF | 0805 | Fab | X5R/X7R, 10V, output |
| R_PS | 1 | 1MΩ | 0402 | Fab | Power save mode (optional) |

**Subtotal:** ~$2.50

### Load Switch (TPS22919)

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| U_SW | 1 | TPS22919DCKR | SOT-23-5 | Fab | SA818 power control |
| C_SW1 | 1 | 100µF | 1206 | Fab | Output bulk capacitor |
| C_SW2 | 1 | 10µF | 0603 | Fab | Output bypass |

**Subtotal:** ~$0.80

---

## USB-C Interface

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| J_USB | 1 | USB4125-GF-A | TH 16-pin | User | GCT through-hole USB-C |
| U_ESD1 | 1 | USBLC6-2SC6 | SOT-23-6 | Fab | D+/D- ESD protection |
| D_VBUS | 1 | PESD5V0S1BL | SOD-323 | Fab | VBUS surge protection |
| R_CC1, R_CC2 | 2 | 5.1kΩ | 0402 | Fab | USB sink identification |
| C_USB | 1 | 10µF | 0603 | Fab | VBUS decoupling |

**Subtotal:** ~$1.20

---

## Battery System

### Charger (MCP73871)

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| U_CHG | 1 | MCP73871-2CCI/ML | QFN-20 (4×4mm) | Fab | Li-Po charge controller |
| R_PROG | 1 | 2kΩ | 0603 | Fab | Charge current = 1000/R = 500mA |
| R_SEL | 1 | 0Ω | 0402 | Fab | USB 500mA input mode |
| C_IN | 1 | 10µF | 0805 | Fab | Input cap, X5R 10V |
| C_VBAT | 1 | 10µF | 0805 | Fab | Battery cap, X5R 10V |
| C_OUT | 1 | 10µF | 0805 | Fab | Output cap, X5R 10V |

**Subtotal:** ~$3.00

### Fuel Gauge (LC709203F)

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| U_FUEL | 1 | LC709203FQH-01TWG | WDFN-8 (2×2.5mm) | Fab | Battery SoC monitor, I2C |
| C_FUEL | 1 | 100nF | 0402 | Fab | Decoupling |

**Subtotal:** ~$2.50

### Shared Thermistor

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| R_NTC | 1 | 10kΩ NTC | 0603 | Fab | B=3380, shared by MCP73871 + LC709203F |

*Place near battery for accurate temperature reading.*

### Battery + Connector

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| BAT1 | 1 | 2000mAh LiPo | Pouch ~50×35×8mm | User | JST-PH connector, 3.7V, with PCM |
| J_BAT | 1 | JST-PH 2-pin | Through-hole | User | Battery connector |

**Subtotal:** ~$6.50

---

## Audio Interface

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| U_DAC | 1 | PCM5102APWR | TSSOP-20 | Fab | I2S stereo DAC for TX |
| C_DAC1 | 1 | 10µF | 0805 | Fab | DVDD decoupling |
| C_DAC2 | 1 | 100nF | 0603 | Fab | AVDD decoupling |
| R_TX1 | 1 | 47kΩ | 0603 | User | TX attenuator (high) |
| R_TX2 | 1 | 470Ω | 0603 | User | TX attenuator (low) |
| C_TX1 | 1 | 10nF | 0603 | User | TX low-pass filter |
| C_TX2 | 1 | 100nF | 0603 | User | TX DC blocking |
| C_RX1 | 1 | 1µF | 0603 | User | RX DC blocking |
| R_RX1, R_RX2 | 2 | 4.7kΩ | 0603 | User | RX anti-alias filter |
| C_RX2 | 1 | 4.7nF | 0603 | User | RX anti-alias filter |
| R_RX3 | 1 | 47kΩ | 0603 | User | RX bias to 3.3V |
| R_RX4 | 1 | 47kΩ | 0603 | User | RX bias to GND |

**Subtotal:** ~$2.10

---

## Display Interface

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| U_DISP | 1 | ILI9341 2.4" module | Breakout | Module | 320×240 SPI, includes SD slot |
| Q_BL | 1 | 2N7002 | SOT-23 | Fab | Backlight control MOSFET |
| R_BL | 1 | 1kΩ | 0603 | User | Gate pull-down |

**Subtotal:** ~$10.00

---

## Camera Interface

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| U_CAM | 1 | OV2640 DVP module | Breakout | Module | 2.54mm headers, parallel DVP |
| R_CAM_RST | 1 | 10kΩ | 0603 | Fab | RESET pullup to 3.3V |
| R_CAM_PWDN | 1 | 10kΩ | 0603 | Fab | PWDN pulldown to GND |

**Subtotal:** ~$6.00

---

## User Controls

### Switches

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| SW_UP | 1 | Tactile 6×6mm | TH | User | D-pad up |
| SW_DOWN | 1 | Tactile 6×6mm | TH | User | D-pad down |
| SW_LEFT | 1 | Tactile 6×6mm | TH | User | D-pad left |
| SW_RIGHT | 1 | Tactile 6×6mm | TH | User | D-pad right |
| SW_CENTER | 1 | Tactile 6×6mm | TH | User | D-pad select |
| SW_PHOTO | 1 | Tactile 12mm | TH | User | Photo capture (larger) |
| SW_AIRPLANE | 1 | Slide SPDT | TH | User | Airplane mode toggle |

### PWR Status LED (Traditional RGB)

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| LED_PWR | 1 | RGB LED | 3528/5050 | Fab | Common cathode |
| R_PWR_R | 1 | 220Ω | 0603 | Fab | Red current limit |
| R_PWR_G | 1 | 100Ω | 0603 | Fab | Green current limit |
| R_PWR_B | 1 | 100Ω | 0603 | Fab | Blue current limit |

**Subtotal:** ~$1.50

---

## Blinky LED Matrix

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| LED_L0–L4 | 5 | WS2812B | 5050 | Fab | Left rabbit ear |
| LED_R0–R4 | 5 | WS2812B | 5050 | Fab | Right rabbit ear |
| LED_B0–B15 | 16 | WS2812B | 5050 | Fab | Display border (16 LEDs) |
| C_LED | 26 | 100nF | 0402 | Fab | Bypass caps (one per LED) |

**Subtotal:** ~$2.40
*(26× WS2812B @ ~$0.08 = $2.08, 26× caps @ ~$0.01 = $0.26)*

---

## MCU and Support

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| U_MCU | 1 | RP2350B | QFN-80 (9×9mm) | Fab | 48 GPIO variant required |
| Y1 | 1 | 12MHz | 3215 | Fab | ±20ppm, CL=10pF (required for USB) |
| C_Y1, C_Y2 | 2 | 15pF | 0402 | Fab | Crystal load caps |
| U_FLASH | 1 | W25Q128JVSIQ | SOIC-8 | Fab | 16MB QSPI flash |
| C_FLASH | 1 | 100nF | 0402 | Fab | Flash decoupling |
| C_MCU | 6 | 100nF | 0402 | Fab | MCU decoupling (one per VDD) |
| C_MCU_BULK | 2 | 10µF | 0603 | Fab | MCU bulk caps |
| SW_BOOT | 1 | Tactile 3×4mm | SMD | Fab | BOOTSEL for UF2 bootloader |
| R_SDA | 1 | 4.7kΩ | 0603 | Fab | I2C SDA pullup |
| R_SCL | 1 | 4.7kΩ | 0603 | Fab | I2C SCL pullup |

**Subtotal:** ~$3.65

*Note: Crystal is required — RP2350 internal oscillator is too inaccurate for USB (needs ±0.25%, ROSC is ±2-5%).*

---

## Connectors (Main Board)

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| J_SAO1 | 1 | 2×3 header | 2.54mm | User | SAO connector 1 |
| J_SAO2 | 1 | 2×3 header | 2.54mm | User | SAO connector 2 |
| J_CAR1 | 1 | 1×8 socket | 2.54mm | User | Carrier signal (left) |
| J_CAR2 | 1 | 1×8 socket | 2.54mm | User | Carrier signal (right) |
| J_CAR3 | 1 | 1×3 socket | 2.54mm | User | Carrier mechanical (left) |
| J_CAR4 | 1 | 1×3 socket | 2.54mm | User | Carrier mechanical (right) |
| J_DEBUG | 1 | TC2030-CTX-NL | Pads only | Fab | Tag-Connect SWD (no component) |

**Subtotal:** ~$2.00

*Note: Debug interface is Tag-Connect pogo pads only — no installed header. Developer needs TC2030-CTX-NL cable (~$40 one-time) for programming/debug.*

---

## SA818 Carrier Board

*Assembled separately — one per frequency band*

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| U_RF | 1 | SA818-V or SA818-U | Module | Fab | VHF or UHF transceiver |
| J_SMA | 1 | SMA right-angle | PCB mount | Fab | Female, 50Ω |
| C_RF1 | 1 | 100nF | 0402 | Fab | Decoupling |
| C_RF2 | 1 | 10µF | 0603 | Fab | Bulk capacitor |
| R_ID | 1 | 0Ω | 0402 | Fab | Band select (GND=VHF, VCC=UHF) |
| J1 | 1 | 1×8 male header | 2.54mm | User | Signal header (left) |
| J2 | 1 | 1×8 male header | 2.54mm | User | Signal header (right) |
| J3 | 1 | 1×3 male header | 2.54mm | User | Mechanical (left) |
| J4 | 1 | 1×3 male header | 2.54mm | User | Mechanical (right) |
| ANT | 1 | Stubby antenna | SMA male | Included | Band-matched |

**Subtotal (per carrier):** ~$17.00
*(SA818 ~$10, SMA ~$2, antenna ~$4, PCB+parts ~$1)*

---

## Passive Component Totals

### Resistors

| Value | Qty | Package | Use |
|-------|-----|---------|-----|
| 0Ω | 1 | 0402 | Carrier band ID |
| 100Ω | 2 | 0603 | PWR LED G/B |
| 220Ω | 1 | 0603 | PWR LED R |
| 470Ω | 1 | 0603 | TX attenuator |
| 1kΩ | 1 | 0603 | Backlight gate |
| 2kΩ | 1 | 0603 | Charge current |
| 4.7kΩ | 4 | 0603 | RX filter (2), I2C pullups (2) |
| 5.1kΩ | 2 | 0402 | USB CC |
| 10kΩ | 4+ | 0603 | Pullups, pulldowns |
| 47kΩ | 3 | 0603 | Audio bias/attenuation |
| 1MΩ | 1 | 0402 | TPS63001 PS |

### Capacitors

| Value | Qty | Package | Use |
|-------|-----|---------|-----|
| 15pF | 2 | 0402 | Crystal load |
| 4.7nF | 1 | 0603 | RX anti-alias |
| 10nF | 1 | 0603 | TX filter |
| 100nF | 40+ | 0402/0603 | Decoupling (LEDs, ICs) |
| 1µF | 1 | 0603 | RX DC block |
| 10µF | 15+ | 0603/0805 | Bulk caps |
| 100µF | 1 | 1206 | Load switch bulk |

---

## Sourcing Notes

### Recommended Suppliers

| Supplier | Best For | Lead Time |
|----------|----------|-----------|
| **LCSC** | Passives, WS2812B, common ICs | 1-2 weeks |
| **Digi-Key** | TI/Microchip ICs, precision parts | 2-3 days |
| **Mouser** | Alternative to Digi-Key | 2-3 days |
| **AliExpress** | Display, camera, SA818 modules | 2-4 weeks |
| **Amazon** | LiPo battery, lanyard, misc | 1-2 days |
| **JLCPCB** | PCB fab, SMT assembly | 1-2 weeks |

### Long Lead-Time Items (Order Early!)

- **SA818 modules** — 2-4 weeks from China
- **Custom PCBs** — 1-2 weeks from fab
- **OV2640 camera modules** — 1-3 weeks
- **ILI9341 display modules** — 1-3 weeks

### Kit Packaging Suggestions

- Group user-solderable parts in labeled bags by subsystem
- Include 2-3 spare 0603 resistors/caps (easy to lose)
- Print assembly instructions with component placement diagram
- Include test procedure checklist

---

## Cost Breakdown by Assembly Type

| Assembly | Component Cost | Notes |
|----------|---------------|-------|
| **Fab (SMT)** | ~$25 | ICs, small passives, LEDs |
| **User (TH)** | ~$12 | Switches, modules, headers |
| **Modules** | ~$16 | Display, camera |
| **Battery** | ~$6 | LiPo cell |
| **Carrier** | ~$17 | Separate board |
| **PCB** | ~$5 | Main + carrier at qty |
| **Total** | **~$81** | Complete kit with carrier |

---

## Revision History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2024-XX | Initial BOM (outdated design) |
| 2.0 | 2025-02 | Complete rewrite: TPS63001, PCM5102A, ILI9341, WS2812B blinky, USB-C with ESD |
| 2.1 | 2025-02 | Debug interface changed to Tag-Connect TC2030-CTX-NL (pads only) |
| 2.2 | 2025-02 | Battery system detailed: MCP73871 charger, LC709203F fuel gauge, shared NTC |
| 2.3 | 2025-02 | MCU support: 12MHz crystal (required for USB), W25Q128 16MB flash |
| 2.4 | 2025-02 | Added BOOTSEL button for UF2 bootloader |

---

*Keep this file in sync with `docs/electrical-design.md`!*
