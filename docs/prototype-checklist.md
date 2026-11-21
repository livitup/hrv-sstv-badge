# Pre-Fab, Parts-in-Hand and Prototype Checklist

Everything that stands between the finished design files and a proven board, in the order it has to happen: desk checks and fab outputs (Part 0), checks that need the physical parts (Part 1), and measurements on the first assembled prototypes (Part 2). The items come out of the November 2026 design review (`engineers-notebook/main-badge-review-2026-11.md`) and the LED supply rail work (`engineers-notebook/led-supply-rail.md`).

This list does **not** replace the subsystem test procedures in `electrical-design.md` ("Testing Notes"). It covers the specific assumptions and open risks the review left behind. Where a test already exists there, this list points to it.

Tick items off here as they are done, and record what was found.

---

## Part 0 — Before ordering boards, no parts needed

Checks that can be done at a desk with the design files and datasheets, and the steps that produce the fab order.

### Desk checks

- [x] **5050 ear LED rated current.** Done 2026-11-20. The WS2812B V5 and WS2812B-V6 datasheets give 12 mA per colour as the working current (the condition for the brightness figures); neither states a separate maximum. That is 36 mA per LED at full white and about 0.94 A for all 26, not the 60 mA / 1.5 A assumed earlier. The WS2812B-2020-V6 datasheet gives the same 12 mA per colour (36 mA white) for the border LEDs. For the V6 parts now specified, both packages have a data-high threshold of 0.55 × supply and a supply range of 3.3–5.3 V.
- [ ] **Critical footprints against datasheets.** Compare pad positions, sizes and pin 1 with the manufacturer's land pattern for: U1 RP2350B (QFN-80), U4 MCP73871 (QFN-20), U5/U11 TPS63001/TPS63000 (VSON-10), U6 TPS22919 (SC-70-6), U7 LC709203F (WDFN-8), U8 PCM5102A (TSSOP-20), U2 flash (SOIC-8), Y1 crystal (3215), J2 USB-C, J10 microSD, J1 JST PH, D1 and the two WS2812B packages, and the SA818 on the carriers.
  - **Progress, 2026-11-20** (board pads compared with the manufacturer's drawing unless noted):
    - **Y1 crystal — PROBLEM.** The footprint is a 3.2 × 1.5 mm two-pad pattern. The three 12 MHz crystals recommended in `electrical-design-decisions.md` are 3.2 × 2.5 mm four-pad parts, and no 12 MHz crystal in a 3.2 × 1.5 mm package was found. The footprint must change to match a real part before fab.
    - **U1 RP2350B — OK.** Pitch 0.4, pad 0.2 × 0.8, exposed pad 3.4: matches the datasheet's recommended footprint except that the pad row sits about 0.05 mm further out. The pads still cover the leads fully.
    - **U4 MCP73871 — OK.** Pitch 0.5, pads 0.25 × 0.825 centred 1.94 mm out, exposed pad 2.5; within Microchip's land pattern (0.30 × 0.80 max, pad spacing 4.00, centre pad 2.8 max).
    - **U5 / U11 TPS6300x — OK.** Exact match to TI's DRC0010J land pattern, including the exposed-pad via positions.
    - **U6 TPS22919 — OK.** Pitch and outer pad edge match TI's DCK0006A pattern; the pads run about 0.3 mm further in under the body than TI draws them, which is harmless.
    - **U8 PCM5102A — OK.** Matches TI's PW0020A pattern to within 0.05 mm.
    - **D1 power LED — OK.** Exact match to Kingbright's soldering pattern, and pad numbering agrees with the datasheet (1 red, 2 anode, 3 green, 4 blue).
    - **WS2812B-V6 and WS2812B-2020-V6 — OK.** Pad spacing and pin functions match both datasheets' recommended pads.
    - **J2 USB-C — OK.** Pad positions, widths, shell stakes and locating pegs match GCT's USB4105 drawing.
    - **J1 JST PH — OK.** Pad sizes match JST's layout for the side-entry SMT header; polarity is covered under Part 1.
    - **Radio module footprint on the carriers — OK.** NiceRF's drawings for the SA818S and the SA868 are identical: 35.6 × 19.0 mm, pads on a 4.45 mm pitch along the long edges and 4.6 mm on the end, with the same 18-pin assignment. The carrier footprint matches. The "2 mm pitch" in `CLAUDE.md` and the BOM is wrong.
    - **Not yet checked** (datasheet could not be opened or was not reached): U7 LC709203F, U2 flash, J10 microSD.
- [ ] **Custom footprints.** Check the three footprints made for this project against their datasheets: `L_Abracon_AOTA-B201610` (including the polarity dot at pad 1), `C_0402_1005Metric_Wide`, and `L_0603HP` on the carriers.

### Reviews in KiCad

- [ ] **Main badge visual pass.** Look over the areas reworked in the November 2026 review: the power section on the back (U4, U5, U6), under and around U1, the LED rail regulator (about x 10–22, y 89–104 mm), the battery connector J1, the SD socket corner, the power LED beside J6, the border LEDs (D17/D18 are the tightest), the `CAM_HREF` jog near D18, and the inner power layer with the radio supply track and the VLED tracks.
- [ ] **UHF carrier visual pass.** The UHF carrier PCB was generated from the VHF layout; look it over as a board in its own right.
- [x] **ERC and DRC.** Re-run both on all three boards after the last change and confirm: no unconnected items, no schematic-parity issues, and only the known excluded items. Done 2026-11-20 on the committed files plus the uncommitted doc edits: main badge 0 unconnected, 0 parity, 2 excluded silk-edge items, ERC 4 library-mismatch warnings; each carrier 0 unconnected, 0 parity, 5 excluded copper-edge items, ERC 1 excluded library warning. Re-run again if any board changes (the crystal footprint will).

### Fab outputs

- [ ] **Generate fab files** for the main badge and both carriers: Gerbers, drill files, pick-and-place positions and BOM.
- [ ] **Inspect them** in a Gerber viewer: layer order, board outline including the ears and feet, drill hits, solder mask openings on the fiducials and test pads, and paste on the fine-pitch parts.
- [ ] **Fiducials — verify with the specific fab.** The main badge has eight 1 mm fiducials, four per side (FID1–FID8). Ask the chosen assembler what they need: how many per side, minimum size and mask opening, keep-out from the board edge, and whether they add their own on panel rails. Remove, move or add fiducials to match before ordering. They were kept as they are pending this check.
- [ ] **Other assembler requirements:** minimum silkscreen text height (two labels are hidden for this reason), via tenting, and whether the 4-layer stackup entered in KiCad matches what will be ordered.
- [ ] **Order a small prototype run** before any quantity order. The carriers can go first.

## Part 1 — With parts in hand, before ordering boards

These check assumptions that are built into the PCB. If one turns out wrong, the board has to change, so do them **before** the fab order if the parts can be bought first.

- [ ] **Camera module pinout.** The 2×9 header on the OV2640 module matches the J4 pin table in `electrical-design.md` exactly, pin for pin, including which row is odd and which is even. Candidate found 2026-11-20: the red-PCB "OV2640 Camera Module ... STM32 Drive Red Board" sold on Amazon (EC Buying store). Its silkscreen, read from the listing photo, matches J4 pin for pin and in the right handedness: lens-side view, row nearer the lens GND, SCL, SDA, D0, D2, D4, D6, DCLK, PWDN; row at the board edge 3.3, VSYNC, HREF, RST, D1, D3, D5, D7, NC. Confirm on the part received.
- [ ] **Camera module has its own clock.** With 3.3 V applied, RESET high and PWDN low, and no clock fed in, PCLK toggles on a scope. The badge supplies no XCLK, so a module that fails this will not work. For the red-board candidate (see above) this is the open question: the listing photos show no oscillator can on the front or the back. The front has two small black chips (one 5-pin, one 6-pin) and the lens holder could hide a part; one of these must be the clock source or the module cannot work here. Buy one and run this test before relying on it. If it fails, the fallback discussed was re-wiring J4 for the Adafruit OV5640 breakout, which has a selectable on-board 24 MHz oscillator.
- [ ] **LED revision.** The WS2812B reels are the V6 revision (WS2812B-V6 and WS2812B-2020-V6), not V5 or earlier. Check the reel label and, if in doubt, measure one dark LED's current at 4.0 V: V6 is 1 µA or less, older parts about 0.6 mA.
- [ ] **Display module backlight polarity.** On the MSP2401, the LED pin lights the backlight when driven **high** (3.3 V), and stays dark when left open or grounded. The manual says so; confirm on a real module. The backlight switch (Q1) assumes this.
- [ ] **Display module pin order.** The module's 14 pins match J3: VCC, GND, CS, RESET, DC, MOSI, SCK, LED, MISO, then the five touch pins. The manual says so; confirm against the silkscreen on the module actually bought, since look-alike boards exist.
- [ ] **Battery connector polarity.** With the battery's JST-PH plug inserted in J1, battery positive lands on J1 pin 1 (the net that goes to the charger's battery pins). JST-PH polarity is not standardised between battery vendors — check the actual battery with a meter. What J1 needs, worked out from the JST PH datasheet on 2026-11-20: hold the plug with the face that has the two small windows (metal visible) toward you and the wires pointing up; **red (+) must be on the left**. Equivalently, with the badge's back toward you and its top edge up, the plug goes in from above with its windows against the board, and red is on the right. The silkscreen above J1 carries "+" and "−" marks to check against. A battery wired the other way must not be plugged in: either re-pin its plug or choose a different battery. This is the same polarity SparkFun documents for its batteries (pin 1 positive) and that Adafruit and Particle are reported to use, so batteries sold as Adafruit- or SparkFun-compatible should fit; many generic packs are the reverse. (EEMB's LP103454 Amazon listing offers both: going by its photos, the option it calls "Reverse" matches J1 and its regular option does not.)
- [ ] **SA818 underside.** The bottom of the SA818 module has no exposed copper or test pads where the RXD, TXD and MIC traces run beneath it on the carrier.
- [ ] **SA818 footprint.** The module's castellated pads line up with the carrier footprint (2 mm pitch).
- [ ] **Display support socket fit.** J23 was placed from the MSP2401 dimension drawing: SD header row 73.01 mm from the 14-pin row, first pin 12.38 mm along from pin 1. Measure both on a real module (drawing tolerance is ±0.2 mm), then solder a 1×3 header into the first three SD holes and check that the module seats in J3 and J23 together without bending. Also note whether the module arrives with the SD holes empty, as in the manual's photo.
- [ ] **Back of the display module.** Note anything conductive on the module's back (SD card shell, header pins, exposed copper) and what it would sit over on the badge.
- [ ] **Battery plug fit.** The battery's JST-PH plug mates with the side-entry connector J1 on the back, and the lead has slack when the pouch is in its final position.
- [ ] **Battery pouch fit.** The planned spot is on the back, to the right of the 3.3 V regulator's inductor. The largest part-free rectangle there is about 41 × 67 mm (x 47.3–88.3, y 44.7–112.1 mm), which should take the 50 × 35 mm pouch in the BOM. Check the real cell against it, keep it off the Tag-Connect debug pads just above (J9) and the VSYS test pad TP4 at the left edge, and confirm the lead reaches J1.
- [ ] **Regulator versus battery pouch.** The LED rail regulator (U11, L9) sits on the back at about x 10–22 mm, y 89–104 mm. Confirm the battery pouch can be mounted without resting on the inductor.

