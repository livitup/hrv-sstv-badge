# Parts-in-Hand and Prototype Checklist

Checks that cannot be done on paper. Each one needs either the physical part or an assembled prototype board. They come out of the November 2026 design review (`engineers-notebook/main-badge-review-2026-11.md`) and the LED supply rail work (`engineers-notebook/led-supply-rail.md`).

This list does **not** replace the subsystem test procedures in `electrical-design.md` ("Testing Notes"). It covers the specific assumptions and open risks the review left behind. Where a test already exists there, this list points to it.

Tick items off here as they are done, and record what was found.

---

## Part 1 — With parts in hand, before ordering boards

These check assumptions that are built into the PCB. If one turns out wrong, the board has to change, so do them **before** the fab order if the parts can be bought first.

- [ ] **Camera module pinout.** The 2×9 header on the OV2640 module matches the J4 pin table in `electrical-design.md` exactly, pin for pin, including which row is odd and which is even.
- [ ] **Camera module has its own clock.** With 3.3 V applied, RESET high and PWDN low, and no clock fed in, PCLK toggles on a scope. The badge supplies no XCLK, so a module that fails this will not work.
- [ ] **Display module backlight polarity.** On the chosen 14-pin ILI9341 module, the LED pin lights the backlight when driven **high** (3.3 V), and stays dark when left open or grounded. The backlight switch (Q1) assumes this.
- [ ] **Display module pin order.** The module's 14 pins match J3: VCC, GND, CS, RESET, DC, MOSI, SCK, LED, MISO, then the five touch pins.
- [ ] **Battery connector polarity.** With the battery's JST-PH plug inserted in J1, battery positive lands on J1 pin 1 (the net that goes to the charger's battery pins). JST-PH polarity is not standardised between battery vendors — check the actual battery with a meter.
- [ ] **SA818 underside.** The bottom of the SA818 module has no exposed copper or test pads where the RXD, TXD and MIC traces run beneath it on the carrier.
- [ ] **SA818 footprint.** The module's castellated pads line up with the carrier footprint (2 mm pitch).
- [ ] **Display support socket position.** Measure the chosen display module: the distance from the 14-pin header row to the pins on the opposite edge (usually a 4-pin SD header), and their position along that edge. A support socket for the display's free end can only be placed once this is known. Free copper-clear area on the board is x ≈ 74.5–81.5 mm, y ≈ 73–88 mm; outside it, traces would have to move.
- [ ] **Back of the display module.** Note anything conductive on the module's back (SD card shell, header pins, exposed copper) and what it would sit over on the badge.
- [ ] **Battery plug fit.** The battery's JST-PH plug mates with the side-entry connector J1 on the back, and the lead has slack when the pouch is in its final position.
- [ ] **Rail voltages at the test pads.** TP1 GND, TP2 +3V3, TP3 VBUS, TP4 VSYS, TP5 VLED, TP6 SA818_VCC (positions in `electrical-design.md`, "Power Test Pads").
- [ ] **Battery pouch fit.** The planned spot is on the back, to the right of the 3.3 V regulator's inductor. The largest part-free rectangle there is about 41 × 67 mm (x 47.3–88.3, y 44.7–112.1 mm), which should take the 50 × 35 mm pouch in the BOM. Check the real cell against it, keep it off the Tag-Connect debug pads just above (J9) and the VSYS test pad TP4 at the left edge, and confirm the lead reaches J1.
- [ ] **Regulator versus battery pouch.** The LED rail regulator (U11, L9) sits on the back at about x 10–22 mm, y 89–104 mm. Confirm the battery pouch can be mounted without resting on the inductor.

## Part 2 — On the first assembled prototypes

### Power

- [ ] **Real SA818 transmit current.** Measure battery current during high-power TX. The docs assume 1.7 A; the load switch (TPS22919) is rated 1.5 A, and the charger's battery switch plus the load switch lose about 0.3 V per amp. This measurement decides whether the radio supply path needs a redesign.
- [ ] **SA818 supply voltage during TX.** Measure the voltage at the carrier's VCC pin while transmitting on a part-discharged battery (about 3.6 V). The SA818 needs at least 3.5 V.
- [ ] **Battery trace temperature.** After a two-minute transmission, check the battery trace and the charger (U4) for heating.
- [ ] **Fuel gauge with USB connected.** Read the LC709203F with and without USB plugged in. It is powered from VSYS, which sits near 5 V on USB, so its voltage reading is expected to be wrong while charging. Record how wrong.
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
- [ ] **Band ID.** GPIO39 reads low on the VHF carrier and high on the UHF carrier once the radio is powered. Note what it reads with no carrier fitted and with the radio off.

### Display, camera and controls

- [ ] **Backlight control.** GPIO29 low turns the backlight on, high turns it off, and PWM dims it smoothly. The backlight is expected to be on during reset.
- [ ] **Power LED.** Each of red, green and blue lights from its own GPIO (driven low = on), confirming the AAA3528 pin order and orientation. Check brightness balance between the three colours, and that the LED is dark, not faintly glowing, while the MCU is held in reset.
- [ ] **SD card.** Card initialises and reads/writes reliably (CS, MISO, DAT1 and DAT2 all have 10 kΩ pull-ups). Try several card brands.
- [ ] **SD card access.** A card can be inserted and removed at the right-hand board edge with the badge assembled, and its outer end sits flush with the edge.
- [ ] **Camera image integrity — mind the long routes.** The eleven camera lines were left as first routed: 118–166 mm long with 2–11 vias each, and the data lines differ in length by up to about 48 mm (the routing guide asked for ±5 mm). That is roughly 0.3 ns of skew, which should not matter at QVGA, but it has not been proven. Capture test patterns and look for wrong pixels, colour speckle or torn lines; repeat while the radio is transmitting. If images are corrupted, the fix is to re-lay the camera lines as a group, not to patch single traces.
- [ ] **Core supply at pin 32.** One of the RP2350's three 1.1 V core-supply pins (pin 32) has no capacitor right beside it; there was no way to ground one. Scope the 1.1 V rail at C1 under load (camera capture, USB, both cores busy) and watch for resets or lock-ups. If there is trouble, see section 8 of `engineers-notebook/main-badge-review-2026-11.md` for the fix to try.
- [ ] **Subsystem tests.** Run the procedures in `electrical-design.md`, "Testing Notes" (power, audio, display, camera, user controls, integrated system).

---

*Created 2026-11-14.*
