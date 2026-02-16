# RDA1846S vs SA818: Transceiver Architecture Analysis

**Date:** 2026-02-16
**Context:** Evaluating whether to replace the SA818 module with the bare RDA1846S transceiver IC it contains internally.

---

## Background

The SA818 module from NiceRF is built around an **RDA1846S** (or pin-compatible AT1846S) transceiver IC. The SA818 wraps this chip with an MCU, TCXO, external PA, RF filtering, T/R switching, and a shielded can — exposing a simple UART/AT-command interface.

The question: should we skip the SA818 module and use the raw RDA1846S chip directly on the badge PCB?

---

## What the RDA1846S Integrates

The RDA1846S is effectively a single-chip SDR for FM transceivers. It includes:

| Built-In Block | What It Does |
|----------------|-------------|
| RF front-end | LNA, I/Q mixers, VCO, PLL/synthesizer |
| Audio ADC | Converts analog mic input to digital for DSP |
| Audio DAC | Drives 32-ohm speaker directly (AC-coupled) |
| FM DSP | Modulation, demodulation, filtering, AFC, AGC |
| CTCSS/DCS | Encode and decode (not needed for SSTV) |
| DTMF | Encode and decode (not needed for SSTV) |
| Squelch | RSSI + noise-based, programmable |
| VOX | Voice-operated transmit (not needed) |
| Pre/de-emphasis | Selectable |
| LDO regulators | Internal power regulation |
| 8 dBm PA | On-chip power amplifier (~6 mW) |

**Package:** QFN-32, 5x5mm, 0.5mm pitch
**Interface:** 3-wire SPI or I2C (register-based, ~100+ registers)
**Supply:** 3.3V-4.8V
**Frequency:** 134-174 MHz, 200-260 MHz, 400-520 MHz
**Crystal:** 12.8, 13, 25.6, or 26 MHz

### What It Does NOT Have

- **No power amplifier beyond 8 dBm** — to reach 1W (30 dBm) you need a ~22 dB external PA
- **No T/R switch** — need external antenna switching between TX PA output and RX LNA input
- **No harmonic filtering** — need external low-pass filter on TX output for FCC compliance
- **No firmware** — purely register-controlled, requires host MCU for all operations
- **No persistent configuration** — no EEPROM, settings lost on power cycle

---

## What the SA818 Module Adds

| SA818 Addition | Purpose |
|----------------|---------|
| Microcontroller | AT command parser, persistent config |
| EEPROM | Stores frequency/settings across power cycles |
| TCXO (1 PPM) | Precision frequency reference |
| External PA | Boosts 8 dBm → 30 dBm (1W) |
| RF matching/filtering | Low-pass filter for harmonic suppression |
| T/R switching | Antenna sharing between TX and RX |
| Metal shield can | RF isolation |
| Audio conditioning | Input/output level matching |

---

## Side-by-Side Comparison

### Engineering Effort

| Task | SA818 (Current Design) | Bare RDA1846S |
|------|------------------------|---------------|
| RF trace routing | None (all on carrier) | 50Ω traces, via stitching, ground planes |
| PA design | Done (inside module) | Design external PA stage (22 dB gain, 1W) |
| T/R switch | Done (inside module) | Design antenna switching circuit |
| LPF for harmonics | Done (inside module) | Design and validate low-pass filter |
| Antenna matching | Done (inside module) | Design matching network |
| Frequency control | UART AT commands | SPI register writes (~100+ registers, complex init sequence) |
| Audio path | Analog MIC/SPK pins | Can use internal ADC/DAC or analog pins |
| Calibration | Factory done | Manual AGC tuning tables, calibration routines |
| RF shielding | Metal can included | Design shielding solution |
| FCC compliance testing | Module pre-tested | Full responsibility on us |

**Estimated engineering time for bare RDA1846S approach: 4-8 weeks** of RF design, layout, testing, and iteration — assuming RF design experience. Without RF experience, significantly longer.

### Cost Per Unit