## Part 2 — On the first assembled prototypes

> **The prototype radios are SA868S modules, not SA818S** (found 2026-11-20). They fit the same footprint and pinout but transmit at about 1.8 W and draw up to 1.0 A, against 1 W and 0.75 A. Production boards will use the SA818S. Every radio measurement below — transmit current, supply sag, harmonics, heat — must be repeated with SA818S modules before it is taken as a result for the production design. Use the SA868's low-power setting when the test does not need full power.

### Power

- [ ] **Rail voltages at the test pads.** TP1 GND, TP2 +3V3, TP3 VBUS, TP4 VSYS, TP5 VLED, TP6 SA818_VCC (positions in `electrical-design.md`, "Power Test Pads").
- [ ] **Sleep current.** With the badge asleep, the radio off and the LEDs sent an all-black frame, measure battery current. The target in the docs is 10 mA and has never been measured. Then check the LED rail's share: it should be about 0.1 mA with V6-revision LEDs. If the chain draws milliamps when dark, the LEDs fitted are an older revision (about 0.6 mA each).
- [ ] **Real SA818 transmit current.** Measure battery current during high-power and low-power TX on both carriers. The SA818S datasheet gives 650 mA typical / 750 mA maximum at high power and 450 / 550 mA at low power; the load switch (TPS22919) is rated 1.5 A. If a module draws much more than 750 mA, revisit section 14 of `engineers-notebook/main-badge-review-2026-11.md`.
- [ ] **Transmit at low battery.** Measure SA818_VCC (TP6) during high-power TX as the battery runs down, and note the battery voltage at which transmit power sags or the module drops out (its minimum supply is 3.3 V; the path from the cell loses about 0.22 V at 0.75 A). Use the result to set a firmware "battery too low to transmit" threshold.
- [ ] **SA818 supply voltage during TX.** Measure the voltage at the carrier's VCC pin while transmitting on a part-discharged battery (about 3.6 V). The SA818 needs at least 3.5 V.
- [ ] **Battery trace temperature.** After a two-minute transmission, check the battery trace and the charger (U4) for heating.
- [ ] **Fuel gauge readings.** Read the LC709203F's cell voltage with and without USB plugged in and compare with a meter on the battery connector. They should agree within a few tens of millivolts in both cases, now that the gauge is powered from the battery net. Check that state of charge moves sensibly through a charge and a discharge.
- [ ] **USB attach.** VBUS capacitance is about 7.9 µF nominal, under the 10 µF USB limit. Plug into a few different hosts and hubs and confirm clean attach, and that the charger still starts reliably with the smaller input capacitors.

