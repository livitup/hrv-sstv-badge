# DEFCON SSTV Badge — Bill of Materials

**Version:** 3.10
**Last Updated:** 2026-11-13
**Source:** KiCad schematic export, verified against `docs/electrical-design.md`

This is the consolidated BOM for the DEFCON SSTV badge. Components are organized by subsystem and assembly method.

---

## Quick Summary

| Category | Est. Cost | Notes |
|----------|-----------|-------|
| Power System | ~$6.00 | TPS63001 buck-boost, TPS63000 LED rail, TPS22919 load switch |
| USB-C Interface | ~$1.20 | Connector + ESD protection |
| Battery System | ~$11.50 | MCP73871, LC709203F, 2000mAh LiPo |
| Audio | ~$2.10 | PCM5102A DAC + filter passives |
| Display | ~$10.00 | ILI9341 module + backlight circuit |
| Camera | ~$6.00 | OV2640 DVP module (must have on-board oscillator) + resistors |
| User Controls | ~$1.30 | Switches, PWR LED, resistors |
| Blinky LEDs | ~$2.28 | 10× WS2812B-5050 + 16× WS2812B-2020 + bypass caps |
| MCU + Support | ~$3.90 | RP2350B, crystal, flash, regulator inductor, passives |
| Connectors | ~$2.00 | SAO, carrier sockets, SD card, debug |
| **Main Badge Total** | **~$66** | Without carrier |
| SA818 Carrier | ~$17.00 | SA818, SMA, antenna, headers |
| **Complete Kit** | **~$83** | Main + one carrier |

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
| U5 | 1 | TPS63001DRCR | QFN-10 (3×3mm) | Fab | Fixed 3.3V output |
| L1 | 1 | 2.2µH | 3×3mm | Fab | Coilcraft XAL3030-222 or equiv (TPS63001 datasheet value) |
| C_IN, C_OUT1 | 2 | 10µF | 0805 | Fab | X5R/X7R, 10V, input/output |

*Note: PS/SYNC resistor intentionally omitted — forced PWM mode provides cleaner audio with minimal ripple. Power save mode would reduce idle current but risks noise coupling into SSTV audio path.*

**Subtotal:** ~$2.50

### LED Supply Rail (TPS63000, 4.0V "VLED")

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| U11 | 1 | TPS63000DRCR | VSON-10 (3×3mm) | Fab | Adjustable buck-boost, set to 4.0V for the WS2812B LEDs |
| L9 | 1 | 2.2µH | 3×3mm | Fab | Same part as L1 |
| C79, C81, C82 | 3 | 10µF | 0805 | Fab | X5R/X7R, 10V, input/output |
| C80 | 1 | 100nF | 0402 | Fab | VINA decoupling |
| R23 | 1 | 1MΩ 1% | 0603 | Fab | Feedback divider, upper |
| R24 | 1 | 143kΩ 1% | 0603 | Fab | Feedback divider, lower (sets 4.0V) |
| C83 | 1 | 2.2pF | 0402 | Fab | Feed-forward capacitor across R23 |

*Note: PS/SYNC is grounded on U11 (power-save mode on), the opposite of U5 — LED ripple does not matter and light-load efficiency does.*

**Subtotal:** ~$2.00 *(estimate)*

### Load Switch (TPS22919)

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| U6 | 1 | TPS22919DCKT | SOT-23-5 | Fab | SA818 power control |
| C17 | 1 | 100µF | 1206 | Fab | Output bulk capacitor |
| C18 | 1 | 10µF | 0603 | Fab | Output bypass |

**Subtotal:** ~$0.80

---

## USB-C Interface

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| J2 | 1 | USB4105-GF-A | SMT+TH hybrid | Fab | GCT USB-C with D+/D- (USB 2.0) |
| U3 | 1 | USBLC6-2SC6 | SOT-23-6 | Fab | D+/D- ESD protection |
| D_VBUS1 | 1 | TVS (PESD5V0S1BL) | SOD-323 | Fab | VBUS surge protection |
| R_CC1, R_CC2 | 2 | 5.1kΩ | 0402 | Fab | USB sink identification (to GND) |
| C_USB1 | 1 | 1µF | 0603 | Fab | VBUS decoupling at the connector (reduced from 10µF to keep total VBUS capacitance under the 10µF USB limit) |

