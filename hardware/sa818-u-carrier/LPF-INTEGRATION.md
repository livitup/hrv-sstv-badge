# UHF Carrier LPF Integration — Step-by-Step

This document walks through adding a 7-pole Chebyshev low-pass filter to the SA818-U (UHF) carrier board, between the SA818 module's ANT pin and the SMA connector.

**For the design rationale, theory, and component selection rules, see the [harmonic filter design notebook entry](../../docs/engineers-notebook/harmonic-filter-design.md).** Read that first if you haven't already.

> **UHF is harder than VHF.** Component parasitics dominate at 470 MHz in a way they don't at 155 MHz. Inductor SRF is a real constraint, layout coupling is tighter, and validation requires equipment that goes well above 1 GHz. Read the VHF integration doc first if you haven't built that one yet — it's a simpler version of the same procedure.

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

**Files:** `sa818-u-carrier.kicad_sch`, `sa818-u-carrier.kicad_pcb`

The filter is in the schematic and laid out on the PCB. ERC is clean; DRC shows 0 unconnected items and 0 schematic-parity issues (the five SMA edge-clearance entries are intentional and excluded).

- **U10 placement (PCB):** `(167, 70)` mm, rotated 180° — moved 27 mm right of its original position to make room for a straight filter
- **U10 ANT pad (pad 12):** `(153.65, 61.175)`
- **J19 placement (PCB):** `(126.65, 52.57)` mm, rotated 90° (edge-mount SMA, unchanged)
- **Board outline:** rectangle from `(80, 50)` to `(200, 90)` — 120mm × 40mm
- **Headers:** J18, J20, J22, J21 are on the **back** (B.Cu) so the pins point into the main badge. Hole positions match the main badge sockets (carrier x = main badge x + 80, y + 50): J18 pin 1 at `(112, 87)`, J20 pin 1 at `(168, 87)`, J22 pin 1 at `(87, 53)`, J21 pin 1 at `(193, 53)`

Other components on the carrier:
- C56: 100nF (0402) and C57: 10µF (0603) — VCC decoupling, next to U10 VDD pad 8
- R18: 10kΩ (0402) — band-ID pull-up to SA818 VCC, at `(143, 87)`
- J19: SMA edge-mount
- U10: SA818U module

---

## Starting Values Revised for Parasitics (2026-11-13)

The values originally calculated below (8.2 pF / 24 nH / 15 pF / 27 nH, fc = 470 MHz) put the cutoff only 4% above the top of the 420–450 MHz band. A first-order model of the as-built layout — trace lengths as 95Ω line sections, 0.5–1.1 nH per shunt cap for the part plus its two vias, inductor Q ≈ 55 — shows that margin is not enough:

| Case | Loss at 435 MHz | Worst return loss 420–450 MHz | 2nd harmonic rejection |
|------|-----------------|-------------------------------|------------------------|
| Original values, ideal (no parasitics) | 0.0 dB | 18.9 dB | 51 dB |
| Original values, as-built layout | 4.8 dB | 2.1 dB | 68 dB |
| Original values, layout compacted to 1.5 mm gaps | 3.9 dB | 2.6 dB | 68 dB |
| **Revised values, as-built layout** | ~0.9 dB | 20 dB | 51 dB |

Compacting the layout recovers about 1 dB of a 5 dB problem, so the geometry was left identical to the VHF carrier and the **starting values were reduced** instead:

| Designator | Original | **Now fitted** |
|------------|----------|----------------|
| C64, C67 | 8.2 pF | **6.8 pF** |
| L6, L8 | 24 nH | **22 nH** |
| C65, C66 | 15 pF | **12 pF** |
| L7 | 27 nH | **24 nH** |

All four are Murata GJM15 / Coilcraft 0603HP values already on the stocking list. The parasitic figures are estimates, not measurements — the prototype NanoVNA sweep still decides the final values. Keep 8.2, 10, 15 pF and 18, 24, 27 nH on hand for tuning. Where the tables further down this document still show the original values, the table above takes precedence.

---

## Filter Design Summary

**Topology:** 7-pole Chebyshev LPF, π-network, 0.1 dB ripple, 50Ω, fc = 470 MHz

