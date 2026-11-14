# Electrical Design

This document describes the electrical design of the DEFCON SSTV Badge, including circuit schematics, component specifications, and pin assignments.

*For design rationale and trade-off analyses, see [Engineer's Notebook: Electrical Design Decisions](engineers-notebook/electrical-design-decisions.md).*

---

## Power System

### Architecture

The SA818 runs directly from the LiPo battery (via load switch), not through the 3.3V regulator. This separates the 1.7A TX current spikes from the main digital rail.

The 26 WS2812B LEDs have their own 4.0V rail (VLED) from a second buck-boost converter, U11 (TPS63000), fed from the same VSYS node as the 3.3V regulator and the load switch. It is not shown in the diagram below; see "LED Supply Rail".

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
| Misc (PWR LED, etc.) | ~20mA | The WS2812B LEDs are on the separate VLED rail, not this one |
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
| L1 | 2.2µH | 3x3mm | Coilcraft XAL3030 or similar (TPS63001 datasheet value) |
| C_IN1 | 10µF | 0805 | X5R/X7R, 10V |
| C_IN2 | 10µF | 0805 | X5R/X7R, 10V |
| C_OUT1 | 10µF | 0805 | X5R/X7R, 10V |
| C_OUT2 | 10µF | 0805 | X5R/X7R, 10V |
| R_PS | 1MΩ | 0402 | Power save threshold (optional) |

## LED Supply Rail (VLED, TPS63000)

### Purpose

The 26 WS2812B LEDs run from their own 4.0 V rail, **VLED**, made by a second buck-boost converter (U11). They are no longer on the 3.3 V rail.

- The WS2812B-2020 datasheet specifies a 3.7–5.3 V supply and the 5050 part 3.5–5.3 V. At 3.3 V both are out of spec, and blue is the first colour to suffer.
- The LED data line is driven by a 3.3 V GPIO. The 5050 datasheet requires a data-high level of 0.7 × VDD, which is 2.8 V at a 4.0 V supply (met by 3.3 V logic with 0.5 V to spare) but 3.5 V at a 5 V supply (not met). 4.0 V is therefore high enough for the supply spec and low enough for the data spec.
- A buck-boost holds 4.0 V whether VSYS is 3.0 V (battery nearly empty) or about 5 V (USB connected). A boost-only converter would pass 5 V through on USB.

See `engineers-notebook/led-supply-rail.md` for the options that were compared.

### Circuit

```
    VSYS ──┬──────────┬──► VIN, VINA, EN        VOUT ──┬───────┬──────┬──► VLED (4.0 V)
           │          │                                │       │      │
         C79        C80        U11 TPS63000          R23     C83    C81, C82
         10µF      100nF       L9 2.2µH across        1M    2.2pF   2× 10µF
           │          │        L1 / L2                 │       │      │
          GND        GND                          FB ──┴───────┘     GND
                               PS/SYNC ── GND          │
                                                      R24 143k
                                                       │
                                                      GND
```

| Ref | Value | Package | Notes |
|-----|-------|---------|-------|
| U11 | TPS63000DRCR | VSON-10 (3×3mm) | Adjustable version of U5 (same footprint and pinout) |
| L9 | 2.2µH | 3×3mm | Same part as L1 |
| C79 | 10µF | 0805 | Input |
| C80 | 100nF | 0402 | VINA decoupling |
| C81, C82 | 10µF | 0805 | Output |
| R23 | 1MΩ 1% | 0603 | Feedback, upper |
| R24 | 143kΩ 1% | 0603 | Feedback, lower |
| C83 | 2.2pF | 0402 | Feed-forward across R23 (datasheet: 2.2 µs / R23) |

**Output voltage:** VOUT = 0.5 V × (1 + R23/R24) = 0.5 × (1 + 1000/143) = **4.00 V**

**PS/SYNC is tied to ground**, which enables power-save mode. The LEDs do not care about ripple, and the rail idles at a few tens of milliamps where power-save mode is more efficient. (U5 does the opposite, for the sake of the audio path.)

**EN is tied to VSYS**, so the rail is always on. A dark WS2812B still draws its quiescent current, so sleep current includes the 26 LEDs. Controlling EN from a spare GPIO would remove that; it is not implemented.

### Limits

- **Current:** the TPS6300x has 1.8 A switches. Boosting a nearly flat battery to 4.0 V, plan on no more than about 0.8 A out. All 26 LEDs at full white would ask for about 1.5 A, so **firmware must cap total LED brightness**. The planned animations draw 70–120 mA.
- **Battery cost:** the LEDs are constant-current, so they take about 20 % more power at 4.0 V than at 3.3 V — roughly 15 mA more from the battery at idle.

### Layout

- U11 and its parts are on the back, at about (10–22, 89–104) mm, between the display socket J3 and the charger. The layout is a copy of U5's.
- VLED is distributed on the inner power layer (In2) as tracks, not as a plane. The trunk follows the board edges so that it does not cut the 3.3 V plane through the middle of the board: from U11 up the left edge, along the top edge, and into each ear. Border rows and columns chain LED to LED.
- The radio supply track on the same layer blocks the top row between D13 and D14; that one link is on the front layer.
- Trunk resistance from U11 to the far ear tip is about 0.2 Ω, so the drop is under 0.1 V at 300 mA.
- VLED is in the `Power` net class (0.2 mm clearance).

---

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
| Bulk Capacitor | 1µF ceramic | 0603 | VBUS decoupling at the connector. Total on VBUS is about 7.9µF (this 1µF + 2.2µF and 4.7µF at the charger), under the 10µF USB inrush limit | ~$0.05 |
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
| J_BAT (J1) | JST-PH 2-pin, SMD side-entry (S2B-PH-SM4-TB) | Standard LiPo connector |

**Polarity:** Red (+) on pin 1, Black (-) on pin 2. Match common LiPo battery wiring.

**Why surface-mount:** J1 sits on the back of the board inside the display area. A through-hole connector there put battery positive and ground pins, 2 mm apart, through to the front directly behind the display module. The SMD part leaves nothing on the front. It is side-entry: the plug lies flat against the back of the board and enters from the top-of-board side. See `engineers-notebook/main-badge-review-2026-11.md`, section 7.

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
    │J3 (GND)                                                  J4 (GND)│  │
    │mechanical           ┌──────┐                              mech   │  │
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
| J3 (top-left) | 7-12mm | 1x3 mechanical, GND (KiCad: J16/J22) |
| J1 (bottom-left) | 32-50mm | 1x8 signal (power + audio) (KiCad: J14/J18) |
| SMA connector | ~47mm | Left of center (KiCad: J13/J19) |
| J2 (bottom-right) | 70-88mm | 1x8 signal (control + data) (KiCad: J15/J20) |
| J4 (top-right) | 108-113mm | 1x3 mechanical, GND (KiCad: J17/J21) |

The 4-point mounting (J1, J2 at bottom + J3, J4 at top corners) prevents wobble and keeps the carrier securely attached.

### Carrier Header Pinouts

**J1 (Left, 1x8) — Power and Audio:**
*(KiCad: J14 on VHF carrier, J18 on UHF carrier; main board socket J7)*

| Pin | Signal | Notes |
|-----|--------|-------|
| 1 | VCC | From TPS22919 on main board |
| 2 | GND | Ground |
| 3 | GND | Ground |
| 4 | MIC+ | TX audio input to SA818 |
| 5 | GND | Ground |
| 6 | SPK+ | RX audio output from SA818 |
| 7 | GND | Ground |
| 8 | GND | Ground |

**J2 (Right, 1x8) — Control and Data:**
*(KiCad: J15 on VHF carrier, J20 on UHF carrier; main board socket J8)*

| Pin | Signal | Notes |
|-----|--------|-------|
| 1 | RXD (UART) | UART RX from SA818 TXD (main badge net: SA818_RX) |
| 2 | TXD (UART) | UART TX to SA818 RXD (main badge net: SA818_TX) |
| 3 | PTT | Push-to-talk (active low) |
| 4 | PD | Power down (active low; also enables TPS22919 load switch) |
| 5 | HL | High/low power select |
| 6 | SQ | Squelch detect |
| 7 | ID | Band detect (GND=VHF, pulled up to SA818 VCC=UHF) |
| 8 | GND | Ground |

> **Locked pinout note:** these pin assignments reflect the main badge schematic and PCB (routing complete, design locked). Both carriers now match in schematic and PCB, with the headers mounted on the back of the carrier — see [carrier-rewiring-checklist.md](engineers-notebook/carrier-rewiring-checklist.md) for the history.

**J3, J4 (Top corners, 1x3 each) — Mechanical:**

All pins connected to GND for RF shielding and mechanical stability.

### Band Auto-Detection

Instead of a DIP switch, the carrier includes a band ID resistor:

| Module | ID Pin State | Implementation |
|--------|--------------|----------------|
| SA818-V (VHF) | LOW | 0Ω resistor to GND on carrier |
| SA818-U (UHF) | HIGH | 10kΩ pull-up (R18) to SA818 VCC on carrier |
| No module | LOW | MCU internal pulldown; same reading as VHF, so detect via UART timeout |

Firmware reads ID pin on startup and configures frequency limits accordingly.

> **Firmware note:** the UHF pull-up is fed from SA818 VCC, which is switched by the TPS22919 load switch. The ID pin only reads HIGH while the carrier is powered, so enable SA818 power before sampling it.

### RF Section (On Carrier)

All RF design is contained on the carrier board:

**Harmonic Low-Pass Filter:**
- Discrete 7-pole Chebyshev LPF (0.1 dB ripple, 50Ω) between the SA818 ANT pin and the SMA, for FCC Part 97 spurious-emission compliance
- VHF: fc = 155 MHz — C58/C61 = 22pF, C59/C60 = 39pF, L3/L5 = 68nH, L4 = 82nH
- UHF: C64/C67 = 6.8pF, C65/C66 = 12pF, L6/L8 = 22nH, L7 = 24nH (reduced from the ideal fc = 470 MHz values to leave room for layout parasitics)
- 0402 C0G caps, Coilcraft 0603HP wirewound inductors, two DNP tuning footprints per filter
- Values are calculated starting points, to be confirmed by prototype tuning
- Rationale: [harmonic-filter-design.md](engineers-notebook/harmonic-filter-design.md); as-built layout: `hardware/sa818-v-carrier/LPF-INTEGRATION.md` and `hardware/sa818-u-carrier/LPF-INTEGRATION.md`

**RF Trace:**
- SA818 ANT pad → filter → SMA centre pin, in a straight line on F.Cu (~27mm end to end)
- 0.8mm wide on the 2-layer 1.6mm carrier; segments are 5-10mm, far shorter than a wavelength, so a true 3mm 50Ω microstrip is not needed (and would not fit the 0402 pads)
- Unbroken ground pour on B.Cu under the whole RF path

**Ground Via Stitching:**
```
    SA818 ANT
         │
    ○  ○ ┃ ○  ○      ← Via fence on both sides
         ┃
    ○  ○ ┃ ○  ○        0.3mm drill; as built: 4mm pitch (VHF), 2mm (UHF)
         ┃
    ○  ○ ┃ ○  ○
         ┃
      ┌──┴──┐
      │ SMA │
      └─────┘
```

**SMA Connector:**
- Edge-mount (Amphenol 132289 footprint)
- Position at top edge, ~47mm from the left edge
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
| J_SMA | SMA edge-mount | PCB edge | Female, 50Ω |
| C1 | 100nF | 0402/0603 | Decoupling |
| C2 | 10µF | 0603/0805 | Bulk |
| R_ID | 0Ω / 10kΩ | 0402 | VHF: 0Ω to GND. UHF: 10kΩ pull-up to SA818 VCC (KiCad: R18) |
| LPF | 4× C, 3× L (+2 DNP) | 0402 / 0603 | Harmonic low-pass filter, band-specific values (see above) |
| J1 | 1x8 male header | 0.1" pitch | Through-hole, pointing backward |
| J2 | 1x8 male header | 0.1" pitch | Through-hole, pointing backward |
| J3 | 1x3 male header | 0.1" pitch | Through-hole, all pins GND (KiCad: J16/J22) |
| J4 | 1x3 male header | 0.1" pitch | Through-hole, all pins GND (KiCad: J17/J21) |
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
     │  □ □ □                                               □ □ □  │ ← J3, J4 (J11, J12)
     │  (J3)                                                 (J4)  │   1x3, GND
     │                                                             │
    ...                                                           ...
     │                                                             │
     │                                                             │
     │  □ □ □ □ □ □ □ □                           □ □ □ □ □ □ □ □  │ ← J1, J2 (J7, J8)
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

**J1 Socket (1x8, KiCad: J7) — Power and Audio:**

| Pin | Signal | Connects To |
|-----|--------|-------------|
| 1 | VCC | TPS22919 output |
| 2 | GND | Ground plane |
| 3 | MIC+ | TX audio filter output |
| 4 | GND | Ground plane |
| 5 | SPK+ | RX audio filter input |
| 6 | GND | Ground plane |
| 7 | GND | Ground plane |
| 8 | GND | Ground plane |

**J2 Socket (1x8, KiCad: J8) — Control and Data:**

| Pin | Signal | Connects To |
|-----|--------|-------------|
| 1 | PTT | RP2350 GPIO (SA818_PTT) |
| 2 | PD | RP2350 GPIO (SA818_PD / TPS22919 EN) |
| 3 | HL | RP2350 GPIO (SA818_HL) |
| 4 | SQ | RP2350 GPIO (SA818_SQ) |
| 5 | TXD | RP2350 UART TX (SA818_TX) |
| 6 | RXD | RP2350 UART RX (SA818_RX) |
| 7 | ID | RP2350 GPIO (SA818_ID, with internal pulldown) |
| 8 | GND | Ground plane |

**J3, J4 Sockets (1x3 each) — Mechanical:**

All pins connected to GND for RF shielding and mechanical stability.

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
- **SD Card:** Standalone SD socket (J10), shared SPI bus with display

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
| SD_CS | SD Card Select | GPIO | For standalone SD socket (J10) |

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

The standalone SD card socket (J10) shares the SPI bus with the display. This means:
- Only one device active at a time (display OR SD card)
- Must manage separate CS pins for each device
- May need to adjust SPI clock when switching (SD cards are pickier)

**This is fine for SSTV** because operations are naturally sequential:
- Load image from SD → then display it
- Receive SSTV → decode to RAM → display → optionally save to SD

No workflow requires simultaneous display update and SD access.

**Pull-ups:** four 10kΩ resistors to 3.3V — R25 on SD_CS, R26 on SPI_MISO, R27 on the card's DAT2 pin and R28 on its DAT1 pin.
- The chip-select pull-up keeps the card deselected while the RP2350 is in reset, when its pins default to a weak pull-down.
- DAT1 and DAT2 are unused in SPI mode. They are pulled up rather than left floating, the usual recommendation for unused SD data lines (not checked against the SD specification text).

**Card access:** the socket (J10, on the back) faces the right-hand board edge. The inserted card's outer end is flush with the edge at x = 120mm. The card overhangs the strip between the socket's contact pads and the edge (x ≈ 116–120mm, y ≈ 25.4–36.5mm on the back); only traces and tented vias may be placed there.

### Backlight PWM Circuit

The 14-pin ILI9341 module that J3 is pinned for lights its backlight when the LED pin is driven **high** (tie it to 3.3 V for always-on). To add PWM dimming, a P-channel MOSFET switches 3.3 V onto that pin:

```
                    +3V3
                     │
                     │ S
    RP2350      ┌──┤ G   Q1 AO3401A (P-channel, SOT-23)
    ┌───────┐   │    │ D
    │  GPIO ├─[1kΩ]  └──────────────────────► Display module LED pin (J3 pin 8)
    │  (BL) │  R11
    └───────┘

    PWM LOW  → MOSFET ON  → Backlight ON
    PWM HIGH → MOSFET OFF → Backlight OFF
    Brightness = fraction of the time the pin is LOW (inverted PWM)
```

**PWM Frequency:** 1-10 kHz (higher = no visible flicker)

**Reset behaviour:** the gate has no pull-up. While the RP2350 is in reset or before firmware configures GPIO29, the pin's default pull-down holds the gate low, so the backlight is on. Firmware turns it off by driving GPIO29 high.

**Note:** An earlier revision used a 2N7002 switching the LED pin to ground. That assumed the LED pin was the backlight cathode, which is wrong for this module (found in the 2026-11-14 schematic review). A module whose pin is the cathode would need the old low-side circuit instead.

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
              AO3401A│ D ←─┘                           │             │
                    ├ S                                │             │
                    │                                  │             │
                  +3V3                                 └─────────────┘
```

### GPIO Summary for Display

| Function | GPIO | Notes |
|----------|------|-------|
| SPI SCK | GPIO6 | Shared SPI clock (SPI0 F0) |
| SPI TX (MOSI) | GPIO7 | Shared SPI data out (SPI0 F0) |
| SPI RX (MISO) | GPIO4 | Shared SPI data in (SPI0 F0) |
| DISP_CS | GPIO5 | Display chip select |
| DISP_DC | GPIO19 | Data/Command select |
| DISP_RST | GPIO20 | Display reset |
| DISP_BL | GPIO29 | Backlight PWM |
| SD_CS | GPIO26 | SD card chip select |

**Total: 6-7 dedicated GPIOs** (plus SPI bus which may be shared with other peripherals)

### Display BOM

| Ref | Value | Package | Assembly | Notes |
|-----|-------|---------|----------|-------|
| U_DISP | ILI9341 2.4" module | Breakout | User | 320×240, SPI, w/ SD slot |
| Q_BL (Q1) | AO3401A | SOT-23 | Fab | Backlight high-side P-MOSFET |
| R_BL | 1kΩ | 0603 | User | Gate resistor |

**Cost:** ~$8-12 for module + ~$0.10 for MOSFET/resistor

### Orientation and Mounting

- **Orientation:** Landscape (320 wide × 240 tall)
- **Mounting:** Module breakout board, likely with pin headers or direct solder
- **Position:** Center of main badge, below the carrier board area

The display is the main visual element — position it prominently for the "retro TV" aesthetic.

---

## Camera Interface

> # ⚠️ THE OV2640 MODULE MUST HAVE ITS OWN ON-BOARD CLOCK OSCILLATOR ⚠️
>
> **The main badge does NOT supply a camera clock.** There is no XCLK net anywhere on the board, the RP2350B has no GPIO assigned to it, and the J4 camera connector has no XCLK pin (pin 18 is not connected). The main badge PCB is routed and locked, so this cannot be added later.
>
> **An OV2640 module that expects an external XCLK will not work on this badge.** With no master clock the sensor produces no pixel clock and no image.
>
> **Before buying modules, confirm all three:**
> 1. The module has a crystal or oscillator can on the PCB (typically 12 MHz or 24 MHz) and the listing or schematic says the sensor is clocked from it.
> 2. The 2×9 header pinout matches the J4 table below **exactly** — in particular, there is **no XCLK pin** on the header.
> 3. You have tested one sample: with power applied, RESET high and PWDN low (as the badge wires them) and no clock fed in, PCLK toggles on a scope.
>
> Buy and test one sample before ordering in quantity. Many common OV2640 breakouts expect the host to supply XCLK and are **not** compatible.

### Overview

The camera captures images for SSTV transmission and provides live viewfinder preview.

**Specifications:**
- **Sensor:** OV2640 2MP CMOS
- **Module:** Pre-assembled breakout with 2.54mm 2×9 pin header (user-solderable), **with its own on-board clock oscillator** (required — see callout above)
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
| XCLK | Master clock | — | **Not connected** | Must be generated on the camera module by its own oscillator; the badge does not drive it |
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

### XCLK (Master Clock) — Supplied by the Module, Not the Badge

The OV2640 sensor needs a master clock (XCLK), typically 12-24 MHz. **On this badge that clock must come from an oscillator on the camera module itself.**

Earlier revisions of this document described generating XCLK from an RP2350 PWM pin. The final schematic and PCB do not do that: GPIO19 is DISP_DC, GPIO20 is DISP_RST, no XCLK net exists, and J4 has no XCLK pin.

Consequences:
- The module's oscillator frequency sets the sensor's input clock. Firmware cannot change it; frame rate and PCLK rate are adjusted only through the OV2640's own PLL and divider registers over SCCB.
- Firmware must be configured for the module's oscillator frequency before programming the sensor's clock registers — a 12 MHz module and a 24 MHz module need different divider settings for the same frame rate.
- A module without an oscillator cannot be rescued with a bodge wire to a spare GPIO in any tidy way: J4 has no pin for it.

### Camera Module Selection

For user-solderable assembly, use an OV2640 module with 2.54mm pin headers.

**Required:** an "OV2640 DVP module" with a 2×9 pin header (Conn_02x09_Socket), an **on-board clock oscillator**, and a header pinout matching the J4 table below (no XCLK pin)

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
    │  ○ ○ ○ ○ ○ ○ ○ ○ ○           │  ← 2×9 (2.54mm) through-hole pins
    │  ○ ○ ○ ○ ○ ○ ○ ○ ○           │     (user-solderable)
    └───────────────────────────────┘

    Typical size: ~25mm × 25mm
```

**J4 Camera Connector Pin Mapping (2×9, Conn_02x09_Socket):**

| Pin | Signal | Pin | Signal |
|-----|--------|-----|--------|
| 1 | GND | 2 | +3.3V |
| 3 | I2C_SCL | 4 | CAM_VSYNC |
| 5 | I2C_SDA | 6 | CAM_HREF |
| 7 | CAM_D0 | 8 | RST (10k pull-up to +3.3V) |
| 9 | CAM_D2 | 10 | CAM_D1 |
| 11 | CAM_D4 | 12 | CAM_D3 |
| 13 | CAM_D6 | 14 | CAM_D5 |
| 15 | CAM_PCLK | 16 | CAM_D7 |
| 17 | PWDN (10k pull-down to GND) | 18 | NC |

**Search terms:** "OV2640 camera module DVP" plus "on-board oscillator" / "with crystal" / "no external clock". Check the board photo for a crystal or oscillator can and check the header pinout against the table above — do not rely on the listing title.

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
    │       │        (not connected to the badge)     │ XCLK ◄── on-board
    │       │                                         │   oscillator │
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
| I2C SCL | GPIO45 | SCCB clock (shared with SAO, fuel gauge) |
| I2C SDA | GPIO44 | SCCB data (shared with SAO, fuel gauge) |
| CAM_XCLK | — | **Not driven by the badge** — module must have its own oscillator |
| CAM_PCLK | GPIO16 | Pixel clock input (PIO) |
| CAM_VSYNC | GPIO17 | Frame sync input |
| CAM_HREF | GPIO18 | Line valid input |
| CAM_D0-D7 | GPIO8-15 | 8× parallel data (PIO), consecutive for PIO |
| CAM_RST | — | Tied HIGH via 10kΩ pullup (not GPIO controlled) |
| CAM_PWDN | — | Tied LOW via 10kΩ pulldown (not GPIO controlled) |

**Dedicated GPIOs: 11** (D0-D7, PCLK, VSYNC, HREF; RST and PWDN are tied with resistors, XCLK is not driven)

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
| U_CAM | OV2640 DVP module **with on-board oscillator** | Breakout (2×9 header) | User | 2.54mm 2×9 pin header, no XCLK pin, ~$5-8. Modules needing external XCLK will not work |
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
| Package | QFN-80 (10×10mm) |
| GPIOs | 48 (GPIO0-GPIO47) |
| CPU | Dual ARM Cortex-M33 @ 150MHz |
| SRAM | 520KB |
| Flash | External SPI (16MB in BOM) |
| PIO | 3× PIO blocks, 12 state machines |
| ADC | 8-channel 12-bit (GPIO40-47) |
| PWM | 12 channels |

### GPIO Pin Assignment

47 of 48 GPIOs are allocated (GPIO47 spare). Camera RST/PWDN are tied with resistors to free pins for SAO. GPIOs are organized by physical side of the QFN-80 package to minimize trace crossings. See `docs/engineers-notebook/gpio-optimization.md` for rationale.

**Top Side — Audio + LED (pins 61-64, face carrier headers):**

| GPIO | Function | Notes |
|------|----------|-------|
| 0 | LED_DATA | WS2812B chain (PIO), routes to ear base |
| 1 | I2S_LRCK | Word select to PCM5102A (PIO) |
| 2 | I2S_DIN | Data to PCM5102A (PIO) |
| 3 | I2S_BCK | Bit clock to PCM5102A (PIO) |

**Left Side — Display SPI (pins 1-4, face display center):**

| GPIO | Function | Notes |
|------|----------|-------|
| 4 | SPI0_RX | MISO from SD |
| 5 | DISP_CS | Display chip select (directly driven) |
| 6 | SPI0_SCK | Clock |
| 7 | SPI0_TX | MOSI to display and SD |

**Left Side — Camera DVP (pins 6-20, face display/camera):**

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
| 19 | DISP_DC | Data/Command select (toggles every SPI txn) |
| 20 | DISP_RST | Display reset (infrequent toggle) |

Note: CAM_RST tied HIGH via 10kΩ, CAM_PWDN tied LOW via 10kΩ (not GPIO controlled).

**Bottom Side — Controls + SAO1 (pins 21-28, face bottom edge):**

| GPIO | Function | Notes |
|------|----------|-------|
| 21 | DPAD_UP | Active low, internal pullup |
| 22 | DPAD_DOWN | Active low, internal pullup |
| 23 | DPAD_LEFT | Active low, internal pullup |
| 24 | DPAD_RIGHT | Active low, internal pullup |
| 25 | DPAD_CENTER | Active low, internal pullup |
| 26 | SD_CS | SD card chip select |
| 27 | SAO1_GPIO1 | SAO connector 1, pin 5 (left edge) |
| 28 | SAO1_GPIO2 | SAO connector 1, pin 6 (left edge) |

**Bottom Side — Display Misc + PWR LED (pins 37-40):**

| GPIO | Function | Notes |
|------|----------|-------|
| 29 | DISP_BL | Backlight PWM (via AO3401A, low = on) |
| 30 | PWR_R | PWR LED red channel (PWM) |
| 31 | PWR_G | PWR LED green channel (PWM) |
| 32 | PWR_B | PWR LED blue channel (PWM) |

**Right Side — SA818 Interface (pins 42-48, face carrier headers):**

| GPIO | Function | Notes |
|------|----------|-------|
| 33 | SA818_PTT | Push-to-talk (active low) |
| 34 | SA818_PD | Power down / TPS22919 enable |
| 35 | SA818_HL | High/Low power select |
| 36 | SA818_TX | UART1_TX → SA818 RXD |
| 37 | SA818_RX | UART1_RX ← SA818 TXD |
| 38 | SA818_SQ | Squelch detect input |
| 39 | SA818_ID | Band ID from carrier |

**Right Side — ADC + SAO2 + I2C (pins 49-57):**

| GPIO | Function | Notes |
|------|----------|-------|
| 40 | RX_AUDIO | ADC input from SA818 SPK (ADC0) |
| 41 | SAO2_GPIO1 | SAO connector 2, pin 5 (right edge) |
| 42 | SAO2_GPIO2 | SAO connector 2, pin 6 (right edge) |
| 43 | AIRPLANE | Airplane mode slide switch |
| 44 | I2C0_SDA | Camera SCCB, LC709203, SAO1, SAO2 |
| 45 | I2C0_SCL | Camera SCCB, LC709203, SAO1, SAO2 |
| 46 | (spare) | Unallocated |
| 47 | (spare) | Unallocated |

**Note:** RP2350B ADC channels are on GPIO40-47 (not GPIO26-29 like RP2040). SAO I2C shared on GPIO44/45. VCC=3.3V, GND from power rails.

### GPIO Assignment Summary

```
GPIO 0-1:   I2C (shared bus)
GPIO 2-4:   SPI (display + SD)
GPIO 5-7:   Display control (CS, DC, RST)
GPIO 8-18:  Camera DVP (D0-D7, PCLK, VSYNC, HREF) — no XCLK, module self-clocked
GPIO 20-21: SAO1 GPIOs
GPIO 22-23: Display BL + SD_CS
GPIO 24-25: Audio I2S (BCK, LRCK)
GPIO 26:    DPAD_RIGHT (moved here to free GPIO40 for ADC)
GPIO 27-28: SAO2 GPIOs
GPIO 29:    Audio I2S (DIN)
GPIO 30-36: SA818 (UART + control)
GPIO 37-39: User controls (DPAD UP/DOWN/LEFT)
GPIO 40:    RX Audio ADC (ADC0)
GPIO 41:    User controls (DPAD CENTER)
GPIO 42:    Spare (unallocated)
GPIO 43:    User controls (AIRPLANE)
GPIO 44-46: PWR LED (RGB)
GPIO 47:    LED_DATA (WS2812B blinky)

Total: 47/48 GPIOs allocated (1 spare)
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
| 20 | — | (DISP_RST, not PWM) |
| 29 | PWM6B | DISP_BL (1-10kHz) |
| 30 | PWM7A | PWR_R |
| 31 | PWM7B | PWR_G |
| 32 | PWM8A | PWR_B |

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
| U_MCU | 1 | RP2350B | QFN-80 (10×10mm) | Fab | Dual Cortex-M33, 48 GPIO |
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

### Power Test Pads

Six 1.0mm test pads on the back of the board give probe access to the supply rails:

| Ref | Net | Position (mm) | Near |
|-----|-----|---------------|------|
| TP1 | GND | (37.0, 101.5) | Below the 3.3V regulator |
| TP2 | +3V3 | (41.0, 83.5) | 3.3V regulator output caps |
| TP3 | VBUS | (23.5, 104.75) | Charger input cap C10 |
| TP4 | VSYS | (46.25, 96.75) | 3.3V regulator input cap C13 |
| TP5 | VLED | (14.5, 87.9) | LED rail regulator output |
| TP6 | SA818_VCC | (17.3, 76.2) | Load switch output cap C17 |

The battery voltage can be probed at the battery connector J1, and the 1.1V core rail at C1.

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
| D-pad | 5× tactile switches | 5 | Menu navigation + capture (center doubles as photo button when idle) |
| Airplane mode | Slide switch | 1 | Disable RF (read by MCU) |
| PWR LED | RGB common-anode | 3 | Battery status indicator |
| LED_DATA | WS2812B chain | 1 | 26-LED blinky (ears + border, TX/RX animations) |

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

**Part:** Kingbright AAA3528LSEEZGKQBKS (PLCC-4, 3.5×2.8mm, 1.9mm high, flat top with the LED window in the middle)

**Pinout (from the datasheet drawings):** pin 1 red cathode, pin 2 common anode, pin 3 green cathode, pin 4 blue cathode.

**Location:** on the front, just left of SAO connector J6, at about (103.5, 50.75) mm.

The part was changed on 2026-11-14 from the right-angle APFA3010LSEEZGKQBKC. The two parts use the same LED chips (same forward voltages), so R14–R16 are unchanged.

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

**Radio activity on the same LED (added 2026-11-14):** the PWR LED also shows transmit and receive. Radio activity takes over the LED while it lasts, then the battery colour returns.

| Radio state | Color | R | G | B | Notes |
|-------------|-------|---|---|---|-------|
| Idle | Battery colour (table above) | — | — | — | Steady, dim |
| Receiving (squelch open or SSTV decode in progress) | Blue | 0 | 0 | 255 | Steady |
| Transmitting (PTT asserted) | Magenta | 255 | 0 | 255 | Steady, full brightness |
| Critical battery (<10%) | Blinking red | 255 | 0 | 0 | Overrides radio colours, so a nearly flat battery is never hidden |

Blue and magenta were chosen because neither is a battery colour (green, yellow, red). The colours are firmware-only and can be changed without touching the hardware.

The blinky animations (ear lightning + border effects) still show TX/RX as well. The PWR LED gives the same information when the animations are dimmed, in battery-saver mode, or if the WS2812B chain has a fault. This replaces the earlier position that no separate TX/RX indicator was needed.

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
    │                              ═══════════════════════════      │
    │                               AIRPLANE MODE (slide sw)        │
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
    │ (DPAD_CENTER)          pullup  (doubles as photo capture)   │
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
    D-pad center doubles as photo capture when idle (no separate photo button)
    PWR LED: Traditional RGB, direct GPIO drive (3 GPIOs)
    Blinky: 26× WS2812B chain (1 GPIO) — ears + display border
```

### GPIO Summary for User Controls

| Function | GPIO | Notes |
|----------|------|-------|
| DPAD_UP | GPIO21 | Active low, internal pullup |
| DPAD_DOWN | GPIO22 | Active low, internal pullup |
| DPAD_LEFT | GPIO23 | Active low, internal pullup |
| DPAD_RIGHT | GPIO24 | Active low, internal pullup |
| DPAD_CENTER | GPIO25 | Active low, internal pullup (doubles as photo capture) |
| AIRPLANE | GPIO26 | Active low, internal pullup |
| PWR_R | GPIO30 | PWR LED red channel (PWM) |
| PWR_G | GPIO31 | PWR LED green channel (PWM) |
| PWR_B | GPIO32 | PWR LED blue channel (PWM) |
| LED_DATA | GPIO0 | WS2812B blinky chain (26 LEDs: ears + border) |

**Total: 10 GPIOs** for user controls (6 switches + 4 LED control)

### User Controls BOM

| Ref | Value | Package | Assembly | Notes |
|-----|-------|---------|----------|-------|
| SW_UP, SW_DOWN, SW_LEFT, SW_RIGHT, SW_CENTER | Tactile 6×6mm | Through-hole | User | D-pad switches (center doubles as photo capture) |
| SW_AIRPLANE | Slide switch SPDT | Through-hole | User | Airplane mode |
| LED_PWR (D1) | AAA3528LSEEZGKQBKS | PLCC-4 3.5×2.8mm | Fab | Kingbright RGB, common-anode |
| R_PWR_R | 220Ω | 0603 | Fab | PWR LED red cathode current limit |
| R_PWR_G | 100Ω | 0603 | Fab | PWR LED green cathode current limit |
| R_PWR_B | 100Ω | 0603 | Fab | PWR LED blue cathode current limit |
| WS2812B (×10) | WS2812B | 5050 SMD | Fab | Ear LEDs — blinky chain (see Blinky section) |
| WS2812B-2020 (×16) | WS2812B-2020 | 2020 SMD | Fab | Border LEDs — blinky chain (see Blinky section) |
| C_LED (×26) | 100nF | 0402 | Fab | WS2812B bypass capacitors |

**Cost:** ~$3.50 total (RGB LED ~$0.10, 26× WS2812B ~$2.10, 26× caps ~$0.26, switches ~$1)

All switches use software debounce (no external caps). *See [Engineer's Notebook](engineers-notebook/electrical-design-decisions.md#debouncing-strategy) for rationale.*

### Blinky LED Matrix

The badge features a 26-LED WS2812B chain for full #badgelife blinky appeal. The PCB is cut in the shape of a retro TV (Simpsons-style) with rabbit ear antennas as part of the main board.

Battery status is handled by a separate traditional RGB LED (see Status LED section).

**Dual-Package Approach:**

The chain uses two different WS2812B packages optimized for their location:
- **Ear LEDs (D2-D11):** 10× WS2812B in **5050 package** (5.0×5.0mm) — large and visible on the rabbit ear antennas, 5 per ear
- **Display border LEDs (D12-D27):** 16× WS2812B-2020 in **2020 package** (2.0×2.0mm) — compact for tight spacing around the display, 3×5×3×5 layout
- Each LED has a 100nF bypass cap between VDD and GND
- All 26 LEDs are powered from the 4.0V **VLED** rail (U11, TPS63000), not from 3.3V — see "LED Supply Rail" in the power section. The data line stays at 3.3V logic.

Both packages use the same WS2812B protocol and are daisy-chained on a single data line.

**LED Sections:**

| Section | LEDs | Package | Index | Purpose |
|---------|------|---------|-------|---------|
| Left Ear | 5 | WS2812B 5050 | #0-4 | L0 (base) → L4 (tip) |
| Right Ear | 5 | WS2812B 5050 | #5-9 | R0 (base) → R4 (tip) |
| Display Border | 16 | WS2812B-2020 | #10-25 | B0-B15 clockwise from top-left |
| **Total** | **26** | | | |

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
- 26 WS2812B × 60mA = 1,560mA ❌ (would drain battery in 1 hour, and is about twice what the VLED regulator can supply — firmware must cap total brightness)
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

**Supply:**
- LED VDD pads and bypass caps connect by vias to VLED tracks on the inner power layer (In2); see "LED Supply Rail"

**Bypass Capacitors:**
- Each WS2812B requires 100nF bypass cap
- Place as close to VDD/VSS pins as possible
- 0402 package to save space

### Blinky BOM

| Ref | Qty | Value | Package | Assembly | Notes |
|-----|-----|-------|---------|----------|-------|
| LED_L0-L4 | 5 | WS2812B | 5050 (5.0×5.0mm) | Fab | Left rabbit ear |
| LED_R0-R4 | 5 | WS2812B | 5050 (5.0×5.0mm) | Fab | Right rabbit ear |
| LED_B0-B15 | 16 | WS2812B-2020 | 2020 (2.0×2.0mm) | Fab | Display border |
| C_LED | 26 | 100nF | 0402 | Fab | Bypass capacitors (one per LED) |

**Cost:**
- 10× WS2812B 5050 @ $0.08 = $0.80
- 16× WS2812B-2020 @ $0.06 = $0.96
- 26× 100nF 0402 @ $0.01 = $0.26
- **Total blinky: ~$2.02**

Note: PWR LED (traditional RGB) is listed in User Controls BOM.

---

## PCB Design

The physical PCB design (outline, dimensions, component placement, manufacturing specs) is documented separately:

**See: [hardware/pcb-design.md](../hardware/pcb-design.md)**

Key specs:
- **Body:** 120mm × 120mm with 2mm corner radius (top only, square at leg junctions)
- **Ears:** ~50mm at ~45°, with ~15mm ball tips
- **Feet:** ~15mm at 15° outward with semicircular ends
- **Total height:** ~190mm
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
2. **Module clock:** With the module powered on the badge, confirm PCLK is toggling on a scope — this proves the module's on-board oscillator is clocking the sensor (the badge supplies no XCLK)
3. **DVP signals:** Verify VSYNC, HREF, PCLK timing with scope
4. **PIO capture:** Capture single frame to RAM, verify data integrity
5. **Resolution test:** Configure for QVGA (320×240), verify frame size
6. **Color format:** Test RGB565 output, verify color accuracy
7. **Viewfinder test:** Continuous capture → display loop, measure FPS
8. **Exposure/brightness:** Test SCCB register writes for image adjustment
9. **JPEG mode:** Test JPEG capture for SD card storage
10. **Power management:** Test PWDN pin, verify current draw in sleep

### User Controls Verification

1. **D-pad switches:** Press each direction, verify GPIO reads LOW (center doubles as photo capture)
2. **Airplane mode switch:** Toggle switch, verify GPIO state changes
3. **Debounce test:** Rapid button presses, verify no double-triggers in firmware
4. **PWR LED (Traditional RGB):**
   - Test each channel individually (R, G, B)
   - Test color mixing: yellow (R+G), cyan (G+B), magenta (R+B), white (R+G+B)
   - Test PWM dimming on each channel
   - Verify battery status colors match LC709203 fuel gauge readings
   - Test blinking mode for critical battery warning

5. **WS2812B Blinky Chain (26 LEDs):**
   - Send full chain test pattern via PIO, verify all 26 LEDs respond
   - LEDs #0-4 (Left Ear, 5050): Test lightning TX animation, materialize RX animation
   - LEDs #5-9 (Right Ear, 5050): Test animations, verify sync with left ear
   - LEDs #10-25 (Border, 2020): Test rainbow chase, waterfall fill effects
   - Test ear tip breathing animation (idle)
   - Test TX capacitor discharge effect (charge → ZAP!)
   - Test RX filling tank effect (pulse per row)
   - Verify all animations are smooth (~30-60ms per step)
   - Test power consumption at various brightness levels
   - Verify auto-dim and power saving modes
6. **Combination test:** Hold multiple D-pad directions, verify all register correctly

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
- Female header sockets (J7, J8 signal; J11, J12 mechanical) — through-hole, easy

**Carrier board (hot air assembly):**
- SA818 module (2mm pitch castellated — needs careful alignment)
- SMA connector (through-hole or reflow depending on part)
- 0402/0603 passives
- Male headers (J14-J17 for VHF, J18-J22 for UHF) — through-hole, easy

### Production Kit Assembly

**Fab pre-assembles:**

*Main badge:*
- All SMD components
- Through-hole header sockets (J7, J8, J11, J12) — could be user-assembled if preferred

*Carrier boards (separate assembly):*
- SA818 module soldered to carrier
- SMA connector
- Decoupling caps and ID resistor
- Male headers (could be user-assembled if preferred)
- Antenna included in kit

**User assembles at conference (main badge):**
- Tactile switches and buttons
- Slide switches (airplane mode, etc.)
- SAO headers
- Audio filter components (0603 resistors and caps — R1-R6, C3-C6)
- Optional: header sockets if not pre-installed

**User assembles at conference (carrier):**
- Optional: male headers if not pre-installed
- Screw on antenna
- Plug carrier into main badge

### Assembly Tip: Antenna Tilt

To angle the antenna slightly backward (more comfortable, won't bump chin):
- When soldering J3/J4 (top mechanical headers, KiCad: J16/J17 or J21/J22) to carrier
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
| 2.4 | 2025-02 | User controls: 5-way D-pad (center doubles as photo capture), airplane switch, status LEDs |
| 2.5 | 2025-02 | All status LEDs consolidated to 2× WS2812B chain (saves 2 GPIOs), rainbow idle animation on TX/RX LED |
| 2.6 | 2025-02 | Blinky LED matrix: 26 WS2812B (10 ears + 16 border), Simpsons TV aesthetic |
| 2.7 | 2025-02 | PWR LED changed to traditional RGB (separate from chain), TX/RX status LED removed (animations are the indicator) |
| 2.8 | 2025-02 | Complete GPIO pin assignment (47/48 allocated, GPIO42 spare), camera RST/PWDN tied to free pins for SAO |
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

- [TPS63001 Datasheet](https://www.ti.com/product/TPS63001) (also covers the TPS63000 used for the LED rail)
- [TPS22919 Datasheet](https://www.ti.com/product/TPS22919)
- [PCM5102A Datasheet](https://www.ti.com/product/PCM5102A)
- [ILI9341 Datasheet](https://cdn-shop.adafruit.com/datasheets/ILI9341.pdf)
- [OV2640 Datasheet](https://www.uctronics.com/download/cam_module/OV2640DS.pdf)
- [SA818 Module Documentation](https://www.nicerf.com/product/sa818-vhf-uhf-band-embedded-walkie-talkie-module.html)
- [Saturn PCB Toolkit](https://saturnpcb.com/saturn-pcb-toolkit/) - Impedance calculator
