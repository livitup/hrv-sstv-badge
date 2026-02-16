# Electrical Design Decisions

This document captures the design decisions, trade-off analyses, and rationale behind the electrical design of the DEFCON SSTV Badge. For the current design specs and schematics, see [electrical-design.md](../electrical-design.md).

---

## Power System

### Problem with the Original Design

The original design used an AMS1117-3.3 LDO to regulate the LiPo battery down to 3.3V. This had several critical issues:

| Issue | Detail |
|-------|--------|
| **Dropout voltage** | AMS1117 requires ~1.2V headroom (needs 4.5V+ input for 3.3V output) |
| **LiPo voltage range** | 4.2V (full) → 3.7V (nominal) → 3.0V (empty) |
| **Result** | Regulator falls out of regulation before battery is half depleted |
| **Current rating** | AMS1117 rated 1A max; SA818 TX peaks at 1.7A |
| **Thermal** | LDO dissipates (Vin-Vout)*I as heat — no thermal solution specified |

The key insight: **Don't run the SA818 through the main 3.3V regulator.** The SA818 module has its own internal regulation and accepts 3.5V-5.5V input directly. Running it from the LiPo (which stays in that range until nearly depleted) means the main 3.3V rail only needs to handle ~250-400mA.

### Buck-Boost Regulator Selection

**Requirements:**
- Input: 3.0V - 5.0V (full LiPo range + MCP73871 SYS output)
- Output: 3.3V regulated
- Load: 250mA typical, 500mA peak with headroom
- High efficiency for battery life
- Hand-solderable for prototyping

#### Candidates Evaluated

**TPS63001 (Texas Instruments) — SELECTED**

| Spec | Value |
|------|-------|
| Input | 1.8V - 5.5V |
| Output | Fixed 3.3V |
| Max Current | 1.2A |
| Peak Efficiency | ~93% |
| Light Load (10mA) | ~80% |
| Package | 3x3mm QFN-10 |
| Price (qty 1) | ~$1.80 |
| Price (qty 100) | ~$1.20 |

**Pros:**
- Fixed 3.3V output — fewer external components
- QFN-10 is hand-solderable with hot air
- Extensive documentation and reference designs
- Power-save mode for good light-load efficiency
- Well-proven part

**Cons:**
- 1.2A limit (sufficient for this application with SA818 on separate rail)

**TPS63802 (Texas Instruments)**

| Spec | Value |
|------|-------|
| Input | 1.8V - 5.5V |
| Output | Fixed 3.3V |
| Max Current | 2A |
| Peak Efficiency | ~95% |
| Light Load (10mA) | ~85% |
| Package | 2x1.5mm WCSP-9 |
| Price (qty 1) | ~$2.80 |

**Pros:** Best efficiency, 2A headroom, tiny
**Cons:** WCSP package difficult to hand solder

**TPS63021 (Texas Instruments)**

| Spec | Value |
|------|-------|
| Input | 1.8V - 5.5V |
| Output | Fixed 3.3V |
| Max Current | 3A |
| Peak Efficiency | ~93% |
| Light Load (10mA) | ~75% |
| Package | 3.5x3.5mm QFN-14 |
| Price (qty 1) | ~$3.20 |

**Pros:** 3A capacity, very robust
**Cons:** Weaker light-load efficiency, overkill current rating

**RT6150B (Richtek)**

| Spec | Value |
|------|-------|
| Input | 2.5V - 5.5V |
| Output | Fixed 3.3V |
| Max Current | 2A |
| Peak Efficiency | ~94% |
| Package | 3x3mm QFN-12 |
| Price (qty 1) | ~$1.10 |

**Pros:** Cheapest option, good efficiency
**Cons:** 2.5V minimum input (less margin), less documentation

#### Efficiency Comparison

```
Efficiency @ 3.7V input → 3.3V output

Load:      10mA    50mA   100mA   250mA   500mA
           ────    ────   ─────   ─────   ─────
TPS63802   85%     92%     94%     95%     94%    ← Best overall
TPS63001   80%     88%     91%     93%     92%    ← Selected
TPS63021   75%     85%     90%     93%     93%    ← Weak at light load
RT6150B    82%     90%     93%     94%     93%    ← Good value
```

#### Battery Life Impact

With 2000mAh battery, 155mA idle current:

| Regulator | Efficiency | Actual Drain | Runtime |
|-----------|------------|--------------|---------|
| TPS63001 | 90% | 172mA | 11.6 hours |
| TPS63802 | 94% | 165mA | 12.1 hours |