**Subtotal:** ~$1.20

---

## Battery System

### Charger (MCP73871)

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| U4 | 1 | MCP73871-2CCI/ML | QFN-20 (4×4mm) | Fab | Li-Po charge controller |
| R3 | 1 | 2kΩ | 0603 | Fab | PROG1: Charge current = 1000/R = 500mA |
| R4 | 1 | 15kΩ | 0603 | Fab | PROG3: Pre-charge/termination = ~67mA |
| C8, C11 | 2 | 10µF | 0805 | Fab | Battery/output caps, X5R 10V |
| C10 | 1 | 2.2µF | 0805 | Fab | Charger input (reduced from 10µF; C76 4.7µF sits at the IN pins) |

**Subtotal:** ~$3.00

### Fuel Gauge (LC709203F)

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| U7 | 1 | LC709203FQH-01TWG | WDFN-8 (2×2.5mm) | Fab | Battery SoC monitor, I2C |
| C84 | 1 | 1µF | 0402 | Fab | Fuel gauge supply decoupling (on the battery net, VBAT) |
| C16 | 1 | 100nF | 0402 | Fab | VSYS decoupling beside the 3.3V regulator's VINA pin |

**Subtotal:** ~$2.50

### Charger Thermistor

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| TH1 | 1 | NTC 10k | 0603 | Fab | B=3380, for the MCP73871 charger (THERM). Not shared with the fuel gauge |

*Place near battery for accurate temperature reading.*

### Battery + Connector

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| BAT1 | 1 | 2000mAh LiPo | Pouch ~50×35×8mm | User | JST-PH connector, 3.7V, with PCM |
| J_BAT (J1) | 1 | JST S2B-PH-SM4-TB | SMD, side-entry | Fab | Battery connector, JST-PH 2-pin. Surface-mount so no pins come through under the display |

**Subtotal:** ~$6.50

---

## Audio Interface

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| U8 | 1 | PCM5102A | TSSOP-20 | Fab | I2S stereo DAC for TX |
| C20 | 1 | 10µF | 0805 | Fab | DVDD decoupling |
| C19 | 1 | 100nF | 0603 | Fab | AVDD decoupling |
| C21, C22 | 2 | 2.2µF | 0603 | Fab | Charge pump caps (CAPP, CAPM) |
| R5 | 1 | 47kΩ | 0603 | User | TX attenuator (high) |
| R6 | 1 | 470Ω | 0603 | User | TX attenuator (low) |
| C24 | 1 | 10nF | 0603 | User | TX low-pass filter |
| C25 | 1 | 100nF | 0603 | User | TX DC blocking |
| C26 | 1 | 1µF | 0603 | User | RX DC blocking |
| R7, R8 | 2 | 4.7kΩ | 0603 | User | RX anti-alias filter |
| C27 | 1 | 4.7nF | 0603 | User | RX anti-alias filter |
| R9 | 1 | 47kΩ | 0603 | User | RX bias to 3.3V |
| R10 | 1 | 47kΩ | 0603 | User | RX bias to GND |

**Subtotal:** ~$2.10

---

## Display Interface

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| J3 | 1 | QDtech / LCDwiki MSP2401 — 2.4" ILI9341 SPI TFT module, no touch (MSP2402 touch version also fits) | Breakout, 77.18 × 42.72 mm | Module | 320×240 SPI display; 14-pin header plugs into J3 |
| J23 | 1 | 1×3 socket | 2.54mm | User | Support for the display's free end (no electrical connection) |
| — | 1 | 1×3 male header | 2.54mm | User | Soldered into the first three SD header holes on the display module; plugs into J23 |
| Q1 | 1 | AO3401A | SOT-23 | Fab | Backlight high-side switch (P-channel) |
| R11 | 1 | 1kΩ | 0603 | User | Gate pull-down |

