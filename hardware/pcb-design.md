# PCB Design — DEFCON SSTV Badge

**Version:** 1.1
**Last Updated:** 2026-03

This document defines the physical PCB design for the main badge, including outline, dimensions, component placement zones, and manufacturing specifications.

---

## Simpsons TV Aesthetic

The badge PCB is cut in the shape of a classic 1950s/60s CRT television, inspired by the iconic TV from The Simpsons opening credits. Key design elements:

- **Rounded rectangular body** — the "cabinet" of the TV
- **Rabbit ear antennas** — V-shaped extensions at the top
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
    │                    ═══                       │  │  ← Lanyard slot (centered)
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
| **Ear length** | 50mm | From body to tip |
| **Ear angle** | ~45° | V-shape spread from center |
| **Leg length** | ~15mm | From body to foot |
| **Leg angle** | ~15° | Outward from vertical |
| **Foot width** | ~8mm | Semicircular rounded feet |
| **Total height** | ~190mm | Ears + body + feet |
| **Corner radius** | 2mm | Top corners only (square at leg junctions) |
| **Ear width at base** | 15mm | Where ear meets body |
| **Ear tip width** | ~15mm | Semicircular ball shape (~15mm diameter) |

---

## Rabbit Ear Detail

```
                    ●(L4) ← ~15mm semicircular ball tip
                   ╱
                  ╱  ~15mm between LEDs
                 ╱
                ●(L3)
               ╱
              ╱   ← ~12mm wide mid-ear
             ╱
            ●(L2)
           ╱
          ╱    ← LED spacing: ~10-12mm
         ╱
        ●(L1)
       ╱
      ╱   ← ~15mm wide at base
     ╱
    ●(L0) ─── junction with body
    │
    └── Body top edge
```

**Ear shape:** Tapered from 15mm at base, with semicircular ball tips (~15mm diameter).

**LED placement:** 5 LEDs per ear, centered on ear width, evenly spaced ~10-12mm apart.

---

## Display Area

```
    ┌─────────────── 70mm ───────────────┐
    │  ┌─────────────────────────────┐   │
    │  │                             │   │
    │  │    Display module           │   │  ← Module: ~43 × 60mm
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
    │  [USB-C]              [BOOTSEL] [PWR LED]   │ ← Bottom edge connectors
    └─────────────────────────────────────────────┘
```

### Back Side Layout

```
    ┌─────────────────────────────────────────────┐
    │                    (ears)                   │
    ├─────────────────────────────────────────────┤
    │                   ═════                     │ ← Lanyard slot visible
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

## Lanyard Slot

A single oval cutout at top center for lanyard attachment.

| Parameter | Value |
|-----------|-------|
| Shape | Oval / slot |
| Size | ~20mm × 5mm |
| Position | Top center, horizontally centered |
| Distance from edge | ~8mm from top edge |

```
              ┌──────────────────────┐
              │                      │
              │       ╔══════╗       │  ← 20mm × 5mm oval slot
              │       ╚══════╝       │
              │                      │
```

**Benefits over corner holes:**
- Cleaner look
- Badge hangs straight
- Lanyard clip threads through easily
- No hardware needed (grommets optional)

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
- Ears: Tapered shapes with semicircular ball tips (at top)
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
- Ears are tapered shapes with rounded tips
- Ensure continuous closed outline
- Verify with DRC before fabrication

### Courtyard Considerations
- Leave clearance around edge for manufacturing
- Account for carrier board overlay zone
- Ensure mounting holes don't conflict with components

### Panelization
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

---

*Ready to make some PCBs!*
