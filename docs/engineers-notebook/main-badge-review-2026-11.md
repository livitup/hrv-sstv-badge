# Main Badge and Carrier Review — November 2026

**Dates:** 2026-11-13 and 2026-11-14
**Scope:** both SA818 carriers, the routed main badge PCB, and a sheet-by-sheet check of the main badge schematic against the part datasheets.

This entry records the decisions made while bringing the design from "routed and DRC-clean" to "reviewed". `docs/project-status.md` lists *what* changed; this file records *why*.

---

## 1. Carrier boards

### Harmonic filter layout: straight line, not folded

The first layout plan folded the 7-pole filter to fit between the SA818 antenna pad and the original SMA position. Because nothing on the carrier is fixed except the four headers that mate with the main badge, the SA818 and the SMA were moved instead, and the filter runs in a straight line on the front layer.

- The SMA was kept clear of the rabbit ears, so the antenna does not sit in front of them. That is why the SA818 moved as well.
- RF trace width is 0.8 mm, not the 1.2 mm first drawn: at 1.2 mm the trace came too close to the ground pads of the shunt capacitors.
- Two ground vias per shunt capacitor, unbroken back-layer ground under the filter, via fence on both sides.

### Headers on the back of the carrier

J14–J17 had their long pins on the same side as the SA818, which is the face that points outward. They were moved to the back so the carrier plugs into the main badge sockets. Hole positions are main-badge socket positions + (80, 50) mm.

### UHF carrier generated from the VHF layout

The UHF carrier had a mirrored-header bug from a group flip. Rather than repair it by hand, the UHF PCB was regenerated from the finished VHF layout so the two boards are mechanically identical.

### UHF filter values reduced from the ideal

See the addendum in `harmonic-filter-design.md`. The ideal fc = 470 MHz values were reduced (6.8 pF / 22 nH / 12 pF / 24 nH) after modelling pad and trace parasitics, which matter far more at UHF than at VHF. These are starting values for prototype tuning.

### Through-hole pins do not need vias

A through-hole pad connects to every copper layer it passes through, so a ground pin on a header reaches the ground plane on its own. Vias beside through-hole pins were removed on the carriers and on the main badge (J5, J6 and others). Surface-mount pads still need a via.

---

## 2. Camera clock: documented as a hard requirement, not fixed

The main badge has no XCLK net and J4 has no XCLK pin. Rather than reopen the routed board at that point, the requirement was documented loudly: **the OV2640 module must have its own oscillator** (callout in `electrical-design.md`, notes in both BOMs and `CLAUDE.md`).

Adding XCLK to the badge (J4 pin 18 is unconnected, GPIO46/47 are spare) is still an open discussion item. It would remove the dependency on one kind of module.

---

## 3. Main badge layout review

### Errors that would have stopped the board powering up

- The charger (U4, MCP73871) OUT pin fed only its own output capacitor. The 3.3 V regulator, load switch and fuel gauge had no supply. A power flag on the net had hidden this from ERC; it was removed.
- The charger IN pins were tied to ground instead of VBUS.

### Radio supply path

The SA818 draws its transmit current through the charger's battery switch and the load switch. The battery, VSYS and SA818_VCC traces were widened, and SA818_VCC was given a 2.0 mm path on the inner power layer with three vias at the load switch end. The inner layer was used because there was no room for that width on the outer layers.

### Additions taken from Raspberry Pi's "Hardware design with RP2350"

- 27 Ω series resistors on USB D+ and D−, close to the chip.
- 1 kΩ in series with BOOTSEL.
- 100 nF on every power pin that had room; six ground vias in the exposed pad.

### RP2350 core regulator: copy the reference layout

Three ways forward were discussed; the one chosen was to copy Raspberry Pi's RP2350B minimal reference layout as closely as a 4-layer board allows. The design guide is explicit that this regulator is layout-sensitive and that the reference layout should be reproduced rather than re-derived.

- Inductor changed to the polarity-marked Abracon AOTA-B201610S3R3-101-T, with the dot at the 1.1 V side.
- 4.7 µF input and output capacitors, 33 Ω + 4.7 µF filter on VREG_AVDD.
- Five back-layer signal tracks that crossed under the regulator were rerouted around it, so the back layer under the regulator carries only the 1.1 V trunk, as in the reference.
- A custom design rule allows the reference's 0.12 mm gaps for the three regulator nets only.

Remaining differences from the reference: copper is drawn as tracks rather than small polygons, and the board is 4-layer with a ground plane directly under the regulator.

### Layout hygiene rules adopted

A function-and-DRC check is not a full review. The board was also brought to these rules:

- Every signal net is one width (0.2 mm).
- Power traces are one width per branch, with a neck only at fine-pitch pins.
- Parts with the same role get the same trace treatment.
- Collinear segments are merged into single traces; short odd-angle segments and dog-legs are straightened.
- No redundant ground daisy-chain links where each pad already has its own via.

The camera lines that still take long routes with several vias were deliberately left alone: fixing them means ripping up and re-laying the whole group.

### Fiducials

Eight 1 mm fiducials (four per side) were added because both faces carry surface-mount parts and the RP2350 is a 0.4 mm pitch QFN. The pattern is slightly asymmetric so a rotated board is detectable.

---

## 4. Schematic review against the datasheets (2026-11-14)

### Border LEDs used the wrong symbol