| Component | SA818 Approach | RDA1846S Approach |
|-----------|---------------|-------------------|
| Transceiver | SA818 ~$10 | RDA1846S ~$3 |
| TCXO | Included | ~$1.50 |
| External PA + bias | Included | ~$2-3 |
| T/R switch | Included | ~$0.50 |
| LPF + matching | Included | ~$0.50 (passives) |
| RF shield | Included | ~$1 (can or pour) |
| PCM5102A DAC | $1.75 | $1.75 (still needed — see audio analysis) |
| RX filter passives | ~$0.30 | ~$0.30 |
| Carrier PCB | ~$1 | N/A (on main board now) |
| SMA connector | ~$2 | ~$2 |
| Antenna | ~$4 | ~$4 |
| **Subtotal (radio)** | **~$19** | **~$17** |
| **Savings** | — | **~$2/unit** |

The cost savings are negligible — roughly $2 per badge.

### SSTV Audio Compatibility

This is the key technical question: does the RDA1846S's built-in audio processing help or hurt SSTV?

**SSTV frequency content:**
- Sync pulse: 1200 Hz
- Black level: 1500 Hz
- White level: 2300 Hz
- All content falls within 1200-2300 Hz

**RDA1846S internal audio filtering:**
- High-pass filter blocks below ~300 Hz (designed to remove CTCSS tones at 67-254 Hz)
- Voice-band passband: ~300-3000 Hz
- SSTV tones at 1200-2300 Hz fall **entirely within the passband**

**Conclusion:** The SA818's audio filtering does NOT harm SSTV signals. All SSTV content is well within the voice-band passband. The built-in high-pass filter only removes sub-300Hz content, which SSTV doesn't use.

**TX path comparison:**

| Approach | SA818 | Bare RDA1846S |
|----------|-------|---------------|
| Source | PCM5102A DAC → analog → SA818 MIC pin | PCM5102A DAC → analog → RDA1846S MIC pin |
| Alternative | — | Could use RDA1846S internal DAC (but lower quality than PCM5102A) |
| FM modulation | SA818 handles it | RDA1846S handles it (same chip inside) |

Either way, the PCM5102A DAC generates the SSTV audio waveform. The RDA1846S's internal DAC is lower quality and would require routing I2S or digital audio to the chip — more complex for no benefit.

**RX path comparison:**

| Approach | SA818 | Bare RDA1846S |
|----------|-------|---------------|
| Output | SA818 SPK pin → filter → RP2350 ADC | RDA1846S SPK pin → filter → RP2350 ADC |
| Alternative | — | Could read RDA1846S internal ADC via SPI registers (but slow, adds latency) |

The RP2350's ADC with our external anti-alias filter is better suited for SSTV capture than reading digital samples from the RDA1846S via SPI.

**Bottom line:** The RDA1846S's built-in audio features don't provide meaningful advantages for SSTV. The audio path is functionally identical either way.

### Assembly and Manufacturing

| Factor | SA818 | Bare RDA1846S |
|--------|-------|---------------|
| Package | Castellated module (fab-assembled on carrier) | QFN-32, 0.5mm pitch (reflow only) |
| PA components | Inside module | Additional discrete components |
| RF passives | Inside module | ~15-20 extra passives on main board |
| User-solderable? | No (carrier is fab-assembled) | No (QFN-32 requires reflow) |
| Main board complexity | Zero RF, potentially 2-layer | RF traces force 4-layer minimum |
| Testing | Module is pre-tested | Must validate RF performance ourselves |

### Sourcing and Risk

| Factor | SA818 | Bare RDA1846S |
|--------|-------|---------------|
| Distributor | NiceRF direct, Envistia Mall, AliExpress | AliExpress/eBay ONLY |
| Mouser/DigiKey | No | No |
| Counterfeit risk | Moderate (established vendor) | High (marketplace sellers) |
| Datasheet quality | Good (AT command reference) | Incomplete (many undocumented registers) |
| Community support | kv4p HT, many ham projects | HamShield, OpenGD77 (smaller community) |
| Production status | Active | Unclear (RDA acquired by Unisoc) |

### Carrier Board Impact

| Factor | SA818 | Bare RDA1846S |
|--------|-------|---------------|
| Carrier board needed? | Yes (isolates RF, enables band swap) | Possibly not — but then RF is on main board |
| Band swapping | Swap entire carrier | Redesign for switchable PA/filtering or abandon band swap |
| Main board RF traces | None | 50Ω impedance-controlled, via stitching, ground planes |