### LED supply rail (VLED)

- [ ] **Output voltage.** 4.0 V ± 0.1 V at U11's output, on battery (full and nearly empty) and on USB.
- [ ] **Stability.** No oscillation or audible whine across light load, animation load and load steps.
- [ ] **Voltage at the far ear tip.** Measure at D7 (right ear tip) with the ears lit at the brightest setting firmware allows. It should stay above 3.7 V.
- [ ] **Current limit.** Find the LED current at which the rail starts to droop on a low battery. Use it to set the firmware brightness cap (the design estimate is about 0.8 A).
- [ ] **Data integrity.** All 26 LEDs respond correctly from 3.3 V data with the rail at 4.0 V, with no flicker or wrong colours on the first LED (D2).
- [ ] **Brightness balance.** Compare the 2020 border LEDs with the 5050 ear LEDs at the same value, and note the scale factor firmware needs.

### USB

- [ ] **Enumeration and data.** The badge enumerates and transfers data reliably. D+ and D− are not length-matched or routed as a pair (accepted deviation; see `hardware/routing-guide.md`).
- [ ] **USB while transmitting.** Repeat the data test during a full-power transmission. If errors appear, the fix is to re-route D+ and D− together as a pair.

### Radio and carrier

- [ ] **Harmonic filter tuning.** Tune both carriers' low-pass filters on a NanoVNA and lock the BOM values. Procedure: `engineers-notebook/harmonic-filter-design.md`, "Validation Strategy".
- [ ] **Harmonic levels.** Confirm 2nd and 3rd harmonics with a real SA818 at full power, through at least 30 dB of attenuation.
- [ ] **Band ID.** With the radio supply on, read GPIO39 with the internal pull-down and then the internal pull-up. Expect low/low on the VHF carrier, high/high on the UHF carrier, and low/high with no carrier. Repeat with the radio supply off and note the readings (UHF is expected to look like VHF then, which is why firmware must only read with the supply on). With USB connected and the UHF carrier fitted, measure the voltage on GPIO39; it should not exceed what the RP2350 datasheet allows for that pin.

