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

Adding XCLK to the badge (J4 pin 18 is unconnected, GPIO46/47 are spare) was left as an open discussion item. It would remove the dependency on one kind of module.

**Closed 2026-11-18:** no clock line will be added. A line that firmware could either drive or leave floating would serve both kinds of module, but only if a clock-less module has its clock input on J4 pin 18, and J4 was not laid out against such a module. The documented requirement stands: the camera module must have its own oscillator.

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
- No gate pull-up was added. During reset the RP2350's default pull-down holds the gate low, so the backlight is on until firmware takes over. A pull-up to make it default off would cost one resistor; this was left as is. **Changed 2026-11-20: pull-up added, see section 15.**
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
| Radio supply drop | Charger battery switch is 200 mΩ typical, load switch 90 mΩ, and the TPS22919 is rated 1.5 A. The real SA818 transmit current decides whether this matters. Closed with no hardware change; see section 14. |
| Fuel gauge supply | U7 was on VSYS, which sits near 5 V with USB connected, so it read the wrong voltage then. Fixed; see section 10. |
| Fuel gauge thermistor | Was shared with the charger. Now disconnected; the gauge uses I2C temperature mode. See section 10. |
| Crystal series resistor | The RP2350 guide uses 1 kΩ; the board had none. Added as R29; see section 11. |
| Band ID | The UHF pull-up goes to the switched radio supply, so it reads as VHF when the radio is off. Settled in firmware, no hardware change; see section 13. |
| SD card pull-ups | DAT1, DAT2, MISO and CS had none. CS and MISO added afterwards; see section 8. |
| VBUS capacitance | About 25 µF, above the 10 µF USB inrush limit. Reduced to about 7.9 µF afterwards; see section 8. |
| Camera XCLK | Closed: the module must have its own oscillator. See section 2. |
| Parts not verifiable on paper | Camera module pinout against J4; battery connector polarity; APFA3010 pad numbering (moot once D1 was changed; see section 9). |

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

## 7. Battery connector changed to surface-mount (2026-11-14)

### The concern

J1, the battery connector, was a through-hole vertical JST-PH on the back of the board at about (25.6, 78.6) mm — inside the display area. Its two pins, battery positive and ground, came through to the front 2 mm apart, directly behind the display module. If the back of the module is conductive there (an SD card shell, header pins, exposed copper) and touches both, the battery is shorted with nothing but its own protection circuit in the way.

How likely that is depends on things the design does not control:

- The display plugs into J3 along one edge only, so the module is cantilevered and its free end can be pressed toward the board.
- The display module has not been selected, so what is on its back is unknown.
- A socket normally holds the module several millimetres clear, but a builder could solder it lower.

It was the only through-hole part under the display other than J3 itself, and the board's vias are tented, so J1 was the whole of the exposure.

### Options considered

1. **Surface-mount JST-PH in the same place — chosen.** Nothing comes through to the front. Small board change.
2. **Move the through-hole connector outside the display area**, to the free strip on the back near the bottom edge. Roughly 30–40 mm of 0.8 mm battery trace to re-route past the charger and USB parts, a longer path for the radio's supply current, and the pins would then show on the front below the screen.
3. **Leave it and insulate** — Kapton behind the display, leads trimmed flush, a spacer under the free end. No board change, but it depends on every builder doing it, on a kit where the user solders the through-hole parts.

### What was built

- J1 is now a JST S2B-PH-SM4-TB: surface-mount, side-entry, same JST-PH plug. It is in the same area of the back, with the plug entering from the top-of-board side and lying flat against the board.
- Pin 1 is still battery positive, as before; the polarity check in `docs/prototype-checklist.md` still applies.
- The battery trace runs between the connector's mounting tabs and its signal pads to reach pin 1. The I2C clock trace that used to pass between the old connector's two pins was re-laid on the back layer to go around the new connector instead of under it: straight down from its via near C38, then one diagonal past the connector's lower-left corner to its via beside the fuel gauge (three segments, 24.4 mm, where the old path was six segments and 25.5 mm). It no longer runs alongside the battery trace.
- The two mounting tabs are soldered but not connected to any net.
- J1 moves from the user-soldered list to fab assembly.

### Trade-offs accepted

- A surface-mount connector resists cable pulls less well than a through-hole one. The two mounting tabs carry that load.
- Side-entry means the battery lead exits along the board toward the top. Leave slack so the plug is not levered sideways.

### Still open: support for the display's free end