Difference is ~30 minutes — not dramatic enough to justify WCSP package difficulty.

#### Decision

**Use TPS63001** for both prototype and production:
- QFN-10 is manageable with hot air for prototypes
- Fab house handles it identically to WCSP in production
- Cost-effective
- Well-documented with proven reference designs

---

## Battery Management

### Why MCP73871?

- Integrated power path management (load sharing)
- System runs from USB when connected, seamlessly switches to battery
- Programmable charge current
- Thermal regulation protects battery
- Status outputs for charge indication

### Why LC709203F?

- No sense resistor needed (uses battery model)
- I2C interface (shares bus with camera, SAO)
- Accurate SoC even at low discharge rates
- Low quiescent current (~4µA)
- DFUN technology for various battery chemistries

### Skipping Dedicated Status GPIOs

We have 0 spare GPIOs, so STAT1/STAT2/PG from the MCP73871 may need to be directly connected to LEDs (with transistors) rather than read by the MCU. Alternatively, the MCP73871 status can be inferred from LC709203F readings (charging = voltage rising, complete = voltage stable at 4.2V).

**Decision:** Skip dedicated status GPIOs — use LC709203F I2C readings to determine charge state. The fuel gauge already tells us:
- Battery voltage (4.2V = full, <3.3V = critical)
- RSOC percentage (0-100%)
- Charging can be inferred from voltage trend

This saves 3 GPIOs and simplifies the design.

---

## Audio Interface

### Why PCM5102A + ADC (Not WM8960 Codec)?

The original design specified a WM8960 stereo codec. We removed it because:

| Factor | WM8960 | PCM5102A + ADC |
|--------|--------|----------------|
| Cost | ~$3.50 | ~$2.00 |
| Package | QFN (hard to inspect) | TSSOP-20 (visible leads) |
| TX quality | Excellent | Excellent |
| RX quality | Excellent | Good (adequate for indoor use) |
| Complexity | I2C configuration required | Hardware config, no I2C |
| Speaker output | Yes (unused) | No |

The WM8960's extra features (stereo, speaker amp, mixer) aren't needed. The PCM5102A is simpler and cheaper.

### Assembly Risk Assessment

If a user makes a mistake on the solderable audio components:

| Problem | Result |
|---------|--------|
| Bad TX filter (R1, R2, C3, C4) | Distorted or weak transmit, but probably still works |
| Bad RX filter (R3-R6, C5, C6) | Degraded receive sensitivity, but TX still works |
| Missing bias (R5, R6) | ADC clips signal, poor receive |

None of these failures are catastrophic — the badge will still partially function.

---

## SA818 Carrier Board

### Why a Separate Carrier Board?

| Benefit | Explanation |
|---------|-------------|
| No RF on main board | Eliminates 50Ω trace routing, via stitching, ground plane complexity |
| Simpler main board | Could potentially be 2-layer (depends on RP2350 routing density) |
| Band-matched antenna | VHF carrier includes VHF stubby, UHF includes UHF stubby |
| User experience | Swap entire RF assembly as one unit, no antenna juggling |
| Testable independently | Validate RF performance before integrating with main badge |
| Aesthetic | Visible carrier with antenna = retro TV look, good for #badgelife |

### Main Board PCB Layers (2 vs 4)

With RF removed from main board, evaluate layer requirements:

| Consideration | 2-Layer | 4-Layer |
|---------------|---------|---------|
| RP2350 routing | Challenging | Comfortable |
| Power integrity | Needs careful planning | Dedicated planes |
| Cost | Cheaper | ~$5-10 more per board |
| EMI/noise | Adequate if careful | Better with planes |

**Decision:** Start with 4-layer design for comfort. Optimize to 2-layer later if cost pressure requires it and routing is achievable.

---

## Display Interface

### Why 320x240?

SSTV modes have varying resolutions:

| Mode | Resolution | Display Strategy |
|------|------------|------------------|
| Robot 36/72 | 320x240 | Perfect 1:1 fit |
| Martin M1/M2 | 320x256 | Crop 16-line gray banner, or center with bars |
| Scottie S1/S2 | 320x256 | Crop 16-line gray banner, or center with bars |
| PD 90 | 320x240 | Perfect 1:1 fit |
| PD 120/180/240 | 320x480 | Scale to fit |

