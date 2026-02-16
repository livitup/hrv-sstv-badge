# Electrical Design

This document describes the electrical design of the DEFCON SSTV Badge, including circuit schematics, component specifications, and pin assignments.

*For design rationale and trade-off analyses, see [Engineer's Notebook: Electrical Design Decisions](engineers-notebook/electrical-design-decisions.md).*

---

## Power System

### Architecture

The SA818 runs directly from the LiPo battery (via load switch), not through the 3.3V regulator. This separates the 1.7A TX current spikes from the main digital rail.

```
                    ┌───────────────────┐
    USB-C 5V ──────►│   MCP73871        │
                    │ Charge Controller │
                    └────────┬──────────┘
                             │
                             ▼
                    ┌─────────────────┐
                    │  2000mAh LiPo   │
                    │   3.0V - 4.2V   │
                    └───────┬─────────┘
                            │
              ┌─────────────┼─────────────┐
              │             │             │
              ▼             ▼             ▼
        ┌──────────┐  ┌──────────┐  ┌──────────┐
        │ LC709203 │  │ TPS63001 │  │ TPS22919 │
        │Fuel Gauge│  │Buck-Boost│  │ Load SW  │
        └──────────┘  └────┬─────┘  └────┬─────┘
                           │             │
                      3.3V Rail      SA818 Direct
                           │         (3.5-5.5V)
                           ▼             │
                    ┌──────────────┐     │
                    │ RP2350       │     │
                    │ Display      │     ▼
                    │ Camera       │  ┌──────────┐
                    │ SD Card      │  │  SA818   │
                    │ Codec        │  │ 1W Radio │
                    └──────────────┘  └──────────┘
```

### Load Analysis

**3.3V Rail loads (excluding SA818):**

| Subsystem | Current | Notes |
|-----------|---------|-------|
| RP2350 | ~50mA | Varies with clock/activity |
| Display + backlight | ~50-80mA | Backlight dominates |
| Camera (active) | ~50mA | Only when capturing |
| SD card (active) | ~50mA | Burst during write |
| PCM5102A DAC | ~10mA | |
| Misc (LEDs, etc.) | ~20mA | |
| **Total typical** | **~250mA** | |
| **Peak** | **~400mA** | All subsystems active |

**SA818 power (separate rail):**

| Mode | Current |
|------|---------|
| Standby | ~20mA |
| RX active | ~50mA |
| TX (1W) | ~1.5-1.7A |

---

## Buck-Boost Converter (TPS63001)

*See [Engineer's Notebook](engineers-notebook/electrical-design-decisions.md#buck-boost-regulator-selection) for regulator selection rationale.*

### TPS63001 Specifications

| Spec | Value |
|------|-------|
| Input | 1.8V - 5.5V |
| Output | Fixed 3.3V |
| Max Current | 1.2A |
| Peak Efficiency | ~93% |
| Package | 3x3mm QFN-10 |

### External Components

| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| L1 | 1µH | 3x3mm | Coilcraft XAL3030 or similar |
| C_IN1 | 10µF | 0805 | X5R/X7R, 10V |
| C_IN2 | 10µF | 0805 | X5R/X7R, 10V |
| C_OUT1 | 10µF | 0805 | X5R/X7R, 10V |
| C_OUT2 | 10µF | 0805 | X5R/X7R, 10V |
| R_PS | 1MΩ | 0402 | Power save threshold (optional) |

---

## SA818 Load Switch

### Purpose

The TPS22919 load switch provides:
- MCU control over SA818 power (airplane mode, sleep)
- Isolation of 1.7A TX current spikes from main rail
- Clean power sequencing

### TPS22919 Specifications

| Parameter | Value |
|-----------|-------|
| Input voltage | 1.0V - 5.5V |
| Continuous current | 1.5A |
| Peak current | 3A |
| On resistance | 90mΩ typical |
| Package | SOT-23-5 |
| Price | ~$0.50 |

### Implementation

```
    VBAT (from LiPo) ──────┬──────────────────────►  To TPS63001
                           │
                           │
                      ┌────┴────┐
                      │         │
                VIN ──┤ TPS22919├── VOUT ──┬──► To SA818 VCC
                      │         │          │
              GPIO ──►┤ ON/EN   │     C1 ══╪══ 100µF (bulk)
                      │         │          │
                GND ──┤ GND     │     C2 ══╪══ 10µF (bypass)
                      │         │          │
                      └─────────┘         GND
```

### Control Logic

| GPIO State | TPS22919 | SA818 | Mode |
|------------|----------|-------|------|
| HIGH | ON | Powered | Normal operation |
| LOW | OFF | Unpowered | Airplane mode / sleep |

---

## USB-C Interface

### Overview

The USB-C connector provides two functions:
1. **Power input** — 5V from USB to MCP73871 battery charger
2. **Data** — D+/D- to RP2350 for programming and UF2 bootloader

### Circuit Design

```
                                          ┌─────────────────────┐
                                          │   USB-C Receptacle  │
                                          │   (Through-hole)    │
                                          │                     │
    To MCP73871 VBUS ◄────────┬───────────┤ VBUS (A4,B4,A9,B9)  │
                              │           │                     │
                         ┌────┴────┐      │                     │
                         │PESD5V0  │      │                     │
                         │(TVS)    │      │                     │
                         └────┬────┘      │                     │
                              │           │                     │
                             GND          │                     │
                                          │                     │
                         ┌─────────────┐  │                     │
    To RP2350 USB_DP ◄───┤ USBLC6-2SC6 │◄─┤ D+ (A6,B6)          │
    To RP2350 USB_DM ◄───┤ (ESD TVS)   │◄─┤ D- (A7,B7)          │
                         └─────────────┘  │                     │
                                          │                     │
                              ┌───────────┤ CC1 (A5)            │
                              │           │                     │
                         5.1kΩ resistor   │                     │
                              │           │                     │
                              ├───────────┤ CC2 (B5)            │
                              │           │                     │
                         5.1kΩ resistor   │                     │
                              │           │                     │
                             GND          │                     │
                                          │                     │
                             GND ─────────┤ GND (A1,B1,A12,B12) │
                                          │                     │
                                          │ SBU1, SBU2 (NC)     │
                                          │ TX1/RX1 pairs (NC)  │
                                          │ TX2/RX2 pairs (NC)  │
                                          └─────────────────────┘
```

### Component Selection

| Component | Part Number | Package | Purpose | Est. Cost |
|-----------|-------------|---------|---------|-----------|
| USB-C Receptacle | USB4105-GF-A (GCT) | SMT+TH hybrid | Power + data connector (USB 2.0) | ~$1.00 |
| VBUS TVS | PESD5V0S1BL | SOD-323 | Surge/ESD protection on 5V | ~$0.10 |
| Data ESD | USBLC6-2SC6 | SOT-23-6 | TVS array for D+/D- | ~$0.20 |
| CC Resistors | 2× 5.1kΩ | 0402 | UFP (sink) identification | ~$0.02 |
| Bulk Capacitor | 10µF ceramic | 0603 | VBUS decoupling | ~$0.05 |
| **Total** | | | | **~$1.17** |

### Design Notes

**CC Resistors (5.1kΩ):**
- Required on both CC1 and CC2 to identify as a USB sink (UFP)
- Tells USB-C sources to provide 5V at up to 1.5A (default USB-C power)
- No USB-PD negotiation needed — simple 5V charging is sufficient

**ESD Protection:**
- USBLC6-2SC6 protects D+/D- with fast TVS diodes
- PESD5V0S1BL on VBUS protects against surges from cheap chargers and ESD
- Both are cheap insurance for a badge that gets plugged into random USB ports

**RP2350 USB Connection:**
- RP2350 has dedicated USB_DP and USB_DM pins (not GPIO)
- Built-in USB 1.1 Full Speed PHY
- Handles cable orientation automatically (symmetric D+/D- routing)
- Supports UF2 bootloader for drag-and-drop firmware updates

**Through-hole Connector:**
- More robust than SMD for a wearable badge
- Withstands repeated plug/unplug cycles better
- Easier hand-soldering for kit assembly

**VBUS Routing:**
- Direct to MCP73871 VBUS input
- 10µF bulk capacitor near connector
- MCP73871 handles charge current limiting and battery management

### USB-C Pinout (16-pin)

| Pin | Name | Connection |
|-----|------|------------|
| A1, B1, A12, B12 | GND | Ground |
| A4, B4, A9, B9 | VBUS | To MCP73871 + TVS |
| A5 | CC1 | 5.1kΩ to GND |
| B5 | CC2 | 5.1kΩ to GND |
| A6, B6 | D+ | To USBLC6-2 → RP2350 USB_DP |
| A7, B7 | D- | To USBLC6-2 → RP2350 USB_DM |
| A2, A3, B2, B3 | TX/RX | NC (USB 2.0 only) |
| A8, B8 | SBU | NC |

*See [Engineer's Notebook](engineers-notebook/electrical-design-decisions.md#usb-c-interface) for alternate part options.*

---

## Battery Management

### Overview

The battery system provides portable power with USB charging and accurate state-of-charge monitoring.

| Component | Part | Function |
|-----------|------|----------|
| Battery | 2000mAh LiPo pouch cell | Energy storage |
| Charger | MCP73871-2CCI/ML | Charge control + load sharing |
| Fuel Gauge | LC709203FQH-01TWG | State-of-charge monitoring |
| Connector | JST-PH 2-pin | Battery connection |

### System Block Diagram

```
                                    ┌─────────────────────────────┐
    USB-C VBUS ────────────────────►│        MCP73871             │
    (5V from host)                  │   Charge Controller         │
                                    │                             │
                                    │  IN ──────► VBAT ──────► OUT│
                                    │             │               │
                                    │  STAT1  STAT2  PG           │
                                    └───┬───────┬────┬────────────┘
                                        │       │    │        │
                                   (to MCU for  │    │        │
                                    status LEDs)│    │        ▼
                                                │    │   ┌─────────┐
                      ┌─────────────────────────┼────┼───┤  LiPo   │
                      │                         │    │   │ 2000mAh │
                      │                         │    │   └────┬────┘
                      │   ┌─────────────────────┘    │        │
                      │   │                          │        │
                      ▼   ▼                          │        │
                  ┌───────────┐                      │        │
                  │ LC709203F │◄─────────────────────┼────────┘
                  │Fuel Gauge │  (monitors battery)  │
                  └─────┬─────┘                      │
                        │ I2C                        │
                        ▼                            ▼
                    RP2350                    To TPS63001 + TPS22919
                  (SoC reading)                  (power rails)
```

### MCP73871 Charge Controller

*See [Engineer's Notebook](engineers-notebook/electrical-design-decisions.md#why-mcp73871) for selection rationale.*

#### MCP73871 Circuit

```
                                   MCP73871-2CCI/ML
                                  ┌────────────────────┐
                                  │                    │
    USB VBUS (5V) ───┬───[C1]────►│ IN          VBAT   ├───┬──► To Battery +
                     │            │                    │   │
                   [C2]           │              PROG  ├───┼──[R_PROG]──► GND
                     │            │                    │   │
                    GND           │              THERM ├───┼──► Thermistor (NTC)
                                  │                    │   │
    System Load ◄────────────────►│ OUT          STAT1 ├───┼──► To RP2350 (Charging)
    (to TPS63001)                 │                    │   │
                                  │              STAT2 ├───┼──► To RP2350 (Charged/Fault)
                                  │                    │   │
                                  │                PG  ├───┼──► To RP2350 (Power Good)
                                  │                    │   │
                                  │          SEL/TIMER ├───┼──[R_SEL]──► GND or VCC
                                  │                    │   │
                                  │       PROG2/TE_N   ├───┼──► VCC (disable timer)
                                  │                    │   │
                                  │              CE_N  ├───┴──► GND (always enabled)
                                  │                    │
                                  │               VSS  ├──────► GND
                                  │                    │
                                  └────────────────────┘
```

#### Key Component Values

| Ref | Value | Purpose | Calculation |
|-----|-------|---------|-------------|
| R_PROG | 2kΩ | Sets charge current | I_CHG = 1000V / R_PROG = 500mA |
| R_SEL | 0Ω to GND | Selects USB input mode | USB 500mA mode |
| C_IN | 10µF | Input decoupling | X5R/X7R, 10V min |
| C_VBAT | 10µF | Battery decoupling | X5R/X7R, 10V min |
| C_OUT | 10µF | Output decoupling | X5R/X7R, 10V min |
| R_THERM | 10kΩ NTC | Battery temperature | Standard 10K NTC, B=3380 |

**Charge current selection:**
- R_PROG = 2kΩ → 500mA charge current (safe for 2000mAh, ~0.25C)
- Charge time: ~4-5 hours from empty
- Could use 1kΩ for 1A charging (~2.5 hours) if faster charging needed

#### Status Outputs

| STAT1 | STAT2 | Meaning |
|-------|-------|---------|
| LOW | HIGH | Charging in progress |
| HIGH | LOW | Charge complete |
| HIGH | HIGH | No battery / Sleep / Disabled |
| LOW | LOW | Fault (over-temp, timer, etc.) |

| PG | Meaning |
|----|---------|
| LOW | Input power good (USB connected) |
| HIGH | No input power (battery only) |

**Note:** Status pins are active-low open-drain. Use internal pullups on RP2350 GPIOs.

#### MCP73871 BOM

| Ref | Qty | Value | Package | Notes |
|-----|-----|-------|---------|-------|
| U_CHG | 1 | MCP73871-2CCI/ML | QFN-20 (4×4mm) | Charge controller |
| C_IN | 1 | 10µF | 0805 | Input cap, X5R 10V |
| C_VBAT | 1 | 10µF | 0805 | Battery cap, X5R 10V |
| C_OUT | 1 | 10µF | 0805 | Output cap, X5R 10V |
| R_PROG | 1 | 2kΩ | 0603 | Charge current (500mA) |
| R_SEL | 1 | 0Ω | 0402 | USB 500mA mode |
| R_THERM | 1 | 10kΩ NTC | 0603 | Battery temp sensor |

**Subtotal:** ~$3.00

### LC709203F Fuel Gauge

*See [Engineer's Notebook](engineers-notebook/electrical-design-decisions.md#why-lc709203f) for selection rationale.*

#### LC709203F Circuit

```
                               LC709203F
                              ┌───────────────┐
                              │               │
    Battery + ────────────────┤ VCC      ALRM ├───► To RP2350 (optional alert)
                              │               │
    Battery - (GND) ──────────┤ VSS      SDA  ├───► I2C SDA (GPIO0)
                              │               │
    NTC Thermistor ───────────┤ THERMISTOR    │
         │                    │          SCL  ├───► I2C SCL (GPIO1)
        [10kΩ NTC]            │               │
         │                    │       TSENSE  ├───► GND (internal temp sense)
        GND                   │               │
                              └───────────────┘
```

#### LC709203F Configuration

| Parameter | Setting | Notes |
|-----------|---------|-------|
| I2C Address | 0x0B | Fixed |
| Battery Profile | APA = 0x0036 | For 2000mAh LiPo (adjust per datasheet) |
| Thermistor B | 0x0D34 | For 10K NTC, B=3380 |
| Change of Parameter | 0x0001 | 1-cell, 3.7V nominal |

**Firmware initialization:**
1. Write APA (Adjustment Pack Application) for battery capacity
2. Write thermistor B value
3. Set Change of Parameter for cell configuration
4. Read RSOC (Relative State of Charge) register for battery %

#### Key Registers

| Register | Address | Description |
|----------|---------|-------------|
| RSOC | 0x0D | Relative State of Charge (0-100%) |
| Cell Voltage | 0x09 | Battery voltage in mV |
| Cell Temp | 0x08 | Temperature (via thermistor) |
| Alarm RSOC | 0x13 | Low battery threshold |
| IC Version | 0x11 | Chip version (verify communication) |

#### LC709203F BOM

| Ref | Qty | Value | Package | Notes |
|-----|-----|-------|---------|-------|
| U_FUEL | 1 | LC709203FQH-01TWG | WDFN-8 (2×2.5mm) | Fuel gauge |
| C_FUEL | 1 | 100nF | 0402 | Decoupling |
| R_NTC | 1 | 10kΩ NTC | 0603 | Shared with MCP73871 |

**Subtotal:** ~$2.50

**Note:** The 10K NTC thermistor is shared between MCP73871 and LC709203F. Both ICs read the same temperature sensor.

### Battery Connector

| Ref | Value | Notes |
|-----|-------|-------|
| J_BAT | JST-PH 2-pin | Standard LiPo connector |

**Polarity:** Red (+) on pin 1, Black (-) on pin 2. Match common LiPo battery wiring.

### Thermistor Placement

The NTC thermistor should be placed:
- Close to the battery (within 10mm if possible)
- On the main PCB near the battery pad/connector
- Away from heat sources (buck-boost inductor, SA818)

For pouch cells, the thermistor can be taped directly to the battery surface for better accuracy.

### Complete Battery Schematic

```
    USB VBUS (5V)                          ┌────────────────────────────────────────┐
         │                                 │                                        │
         │                                 │        MCP73871                        │
         │                                 │       ┌──────────┐                     │
         └─────────────────────────────────┼──────►│ IN       │                     │
                                           │       │          │                     │
                                           │   ┌───┤ OUT      ├──────────────────►  │
                                           │   │   │          │   To TPS63001       │
                                           │   │   │    VBAT  ├───┬──────────────►  │
                                           │   │   │          │   │  To TPS22919    │
              ┌────────────────────────────┼───┼───┤ STAT1    │   │                 │
              │                            │   │   │          │   │   ┌─────────┐   │
              │  ┌─────────────────────────┼───┼───┤ STAT2    │   └───┤  LiPo   │   │
              │  │                         │   │   │          │       │ 2000mAh │   │
              │  │  ┌──────────────────────┼───┼───┤ PG       │       └────┬────┘   │
              │  │  │                      │   │   │          │            │        │
              │  │  │                      │   │   │   THERM  ├────────────┼───┐    │
              │  │  │                      │   │   └──────────┘            │   │    │
              │  │  │                      │   │                           │   │    │
              ▼  ▼  ▼                      │   │                           │   │    │
           RP2350                          │   │   LC709203F               │   │    │
           GPIO                            │   │  ┌──────────┐             │   │    │
        (status)                           │   │  │          │             │   │    │
                                           │   └──┤ VCC      │◄────────────┘   │    │
              ┌────────────────────────────┼──────┤ SDA      ├──► I2C SDA      │    │
              │                            │      │          │                 │    │
              └────────────────────────────┼──────┤ SCL      ├──► I2C SCL      │    │
                                           │      │          │                 │    │
                                           │      │THERMISTOR├─────────────────┤    │
                                           │      └──────────┘                 │    │
                                           │                               ┌───┴───┐│
                                           │                               │10K NTC││
                                           │                               └───┬───┘│
                                           │                                   │    │
                                           └───────────────────────────────────┴────┘
                                                                              GND
```

### GPIO Usage for Battery Status

| GPIO | Function | Notes |
|------|----------|-------|
| TBD | STAT1 | Charging indicator (active low) |
| TBD | STAT2 | Charge complete (active low) |
| TBD | PG | Power good (active low) |
| 0 | I2C SDA | LC709203F fuel gauge (shared) |
| 1 | I2C SCL | LC709203F fuel gauge (shared) |

**Note:** STAT1/STAT2/PG are not connected to GPIOs. Charge state is inferred from LC709203F I2C readings. See [Engineer's Notebook](engineers-notebook/electrical-design-decisions.md#skipping-dedicated-status-gpios) for rationale.

### Battery Selection

**Requirements:**
- Capacity: 2000mAh (target runtime: 10+ hours idle)
- Chemistry: Li-Po (single cell, 3.7V nominal)
- Form factor: Pouch cell, fits badge footprint
- Connector: JST-PH 2-pin
- Protection: Built-in PCM (protection circuit module) preferred

**Recommended dimensions:** ~50mm × 35mm × 8mm (typical 2000mAh pouch)

**Common part numbers:**
- 503759 (50mm × 37mm × 5.9mm, 2000mAh)
- 103450 (10mm × 34mm × 50mm, 1800-2000mAh)

### Safety Considerations

1. **Over-discharge protection:** MCP73871 + battery PCM
2. **Over-charge protection:** MCP73871 (4.2V cutoff) + battery PCM
3. **Over-current protection:** Battery PCM
4. **Thermal protection:** MCP73871 thermal regulation + NTC monitoring
5. **Reverse polarity:** Add reverse protection diode or MOSFET (optional, increases cost)

**Recommendation:** Use batteries with built-in protection circuit (PCM). This is standard for quality LiPo cells and provides a second layer of safety.

---

## Audio Interface

### Overview

The audio interface uses a PCM5102A DAC for transmit and the RP2350 internal ADC for receive.

*See [Engineer's Notebook](engineers-notebook/electrical-design-decisions.md#why-pcm5102a--adc-not-wm8960-codec) for codec selection rationale.*

### Audio Signal Flow

```
    ┌─────────────────────────────────────────────────────────────────────────┐
    │                              MAIN BADGE                                 │
    │                                                                         │
    │   RP2350                              PCM5102A                          │
    │  ┌───────┐                           ┌─────────┐                        │
    │  │       │ GPIO(BCK)  ──────────────→│ BCK     │                        │
    │  │       │ GPIO(LRCK) ──────────────→│ LRCK    │                        │
    │  │       │ GPIO(DIN)  ──────────────→│ DIN     │    TX                  │
    │  │       │                           │         │   Filter    To SA818   │
    │  │       │                           │  VOUTL ─┼────────────→ MIC+/MIC- │
    │  │       │                           └─────────┘              (carrier) │
    │  │       │                                                              │
    │  │       │                              RX                   From SA818 │
    │  │       │ ADC GPIO ←──────────────── Filter ←───────────── SPK+/SPK-   │
    │  │       │                                                   (carrier)  │
    │  └───────┘                                                              │
    │                                                                         │
    └─────────────────────────────────────────────────────────────────────────┘
```

### Signal Levels

| Signal | Level | Notes |
|--------|-------|-------|
| PCM5102A output | ~3V peak-to-peak | Full-scale DAC output |
| SA818 MIC input | ~10-50mV | Needs ~40dB attenuation |
| SA818 SPK output | ~300-500mV | AC coupled |
| RP2350 ADC input | 0-3.3V | Needs DC bias at 1.65V |

---

## TX Path: PCM5102A DAC Circuit

### PCM5102A Connections

```
                                        VCC (3.3V)
                                            │
                              ┌─────────────┼─────────────┐
                              │             │             │
                         C1 ══╪══ 10µF  C2 ══╪══ 100nF    │
                              │             │             │
    RP2350                    │             │             │
    ──────────────────────────┼─────────────┼─────────────┼──────────
                              │             │             │
    GPIO (BCK)  ──────────────┼──→ BCK      │             │
                              │             │             │
    GPIO (LRCK) ──────────────┼──→ LRCK     │             │
                              │      ┌──────┴──────┐      │
    GPIO (DIN)  ──────────────┼──→ DIN             │      │
                              │      │  PCM5102A   │      │
                              │      │             │      │
                         VCC ─┼──→ VCC    AVDD ←── ┼── VCC
                              │      │             │
                         VCC ─┼──→ DVDD            │
                              │      │             │
                        3.3V ─┼──→ CPVDD           │
                              │      │             │
                         GND ─┼──→ GND             │
                              │      │             │
                              │      │   VOUTL ─── ┼──→ To TX Filter
                              │      │             │
                              │      │   VOUTR ─── ┼──→ NC
                              │      │             │
    Config pins (directly):   │      │             │
                              │      │             │
                         GND ─┼──→ FMT             │      (I2S format)
                         GND ─┼──→ DEMP            │      (de-emphasis off)
                         GND ─┼──→ FLT             │      (normal filter)
                         VCC ─┼──→ XSMT            │      (soft mute off)
                              │      │             │
                              │      └─────────────┘
                              │
    ──────────────────────────┴──────────────────────────────────────
```

**Key points:**
- No I2C needed — configuration is via pin strapping
- FMT, DEMP, FLT tied to GND; XSMT tied to VCC
- Only using left channel output (VOUTL)

### TX Output Filter & Attenuator

The PCM5102A outputs ~3V peak-to-peak. The SA818 MIC input wants ~30mV. We need:
1. **Attenuation** (~100:1 = -40dB)
2. **Low-pass filtering** (remove DAC high-frequency artifacts)
3. **DC blocking** (only pass AC audio)

```
    PCM5102A                                                    To SA818
    VOUTL (~3Vpp)                                               MIC+ (~30mVpp)
       │                                                            │
       │      R1              R2              C4                    │
       ├────[47kΩ]────┬────[470Ω]────┬──────┤├──────────────────────┤
       │              │              │     100nF                    │
       │         C3 ══╪══ 10nF       │     (DC block)               │
       │              │              │                              │
       │             GND            GND                             │
       │                                                            │
      GND ──────────────────────────────────────────────────────── MIC-
```

**How it works:**

| Component | Function | Value |
|-----------|----------|-------|
| R1 + R2 | Voltage divider: 470/(47k+470) ≈ 1/100 | -40dB attenuation |
| C3 | Filter cap with R2, blocks high frequencies | fc ≈ 34kHz |
| C4 | DC blocking, only passes AC audio | 100nF |

**Output level:** 3Vpp × 0.01 = 30mVpp ✓

---

## RX Path: RP2350 ADC Circuit

### Anti-Alias Filter + Bias Network

The SA818 SPK output is ~300-500mV AC centered at 0V. The RP2350 ADC needs:
1. **DC blocking** (remove any offset from SA818)
2. **Anti-alias filtering** (block frequencies above ~7kHz)
3. **Bias** (shift signal to 1.65V center)

```
    From SA818                                              To RP2350
    SPK+ (~400mVpp)                                         ADC GPIO
       │                                                       │
       │     C5            R3             R4                   │
       ├────┤├────────────[4.7kΩ]───┬────[4.7kΩ]───────────────┤
       │   1µF                      │                          │
       │   (DC block)          C6 ══╪══ 4.7nF                  │
       │                            │  (anti-alias)            │
       │                           GND                         │
       │                                                       │
       │                      R5 [47kΩ]──── VCC (3.3V)         │
       │                            │                          │
       │                            ├──────────────────────────┘
       │                            │  (bias = 1.65V)
       │                      R6 [47kΩ]
       │                            │
       │                           GND
       │
      SPK- ─────────────────────────────────────────────────── GND
```

**How it works:**

| Component | Function | Value |
|-----------|----------|-------|
| C5 | DC block — removes SA818 offset, passes audio | 1µF |
| R3 + R4 + C6 | Anti-alias filter, fc ≈ 7.2kHz | Blocks frequencies that cause aliasing |
| R5 + R6 | Bias divider, creates 1.65V center point | Shifts AC into ADC range |

**Anti-alias cutoff:** fc = 1/(2π × 4.7k × 4.7nF) ≈ 7.2kHz

This is above the SSTV maximum (~2.3kHz) but below the Nyquist limit for a ~20kHz sample rate.

---

## Complete Audio Schematic

```
    ┌─────────────────────────────────────────────────────────────────────────┐
    │                                                                         │
    │   RP2350                              PCM5102A                          │
    │  ┌───────┐                           ┌─────────┐                        │
    │  │       │ GPIO(BCK)  ──────────────→│ BCK     │                        │
    │  │       │ GPIO(LRCK) ──────────────→│ LRCK    │                        │
    │  │       │ GPIO(DIN)  ──────────────→│ DIN     │                        │
    │  │       │                           │         │                        │
    │  │       │                           │  VOUTL ─┼──┐                     │
    │  │       │                           └─────────┘  │                     │
    │  │       │                                        │                     │
    │  │       │          ┌─────────────────────────────┘                     │
    │  │       │          │                                                   │
    │  │       │          │    R1          R2         C4                      │
    │  │       │          └──[47kΩ]──┬──[470Ω]──┬────┤├─────────────→ MIC+    │
    │  │       │                     │          │   100nF            (to J1)  │
    │  │       │                C3 ══╪══       GND                            │
    │  │       │                    10nF                                      │
    │  │       │                     │                                        │
    │  │       │                    GND                              MIC- ←GND│
    │  │       │                                                     (to J1)  │
    │  │       │                                                              │
    │  │       │                                                              │
    │  │       │          ┌───────────────────────────────────────── SPK+     │
    │  │       │          │                                         (from J1) │
    │  │       │          │    C5         R3          R4                      │
    │  │       │          └───┤├───────[4.7kΩ]──┬──[4.7kΩ]──┬────────────┐    │
    │  │       │             1µF                │           │            │    │
    │  │       │                           C6 ══╪══        ─┼─ Bias      │    │
    │  │       │                              4.7nF        ┌┴┐           │    │
    │  │       │                                │     R5   │ │ 47kΩ      │    │
    │  │       │                               GND   VCC───┤ │           │    │
    │  │       │                                           └┬┘           │    │
    │  │       │                                            ├────────────┘    │
    │  │       │                                           ┌┴┐                │
    │  │       │ ADC GPIO ←────────────────────────────────┤ │ R6 47kΩ        │
    │  │       │                                           └┬┘                │
    │  └───────┘                                           GND                │
    │                                                                         │
    │                                                       SPK- ← GND        │
    │                                                       (from J1)         │
    └─────────────────────────────────────────────────────────────────────────┘
```

---

## Audio BOM

| Ref | Value | Package | Assembly | Notes |
|-----|-------|---------|----------|-------|
| U_DAC | PCM5102A | TSSOP-20 | Fab/hot air | I2S DAC |
| C1 | 10µF | 0805 | Fab/hot air | DVDD decoupling |
| C2 | 100nF | 0603 | Fab/hot air | AVDD decoupling |
| R1 | 47kΩ | 0603 | User | TX attenuator |
| R2 | 470Ω | 0603 | User | TX attenuator |
| C3 | 10nF | 0603 | User | TX filter |
| C4 | 100nF | 0603 | User | TX DC block |
| C5 | 1µF | 0603 | User | RX DC block |
| R3 | 4.7kΩ | 0603 | User | RX filter |
| R4 | 4.7kΩ | 0603 | User | RX filter |
| C6 | 4.7nF | 0603 | User | RX anti-alias |
| R5 | 47kΩ | 0603 | User | RX bias |
| R6 | 47kΩ | 0603 | User | RX bias |

**Total cost:** ~$2.05 (PCM5102A ~$1.75 + passives ~$0.30)

**User-solderable:** All 0603 resistors and capacitors (R1-R6, C3-C6).

**Fab/hot air:** PCM5102A (TSSOP-20) and its decoupling caps (C1, C2).

---

## SA818 Carrier Board Design

### Overview

The carrier board is a full-width PCB that overlays the top portion of the main badge. It contains the SA818 module, SMA connector, and antenna — keeping **all RF circuitry on the carrier** and completely off the main badge.

*See [Engineer's Notebook](engineers-notebook/electrical-design-decisions.md#why-a-separate-carrier-board) for carrier board rationale.*

### Physical Arrangement

```
    FRONT VIEW (worn on chest, looking at badge)

    ┌────────────────────────────────────────┐ ─┬─ Top edge
    │┌──────────────────────────────────────┐│  │  (both boards flush)
    ││           CARRIER BOARD              ││  │
    ││  (SA818, SMA, antenna - visible)     ││  │  ~25mm carrier height
    │└──────────────────────────────────────┘│  │
    │                                        │ ─┴─
    │                                        │
    │           MAIN BADGE                   │
    │    (display, buttons, etc - visible)   │
    │                                        │
    │                                        │
    └────────────────────────────────────────┘


    SIDE VIEW (cross-section)

                        antenna
                           ↑
                      ┌────┴───┐
         CARRIER  ════╪════════╪════  ← In FRONT of main badge
                      │        │
         MAIN     ════╪════════╪════  ← Behind carrier
                      └────────┘
                    headers connect
                    (pointing backward)
```

### SA818 Pinout Reference

| Pin | Name | Function |
|-----|------|----------|
| 1 | VCC | Power 3.5-5.5V |
| 2 | GND | Ground |
| 3 | MIC+ | Audio input + |
| 4 | MIC- | Audio input - |
| 5 | SPK+ | Audio output + |
| 6 | SPK- | Audio output - |
| 7 | PTT | Push-to-talk (active low) |
| 8 | PD | Power down (active low) |
| 9 | HL | High/Low power select |
| 10 | SQ | Squelch detect output |
| 11 | TXD | UART TX (module → MCU) |
| 12 | RXD | UART RX (MCU → module) |
| 13-15 | NC | Not connected |
| 16 | GND | Ground |

### Carrier Board Layout

```
                              ← 120mm (full badge width) →

    ┌──────────────────────────────────────────────────────────────────┐
    │○ ○ ○                                                        ○ ○ ○│ ─┬─
    │J3 (NC)                                                    J4 (NC)│  │
    │mech only            ┌──────┐                              mech   │  │
    │                     │ SMA  │ ← right-angle connector             │  │
    │                     │  ↑   │   antenna points UP                 │  │
    │                     └──┬───┘   (tilt awat slightly at assembly)  │ ~25mm
    │                        │                                         │  │
    │  ┌─────────────────────┴───────────────┐                         │  │
    │  │            50Ω trace                │                         │  │
    │  │   ┌───────────────────────────┐     │    C1 ▪ 100nF           │  │
    │  │   │                           │     │    C2 ▪ 10µF            │  │
    │  │   │       SA818-V/U           │     │                         │  │
    │  │   │      (horizontal)         │     │    R_ID ▪ (band select) │  │
    │  │   │                           │     │                         │  │
    │  │   └───────────────────────────┘     │                         │  │
    │  │            GND via fence            │                         │  │
    │  └─────────────────────────────────────┘                         │  │
    │                                                                  │ ─┴─
    │  ○ ○ ○ ○ ○ ○ ○ ○                            ○ ○ ○ ○ ○ ○ ○ ○      │
    │  J1 (1x8)                                    J2 (1x8)            │
    │  signal header                              signal header        │
    └──────────────────────────────────────────────────────────────────┘
                  │                                      │
                  ▼                                      ▼
    ════════════════════════════════════════════════════════════════════
                         MAIN BADGE (top edge, behind carrier)

    Component side faces FORWARD (visible when worn)
    Headers point BACKWARD into main badge sockets
```

### Connector Placement (for mechanical stability)

| Element | Position from left edge | Purpose |
|---------|------------------------|---------|
| J3 (top-left) | 5-13mm | 1x3 mechanical, NC |
| J1 (bottom-left) | 10-30mm | 1x8 signal (power + audio) |
| SMA connector | ~55-65mm | Center-right |
| J2 (bottom-right) | 90-110mm | 1x8 signal (control + data) |
| J4 (top-right) | 107-115mm | 1x3 mechanical, NC |

The 4-point mounting (J1, J2 at bottom + J3, J4 at top corners) prevents wobble and keeps the carrier securely attached.

### Carrier Header Pinouts

**J1 (Left, 1x8) — Power and Audio:**

| Pin | Signal | Notes |
|-----|--------|-------|
| 1 | VCC | From TPS22919 on main board |
| 2 | GND | Power ground |
| 3 | GND | Audio ground |
| 4 | MIC+ | Audio to SA818 |
| 5 | MIC- | Audio to SA818 |
| 6 | SPK+ | Audio from SA818 |
| 7 | SPK- | Audio from SA818 |
| 8 | NC | Spare |

**J2 (Right, 1x8) — Control and Data:**

| Pin | Signal | Notes |
|-----|--------|-------|
| 1 | TXD | SA818 → MCU |
| 2 | RXD | MCU → SA818 |
| 3 | PTT | Push-to-talk |
| 4 | PD | Power down |
| 5 | HL | High/low power |
| 6 | SQ | Squelch detect |
| 7 | ID | Band detect |
| 8 | GND | Signal ground |

**J3, J4 (Top corners, 1x3 each) — Mechanical only:**

All pins NC. These provide physical stability only — no electrical connection.

### Band Auto-Detection

Instead of a DIP switch, the carrier includes a band ID resistor:

| Module | ID Pin State | Implementation |
|--------|--------------|----------------|
| SA818-V (VHF) | LOW | 0Ω resistor to GND on carrier |
| SA818-U (UHF) | HIGH | 0Ω resistor to VCC on carrier |
| No module | Floating | MCU pulldown, detect via UART timeout |

Firmware reads ID pin on startup and configures frequency limits accordingly.

### RF Section (On Carrier)

All RF design is contained on the carrier board:

**50Ω Trace:**
- SA818 ANT pin to SMA connector
- Keep as short as possible (~15-20mm)
- 0.36mm (14 mil) width for 50Ω on standard 4-layer stackup
- Or calculate for 2-layer if carrier is simpler

**Ground Via Stitching:**
```
    SA818 ANT
         │
    ○  ○ ┃ ○  ○      ← Via fence on both sides
         ┃
    ○  ○ ┃ ○  ○        0.3mm drill, 1mm spacing
         ┃
    ○  ○ ┃ ○  ○
         ┃
      ┌──┴──┐
      │ SMA │
      └─────┘
```

**SMA Connector:**
- Right-angle, PCB mount
- Position at top edge, center-right
- Antenna points UP when badge is worn
- Slight backward tilt (~10-15°) achieved during assembly by not fully seating top header pins

### Included Antenna

Each carrier ships with a matched stubby antenna:

| Carrier | Antenna | Approximate Length |
|---------|---------|-------------------|
| SA818-V (VHF) | 2m stubby whip | 15-20cm (6-8") |
| SA818-U (UHF) | 70cm stubby whip | 8-10cm (3-4") |

User receives a complete, band-matched RF assembly. No antenna selection confusion.

### Carrier BOM

| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| U1 | SA818-V or SA818-U | Module | 2mm pitch castellated |
| J_SMA | SMA right-angle | PCB mount | Female, 50Ω |
| C1 | 100nF | 0402/0603 | Decoupling |
| C2 | 10µF | 0603/0805 | Bulk |
| R_ID | 0Ω | 0402 | To GND (VHF) or VCC (UHF) |
| J1 | 1x8 male header | 0.1" pitch | Through-hole, pointing backward |
| J2 | 1x8 male header | 0.1" pitch | Through-hole, pointing backward |
| J3 | 1x3 male header | 0.1" pitch | Through-hole, mechanical only |
| J4 | 1x3 male header | 0.1" pitch | Through-hole, mechanical only |
| ANT | Stubby antenna | SMA male | VHF or UHF matched |

**Per-carrier cost:** ~$15-20 (SA818 ~$10, SMA ~$1-2, antenna ~$3-5, PCB + passives ~$2)

### Mechanical Stack

```
Side view (assembled):

                     antenna (~15-20cm VHF, ~8-10cm UHF)
                         ↑
                    ┌────┴────┐
                    │   SMA   │
    CARRIER   ══════╪═════════╪══════  ← ~1.6mm PCB + ~5mm SA818 height
                    │         │
              ┌─────┴───┐ ┌───┴─────┐
              │ headers │ │ headers │   ← ~8-11mm pin length
              └─────┬───┘ └───┬─────┘
    MAIN      ══════╪═════════╪══════  ← Main badge PCB
                    sockets on
                    front of main

    Gap between boards: ~5-8mm (header pin length minus insertion depth)
    Total stack: ~12-15mm thick at carrier area
```

---

## Main Board — Carrier Interface

### No RF on Main Board

With all RF contained on the carrier, the main badge PCB:
- Has **NO 50Ω traces**
- Has **NO RF via stitching requirements**
- Has **NO SMA connector**
- Only needs 4 female header sockets at the top edge

This dramatically simplifies the main board design. A 2-layer PCB may be feasible depending on RP2350 routing density.

### Socket Placement (Main Board Top Edge)

```
    MAIN BADGE - TOP EDGE (front side, looking at badge)

     ┌─────────────────────────────────────────────────────────────┐
     │                                                             │
     │  □ □ □                                               □ □ □  │ ← J3, J4 so
     │  (J3)                                                 (J4)  │   1x3, mech.
     │                                                             │
    ...                                                           ...
     │                                                             │
     │                                                             │
     │  □ □ □ □ □ □ □ □                           □ □ □ □ □ □ □ □  │ ← J1, J2
     │  (J1 socket)                                 (J2 socket)    │   1x8, sig
     │                                                             │
     │                                                             │
     │               ┌─────────────────────────┐                   │
     │               │                         │                   │
     │               │        DISPLAY          │                   │
     │               │                         │                   │
     │               └─────────────────────────┘                   │
     │                                                             │
     │                    (rest of badge...)                       │
     │                                                             │
     └─────────────────────────────────────────────────────────────┘
```

### Main Board Socket Connections

**J1 Socket (1x8) — Power and Audio:**

| Pin | Signal | Connects To |
|-----|--------|-------------|
| 1 | VCC | TPS22919 output |
| 2 | GND | Ground plane |
| 3 | GND | Ground plane |
| 4 | MIC+ | TX audio filter output |
| 5 | MIC- | GND |
| 6 | SPK+ | RX audio filter input |
| 7 | SPK- | GND |
| 8 | NC | — |

**J2 Socket (1x8) — Control and Data:**

| Pin | Signal | Connects To |
|-----|--------|-------------|
| 1 | TXD | RP2350 UART RX |
| 2 | RXD | RP2350 UART TX |
| 3 | PTT | RP2350 GPIO |
| 4 | PD | RP2350 GPIO (directly drive) or TPS22919 EN (power control) |
| 5 | HL | RP2350 GPIO |
| 6 | SQ | RP2350 GPIO |
| 7 | ID | RP2350 GPIO (with internal pulldown) |
| 8 | GND | Ground plane |

**J3, J4 Sockets (1x3 each) — Mechanical:**

No electrical connection. Just sockets to receive mechanical support pins from carrier.

*See [Engineer's Notebook](engineers-notebook/electrical-design-decisions.md#main-board-pcb-layers-2-vs-4) for PCB layer discussion.*

---

## Display Interface

### Overview

The display shows SSTV images, menus, and status information.

**Specifications:**
- **Module:** 2.4" ILI9341-based TFT LCD breakout
- **Resolution:** 320×240 pixels (landscape orientation)
- **Interface:** SPI (up to 62.5 MHz)
- **Backlight:** PWM dimming via GPIO + MOSFET
- **Touch:** None (D-pad navigation instead)
- **SD Card:** Using module's built-in SD slot (shared SPI bus)

*See [Engineer's Notebook](engineers-notebook/electrical-design-decisions.md#why-320x240) for resolution selection rationale.*

### ILI9341 Module Pinout

| Module Pin | Signal | RP2350 Connection | Notes |
|------------|--------|-------------------|-------|
| VCC | Power | 3.3V rail | Module has onboard regulator if 5V tolerant |
| GND | Ground | GND | |
| CS | Chip Select | GPIO | directly |
| RESET | Reset | GPIO | Software reset capability |
| DC/RS | Data/Command | GPIO | Directly driven |
| SDI/MOSI | SPI Data In | SPI TX GPIO | Directly driven |
| SCK | SPI Clock | SPI SCK GPIO | |
| LED/BLK | Backlight | GPIO via MOSFET | PWM dimming |
| SDO/MISO | SPI Data Out | SPI RX GPIO | Optional, for read-back |
| SD_CS | SD Card Select | GPIO | For module's SD slot |

### SPI Configuration

| Parameter | Value |
|-----------|-------|
| SPI Mode | Mode 0 (CPOL=0, CPHA=0) |
| Max Clock | 62.5 MHz |
| Data Width | 8-bit or 16-bit |
| Full Frame | 320×240×16bpp = 153,600 bytes |
| Frame Time | ~20ms at 62.5 MHz → ~50 FPS capable |

For static SSTV images, even 10 FPS is more than sufficient.

### SD Card (Shared SPI Bus)

The display module's built-in SD card slot shares the SPI bus with the display. This means:
- Only one device active at a time (display OR SD card)
- Must manage separate CS pins for each device
- May need to adjust SPI clock when switching (SD cards are pickier)

**This is fine for SSTV** because operations are naturally sequential:
- Load image from SD → then display it
- Receive SSTV → decode to RAM → display → optionally save to SD

No workflow requires simultaneous display update and SD access.

### Backlight PWM Circuit

Most modules wire the backlight LED directly to VCC. To add PWM dimming, we switch the LED ground path:

```
    RP2350                                          Display Module
    ┌───────┐                                       LED/BLK pin
    │       │                                            │
    │  GPIO ├────┐                                       │
    │  (BL) │    │                                       │
    │       │   [1kΩ] R_BL                               │
    └───────┘    │                                       │
                 └──┤ G                                  │
                    │      ┌─────────────────────────────┘
                    │ 2N7002
                    ├ S    │ D
                    │      │
                   GND    GND

    PWM HIGH → MOSFET ON → Backlight ON
    PWM LOW  → MOSFET OFF → Backlight OFF
    PWM duty cycle controls brightness
```

**PWM Frequency:** 1-10 kHz (higher = no visible flicker)

**Note:** Some modules have the LED anode on the BLK pin (switch high side). Check your specific module. The 2N7002 circuit above assumes LED cathode on BLK pin (more common).

### Complete Display Schematic

```
    RP2350                                              ILI9341 Module
    ┌───────┐                                          ┌─────────────┐
    │       │                                          │             │
    │  3.3V ├──────────────────────────────────────────┤ VCC         │
    │       │                                          │             │
    │   GND ├──────────────────────────────────────────┤ GND         │
    │       │                                          │             │
    │  GPIO ├──────────────────────────────────────────┤ CS          │
    │(DISP_CS)                                         │             │
    │       │                                          │             │
    │  GPIO ├──────────────────────────────────────────┤ RESET       │
    │(DISP_RST)                                        │             │
    │       │                                          │             │
    │  GPIO ├──────────────────────────────────────────┤ DC          │
    │(DISP_DC)                                         │             │
    │       │                                          │             │
    │  SPI  ├──────────────────────────────────────────┤ SDI (MOSI)  │
    │  TX   │                                          │             │
    │       │                                          │             │
    │  SPI  ├──────────────────────────────────────────┤ SCK         │
    │  SCK  │                                          │             │
    │       │                                          │             │
    │  SPI  ├──────────────────────────────────────────┤ SDO (MISO)  │
    │  RX   │                                          │  (optional) │
    │       │                                          │             │
    │  GPIO ├──────────────────────────────────────────┤ SD_CS       │
    │(SD_CS)│                                          │  (SD card)  │
    │       │                                          │             │
    │  GPIO ├────┐                                     │             │
    │(DISP_BL)   │                                     │             │
    │       │   [1kΩ]                                  │             │
    └───────┘    │                                     │             │
                 └──┤ G                                │             │
                    │      ┌───────────────────────────┤ LED (BLK)   │
               2N7002│ D ←─┘                           │             │
                    ├ S                                │             │
                    │                                  │             │
                   GND                                 └─────────────┘
```

### GPIO Summary for Display

| Function | GPIO | Notes |
|----------|------|-------|
| SPI SCK | TBD | Shared SPI clock |
| SPI TX (MOSI) | TBD | Shared SPI data out |
| SPI RX (MISO) | TBD | Shared SPI data in (optional for display, needed for SD) |
| DISP_CS | TBD | Display chip select |
| DISP_DC | TBD | Data/Command select |
| DISP_RST | TBD | Display reset |
| DISP_BL | TBD | Backlight PWM |
| SD_CS | TBD | SD card chip select |

**Total: 6-7 dedicated GPIOs** (plus SPI bus which may be shared with other peripherals)

### Display BOM

| Ref | Value | Package | Assembly | Notes |
|-----|-------|---------|----------|-------|
| U_DISP | ILI9341 2.4" module | Breakout | User | 320×240, SPI, w/ SD slot |
| Q_BL | 2N7002 | SOT-23 | Fab | Backlight MOSFET |
| R_BL | 1kΩ | 0603 | User | Gate resistor |

**Cost:** ~$8-12 for module + ~$0.10 for MOSFET/resistor

### Orientation and Mounting

- **Orientation:** Landscape (320 wide × 240 tall)
- **Mounting:** Module breakout board, likely with pin headers or direct solder
- **Position:** Center of main badge, below the carrier board area

The display is the main visual element — position it prominently for the "retro TV" aesthetic.

---

## Camera Interface

### Overview

The camera captures images for SSTV transmission and provides live viewfinder preview.

**Specifications:**
- **Sensor:** OV2640 2MP CMOS
- **Module:** Pre-assembled breakout with 2.54mm pin header (user-solderable)
- **Interface:** Parallel DVP (8-bit) + SCCB (I2C) for control
- **Resolution:** QVGA (320×240) for SSTV, up to UXGA (1600×1200) if needed
- **Output:** RGB565 or YUV422 (for viewfinder), JPEG (for storage)

*See [Engineer's Notebook](engineers-notebook/electrical-design-decisions.md#why-parallel-dvp-not-spi) for interface selection rationale.*

### OV2640 DVP Pinout

| Pin | Signal | Direction | RP2350 Connection | Notes |
|-----|--------|-----------|-------------------|-------|
| 3V3 | Power | — | 3.3V rail | ~50mA active |
| GND | Ground | — | GND | |
| SIOC | I2C Clock | ← | Shared I2C SCL | SCCB (I2C-compatible) |
| SIOD | I2C Data | ↔ | Shared I2C SDA | SCCB (I2C-compatible) |
| VSYNC | Frame sync | → | GPIO | High during vertical blank |
| HREF | Line valid | → | GPIO | High when pixel data valid |
| PCLK | Pixel clock | → | GPIO | Clock from camera |
| XCLK | Master clock | ← | GPIO (PWM) | ~20MHz clock to camera |
| D0-D7 | Pixel data | → | 8 GPIOs | 8-bit parallel data |
| RESET | Reset | ← | GPIO | Active low (or tie to 3.3V via 10K) |
| PWDN | Power down | ← | GPIO | Active high (or tie to GND) |

### DVP Timing

```
    VSYNC ────┐                                              ┌────
              │             Frame N                          │
              └──────────────────────────────────────────────┘

    HREF      ┌──┐  ┌──┐  ┌──┐  ┌──┐  ┌──┐  ┌──┐  ┌──┐  ┌──┐
           ───┘  └──┘  └──┘  └──┘  └──┘  └──┘  └──┘  └──┘  └───
              Line0  Line1 Line2  ...                    Line239

    PCLK      ┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐
           ───┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└───
              Each rising edge = one pixel byte

    D[7:0]    ═══╦═══╦═══╦═══╦═══╦═══╦═══╦═══╦═══╦═══╦═══╦═══╦═══
                 P0   P1   P2   P3   ...                    P639
              (RGB565 = 2 bytes per pixel, so 640 bytes per line for 320 pixels)
```

### PIO Implementation

The RP2350's PIO (Programmable I/O) is ideal for DVP capture:

1. **PIO program** watches HREF and PCLK
2. **On PCLK rising edge** (when HREF high), shift in D0-D7
3. **DMA** transfers data directly to RAM buffer
4. **CPU is free** to process previous frame / update display

This enables zero-copy capture with minimal CPU involvement.

### XCLK Generation

The camera needs a master clock input (XCLK), typically 20-24 MHz.

**Option A: PWM output**
- Configure RP2350 PWM for 20MHz, 50% duty cycle
- Simple, uses one GPIO

**Option B: PIO clock output**
- PIO can generate precise clocks
- More flexible frequency control

**Recommendation:** PWM is simpler and sufficient.

```
    RP2350 PWM GPIO ─────────────────────────────→ OV2640 XCLK

    PWM config:
    - System clock: 150MHz (typical)
    - Divider: 7.5 → 20MHz output
    - Duty: 50%
```

### Camera Module Selection

For user-solderable assembly, use an OV2640 module with 2.54mm pin headers.

**Recommended:** Generic "OV2640 DVP module" with 18-24 pin header

```
    ┌───────────────────────────────┐
    │    ┌─────────────────────┐    │
    │    │     ░░░░░░░░░░░     │    │  ← Lens
    │    │     ░ OV2640  ░     │    │
    │    │     ░ Sensor  ░     │    │
    │    │     ░░░░░░░░░░░     │    │
    │    └─────────────────────┘    │
    │                               │
    │  ┌───────────────────────┐    │
    │  │ Supporting circuitry  │    │  ← Voltage reg, crystal, etc.
    │  └───────────────────────┘    │
    │                               │
    │  ○ ○ ○ ○ ○ ○ ○ ○ ○ ○ ○ ○ ○    │  ← 2.54mm through-hole pins
    │  (user-solderable)            │
    └───────────────────────────────┘

    Typical size: ~25mm × 25mm
```

**Search terms:** "OV2640 camera module DVP" or "OV2640 module for STM32/ESP32"

**Price:** ~$5-8

### Camera Schematic

```
    RP2350B                                            OV2640 Module
    ┌───────┐                                         ┌─────────────┐
    │       │                                         │             │
    │  3.3V ├─────────────────────────────────────────┤ 3V3         │
    │       │                                         │             │
    │   GND ├─────────────────────────────────────────┤ GND         │
    │       │                                         │             │
    │  I2C  ├─────────────────────────────────────────┤ SIOC (SCL)  │
    │  SCL  │              (shared bus)               │             │
    │       │                                         │             │
    │  I2C  ├─────────────────────────────────────────┤ SIOD (SDA)  │
    │  SDA  │              (shared bus)               │             │
    │       │                                         │             │
    │  GPIO ├──────────────────────────(PWM 20MHz)────┤ XCLK        │
    │(CAM_XCLK)                                       │             │
    │       │                                         │             │
    │  GPIO ├─────────────────────────────────────────┤ PCLK        │
    │(CAM_PCLK)                                       │             │
    │       │                                         │             │
    │  GPIO ├─────────────────────────────────────────┤ VSYNC       │
    │(CAM_VSYNC)                                      │             │
    │       │                                         │             │
    │  GPIO ├─────────────────────────────────────────┤ HREF        │
    │(CAM_HREF)                                       │             │
    │       │                                         │             │
    │  GPIO ├─────────────────────────────────────────┤ D0          │
    │(CAM_D0)│                                        │             │
    │   :   │                                         │     :       │
    │   :   │            (8 data lines)               │     :       │
    │  GPIO ├─────────────────────────────────────────┤ D7          │
    │(CAM_D7)│                                        │             │
    │       │                                         │             │
    │  GPIO ├────────┬────────────────────────────────┤ RESET       │
    │(CAM_RST)       │                                │             │
    │       │   [10kΩ] (pullup to 3.3V)               │             │
    │       │        │                                │             │
    │       │       3.3V                              │             │
    │       │                                         │             │
    │  GPIO ├────────┬────────────────────────────────┤ PWDN        │
    │(CAM_PWDN)      │                                │             │
    │       │   [10kΩ] (pulldown to GND)              │             │
    │       │        │                                │             │
    └───────┘       GND                               └─────────────┘

    Note: RESET and PWDN can be directly driven or tied via resistors
    if GPIO control isn't needed. Driving them allows power-saving sleep.
```

### GPIO Summary for Camera

| Function | GPIO | Notes |
|----------|------|-------|
| I2C SCL | Shared | SCCB clock (shared with SAO) |
| I2C SDA | Shared | SCCB data (shared with SAO) |
| CAM_XCLK | TBD | PWM output, ~20MHz |
| CAM_PCLK | TBD | Pixel clock input (PIO) |
| CAM_VSYNC | TBD | Frame sync input |
| CAM_HREF | TBD | Line valid input |
| CAM_D0-D7 | TBD | 8× parallel data (PIO) |
| CAM_RST | TBD | Reset (optional, can tie high) |
| CAM_PWDN | TBD | Power down (optional, can tie low) |

**Dedicated GPIOs: 10-12** (depending on whether RST/PWDN are GPIO-controlled)

### Viewfinder Performance

| Parameter | Value |
|-----------|-------|
| Resolution | QVGA (320×240) |
| Color format | RGB565 |
| Frame size | 153,600 bytes |
| DVP capture rate | 30+ FPS (PIO + DMA) |
| Display update rate | ~50 FPS max (SPI limited) |
| **Effective viewfinder** | **15-20 FPS** (capture + display) |

15-20 FPS is smooth enough for a viewfinder experience.

### Camera BOM

| Ref | Value | Package | Assembly | Notes |
|-----|-------|---------|----------|-------|
| U_CAM | OV2640 DVP module | Breakout | User | 2.54mm pin header, ~$5-8 |
| R_CAM_RST | 10kΩ | 0603 | Fab | Reset pullup (optional if GPIO controlled) |
| R_CAM_PWDN | 10kΩ | 0603 | Fab | PWDN pulldown (optional if GPIO controlled) |

**Cost:** ~$5-8 for module + ~$0.02 for resistors

### Camera Mounting

Position the camera for the "take a picture" use case:
- **Front-facing** (same side as display) — selfie/badge-to-badge photos
- Near top of badge, possibly integrated into the "TV antenna" aesthetic
- Ensure lens isn't blocked by carrier board or antenna

---

## MCU: RP2350B

*See [Engineer's Notebook](engineers-notebook/electrical-design-decisions.md#why-rp2350b-qfn-80) for MCU selection rationale.*

### RP2350B Specifications

| Parameter | Value |
|-----------|-------|
| Package | QFN-80 (9×9mm) |
| GPIOs | 48 (GPIO0-GPIO47) |
| CPU | Dual ARM Cortex-M33 @ 150MHz |
| SRAM | 520KB |
| Flash | External SPI (16MB in BOM) |
| PIO | 3× PIO blocks, 12 state machines |
| ADC | 8-channel 12-bit (GPIO40-47) |
| PWM | 12 channels |

### GPIO Pin Assignment

All 48 GPIOs are allocated. Camera RST/PWDN are tied with resistors to free pins for SAO.

**I2C Bus (Shared):**

| GPIO | Function | Notes |
|------|----------|-------|
| 0 | I2C0_SDA | Camera SCCB, LC709203, SAO1, SAO2 |
| 1 | I2C0_SCL | Camera SCCB, LC709203, SAO1, SAO2 |

**SPI Bus (Display + SD):**

| GPIO | Function | Notes |
|------|----------|-------|
| 2 | SPI0_SCK | Clock |
| 3 | SPI0_TX | MOSI to display and SD |
| 4 | SPI0_RX | MISO from SD (display doesn't need) |

**Display Control:**

| GPIO | Function | Notes |
|------|----------|-------|
| 5 | DISP_CS | Display chip select (directly driven) |
| 6 | DISP_DC | Data/Command select |
| 7 | DISP_RST | Display reset |
| 22 | DISP_BL | Backlight PWM (via 2N7002) |
| 23 | SD_CS | SD card chip select |

**Camera DVP (PIO0):**

| GPIO | Function | Notes |
|------|----------|-------|
| 8 | CAM_D0 | Parallel data bit 0 |
| 9 | CAM_D1 | Parallel data bit 1 |
| 10 | CAM_D2 | Parallel data bit 2 |
| 11 | CAM_D3 | Parallel data bit 3 |
| 12 | CAM_D4 | Parallel data bit 4 |
| 13 | CAM_D5 | Parallel data bit 5 |
| 14 | CAM_D6 | Parallel data bit 6 |
| 15 | CAM_D7 | Parallel data bit 7 |
| 16 | CAM_PCLK | Pixel clock input (PIO) |
| 17 | CAM_VSYNC | Frame sync |
| 18 | CAM_HREF | Line valid |
| 19 | CAM_XCLK | Master clock output (PWM ~20MHz) |

Note: CAM_RST tied HIGH via 10kΩ, CAM_PWDN tied LOW via 10kΩ (not GPIO controlled).

**SAO Connectors:**

| GPIO | Function | Notes |
|------|----------|-------|
| 20 | SAO1_GPIO1 | SAO connector 1, pin 5 |
| 21 | SAO1_GPIO2 | SAO connector 1, pin 6 |
| 27 | SAO2_GPIO1 | SAO connector 2, pin 5 |
| 28 | SAO2_GPIO2 | SAO connector 2, pin 6 |

SAO I2C shared on GPIO0/1. VCC=3.3V, GND from power rails.

**Audio Interface:**

| GPIO | Function | Notes |
|------|----------|-------|
| 24 | I2S_BCK | Bit clock to PCM5102A |
| 25 | I2S_LRCK | Word select (L/R clock) |
| 29 | I2S_DIN | Data to PCM5102A |
| 40 | RX_AUDIO | ADC input from SA818 SPK (ADC0) |

**Note:** RP2350B ADC channels are on GPIO40-47 (not GPIO26-29 like RP2040).

**SA818 Interface:**

| GPIO | Function | Notes |
|------|----------|-------|
| 30 | SA818_TX | UART1_TX → SA818 RXD |
| 31 | SA818_RX | UART1_RX ← SA818 TXD |
| 32 | SA818_PTT | Push-to-talk (active low) |
| 33 | SA818_PD | Power down / TPS22919 enable |
| 34 | SA818_HL | High/Low power select |
| 35 | SA818_SQ | Squelch detect input |
| 36 | SA818_ID | Band ID from carrier |

**User Controls:**

| GPIO | Function | Notes |
|------|----------|-------|
| 26 | DPAD_RIGHT | Active low, internal pullup (moved from GPIO40 for ADC) |
| 37 | DPAD_UP | Active low, internal pullup |
| 38 | DPAD_DOWN | Active low, internal pullup |
| 39 | DPAD_LEFT | Active low, internal pullup |
| 41 | DPAD_CENTER | Active low, internal pullup |
| 42 | PHOTO | Active low, internal pullup |
| 43 | AIRPLANE | Active low, internal pullup |

**LEDs:**

| GPIO | Function | Notes |
|------|----------|-------|
| 44 | PWR_R | PWR LED red channel (PWM) |
| 45 | PWR_G | PWR LED green channel (PWM) |
| 46 | PWR_B | PWR LED blue channel (PWM) |
| 47 | LED_DATA | WS2812B chain (PIO) |

### GPIO Assignment Summary

```
GPIO 0-1:   I2C (shared bus)
GPIO 2-4:   SPI (display + SD)
GPIO 5-7:   Display control (CS, DC, RST)
GPIO 8-19:  Camera DVP (D0-D7, PCLK, VSYNC, HREF, XCLK)
GPIO 20-21: SAO1 GPIOs
GPIO 22-23: Display BL + SD_CS
GPIO 24-25: Audio I2S (BCK, LRCK)
GPIO 26:    DPAD_RIGHT (moved here to free GPIO40 for ADC)
GPIO 27-28: SAO2 GPIOs
GPIO 29:    Audio I2S (DIN)
GPIO 30-36: SA818 (UART + control)
GPIO 37-39: User controls (DPAD UP/DOWN/LEFT)
GPIO 40:    RX Audio ADC (ADC0)
GPIO 41-43: User controls (DPAD CENTER, PHOTO, AIRPLANE)
GPIO 44-46: PWR LED (RGB)
GPIO 47:    LED_DATA (WS2812B blinky)

Total: 48/48 GPIOs allocated (0 spare)
```

### I2C Bus (Shared)

The I2C bus on GPIO0 (SDA) and GPIO1 (SCL) is shared by multiple devices:

| Device | I2C Address | Function |
|--------|-------------|----------|
| OV2640 | 0x30 (SCCB) | Camera configuration |
| LC709203F | 0x0B | Battery fuel gauge |
| SAO1 | (varies) | Shitty Add-On 1 |
| SAO2 | (varies) | Shitty Add-On 2 |

**Pullup Resistors:**

```
    3.3V ─────┬─────────────┬─────────────────────────────────►
              │             │
             [R]           [R]
            4.7kΩ         4.7kΩ
              │             │
    GPIO0 ────┴── SDA ──────┼──► OV2640, LC709203F, SAO1, SAO2
                            │
    GPIO1 ──────── SCL ─────┴──► OV2640, LC709203F, SAO1, SAO2
```

| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| R_SDA | 4.7kΩ | 0603 | I2C SDA pullup |
| R_SCL | 4.7kΩ | 0603 | I2C SCL pullup |

**Notes:**
- 4.7kΩ is suitable for 400kHz I2C (Fast Mode)
- Camera SCCB is compatible with I2C
- If SAO devices have their own pullups, total resistance may be lower (acceptable)
- For 1MHz I2C (Fast Mode Plus), use 2.2kΩ pullups instead

### PIO Allocation

| PIO | State Machines | Function |
|-----|----------------|----------|
| PIO0 | SM0-SM1 | Camera DVP capture (D0-D7 + PCLK) |
| PIO1 | SM0 | WS2812B LED driver |
| PIO2 | (available) | Future use |

### PWM Usage

| GPIO | PWM Slice | Function |
|------|-----------|----------|
| 19 | — | CAM_XCLK (~20MHz) |
| 22 | — | DISP_BL (1-10kHz) |
| 44 | — | PWR_R |
| 45 | — | PWR_G |
| 46 | — | PWR_B |

### Crystal Oscillator

*See [Engineer's Notebook](engineers-notebook/electrical-design-decisions.md#why-a-crystal-is-required) for crystal requirement rationale and part recommendations.*

**Crystal Circuit:**

```
                    RP2350B
                   ┌────────────────┐
                   │                │
                   │    XIN  ●──────┼────┬────[Y1 12MHz]────┬────●  XOUT
                   │                │    │                  │
                   │                │   [C1]               [C2]
                   │                │   15pF               15pF
                   │                │    │                  │
                   │                │   GND                GND
                   └────────────────┘
```

**Component Selection:**

| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| Y1 | 12MHz | 3215 (3.2×1.5mm) | ±20ppm, 10pF load, fundamental |
| C1, C2 | 15pF | 0402 | Load capacitors (2 × CL - C_stray = 2×10 - 5 = 15pF) |

**Layout notes:**
- Keep crystal close to XIN/XOUT pins (<5mm trace length)
- Ground plane under crystal
- No high-speed signals routed near crystal
- Load caps close to crystal, directly to ground

### External Flash

The RP2350 boots from external QSPI flash. This stores firmware and can also store user data (images, settings).

**Flash Circuit:**

```
                    RP2350B                          W25Q128
                   ┌────────────────┐               ┌─────────────┐
                   │                │               │             │
                   │  QSPI_SS  ●────┼───────────────┤ /CS         │
                   │                │               │             │
                   │  QSPI_SCK ●────┼───────────────┤ CLK         │
                   │                │               │             │
                   │  QSPI_SD0 ●────┼───────────────┤ DI (IO0)    │
                   │                │               │             │
                   │  QSPI_SD1 ●────┼───────────────┤ DO (IO1)    │
                   │                │               │             │
                   │  QSPI_SD2 ●────┼───────────────┤ /WP (IO2)   │
                   │                │               │             │
                   │  QSPI_SD3 ●────┼───────────────┤ /HOLD (IO3) │
                   │                │               │             │
                   └────────────────┘               │        VCC  ├───► 3.3V
                                                    │             │
                                                    │        GND  ├───► GND
                                                    └─────────────┘
                                                          │
                                                         [C] 100nF
                                                          │
                                                         GND
```

**Component Selection:**

| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| U_FLASH | W25Q128JVSIQ | SOIC-8 | 16MB, QSPI, 133MHz |
| C_FLASH | 100nF | 0402 | Decoupling, close to VCC |

*See [Engineer's Notebook](engineers-notebook/electrical-design-decisions.md#why-16mb-flash) for flash size rationale and alternatives.*

**Layout notes:**
- Keep traces short (<25mm total)
- Match trace lengths for high-speed operation
- 100nF decoupling cap close to flash VCC
- Ground plane under flash

### BOOTSEL Button

The BOOTSEL button allows entering the RP2350 USB bootloader (UF2 mass storage mode) for firmware updates.

**How it works:**
1. Hold BOOTSEL button
2. Plug in USB (or press reset while holding BOOTSEL)
3. RP2350 appears as USB mass storage device
4. Drag-and-drop .uf2 firmware file
5. Device automatically reboots with new firmware

**Circuit:**

```
    RP2350B
   ┌────────────────┐
   │                │
   │  QSPI_SS  ●────┼────┬────────► To Flash /CS
   │                │    │
   └────────────────┘    │
                         │
                    ┌────┴────┐
                    │         │
                    │ BOOTSEL │  ← Small tactile switch
                    │  (SW)   │
                    │         │
                    └────┬────┘
                         │
                        GND
```

**Notes:**
- QSPI_SS is a dedicated pin (not GPIO) — doesn't affect GPIO count
- When button pressed, QSPI_SS is pulled LOW
- At boot, if QSPI_SS is LOW, RP2350 enters USB bootloader
- Small switch is fine — only pressed during firmware updates
- Place on back of PCB or recessed to prevent accidental activation

**Component:**

| Ref | Value | Package | Assembly | Notes |
|-----|-------|---------|----------|-------|
| SW_BOOT | Tactile 3×4mm or 4×4mm | SMD | Fab | BOOTSEL, recessed placement |

**Alternatives to physical button:**
- Use `picotool reboot -f` command (requires working firmware)
- Use SWD via Tag-Connect (always works)
- Double-tap reset (if firmware implements it)

For a production badge, the physical button provides a reliable recovery method if firmware is corrupted.

### RP2350 Internal Voltage Regulator

The RP2350 contains an internal switching regulator that converts the 3.3V input to 1.1V for the CPU core. This regulator requires an external inductor.

**Power Architecture:**
```
+3.3V ──► VREG_VIN ──► [Internal Switcher] ──► VREG_LX ──┬──► L2 (3.3µH) ──┬──► DVDD (1.1V core)
                                                         │                  │
                                                         └──────────────────┘
                                                                            │
                                                                           ┴ C_DVDD (1µF)
                                                                            │
                                                                          GND
```

**Power Pin Connections:**

| Pin | Function | Connection |
|-----|----------|------------|
| VREG_VIN | Regulator input (3.3V) | +3V3 via 10µF decoupling |
| VREG_AVDD | Regulator analog supply | +3V3 via 100nF decoupling |
| VREG_LX | Switching node output | To inductor L2 |
| VREG_FB | Feedback (internal) | No external connection required |
| VREG_PGND | Regulator power ground | GND |
| DVDD | Core voltage output (1.1V) | From inductor L2, 1µF to GND |
| IOVDD | I/O power (×6 internal) | +3V3 via 100nF decoupling |
| USB_OTP_VDD | USB/OTP power | +3V3 via 100nF decoupling |
| ADC_AVDD | ADC analog power | +3V3 via 100nF decoupling |
| QSPI_IOVDD | Flash I/O power | +3V3 via 100nF decoupling |

**Inductor Selection:**
- Value: 3.3µH (recommended by Raspberry Pi)
- DCR: <100mΩ
- Saturation current: >500mA
- Package: 3×3mm or similar
- Example: Murata LQH3NPN3R3MM0, Bourns SRN3015-3R3M

### MCU BOM

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| U_MCU | 1 | RP2350B | QFN-80 (9×9mm) | Fab | Dual Cortex-M33, 48 GPIO |
| L2 | 1 | 3.3µH | 3×3mm | Fab | Internal regulator inductor |
| Y1 | 1 | 12MHz | 3215 | Fab | ±20ppm, CL=10pF |
| C_Y1, C_Y2 | 2 | 15pF | 0402 | Fab | Crystal load caps |
| U_FLASH | 1 | W25Q128JVSIQ | SOIC-8 | Fab | 16MB QSPI flash |
| C_FLASH | 1 | 100nF | 0402 | Fab | Flash decoupling |
| C_MCU | 6 | 100nF | 0402 | Fab | MCU decoupling (one per VDD) |
| C_MCU_BULK | 2 | 10µF | 0603 | Fab | MCU bulk caps |
| C_DVDD | 1 | 1µF | 0402 | Fab | DVDD decoupling |
| SW_BOOT | 1 | Tactile 3×4mm | SMD | Fab | BOOTSEL for UF2 bootloader |

**Subtotal:** ~$3.85

---

## Debug Interface (Tag-Connect)

### Overview

Programming and debugging uses a Tag-Connect pogo-pin interface instead of a traditional header. This provides a professional appearance with no visible connector while still allowing easy programming and debugging.

### Tag-Connect TC2030-CTX-NL

| Parameter | Value |
|-----------|-------|
| Connector | TC2030-CTX-NL (6-pin, no legs) |
| Footprint | 0.34" × 0.10" (8.6mm × 2.5mm) |
| Interface | ARM SWD |
| Mating cable | TC2030-CTX-NL-ARM (Digi-Key) |

### Pinout

```
    TC2030-CTX-NL Footprint (top view, from component side)

          ┌─────────────────────────┐
          │   ○       ○       ○     │  ← Pin 1, 2, 3
          │   1       2       3     │
          │                         │
          │   ○       ○       ○     │  ← Pin 4, 5, 6
          │   4       5       6     │
          └─────────────────────────┘

    Pin assignments (ARM SWD):
    ┌─────┬────────┬─────────────────────┐
    │ Pin │ Signal │ RP2350 Connection   │
    ├─────┼────────┼─────────────────────┤
    │ 1   │ VCC    │ 3.3V (target power) │
    │ 2   │ SWDIO  │ Dedicated SWDIO pin │
    │ 3   │ RESETn │ RUN pin             │
    │ 4   │ SWCLK  │ Dedicated SWCLK pin │
    │ 5   │ GND    │ Ground              │
    │ 6   │ SWO    │ NC (not used)       │
    └─────┴────────┴─────────────────────┘
```

### Design Notes

**Footprint placement:**
- Place on back of PCB (less visible when worn)
- Keep away from battery and other tall components
- Ensure probe can make contact without obstruction
- Orient consistently (pin 1 indicator visible)

**No legs variant (NL):**
- Smaller footprint than legged version
- Requires holding probe during programming
- Suitable for production programming and occasional debug
- Lower cost than installing a header

**Programming workflow:**
1. Hold TC2030 probe against pads
2. Connect to debug probe (Picoprobe, J-Link, etc.)
3. Flash via OpenOCD or Picotool
4. UF2 bootloader also available via USB (primary method for users)

**Alternative: Add test points**
- If debugging is frequent during development, add labeled test points nearby
- SWCLK, SWDIO, GND, RUN as 1.27mm pads for clip leads

### Probe Compatibility

| Probe | Interface | Notes |
|-------|-----------|-------|
| Raspberry Pi Debug Probe | SWD | Native RP2350 support |
| Picoprobe (Pico as probe) | SWD | Low cost option |
| SEGGER J-Link | SWD | Professional, fast |
| CMSIS-DAP | SWD | Generic ARM debug |

### BOM Addition

| Ref | Qty | Value | Assembly | Notes |
|-----|-----|-------|----------|-------|
| J_DEBUG | 1 | TC2030-CTX-NL footprint | Fab | Pads only, no component |

**Cost:** $0 (just PCB pads)

**Cable cost:** ~$35-40 for TC2030-CTX-NL + ARM adapter (one-time purchase for developer)

---

## User Controls

### Overview

User controls provide physical interaction with the badge.

| Control | Type | GPIOs | Purpose |
|---------|------|-------|---------|
| D-pad | 5× tactile switches | 5 | Menu navigation (up/down/left/right/center) |
| Photo button | 1× tactile switch | 1 | Capture image |
| Airplane mode | Slide switch | 1 | Disable RF (read by MCU) |
| Status LEDs | 3× LEDs | 3 | Power, TX, RX indicators |

**Total: 10 GPIOs** for user controls

### D-Pad (5-Way Navigation)

Five individual tactile switches arranged in a plus pattern with center select.

```
              ┌───────┐
              │  UP   │
              │  (▲)  │
              └───┬───┘
    ┌───────┐ ┌───┴───┐ ┌───────┐
    │ LEFT  │ │CENTER │ │ RIGHT │
    │  (◄)  ├─┤  (●)  ├─┤  (►)  │
    └───────┘ └───┬───┘ └───────┘
              ┌───┴───┐
              │ DOWN  │
              │  (▼)  │
              └───────┘
```

**Circuit (active-low with internal pullup):**

```
    3.3V (internal pullup in RP2350)
      │
      ├──────────────────── GPIO (D-pad UP)
      │                        │
      │                    ┌───┴───┐
      │                    │  SW   │ Tactile switch
      │                    └───┬───┘
      │                        │
     GND ──────────────────────┘

    Pressed: GPIO reads LOW
    Released: GPIO reads HIGH (pullup)
```

Each D-pad button connects GPIO to GND when pressed. RP2350 internal pullups eliminate external resistors.

**Recommended switch:** 6mm × 6mm tactile switch (through-hole or SMD), e.g., Omron B3F series or generic equivalent.

### Photo Button

Larger tactile switch for ergonomic "shutter" feel.

```
    RP2350
    ┌───────┐
    │       │
    │  GPIO ├────────────┬──────────┐
    │(PHOTO)│            │          │
    │       │       ┌────┴────┐     │
    └───────┘       │   SW    │     │  Tactile switch
                    │ (12mm)  │     │  (larger, satisfying click)
                    └────┬────┘     │
                         │          │
                        GND ────────┘

    Same active-low circuit as D-pad
```

**Recommended switch:** 12mm tactile switch for bigger target and better tactile feedback.

### Airplane Mode Switch

Slide switch that MCU reads to determine RF enable/disable state.

```
    RP2350                              Slide Switch
    ┌───────┐                          ┌─────────────┐
    │       │                          │  ○     ○    │
    │  GPIO ├──────────────────────────┤  ON   OFF   │
    │(AIRPLANE)                        │      ●──────┼─── GND
    │       │                          └─────────────┘
    └───────┘

    Switch ON position: GPIO pulled to GND → reads LOW → RF enabled
    Switch OFF position: GPIO floating → reads HIGH (pullup) → RF disabled (airplane mode)

    (Or reverse logic, depending on preference)
```

**Recommended switch:** SS12D00 or similar SPDT slide switch (through-hole, user-solderable).

**Firmware behavior:**
1. On boot, read airplane mode switch
2. If airplane mode ON: disable TPS22919 (SA818 unpowered), show indicator
3. If airplane mode OFF: normal operation
4. Optionally poll switch periodically or use interrupt on change

### Status LED (PWR)

A traditional RGB LED provides battery status indication, separate from the WS2812B blinky chain. *See [Engineer's Notebook](engineers-notebook/electrical-design-decisions.md#why-traditional-rgb-led-not-ws2812b-for-pwr-status) for selection rationale.*

**PWR LED Circuit (Common Anode RGB):**

**Part:** Kingbright APFA3010LSEEZGKQBKC (PLCC-4, 3.0×1.0mm)

```
                         +3.3V
                           │
                      LED Anode (A)
                           │
                   ┌───────┼───────┐
                   │       │       │
                   R       G       B   (cathodes)
                   │       │       │
    RP2350      [220Ω]  [100Ω]  [100Ω]
    ┌───────┐      │       │       │
    │ GPIO  ├──────┘       │       │
    │(PWR_R)│              │       │
    │       │              │       │
    │ GPIO  ├──────────────┘       │
    │(PWR_G)│                      │
    │       │                      │
    │ GPIO  ├──────────────────────┘
    │(PWR_B)│
    └───────┘

    Logic is INVERTED: GPIO LOW = LED on, GPIO HIGH = LED off
    PWM duty cycle inverted for brightness control
```

**Resistor Values:**
- Red: 220Ω (Vf ~2.0V, ~5mA)
- Green: 100Ω (Vf ~3.0V, ~3mA)
- Blue: 100Ω (Vf ~3.0V, ~3mA)

**Battery Status Colors:**

| State | Color | R | G | B | LiPo Voltage | SoC |
|-------|-------|---|---|---|--------------|-----|
| Full/Good | Green | 0 | 255 | 0 | >3.7V | >50% |
| Medium | Yellow | 255 | 200 | 0 | 3.5V - 3.7V | 20-50% |
| Low | Red | 255 | 0 | 0 | 3.3V - 3.5V | 10-20% |
| Critical | Blinking Red | 255 | 0 | 0 | <3.3V | <10% |

*Note: R/G/B values are logical brightness (0=off, 255=full). Firmware inverts these for common-anode LED (PWM 255 → GPIO LOW → LED on).*

The LC709203 fuel gauge provides accurate state-of-charge readings via I2C.

**Note:** TX/RX status is indicated by the blinky animations (ear lightning + border effects), so no separate TX/RX status LED is needed.

**GPIO Summary for Status + Blinky:**

| Function | GPIOs | Notes |
|----------|-------|-------|
| PWR LED (RGB) | 3 | PWR_R, PWR_G, PWR_B — direct drive with PWM |
| Blinky chain (WS2812B) | 1 | LED_DATA — 26 LEDs total |
| **Total** | **4** | |

### Physical Layout

```
    Badge Front View

    ┌────────────────────────────────────────────────────────────┐
    │                      [CARRIER BOARD]                       │
    │                        (antenna)                           │
    ├────────────────────────────────────────────────────────────┤
    │                                                            │
    │   ● PWR (RGB LED)                    ┌────────────────┐    │
    │     (battery status)                 │                │    │
    │                                      │    DISPLAY     │    │
    │                                      │    320×240     │    │
    │                                      │                │    │
    │              ┌───┐                   └────────────────┘    │
    │              │ ▲ │                                         │
    │          ┌───┼───┼───┐                                     │
    │          │ ◄ │ ● │ ► │   D-pad                             │
    │          └───┼───┼───┘   (5 switches)       [CAMERA]       │
    │              │ ▼ │                            ◯            │
    │              └───┘                                         │
    │                                                            │
    │   ◯ PHOTO                 ═══════════════════════════      │
    │   (large button)           AIRPLANE MODE (slide sw)        │
    │                                                            │
    └────────────────────────────────────────────────────────────┘
```

### User Controls Schematic

```
    RP2350B
    ┌─────────────────────────────────────────────────────────────┐
    │                                                             │
    │  GPIO ──────────────────┬────[SW_UP]────── GND              │
    │ (DPAD_UP)              pullup                               │
    │                                                             │
    │  GPIO ──────────────────┬────[SW_DOWN]──── GND              │
    │ (DPAD_DOWN)            pullup                               │
    │                                                             │
    │  GPIO ──────────────────┬────[SW_LEFT]──── GND              │
    │ (DPAD_LEFT)            pullup                               │
    │                                                             │
    │  GPIO ──────────────────┬────[SW_RIGHT]─── GND              │
    │ (DPAD_RIGHT)           pullup                               │
    │                                                             │
    │  GPIO ──────────────────┬────[SW_CENTER]── GND              │
    │ (DPAD_CENTER)          pullup                               │
    │                                                             │
    │  GPIO ──────────────────┬────[SW_PHOTO]─── GND              │
    │ (PHOTO)                pullup                               │
    │                                                             │
    │  GPIO ──────────────────┬────[SLIDE_SW]─── GND              │
    │ (AIRPLANE)             pullup    (airplane mode)            │
    │                                                             │
    │  GPIO ──[220Ω]── PWR_R ──┐                                  │
    │ (PWR_R)                   │                                 │
    │  GPIO ──[100Ω]── PWR_G ──┼── RGB LED (battery status)       │
    │ (PWR_G)                   │                                 │
    │  GPIO ──[100Ω]── PWR_B ──┘                                  │
    │ (PWR_B)                                                     │
    │                                                             │
    │  GPIO ──► WS2812B chain (26 LEDs) ──► ears + border blinky  │
    │ (LED_DATA)                                                  │
    │                                                             │
    └─────────────────────────────────────────────────────────────┘

    All switches: active-low with internal pullups (no external resistors needed)
    PWR LED: Traditional RGB, direct GPIO drive (3 GPIOs)
    Blinky: 26× WS2812B chain (1 GPIO) — ears + display border
```

### GPIO Summary for User Controls

| Function | GPIO | Notes |
|----------|------|-------|
| DPAD_UP | TBD | Active low, internal pullup |
| DPAD_DOWN | TBD | Active low, internal pullup |
| DPAD_LEFT | TBD | Active low, internal pullup |
| DPAD_RIGHT | TBD | Active low, internal pullup |
| DPAD_CENTER | TBD | Active low, internal pullup |
| PHOTO | TBD | Active low, internal pullup |
| AIRPLANE | TBD | Active low, internal pullup |
| PWR_R | TBD | PWR LED red channel (PWM) |
| PWR_G | TBD | PWR LED green channel (PWM) |
| PWR_B | TBD | PWR LED blue channel (PWM) |
| LED_DATA | TBD | WS2812B blinky chain (26 LEDs: ears + border) |

**Total: 11 GPIOs** for user controls (7 switches + 4 LED control)

### User Controls BOM

| Ref | Value | Package | Assembly | Notes |
|-----|-------|---------|----------|-------|
| SW_UP, SW_DOWN, SW_LEFT, SW_RIGHT, SW_CENTER | Tactile 6×6mm | Through-hole | User | D-pad switches |
| SW_PHOTO | Tactile 12mm | Through-hole | User | Photo button (larger) |
| SW_AIRPLANE | Slide switch SPDT | Through-hole | User | Airplane mode |
| LED_PWR | APFA3010LSEEZGKQBKC | PLCC-4 | Fab | Kingbright RGB, common-anode |
| R_PWR_R | 220Ω | 0603 | Fab | PWR LED red cathode current limit |
| R_PWR_G | 100Ω | 0603 | Fab | PWR LED green cathode current limit |
| R_PWR_B | 100Ω | 0603 | Fab | PWR LED blue cathode current limit |
| WS2812B (×26) | WS2812B | 5050 SMD | Fab | Blinky chain (see Blinky section) |
| C_LED (×26) | 100nF | 0402 | Fab | WS2812B bypass capacitors |

**Cost:** ~$3.50 total (RGB LED ~$0.10, 26× WS2812B ~$2.10, 26× caps ~$0.26, switches ~$1)

All switches use software debounce (no external caps). *See [Engineer's Notebook](engineers-notebook/electrical-design-decisions.md#debouncing-strategy) for rationale.*

### Blinky LED Matrix

The badge features a 26-LED WS2812B chain for full #badgelife blinky appeal. The PCB is cut in the shape of a retro TV (Simpsons-style) with rabbit ear antennas as part of the main board.

Battery status is handled by a separate traditional RGB LED (see Status LED section).

**LED Sections:**

| Section | LEDs | Index | Purpose |
|---------|------|-------|---------|
| Left Ear | 5 | #0-4 | L0 (base) → L4 (tip) |
| Right Ear | 5 | #5-9 | R0 (base) → R4 (tip) |
| Display Border | 16 | #10-25 | B0-B15 clockwise from top-left |
| **Total** | **26** | | |

**Physical Layout:**

```
                      ●(L4)            ●(R4)        ← Ear tips
                     ╱                    ╲
                    ●(L3)                ●(R3)
                   ╱                        ╲
                  ●(L2)                    ●(R2)
                 ╱                            ╲
                ●(L1)                        ●(R1)
               ╱                                ╲
    ┌─────────●(L0)──────────────────────────●(R0)─────────┐
    │                 [CARRIER BOARD]                      │
    ├──────────────────────────────────────────────────────┤
    │                                                      │
    │      ●(B0)   ●(B1)   ●(B2)   ●(B3)   ●(B4)           │
    │        ┌────────────────────────────────┐            │
    │   ●(B15)                                │●(B5)       │
    │        │                                │            │
    │   ●(B14)         DISPLAY                │●(B6)       │
    │        │          320×240               │            │
    │   ●(B13)                                │●(B7)       │
    │        │                                │            │
    │        └────────────────────────────────┘            │
    │      ●(B12)  ●(B11)  ●(B10)  ●(B9)   ●(B8)           │
    │                                                      │
    │   ● PWR (RGB LED - separate, not in chain)           │
    └──────────────────────────────────────────────────────┘
```

**WS2812B Chain Order (26 LEDs):**

```
GPIO ─►[L0]─►[L1]─►[L2]─►[L3]─►[L4]─►[R0]─►[R1]─►[R2]─►[R3]─►[R4]─►[B0]─►...─►[B15]
        #0    #1    #2    #3    #4    #5    #6    #7    #8    #9   #10       #25
```

**Index Map:**

| Index | LED ID | Location |
|-------|--------|----------|
| 0-4 | L0-L4 | Left rabbit ear (base to tip) |
| 5-9 | R0-R4 | Right rabbit ear (base to tip) |
| 10-14 | B0-B4 | Border top (left to right) |
| 15-17 | B5-B7 | Border right (top to bottom) |
| 18-22 | B8-B12 | Border bottom (right to left) |
| 23-25 | B13-B15 | Border left (bottom to top) |

### Blinky Animations

**Animation Colors:**

| Name | R | G | B | Hex | Use |
|------|---|---|---|-----|-----|
| Amber | 255 | 128 | 0 | #FF8000 | TX energy |
| Green | 0 | 255 | 0 | #00FF00 | RX signal |
| Light Blue | 100 | 200 | 255 | #64C8FF | RF discharge/materialize |
| White | 255 | 255 | 255 | #FFFFFF | Lightning flash |

**Rabbit Ears - Transmitting (TX) - "Lightning Strike":**

Signal energy travels UP the antenna, then DISCHARGES into the air like lightning.

```
Frame 1:   L0,R0 = AMBER           (energy starts at base)
Frame 2:   L1,R1 = AMBER           (moves up)
Frame 3:   L2,R2 = AMBER
Frame 4:   L3,R3 = AMBER
Frame 5:   L4,R4 = AMBER           (reaches tip)
Frame 6:   L4,R4 = LIGHT BLUE      (discharge begins!)
Frame 7:   L4,R4 = WHITE           (bright flash - RF released!)
Frame 8:   L4,R4 fade → OFF        (energy dissipates into ether)
→ repeat

Timing: ~50ms per frame (~400ms full cycle)
Trail effect: previous LEDs fade as new ones light
Effect: Energy pulses up antenna, then "lightning strikes" out into the air ⚡
```

**Rabbit Ears - Receiving (RX) - "Signal From The Ether":**

Signal MATERIALIZES from thin air at the tips, then flows DOWN into the badge.

```
Frame 1:   L4,R4 = WHITE           (signal appears from the ether!)
Frame 2:   L4,R4 = LIGHT BLUE      (condensing from RF...)
Frame 3:   L4,R4 = GREEN           (now it's data!)
Frame 4:   L3,R3 = GREEN           (flows down)
Frame 5:   L2,R2 = GREEN
Frame 6:   L1,R1 = GREEN
Frame 7:   L0,R0 = GREEN           (signal received into badge)
Frame 8:   all dim                 (ready for next)
→ repeat

Timing: ~50ms per frame (~400ms full cycle)
Trail effect: previous LEDs fade as new ones light
Effect: RF materializes at tips like magic, then flows into the badge 📡
```

**Animation Summary:**

```
TX (Transmitting):                    RX (Receiving):

    ⚡ WHITE flash                        ✨ WHITE appears
       ↑                                    ↓
    💠 LIGHT BLUE                        💠 LIGHT BLUE
       ↑                                    ↓
    🟠 AMBER ━━━━┓                       🟢 GREEN
    🟠 AMBER     ┃ chase UP                 🟢 GREEN
    🟠 AMBER     ┃                          🟢 GREEN  chase DOWN
    🟠 AMBER ━━━━┛                          🟢 GREEN
    [base]                               [base]

Energy OUT → discharge lightning    Signal IN ← materialize from ether
```

**Rabbit Ears - Idle - "Listening To The Ether":**

Tips gently breathe in subtle blue — the badge is alive, quietly monitoring the airwaves.

```
L4 and R4 (tips only) breathe in soft blue:

Brightness cycle (sinusoidal):
  0% ──── 10% ──── 0% ──── 10% ──── ...
        ↗︎      ↘︎      ↗︎      ↘︎

Color: Soft Blue (R:50, G:100, B:180) — #3264B4
Cycle time: ~4 seconds (very slow, calming)
Max brightness: 10-15% (subtle, not distracting)
LEDs affected: L4 and R4 only (ear tips)

Effect: Badge appears to be "breathing" — alive and listening
```

The slow, dim pulse is:
- **Subtle** — doesn't distract from display or conversation
- **Power efficient** — only 2 LEDs at <15% brightness (~2mA)
- **Atmospheric** — gives the badge personality, like it's alive

**Display Border - Idle (Rainbow Chase):**

```
Rainbow flows around the display frame continuously.

- Each LED offset by 360°/16 = 22.5° in hue
- Hue rotates over time: +1° every ~30ms
- Full spectrum rotation: ~11 seconds
- Brightness: 25-40% (configurable)

Effect: Smooth rainbow flowing clockwise around display
```

**Border "Rows" for Animation:**

```
Row 1 (top):      B0   B1   B2   B3   B4
                   ┌─────────────────────┐
Row 2 (upper):  B15│                     │B5
Row 3 (middle): B14│      DISPLAY        │B6
Row 4 (lower):  B13│                     │B7
                   └─────────────────────┘
Row 5 (bottom):   B12  B11  B10  B9   B8
```

**Display Border - RX/Decoding - "Filling The Tank":**

Each antenna pulse deposits another row — badge fills up with received data.

```
RX Animation (ear pulse per row):

1. Ear tips: WHITE → BLUE → GREEN (signal materializes)
2. Row 5 (bottom) turns ON green
3. Green chases down ears to base

4. Ear tips: WHITE → BLUE → GREEN (another pulse)
5. Row 4 turns ON green (now rows 4-5 lit)
6. Green chases down ears

7. Ear tips: WHITE → BLUE → GREEN
8. Row 3 turns ON green (now rows 3-5 lit)
9. Green chases down ears

10. Ear tips: WHITE → BLUE → GREEN
11. Row 2 turns ON green (now rows 2-5 lit)
12. Green chases down ears

13. Ear tips: WHITE → BLUE → GREEN
14. Row 1 turns ON green (FULL BORDER!)
15. Green chases down ears

16. Brief hold — border fully lit
17. All OFF (buffer processed, ready for more)
→ repeat

Timing: ~400ms per ear cycle, ~2.5 seconds for full fill
Color: Green (R:0, G:255, B:0)
Effect: Each RF pulse "deposits" data into the badge — filling up like a tank 📥
```

**Display Border - TX - "Capacitor Discharge":**

Dramatic charge-up, then synchronized ZAP! Like a Tesla coil or camera flash.

```
TX Animation (charge & discharge):

CHARGE PHASE (border only, ears dark):
1. Row 5 = AMBER       (~30ms)
2. Rows 4-5 = AMBER    (~30ms)
3. Rows 3-5 = AMBER    (~30ms)
4. Rows 2-5 = AMBER    (~30ms)
5. Rows 1-5 = AMBER    (FULL — charged!)
6. Hold... tension builds... (~200ms)

DISCHARGE PHASE (synchronized explosion):
7. Ears: AMBER chase UP (L0→L4, R0→R4)
8. Ears: Tips go LIGHT BLUE
9. Ears: Tips go WHITE — border FLASHES WHITE
10. EVERYTHING OFF AT ONCE — ZAP! 💥
11. Brief darkness (~150ms) — dramatic pause

→ repeat

Timing: ~500ms charge + ~400ms discharge = ~1 second cycle
Colors: Amber charge, white flash
Effect: Charge... charge... BOOM! ⚡ Like lightning or a camera flash
```

**Complete Animation Flow:**

```
TX "Capacitor Discharge":              RX "Filling The Tank":

    [charge border bottom→top]              [ear pulse]
              ↓                                  ↓
    ┌──────────────┐                       Row 5 ON
    │ ████████████ │ FULL!                     ↓
    │ ████████████ │                       [ear pulse]
    │ ████████████ │                            ↓
    └──────────────┘                       Row 4 ON
              ↓                                  ↓
    [ears chase UP]                        [ear pulse]
              ↓                                  ↓
         ⚡ ZAP! ⚡                          Row 3 ON
    [everything WHITE]                          ...
              ↓                                  ↓
       ALL OFF 💥                          [border FULL]
                                                ↓
                                           ALL OFF

  Charge → EXPLODE!                    Pulse → Fill → Pulse → Fill
```

**Display Border - Low Battery Warning:**

- Border flashes red when battery critical (<10%)
- Overrides other animations to alert user

### Blinky Power Budget

**Worst case (all white, full brightness):**
- 26 WS2812B × 60mA = 1,560mA ❌ (would drain battery in 1 hour)
- PWR LED: ~10mA (negligible)

**Realistic usage with smart animations:**

| Mode | Description | Avg Current |
|------|-------------|-------------|
| Sleep | PWR LED only @ dim | ~3mA |
| Idle | Border rainbow @ 25%, ear tips breathing | ~70mA |
| TX active | Capacitor charge + discharge | ~120mA |
| RX active | Ear pulses + border fill | ~100mA |
| Max brightness | All @ 100% (not recommended) | ~380mA |

**Battery life estimates (2000mAh):**

| Mode | LED Current | System Total | Runtime |
|------|-------------|--------------|---------|
| Sleep | 3mA | ~50mA | ~40 hours |
| Idle blinky | 70mA | ~250mA | ~8 hours |
| Active TX/RX | 110mA | ~280mA | ~7 hours |

**Power saving features (firmware):**
- Auto-dim after 30s inactivity
- Brightness menu setting (25%, 50%, 75%, 100%)
- "Battery saver" mode: status LEDs only, border off
- Airplane mode: minimal LED activity

### Blinky PCB Routing

**Rabbit Ears:**
- LEDs placed along PCB outline of each ear shape
- Data chain: up left ear (L0→L4), jump to right ear base, up right ear (R0→R4)
- Jump from L4 to R0 routes across carrier board area (back layer or around edge)
- Ground pours on both layers for solid return path

**Display Border:**
- 16 LEDs in rectangular ring around display module
- Top: 5 LEDs (B0-B4), ~12mm spacing
- Right: 3 LEDs (B5-B7), ~15mm spacing
- Bottom: 5 LEDs (B8-B12), ~12mm spacing
- Left: 3 LEDs (B13-B15), ~15mm spacing
- Chain is continuous, clockwise from top-left

**Bypass Capacitors:**
- Each WS2812B requires 100nF bypass cap
- Place as close to VDD/VSS pins as possible
- 0402 package to save space

### Blinky BOM

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| LED_L0-L4 | 5 | WS2812B | 5050 | Fab | Left rabbit ear |
| LED_R0-R4 | 5 | WS2812B | 5050 | Fab | Right rabbit ear |
| LED_B0-B15 | 16 | WS2812B | 5050 | Fab | Display border |
| C_LED | 26 | 100nF | 0402 | Fab | Bypass capacitors |

**Cost:**
- 26× WS2812B @ $0.08 = $2.08
- 26× 100nF 0402 @ $0.01 = $0.26
- **Total blinky: ~$2.35**

Note: PWR LED (traditional RGB) is listed in User Controls BOM.

---

## PCB Design

The physical PCB design (outline, dimensions, component placement, manufacturing specs) is documented separately:

**See: [hardware/pcb-design.md](../hardware/pcb-design.md)**

Key specs:
- **Body:** 120mm × 95mm with 8mm corner radius
- **Ears:** 50mm long, 30° angle, tapered 15mm→8mm
- **Total height:** ~140mm
- **Color:** Purple soldermask (Simpsons TV aesthetic)
- **Layers:** 4-layer recommended

---

## Testing Notes

### Power System Verification (Main Board)

1. Verify 3.3V rail: 3.3V ±0.1V across full battery range (4.2V → 3.0V)
2. Load test: Apply 500mA load, verify no dropout
3. Efficiency: Measure input/output power at various loads
4. TPS22919: Verify clean enable/disable, check inrush current

### Carrier Board RF Verification

Test carrier independently before mating with main badge:

1. **Continuity:** SA818 ANT pin to SMA center pin
2. **VNA test (if available):** Check return loss (S11), target better than -10dB across band
3. **Power test:** Verify TX output power with dummy load (1W expected)
4. **Thermal:** Monitor SA818 temperature during extended TX
5. **Antenna match:** Verify SWR with included stubby antenna

### Audio Verification

**TX path testing:**
1. Generate 1kHz sine wave via I2S to PCM5102A
2. Measure at VOUTL: expect ~3Vpp
3. Measure after attenuator (R1/R2): expect ~30mVpp
4. Connect to oscilloscope and verify clean waveform, no clipping
5. Sweep through SSTV frequency range (1200-2300 Hz), verify consistent level

**RX path testing:**
1. Inject known audio signal into RX filter input (simulating SA818 SPK)
2. Measure DC level at ADC input: expect ~1.65V (bias)
3. Measure AC signal at ADC input: should be centered on 1.65V
4. Sample with RP2350 ADC, verify signal matches input
5. Test with actual SA818 SPK output

**End-to-end audio test:**
1. Generate SSTV test pattern, transmit via SA818
2. Receive with another radio/SDR, verify clean audio
3. Receive SSTV signal from external source, decode on badge
4. Badge-to-badge test at various distances

### Display Verification

1. **SPI communication:** Send test pattern, verify display responds
2. **Color test:** Display red, green, blue, white screens — verify no stuck pixels or color issues
3. **Orientation:** Verify landscape mode (320 wide × 240 tall)
4. **Backlight PWM:** Test dimming from 0% to 100%, verify smooth transition
5. **SD card access:** Read/write test file via shared SPI bus
6. **Bus arbitration:** Verify display CS and SD CS don't conflict
7. **Frame rate:** Measure actual update time for full-screen refresh
8. **SSTV image test:** Display actual SSTV test pattern, verify correct aspect ratio

### Camera Verification

1. **I2C/SCCB communication:** Read OV2640 chip ID registers (0x0A, 0x0B)
2. **XCLK generation:** Verify 20MHz clock output with oscilloscope
3. **DVP signals:** Verify VSYNC, HREF, PCLK timing with scope
4. **PIO capture:** Capture single frame to RAM, verify data integrity
5. **Resolution test:** Configure for QVGA (320×240), verify frame size
6. **Color format:** Test RGB565 output, verify color accuracy
7. **Viewfinder test:** Continuous capture → display loop, measure FPS
8. **Exposure/brightness:** Test SCCB register writes for image adjustment
9. **JPEG mode:** Test JPEG capture for SD card storage
10. **Power management:** Test PWDN pin, verify current draw in sleep

### User Controls Verification

1. **D-pad switches:** Press each direction, verify GPIO reads LOW
2. **Photo button:** Press and verify GPIO response
3. **Airplane mode switch:** Toggle switch, verify GPIO state changes
4. **Debounce test:** Rapid button presses, verify no double-triggers in firmware
5. **PWR LED (Traditional RGB):**
   - Test each channel individually (R, G, B)
   - Test color mixing: yellow (R+G), cyan (G+B), magenta (R+B), white (R+G+B)
   - Test PWM dimming on each channel
   - Verify battery status colors match LC709203 fuel gauge readings
   - Test blinking mode for critical battery warning

6. **WS2812B Blinky Chain (26 LEDs):**
   - Send full chain test pattern via PIO, verify all 26 LEDs respond
   - LEDs #0-4 (Left Ear): Test lightning TX animation, materialize RX animation
   - LEDs #5-9 (Right Ear): Test animations, verify sync with left ear
   - LEDs #10-25 (Border): Test rainbow chase, waterfall fill effects
   - Test ear tip breathing animation (idle)
   - Test TX capacitor discharge effect (charge → ZAP!)
   - Test RX filling tank effect (pulse per row)
   - Verify all animations are smooth (~30-60ms per step)
   - Test power consumption at various brightness levels
   - Verify auto-dim and power saving modes
7. **Combination test:** Hold multiple D-pad directions, verify all register correctly

### Integrated System Test

1. Plug carrier into main badge
2. Verify band auto-detection (ID pin read correctly)
3. Test UART communication with SA818
4. Test audio path (MIC/SPK signals)
5. Test PTT and power control
6. Full TX/RX functional test

---

## Assembly Notes

### Prototype Build

**Main board (hot air assembly):**
- TPS63001 (QFN-10)
- TPS22919 (SOT-23-5)
- PCM5102A (TSSOP-20)
- All 0402/0603 passives
- RP2350 and other fine-pitch ICs
- Female header sockets (J1-J4) — through-hole, easy

**Carrier board (hot air assembly):**
- SA818 module (2mm pitch castellated — needs careful alignment)
- SMA connector (through-hole or reflow depending on part)
- 0402/0603 passives
- Male headers (J1-J4) — through-hole, easy

### Production Kit Assembly

**Fab pre-assembles:**

*Main badge:*
- All SMD components
- Through-hole header sockets (J1-J4) — could be user-assembled if preferred

*Carrier boards (separate assembly):*
- SA818 module soldered to carrier
- SMA connector
- Decoupling caps and ID resistor
- Male headers (could be user-assembled if preferred)
- Antenna included in kit

**User assembles at conference (main badge):**
- Tactile switches and buttons
- Slide switches (airplane mode, etc.)
- Battery connector (JST-PH)
- SAO headers
- Audio filter components (0603 resistors and caps — R1-R6, C3-C6)
- Optional: header sockets if not pre-installed

**User assembles at conference (carrier):**
- Optional: male headers if not pre-installed
- Screw on antenna
- Plug carrier into main badge

### Assembly Tip: Antenna Tilt

To angle the antenna slightly backward (more comfortable, won't bump chin):
- When soldering J3/J4 (top mechanical headers) to carrier
- Don't push pins fully through
- Leave ~1-2mm extra on top side
- This tilts the carrier back ~10-15° when installed

---

## Project Status

Design status and open items are tracked in a separate document:

**See: [project-status.md](project-status.md)**

---

## Design Revision History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2025-02 | Initial power system redesign (TPS63001 + TPS22919) |
| 2.0 | 2025-02 | Full-width carrier board, all RF on carrier, front-mount overlay design |
| 2.1 | 2025-02 | Audio interface: PCM5102A DAC + RP2350 ADC, filter/bias circuits |
| 2.2 | 2025-02 | Display interface: ILI9341 320×240, PWM backlight, shared SPI with SD |
| 2.3 | 2025-02 | Camera interface: OV2640 parallel DVP, requires RP2350B (QFN-80) |
| 2.4 | 2025-02 | User controls: 5-way D-pad, photo button, airplane switch, status LEDs |
| 2.5 | 2025-02 | All status LEDs consolidated to 2× WS2812B chain (saves 2 GPIOs), rainbow idle animation on TX/RX LED |
| 2.6 | 2025-02 | Blinky LED matrix: 26 WS2812B (10 ears + 16 border), Simpsons TV aesthetic |
| 2.7 | 2025-02 | PWR LED changed to traditional RGB (separate from chain), TX/RX status LED removed (animations are the indicator) |
| 2.8 | 2025-02 | Complete GPIO pin assignment (48/48 allocated), camera RST/PWDN tied to free pins for SAO |
| 2.9 | 2025-02 | USB-C interface: through-hole connector, VBUS + data ESD protection, CC resistors for sink ID |
| 2.10 | 2025-02 | Debug interface: Tag-Connect TC2030-CTX-NL pogo pads (no header) |
| 2.11 | 2025-02 | Battery management: MCP73871 charger + LC709203F fuel gauge, shared thermistor |
| 2.12 | 2025-02 | Crystal oscillator (12MHz) and external flash (W25Q128 16MB) circuits |
| 2.13 | 2025-02 | BOOTSEL button added for UF2 bootloader, Open Items audit and cleanup |
| 2.14 | 2025-02 | I2C pullups (4.7kΩ) added for shared bus |
| 2.15 | 2025-02 | PCB outline design: Simpsons TV shape, dimensions, component zones, mounting |
| 2.16 | 2025-02 | Split PCB design to separate file (hardware/pcb-design.md) |
| 2.17 | 2025-02 | Split project status/open items to separate file (docs/project-status.md) |
| 2.18 | 2025-02 | RP2350 internal voltage regulator: added L2 (3.3µH inductor), C_DVDD (1µF), power pin documentation |

---

## References

- [TPS63001 Datasheet](https://www.ti.com/product/TPS63001)
- [TPS22919 Datasheet](https://www.ti.com/product/TPS22919)
- [PCM5102A Datasheet](https://www.ti.com/product/PCM5102A)
- [ILI9341 Datasheet](https://cdn-shop.adafruit.com/datasheets/ILI9341.pdf)
- [OV2640 Datasheet](https://www.uctronics.com/download/cam_module/OV2640DS.pdf)
- [SA818 Module Documentation](https://www.nicerf.com/product/sa818-vhf-uhf-band-embedded-walkie-talkie-module.html)
- [Saturn PCB Toolkit](https://saturnpcb.com/saturn-pcb-toolkit/) - Impedance calculator
