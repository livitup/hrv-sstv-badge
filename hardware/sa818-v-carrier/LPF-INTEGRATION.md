# VHF Carrier LPF Integration — Step-by-Step

This document walks through adding a 7-pole Chebyshev low-pass filter to the SA818-V (VHF) carrier board, between the SA818 module's ANT pin and the SMA connector.

**For the design rationale, theory, and component selection rules, see the [harmonic filter design notebook entry](../../docs/engineers-notebook/harmonic-filter-design.md).** Read that first if you haven't already.

---

## Status

- [x] Schematic edits
- [x] Footprint placement on PCB
- [x] Trace routing (0.8 mm RF path — see as-built notes)
- [x] Ground stitching and via fence
- [x] DRC clean
- [ ] Prototype board ordered (5–10 units)
- [ ] Prototype tuning measurements (NanoVNA)
- [ ] BOM locked with specific part numbers
- [ ] Pre-production validation (10 units, NanoVNA + HackRF)
- [ ] Production run cleared

---

## Current State (As Built, 2026-11-13)

**Files:** `sa818-v-carrier.kicad_sch`, `sa818-v-carrier.kicad_pcb`

The filter is in the schematic and laid out on the PCB. ERC is clean; DRC shows 0 unconnected items and 0 schematic-parity issues (the five SMA edge-clearance entries are intentional and excluded).

- **U9 placement (PCB):** `(167, 70)` mm, rotated 180° — moved 27 mm right of its original position to make room for a straight filter
- **U9 ANT pad (pad 12):** `(153.65, 61.175)`
- **J13 placement (PCB):** `(126.65, 52.57)` mm, rotated 90° (edge-mount SMA, unchanged)
- **Board outline:** rectangle from `(80, 50)` to `(200, 90)` — 120mm × 40mm
- **Headers:** J14, J15, J16, J17 are on the **back** (B.Cu) so the pins point into the main badge. Hole positions match the main badge sockets (carrier x = main badge x + 80, y + 50): J14 pin 1 at `(112, 87)`, J15 pin 1 at `(168, 87)`, J16 pin 1 at `(87, 53)`, J17 pin 1 at `(193, 53)`

Other components on the carrier:
- C54: 100nF (0402) and C55: 10µF (0603) — VCC decoupling, next to U9 VDD pad 8
- R18: 0Ω (0402) — band-ID pulldown to GND, at `(143, 87)`
- J13: SMA edge-mount
- U9: SA818V module

---

## Filter Design Summary

**Topology:** 7-pole Chebyshev LPF, π-network, 0.1 dB ripple, 50Ω, fc = 155 MHz

```
   U9 pin 12 (ANT)──┬── L2 ──┬── L4 ──┬── L6 ──┬── J13 pin 1 (SMA center)
                    │        │        │        │
                   C1       C3       C5       C7
                    │        │        │        │
                   GND      GND      GND      GND
```

**Calculated starting values (will be tuned during prototype phase):**

| Designator | Value | Package | Type |
|------------|-------|---------|------|
| C58 | 22 pF | 0402 | C0G/NP0, ±2%, 50V |
| L3 | 68 nH | 0603 wirewound | High-Q, ±2%, ≥200mA |
| C59 | 39 pF | 0402 | C0G/NP0, ±2%, 50V |
| L4 | 82 nH | 0603 wirewound | High-Q, ±2%, ≥200mA |
| C60 | 39 pF | 0402 | C0G/NP0, ±2%, 50V |
| L5 | 68 nH | 0603 wirewound | High-Q, ±2%, ≥200mA |
| C61 | 22 pF | 0402 | C0G/NP0, ±2%, 50V |

**Tunability footprints (DNP — leave unpopulated in BOM):**

| Designator | Value | Package | Purpose |
|------------|-------|---------|---------|
| C62 | DNP | 0402 | Input-side tweak cap |
| C63 | DNP | 0402 | Output-side tweak cap |

> **Note on reference designators:** C56/C57 are reserved for the UHF carrier. Continuing the C58+ sequence here keeps the two carrier boards' BOMs distinct. L1/L2 are not used elsewhere on the VHF carrier (L1 is on the main badge); L3-L5 are unique to this carrier.

---

## Step 1: Source Candidate Components