### Display, camera and controls

- [ ] **Backlight control.** GPIO29 low turns the backlight on, high turns it off, and PWM dims it smoothly. The backlight must be fully **off** during reset and in BOOTSEL mode (R30 holds the gate up): hold RUN low or enter BOOTSEL in a dark room and check there is no faint glow. If there is, measure the gate voltage at Q1 and reduce R30.
- [ ] **Power LED.** Each of red, green and blue lights from its own GPIO (driven low = on), confirming the AAA3528 pin order and orientation. Check brightness balance between the three colours, and that the LED is dark, not faintly glowing, while the MCU is held in reset.
- [ ] **SD card.** Card initialises and reads/writes reliably (CS, MISO, DAT1 and DAT2 all have 10 kΩ pull-ups). Try several card brands.
- [ ] **SD card access.** A card can be inserted and removed at the right-hand board edge with the badge assembled, and its outer end sits flush with the edge.
- [ ] **Camera image integrity — mind the long routes.** The eleven camera lines were left as first routed: 118–166 mm long with 2–11 vias each, and the data lines differ in length by up to about 48 mm (the routing guide asked for ±5 mm). That is roughly 0.3 ns of skew, which should not matter at QVGA, but it has not been proven. Capture test patterns and look for wrong pixels, colour speckle or torn lines; repeat while the radio is transmitting. If images are corrupted, the fix is to re-lay the camera lines as a group, not to patch single traces.
- [ ] **Crystal start-up and drive.** R29 (1 kΩ) sits between XOUT and the crystal. Its value comes from the RP2350 hardware design guide, which chose it for that guide's crystal; the badge's crystal part is not fixed yet. Check that the 12 MHz oscillator starts reliably cold and warm and that USB enumerates every time. If start-up is slow or fails, try a lower value; check the chosen crystal's maximum drive level in its datasheet.
- [ ] **Core supply at pin 32.** One of the RP2350's three 1.1 V core-supply pins (pin 32) has no capacitor right beside it; there was no way to ground one. Scope the 1.1 V rail at C1 under load (camera capture, USB, both cores busy) and watch for resets or lock-ups. If there is trouble, see section 8 of `engineers-notebook/main-badge-review-2026-11.md` for the fix to try.
- [ ] **Subsystem tests.** Run the procedures in `electrical-design.md`, "Testing Notes" (power, audio, display, camera, user controls, integrated system).

---

*Created 2026-11-14.*