A second socket near the display's right edge would stop the module being pressed down. There is room: a scan of all four layers shows a 1×4 socket fits without touching any copper at x ≈ 74.5–81.5 mm for a band of y between about 73 and 88 mm. The exact position has to come from the module — many 14-pin ILI9341 modules have a 4-pin SD header on the far edge that such a socket could mate with — so this waited on the module being chosen and measured. **Done 2026-11-18; see section 12.**

---

## 8. Smaller review items worked through (2026-11-14)

The list of smaller hardware items from the review was worked through in one pass. What was done, what was not, and why.

### Done

**USB input capacitance.** VBUS carried about 25 µF (10 µF at the connector, 10 µF and 4.7 µF at the charger). The USB limit for a device is 10 µF, because a larger value draws an inrush spike the host may reject. Values only were changed: C_USB1 10 µF → 1 µF and C10 10 µF → 2.2 µF, leaving C76 (4.7 µF) at the charger's input pins. Total is about 7.9 µF. The MCP73871 asks for at least 4.7 µF at its input, which C76 plus C10 still provides.

**SD card pull-ups.** Four 10 kΩ resistors to 3.3 V: R25 on SD_CS, R26 on MISO, R27 on DAT2 and R28 on DAT1. The chip-select pull-up is the one that matters most: with the RP2350 in reset its pins have a weak pull-down, which would otherwise select the card. DAT1 and DAT2 are unused in SPI mode; the card makers require them pulled up rather than left floating. SanDisk's OEM product manual says it is the host designer's responsibility to fit pull-ups on all data lines even if only DAT0 is used, because floating DAT1 and DAT2 inputs can cause unexpectedly high current draw; Kingston's datasheet likewise says the unused DAT lines are inputs at power-up. (Checked 2026-11-14 against those vendor documents. The SD Association's own specification text was not read.) They were first left off because the pads were boxed in (see below), and added once the socket was turned round.

**BOOTSEL resistor.** R19 slid along its existing trace toward the MCU. The unterminated stub hanging off the flash chip-select line went from about 36 mm to about 14 mm. Nothing nearer was free on that layer.

**1.1 V links under the MCU.** The back-layer links between the core-supply vias were 0.2 mm. They are now 0.4 mm where they clear their neighbours by 0.2 mm, 0.3 mm on the link toward pin 10, and unchanged at 0.2 mm on the link to pin 32, where nothing wider fits. The front-layer stubs into the 0.4 mm pitch pins stay at 0.2 mm; the pads are that width.

**Test pads.** Six 1.0 mm pads on the back: GND, +3V3, VBUS, VSYS, VLED and SA818_VCC, each beside the part that makes or uses that rail. Battery voltage and the 1.1 V rail were left without pads because they can be probed at J1 and C1, and adding pads would have meant renaming nets.

**Kinked LED data joints.** The data traces into D25 and D27 each went up, sideways and down to reach the pad. D25 now leaves the pad sideways and joins its diagonal directly; D27 has two fewer bends (a direct run would pass too close to a ground via).

### Not done, with reasons

**Third core-supply capacitor at U1 pin 32.** Tried again and still not possible without moving other traces. A 0402 does now fit on the back, about 1 mm from pin 32 beside the via that feeds it, and its supply pad connects. Its **ground** pad cannot be connected: the spot is fenced in on the back layer by the SWCLK, SWDIO, RUN and D-pad traces, and every position for a ground via within 3.5 mm is blocked on the front by the MCU's own pads and fan-out. The least-bad via position is blocked only by the crystal's XOUT trace, which was judged not worth disturbing. Pin 32's nearest capacitor therefore stays about 9.5 mm away along 0.2 mm traces; the other two core-supply pins each have a capacitor under the chip. If the prototype shows core-supply trouble, nudging XOUT about 0.3 mm to open that via is the fix to try.

**Camera lines with long routes.** Left alone on the owner's instruction.

**Reference labels for R23 and C83.** They stay hidden. The only free silkscreen space would need 0.7 mm text, and the board's minimum is 0.8 mm.

**Short joints on the inner-layer LED rail.** Ten sub-0.2 mm segments remain where tracks meet at junctions. They are on an inner layer, cause no rule violations, and removing them means moving junction points on a finished rail.

### Found along the way: the SD card socket faced inward

Placing the pull-ups showed that the socket's footprint draws the inserted card overhanging about 4 mm past the contact pads, and that those pads faced the middle of the board. The card would have been inserted from the board-interior side. `hardware/pcb-design.md` has always said "SD card slot accessible at edge", and the owner confirmed the card should sit flush with the edge, facing out.

