# Harmonic Low-Pass Filter Design

This document captures the rationale, topology, component selection, and validation strategy for the harmonic suppression low-pass filters on the SA818-V (VHF) and SA818-U (UHF) carrier boards.

For step-by-step integration instructions, see:
- [VHF carrier LPF integration](../../hardware/sa818-v-carrier/LPF-INTEGRATION.md)
- [UHF carrier LPF integration](../../hardware/sa818-u-carrier/LPF-INTEGRATION.md)

---

## The Problem

The SA818 module is well-known in the amateur radio community for poor harmonic suppression. Independent measurements typically show:

| Harmonic | Typical Level (raw, no filter) |
|----------|-------------------------------|
| 2nd | -30 to -40 dBc |
| 3rd | -40 to -50 dBc |
| Higher | -50 dBc and below |

**FCC Part 97.307(d)** requires that for amateur transmitters in the 30–225 MHz band operating below 25W, mean spurious emissions be **at least 43 dB below the mean power** of the fundamental. For 222 MHz and above with similar power, the requirement is **60 dB**.

The SA818 alone, transmitting on the air, is therefore **likely to be non-compliant** with Part 97 — particularly on 70cm where the rule is stricter. Anyone operating this badge on the air without external filtering is technically violating their license.

This is a known issue. Many community SA818 projects (digital APRS trackers, DMR hotspots, packet radio) include external low-pass filtering for exactly this reason.

---

## Carriers, Not Main Badge

All RF on this project is confined to the SA818 carrier boards. The main badge has zero RF traces. This means:

- **The filter goes on each carrier** (one design for VHF, one for UHF)
- **No respin of the main badge** is required to add filtering
- **Per-band filter values** fit naturally — each carrier is band-specific anyway

Both carriers currently route the SA818 ANT pin directly to the SMA connector. There is no filtering of any kind on the RF path. The only existing components on the carriers are power decoupling caps (C54/C55 on VHF, C56/C57 on UHF) and the band-ID resistor (R18).

---

## Why Not a Ceramic Chip Filter?

A common solution in commercial designs is a single ceramic LPF chip (Murata LFL series, TDK DEA series, Johanson LF/LP series). These are appealing — one part, no tuning, no inductors to source. But for our application they have legitimate weaknesses:

### 1. Power handling at the limit

Most ceramic LPF chips are rated for 0.5–1W of through-power. The SA818 puts out 1W at full power (HIGH mode). At the edge of the chip's rating:

- Stopband attenuation can degrade dynamically — measured -25 dBc at low power but only -18 dBc at full power
- Some parts compress or distort, adding new spurious products
- A few parts are simply damaged at sustained 1W

Datasheets rarely characterize this behavior, so the actual delivered attenuation under real transmit conditions is often worse than spec.

### 2. Modest stopband attenuation

Typical ceramic chip LPFs offer 20–30 dB attenuation in the stopband. Combined with the SA818's raw -30 to -35 dBc 2nd harmonic, that yields roughly:

- **Ceramic chip total**: -50 to -65 dBc — passable for VHF, marginal for UHF

A well-built discrete LC LPF can offer 50–60+ dB of *additional* stopband attenuation, yielding -80+ dBc total. The discrete option has ~20 dB more headroom against worst-case SA818 batches and component drift.

### 3. Insertion loss

Ceramic chips often present 1.5–3 dB of passband insertion loss. At 1W input, this means 30–50% of the transmit power never reaches the antenna. A discrete LPF using high-Q wirewound inductors and C0G capacitors typically presents 0.5–1.5 dB.

### 4. The empirical evidence

The decision driver: independent test videos of SA818 + ceramic LPF chips show only modest improvement, with harmonics often still visibly above the noise floor on a spectrum analyzer. The same builds with discrete LPFs show harmonics buried in the noise. This isn't bad implementation — it's a real gap in performance ceiling.

**Conclusion:** Use a discrete LPF.

---

## Topology Selection: 7-Pole Chebyshev

The chosen topology is a **7-pole Chebyshev low-pass filter, π-network, 0.1 dB passband ripple, 50Ω**.