**Subtotal:** ~$10.00

---

## Camera Interface

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| J4 | 1 | OV2640 DVP module **with on-board clock oscillator** | Breakout | Module | 2×9 socket (PinSocket_2x09_P2.54mm_Vertical), parallel DVP. **Must be self-clocked — no XCLK from the badge** |
| R12, R13 | 2 | 10kΩ | 0603 | Fab | RESET pullup / PWDN pulldown |

> [!CAUTION]
> **The OV2640 module MUST have its own on-board oscillator.** The main badge has no XCLK net and J4 has no XCLK pin, and the board is locked. A module that expects an external clock will never produce an image. Check for a crystal/oscillator on the module, check that its 2×9 header pinout matches J4 (no XCLK pin), and test one sample before buying in quantity. Full details: `docs/electrical-design.md` → Camera Interface.

**Subtotal:** ~$6.00

---

## User Controls

### Switches

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| SW1-SW5 | 5 | Tactile 6×6mm | TH | User | D-pad (up/down/left/right/center); center doubles as photo capture |
| SW7 | 1 | Slide SPDT | TH | User | Airplane mode toggle |

### PWR Status LED (Traditional RGB)

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| D1 | 1 | AAA3528LSEEZGKQBKS | PLCC-4 (3.5×2.8mm, 1.9mm high) | Fab | Kingbright flat-top RGB LED, **common-anode** (pin 2). Battery status and TX/RX indicator |
| R14 | 1 | 220Ω | 0603 | Fab | Red cathode current limit |
| R15, R16 | 2 | 100Ω | 0603 | Fab | Green/Blue cathode current limit |

**Note:** Common-anode LED — anode connects to +3.3V, cathodes through resistors to GPIOs. Firmware uses inverted logic: GPIO LOW = LED on.

**Subtotal:** ~$1.30

---

## Blinky LED Matrix

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| D2–D11 | 10 | WS2812B | 5050 (5.0×5.0mm) | Fab | Rabbit ear antennas (5 per ear) |
| D12–D27 | 16 | WS2812B-2020 | 2020 (2.0×2.0mm) | Fab | Display border (3×5×3×5 layout) |
| C28–C53 | 26 | 100nF | 0402 | Fab | Bypass caps (one per LED) |

**Subtotal:** ~$2.28
*(10× WS2812B-5050 @ ~$0.08 = $0.80, 16× WS2812B-2020 @ ~$0.06 = $0.96, 26× caps @ ~$0.02 = $0.52)*

---

## MCU and Support

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| U1 | 1 | RP2350B | QFN-80 (10×10mm) | Fab | 48 GPIO variant required |
| L2 | 1 | 3.3µH, Abracon AOTA-B201610S3R3-101-T | 0806 (2016) | Fab | RP2350 regulator inductor. **Polarity-marked: the dot goes at pad 1 (1.1 V side). Do not substitute** — see the Raspberry Pi hardware design guide |
| Y1 | 1 | 12MHz | 3215 | Fab | ±20ppm, CL=10pF (required for USB) |
| C_Y1, C_Y2 | 2 | 15pF | 0402 | Fab | Crystal load caps |
| U2 | 1 | W25Q128JVS | SOIC-8 | Fab | 16MB QSPI flash |
| C9 | 1 | 100nF | 0402 | Fab | Flash decoupling |
| C3-C7 | 5 | 100nF | 0402 | Fab | MCU VDD decoupling |
| C2 | 1 | 10µF | 0603 | Fab | MCU bulk cap |
| C1 | 1 | 4.7µF | 0402 (wide-pad footprint) | Fab | RP2350 regulator output capacitor (1.1 V) |
| C77 | 1 | 4.7µF | 0402 (wide-pad footprint) | Fab | RP2350 regulator input capacitor (VREG_VIN) |
| R22 | 1 | 33Ω | 0402 | Fab | VREG_AVDD filter resistor |
| C78 | 1 | 4.7µF | 0402 | Fab | VREG_AVDD filter capacitor |
| C70, C72 | 2 | 100nF | 0402 | Fab | DVDD decoupling under U1 at pins 10 and 51 (back side) |
| R20, R21 | 2 | 27Ω | 0402 | Fab | USB D−/D+ series termination at U1 (per RP2350 hardware design guide) |
| R19 | 1 | 1kΩ | 0402 | Fab | In series with BOOTSEL switch on QSPI_SS (back side, about 14mm from the flash chip-select line) |
| R29 | 1 | 1kΩ | 0402 | Fab | Crystal series resistor between XOUT and Y1 (limits crystal drive level) |
| R25–R28 | 4 | 10kΩ | 0402 | Fab | SD card pull-ups to 3.3V: chip select (R25), data out / MISO (R26), DAT2 (R27), DAT1 (R28) |
| TP1–TP6 | 6 | Test pad 1.0mm | Pad only | — | Back side: GND, +3V3, VBUS, VSYS, VLED, SA818_VCC (no component, excluded from BOM) |
| SW_BOOT1 | 1 | Tactile 3×4mm | SMD | Fab | BOOTSEL for UF2 bootloader |
| R1, R2 | 2 | 4.7kΩ | 0603 | Fab | I2C SDA/SCL pullups (to +3V3) |
| R17 | 1 | 10kΩ | 0603 | Fab | RUN pin pullup |