J10 was turned 180° and moved 2.6 mm so the inserted card's outer end lands exactly on the board edge (x = 120 mm). The four SPI lines were re-laid on the back layer as parallel runs under the socket body, between its two mounting pads. Power and ground pins have their own vias in the strip between the pads and the edge, which is under the card, so only traces and tented vias are there. With the pads now facing open board, the DAT1 and DAT2 pull-ups could be added just above and below the socket.

### Battery location

The owner's intended battery position is on the back, to the right of the 3.3 V regulator's inductor. The parts that looked like they bounded it (the border LEDs and their capacitors) are on the front. On the back, the largest part-free rectangle there is about 41 × 67 mm (x 47.3–88.3, y 44.7–112.1 mm), bounded by the inductor and a test pad on the left, a D-pad switch's pins on the right, the Tag-Connect pads above and the slide switch below. No through-hole pins come out inside it and the vias are tented. That is enough for the 50 × 35 mm pouch in the BOM; the real cell still has to be checked against it.

---

## 9. Power LED: new part, new place, second job (2026-11-14)

### What changed

- **Part:** D1 went from the Kingbright APFA3010 (right-angle, shines sideways along the board) to the Kingbright AAA3528LSEEZGKQBKS (3.5 × 2.8 mm flat-top package, LED window in the middle, shines straight out of the front).
- **Place:** from the middle of the board below the MCU to just left of SAO connector J6.
- **Job:** it now shows transmit and receive as well as battery level. The firmware plan is in `firmware-architecture.md`.

### Why this part

The owner asked for a small rectangular flat-top SMD. The AAA3528LSEEZGKQBKS was chosen because its part-number suffix names the same three LED chips as the old part, and its datasheet gives the same forward voltages (red 1.8 V, green and blue 2.65 V typical at 2 mA). R14–R16 keep their values, and the schematic symbol's pin functions are unchanged: pin 1 red, pin 2 common anode, pin 3 green, pin 4 blue.

The pinout was read from the datasheet's package and tape drawings. KiCad's footprint for this Kingbright package numbers its pads in the same rotational order, so it was used as it is. This check mattered: the same mistake of trusting a library pairing is what miswired the border LEDs earlier in this review.

### Why beside J6 and not J5

Both SAO connectors were offered. The three LED GPIOs leave the MCU on the side nearer J6. A trial route to J6 needed about 115 mm of trace and six vias; any position near J5 needed over 200 mm and around twenty.

Each trace changes layer once, because the front layer between the resistors and the LED is crossed by two other traces. Two vias per trace is the minimum the board allows.

Older documents (`hardware/pcb-design.md`, `gpio-optimization.md`) planned this LED for the bottom edge beside USB and BOOTSEL. It was never routed there.

### "Wired backwards": can it be made simpler?

The LED is common-anode: the shared pin goes to 3.3 V and each colour lights when its GPIO is driven **low**. That inversion has been flagged several times as something for firmware to remember, so it was looked at for a hardware fix.

- A hardware fix would mean a common-cathode LED instead (shared pin to ground, GPIO high = on). No common-cathode part in this package with a verified pinout was identified, and the gain is small.
- The RP2350 can invert a pin's output in hardware with one register setting per pin (in the Pico SDK, `gpio_set_outover(pin, GPIO_OVERRIDE_INVERT)`). Set once at start-up, every later write is in normal logic: 1 = on, a PWM duty of 100% = full brightness.

Decision: keep the common-anode part and do the inversion once in firmware set-up. The same one-line setting applies to the backlight pin, which is also active-low. Both are written up in `firmware-notes.md` as a set-up step rather than a thing to remember everywhere.

---

## 10. Fuel gauge wiring corrected (2026-11-17)

### What was wrong

Three things, all checked against the LC709203F datasheet:

1. **Supply on the wrong net.** The gauge measures its own VDD pin; that *is* its battery voltage reading. The datasheet's pin table says to connect VDD to the battery's positive terminal. U7's VDD was on VSYS, the charger's output. On battery the two are close, but with USB plugged in VSYS sits near 5 V: the reading is wrong, and it is above the part's 4.5 V operating maximum (absolute maximum 6.5 V). `electrical-design.md` had always drawn VDD on the battery; the schematic did not match it.
2. **Thermistor pin tied to the charger's thermistor.** TSENSE was connected to the same NTC as the MCP73871's THERM pin, with the gauge's TSW drive pin left open. The two parts read a thermistor in incompatible ways: the charger pushes a constant current through it, while the gauge switches TSW on through a matching resistor and measures the divider. Shared like this, the gauge cannot read it and may disturb the charger's reading. The datasheet also says TSW and TSENSE "must be disconnected" when not in use.
3. **Alarm pin floating.** The datasheet says to tie ALARMB to VSS when it is not used.