```
   RF_IN ──┬──── L2 ────┬──── L4 ────┬──── L6 ────┬── RF_OUT
           │            │            │            │
          C1           C3           C5           C7
           │            │            │            │
          GND          GND          GND          GND
```

### Why these choices?

**7-pole** (vs. 5-pole or 9-pole):
- 5-pole: Insufficient roll-off. ~30 dB at 2× cutoff. Marginal for our needs.
- 7-pole: ~50–60 dB at 2× cutoff, ~70 dB at 3× cutoff. Sweet spot for amateur use.
- 9-pole: ~70+ dB at 2× cutoff, but more parts, more layout sensitivity, more loss. Diminishing returns for the harmonic levels we need to suppress.

**Chebyshev** (vs. Butterworth or elliptic):
- Butterworth: Maximally flat passband but slower roll-off. Would need 9-pole to match 7-pole Chebyshev's stopband.
- Chebyshev: Slight passband ripple (0.1 dB is imperceptible) in exchange for sharper roll-off. Standard amateur radio choice.
- Elliptic (Cauer): Even sharper but has stopband nulls and zeros — sensitive to component tolerances, harder to tune, narrowband notches that drift.

**0.1 dB ripple** (vs. 0.5 dB or 1 dB):
- Lower ripple = gentler roll-off but better return loss in passband
- 0.1 dB is the standard ham-radio choice — return loss typically <-20 dB across the band

**π-network** (caps to ground at endpoints) vs. T-network (inductors at endpoints):
- π is preferred: caps to ground are easier to implement well at VHF/UHF (short ground vias)
- T-network with series-input inductor is more sensitive to source impedance variations from the SA818

---

## Component Values

Calculated from standard Chebyshev g-coefficients (n=7, 0.1 dB ripple):

| Element | g-value |
|---------|---------|
| g1 | 1.1812 |
| g2 | 1.4228 |
| g3 | 2.0967 |
| g4 | 1.5734 |
| g5 | 2.0967 |
| g6 | 1.4228 |
| g7 | 1.1812 |

Element values: `Cn = gn / (2π·fc·Z0)` for shunt caps, `Ln = gn·Z0 / (2π·fc)` for series inductors.

### VHF Carrier (2m band, 144–148 MHz)

Cutoff frequency: **fc = 155 MHz** (above the 2m band with margin)

| Element | Calculated | Standard E12 value |
|---------|-----------|-------------------|
| C1, C7 | 24.3 pF | **22 pF** |
| L2, L6 | 73 nH | **68 nH** |
| C3, C5 | 43 pF | **39 pF** |
| L4 | 81 nH | **82 nH** |

Expected 2nd harmonic (288 MHz) attenuation: **~50–55 dB**.

### UHF Carrier (70cm band, 420–450 MHz)

Cutoff frequency: **fc = 470 MHz** (above the 70cm band with margin)

| Element | Calculated | Standard E12 value |
|---------|-----------|-------------------|
| C1, C7 | 8.0 pF | **8.2 pF** |
| L2, L6 | 24 nH | **24 nH** |
| C3, C5 | 14.2 pF | **15 pF** |
| L4 | 27 nH | **27 nH** |

Expected 2nd harmonic (870 MHz) attenuation: **~45–55 dB** (parasitics matter more here, hence the wider expected range).

**These calculated values are starting points.** Component parasitics (especially inductor SRF and trace inductance) shift the realized response. Both designs require a prototype tuning round before locking the BOM. See validation section below.

---

## Component Selection Rules

The filter performance is dominated by component quality. These rules are not negotiable:

### Capacitors

| Requirement | Why |
|-------------|-----|
| **C0G/NP0 dielectric only** | X7R/X5R have voltage and temperature coefficients that detune the filter. Q drops to ~50 at RF. C0G holds ±30 ppm/°C and Q >1000. |
| **±2% tolerance or better** | At 470 MHz, ±5% on a 15 pF cap shifts cutoff by several MHz |
| **50V minimum rating** | RF voltages can swing high inside resonant networks; 1W into 50Ω is 7V peak but internal node voltages can exceed this |
| **0402 size** | 0603 has too much parasitic inductance at UHF (~0.5 nH from pad/lead). 0402 is ~0.3 nH. 0201 is even better but harder to assemble. |

