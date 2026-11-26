# PCB Design — DEFCON SSTV Badge

**Version:** 1.1
**Last Updated:** 2026-03

This document defines the physical PCB design for the main badge, including outline, dimensions, component placement zones, and manufacturing specifications.

---

## Simpsons TV Aesthetic

The badge PCB is cut in the shape of a classic 1950s/60s CRT television, inspired by the iconic TV from The Simpsons opening credits. Key design elements:

- **Rounded rectangular body** — the "cabinet" of the TV
- **Rabbit ear antennas** — V-shaped extensions at the top (separate plug-in boards since 2026-11-25)
- **Display as the "screen"** — centered in the cabinet
- **Stubby feet** — angled legs at bottom corners, badge can stand on a table
- **Retro proportions** — slightly taller than wide, chunky feel

---

## Overall Dimensions

```
                         ← 120mm →
                    (matches carrier width)

        ↑     ●(L4)                    ●(R4)
        │        ╲                    ╱
     ~50mm        ╲                  ╱           ← Rabbit ears
        │          ╲                ╱              (LED tips)
        ↓           ╲              ╱
                     ╲            ╱
    ┌─────────────────●(L0)──●(R0)─────────────────┐ ─┬─
    │                                              │  │
    │       ○                          ○           │  │  ← Lanyard holes (one each side)
    │   ┌─────────────────────────────────────┐    │  │
    │   │         CARRIER BOARD AREA          │    │  │
    │   │            (overlaps here)          │    │ ~25mm carrier overlap
    │   └─────────────────────────────────────┘    │  │
    │                                              │  │
    │       ●      ●      ●      ●      ●          │ ─┴─
    │     ┌───────────────────────────────┐        │
    │   ● │                               │ ●      │
    │     │                               │        │
    │   ● │          DISPLAY              │ ●      │  ← ~120mm body height
    │     │          320×240              │        │
    │   ● │         (2.4" TFT)            │ ●      │
    │     │                               │        │
    │     └───────────────────────────────┘        │
    │       ●      ●      ●      ●      ●          │
    │                                              │
    │  ◀──▶   ◀──▶                   ●PWR          │  ← D-pad, buttons
    │  D-pad  D-pad                                │
    │         ◀──▶                     ◎ CAM       │  ← Camera lens
    │                                              │
    └────┬────────────────────────────────────┬────┘
         │                                    │
         ╱                                    ╲      ← Stubby angled feet
    ____╱                                      ╲____   (~15mm, 15° outward)

                         ↑
                    ← 120mm →
```

---

## Detailed Measurements

| Dimension | Value | Notes |
|-----------|-------|-------|
| **Total width** | 120mm | Matches carrier board |
| **Body height** | 120mm | Main TV cabinet |
| **Ear length** | 95.5mm board | Plug-in board; 82.5mm shows above the body edge along the ear, 13mm is hidden behind the badge |
| **Ear angle** | ~45° | V-shape spread from center |
| **Leg length** | ~15mm | From body to foot |
| **Leg angle** | ~15° | Outward from vertical |
| **Foot width** | ~8mm | Semicircular rounded feet |
| **Total height** | ~199mm | Ears plugged in + body + feet. Main board alone: 127.8 × 138.5mm bounding box |
| **Corner radius** | 2mm | Top corners only (square at leg junctions) |
| **Ear width** | 10.8mm | Straight strip |
| **Ear tip width** | ~15mm | Semicircular ball shape (~15mm diameter) |

---

## Rabbit Ears (plug-in boards)

*Changed 2026-11-25. The ears were part of the main outline; they are now separate boards. The outline drawing above still shows the assembled badge. Reasons and details: `docs/engineers-notebook/plug-in-ears.md`.*

```
        front view, left ear                      side view

          ●  D2  ← 15mm ball tip                 ear      badge
         ╱                                        │
        ●  D3                                     │
       ╱                                          │
      ●  D4          LEDs 13.6mm apart            │
     ╱                                            │         ← top edge of badge
    ●  D5                                         │ ┌──┐ ║
   ╱                                              │═╡  ╞═║  ← header on ear,
  ●  D6                                           │ └──┘ ║     socket on badge back
 ╱                                                       ║
══════════ top edge of badge ═══                     (back)  (front)
 ╲ hidden 13mm: header J26
```

