# Main Badge PCB Routing Guide

**For:** defcon-sstv-badge.kicad_pcb
**Board:** 4-layer, 1.6mm FR4, 1oz copper
**Design rules:** 6/6 mil trace/space, 0.3mm min drill

---

## Layer Assignment

| Layer | Name | Purpose |
|-------|------|---------|
| F.Cu | Signal | Components, high-speed signals, most routing |
| In1.Cu | GND | Solid ground plane — protect this, minimize cuts |
| In2.Cu | PWR | 3.3V power plane + VBAT zone |
| B.Cu | Signal | Secondary routing, Tag-Connect (J9), battery pads |

**Rule of thumb:** Route on F.Cu and B.Cu, pour on In1.Cu and In2.Cu. Every signal has a ground reference directly below it on In1.Cu.

---

## Power Plane Strategy (In2.Cu)

The power plane has two distinct zones:

```
┌─────────────────────────────────────────────┐
│              VBAT ZONE                      │  ← Direct from LiPo, feeds TPS63001 + TPS22919
│   (TPS63001 input, TPS22919 input,          │
│    MCP73871 battery rail, LC709203F)         │
├─────────────────────────────────────────────┤
│              +3V3 ZONE                      │  ← TPS63001 output, feeds everything else
│   (RP2350B, display, camera, PCM5102A,      │
│    WS2812B chain, SAO connectors)           │
└─────────────────────────────────────────────┘
```

- Keep the two zones clearly separated with a gap
- Connect each zone to its source with fat traces or fills on F.Cu/B.Cu as well
- TPS22919 output goes to carrier header J7 pin 1 — route on F.Cu with wide trace (0.5mm+), NOT through the power plane

---

## Routing Priority Order

Route these first — they have the tightest constraints and set the placement of everything else.

### Priority 1: RP2350B Power + Internal Regulator

**Do this before any signal routing.** Get all decoupling caps placed and connected.

| Component | Value | Placement Rule |
|-----------|-------|----------------|
| C3-C7 | 5× 100nF (0402) | One per IOVDD pin, <3mm from pin, via straight to GND plane |
| C2 | 10µF (0603) | MCU bulk cap, within 5mm of any VDD pin |
| C1 | 1µF (0402) | DVDD decoupling, <2mm from DVDD pin, via straight to GND |
| L2 | 3.3µH (3×3mm) | Between VREG_LX and DVDD — keep this loop TIGHT and short |

**VREG inductor loop (L2) is critical:**
```
    RP2350B                    L2 (3.3µH)
    ┌──────┐                  ┌──────┐
    │VREG_LX├─── short! ──────┤      ├─── short! ───┐
    │       │                  └──────┘               │
    │ DVDD  ├─── short! ───── C1 (1µF) ──────────────┘
    │       │                     │
    │VREG_PGND├── via ──── GND plane
    └──────┘
```
- Keep L2 within 5mm of RP2350B
- The loop area (LX → L2 → DVDD → C1 → GND → PGND) must be as small as possible
- VREG_PGND should via directly to the ground plane

### Priority 2: Crystal (Y1)

| Rule | Spec |
|------|------|
| Distance from XIN/XOUT | <5mm trace length |
| Load caps (C_Y1, C_Y2) | 15pF, place between crystal and GND, <3mm from crystal pads |
| Ground plane | Solid, unbroken In1.Cu under crystal — no signal routing underneath |
| Keep-out | No high-speed signals within 5mm of crystal |
| Guard pour | Optional F.Cu ground pour ring around crystal area |

```
    RP2350B          Y1 (12MHz)
    ┌──────┐        ┌────────┐
    │  XIN ├────────┤        │
    │      │   C_Y1 │        │  <5mm traces
    │ XOUT ├────────┤        │
    └──────┘   C_Y2 └────────┘
                │  │
               GND GND (via to In1.Cu)
```

### Priority 3: QSPI Flash (U2 — W25Q128JVS)

| Rule | Spec |
|------|------|
| Distance from RP2350B | As close as possible, <15mm traces |
| Trace length matching | Match all 6 signals within ±2mm of each other |
| Trace width | 8 mil (0.2mm) minimum |
| Spacing | 8 mil between QSPI signals |
| Ground plane | Solid In1.Cu under all QSPI traces |
| C9 (100nF) | <3mm from U2 VCC pin |

**Signals (dedicated RP2350B pins, not GPIO):**
- QSPI_SS (directly from RP2350B, also to SW_BOOT1)
- QSPI_SCK
- QSPI_SD0, SD1, SD2, SD3

