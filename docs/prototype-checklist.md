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
- [ ] **Regulator versus battery pouch.** The LED rail regulator (U11, L9) sits on the back at about x 10–22 mm, y 89–104 mm. Confirm the battery pouch can be mounted without resting on the inductor.

## Part 2 — On the first assembled prototypes

### Power

- [ ] **Real SA818 transmit current.** Measure battery current during high-power TX. The docs assume 1.7 A; the load switch (TPS22919) is rated 1.5 A, and the charger's battery switch plus the load switch lose about 0.3 V per amp. This measurement decides whether the radio supply path needs a redesign.
- [ ] **SA818 supply voltage during TX.** Measure the voltage at the carrier's VCC pin while transmitting on a part-discharged battery (about 3.6 V). The SA818 needs at least 3.5 V.
- [ ] **Battery trace temperature.** After a two-minute transmission, check the battery trace and the charger (U4) for heating.
- [ ] **Fuel gauge with USB connected.** Read the LC709203F with and without USB plugged in. It is powered from VSYS, which sits near 5 V on USB, so its voltage reading is expected to be wrong while charging. Record how wrong.
- [ ] **USB inrush.** About 25 µF sits on VBUS against a 10 µF USB limit. Plug into a few different hosts and hubs and confirm none of them trip.

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
- [ ] **Power LED.** Each of red, green and blue lights from its own GPIO. This also confirms the APFA3010 pad numbering if that has not been settled from the datasheet drawing.
- [ ] **SD card.** Card initialises and reads/writes reliably. DAT1, DAT2, MISO and CS have no pull-ups; add them in the next revision if cards misbehave.
- [ ] **Subsystem tests.** Run the procedures in `electrical-design.md`, "Testing Notes" (power, audio, display, camera, user controls, integrated system).

---

*Created 2026-11-14.*