Before editing KiCad, verify these parts exist on LCSC (for JLCPCB assembly) and Digi-Key (for prototype hand-build).

### Capacitors — Murata GJM15 series, C0G, ±2%, 50V, 0402

| Value | Murata P/N | LCSC P/N |
|-------|-----------|----------|
| 22 pF | GJM1555C1H220JB01D | _verify before ordering_ |
| 39 pF | GJM1555C1H390JB01D | _verify before ordering_ |
| 27 pF | GJM1555C1H270JB01D | _tuning candidate_ |
| 47 pF | GJM1555C1H470JB01D | _tuning candidate_ |
| 18 pF | GJM1555C1H180JB01D | _tuning candidate_ |

> Stock 18, 22, 27 pF and 33, 39, 47 pF for the prototype tuning round.

### Inductors — Coilcraft 0603HP or Murata LQW18A, wirewound, Q≥50, ±2%

> **Why 0603HP and not 0603DC for VHF?** The 0603DC has comparable or slightly lower Q at our values (68 nH: HP Q=73 vs DC Q=63 at 900 MHz) and significantly lower SRF (HP 2.0 GHz vs DC 1.65 GHz at 68 nH). The 0603HP wins at the values we need. See [harmonic filter design notebook](../../docs/engineers-notebook/harmonic-filter-design.md#0603hp-vs-0603dc-at-our-specific-values) for the full comparison table.

| Value | Coilcraft 0603HP P/N | Murata LQW18A P/N | LCSC |
|-------|----------------------|-------------------|------|
| 68 nH | 0603HP-68NXJLU | LQW18AN68NJ00D | _verify_ |
| 82 nH | 0603HP-82NXJLU | LQW18AN82NJ00D | _verify_ |
| 56 nH | 0603HP-56NXJLU | LQW18AN56NJ00D | _tuning candidate_ |
| 75 nH | 0603HP-75NXJLU | LQW18AN75NJ00D | _tuning candidate_ |
| 91 nH | 0603HP-91NXJLU | LQW18AN91NJ00D | _tuning candidate_ |

> Stock 56, 68, 75, 82, 91 nH for the prototype tuning round.

> **Verified SRF (from datasheet, Doc 537 rev 2012-01):**
> - 0603HP-68N: SRF = 2.00 GHz, Q@250MHz = 46, Q@900MHz = 73
> - 0603HP-82N: SRF = 1.80 GHz, Q@250MHz = 45, Q@900MHz = 62
>
> Both have plenty of SRF margin above the 432 MHz 3rd harmonic.

---

## Step 2: Schematic Edits in KiCad (Done)

Open `hardware/sa818-v-carrier/sa818-v-carrier.kicad_sch` in KiCad 9.

### 2a. Identify the existing ANT-to-SMA wire

In the schematic editor, locate the wire connecting U9 pin 12 (labeled "ANT") to J13 pin 1 (the SMA center pin). This is currently a single wire — the filter components will replace it.

### 2b. Place the new components

Add the following symbols from the standard libraries:

| Symbol | Library | Designator |
|--------|---------|-----------|
| Capacitor | Device:C | C58 |
| Inductor | Device:L | L3 |
| Capacitor | Device:C | C59 |
| Inductor | Device:L | L4 |
| Capacitor | Device:C | C60 |
| Inductor | Device:L | L5 |
| Capacitor | Device:C | C61 |
| Capacitor | Device:C | C62 (DNP) |
| Capacitor | Device:C | C63 (DNP) |

Arrange them as the π-network shown in the design summary above.

### 2c. Wire the network

All caps are shunt-to-GND. The signal path runs through the inductors only:

```
U9 pin 12 ──┬──┬── L3 ──┬── L4 ──┬── L5 ──┬──┬── J13 pin 1
            │  │        │        │        │  │
           C58 C62     C59      C60      C61 C63
            │ (DNP)     │        │        │ (DNP)
            │  │        │        │        │  │
           GND GND     GND      GND      GND GND
```

C62 and C63 are unpopulated tunability footprints — placed near the input and output of the filter as shunts to ground, so a tweak cap can be added there during prototype tuning if needed. They are wired as standard shunt caps (one pad on the signal node, other pad on GND), but populated as DNP in the BOM.

### 2d. Set component properties

For each populated component, set Reference, Value, and Footprint as listed in the BOM table above.

For C62 and C63 (DNP):
- Reference: C62, C63
- Value: `DNP`
- Footprint: `Capacitor_SMD:C_0402_1005Metric`
- Right-click → Properties → Attributes → check **"Do not populate"** and **"Exclude from bill of materials"**

> **Verify the schematic Save:** before continuing, save the schematic and run **Inspect → Electrical Rules Checker** (ERC). Should be clean. Any warnings about disconnected pins on C62/C63 are expected (they're shunts) — should not flag as errors.

---

## Steps 3–7: PCB Layout (As Built)

> **Why this differs from the original plan.** The first draft of this document assumed the ANT pad was ~25 mm from the SMA and called for a 3 mm (50Ω) microstrip. On the real board the ANT pad sat directly below the SMA centre pin with under 4 mm of clear space, so U9 was moved 27 mm to the right and the filter laid out in a straight line between the two. A 3 mm trace cannot pass the 0402 pads, so the RF path is 0.8 mm wide; the runs are 5–10 mm, far shorter than a wavelength, and prototype tuning absorbs the small added inductance.

### Placement (F.Cu)

Shunt caps stand perpendicular to the RF line with their signal pad on the line; inductors lie along it. The line leaves the ANT pad at y = 61.175, steps up through the vertical L4 to y = 59.355, and turns up into the SMA centre pin at x = 126.65.

| Ref | X | Y | Orientation | Notes |
|-----|-----|-----|-------------|-------|
| C62 (DNP) | 146.8 | 60.695 | 90 | Input tweak cap, GND pad up |
| C58 | 145.4 | 61.655 | 90 | GND pad down |
| L3 | 142.2 | 61.175 | 0 | Horizontal |
| C59 | 139.0 | 61.655 | 90 | GND pad down |
| L4 | 136.1 | 60.265 | 270 | Vertical — perpendicular to L3/L5 |
| C60 | 133.2 | 59.835 | 270 | GND pad down |
| L5 | 130.0 | 59.355 | 0 | Horizontal |
| C61 | 126.65 | 59.835 | 270 | GND pad down, at the corner below the SMA |
| C63 (DNP) | 126.17 | 57.6 | 180 | Output tweak cap, GND pad left |

```
  x=126.65      130       133.2  136.1    139      142.2    145.4 146.8      153.65
    SMA
     │
 GND[C63]┤                                                           GND
     │                                                              [C62]
     ├──────────[ L5 ]─────┬──────┐   y=59.355                        │
   [C61]                 [C60]  [ L4 ]                                │
    GND                   GND     └───────┬────────[ L3 ]──────┬──────┴──────── ANT pad
                                        [C59]                [C58]     y=61.175
                                         GND                  GND
```

- Populated parts are at least 2 mm apart body to body.
- L3 and L5 are in line and 12 mm apart; L4 is perpendicular to both.

### Routing

- **RF path:** 0.8 mm, F.Cu, four straight segments pad centre to pad centre. The only corner is on C61's pad.
- **VCC:** 1.0 mm from J14 pin 1 along y = 81.5 and up a 45° diagonal to C55, then 0.8 mm on the pad centreline through C54 into U9 pad 8. The last section is 0.8 mm, not 1.0 mm, to keep 0.28 mm clear of C54's ground pad (changed from 0.6 mm on 2026-11-21).
- **PTT, PD, H/L:** F.Cu, straight down from U9's bottom-row pads to J15.
- **RXD, TXD:** F.Cu, under the module, exiting between bottom-row pads 4 and 3.
- **MIC, AF_OUT, SQ:** F.Cu to a via just below the module, then B.Cu along y = 82.5 / 84 / 85.3 to the header pins. These are the only B.Cu tracks, and all sit in the bottom strip of the board.

### Ground

- **B.Cu under the filter is an unbroken ground pour** — no tracks cross it.
- **Cap ground vias:** two 0.6 mm / 0.3 mm vias per cap (including the DNP positions), joined to the GND pad by a short 0.6 mm stub.
- **Via fence:** 12 vias in two rows on a 4 mm pitch: y = 57.3 (x = 129.5 … 145.5) and y = 64.3 (x = 124 … 147.5). The SMA's own ground pads close the left end of the top row.

### Checks to repeat after any layout change

1. **Inspect → Design Rules Checker** with schematic parity ticked: 0 unconnected, 0 parity issues.
2. Ground plane unbroken under the filter; adjacent inductors perpendicular; two ground vias per shunt cap.
3. Header pin-1 holes still at the coordinates listed above — these are what make the carrier mate with the main badge.

---

## Step 8: Order Prototype Boards

Order **5–10 prototype boards** with the calculated values populated. JLCPCB or PCBWay are both fine; the carrier is small (120×40mm) so unit cost is low.

If using JLCPCB assembly:
- Confirm all part numbers are in stock on LCSC
- Verify part orientation in the assembly file
- The C62/C63 DNP positions should be excluded from the assembly BOM

---

## Step 9: Prototype Tuning

When boards arrive:

### 9a. NanoVNA setup

1. Calibrate NanoVNA with SOL kit covering 100 MHz – 1.5 GHz.
2. Build a fixture: solder SMA pigtails to the SA818 ANT trace and the J13 SMA. (Or skip this — drive U9's ANT pad directly with a small pigtail.)

### 9b. Measure each board

Sweep 100 MHz – 1 GHz. For each board, record:

- **Insertion loss (S21) at 146 MHz:** target < 1 dB
- **Return loss (S11) across 144–148 MHz:** target < -15 dB
- **Stopband attenuation (S21) at 288 MHz (2nd harmonic):** target > 50 dB
- **Stopband attenuation (S21) at 432 MHz (3rd harmonic):** target > 60 dB

### 9c. Decide on tuning

- If the **mean** insertion loss is high (>1.5 dB) or the passband isn't centered on the band, increase or decrease cap values uniformly. Common case: cutoff is high → increase C1/C7 from 22 pF to 27 pF, rebuild a few boards, re-measure.
- If individual boards vary widely (>1 dB IL, >5 dB stopband), check for solder defects and component lot consistency — not a design issue.
- If 2nd harmonic attenuation is below 50 dB, check inductor SRF and consider switching to higher-Q parts (Coilcraft 0603HP if you used Murata, or vice versa).

### 9d. Lock the BOM

Once mean response is on target, **freeze the part numbers** in the BOM. Document them in `docs/bom.md` under a new SA818-V carrier section.

---

## Step 10: Pre-Production Validation

Build **10 boards** with the locked BOM. NanoVNA-measure all 10. The cluster of measurements should be tight — passband variation under 5%, stopband variation under 5 dB.

Then drive 2–3 boards from a real SA818 at full power into a 30 dB attenuator → HackRF or NanoVNA spectrum mode:

- Verify **fundamental** is clean and at expected level
- Verify **2nd harmonic** is below -50 dBc
- Verify **3rd harmonic** is below -60 dBc

If both pass, the design is cleared for the production run.

---

## Step 11: Production QC (during 100-board build)

Build a pogo-pin test fixture that connects to the carrier's RF path. NanoVNA-measure 5–10% of finished boards. Record:

- Insertion loss at 146 MHz
- Return loss across 144–148 MHz
- Stopband attenuation at 288 MHz

Reject boards that fall outside the locked design's distribution. Most failures will be solder defects, not design issues.

---

## Updates to Other Documents

When this work is complete, update:

- [x] `docs/bom.md` — add LPF section under SA818-V carrier
- [x] `docs/electrical-design.md` — describe filter design in carrier section
- [x] `docs/project-status.md` — flip carrier status to "in progress" then "complete"
- [x] `CLAUDE.md` — update SA818 carrier description if needed

---

## Resources

- [Harmonic filter design rationale](../../docs/engineers-notebook/harmonic-filter-design.md) — why we're doing this and how the values were calculated
- [Coilcraft 0603HP datasheet](https://www.coilcraft.com/en-us/products/rf/inductors/) — high-Q wirewound inductors
- [Murata LQW18A series datasheet](https://www.murata.com/en-us/products/inductor/chip/) — wirewound inductors
- [NanoVNA filter measurement tutorial](https://nanovna.com/) — official documentation
- ARRL Handbook chapter on RF filters — reference designs and measurement technique

---

*Last updated: 2026-11-13*