Recommended part family: **Murata GJM15 series** (0402, C0G, ±2%, 50V). Available on LCSC for JLCPCB assembly.

Specific part numbers will be locked during prototype tuning (see [VHF integration doc](../../hardware/sa818-v-carrier/LPF-INTEGRATION.md) and [UHF integration doc](../../hardware/sa818-u-carrier/LPF-INTEGRATION.md)).

### Inductors

| Requirement | Why |
|-------------|-----|
| **Wirewound, NOT multilayer** | Multilayer chip inductors have Q ~20–30 at VHF, abysmal at UHF. Wirewound inductors maintain Q >50 well into UHF. |
| **Q ≥ 50 at fc** | Lower Q increases insertion loss and broadens roll-off |
| **SRF ≥ 3× highest harmonic of interest** | Above SRF the inductor looks capacitive — filter behavior breaks. For VHF need SRF >1 GHz; for UHF need >2.5 GHz. |
| **±2% tolerance** | Same reasoning as caps; inductor variation directly shifts cutoff |
| **Current rating ≥ 200 mA** | 1W into 50Ω is 140 mA RMS, but resonant currents inside the network exceed this. 3× margin. |

Recommended part families:
- **Coilcraft 0603HP** series (wirewound, high-Q, datasheet S-parameters available)
- **Coilcraft 0603DC** series (wirewound, AEC-Q200 qualified, "more cost effective" per Coilcraft, slightly higher Q at low-inductance values)
- **Murata LQW18A** series (wirewound, lower cost, available on LCSC)

#### 0603HP vs 0603DC at our specific values

Coilcraft markets the 0603DC as a more cost-effective alternative with "up to 40% higher Q" than the 0603HP. The 40% claim is real but applies most strongly at low-inductance values (1.6–15 nH range), not at our larger LPF values. Verified from datasheets (0603HP: Doc 537, 2012; 0603DC: Doc 1605, 2022):

| L | Series | Q @ 250 MHz | Q @ 900 MHz | Q @ 1.7 GHz | Q @ 2.4 GHz | SRF |
|---|--------|------|------|------|------|------|
| 22 nH | 0603HP | 44 | 77 | 88 | — | 3.15 GHz |
| 22 nH | 0603DC | — | **88** | 84 | **56** | 2.70 GHz |
| 24 nH | 0603HP | 42 | 77 | **91** | — | **2.95 GHz** |
| 24 nH | 0603DC | NOT AVAILABLE — DC has no 24 nH part |
| 27 nH | 0603HP | 44 | 74 | **84** | — | **2.80 GHz** |
| 27 nH | 0603DC | — | **82** | 67 | 40 | 2.50 GHz |
| 68 nH | 0603HP | 46 | **73** | 49 | — | **2.00 GHz** |
| 68 nH | 0603DC | — | 63 | — | — | 1.65 GHz |
| 82 nH | 0603HP | 45 | 62 | — | — | **1.80 GHz** |
| 82 nH | 0603DC | — | **66** | — | — | 1.55 GHz |

Bold = winner. Key takeaways:

- **0603HP wins on SRF at every value** (~18–25% higher). Most relevant at UHF where 3rd harmonic approaches SRF.
- **0603DC has slightly higher Q at 900 MHz at small values** (10–15% better at 22, 27 nH). Reverses at larger values (HP wins at 68 nH).
- **At 1.7 GHz, 0603HP has higher Q across the board** (closer to its higher SRF gives it more headroom).
- **0603DC has no 24 nH part** — to use it, redesign UHF L6/L8 around 22 nH.
- **0603DC is AEC-Q200 Grade 1 qualified**; 0603HP is not. Irrelevant for hobby use but indicates DC is a newer, more rigorously qualified product line.
- **0603DC has 1.5–2× higher Irms rating**. Not relevant — we only need ~200 mA.

#### Recommendation per band