D12–D27 are WS2812B-2020 parts on the 2020 footprint, but used the 5050 `WS2812B` symbol. The two packages number their pins differently:

| Pad | 5050 WS2812B | WS2812B-2020 |
|---|---|---|
| 1 | VDD | DOUT |
| 2 | DOUT | GND |
| 3 | GND | DIN |
| 4 | DIN | VDD |

All 16 were miswired. The fix was to switch to KiCad's `WS2812B-2020` symbol and re-route the 16 LEDs. No rotation of the footprint reproduces the old pad-to-net mapping, so a re-route was unavoidable. Each LED's power pad and ground pad now has its own via to the plane, except D23's ground, which ties to a neighbouring ground pad. Next to D18 the back-layer `CAM_HREF` trace was given a 0.75 mm jog to make room for a via.

**Lesson:** a symbol and footprint that are each valid library parts can still be a wrong pair. Check pad numbering against the datasheet whenever a part is used in a second package.

### TX attenuator was not a divider

R6 (470 Ω) was in series with the signal instead of going to ground, so the DAC output reached the SA818 microphone input almost unattenuated and low-pass filtered at about 340 Hz. R6 now goes from the R5/C24 node to ground, giving the documented 100:1 divider.

### Backlight switch: high side, P-channel

The original circuit used a 2N7002 to pull the display module's LED pin to ground, on the assumption that the pin is the backlight cathode. On the 14-pin ILI9341 module that J3 is pinned for, the LED pin must be driven **high**.

- An N-channel FET cannot switch the high side from a 3.3 V gate, so Q1 became an AO3401A P-channel FET in the same SOT-23 footprint: source to 3.3 V, drain to the LED pin.
- PWM is inverted: GPIO low = backlight on.
- No gate pull-up was added. During reset the RP2350's default pull-down holds the gate low, so the backlight is on until firmware takes over. A pull-up to make it default off would cost one resistor; this was left as is.
- The pin was not simply tied to 3.3 V, because that would remove dimming and any backlight timeout. The docs contain no decision that the display is always on; `firmware-architecture.md` defines an idle state with the display off.

### TPS63001 inductor

L1 changed from 1 µH to 2.2 µH, the value in the TPS63001 datasheet's application circuit. Same footprint.

### Checked and found correct

Flash, USB (CC resistors, ESD, series resistors), SAO, Tag-Connect, SD card SPI mapping, I2C pull-ups, RX audio chain, PCM5102A straps and capacitors, charger settings (500 mA input limit, 500 mA charge, 67 mA termination), TPS63001 pin wiring, MCU pin functions, and the ten 5050 ear LEDs.

---

## 5. Open questions at the end of this review

| Question | What is known |
|---|---|
| WS2812B supply voltage | The 2020 datasheet specifies 3.7–5.3 V; the LEDs ran from 3.3 V. **Resolved after this review** by a dedicated 4.0 V LED rail — see `led-supply-rail.md`. |
| Radio supply drop | Charger battery switch is 200 mΩ typical, load switch 90 mΩ, and the TPS22919 is rated 1.5 A. The real SA818 transmit current decides whether this matters. |
| Fuel gauge supply | U7 is on VSYS, which sits near 5 V with USB connected, so it reads the wrong voltage then. |
| Fuel gauge thermistor | Shared with the charger; the gauge's own drive pin is unconnected. Firmware should use I2C temperature mode. |
| Crystal series resistor | The RP2350 guide uses 1 kΩ; the board has none. |
| Band ID | The UHF pull-up goes to the switched radio supply, so it reads as VHF when the radio is off. A scheme that separates no board / VHF / UHF is still to be discussed. |
| SD card pull-ups | DAT1, DAT2, MISO and CS have none. |
| VBUS capacitance | About 25 µF, above the 10 µF USB inrush limit. |
| Camera XCLK | See section 2. |
| Parts not verifiable on paper | Camera module pinout against J4; battery connector polarity; APFA3010 pad numbering. |

---

## 6. Accepted deviations

### USB D+ and D− are not length-matched (accepted 2026-11-14)

`hardware/routing-guide.md` asks for D+ and D− within ±2 mm and routed as a pair. The board as routed has neither:

| Line | Length, connector to MCU pin | Vias |
|---|---|---|
| D+ | about 215 mm | 5 |
| D− | about 170 mm | 8 |

**Why this was accepted rather than fixed**

- The RP2350's USB is Full Speed (12 Mbps). One bit lasts about 83 ns; 45 mm of mismatch is roughly 0.3 ns of skew, a fraction of a percent of a bit. Tight matching is a High Speed (480 Mbps) requirement, and this chip does not do High Speed. The routing guide itself says impedance control is "not critical at Full Speed".
- Adding a 45 mm meander to D− would satisfy the ±2 mm rule on paper by making the shorter line longer, on a crowded board, for no electrical gain.
- The change with real value would be re-routing both lines together as a side-by-side pair, which rejects noise better. That means clearing a corridor of roughly 90 mm from the USB connector to the MCU through finished routing — a large, risky job. Earlier single-trace attempts only brought D+ from 209 mm to 176 mm.

**The risk that remains**

The two lines run separately, so they do not reject common-mode noise as well as a pair would. The situation to watch is USB traffic while the 1 W radio is transmitting.

**What would reopen this**

USB enumeration failures or data errors on the first prototype, particularly during transmit. The response then is the pair re-route, not a meander.

---

*Written 2026-11-14.*