### What was done

- **VDD moved to the battery net.** The net was given a name, VBAT, so it could be labelled at both ends in the schematic. On the board, a 0.2 mm sense line runs from the battery trunk between J1 and the charger, around the top of U7, to its VDD pin, on the back layer with no vias. The line carries microamps, so its width does not matter.
- **TSENSE disconnected** from the thermistor net; about 25 mm of track that carried it was removed. The gauge runs in I2C temperature mode. TH1 now serves the charger alone.
- **ALARMB grounded** to the gauge's own exposed pad.
- **Ground brought in through the exposed pad.** The gauge used to be grounded from a single via outside the package, through pin 1 and a pair of 0.8 mm traces into the exposed pad. It now has two ground vias in the exposed pad itself, and pins 1, 2 and 4 each run a short 0.25 mm stub straight into the pad. The datasheet marks the exposed pad "don't care", so this is tidiness and a shorter ground path, not a functional fix.
- **C84, 1 µF, added at VDD**, as in the datasheet's application circuit. The gauge's supply now arrives down a long thin line, so a local capacitor matters more than it did.
- In the schematic symbol, ALARMB's pin type was changed from open-collector to passive so that grounding it does not raise an ERC error against the ground net's power flag. This is the same workaround already used on the TPS63001 symbol, and adds one "symbol doesn't match library" warning.

### Why not give the gauge its own thermistor

It would take three more parts (a second NTC, a matching 10 kΩ resistor and a 100 Ω series resistor) and somewhere near the battery to put the sensor. Temperature mainly improves the gauge's accuracy at temperature extremes. This is a badge worn indoors; a fixed room-temperature value over I2C is good enough, and can be revisited if the state-of-charge readings prove poor.

### Side effects

- The gauge is now powered whenever a battery is connected, even with the badge off. It draws a few microamps.
- The battery net is called VBAT in the schematic and on the board. It was previously the unnamed net at J1 pin 1.

---

## 11. Crystal series resistor added (2026-11-18)

### What changed

R29, 1 kΩ 0402, now sits in series between the RP2350's XOUT pin and the crystal. The load capacitor C_Y2 stays on the crystal side of the resistor, as in the RP2350 hardware design guide.

### Why

The resistor limits how hard the chip drives the crystal. Small crystals have a maximum drive level, and over-driving one shortens its life or shifts its frequency. The guide includes this resistor; the board had left it out. Earlier documents hold no decision either way: the crystal section of `electrical-design.md` showed only the crystal and two capacitors, and the question was first raised in section 5 of this review.

### Value

1 kΩ is the guide's value, picked there for the guide's own crystal. The badge's crystal is specified only as a 12 MHz part (then labelled "3215"; see section 17) with 10 pF load (three candidates are listed in `electrical-design-decisions.md`), so 1 kΩ is a starting value. A prototype check for start-up and USB enumeration is in `docs/prototype-checklist.md`.

### Layout

There is no room on the direct run from pin to crystal: the SWDIO and RUN vias, a 3.3 V via and the XIN trace hem it in. R29 is placed just right of the crystal at (63.85, 44.2), standing vertically. The XOUT trace passes between R17 and the top of the crystal pad to R29's upper pad, and R29's lower pad joins the crystal pad directly. Everything is on the front layer with no vias; the XOUT run is about 1.5 mm longer than before.

One silkscreen consequence: the "Y1" label moved to the left of the crystal to make room. R29's label sits to the right of the resistor.

---

## 12. Display module chosen, support socket added (2026-11-18)

### What the documents said before

The display was "a 2.4 inch ILI9341 module", not a specific part. `project-status.md` listed sourcing a specific module as open, J3 was pinned for "the common 14-pin ILI9341 module", touch was ruled out (D-pad navigation), and the module's own SD slot was not to be used (J10 is the card socket). Section 7 left a support socket for the display's free end waiting on a module.

### Module: QDtech / LCDwiki MSP2401

Chosen because it is the board the earlier documents were already describing, it has a published dimension drawing, and it has a second row of holes on the far edge to hang a support from.

- MSP2401 is the version without a touch panel; MSP2402 is the same board with one. Either fits. The no-touch one is specified because the badge does not use touch.
- Pin order and backlight polarity in the manual match J3 and Q1 as built, so nothing else on the badge changes.
- The far-edge holes are the module's SD header (SD_CS, SD_MOSI, SD_MISO, SD_SCK). The manual's photo shows them without pins, so the builder solders a header in. That suits the kit: the builder is already soldering through-hole parts.