**Tip:** Place U2 directly adjacent to the QSPI pins on the RP2350B (usually one corner of the QFN-80). Route all 6 signals on F.Cu, no vias if possible.

### Priority 4: USB (J2 → U3 → RP2350B)

| Rule | Spec |
|------|------|
| D+/D- impedance | 90Ω differential (not critical at Full Speed, but good practice) |
| Length matching | D+ and D- within ±2mm |
| Trace width | ~12 mil (0.3mm) for 90Ω diff on 1.6mm 4-layer |
| Spacing | 6 mil between D+ and D- |
| ESD protection | U3 (USBLC6-2SC6) inline, as close to J2 as possible |
| CC resistors | R_CC1, R_CC2 (5.1kΩ) close to J2 |
| VBUS cap | C_USB1 (10µF) close to J2 |
| TVS | D_VBUS1 close to J2 |

**Route order:** J2 → U3 (ESD) → RP2350B USB_DP/USB_DM

USB is Full Speed (12 Mbps) so impedance control is nice-to-have, not mandatory. Keep the traces short and matched and you'll be fine.

### Priority 5: Camera DVP Bus (GPIO8-19 → J4)

| Rule | Spec |
|------|------|
| Data bus (D0-D7) | GPIO8-15, must be consecutive for PIO capture |
| Length matching | Match D0-D7 within ±5mm (QVGA speed is forgiving) |
| PCLK (GPIO16) | Treat as clock — slightly shorter than data, or matched |
| XCLK (GPIO19) | 20MHz output — route away from analog audio signals |
| Trace width | 8 mil (0.2mm) minimum |
| Ground plane | Solid In1.Cu reference under DVP traces |
| Bus routing | Keep D0-D7 as a group, parallel routing preferred |

**Camera connector J4** is a 2×9 through-hole socket. Place it where the camera module will sit (lower-right area per pcb-design.md). Route the DVP bus as a group from the RP2350B GPIO8-19 cluster to J4.

**Pin mapping at J4 connector (watch the pin order):**
| J4 Pin | Signal | GPIO |
|--------|--------|------|
| 7 | CAM_D0 | 8 |
| 10 | CAM_D1 | 9 |
| 9 | CAM_D2 | 10 |
| 12 | CAM_D3 | 11 |
| 11 | CAM_D4 | 12 |
| 14 | CAM_D5 | 13 |
| 13 | CAM_D6 | 14 |
| 16 | CAM_D7 | 15 |
| 15 | CAM_PCLK | 16 |
| 4 | CAM_VSYNC | 17 |
| 6 | CAM_HREF | 18 |
| (XCLK from MCU) | CAM_XCLK | 19 |

**Note:** R12 (10kΩ pullup on RST, pin 8) and R13 (10kΩ pulldown on PWDN, pin 17) should be placed close to J4.

### Priority 6: I2S Audio (GPIO24-25, 29 → U8)

| Rule | Spec |
|------|------|
| Trace length | Keep short, <50mm |
| Separation | Route I2S signals away from TPS63001 inductor (L1) — >10mm clearance |
| Ground plane | Solid In1.Cu under I2S traces and PCM5102A |
| Analog output | PCM5102A VOUTL is sensitive — route TX filter components (R5, R6, C24, C25) with short traces, away from digital noise |

**PCM5102A (U8) placement tip:** Put it between the RP2350B and the carrier header J7 (which carries MIC+ audio to the SA818). This minimizes the analog audio trace length to the carrier.

**PCM5102A decoupling — place close to U8:**
| Cap | Value | Pin |
|-----|-------|-----|
| C19 | 100nF | AVDD |
| C20 | 10µF | DVDD |
| C21 | 2.2µF | CAPP |
| C22 | 2.2µF | CAPM |

**TX audio path routing (analog — keep short and clean):**
```
U8 VOUTL → R5 (47k) → node → R6 (470Ω) to GND
                         ↓
                    C24 (10nF) to GND
                         ↓
                    C25 (100nF) DC block
                         ↓
                    J7 pin 3 (MIC+)
```

**RX audio path routing (analog — keep short and clean):**
```
J7 pin 5 (SPK+) → C26 (1µF) DC block → R7 (4.7k) → node → R8 (4.7k) → bias
                                                        ↓
                                                   C27 (4.7nF) to GND
                                                        ↓
                                                   GPIO40 (ADC0)
```

Route RX audio on the opposite side of the board from the TPS63001 switching regulator if possible.

---

## Lower Priority (Route After the Above)

### SPI Bus (GPIO2-4 → J3 Display + J10 SD Card)

Not speed-critical at these frequencies. Standard 8 mil traces, no length matching needed.

