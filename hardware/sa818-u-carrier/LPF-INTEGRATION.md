# UHF Carrier LPF Integration — Step-by-Step

This document walks through adding a 7-pole Chebyshev low-pass filter to the SA818-U (UHF) carrier board, between the SA818 module's ANT pin and the SMA connector.

**For the design rationale, theory, and component selection rules, see the [harmonic filter design notebook entry](../../docs/engineers-notebook/harmonic-filter-design.md).** Read that first if you haven't already.

> **UHF is harder than VHF.** Component parasitics dominate at 470 MHz in a way they don't at 155 MHz. Inductor SRF is a real constraint, layout coupling is tighter, and validation requires equipment that goes well above 1 GHz. Read the VHF integration doc first if you haven't built that one yet — it's a simpler version of the same procedure.

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

**File:** `sa818-u-carrier.kicad_sch`, `sa818-u-carrier.kicad_pcb`

The current carrier has a direct connection between U10 (SA818U, ANT pin = pin 12) and J19 (SMA edge connector). No filter components on the RF path.

- **U10 placement (PCB):** `(140, 70)` mm, rotated 180°
- **J19 placement (PCB):** `(126.65, 52.57)` mm, rotated 90°
- **Board outline:** rectangle from `(80, 50)` to `(200, 90)` — 120mm × 40mm

Existing components on carrier:
- C56: 100nF (0402) — VCC decoupling
- C57: 10µF (0402) — VCC decoupling
- R18: DNP (0402) — band-ID pulldown (UHF = open)
- J18: 1×8 male header (Power/Audio signals)
- J19: SMA edge-mount
- J20: 1×8 male header (Control/Data signals)
- J21, J22: 1×3 male headers (mechanical/GND)
- U10: SA818U module

> **Note on connector numbering:** the UHF carrier connector designators differ from the VHF carrier. J19 on UHF is the SMA (vs. J13 on VHF). Memory notes describing "J18 SMA" reflect an earlier numbering plan that doesn't match the current schematic — verify against the actual `.kicad_sch` file before citing.

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

## Step 2: Schematic Edits in KiCad

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

## Step 3: Update PCB from Schematic

In the KiCad PCB editor (`sa818-u-carrier.kicad_pcb`):

1. **File → Update PCB from Schematic** (`F8`)
2. Confirm the dialog. New components C64–C69, L6–L8 should appear in a corner of the board, ratsnested.
3. Click **Update PCB**.

---

## Step 4: PCB Placement

The filter goes on **F.Cu**, between U10's ANT pin and J19's center pin.

### 4a. Reference geometry

- **U10 (SA818U) ANT pin** is on the left side of U10 footprint. With U10 at `(140, 70)` rotated 180°, the ANT pin is at approximately `(150.5, 71.4)` — confirm by clicking pin 12 on the U10 footprint and reading the coordinates in the status bar.
- **J19 (SMA) pin 1** is at approximately `(126.65, 52.57)` — center pin of the SMA, on the top edge of the board.
- Distance from U10 ANT to J19 pin 1: ~25mm. The UHF filter section is shorter than the VHF one (~12mm vs ~25mm), so there's even more room.

### 4b. Suggested layout

```
                  J19 (SMA, top edge)
                       │
                       │  ← 50Ω microstrip
                       │
   ┌───────────────────┼────────────────────────────┐
   │                   ▼                            │
   │   [C69 DNP]   [C67]                           │
   │       │         │                              │
   │      ━━━L8━━━━━━┻━━ L7 ━━━━━━┻━━ L6 ━━━━━━┻━━━┓
   │                   │              │              ┃
   │                  [C66]          [C65]          [C64]
   │                                                 ┃
   │   [C68 DNP]                                     ┃
   │                                                 ┃
   └─────────────────────────────────────────────────┃─── U10 ANT pin
                                                     │
                                                  (50Ω microstrip)
```

Place components in a roughly straight line between U10 ANT pin and J19. Components ordered: C64, L6, C65, L7, C66, L8, C67. Tweak caps C68 and C69 placed near the input and output respectively.

### 4c. Detailed placement rules

- **Inductor orientation:** L6 at 0°, L7 at 90°, L8 at 0° (perpendicular alternation). At UHF this matters even more than at VHF — adjacent parallel inductors at 470 MHz can couple enough to ruin the response.
- **Cap-to-GND vias:** each shunt cap (C64, C65, C66, C67) gets **2 ground vias** within 0.3mm of the GND pad. UHF needs tighter ground via placement than VHF — even 0.5mm of trace from cap to via adds enough inductance to detune the filter.
- **Spacing:** minimum 1.5mm between adjacent components — at UHF, mutual coupling between adjacent 0603 inductors is a real concern. If space allows, increase to 2mm.
- **Stay clear:** keep the filter section ≥3mm away from C56, C57 (power decoupling) and ≥3mm from any digital signal traces (UART RX/TX, PTT, PD, HL, SQ, ID).
- **Total filter length:** target ~12mm tip-to-tail. Compact = better at UHF.