### Where the socket goes

From the drawing: 14-pin row 2.00 mm from one short edge, SD row 2.17 mm from the other, board 77.18 mm long, so the rows are 73.01 mm apart. First SD pin is 17.23 mm from the long edge and pin 1 of the 14-pin row is 4.85 mm from the same edge, so 12.38 mm along. With J3 pin 1 at (8.636, 69.088) that puts the SD pins at x = 81.646 mm and y = 81.468, 84.008, 86.548 and 89.088 mm.

The earlier scan had found room for a 1×4 at x ≈ 74.5–81.5, y ≈ 73–88. The real position is at the edge of that: the first three pins are clear on all four layers, and the fourth sits on CAM_D3 (front) and USB D+ (back).

### Why three pins, not four

- A support needs to hold the end up, not carry signals. Three pins do that.
- Moving USB D+ would disturb the pair whose length mismatch was measured and accepted in section 6; CAM_D3 is one of the long camera routes that were deliberately left alone.
- The fourth module pin, if fitted, hangs beside the socket above the board and touches nothing.

J23's pads are not connected to anything, so the module's SD lines stay isolated from the badge.

### Other things this settled

- The silkscreen display outline was 77 × 43 mm but assumed the header row 3 mm from the board edge. It is now the module's real outline. The module clears the border LEDs on all four sides (closest is the right-hand column, about 0.8 mm from the LED bodies).
- With both ends of the module on sockets, the worry in section 7 about the free end being pressed onto the board no longer applies.

### Not checked

No module was in hand. The numbers come from the manufacturer's drawing (tolerance ±0.2 mm). Boards sold as "2.4 inch ILI9341 SPI" by other makers may differ; the prototype checklist has the fit check.