| Signal | GPIO | Goes To |
|--------|------|---------|
| SPI_SCK | 2 | J3 + J10 |
| SPI_MOSI | 3 | J3 + J10 |
| SPI_MISO | 4 | J10 (display doesn't use) |
| DISP_CS | 5 | J3 only |
| DISP_DC | 6 | J3 only |
| DISP_RST | 7 | J3 only |
| DISP_BL | 22 | Q1 gate (MOSFET for backlight) |
| SD_CS | 23 | J10 only |

Route SPI as a bus from the RP2350B, branching to both J3 and J10. The bus can use vias to B.Cu if needed.

### I2C Bus (GPIO0-1)

Shared by 4 devices: camera (J4), fuel gauge (U7), SAO1 (J5), SAO2 (J6).

- Route as a bus with stubs to each device
- R1, R2 (4.7kΩ pullups to +3V3) should be near the RP2350B, not at the far end
- Standard traces, no constraints

### SA818 Control Signals (GPIO30-36 → J8)

All low-speed digital (UART 9600 baud, GPIO). Route with standard traces to carrier header J8. No constraints.

### User Controls (GPIO26, 37-39, 41, 43)

All switches with internal pullups. Route with any convenient path. Switches are through-hole, so traces can run on B.Cu underneath them.

### WS2812B LED Chain (GPIO47)

Single-wire protocol at 800kHz. Route as a daisy chain:

```
GPIO47 → D2(L0) → D3(L1) → D4(L2) → D5(L3) → D6(L4)  ← Left ear, up
                                                    ↓
         D11(R4) → D10(R3) → D9(R2) → D8(R1) → D7(R0)  ← Right ear, up
            ↓
         D12(B0) → D13(B1) → ... → D27(B15)              ← Display border, clockwise
```

**Routing tips:**
- Ear LEDs (D2-D11): Route data along the ear PCB outline, single trace
- The jump from D6 (left ear tip) to D7 (right ear base) is the longest run — route across the carrier overlap zone on B.Cu
- Border LEDs (D12-D27): Route around the display area
- **Each LED needs its 100nF bypass cap (C28-C53)** placed within 3mm of the LED's VDD/VSS pins
- 3.3V power to WS2812B chain needs decent trace width (0.3mm+) — 26 LEDs × ~12mA = ~310mA idle, peaks higher
- Consider a second 3.3V power injection point halfway through the chain (at D12, border start) to prevent voltage drop

### PWR LED (D1 — APFA3010)

GPIO44/45/46 through R14/R15/R16 to D1 cathodes. D1 anode to +3V3. Place near bottom edge per pcb-design.md. Standard routing.

---

## Power Distribution Routing

### TPS63001 (U5) — Buck-Boost Layout

**This is the noisiest part of the board.** Keep it compact and away from analog audio.

```
    C_IN (10µF)          L1 (1µH)           C_OUT1 (10µF)
    ┌────────┐          ┌──────┐           ┌────────┐
    │        │          │      │           │        │
VBAT────┤        ├──VIN  SW├──┤      ├──VOUT──┤        ├── +3V3
    │        │          │      │           │        │
    └───┬────┘          └──────┘           └───┬────┘
        │              U5 (TPS63001)           │
       GND              ┌──────┐              GND
                         │ PGND │
                         └──┬───┘
                           GND
```

**Layout rules:**
- Input cap (C_IN) close to VIN, <5mm
- Output cap (C_OUT1) close to VOUT, <5mm
- L1 close to SW pin, <5mm
- PGND (power ground pad) → via directly to GND plane
- Keep the VIN→SW→L1→VOUT loop area small
- Place U5 away from crystal (Y1), camera, and audio analog traces — >15mm separation recommended

### TPS22919 (U6) — Load Switch

- C17 (100µF 1206) close to output — this is a big cap, plan board space
- C18 (10µF) also close to output
- Route output to J7 pin 1 with wide trace (0.5mm minimum, 1mm preferred for 1.7A TX peaks)

### MCP73871 (U4) — Battery Charger

- Place near USB-C connector (J2) and battery connector (J1)
- R3, R4 close to PROG1/PROG3 pins
- TH1 (thermistor) close to battery area, away from L1/U5

---

## Carrier Header Placement

These set the mechanical interface with the carrier board. Place precisely:

```
    ┌─────────────────────────────────────────────────────────────┐
    │  J11(1×3,GND)                              J12(1×3,GND)    │  ← Top edge
    │  ~5-13mm from left                         ~107-115mm       │
    │                                                             │
    │                    CARRIER OVERLAP ZONE                      │
    │                    (~25mm deep from top)                     │
    │                                                             │
    │  J7(1×8,pwr/audio)                    J8(1×8,ctrl/data)    │  ← ~25mm from top
    │  ~10-30mm from left                   ~90-110mm from left   │
    └─────────────────────────────────────────────────────────────┘
```

All four headers are **female sockets on the main badge front side**, receiving male pins from the carrier.

**J7 pinout (power/audio):** VCC, GND, MIC+, GND, SPK+, GND, GND, GND
**J8 pinout (control/data):** PTT, PD, HL, SQ, TX, RX, ID, GND

---

## Ground Plane (In1.Cu) Protection

The ground plane is your most important routing asset. Protect it:

- **Never** route signals on In1.Cu — it's a dedicated ground plane
- Minimize via count in critical areas (under crystal, QSPI, camera bus)
- If you must break the plane, ensure return current paths aren't disrupted
- Pour ground on F.Cu and B.Cu in empty areas for additional shielding and thermal relief

**Critical unbroken zones:**
1. Under Y1 crystal and load caps
2. Under U2 QSPI flash
3. Under camera DVP bus routing
4. Under U8 PCM5102A and analog audio traces
5. Under USB D+/D- traces

---

## Thermal Considerations

- **RP2350B exposed pad:** Connect to GND plane with multiple vias (9+ thermal vias in a 3×3 grid)
- **TPS63001 power pad:** Connect to GND plane with thermal vias
- **MCP73871 power pad:** Connect to GND plane with thermal vias

---

## DRC Settings for KiCad

Set these in Board Setup → Design Rules:

| Rule | Value |
|------|-------|
| Minimum trace width | 0.15mm (6 mil) |
| Minimum clearance | 0.15mm (6 mil) |
| Minimum via drill | 0.3mm |
| Minimum via diameter | 0.6mm |
| Copper to edge clearance | 0.25mm |

**Net classes to define:**

| Net Class | Trace Width | Clearance | Via Size | Use |
|-----------|-------------|-----------|----------|-----|
| Default | 0.2mm | 0.15mm | 0.6/0.3mm | General signals |
| Power | 0.4mm | 0.2mm | 0.8/0.4mm | 3.3V, VBAT, VBUS |
| USB | 0.3mm | 0.15mm | 0.6/0.3mm | USB D+/D- |
| HighCurrent | 0.8mm | 0.2mm | 0.8/0.4mm | SA818 VCC (to J7 pin 1) |

---

## Checklist Before Starting

- [ ] Board setup: 4 layers configured (F.Cu, In1.Cu/GND, In2.Cu/PWR, B.Cu)
- [ ] Design rules entered (trace/space, via sizes, net classes)
- [ ] Board outline complete (TV shape with ears and feet) — already done
- [ ] All footprints assigned and imported
- [ ] Netlisted from schematic (Update PCB from Schematic)
- [ ] Component placement done (see pcb-design.md for zones)
- [ ] Ground plane poured on In1.Cu
- [ ] Power plane zones defined on In2.Cu (VBAT + 3V3)

## Suggested Routing Order Checklist

- [ ] Place and connect all RP2350B decoupling caps (C1-C7)
- [ ] Route VREG inductor loop (L2, C1)
- [ ] Place and route crystal (Y1, C_Y1, C_Y2)
- [ ] Place and route QSPI flash (U2, C9)
- [ ] Route USB (J2 → U3 → RP2350B)
- [ ] Route camera DVP bus (RP2350B → J4)
- [ ] Place and route TPS63001 (U5, L1, C_IN, C_OUT1)
- [ ] Place and route TPS22919 (U6, C17, C18)
- [ ] Place and route MCP73871 (U4, R3, R4, C8/C10/C11)
- [ ] Route I2S audio (RP2350B → U8)
- [ ] Route TX audio filter chain (U8 → R5/R6/C24/C25 → J7 pin 3)
- [ ] Route RX audio filter chain (J7 pin 5 → C26/R7/R8/C27/R9/R10 → GPIO40)
- [ ] Route SPI bus (RP2350B → J3 display → J10 SD card)
- [ ] Route I2C bus (RP2350B → J4, U7, J5, J6)
- [ ] Route SA818 control signals (RP2350B → J8)
- [ ] Route carrier power (VBAT → U6 → J7 pin 1)
- [ ] Route WS2812B LED chain (GPIO47 → D2...D27)
- [ ] Route user controls (switches → RP2350B)
- [ ] Route PWR LED (RP2350B → R14/R15/R16 → D1)
- [ ] Fill ground pours on F.Cu and B.Cu
- [ ] Run DRC — zero errors
- [ ] Visual inspection of ground plane continuity on In1.Cu

---

*Good luck. No RF on this board — you've got this.*