---

## Step 5: Microstrip Routing

The signal path from U10 ANT through the filter to J19 is a 50Ω microstrip line.

### 5a. Calculate microstrip width

Use **KiCad → Tools → PCB Calculator → Transmission Lines** with:

- **Type:** Microstrip Line
- **Substrate:** εr = 4.3 (FR4), tan δ = 0.02
- **H (substrate height):** 1.6mm (or whatever the carrier stackup specifies)
- **T (copper thickness):** 0.035mm (1 oz)
- **Z0 (target):** 50Ω

For 1.6mm FR4 / 1 oz copper, the calculated trace width is approximately **3.0mm**. **This is the same as VHF** — microstrip impedance is frequency-independent for these dimensions.

> Verify the carrier stackup before trusting this width. If the carrier is 2-layer 1.0mm or 0.8mm FR4, the trace width is significantly different. **At UHF, getting the microstrip width right matters more** — 5% off can shift filter response visibly.

### 5b. Route the signal

1. Set the active routing layer to **F.Cu**.
2. Set trace width to the calculated 50Ω value (~3.0mm).
3. Route from U10 pin 12 → C64 node → L6 → C65 node → L7 → C66 node → L8 → C67 node → J19 pin 1.
4. Trace the signal as **a single straight line where possible**. Bends should be 45° or chamfered, never 90°. At UHF, even chamfered 45° corners introduce small reflections — keep the path as straight as possible.
5. The trace tapers naturally as it enters and exits the filter section — the cap pads are smaller than the 3mm microstrip; KiCad will neck down automatically. **At UHF, the taper transition can affect return loss** — keep the necked-down section short.

### 5c. Tweak cap nets

C68 and C69 connect:
- C68 pad 1: tied to the trace just after U10 ANT (between U10 and C64)
- C68 pad 2: tied to GND
- C69 pad 1: tied to the trace just before J19 (between C67 and J19)
- C69 pad 2: tied to GND

These are unpopulated — the 0402 footprint is just a place to add a cap if needed during tuning.

---

## Step 6: Ground Plane and Via Stitching

UHF is more aggressive about ground than VHF. Don't compromise here.

### 6a. Solid ground under the filter

Verify: on the layer immediately below F.Cu (which on this 2-layer carrier is **B.Cu**), the area under the filter section must be **a solid ground pour**, with no traces or other signals routed through it.

If anything is routed on B.Cu under the filter, reroute it elsewhere. The ground plane must be unbroken. **Even a thin trace running underneath the filter can cause a noticeable bump in S11 at UHF.**

### 6b. Via fence

Add via stitching around the filter section. Vias every **~3mm** along the perimeter (tighter than the VHF ~5mm spacing because UHF wavelength is shorter and EM leakage is faster).

In KiCad: **Place → Add Vias** with size 0.6mm OD / 0.3mm drill, on the GND net.

### 6c. Cap ground vias

For each shunt cap (C64, C65, C66, C67, and the DNP positions C68, C69):
- Place 2 vias **within 0.3mm** of the cap's GND pad. (VHF spec was 0.5mm — UHF tightens this.)
- Vias connect F.Cu (where the cap is) to B.Cu (ground plane).
- If possible, connect the cap's GND pad directly to the via with no intermediate trace.

### 6d. Optional: ground pour around the filter

Add a small F.Cu ground pour ring around the filter section (with appropriate clearance to the 50Ω microstrip) and stitch it heavily to the B.Cu ground plane. This creates a quasi-shielded enclosure for the filter and improves stopband performance.

---

## Step 7: DRC and Visual Inspection

1. **Run DRC:** **Inspect → Design Rules Checker**. Resolve any errors.
2. **Visual inspection:**
   - Filter section is a clean straight line between U10 ANT and J19
   - Ground plane is unbroken under the filter (check B.Cu visibility in KiCad)
   - Adjacent inductors are perpendicular to each other
   - Each shunt cap has 2 ground vias within 0.3mm
   - Via fence (3mm spacing) surrounds the filter section
   - No 90° trace bends in the RF path
   - Filter section length ≤ 15mm
3. **Check 3D view:** **View → 3D Viewer**. Verify components don't physically overlap or interfere.

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

- [ ] `docs/bom.md` — add LPF section under SA818-U carrier
- [ ] `docs/electrical-design.md` — describe filter design in carrier section
- [ ] `docs/project-status.md` — flip carrier status to "in progress" then "complete"
- [ ] `CLAUDE.md` — update SA818 carrier description if needed

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

*Last updated: 2026-04-30*