```
   U10 pin 12 (ANT)──┬── L6 ──┬── L7 ──┬── L8 ──┬── J19 pin 1 (SMA center)
                     │        │        │        │
                    C64      C65      C66      C67
                     │        │        │        │
                    GND      GND      GND      GND
```

**Calculated starting values (will be tuned during prototype phase):**

| Designator | Value | Package | Type |
|------------|-------|---------|------|
| C64 | 8.2 pF | 0402 | C0G/NP0, ±2%, 50V |
| L6 | 22 or 24 nH | 0603 wirewound | High-Q, ±2%, ≥200mA, SRF ≥ 2.5 GHz (final value selected during prototype tuning) |
| C65 | 15 pF | 0402 | C0G/NP0, ±2%, 50V |
| L7 | 27 nH | 0603 wirewound | High-Q, ±2%, ≥200mA, SRF ≥ 2.5 GHz |
| C66 | 15 pF | 0402 | C0G/NP0, ±2%, 50V |
| L8 | 22 or 24 nH | 0603 wirewound | High-Q, ±2%, ≥200mA, SRF ≥ 2.5 GHz (final value selected during prototype tuning) |
| C67 | 8.2 pF | 0402 | C0G/NP0, ±2%, 50V |

**Tunability footprints (DNP — leave unpopulated in BOM):**

| Designator | Value | Package | Purpose |
|------------|-------|---------|---------|
| C68 | DNP | 0402 | Input-side tweak cap |
| C69 | DNP | 0402 | Output-side tweak cap |

> **Note on reference designators:** continuing the per-carrier sequence (VHF used C58–C63, L3–L5; UHF uses C64–C69, L6–L8) keeps the assembly BOMs unambiguous if the two carrier kits ever ship together.

> **Note on inductor package:** at our values (22–27 nH), the **Coilcraft 0603HP and 0603DC have SRF of 2.5–3.15 GHz**, comfortably above our 1.4 GHz 3rd harmonic. 0603 wirewound is the right choice. Avoid multilayer chip inductors at any size — they have low Q and SRF in the 1–2 GHz range.

---

## Step 1: Source Candidate Components

Before editing KiCad, verify these parts exist on LCSC (for JLCPCB assembly) and Digi-Key (for prototype hand-build).

### Capacitors — Murata GJM15 series, C0G, ±2%, 50V, 0402

| Value | Murata P/N | LCSC P/N |
|-------|-----------|----------|
| 8.2 pF | GJM1555C1H8R2DB01D | _verify before ordering_ |
| 15 pF | GJM1555C1H150JB01D | _verify before ordering_ |
| 6.8 pF | GJM1555C1H6R8DB01D | _tuning candidate_ |
| 10 pF | GJM1555C1H100JB01D | _tuning candidate_ |
| 12 pF | GJM1555C1H120JB01D | _tuning candidate_ |
| 18 pF | GJM1555C1H180JB01D | _tuning candidate_ |

> Stock 6.8, 8.2, 10, 12, 15, 18 pF for the prototype tuning round. UHF is more sensitive to component shifts so plan for a wider sweep.

### Inductors — empirical comparison required (0603HP vs 0603DC vs Murata)

> **Decision (2026-05-10):** the comparison below was skipped. Both carriers use the **Coilcraft 0603HP** series (L6/L8 = 24 nH, L7 = 27 nH on UHF) — it has the higher SRF at every value needed, and one series keeps the BOM and footprint simple. The tables are kept for reference.

UHF is sensitive enough that the inductor choice should be made **empirically during the prototype tuning round**, not committed in advance. Stock candidates from multiple series and let the NanoVNA pick the winner.