| Item | Value |
|------|-------|
| Ear board | 15.0 × 95.5 mm, 2 layers, 1.6 mm; one design for both ears (`hardware/ear`) |
| Strip width | 10.83 mm, with a 15 mm diameter ball at the tip |
| LEDs | 5 × WS2812B 5050, D2 at the tip to D6; 20.7, 34.3, 47.9, 61.4 and 75.0 mm above the badge's top edge along the ear |
| Hidden part | The bottom 13 mm sits behind the badge and carries the 2×3 header (J26, on the LED side) |
| Sockets on the badge | J24 at (32.96, 5.30) mm and J25 at (87.04, 5.30) mm, on the back, turned 45° outward |
| Stand-off | The ear stands about 11 mm behind the badge's back surface |
| Height | Ear tips reach 60.5 mm above the top edge, the same as the one-piece board |

The socket carries the 45° angle, so the ear itself is a straight strip and panelises densely.

---

## Display Area

```
    ┌─────────────── 70mm ───────────────┐
    │  ┌─────────────────────────────┐   │
    │  │                             │   │
    │  │    Display module           │   │  ← Module: 42.72 × 77.18mm (MSP2401)
    │  │    (ILI9341 2.4")           │   │
    │  │                             │   │
 50mm  │    Visible: 37 × 49mm       │   │
    │  │                             │   │
    │  │                             │   │
    │  └─────────────────────────────┘   │
    │                                    │
    │  SD card slot accessible at edge   │
    └────────────────────────────────────┘

LED border sits OUTSIDE module footprint
```

---

## Border LED Placement

```
              ●(B0) ●(B1) ●(B2) ●(B3) ●(B4)
              ↓     ↓     ↓     ↓     ↓
          ┌───┬─────┬─────┬─────┬─────┬───┐
          │   │                       │   │
    ●(B15)├───┤                       ├───┤●(B5)
          │   │                       │   │
    ●(B14)├───┤      DISPLAY          ├───┤●(B6)
          │   │                       │   │
    ●(B13)├───┤                       ├───┤●(B7)
          │   │                       │   │
          └───┴─────┬─────┬─────┬─────┴───┘
              ↑     ↑     ↑     ↑     ↑
              ●(B12)●(B11)●(B10)●(B9) ●(B8)

    Top/Bottom: 5 LEDs, ~12mm spacing
    Sides: 3 LEDs, ~15mm spacing
```

---

## Component Placement Zones

### Front Side (Component Side)

```
    ┌─────────────────────────────────────────────┐
    │                    CARRIER                  │ ← SA818 carrier overlays here
    ├─────────────────────────────────────────────┤
    │  ┌───────────────────────────────────────┐  │
    │  │ LED_L0-L4 chain │ LED_R0-R4 chain     │  │ ← Ear LEDs
    │  └───────────────────────────────────────┘  │
    │                                             │
    │  ┌───────────────────────────────────────┐  │
    │  │            DISPLAY ZONE               │  │ ← ILI9341 module
    │  │         (Border LEDs around)          │  │
    │  └───────────────────────────────────────┘  │
    │                                             │
    │ ┌────────────┐           ┌────────────────┐ │
    │ │  CONTROLS  │           │  MCU + POWER   │ │ ← RP2350B, TPS63001, etc
    │ │  D-pad     │           │  MCP73871      │ │
    │ │  Buttons   │           │  LC709203F     │ │
    │ └────────────┘           └────────────────┘ │
    │                                             │
    │   ┌────────┐                   ┌────────┐   │
    │   │BATTERY │                   │ CAMERA │   │ ← 2000mAh LiPo + OV2640
    │   │  AREA  │                   └────────┘   │
    │   └────────┘                                │
    │                                             │
    │  [USB-C]              [BOOTSEL]             │ ← Bottom edge connectors (PWR LED is now beside SAO connector J6)
    └─────────────────────────────────────────────┘
```