Martin and Scottie's 256 lines include 16 gray lines reserved for callsign banners — the actual image content is 240 lines. The 320x240 display covers the vast majority of real-world SSTV traffic (Martin M1, Scottie S1, Robot 36/72) with native or near-native resolution.

---

## Camera Interface

### Why Parallel DVP (Not SPI)?

| Factor | Parallel DVP | SPI Module |
|--------|--------------|------------|
| Live viewfinder | Fast, 30+ FPS | Slow, ~1-2 FPS |
| GPIO usage | 12 dedicated | 4 dedicated |
| Capture control | Full timing control | Module handles it |
| Cost | ~$5-8 | ~$10-15 |

**Live viewfinder requires parallel DVP.** Showing real-time preview before capture is essential for a camera badge. SPI modules are too slow for this.

### GPIO Budget: RP2350B Required

Parallel DVP uses 12 GPIOs for the camera alone. Combined with display, audio, SA818, and user controls, we need ~40 GPIOs total.

| RP2350 Variant | Package | GPIOs | Status |
|----------------|---------|-------|--------|
| RP2350A | QFN-60 | 30 | Not enough |
| **RP2350B** | **QFN-80** | **48** | **Required** |

---

## MCU Selection

### Why RP2350B (QFN-80)?

The original design mentioned RP2350 generically. After tallying GPIO requirements:

| Subsystem | Dedicated GPIOs |
|-----------|-----------------|
| Display + SD | 5 |
| Audio (I2S + ADC) | 4 |
| SA818 control | 5 |
| User controls | 5 |
| SAO extra | 2 |
| Camera DVP | 12 |
| **Total dedicated** | **~33** |
| Shared buses | +7 |
| **Total needed** | **~40** |

| Variant | Package | GPIOs | Verdict |
|---------|---------|-------|---------|
| RP2350A | QFN-60 | 30 | Not enough |
| **RP2350B** | **QFN-80** | **48** | **Selected** |

### Why a Crystal is Required

The RP2350 has an internal Ring Oscillator (ROSC), but it's too inaccurate:

| Requirement | Tolerance Needed | ROSC Accuracy | Crystal Accuracy |
|-------------|------------------|---------------|------------------|
| USB 2.0 | ±0.25% | ±2-5% | ±20ppm |
| SSTV line timing | ±0.1% | ±2-5% | ±20ppm |
| I2S audio | Low jitter | Variable | Stable |

**Bottom line:** USB won't work without a crystal. SSTV timing precision is a bonus.

### Crystal Recommendations

- ABM8-12.000MHZ-B2-T (Abracon) — 3215, ±20ppm, CL=10pF, ~$0.35
- ECS-120-10-36Q-ES-TR (ECS) — 3215, ±10ppm, CL=10pF, ~$0.40
- FA-238 12.0000MF10Z-K3 (Epson) — 3215, ±10ppm, CL=10pF, ~$0.50

### Why 16MB Flash?

- Firmware: ~1-2MB typical
- SSTV images: ~150KB each (320x240 RGB565)
- Stored images: 50+ images easily
- Future expansion: Plenty of headroom

### Flash Alternatives

- W25Q128JVSIM (SOIC-8 wide) — easier hand soldering
- W25Q64JVSSIQ (8MB) — cheaper, still plenty of space
- GD25Q128E (GigaDevice) — drop-in compatible, often cheaper

---

## USB-C Interface

### Alternate Part Options

**USB-C Connector alternatives:**
- USB4110-GF-A (GCT) — Mid-mount SMD if TH doesn't fit
- TYPE-C-31-M-12 (Korean Hroparts) — Budget option

**ESD Protection alternatives:**
- TPD2E001 (TI) — Single-channel, need 2 for D+/D-
- ESD5V3U4RS (Infineon) — Integrated VBUS + data protection

---

## User Controls

### Why Traditional RGB LED (Not WS2812B) for PWR Status?

- **Visually distinct** — smaller package, clearly "utility" vs "blinky"
- **Independent** — works even if WS2812B chain has issues
- **Reliable** — direct GPIO drive, no protocol timing required
- **Always visible** — battery status is critical info, separate from animations

### Debouncing Strategy

**Hardware debouncing:** Add 100nF capacitor from GPIO to GND (optional, increases BOM).

**Software debouncing (selected):**
- Sample switch state
- Require stable reading for 10-20ms before registering change
- Simpler, no extra components

---

*Last updated: 2026-02-15*