- **VHF (68, 82 nH):** Use 0603HP. SRF advantage matters, Q is comparable, no real upside to switching.
- **UHF (22 or 24 nH, 27 nH):** Worth empirically comparing during prototype tuning. The 0603DC-22N has standout Q numbers (88/84/56 at 900M/1.7G/2.4G) and could be a strong choice if we redesign L6/L8 around 22 nH instead of 24 nH. For L7 (27 nH) it's a wash — DC wins at 900 MHz, HP wins at 1.7 GHz. **Stock both 0603HP and 0603DC variants for the prototype tuning phase**, build small batches of each, and pick the winner from measurements.

Specific part numbers will be locked during prototype tuning.

---

## Layout Rules

The filter performance lives or dies by layout. These rules are based on standard amateur radio practice and Coilcraft / Murata application notes:

### 50Ω microstrip in/out

- Calculate trace width for the chosen PCB stackup. For 1.6mm FR4 (εr ≈ 4.3), 1 oz copper, microstrip on outer layer over solid ground: ~3.0mm wide.
- Use a microstrip calculator (KiCad has one built in: PCB Calculator → Transmission Lines).
- Trace width should taper smoothly into the filter section.

### Ground plane

- **Solid, unbroken ground plane directly under the filter section** on the layer below F.Cu.
- Every shunt capacitor's GND pad gets **at least 2 vias straight down** to the ground plane. Shared ground vias for adjacent caps are NOT acceptable — each cap needs its own ground return.
- Via fence around the entire filter section (vias every ~5mm along the boundary) for shielding.

### Inductor placement

- Adjacent series inductors should be mounted **perpendicular to each other** (rotated 90° relative to neighbors) to minimize mutual coupling. This is a common DIY filter mistake.
- Alternative: use shielded inductors. Coilcraft 0603HP, 0603DC, and Murata LQW18A series are all unshielded; shielded variants exist but cost more.

### Trace routing

- Signal path must be **linear** — no T-junctions, no stubs.
- Inductor pads connect directly to the next inductor or to the cap node. No long traces between filter elements.
- Total filter length: ~25mm at VHF, ~12mm at UHF.

### Tunability margin

Leave **two unpopulated 0402 cap footprints** in the filter section: one near the input, one near the output. These give a place to add 1–3 pF if a future board lot drifts. Leave them DNP in the BOM.

---

## Validation Strategy

This is a **tune-once-and-produce** design, not a per-unit-tune design. The plan to get there:

### Phase 1: Prototype tuning (5–10 boards)

1. Fab the carrier with calculated component values. Order extras of every value and a few neighboring values (e.g., 18, 22, 27 pF for an 22 pF position).
2. Assemble 5–10 boards.
3. **Measure each one with the NanoVNA**:
   - S21 (forward gain): plot from 100 MHz to 1.5 GHz. Record passband insertion loss, 2nd harmonic attenuation, 3rd harmonic attenuation.
   - S11 (input return loss): plot across passband. Should be better than -15 dB across the entire amateur band of interest.
4. Plot the distribution of all boards. Look at the **mean response**, not any individual board.
5. If the mean cutoff is off-target (too high or too low), substitute component values uniformly across all boards (e.g., 22 pF → 27 pF) and rebuild a few. Iterate maybe 1–3 times.
6. Once the mean response is on target, **lock the BOM** with specific manufacturer part numbers — no substitutions allowed at production.

### Phase 2: Pre-production validation (10 boards of locked design)

1. NanoVNA measure every board. Confirm passband and stopband cluster tightly around the locked design's response.
2. Drive 2–3 boards with a real SA818 at full power into a 30 dB attenuator → HackRF or NanoVNA spectrum mode. Verify 2nd harmonic stays below the target (-50 dBc on VHF, -60 dBc on UHF).
3. If clustering is tight and harmonics are below target, the design is cleared for the full production run.

### Phase 3: Production QC (5–10% spot-check on full run)

