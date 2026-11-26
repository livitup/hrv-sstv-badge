# Firmware Notes from the Hardware Design

Things the hardware forces on, or leaves open for, the firmware. Firmware development has not started; this page exists so these points are not lost between the board work and the first line of code.

Each note says where it comes from. `docs/firmware-architecture.md` remains the firmware design document — this page is the list of hardware-driven constraints and open questions to fold into it.

---

## Constraints the firmware must respect

### Active-low outputs: invert once at start-up

Two outputs on this board are active-low: the backlight (GPIO29) and the three PWR LED channels (GPIO30–32). Rather than inverting every value in code, set the RP2350's output-inversion override once during initialisation:

```c
// After assigning each pin to PWM
gpio_set_outover(DISP_BL, GPIO_OVERRIDE_INVERT);
gpio_set_outover(PWR_R,   GPIO_OVERRIDE_INVERT);
gpio_set_outover(PWR_G,   GPIO_OVERRIDE_INVERT);
gpio_set_outover(PWR_B,   GPIO_OVERRIDE_INVERT);
```

From then on the rest of the firmware uses normal logic: a PWM duty of 100% means fully on. (`pwm_set_output_polarity()` does the same job at the PWM slice level.) Confirm the call names against the Pico SDK version in use.

Until that code runs — in reset and early boot — the pins are inputs with a weak pull-down. The backlight stays **off** (R30 pulls Q1's gate up, added 2026-11-20), and the PWR LED channels see a few microamps. Check on the prototype that the PWR LED is not visibly glowing in reset.

### Backlight PWM is inverted

GPIO29 **low** turns the backlight on. It is off by default, so firmware must turn it on: initialise the display, draw the first frame, then enable the backlight. That order gives a clean start with no white flash. Q1 is a P-channel high-side switch (AO3401A), so brightness is the fraction of the time the pin is low. Use the output-inversion setting above and this stops mattering to the rest of the code.

- The gate has no pull-up. While the RP2350 is in reset, and until firmware configures GPIO29, the pin's default pull-down holds the gate low and the backlight is **on**. Drive the pin high early in start-up if a dark boot is wanted.
- Source: schematic review, 2026-11-14 (`main-badge-review-2026-11.md`, section 4).

### Cap total WS2812B brightness

The LED rail regulator (U11, TPS63000, 4.0 V) can supply roughly 0.8 A when boosting from a low battery. All 26 LEDs at full white would ask for about 0.94 A (26 × 36 mA; the V5 and V6 datasheets give 12 mA per colour as the working current, not the 20 mA per colour often assumed). That is still above what the regulator can give on a low battery, so the LED driver needs a global current budget, not just a per-LED brightness setting — but the cap only bites near full white on most of the chain. Budget 12 mA per lit colour channel at full value.

- The 0.8 A figure is an estimate from the regulator's 1.8 A switch limit, not a measured value. Measure it on the first prototype.
- Source: `led-supply-rail.md`.

### LED chain order: border first, ears last (changed 2026-11-25)

The chain is border #0–15 (D12–D27, clockwise from top-left), left ear #16–20, right ear #21–25. Inside each ear the first index is the **tip** and the last is the base. Earlier documents had the ears first.

- The ears are plug-in boards and may be missing. With the left ear unplugged, a bypass resistor passes the data on and the right ear's LEDs appear as #16–20. Firmware cannot detect this. Always send all 26 LEDs' worth of data.
- Keep the ear and border groups behind one index map in the driver so a later reorder is a one-line change.
- Source: `plug-in-ears.md`.

### The two LED packages will not match in brightness

Both packages are driven at 12 mA per colour (36 mA at full white), read from the WS2812B-V6 and WS2812B-2020-V6 datasheets. They still differ in brightness: typical figures are 310 / 780 / 215 mcd (red / green / blue) for the 5050 ear LEDs against 200 / 520 / 160 mcd for the 2020 border LEDs, so the border is about two-thirds as bright at the same value. Plan a per-group scale factor and tune it by eye on the prototype.

### WS2812B reset gap: more than 280 µs

After the last bit of a frame, hold the LED data line low for **more than 280 µs** before sending the next frame. Both current revisions of the LED (V5 and V6) require this; the 50 µs figure in older datasheets and many tutorials and example drivers is too short and gives flicker or frames that never latch. Use 300 µs or more.

Bit timing from the WS2812B-V6 datasheet: 0 bit 220–380 ns high, 1 bit 580 ns–1 µs high, low time 580 ns–1 µs, bit period at least 1.25 µs (800 kbit/s).

- Source: WS2812B V5, WS2812B-V6 and WS2812B-2020-V6 datasheets, read 2026-11-20. All three give the same reset figure and bit timing.

### LED rail is always on

U11's enable pin is tied to VSYS, so firmware cannot switch the LED rail off. This was reviewed on 2026-11-20 and left as is: the BOM now specifies the V6 revision of both LED types, which draw 1 µA or less each when dark, so the rail adds roughly 0.1 mA in sleep (mostly the idling regulator).

What firmware must do for sleep: send one all-black frame and then stop driving the chain. The LEDs hold their last value, so a frame that is not black keeps burning current.

If boards are ever built with older-revision LEDs (about 0.6 mA each when dark), the chain would add about 16 mA in sleep and nothing in firmware can fix it.

### SA818 H/L pin: open-drain only

Drive GPIO35 low for low power, and release it (input / high-impedance) for high power. Do not drive it high.

- Source: schematic review, 2026-11-14. This is from the SA818 module's usual application guidance and should be confirmed against the datasheet for the module actually bought.

### Do not drive radio pins while the radio is unpowered

GPIO34 (SA818_PD) also enables the load switch that powers the SA818. While that switch is off, PTT, TX and H/L must not be driven high, or the module is back-powered through its I/O pins. Set them low or high-impedance before turning the switch off, and only drive them after it is on.

### Fuel gauge: use I2C temperature mode

The LC709203F (U7) has no thermistor. Its TSW and TSENSE pins are unconnected, which is the datasheet's "temperature input via I2C" arrangement, so firmware must put it in I2C temperature mode and either write a temperature or leave the default. The charger's thermistor (TH1) is not readable by the MCU.

- A real battery temperature is not available to firmware. The RP2350's internal temperature sensor measures the chip, not the cell; for an indoor badge a fixed 25 °C is a reasonable starting point.
- The gauge is powered from the battery net (VBAT), so its voltage and state-of-charge readings are valid with or without USB connected. (Before 2026-11-17 it was on VSYS and read wrong while charging.)
- The gauge is always powered while a battery is connected, including when the rest of the badge is off.
- ALARMB is tied to ground, so the gauge's alarm output is not available; poll it over I2C.
- Per the datasheet, the Status Bit register (0x16) selects the mode: 0x0000 is I2C mode and is the power-on default, and the Cell Temperature register (0x08) defaults to 25 °C. So the gauge works with no temperature set-up at all; writing 0x08 is only needed to supply a better value.

### SD card chip select has an external pull-up

R25 (10 kΩ) holds SD_CS high while the RP2350 is in reset, so the card stays deselected until firmware drives the pin. MISO has a 10 kΩ pull-up (R26) as well, and the card's unused DAT1 and DAT2 pins are pulled up on the board (R27, R28). The display's chip select (GPIO5) has no pull-up; drive it high early in start-up before touching the shared SPI bus.

### ⚠️ MUST IMPLEMENT: carrier detection by reading the ID pin twice (decided 2026-11-20)

**This is required firmware, not a nice-to-have.** Without it the badge cannot tell an empty carrier socket from a VHF carrier. No hardware will be changed to help; the decision was to solve this in code.

**The hardware.** GPIO39 (SA818_ID) goes to one resistor on the carrier:

- VHF carrier: 0 Ω to ground.
- UHF carrier: 10 kΩ pull-up to the radio supply, which only exists while the load switch is on.
- No carrier: the pin floats.

**The method.** With the radio supply on, read the pin once with the internal pull-down and once with the internal pull-up:

| Pull-down read | Pull-up read | Meaning |
|---|---|---|
| low | low | VHF carrier |
| high | high | UHF carrier |
| low | high | No carrier (the pin follows whichever pull is on) |
| high | low | Not possible; treat as a fault |

```c
typedef enum { CARRIER_NONE, CARRIER_VHF, CARRIER_UHF, CARRIER_FAULT } carrier_t;

// Call only with the radio supply on (SA818_PD high) and settled.
carrier_t carrier_detect(void) {
    gpio_init(SA818_ID);
    gpio_set_dir(SA818_ID, GPIO_IN);

    gpio_pull_down(SA818_ID);
    sleep_ms(2);
    bool with_down = gpio_get(SA818_ID);

    gpio_pull_up(SA818_ID);
    sleep_ms(2);
    bool with_up = gpio_get(SA818_ID);

    gpio_pull_down(SA818_ID);            // leave it defined

    if (!with_down && !with_up) return CARRIER_VHF;
    if ( with_down &&  with_up) return CARRIER_UHF;
    if (!with_down &&  with_up) return CARRIER_NONE;
    return CARRIER_FAULT;
}
```

Call names are from the Pico SDK as remembered; confirm against the SDK version in use. The 2 ms waits are a starting value.

**Rules that go with it:**

1. **Power first.** Raise GPIO34, wait for the radio supply to settle, then detect. With the supply off, the UHF pull-up is tied to a dead rail that sits near ground, so a UHF carrier is expected to read low/low — the same as VHF. A reading taken with the radio off must never be used.
2. **Detect every time the radio is powered up**, not just at boot. A carrier can be swapped while the badge is in airplane mode or asleep.
3. **Do nothing on the radio until detection has run.** Frequency limits, the band shown on screen and transmit permission all depend on it.
4. **No carrier means no radio.** Turn the supply back off, disable transmit and receive, and show it on screen.
5. **Cross-check over the serial link.** A fitted SA818 answers `AT+DMOCONNECT`. If the ID says a carrier is there and the module does not answer, or the reverse, report a fault rather than guessing. Whether the module also rejects out-of-band frequencies is to be confirmed on a real one.

**Why not hardware.** Both carriers and the main badge are finished, and the method above separates all three states with what is already there. The only thing given up is identifying the carrier with the radio off, which nothing needs. Reasoning and the alternatives are in `main-badge-review-2026-11.md`, section 13.

- The UHF pull-up can sit near 5 V with USB connected. Raspberry Pi states that digital-only pins tolerate 5 V while the chip's I/O supply is on; GPIO39 is digital-only, but this has not been checked against the datasheet's pin table. The current through 10 kΩ is about 0.15 mA in the worst case. On the prototype checklist.

---

### Prototype radios are SA868S; production radios are SA818S

The modules on hand for prototypes are NiceRF SA868S (about 1.8 W, up to 1.0 A on transmit). Production boards will use the SA818S (1 W, 0.75 A). The pinout is the same. The SA868 datasheet has its own command section; it has not been compared line by line with the SA818S commands, so do that before writing the radio driver, and keep anything module-specific behind one small interface. The SA868's feature list gives 12.5 kHz channel width only, where the SA818S offers 12.5 or 25 kHz.

- Source: SA868 V1.3 and SA818S V1.7 datasheets, read 2026-11-20.

### Block transmit when the battery is too low

The SA818 needs at least 3.3 V and the path from the cell loses about 0.22 V at full transmit current. Near the end of the battery, transmit power sags or the module drops out. Firmware should refuse to transmit below a battery-voltage threshold and say why on screen. The threshold comes from the prototype test "Transmit at low battery" in `docs/prototype-checklist.md`.

- Source: radio supply review, 2026-11-20 (`main-badge-review-2026-11.md` section 14).

---

## Already designed — do not re-decide

### The PWR LED also shows transmit and receive (decided 2026-11-14)

D1 was originally battery status only, with TX/RX shown by the WS2812B animations alone. The owner decided it should show radio activity too. The plan, in priority order: blinking red for a critical battery, magenta while transmitting, blue while receiving, otherwise the battery colour.

- Blue and magenta were picked because they are not battery colours. They are a starting point and are firmware-only.
- This supersedes the note in `electrical-design.md` that no separate TX/RX indicator was needed. The reasons for keeping D1 off the WS2812B chain (`electrical-design-decisions.md`) apply even more now: the radio state stays visible when the animations are dimmed or the chain has a fault.
- Details: `firmware-architecture.md`, "Status LED (PWR LED, D1)".

### The display is not always on

`firmware-architecture.md` already provides for the display turning off:

- `POWER_STATE_IDLE` is defined as "Display off, radio standby".
- The configuration structure has `display_brightness` and `display_timeout_ms`.
- There is a `power_set_display_brightness()` function and a `UI_EVENT_TIMEOUT` event.

The hardware supports this: the backlight is under GPIO29 PWM control. What is not yet specified is the default timeout value and what wakes the display.

---

## Open questions

### TODO: What to do with the debug port

The badge should have a working debug port, and since this is a DEF CON badge it is also an obvious place for something fun. What is smart, reasonable, possible or insane here has not been worked out. This needs its own analysis before firmware design goes far.

**What the hardware has today**

- J9 is a Tag-Connect TC2030-NL footprint on the back: six bare pogo pads carrying ARM **SWD** (SWDIO, SWCLK, RUN/reset, 3.3 V, ground). There is no connector fitted.
- The RP2350's debug port is SWD. The board has no separate JTAG pins, so "the JTAG port" in practice means this SWD port.
- USB also gives the UF2 bootloader (BOOTSEL button), which `electrical-design.md` names as the primary way for users to load firmware.

**Questions to answer**

1. **Access.** The pads need a roughly $40 Tag-Connect cable. Is that acceptable for attendees, or should there be solderable pads or a header as well?
2. **Open or locked?** Leaving SWD open means anyone can read and rewrite the flash. That conflicts with the amateur-licence unlock in `CLAUDE.md`, which is described as preventing "casual circumvention" of the transmit lock. Decide whether that is acceptable, and what "casual" should mean on a badge whose owners are hackers.
3. **What the RP2350 offers.** The chip has one-time-programmable settings for disabling debug and for secure boot. These need to be read up on before deciding anything; one-time settings cannot be undone.
4. **The fun part.** Options range from leaving it open and documented, through a hidden console or challenge reachable over the debug port, to a deliberate puzzle about getting in. None has been evaluated.
5. **Regulatory angle.** Whatever is chosen must not make it trivial to transmit without a callsign by accident.

**Status:** not started. Nothing here is decided.

### Display timeout default and wake source

See "The display is not always on" above. The value of `display_timeout_ms` and what wakes the display (any button, radio activity) are not specified.

---

*Created 2026-11-14.*
