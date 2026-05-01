# VHF Carrier LPF Integration — Step-by-Step

This document walks through adding a 7-pole Chebyshev low-pass filter to the SA818-V (VHF) carrier board, between the SA818 module's ANT pin and the SMA connector.

**For the design rationale, theory, and component selection rules, see the [harmonic filter design notebook entry](../../docs/engineers-notebook/harmonic-filter-design.md).** Read that first if you haven't already.

---

## Status

- [ ] Schematic edits
- [ ] Footprint placement on PCB
- [ ] Trace routing (microstrip)
- [ ] Ground stitching and via fence
- [ ] DRC clean
- [ ] Prototype board ordered (5–10 units)
- [ ] Prototype tuning measurements (NanoVNA)
- [ ] BOM locked with specific part numbers
- [ ] Pre-production validation (10 units, NanoVNA + HackRF)
- [ ] Production run cleared

---

## Current State (Baseline)

**File:** `sa818-v-carrier.kicad_sch`, `sa818-v-carrier.kicad_pcb`

The current carrier has a direct connection between U9 (SA818V, ANT pin = pin 12) and J13 (SMA edge connector). No filter components on the RF path.

- **U9 placement (PCB):** `(140, 70)` mm, rotated 180°
- **J13 placement (PCB):** `(126.65, 52.57)` mm, rotated 90°
- **Board outline:** rectangle from `(80, 50)` to `(200, 90)` — 120mm × 40mm

Existing components on carrier:
- C54: 100nF (0402) — VCC decoupling
- C55: 10µF (0402) — VCC decoupling
- R18: 0Ω (0402) — band-ID pulldown
- J13: SMA edge-mount
- J14, J15: 1×8 male headers (signal)
- J16, J17: 1×3 male headers (mechanical/GND)
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

## Step 2: Schematic Edits in KiCad

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

## Step 3: Update PCB from Schematic

In the KiCad PCB editor (`sa818-v-carrier.kicad_pcb`):

1. **File → Update PCB from Schematic** (`F8`)
2. Confirm the dialog. New components C58–C63, L3–L5 should appear in a corner of the board, ratsnested.
3. Click **Update PCB**.

---

## Step 4: PCB Placement

The filter goes on **F.Cu**, between U9's ANT pin and J13's center pin.

### 4a. Reference geometry

- **U9 (SA818V) ANT pin** is on the left side of U9 footprint. With U9 at `(140, 70)` rotated 180°, the ANT pin is at approximately `(150.5, 71.4)` — confirm by clicking pin 12 on the U9 footprint and reading the coordinates in the status bar.
- **J13 (SMA) pin 1** is at approximately `(126.65, 52.57)` — center pin of the SMA, on the top edge of the board.
- Distance from U9 ANT to J13 pin 1: ~25mm. Plenty of room for a ~25mm filter section between them.

### 4b. Suggested layout

```
                  J13 (SMA, top edge)
                       │
                       │  ← 50Ω microstrip
                       │
   ┌───────────────────┼────────────────────────────┐
   │                   ▼                            │
   │   [C63 DNP]   [C61]                           │
   │       │         │                              │
   │      ━━━L5━━━━━━┻━━ L4 ━━━━━━┻━━ L3 ━━━━━━┻━━━┓
   │                   │              │              ┃
   │                  [C60]          [C59]          [C58]
   │                                                 ┃
   │   [C62 DNP]                                     ┃
   │                                                 ┃
   └─────────────────────────────────────────────────┃─── U9 ANT pin
                                                     │
                                                  (50Ω microstrip)
```

Place components in a roughly straight line between U9 ANT pin and J13. Components ordered: C58, L3, C59, L4, C60, L5, C61. Tweak caps C62 and C63 placed near the input and output respectively.

### 4c. Detailed placement rules

- **Inductor orientation:** L3 at 0°, L4 at 90°, L5 at 0° (perpendicular alternation). Or: L3 horizontal, L4 vertical, L5 horizontal. Avoid having all three inductors with windings parallel to each other.
- **Cap-to-GND vias:** each shunt cap (C58, C59, C60, C61) gets **2 ground vias** within 0.5mm of the GND pad. No shared vias between caps.
- **Spacing:** minimum 2mm between adjacent components (cap to inductor, inductor to inductor) to prevent mutual coupling.
- **Stay clear:** keep the filter section ≥3mm away from C54, C55 (power decoupling) and ≥3mm from any digital signal traces (UART RX/TX, PTT, PD, HL, SQ, ID).

