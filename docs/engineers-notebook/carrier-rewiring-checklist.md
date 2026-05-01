# Carrier Connector Rewiring Checklist

**Status:** Pending — both carriers need this work before fabrication.

Both SA818 carrier boards (VHF and UHF) currently have J14/J18 (Power/Audio) and J15/J20 (Control/Data) pin assignments that **do not match** the main badge J7/J8 pin assignments. When a carrier is plugged into the main badge, every signal except VCC, GND, and ID lands on the wrong pin.

The main badge pinout is **locked** (routing complete). The carriers must be rewired to match.

For background and how this happened, see [carrier connector mismatch findings](#why-this-needs-doing) at the bottom of this doc.

---

## Source of Truth: Main Badge J7 and J8

These are the locked pin assignments that the carriers must mate with.

### J7 — Power/Audio (main badge)

| Pin | Net |
|-----|-----|
| 1 | SA818_VCC |
| 2 | GND |
| 3 | GND |
| 4 | MIC+ |
| 5 | GND |
| 6 | SPK+ |
| 7 | GND |
| 8 | (unconnected — see note below) |

> **Note on pin 8:** main badge J7 pin 8 is currently floating (the original spec called for GND, but the trace was never routed and the main badge is locked). The carriers will provide GND on pin 8, which makes the pin GND when mated. No issue.

### J8 — Control/Data (main badge)

| Pin | Net |
|-----|-----|
| 1 | SA818_RX |
| 2 | SA818_TX |
| 3 | SA818_PTT |
| 4 | SA818_PD |
| 5 | SA818_HL |
| 6 | SA818_SQ |
| 7 | SA818_ID |
| 8 | GND |

> **UART naming convention:** "SA818_RX" is the main badge's UART RX line — data flows FROM the SA818's TXD pin INTO the main badge. "SA818_TX" is the main badge's UART TX — data flows FROM the main badge INTO the SA818's RXD pin. Carrier-side wiring must connect SA818 TXD to J15/J20 pin 1 (mates with SA818_RX) and SA818 RXD to J15/J20 pin 2 (mates with SA818_TX).

---

## Required Carrier Pinouts (after rewiring)

These are the **target** pin assignments. Both carriers (VHF and UHF) get the same changes — different connector references but same logic.

### J14 (VHF) / J18 (UHF) — Power/Audio

| Pin | Connect to | Currently | Change required |
|-----|-----------|-----------|-----------------|
| 1 | U9/U10 VDD (pad 8) | VDD | ✓ no change |
| 2 | GND | GND | ✓ no change |
| 3 | **GND** | MIC_IN | **disconnect MIC_IN from pin 3, connect GND** |
| 4 | **U9/U10 MIC_IN (pad 18)** | GND | **disconnect GND, connect MIC_IN** |
| 5 | **GND** | AF_OUT | **disconnect AF_OUT from pin 5, connect GND** |
| 6 | **U9/U10 AF_OUT (pad 3)** | GND | **disconnect GND, connect AF_OUT** |
| 7 | GND | GND | ✓ no change |
| 8 | GND | GND | ✓ no change |

**Net effect:** MIC moves from pin 3 → pin 4. SPK moves from pin 5 → pin 6. (One-pin shift for the audio signals.)

### J15 (VHF) / J20 (UHF) — Control/Data

| Pin | Connect to | Currently | Change required |
|-----|-----------|-----------|-----------------|
| 1 | **U9/U10 TXD (pad 17)** | ~PTT (pad 5) | **rewire** |
| 2 | **U9/U10 RXD (pad 16)** | ~PD (pad 6) | **rewire** |
| 3 | **U9/U10 ~PTT (pad 5)** | H/L (pad 7) | **rewire** |
| 4 | **U9/U10 ~PD (pad 6)** | ~SQ (pad 1) | **rewire** |
| 5 | **U9/U10 H/L (pad 7)** | RXD (pad 16) | **rewire** |
| 6 | **U9/U10 ~SQ (pad 1)** | TXD (pad 17) | **rewire** |
| 7 | R18 (band ID) | R18 | ✓ no change |
| 8 | GND | GND | ✓ no change |

**Net effect:** UART pins move from 5/6 to 1/2 (and TXD/RXD swap, since SA818 TXD must mate with main badge RX). Control signals (PTT/PD/HL/SQ) shift from pins 1-4 to pins 3-6.

---

## Schematic Edit Steps

Do this for **both** carriers (sa818-v-carrier.kicad_sch and sa818-u-carrier.kicad_sch). The procedure is identical; only the designators differ (U9/J14/J15 on VHF; U10/J18/J20 on UHF).

### Step 1: Open the carrier schematic in KiCad

Open `hardware/sa818-v-carrier/sa818-v-carrier.kicad_sch` first. After completing it, repeat for `hardware/sa818-u-carrier/sa818-u-carrier.kicad_sch`.

### Step 2: Rewire J14/J18 (Power/Audio)

The SA818 module connects to J14/J18 via three signal wires (VDD on pin 1, MIC_IN on pin 3, AF_OUT on pin 5). Two of those need to move.

1. **Locate the wire connecting U9/U10 pad 18 (MIC_IN) to J14/J18 pin 3.**
2. Delete the wire segment that lands on J14/J18 pin 3.
3. Draw a new wire segment from the existing MIC_IN trace to J14/J18 pin 4.
4. Add a GND symbol on J14/J18 pin 3 (the now-orphaned pin).
5. **Locate the wire connecting U9/U10 pad 3 (AF_OUT) to J14/J18 pin 5.**
6. Delete the wire segment that lands on J14/J18 pin 5.
7. Draw a new wire segment from the existing AF_OUT trace to J14/J18 pin 6.
8. Add a GND symbol on J14/J18 pin 5 (the now-orphaned pin).
9. Remove the GND symbols that were originally on pins 4 and 6 (replaced by the signal wires now).

### Step 3: Rewire J15/J20 (Control/Data)

This is the bigger change. Six signal wires all need to land on different pins.

The fastest approach is to **delete all 6 signal wires between U9/U10 and J15/J20** and then redraw them per the new pin map. Don't change anything about U9/U10 — only the J15/J20 endpoints move.

| Signal | From U9/U10 pad | New J15/J20 pin |
|--------|-----------------|-----------------|
| TXD | pad 17 | pin 1 |
| RXD | pad 16 | pin 2 |
| ~PTT | pad 5 | pin 3 |
| ~PD | pad 6 | pin 4 |
| H/L | pad 7 | pin 5 |
| ~SQ | pad 1 | pin 6 |

R18 stays connected to pin 7. GND stays on pin 8. No changes there.

> **Tip:** in KiCad, you can move a wire endpoint by clicking it and dragging, instead of deleting and redrawing. This is faster but more error-prone — ensure you don't accidentally connect to a different pin.

### Step 4: Run ERC

Save and run **Inspect → Electrical Rules Checker**. Expected: zero errors. If any pin shows up as disconnected or with multiple drivers, you've introduced a wiring error — fix before continuing.

### Step 5: Update PCB from Schematic

Open the PCB editor. **File → Update PCB from Schematic** (`F8`). KiCad will report changed connections — this is expected. Apply the changes.

The PCB will now have **rip-up ratsnest lines** for all the moved signals — they need to be re-routed.

---

## PCB Re-routing Steps

### Step 6: Identify affected traces

The original carrier PCB had short, direct traces from the SA818 module to J14/J15. With the new pin assignments, those traces need to land on different pads. Most likely you'll need to:

1. Delete the old traces between U9/U10 and J14/J15 (and J18/J20 on UHF).
2. Re-route from scratch.

### Step 7: Re-route signal traces

The carriers are simple boards (no high-speed signals on these pins, all are DC or low-speed UART). Standard 0.2mm trace width is fine.

**Recommended order:**
1. Re-route VDD (pin 1) and GND fills first (high-current paths).
2. Re-route audio signals (MIC_IN to pin 4, AF_OUT to pin 6) — short, direct paths from SA818 audio pins.
3. Re-route control signals (PTT, PD, HL, SQ) to pins 3-6.
4. Re-route UART (TXD to pin 1, RXD to pin 2) — these are long traces from the SA818's UART pins (pad 16/17) all the way to the front-edge connector pins 1/2.

> **Note on UART trace routing:** TXD and RXD currently route from pads 16/17 to J15/J20 pins 5/6 (closest pins). After rewiring, they need to route to pins 1/2 — likely longer traces. UART at 9600 baud is forgiving; trace length isn't a problem.

### Step 8: Run DRC

**Inspect → Design Rules Checker.** Resolve any errors. Should be clean — same design rules as before, just different signal-to-pad mappings.

### Step 9: Visual inspection

- All 6 signals on J15/J20 pins 1-6 connected to expected SA818 pads
- Audio signals on J14/J18 pins 4 and 6
- GND fills uninterrupted under all signal traces
- No orphan vias or stub traces from the old routing

---

## Validation: Cross-Project Consistency Check

After both carriers are updated, validate by comparing pad nets across the three KiCad projects.

### Step 10: Net-by-net cross-check

For each pin, the main badge net name must match what the carrier connects to functionally:

| Connector pin | Main badge net | VHF carrier connects to | UHF carrier connects to | Status |
|---------------|---------------|-------------------------|-------------------------|--------|
| J7/J14/J18 pin 1 | SA818_VCC | U9 pad 8 (VDD) | U10 pad 8 (VDD) | should match |
| J7/J14/J18 pin 4 | MIC+ | U9 pad 18 (MIC_IN) | U10 pad 18 (MIC_IN) | should match (after fix) |
| J7/J14/J18 pin 6 | SPK+ | U9 pad 3 (AF_OUT) | U10 pad 3 (AF_OUT) | should match (after fix) |
| J8/J15/J20 pin 1 | SA818_RX | U9 pad 17 (TXD) | U10 pad 17 (TXD) | should match (after fix) |
| J8/J15/J20 pin 2 | SA818_TX | U9 pad 16 (RXD) | U10 pad 16 (RXD) | should match (after fix) |
| J8/J15/J20 pin 3 | SA818_PTT | U9 pad 5 (~PTT) | U10 pad 5 (~PTT) | should match (after fix) |
| J8/J15/J20 pin 4 | SA818_PD | U9 pad 6 (~PD) | U10 pad 6 (~PD) | should match (after fix) |
| J8/J15/J20 pin 5 | SA818_HL | U9 pad 7 (H/L) | U10 pad 7 (H/L) | should match (after fix) |
| J8/J15/J20 pin 6 | SA818_SQ | U9 pad 1 (~SQ) | U10 pad 1 (~SQ) | should match (after fix) |
| J8/J15/J20 pin 7 | SA818_ID | R18 → GND (VHF) / R18 DNP (UHF) | same | unchanged |
| J8/J15/J20 pin 8 | GND | GND | GND | unchanged |

Quick verification command (once both carriers are updated):

```bash
# From repo root, sanity check all three projects' pad nets agree
grep -A 2 'pad "[1-8]" thru_hole' hardware/main_badge/defcon-sstv-badge.kicad_pcb | grep 'net.*"SA818\|net.*"GND\|net.*"MIC\|net.*"SPK'
grep -A 1 'pad "[1-8]" thru_hole' hardware/sa818-v-carrier/sa818-v-carrier.kicad_pcb | grep 'pinfunction'
grep -A 1 'pad "[1-8]" thru_hole' hardware/sa818-u-carrier/sa818-u-carrier.kicad_pcb | grep 'pinfunction'
```

---

## Why This Needs Doing

The main badge J7 and J8 schematic pin assignments deviate from the originally documented intent and from the carriers' current pin assignments. Probable cause: at some point during main badge layout, the connectors got physically rotated 180° on the PCB for routing/mechanical reasons, and during that rework the signals were reassigned to different schematic pins to keep traces short. The carriers were never updated to follow.

Detected on 2026-05-01 while spot-checking a wiring concern (RP2350B GPIO34 to TPS22919 ON pin to J8 pin 4). The trace through the main badge looked fine, but when the carrier-side pinouts were checked, every signal except VCC/GND/ID landed on the wrong pin.

ERC and DRC don't catch this because each KiCad project (main badge, VHF carrier, UHF carrier) has its own netlist — no cross-project validation exists.

**Lesson learned:** for projects with multi-board mating, define the connector pinout in **one** place and treat all boards' connectors as derived. Consider adding a "connectors-of-truth" doc that's updated *first*, before any schematic, when the pinout changes.

---

## Status Tracking

- [ ] VHF carrier — schematic edits (J14, J15)
- [ ] VHF carrier — PCB re-route
- [ ] VHF carrier — DRC clean
- [ ] UHF carrier — schematic edits (J18, J20)
- [ ] UHF carrier — PCB re-route
- [ ] UHF carrier — DRC clean
- [ ] Cross-project net validation pass
- [ ] CLAUDE.md updated to reflect locked pinout
- [ ] routing-guide.md updated
- [ ] electrical-design.md updated (if it has J7/J8 pinout)

---

*Last updated: 2026-05-01*