*Added in the 2026-11 board review, in other sections' circuits: C73 (1µF, 0603) at the TPS22919 input, C76 (4.7µF, 0603) at the MCP73871 IN pins, C74/C75 (100nF, 0402) at the PCM5102A DVDD and CPVDD pins. All fab-assembled. C71 is not used.*

**Subtotal:** ~$3.95

*Notes:*
- *Crystal is required — RP2350 internal oscillator is too inaccurate for USB (needs ±0.25%, ROSC is ±2-5%).*
- *L2 is required for the RP2350's internal switching regulator (converts 3.3V → 1.1V core voltage). The regulator parts and their placement follow Raspberry Pi's RP2350B minimal reference design; the 4.7µF 0402 parts must be rated for that value at 3.3 V bias.*
- *I2C pullups MUST connect to +3V3 (IOVDD level), NOT to DVDD (1.1V core).*

---

## Connectors (Main Board)

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| J1 | 1 | JST S2B-PH-SM4-TB (JST-PH 2-pin) | SMD, side-entry | Fab | Battery connector, on the back; plug enters from the top-of-board side |
| J5 | 1 | 2×3 header | 2.54mm | User | SAO connector 1 |
| J6 | 1 | 2×3 header | 2.54mm | User | SAO connector 2 |
| J7, J8 | 2 | 1×8 socket | 2.54mm | User | Carrier signal headers |
| J9 | 1 | TC2030-CTX-NL | Pads only | Fab | Tag-Connect SWD (no component, excluded from BOM) |
| J10 | 1 | Micro SD Card | SMD | Fab | Standalone SD card socket |
| J11, J12 | 2 | 1×3 socket | 2.54mm | User | Carrier mechanical (GND) |

**Subtotal:** ~$2.50

*Note: Debug interface is Tag-Connect pogo pads only — no installed header. Developer needs TC2030-CTX-NL cable (~$40 one-time) for programming/debug.*

---

## SA818 Carrier Boards

*Assembled separately — one per frequency band. Two variants with the same PCB layout; they differ in the module, the band-ID resistor, and the harmonic low-pass filter values. LPF values are calculated starting points and may change after prototype tuning — see `docs/engineers-notebook/harmonic-filter-design.md`.*