---

## Step 5: Microstrip Routing

The signal path from U9 ANT through the filter to J13 is a 50Ω microstrip line.

### 5a. Calculate microstrip width

Use **KiCad → Tools → PCB Calculator → Transmission Lines** with:

- **Type:** Microstrip Line
- **Substrate:** εr = 4.3 (FR4), tan δ = 0.02
- **H (substrate height):** 1.6mm (or whatever the carrier stackup specifies)
- **T (copper thickness):** 0.035mm (1 oz)
- **Z0 (target):** 50Ω

For 1.6mm FR4 / 1 oz copper, the calculated trace width is approximately **3.0mm**.

> Verify the carrier stackup before trusting this width. If the carrier is 2-layer 1.0mm or 0.8mm FR4, the trace width is significantly different.

### 5b. Route the signal

1. Set the active routing layer to **F.Cu**.
2. Set trace width to the calculated 50Ω value (~3.0mm). Add this as a custom net class width if it's not already defined.
3. Route from U9 pin 12 → C58 node → L3 → C59 node → L4 → C60 node → L5 → C61 node → J13 pin 1.
4. Trace the signal as **a single straight line where possible**. Bends should be 45° or chamfered, never 90°.
5. The trace tapers naturally as it enters and exits the filter section — the cap pads are smaller than the 3mm microstrip; KiCad will neck down automatically.

### 5c. Tweak cap nets

C62 and C63 connect:
- C62 pad 1: tied to the trace just after U9 ANT (between U9 and C58)
- C62 pad 2: tied to GND
- C63 pad 1: tied to the trace just before J13 (between C61 and J13)
- C63 pad 2: tied to GND

These are unpopulated — the 0402 footprint is just a place to add a cap if needed during tuning.

---

## Step 6: Ground Plane and Via Stitching

### 6a. Solid ground under the filter

Verify: on the layer immediately below F.Cu (which on this 2-layer carrier is **B.Cu**), the area under the filter section must be **a solid ground pour**, with no traces or other signals routed through it.

If anything is routed on B.Cu under the filter, reroute it elsewhere. The ground plane must be unbroken.

### 6b. Via fence

Add via stitching around the filter section. Vias every ~5mm along the perimeter of the filter section, connecting F.Cu ground pour to B.Cu ground pour.

In KiCad: **Place → Add Vias** with size 0.6mm OD / 0.3mm drill, on the GND net.

### 6c. Cap ground vias

For each shunt cap (C58, C59, C60, C61, and the DNP positions C62, C63):
- Place 2 vias within 0.5mm of the cap's GND pad.
- Vias connect F.Cu (where the cap is) to B.Cu (ground plane).

---

## Step 7: DRC and Visual Inspection

1. **Run DRC:** **Inspect → Design Rules Checker**. Resolve any errors.
2. **Visual inspection:**
   - Filter section is a clean straight line between U9 ANT and J13
   - Ground plane is unbroken under the filter
   - Adjacent inductors are perpendicular to each other
   - Each shunt cap has 2 ground vias within 0.5mm
   - Via fence surrounds the filter section
   - No 90° trace bends in the RF path
3. **Check 3D view:** **View → 3D Viewer**. Verify components don't physically overlap or interfere.

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

- [ ] `docs/bom.md` — add LPF section under SA818-V carrier
- [ ] `docs/electrical-design.md` — describe filter design in carrier section
- [ ] `docs/project-status.md` — flip carrier status to "in progress" then "complete"
- [ ] `CLAUDE.md` — update SA818 carrier description if needed

---

## Resources

- [Harmonic filter design rationale](../../docs/engineers-notebook/harmonic-filter-design.md) — why we're doing this and how the values were calculated
- [Coilcraft 0603HP datasheet](https://www.coilcraft.com/en-us/products/rf/inductors/) — high-Q wirewound inductors
- [Murata LQW18A series datasheet](https://www.murata.com/en-us/products/inductor/chip/) — wirewound inductors
- [NanoVNA filter measurement tutorial](https://nanovna.com/) — official documentation
- ARRL Handbook chapter on RF filters — reference designs and measurement technique

---

*Last updated: 2026-04-30*