### Back Side Layout

```
    ┌─────────────────────────────────────────────┐
    │       [J24 ear socket]   [J25 ear socket]   │
    ├─────────────────────────────────────────────┤
    │     ○                               ○       │ ← Lanyard holes
    │                                             │
    │   ┌──────────────────────────────────────┐  │
    │   │                                      │  │
    │   │         BATTERY POUCH                │  │ ← 2000mAh LiPo here
    │   │         (taped or pocketed)          │  │
    │   │                                      │  │
    │   └──────────────────────────────────────┘  │
    │                                             │
    │           [●●●●●●]                          │ ← Tag-Connect debug pads
    │           TC2030                            │
    │                                             │
    │          (minimal components on back)       │
    └─────────────────────────────────────────────┘
```

---

## Lanyard Holes

*Changed 2026-11-25. The original plan was a single 20 × 5 mm slot at top centre. It was never put on the board, and there is no room for it: the flash chip U2 sits where the slot would go, the 6.5 mm above U2 would leave only about 1.5 mm of board over a 5 mm slot, and the carrier (with its SMA connector and filter) covers that spot from the front.*

Two round holes at the top edge instead, one each side:

| Parameter | Value |
|-----------|-------|
| Holes | H1 at (20.0, 5.5) mm, H2 at (100.0, 5.5) mm |
| Diameter | 4.0 mm, not plated |
| Board above each hole | 3.5 mm |
| Position | Between the corner carrier sockets (J11, J12) and the ear sockets (J24, J25) |

- Two points 80 mm apart keep the badge hanging straight.
- The holes are behind the carrier board. The lanyard threads in from the back, up through the gap between the two boards and over the top edge. A cord loop or split ring fits; a bulky swivel clip may not. Check on a prototype.
- The carrier has no parts in front of either hole.
- The LED supply track on the inner power layer was moved from y = 6.5 mm to y = 9.8 mm along the top-left to clear H1.

---

## PCB Layer Stack

| Layer | Use |
|-------|-----|
| Top copper | Signal routing, LED pads, component pads |
| Top silkscreen | Component labels, badge art, callsign area |
| Top soldermask | Solder mask with color |
| Inner 1 | Ground plane (if 4-layer) |
| Inner 2 | Power plane (if 4-layer) |
| Bottom copper | Ground pour, some routing, Tag-Connect, battery pads |
| Bottom silkscreen | Labels, artwork |
| Bottom soldermask | Solder mask |

**Layer count:** Targeting 4-layer for easier routing and better signal integrity. 2-layer may be possible since RF is on carrier.

---

## Silkscreen Art

Based on the classic Simpsons TV design:
- Dials stacked **vertically on the right** side of the screen
- Speaker grill **below the dials**
- Screen frame with rounded corners

```
    ┌─────────────────────────────────────────────┐
    │          "HRV"        "SSTV BADGE"          │ ← Top banner
    ├─────────────────────────────────────────────┤
    │                                             │
    │   ╔═══════════════════════╗      ┌───┐     │
    │   ║                       ║      │ ◯ │     │ ← Channel dial
    │   ║                       ║      └───┘     │
    │   ║       (display)       ║                │
    │   ║        320×240        ║      ┌───┐     │ ← Volume dial
    │   ║                       ║      │ ◯ │     │
    │   ║                       ║      └───┘     │
    │   ╚═══════════════════════╝                │
    │                                  ┌─────┐   │
    │                                  │▓▓▓▓▓│   │ ← Speaker grill
    │                                  │▓▓▓▓▓│   │   (horizontal lines)
    │                                  │▓▓▓▓▓│   │
    │                                  └─────┘   │
    │   [D-pad]    [buttons]                     │
    │                                            │
    │    "73 de HRV"                    v1.0    │ ← Bottom info
    └────┬──────────────────────────────────┬────┘
         ╱                                  ╲
    ____╱                                    ╲____    ← Stubby angled feet
                                                       (part of PCB outline)
```