Sources: [MSP2402/MSP2401 user manual](https://www.lcdwiki.com/res/MSP2402/2.4inch_SPI_Module_MSP2402_User_Manual_EN.pdf), [MSP2401 dimension drawing](https://www.lcdwiki.com/res/MSP2402/MSP2401_Size_QD.pdf).

## 13. Band ID: solved in firmware, hardware unchanged (2026-11-20)

### What the documents said before

- Each carrier has one resistor on the ID pin: 0 Ω to ground (VHF) or 10 kΩ to the radio supply (UHF), read on GPIO39.
- `electrical-design.md` listed "no module" as reading low, the same as VHF, to be caught by a UART timeout.
- `carrier-rewiring-checklist.md` added the UHF pull-up and noted the pin is only valid once the radio supply is on.
- Section 5 of this review and the firmware notes both left "a scheme that separates no board / VHF / UHF" open, with a hardware change under discussion.

### Two problems, two causes

1. **Radio off looks like VHF.** The UHF pull-up is fed from the radio supply. That supply comes from the battery through the TPS22919 load switch, which GPIO34 turns on — the radio is not permanently powered. With the switch off there is nothing to pull the pin high.
2. **No carrier looks like VHF.** A grounded pin and a floating pin with a pull-down both read low. This has nothing to do with power.

### Options considered

- **A. Firmware only.** With the radio on, read the pin with the internal pull-down, then with the internal pull-up. VHF reads low/low, UHF high/high, an empty socket low/high. Fixes problem 2 completely. Problem 1 becomes a rule: power the radio before reading.
- **B. Move the ID line to GPIO46** (analogue-capable, spare) and make the UHF resistor about 100 kΩ to ground. Three voltage levels, readable with the radio off, room for more bands. Costs a re-routed trace on the main badge and a change to the UHF carrier.
- **C. Capacitor on the UHF carrier** in place of the pull-up; firmware times the discharge. Works with the radio off and leaves the main badge alone, but is the fiddliest to get right in code.

### Decision: A

The carrier is fitted but unpowered in three situations — before firmware starts the radio, in airplane mode, and in sleep — and in none of them does the badge need to know the band. It matters only when the radio is about to be used, and then it is powered. So the one thing A gives up costs nothing, and both boards stay as they are.

B remains the better design if a third band or detection without radio power is ever wanted.

### Consequences

- This is now required firmware. `firmware-notes.md` carries the method, a reference implementation and the rules (detect on every radio power-up; no radio activity before detection; cross-check with the serial handshake).
- The UHF pull-up can reach about 5 V with USB connected. Expected to be within what a digital-only RP2350 pin tolerates, but not confirmed from the datasheet's pin table; a prototype check was added.

## 14. Radio supply drop and load switch rating: no change (2026-11-20)

### What the documents said before

The SA818's transmit current was given as 1.7 A (or "1.5–1.7 A") in `CLAUDE.md`, `electrical-design.md`, `electrical-design-decisions.md`, the routing guide and the firmware battery-life table. None of them cites a source. On that number, this review flagged that the TPS22919 load switch is rated 1.5 A and that the path from battery to radio would lose about 0.5 V.

### What the datasheet says

NiceRF's SA818S specification: transmit current 650 mA typical / 750 mA maximum at high power (1 W), 450 / 550 mA at low power (0.5 W), receive 60 mA; supply 3.3 V minimum, 4.2 V typical, 5.5 V maximum. These were read from search-result excerpts of the specification and the manufacturer's product page, which agree; the PDF itself was not opened. No SA818 source giving 1.7 A was found. NiceRF's higher-power SA818Pro is listed at 1.0 A maximum.

### What follows

- **Load switch:** 0.75 A against a 1.5 A rating. No change.
- **Drop:** 200 mΩ (charger battery switch) + 90 mΩ (load switch) at 0.75 A is about 0.22 V. With the cell at about 3.5 V under load the radio is at its 3.3 V minimum, so transmit sags or stops near the end of the battery. Accepted: it costs the last few percent of charge. A firmware low-battery transmit lockout is the tidy way to handle it, with the threshold taken from a prototype measurement.
- **On USB:** the radio rail sits near 5 V, inside the 5.5 V maximum.
- The 2.0 mm SA818_VCC path added in section 3 stays; it is more than needed and costs nothing.

### Decision

No hardware change. The documents were corrected to the datasheet figures, including the transmit battery life (about 2 hours continuous rather than 1.2). The original reason for taking the radio off the 3.3 V rail still holds: with the rest of the badge, the load would still have exceeded the old 1 A regulator.

### Still to do

SA818 modules vary between batches and sellers. The prototype checklist keeps the transmit-current measurement and adds a low-battery transmit test.

Sources: [NiceRF SA818S specification](https://atta.szlcsc.com/upload/public/pdf/source/20250909/77A4136874318C9FB929E64DC658FE59.pdf), [NiceRF SA818S product page](https://nicerf.com/walkie-talkie-module/1w-embedded-walkie-talkie-module-sa818s.html).

## 15. Backlight now off by default (2026-11-20)

### What the documents said before

Section 4 recorded the missing gate pull-up as a deliberate leave-alone, and `electrical-design.md` and the prototype checklist both described "backlight on during reset" as expected.

### Why change it

With no pull-up, the backlight is fully on whenever firmware is not driving GPIO29:

- at power-up and reset, before the display is initialised, so the screen flashes white;
- for as long as the badge sits in BOOTSEL mode, at 50–80 mA;
- after a crash early in start-up.

Sleep was never affected, since firmware sets the pin first. The only thing in favour of leaving it was that a lit screen is a crude sign of life. A clean start on something worn and shown off was judged worth one resistor.

### What was added

R30, 4.7 kΩ 0402, from Q1's gate to 3.3 V. It was first placed between R11 and Q1 with its own via; both parts were re-arranged the next day (section 20).

### Why 4.7 kΩ and not 10 kΩ

10 kΩ was the first choice. Against the RP2350's weak pull-down (taken as 50–80 kΩ, from memory of the datasheet) plus R11, a 10 kΩ pull-up leaves the gate about 0.54 V below the source in the worst case. The AO3401A's threshold is 0.5 V minimum, so that has no margin. 4.7 kΩ leaves about 0.28 V. The cost is about 0.6 mA while the backlight is on, against a backlight that draws 50 mA or more.

### Also corrected

The BOM described R11 as a gate pull-down. It is a 1 kΩ resistor in series with the gate.

## 16. SD card: keep the on-board socket, do not use the display's (2026-11-20)

### The question

The chosen display module has its own SD card slot, and three of that slot's header pins are already plugged into the badge to support the display (J23, section 12). Could the badge's own socket, J10, be deleted and the display's slot used instead?

### What the documents said before

A standalone socket is recorded as the design in `CLAUDE.md`, `electrical-design.md`, `project-status.md` and the BOM, and it was the owner's plan from early in the project. None of them gives a reason. This section supplies one.

### Decision: keep J10, leave the module's SD pins unconnected

Reasons, in rough order of weight:

- **Access.** The module's slot is on its back, in the gap between the display and the badge. Read from the module's dimension drawing, it opens toward the display's top edge, so the card would go in from above, behind the top row of border LEDs and toward the carrier. Exact clearances were not measured. J10 puts the card flush at the board edge, facing out, which was a specific request (section 8).
- **Card size.** The manual's photo shows a full-size SD slot on the module. J10 takes microSD.
- **Speed.** The module's SD lines pass through series resistors (R1–R3 in the manual's photo). On boards of this style they are believed to be about 1 kΩ, which slows the bus; this was not measured or confirmed from a schematic.
- **The fourth pin.** The slot needs four signals. J23 has three pins because the fourth position lands on CAM_D3 and USB D+ (section 12). Both traces would have to move, and four signals would need routing across to the display's far edge.
- **Module lock-in.** Any 14-pin ILI9341 module with the right pin order works today, with or without an SD slot. Using the module's slot would tie the badge to modules that have one, wired the same way.

What deleting J10 would have saved: one fab-placed socket, four pull-up resistors (R25–R28) and a little board area.

Wiring the module's slot as a second, optional card slot was also considered. It carries the same fourth-pin and routing cost for little gain, so it was not done.

## 17. Crystal footprint corrected (2026-11-20)

### What was wrong

Y1's footprint was a 3.2 × 1.5 mm two-pad pattern. The documents called the crystal "3215" throughout, and the footprint matched that label. But the 12 MHz crystals the design intends to use (the Abracon ABM8 family, as in Raspberry Pi's reference design) are 3.2 × 2.5 mm with four pads. A 3.2 × 1.5 mm two-pad package is what 32 kHz watch crystals come in; no 12 MHz part in that size was found. The board as drawn could not have been populated with a working crystal.