> **Why this matters at UHF and not at VHF:** at 470 MHz, we're closer to the inductor's SRF and Q starts to matter at multiple frequencies (fundamental, 2nd harmonic at 940 MHz, 3rd at 1.4 GHz). Different series have different Q-vs-frequency curves. The 0603DC has higher Q at low values at 900 MHz but lower SRF; the 0603HP has higher Q across the board at 1.7 GHz. See [harmonic filter design notebook](../../docs/engineers-notebook/harmonic-filter-design.md#0603hp-vs-0603dc-at-our-specific-values) for the full comparison table.

#### 0603HP series (Coilcraft Doc 537)

| Value | P/N | Q@900M | Q@1.7G | SRF |
|-------|-----|--------|--------|-----|
| 22 nH | 0603HP-22NXGLU | 77 | 88 | 3.15 GHz |
| 24 nH | 0603HP-24NXGLU | 77 | 91 | 2.95 GHz |
| 27 nH | 0603HP-27NXGLU | 74 | 84 | 2.80 GHz |
| 18 nH | 0603HP-18NXGLU | — | — | 3.30 GHz |
| 33 nH | 0603HP-33NXGLU | — | — | 2.70 GHz |

#### 0603DC series (Coilcraft Doc 1605, AEC-Q200)

| Value | P/N | Q@900M | Q@1.7G | Q@2.4G | SRF |
|-------|-----|--------|--------|--------|-----|
| 22 nH | 0603DC-22NXGRW | **88** | 84 | 56 | 2.70 GHz |
| 27 nH | 0603DC-27NXGRW | **82** | 67 | 40 | 2.50 GHz |
| 18 nH | 0603DC-18NXGRW | 80 | 72 | 50 | 2.95 GHz |
| 20 nH | 0603DC-20NXGRW | 80 | 70 | 55 | 3.70 GHz |
| 30 nH | 0603DC-30NXGRW | 77 | 69 | 41 | 3.00 GHz |
| **24 nH NOT AVAILABLE in 0603DC** | | | | | |

> **G suffix = 2% tolerance.** Always specify G for tight tolerance — J = 5% which is too loose for our purposes.

#### Murata LQW18A series

| Value | P/N | LCSC | Notes |
|-------|-----|------|-------|
| 22 nH | LQW18AN22NJ00D | _verify_ | Lower cost; may not match Coilcraft Q |
| 24 nH | LQW18AN24NJ00D | _verify_ | |
| 27 nH | LQW18AN27NJ00D | _verify_ | |

#### Stock for prototype tuning

Buy enough of each variant to build 5 boards of each candidate set:

- **Set A: All 0603HP** — L6/L8 = 24 nH, L7 = 27 nH (matches the calculated topology exactly)
- **Set B: All 0603DC** — L6/L8 = 22 nH, L7 = 27 nH (uses 0603DC's standout 22 nH part, slightly retuned topology)
- **Set C: Mixed** — L6/L8 = 22 nH 0603DC (best-Q value), L7 = 27 nH 0603HP (better high-frequency Q)
- **Set D: Murata LQW18A** for cost-comparison reference

Build all four sets, NanoVNA-measure them, pick the winner from the data. **Do not pre-commit to a series before measuring.**

> **Critical: verify SRF ≥ 2.5 GHz for every selected value.** All values must remain inductive at 1.4 GHz (3rd harmonic of 470 MHz). Both 0603HP and 0603DC at our values meet this. Multilayer chip inductors have SRF in the 1–2 GHz range — **do not use multilayer**.

> **Note on packages:** the original draft of this doc specified Coilcraft 0402DC inductors. After verifying datasheets, **0603DC and 0603HP are the correct families** — Coilcraft's 0402-size RF inductor series have different part designators and lower SRF than 0603. The slightly larger package is fine for layout at UHF, and the Q/SRF advantages are worth the small footprint penalty.

---

## Step 2: Schematic Edits in KiCad (Done)

Open `hardware/sa818-u-carrier/sa818-u-carrier.kicad_sch` in KiCad 9.

### 2a. Identify the existing ANT-to-SMA wire

In the schematic editor, locate the wire connecting U10 pin 12 (labeled "ANT") to J19 pin 1 (the SMA center pin). This is currently a single wire — the filter components will replace it.

### 2b. Place the new components

Add the following symbols from the standard libraries:

| Symbol | Library | Designator |
|--------|---------|-----------|
| Capacitor | Device:C | C64 |
| Inductor | Device:L | L6 |
| Capacitor | Device:C | C65 |
| Inductor | Device:L | L7 |
| Capacitor | Device:C | C66 |
| Inductor | Device:L | L8 |
| Capacitor | Device:C | C67 |
| Capacitor | Device:C | C68 (DNP) |
| Capacitor | Device:C | C69 (DNP) |

Arrange them as the π-network shown in the design summary above.

### 2c. Wire the network

All caps are shunt-to-GND. The signal path runs through the inductors only:

```
U10 pin 12 ──┬──┬── L6 ──┬── L7 ──┬── L8 ──┬──┬── J19 pin 1
             │  │        │        │        │  │
            C64 C68     C65      C66      C67 C69
             │ (DNP)     │        │        │ (DNP)
             │  │        │        │        │  │
            GND GND     GND      GND      GND GND
```

C68 and C69 are unpopulated tunability footprints — placed near the input and output of the filter as shunts to ground, so a tweak cap can be added there during prototype tuning if needed. They are wired as standard shunt caps (one pad on the signal node, other pad on GND), but populated as DNP in the BOM.

### 2d. Set component properties

For each populated component, set Reference, Value, and Footprint as listed in the BOM table above.

For C68 and C69 (DNP):
- Reference: C68, C69
- Value: `DNP`
- Footprint: `Capacitor_SMD:C_0402_1005Metric`
- Right-click → Properties → Attributes → check **"Do not populate"** and **"Exclude from bill of materials"**

> **Verify the schematic save:** before continuing, save the schematic and run **Inspect → Electrical Rules Checker** (ERC). Should be clean. Any warnings about disconnected pins on C68/C69 are expected (they're shunts) — should not flag as errors.

---

## Steps 3–7: PCB Layout (As Built)

> **Why this differs from the original plan.** The first draft of this document assumed the ANT pad was ~25 mm from the SMA and called for a 3 mm (50Ω) microstrip. On the real board the ANT pad sat directly below the SMA centre pin with under 4 mm of clear space, so U10 was moved 27 mm to the right and the filter laid out in a straight line between the two. A 3 mm trace cannot pass the 0402 pads, so the RF path is 0.8 mm wide; the runs are 5–10 mm, far shorter than a wavelength, and prototype tuning absorbs the small added inductance.

### Placement (F.Cu)

Shunt caps stand perpendicular to the RF line with their signal pad on the line; inductors lie along it. The line leaves the ANT pad at y = 61.175, steps up through the vertical L7 to y = 59.355, and turns up into the SMA centre pin at x = 126.65.

| Ref | X | Y | Orientation | Notes |
|-----|-----|-----|-------------|-------|
| C68 (DNP) | 146.8 | 60.695 | 90 | Input tweak cap, GND pad up |
| C64 | 145.4 | 61.655 | 90 | GND pad down |
| L6 | 142.2 | 61.175 | 0 | Horizontal |
| C65 | 139.0 | 61.655 | 90 | GND pad down |
| L7 | 136.1 | 60.265 | 270 | Vertical — perpendicular to L6/L8 |
| C66 | 133.2 | 59.835 | 270 | GND pad down |
| L8 | 130.0 | 59.355 | 0 | Horizontal |
| C67 | 126.65 | 59.835 | 270 | GND pad down, at the corner below the SMA |
| C69 (DNP) | 126.17 | 57.6 | 180 | Output tweak cap, GND pad left |

```
  x=126.65      130       133.2  136.1    139      142.2    145.4 146.8      153.65
    SMA
     │
 GND[C69]┤                                                           GND
     │                                                              [C68]
     ├──────────[ L8 ]─────┬──────┐   y=59.355                        │
   [C67]                 [C66]  [ L7 ]                                │
    GND                   GND     └───────┬────────[ L6 ]──────┬──────┴──────── ANT pad
                                        [C65]                [C64]     y=61.175
                                         GND                  GND
```

- Populated parts are at least 2 mm apart body to body.
- L6 and L8 are in line and 12 mm apart; L7 is perpendicular to both.

### Routing

- **RF path:** 0.8 mm, F.Cu, four straight segments pad centre to pad centre. The only corner is on C67's pad.
- **VCC:** 1.0 mm from J18 pin 1 along y = 81.5, narrowing to 0.6 mm past C57/C56 into U10 pad 8.
- **PTT, PD, H/L:** F.Cu, straight down from U10's bottom-row pads to J20.
- **RXD, TXD:** F.Cu, under the module, exiting between bottom-row pads 4 and 3.
- **MIC, AF_OUT, SQ:** F.Cu to a via just below the module, then B.Cu along y = 82.5 / 84 / 85.3 to the header pins. These are the only B.Cu tracks, and all sit in the bottom strip of the board.

### Ground

- **B.Cu under the filter is an unbroken ground pour** — no tracks cross it.
- **Cap ground vias:** two 0.6 mm / 0.3 mm vias per cap (including the DNP positions), joined to the GND pad by a short 0.6 mm stub.
- **Via fence:** 22 vias in two rows on a 2 mm pitch: y = 57.3 (x = 129.5 … 145.5) and y = 64.3 (x = 124 … 147.5). The SMA's own ground pads close the left end of the top row.

### UHF-specific caveats

- **The layout reuses the VHF geometry.** The filter is about 20 mm end to end, not the ~12 mm originally targeted for UHF. The interconnect adds roughly 1–1.5 nH between elements against 24–27 nH inductors; expect to absorb that in prototype tuning. If the measured response can't be tuned in, a compacted UHF-only layout is the next step.
- **Cap ground vias** sit about 0.24 mm from the pad edge, inside the 0.3 mm target.
- **In the schematic** the filter is drawn in a row below the main circuit and joined to U10 pin 12 with a local label `ANT`, so the PCB net is named `/ANT`.

### Checks to repeat after any layout change

1. **Inspect → Design Rules Checker** with schematic parity ticked: 0 unconnected, 0 parity issues.
2. Ground plane unbroken under the filter; adjacent inductors perpendicular; two ground vias per shunt cap.
3. Header pin-1 holes still at the coordinates listed above — these are what make the carrier mate with the main badge.

---

## Step 8: Order Prototype Boards

Order **5–10 prototype boards** with the calculated values populated. JLCPCB or PCBWay are both fine; the carrier is small (120×40mm) so unit cost is low.

If using JLCPCB assembly:
- Confirm all part numbers are in stock on LCSC (UHF parts can be harder to find than VHF)
- Verify part orientation in the assembly file
- The C68/C69 DNP positions should be excluded from the assembly BOM

> **Tip for UHF:** order extra boards (10–12) instead of 5. The wider tuning sweep needed for UHF means more rebuilds during prototype phase.

---

## Step 9: Prototype Tuning

When boards arrive:

### 9a. NanoVNA setup

1. Calibrate NanoVNA with SOL kit covering 100 MHz – 1.5 GHz. NanoVNA H4 / V2 / Plus4 all cover this range. The original NanoVNA-H (300 MHz max) is **not sufficient** for UHF tuning.
2. Build a fixture: solder SMA pigtails to the SA818 ANT trace and the J19 SMA. Or skip the input pigtail — drive U10's ANT pad directly with a short pigtail.

### 9b. Measure each board

Sweep 100 MHz – 1.5 GHz. For each board, record:

- **Insertion loss (S21) at 435 MHz:** target < 1.5 dB (UHF has slightly higher IL than VHF due to higher-frequency parasitic losses)
- **Return loss (S11) across 420–450 MHz:** target < -15 dB
- **Stopband attenuation (S21) at 870 MHz (2nd harmonic):** target > 50 dB
- **Stopband attenuation (S21) at 1.305 GHz (3rd harmonic):** target > 60 dB

> **NanoVNA frequency limit caveat:** NanoVNA H4 measurements above 1 GHz lose dynamic range. The 1.305 GHz 3rd harmonic measurement may report -50 to -55 dB even when actual attenuation is deeper. If the 2nd harmonic is solidly >50 dB and the 3rd harmonic shows reasonable rolloff, you're probably good. The HackRF measurement in Step 10 is the final word on harmonic compliance.

### 9c. Decide on tuning

- If the **mean** insertion loss is high (>2 dB), inductor Q is the likely culprit — try a different series from your stocked candidates (HP vs DC vs Murata). Look for the set with the lowest passband insertion loss.
- If the cutoff is shifted, adjust caps uniformly: cutoff too high → increase C1/C7 (8.2 pF → 10 pF), cutoff too low → decrease (8.2 pF → 6.8 pF). Inductors can also be tweaked but caps are easier to swap.
- If individual boards vary widely (>2 dB IL, >5 dB stopband), check for solder defects and component lot consistency.
- If 2nd harmonic attenuation is below 50 dB, the inductor SRF is likely too low — **switch inductor manufacturer or series** before changing values.

### 9d. Lock the BOM

Once mean response is on target, **freeze the part numbers** in the BOM. Document them in `docs/bom.md` under a new SA818-U carrier section.

---

## Step 10: Pre-Production Validation

Build **10 boards** with the locked BOM. NanoVNA-measure all 10. The cluster of measurements should be tight — passband variation under 5%, stopband variation under 5 dB.

Then drive 2–3 boards from a real SA818 at full power into a 30 dB attenuator → HackRF spectrum mode:

- Verify **fundamental** at 435 MHz is clean and at expected level
- Verify **2nd harmonic at 870 MHz** is below -50 dBc (target -60 dBc for Part 97 margin on 70cm)
- Verify **3rd harmonic at 1.305 GHz** is below -60 dBc
- Verify **higher harmonics** (1.74 GHz, 2.175 GHz) are below noise floor

> **HackRF dynamic range tip:** the HackRF One has ~50 dB instantaneous dynamic range. Measuring -60 dBc directly is at the limit. **Use the comparison method:** measure with the SA818 ANT directly to attenuator (filter bypassed), record harmonic levels. Then measure with filter inline. The *difference* between the two readings is the filter's contribution, with 80+ dB effective dynamic range.

If both passband and harmonic measurements pass, the design is cleared for the production run.

---

## Step 11: Production QC (during 100-board build)

Build a pogo-pin test fixture that connects to the carrier's RF path. NanoVNA-measure 5–10% of finished boards. Record:

- Insertion loss at 435 MHz
- Return loss across 420–450 MHz
- Stopband attenuation at 870 MHz

Reject boards that fall outside the locked design's distribution. Most failures will be solder defects, not design issues.

---

## UHF-Specific Pitfalls to Watch For

These bite UHF builders regularly. Catch them early:

| Symptom | Likely Cause | Fix |
|---------|--------------|-----|
| Insertion loss > 3 dB | Multilayer chip inductors, low Q | Switch to wirewound (Coilcraft 0603HP/0603DC or Murata LQW18A) |
| Cutoff shifted high by 50+ MHz | Inductor SRF too low (looks capacitive) | Use higher-SRF inductor series |
| 2nd harmonic only 30–35 dB down | Component coupling between adjacent inductors | Rotate inductors to perpendicular orientation; add ground via fence |
| Return loss bumpy in passband | Cap ground via inductance | Add second via per cap GND pad, place within 0.3mm |
| Wide unit-to-unit variation | Component tolerance ±5% (sourced wrong part) | Verify part numbers — must be ±2% C0G caps and ±2% wirewound inductors |
| Filter response correct but low fundamental | Cracked solder joint or open inductor | Visual + ohmmeter check; rework |

---

## Updates to Other Documents

When this work is complete, update:

- [x] `docs/bom.md` — add LPF section under SA818-U carrier
- [x] `docs/electrical-design.md` — describe filter design in carrier section
- [x] `docs/project-status.md` — flip carrier status to "in progress" then "complete"
- [x] `CLAUDE.md` — update SA818 carrier description if needed

---

## Resources

- [Harmonic filter design rationale](../../docs/engineers-notebook/harmonic-filter-design.md) — why we're doing this and how the values were calculated
- [VHF carrier LPF integration](../sa818-v-carrier/LPF-INTEGRATION.md) — sister document; build that one first if you can
- [Coilcraft 0603DC datasheet](https://www.coilcraft.com/getmedia/f3d98f97-cbe2-41fa-b3a9-518f674e881e/0603dc.pdf) (Doc 1605) — wirewound inductors, AEC-Q200 qualified
- [Coilcraft 0603HP datasheet](https://datasheet.octopart.com/0603HP-8N2XGE-Coilcraft-datasheet-10791376.pdf) (Doc 537) — wirewound inductors, higher SRF
- [Murata LQW15A series datasheet](https://www.murata.com/en-us/products/inductor/chip/) — wirewound inductors
- [NanoVNA H4 user guide](https://nanovna.com/) — frequency range covers UHF 2nd harmonic
- ARRL Handbook chapter on RF filters — reference designs and measurement technique

---

*Last updated: 2026-11-13*