**Art elements:**
- "HRV" or "Ham Radio Village" as banner text
- Two circular dial graphics (silkscreen circles with lines for "pointer")
- Rectangular speaker grill with horizontal lines
- Screen frame around display area
- "73 de HRV" at bottom

---

## Feet (PCB Extensions)

The badge includes two short, angled feet at the bottom corners — part of the PCB outline, not separate pieces.

```
    Main body
    ┌──────────────────────────────────────────┐
    │                                          │
    │                                          │
    └────┬────────────────────────────────┬────┘
         │         104mm flat bottom      │
         ╱                                ╲
        ╱   ← 15° outward angle           ╲     ← ~15mm leg length
   ____╱                                    ╲____
  (   )                                    (   )  ← 8mm semicircular feet
```

| Parameter | Value |
|-----------|-------|
| Leg length | ~15mm |
| Leg angle | ~15° outward from vertical |
| Foot shape | 8mm diameter semicircle |
| Leg width | 8mm (matches foot) |
| Body junction | Square corners (no radius) |
| Bottom edge between legs | 104mm flat |

**Benefits:**
- Authentic Simpsons TV aesthetic
- Badge can stand up on a table
- Adds visual weight to bottom for balanced look

---

## Edge Cuts (KiCad)

The PCB outline will be defined as edge cuts in KiCad with:
- Main body: Rounded rectangle (2mm corner radius at top, square at bottom for leg junctions)
- Ears: not part of the main outline; the top edge is straight. The ear board has its own outline (`hardware/ear`)
- Feet: Angled extensions with semicircular rounded ends (at bottom)
- All corners and transitions smoothed with arcs
- Minimum internal angle: >90° for manufacturability

---

## Color Scheme

### Soldermask Options

| Option | Color | Notes |
|--------|-------|-------|
| A | **Purple** | Classic Simpsons TV color |
| B | Blue | Also evokes old TVs |
| C | Black | Premium look, LEDs pop |
| D | Green | Classic PCB, hackery feel |

**Recommendation:** Purple soldermask with white silkscreen for authentic Simpsons aesthetic.

---

## Manufacturing Specifications

| Parameter | Value | Notes |
|-----------|-------|-------|
| Min trace/space | 6/6 mil | Standard |
| Min drill | 0.3mm | Standard |
| Board thickness | 1.6mm | Standard FR4 |
| Copper weight | 1oz | All layers |
| Surface finish | HASL or ENIG | ENIG for fine pitch |
| Edge plating | Not required | No edge connectors |
| Slots | May need | Camera or display mounting |

---

## KiCad Implementation Notes

### Edge Cuts Layer
- Use arcs for rounded top corners (2mm radius) and semicircular feet
- The ears are a separate board; see "Rabbit Ears (plug-in boards)"
- Ensure continuous closed outline
- Verify with DRC before fabrication

### Courtyard Considerations
- Leave clearance around edge for manufacturing
- Account for carrier board overlay zone
- Ensure mounting holes don't conflict with components

### Panelization
- Without the ears the main badge is a 127.8 × 138.5mm shape (body plus feet); the ear boards are 15 × 95.5mm strips that pack side by side
- Badge shape may require custom panelization
- V-scoring not suitable for curved edges
- Tab routing recommended

---

## Related Documents

- [Electrical Design](../docs/electrical-design.md) — Circuit schematics and component selection
- [BOM](../docs/bom.md) — Complete bill of materials
- [Project Status](../docs/project-status.md) — Design status tracker

---

## Revision History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2025-02 | Initial PCB outline design |
| 1.1 | 2026-03 | Added feet, updated ears to ball tips, body 120mm, corners 2mm |
| 1.2 | 2026-11 | Rabbit ears became separate plug-in boards on sockets at the back of the top edge. Lanyard slot replaced by two 4 mm holes at the top edge (the slot was never on the board and does not fit) |

---

*Ready to make some PCBs!*