1. Build a **pogo-pin test fixture** that connects to the carrier's RF input and SMA output. Half a day of work, saves enormous time.
2. NanoVNA measures: insertion loss at center frequency, return loss in band, attenuation at 2nd harmonic. ~30 seconds per board.
3. Reject any obvious outlier (likely a solder defect, not a design issue). Rework or replace components on rejected boards.

### Why this works

Unit-to-unit variation across 100 boards comes from three sources, in decreasing order of impact:

1. **Component tolerance.** With ±2% C0G caps and ±2% wirewound inductors, expected passband cutoff variation is ±2–3%. Stopband attenuation varies ±3–5 dB. Both are well inside acceptable.
2. **PCB process variation.** Modern fabs (JLCPCB and similar) hold FR4 thickness and εr to ±5% on impedance. Same on every board.
3. **Assembly variation.** Pick-and-place is highly repeatable. Reflow is repeatable.

The bad reputation discrete LC filters have for "needing per-unit tuning" comes from QRP/homebrew kits using ±5–10% components and assuming a trim. Tighten the tolerances, lock the BOM to specific part numbers from a single lot, and per-unit variation drops below the QC threshold.

### Single-lot sourcing

For the production run of 100 boards, **buy all inductors from a single production lot**. Coilcraft and Murata typically hold ±0.5–1% within a single lot, which is much tighter than the ±2% datasheet spec. This eliminates lot-to-lot drift as a variable.

For 100 boards × 3 inductor positions × ≥2× margin = 600 inductors per value. Order in single lots from Digi-Key/Mouser (they show lot information on order).

---

## Equipment Required

Project owner has all of this:

| Tool | Used For |
|------|----------|
| NanoVNA (H4, V2, or Plus4) | S-parameter characterization 100 MHz–1.5 GHz. The workhorse. |
| HackRF One | Live verification with actual SA818 transmitting at full power. Covers up to 6 GHz. |
| RTL-SDR | Backup; useful for quick sanity checks |

**Not required:** lab-grade spectrum analyzer. The NanoVNA does the precision work; the HackRF confirms system-level performance with the actual transmitter.

**Always required:** 50Ω dummy load and at least 30 dB of RF attenuation between the SA818 and any SDR. 1W into a HackRF input will destroy it.

---

## Risks and Mitigations

| Risk | Mitigation |
|------|-----------|
| Calculated values don't land on standard E12 values | Prototype tuning round substitutes nearest standards; expected. |
| Inductor SRF too low at UHF | Use Coilcraft 0603HP series (high SRF); verify SRF in datasheet before sourcing. |
| Component parasitics shift response (especially UHF) | Prototype tuning round measures real response; values adjusted accordingly. |
| Lot-to-lot drift in production | Buy inductors in single lots for the entire run. |
| One-off solder defects in production | 5–10% NanoVNA spot-check catches outliers. |
| Cannot measure 2nd harmonic of UHF (870 MHz) accurately | NanoVNA H4 covers up to 1.5 GHz; HackRF covers up to 6 GHz. Both work. |
| Insertion loss higher than expected | Acceptable up to 2 dB; if higher, switch to 0603HP inductors (higher Q). |

---

## Cost Impact

Per carrier: **~$3.50 in additional components** (4 caps + 3 inductors + 2 unpopulated tunability footprints + assembly time).

Discrete LPF (chosen):
- 4× C0G 0402 caps: ~$0.60
- 3× wirewound inductors: ~$2.40
- 2× DNP cap footprints: $0
- Total: ~$3.00 BOM + ~$0.50 assembly time

Ceramic chip alternative (rejected):
- 1× ceramic LPF chip: ~$1.50–3.00
- Total: ~$1.50–3.00

**Cost premium for discrete: ~$0.50–2.00 per carrier.** Negligible for the performance gain (additional 20+ dB stopband attenuation, lower insertion loss, full Part 97 compliance margin).

---

## Decision

**Add a 7-pole Chebyshev LPF to both the SA818-V (VHF, fc=155 MHz) and SA818-U (UHF, fc=470 MHz) carrier boards.**

Implementation steps and KiCad-specific procedures: see the per-board integration documents linked at the top of this file.

---

*Last updated: 2026-04-30*