### SA818-V Carrier (VHF, 134-174 MHz)

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| U9 | 1 | SA818V | Module (2mm pitch) | Fab | VHF transceiver |
| J13 | 1 | SMA edge-mount | Amphenol 132289 footprint | Fab | Female, 50Ω, LPF output |
| J14 | 1 | 1×8 male header | 2.54mm | User | Power/Audio (mounted on back) |
| J15 | 1 | 1×8 male header | 2.54mm | User | Control/Data (mounted on back) |
| J16 | 1 | 1×3 male header | 2.54mm | User | Mechanical (GND) |
| J17 | 1 | 1×3 male header | 2.54mm | User | Mechanical (GND) |
| C54 | 1 | 100nF | 0402 | Fab | Decoupling |
| C55 | 1 | 10µF | 0603 | Fab | Bulk capacitor |
| R18 | 1 | 0Ω | 0402 | Fab | Band ID → GND (VHF) |
| C58, C61 | 2 | 22pF C0G ±2% | 0402 | Fab | Harmonic LPF shunt (Murata GJM15) |
| C59, C60 | 2 | 39pF C0G ±2% | 0402 | Fab | Harmonic LPF shunt (Murata GJM15) |
| C62, C63 | — | DNP | 0402 | — | LPF tuning footprints (unpopulated) |
| L3, L5 | 2 | 68nH | 0603 wirewound | Fab | Harmonic LPF series (Coilcraft 0603HP) |
| L4 | 1 | 82nH | 0603 wirewound | Fab | Harmonic LPF series (Coilcraft 0603HP) |
| ANT | 1 | VHF stubby antenna | SMA male | Included | 144 MHz band |

### SA818-U Carrier (UHF, 400-480 MHz)

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| U10 | 1 | SA818U | Module (2mm pitch) | Fab | UHF transceiver |
| J19 | 1 | SMA edge-mount | Amphenol 132289 footprint | Fab | Female, 50Ω, LPF output |
| J18 | 1 | 1×8 male header | 2.54mm | User | Power/Audio (mounted on back) |
| J20 | 1 | 1×8 male header | 2.54mm | User | Control/Data (mounted on back) |
| J21 | 1 | 1×3 male header | 2.54mm | User | Mechanical (GND) |
| J22 | 1 | 1×3 male header | 2.54mm | User | Mechanical (GND) |
| C56 | 1 | 100nF | 0402 | Fab | Decoupling |
| C57 | 1 | 10µF | 0603 | Fab | Bulk capacitor |
| R18 | 1 | 10kΩ | 0402 | Fab | Band ID pull-up to SA818 VCC (UHF) |
| C64, C67 | 2 | 6.8pF C0G ±2% | 0402 | Fab | Harmonic LPF shunt (Murata GJM15) |
| C65, C66 | 2 | 12pF C0G ±2% | 0402 | Fab | Harmonic LPF shunt (Murata GJM15) |
| C68, C69 | — | DNP | 0402 | — | LPF tuning footprints (unpopulated) |
| L6, L8 | 2 | 22nH | 0603 wirewound | Fab | Harmonic LPF series (Coilcraft 0603HP) |
| L7 | 1 | 24nH | 0603 wirewound | Fab | Harmonic LPF series (Coilcraft 0603HP) |
| ANT | 1 | UHF stubby antenna | SMA male | Included | 430 MHz band |

### Carrier Header Pinouts

**J14 / J18 (Power/Audio) — mates with main badge J7:**
| Pin | Signal | Description |
|-----|--------|-------------|
| 1 | SA818_VCC | Power from main board load switch |
| 2 | GND | Ground |
| 3 | GND | Ground |
| 4 | MIC+ | TX audio input |
| 5 | GND | Ground |
| 6 | SPK+ | RX audio output |
| 7 | GND | Ground |
| 8 | GND | Ground |

**J15 / J20 (Control/Data) — mates with main badge J8:**
| Pin | Signal | Description |
|-----|--------|-------------|
| 1 | SA818_RX | UART RX (SA818 TXD→MCU) |
| 2 | SA818_TX | UART TX (MCU→SA818 RXD) |
| 3 | SA818_PTT | Push-to-talk (active low) |
| 4 | SA818_PD | Power down (active low) |
| 5 | SA818_HL | High/Low power select |
| 6 | SA818_SQ | Squelch output |
| 7 | SA818_ID | Band ID (GND=VHF, pulled up to SA818 VCC=UHF) |
| 8 | GND | Ground |