If we go bare RDA1846S, we either:
1. **Keep a carrier board** — but now we're designing our own RF module from scratch (negating the whole point)
2. **Put RF on the main board** — losing the clean separation that makes the current design elegant, forcing 4-layer, and making band-swapping much harder

---

## Notable Precedent: HamShield

The [HamShield](https://github.com/EnhancedRadioDevices/HamShield) project by Enhanced Radio Devices successfully used the bare A1846S (Auctus variant, pin-compatible with RDA1846S) for amateur radio including SSTV. Key takeaways:

- They limited output to **250 mW** (not 1W) to simplify PA design and FCC compliance
- They wrote a substantial Arduino library for register programming
- The project took **significant development time** for RF validation
- Open-source KiCad files are available as reference

If we were to pursue the bare chip approach, HamShield would be the starting point for both hardware reference design and register programming code.

---

## Decision Matrix

| Criterion | Weight | SA818 | RDA1846S | Notes |
|-----------|--------|-------|----------|-------|
| Engineering time | High | **10** | 3 | 4-8 weeks of RF design vs already done |
| Cost per unit | Medium | 7 | **8** | ~$2 savings, marginal |
| SSTV audio quality | High | **8** | 8 | Functionally identical for SSTV |
| Assembly complexity | High | **9** | 5 | Module vs QFN + PA + passives |
| RF performance risk | High | **9** | 5 | Pre-tested module vs DIY RF |
| Board simplicity | High | **10** | 4 | No RF on main board vs 50Ω routing |
| Band swapping | Medium | **9** | 4 | Carrier swap vs major redesign |
| Sourcing reliability | Medium | **7** | 4 | Both China-sourced, module more established |
| Hackability/education | Low | 5 | **8** | Bare chip is more interesting to tinker with |
| **Weighted Total** | | **~8.5** | **~5.0** | |

---

## Recommendation: Stay with SA818

The SA818 module is the right choice for this project. The reasons:

1. **No SSTV benefit.** The RDA1846S's built-in audio features don't improve SSTV signal quality. The audio path is functionally identical either way — we still need the PCM5102A DAC and RP2350 ADC.

2. **Massive time investment for $2 savings.** 4-8 weeks of RF design work to save ~$2 per badge is not a good trade-off, especially with DEFCON deadlines.

3. **The carrier board architecture is elegant.** Zero RF on the main board, band-swappable carriers, pre-tested RF — this is a feature, not a compromise.

4. **RF design is the hardest part of radio projects.** PA design, harmonic filtering, T/R switching, impedance matching, and FCC compliance testing require specialized expertise and equipment (spectrum analyzer, VNA). The SA818 module packages all of this in a tested, shielded package.

5. **Risk management.** With DEFCON as a hard deadline, introducing RF design risk is unwise. The SA818 is a known-working solution used by many ham radio projects (kv4p HT, etc.).

The bare RDA1846S would be an interesting choice for a v2.0 badge or a dedicated RF experimentation project where the journey is the goal. For DEFCON 33, the SA818 module is the pragmatic choice.

---

## References

- [RDA1846S Datasheet](https://www.elinfor.com/pdf/RDA/RDA1846-RDA.pdf)
- [AT1846S Programming Guide (OpenGD77)](https://www.opengd77.com/downloads/datasheets/AT1846S/RDA1846S_programming_guide_1.2.pdf)
- [HamShield Project](https://hackaday.io/project/6749-hamshield-vhfuhf-transceiver-for-arduino)
- [HamShield Arduino Library (GitHub)](https://github.com/EnhancedRadioDevices/HamShield)
- [HamShieldMini Hardware (GitHub)](https://github.com/EnhancedRadioDevices/HamShieldMini)
- [OpenGD77 Firmware (GitHub)](https://github.com/open-ham/OpenGD77)
- [kv4p HT (uses SA818)](https://kv4p.com/)
- [SOCORAD32 — ESP32 + RDA1846 (GitHub)](https://github.com/MordFIdel/SOCORAD32)
- [SA818 Module (NiceRF)](https://www.nicerf.com/walkie-talkie-module/sa818-embedded-walkie-talkie-module.html)

---

*Last updated: 2026-02-16*