It was found by the pre-fab footprint check, comparing the board's pads with the ABM8 datasheet. `project-status.md` had carried an unticked "verify crystal footprint" item since the MCU sheet was drawn.

### What changed

- **Schematic:** Y1 is now the four-pin crystal symbol. Pins 1 and 3 are the crystal, pins 2 and 4 are the case, tied to ground.
- **Footprint:** KiCad's 3225 four-pad pattern, checked against Abracon's ABM8 drawing (pads 1 and 3 crystal, 2 and 4 ground, diagonal).
- **Layout:** the crystal stands upright at (61.3, 45.5), so that its two crystal pads are top-left and bottom-right. XIN drops straight onto the top-left pad; the bottom-right pad goes to R29 and C_Y2, which now sit in a column to the crystal's right. C_Y1 stands to the crystal's left. The two ground pads are joined by a short trace under the crystal and tied to the existing ground via on the left; C_Y2 has its own ground via. Everything is on the front layer.
- The XOUT trace from the chip to R29 (section 11) did not move.

### Crystal part

Settled the same day: Abracon ABM8-272-T3. Its datasheet gives 12 MHz, 10 pF load, ±30 ppm. Raspberry Pi's "Hardware design with RP2350" uses this part with 15 pF load capacitors and a 1 kΩ series resistor, which is exactly what is on the board, so section 11's caution about the resistor value being "a starting value" no longer applies as long as this crystal is fitted. One of the three parts listed before, ABM8-12.000MHZ-B2-T, is given as an 18 pF part by a distributor and would not suit.

## 18. SD socket shell grounded (2026-11-20)

The pre-fab footprint check compared J10 with Molex's sales drawing. The pads were right, but the pairing of symbol and footprint was not: KiCad's generic microSD symbol calls pin 9 "shield", while on the Molex 104031-0811 footprint pad 9 is one of the two card-detect contacts and the metal shell is pad 11. The shell's four pads were therefore soldered to nothing, and a card-detect contact was grounded instead.

Nothing would have stopped working, but the shell sits at the board edge where fingers touch it on every card change, and a floating shell gives a static discharge no path to ground.

Fix: the symbol embedded in the connectors sheet gained a pin 11 ("SHIELD"), stacked on pin 9 and tied to ground, and each of the four shell pads got a ground via beside it. This adds one "symbol does not match library" warning to ERC, which is expected.

Molex's own downloadable symbol and footprint were looked at as an alternative. They would not have helped: Molex's symbol has pins 1–10 only, so it leaves the shell pads (numbered 11–14 there) unconnected too, and its footprint is the same pad pattern to within 0.05 mm.

## 19. Charger exposed pad given vias (2026-11-20)