**J16, J17 / J21, J22 (Mechanical):** All pins → GND (extra ground connections for RF)

**Subtotal (per carrier):** ~$17.00
*(SA818 ~$10, SMA ~$2, antenna ~$4, PCB+parts ~$1)*

---

## Passive Component Totals

### Resistors

| Value | Qty | Package | Use |
|-------|-----|---------|-----|
| 0Ω | 1 | 0402 | VHF carrier band ID (R18) |
| 10kΩ | 5 | 0402 | UHF carrier band ID pull-up (R18), SD card pull-ups (R25–R28) |
| 100Ω | 2 | 0603 | PWR LED G/B (R15, R16) |
| 220Ω | 1 | 0603 | PWR LED R (R14) |
| 470Ω | 1 | 0603 | TX attenuator (R6) |
| 27Ω | 2 | 0402 | USB series termination (R20, R21) |
| 33Ω | 1 | 0402 | RP2350 VREG_AVDD filter (R22) |
| 1kΩ | 1 | 0603 | Backlight gate (R11) |
| 1kΩ | 2 | 0402 | BOOTSEL series resistor (R19), crystal series resistor (R29) |
| 2kΩ | 1 | 0603 | Charge current PROG1 (R3) |
| 4.7kΩ | 4 | 0603 | RX filter (R7, R8), I2C pullups (R1, R2) |
| 5.1kΩ | 2 | 0402 | USB CC (R_CC1, R_CC2) |
| 10kΩ | 3 | 0603 | Camera RST/PWDN (R12, R13), RUN pullup (R17) |
| 15kΩ | 1 | 0603 | Pre-charge/termination PROG3 (R4) |
| 47kΩ | 3 | 0603 | TX attenuator (R5), RX bias (R9, R10) |

### Thermistor

| Value | Qty | Package | Use |
|-------|-----|---------|-----|
| NTC 10k | 1 | 0603 | Temperature sensing, B=3380 (TH1) |

### Capacitors

| Value | Qty | Package | Use |
|-------|-----|---------|-----|
| 6.8pF | 2 | 0402 C0G | UHF carrier LPF (C64, C67) |
| 15pF | 2 | 0402 | Crystal load (C_Y1, C_Y2) |
| 12pF | 2 | 0402 C0G | UHF carrier LPF (C65, C66) |
| 22pF | 2 | 0402 C0G | VHF carrier LPF (C58, C61) |
| 39pF | 2 | 0402 C0G | VHF carrier LPF (C59, C60) |
| 4.7nF | 1 | 0603 | RX anti-alias (C27) |
| 10nF | 1 | 0603 | TX filter (C24) |
| 100nF | 42 | 0402/0603 | Decoupling: MCU (C3-C7, C9, C70, C72), LEDs (C28-C53), ICs (incl. C74, C75), carriers (C54, C56) |
| 1µF | 4 | 0402/0603 | RX DC block (C26), load switch input (C73), VBUS at connector (C_USB1), fuel gauge supply (C84) |
| 4.7µF | 3 | 0402 | RP2350 regulator (C1, C77, C78) |
| 4.7µF | 1 | 0603 | Charger input (C76) |
| 2.2µF | 2 | 0603 | PCM5102A charge pump (C21, C22) |
| 2.2µF | 1 | 0805 | Charger input (C10) |
| 10µF | 15 | 0603/0805 | Bulk caps throughout, LED rail (C79, C81, C82), carriers (C55, C57) |
| 100µF | 1 | 1206 | Load switch bulk (C17) |

### Inductors

| Value | Qty | Package | Use |
|-------|-----|---------|-----|
| 22nH | 2 | 0603 wirewound | UHF carrier LPF (L6, L8) |
| 24nH | 1 | 0603 wirewound | UHF carrier LPF (L7) |
| 68nH | 2 | 0603 wirewound | VHF carrier LPF (L3, L5) |
| 82nH | 1 | 0603 wirewound | VHF carrier LPF (L4) |
| 2.2µH | 2 | 3×3mm | TPS63001 buck-boost (L1), TPS63000 LED rail (L9) |
| 3.3µH | 1 | 0806 (2016) | RP2350 internal regulator (L2), Abracon AOTA-B201610S3R3-101-T |

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
- **OV2640 camera modules** — 1-3 weeks. **Must have an on-board clock oscillator; order one sample and test it first**
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
| **User (TH)** | ~$11 | Switches, modules, headers |
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
| 2.5 | 2025-02 | Added L2 (3.3µH) and C_DVDD (1µF) for RP2350 internal regulator |
| 3.0 | 2026-02 | **Synchronized with KiCad schematic**: Updated all ref designators to match KiCad, added R4 (15k PROG3), added C21/C22 (2.2µF charge pump), added J10 (SD card), fixed TH1 as NTC, removed PS/SYNC resistor (intentional), added I2C pullup warning |
| 3.1 | 2026-02 | **Added SA818 carrier boards**: VHF (U9, J13-J17, C54-C55, R18) and UHF (U10, J18-J22, C56-C57) schematics complete with header pinout tables, mechanical headers connected to GND |
| 3.2 | 2026-03 | **LED split**: WS2812B-5050 for ears (D2-D11), WS2812B-2020 for display border (D12-D27). **Removed SW6** (12mm photo button) — D-pad center doubles as capture. Camera connector J4 updated to 2×9 socket. |
| 3.3 | 2026-11 | **Carrier harmonic LPFs** added (VHF C58-C63/L3-L5, UHF C64-C69/L6-L8), UHF R18 = 10kΩ pull-up, SMA corrected to edge-mount, carrier header pinout corrected to the locked main badge order. **Camera module requirement**: OV2640 module must have its own on-board oscillator (badge supplies no XCLK). **Main badge review fixes**: added R19 (1kΩ BOOTSEL), R20/R21 (27Ω USB), C70/C72 (DVDD), C73 (load switch input), C74/C75 (DAC supply pins), C76 (charger input). |
| 3.4 | 2026-11 | **Schematic review fixes**: Q1 → AO3401A (backlight high-side), L1 → 2.2µH. **LED supply rail added**: U11 TPS63000, L9, C79–C83, R23, R24 (4.0V VLED for all 26 WS2812B). |
| 3.5 | 2026-11 | **Battery connector J1** changed from through-hole vertical JST-PH (user-soldered) to SMD side-entry S2B-PH-SM4-TB (fab-assembled), so its pins no longer come through under the display. |
| 3.6 | 2026-11 | **Small review items**: VBUS capacitance cut to about 7.9µF (C_USB1 10µF→1µF, C10 10µF→2.2µF); SD card socket J10 turned to face the board edge (card flush with the edge) and SD card pull-ups R25–R28 (10kΩ) added; six test pads TP1–TP6 added; BOOTSEL resistor R19 moved closer to the flash chip-select line. |
| 3.7 | 2026-11 | **PWR LED D1** changed from the right-angle APFA3010 to the flat-top AAA3528LSEEZGKQBKS (same LED chips, so R14–R16 are unchanged) and moved next to SAO connector J6. It now also indicates TX/RX. |
| 3.8 | 2026-11 | **Fuel gauge wiring corrected**: U7 supply moved from VSYS to the battery net (VBAT), its thermistor pin disconnected from the charger's NTC, ALARMB grounded, and C84 (1µF) added at its supply pin. |
| 3.9 | 2026-11 | **Crystal series resistor R29** (1kΩ, 0402) added between XOUT and Y1, as in the RP2350 hardware design guide. |
| 3.10 | 2026-11 | **Display module specified**: QDtech / LCDwiki MSP2401 (2.4" ILI9341, no touch). **Display support socket J23** (1×3, no connection) added under the module's SD header position, plus a 1×3 pin header to solder to the module. |

---

*This BOM is generated from KiCad schematic and verified against `docs/electrical-design.md`.*