During the owner's visual pass, a question about U4 pin 3 being tied to the chip's centre pad led to checking how that pad reaches ground. Pin 3 (input select) is correctly grounded, which selects the 500 mA USB input limit. But the centre pad (pad 21) had no vias under it: the pad and pins 3, 10 and 11 all reached the ground plane through one via outside the package, along 0.3–0.4 mm traces. Every other chip with an exposed pad had vias (U1 six, U5 and U11 four each, U7 two). The November review widened the charger's supply traces and added vias under U1 but did not look under U4.

The charger is linear, so it turns the difference between USB voltage and battery voltage into heat: about 0.65 W at 500 mA into a 3.7 V battery and about 1 W into a flat one. The exposed pad is the heat path. Without vias the chip would have protected itself by cutting charge current, so charging would have been slower than designed, and it would have run hot beside the battery connector.

Five 0.3 mm vias were added in the pad, four at the corners of a 1.6 mm square and one in the centre, down to the ground plane. Microchip's land pattern shows nine on a 1.0 mm pitch; five keeps them clear of the edge of the pad. Charge-time and temperature are already on the prototype checklist.

## 20. Backlight switch re-arranged around the LED trunk (2026-11-21)

### What the owner saw

On the visual pass: Q1's 3.3 V pad had two vias joined by a very crooked trace on the back.

### Why it was like that

The LED rail's main trunk runs down the left edge on the inner power layer at x = 4.1 mm. It cuts the strip of 3.3 V plane between itself and the board edge off from the rest of the plane. Q1 sat in that strip, so a via under it reached only the cut-off sliver. When the LED rail went in, this was patched with a hop: a via to the back, a short back-layer trace under the trunk, and a second via up into the main plane. The hop was auto-routed on a 0.1 mm grid and left as a 12-segment staircase. R30, added later, got its own via in the same sliver and so depended on that hop as well.

### What changed

Anything needing 3.3 V here has to put its via east of about x = 5.0 mm. So:

- Q1 moved to (4.8, 64.0). Its source pad runs to a single via at (5.3, 65.4), east of the trunk, straight into the main plane.
- R30 moved to (2.1, 63.56), in line with the gate trace: the trace runs straight down from R11 into R30's top pad and across to Q1's gate, and R30's bottom pad joins Q1's source pad. R30 has no via of its own.
- Q1's drain pad now sits on the existing vertical trace to the display's LED pin, which was simply extended up into it.
- R11 did not move.

Three vias and a back-layer hop became one via. Nothing depends on the cut-off sliver any more.

### Rule

Parts that need the 3.3 V plane must not be placed between the left-edge LED trunk and the board edge unless they can reach a via east of x = 5 mm on the front layer. Auto-routed traces are to be replaced with hand-laid segments before the work is called done.

---

## 21. Visual review finished; all three boards locked (2026-11-21)

The owner finished the visual pass of the main badge and both carriers and locked all three for the prototype run. The boards are not to be changed again unless the owner reopens them.

### What the pass changed, beyond sections 17 to 20

**Main badge**

- **Trace ends.** Several traces stopped at the edge of a pad instead of running into it (U8 pins 13 to 15, U2 pins 1, 2, 3 and 7, pin 5 and pin 10 of both regulators, C21). Each got a short stub to the pad centre.
- **Redundant ground vias** removed at C38 and C53.
- **LED supply tracks** re-laid on the front and on the inner power layer: one via per LED along the top and bottom rows, both side columns and both ears. Details are in `led-supply-rail.md`.
- **LED regulator feed** straightened into hand-laid 45° segments.
- **Owner's moves kept:** test pads TP2, TP3 and TP5, and R24's ground via, which came out from under the display header.

**Carriers**

- **Supply trace into the radio module.** It was 1.0 mm narrowing to 0.6 mm at the two decoupling capacitors. It is now 1.0 mm to the 10 µF capacitor and 0.8 mm from there through the 100 nF capacitor to module pin 8, on both boards. A full 1.0 mm does not fit on the pad centreline: it leaves 0.18 mm to the 100 nF capacitor's ground pad against a 0.20 mm rule. 0.8 mm leaves 0.28 mm.

### Final checks on the locked files

- Main badge: 0 unconnected, 0 schematic-parity issues, 2 excluded silkscreen-edge items. ERC: 5 library-mismatch warnings, all from deliberately edited symbols.
- Each carrier: 0 unconnected, 0 schematic-parity issues, 5 excluded copper-edge items.

### What is left before ordering

Fab files and their inspection, the assembler questions, and the radio module pin-direction meter test. All are in `prototype-checklist.md`.

---

*Written 2026-11-14; section 10 added 2026-11-17; sections 11 and 12 added 2026-11-18; sections 13 to 19 added 2026-11-20; sections 20 and 21 added 2026-11-21.*
