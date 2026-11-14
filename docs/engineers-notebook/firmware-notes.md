# Firmware Notes from the Hardware Design

Things the hardware forces on, or leaves open for, the firmware. Firmware development has not started; this page exists so these points are not lost between the board work and the first line of code.

Each note says where it comes from. `docs/firmware-architecture.md` remains the firmware design document — this page is the list of hardware-driven constraints and open questions to fold into it.

---

## Constraints the firmware must respect

### Backlight PWM is inverted

GPIO29 **low** turns the backlight on. Q1 is a P-channel high-side switch (AO3401A), so brightness is the fraction of the time the pin is low.

- The gate has no pull-up. While the RP2350 is in reset, and until firmware configures GPIO29, the pin's default pull-down holds the gate low and the backlight is **on**. Drive the pin high early in start-up if a dark boot is wanted.
- Source: schematic review, 2026-11-14 (`main-badge-review-2026-11.md`, section 4).

### Cap total WS2812B brightness

The LED rail regulator (U11, TPS63000, 4.0 V) can supply roughly 0.8 A when boosting from a low battery. All 26 LEDs at full white would ask for about 1.5 A. The LED driver needs a global current budget, not just a per-LED brightness setting.

- The 0.8 A figure is an estimate from the regulator's 1.8 A switch limit, not a measured value. Measure it on the first prototype.
- Source: `led-supply-rail.md`.

### The two LED packages will not match in brightness

The 16 border LEDs are WS2812B-2020 parts (described in KiCad's library as 12 mA parts); the 10 ear LEDs are the larger 5050 package. Expect the border to look dimmer than the ears at the same value, and plan a per-group scale factor. The 5050's rated current has not been checked against its datasheet.

### LED rail is always on

U11's enable pin is tied to VSYS. A dark WS2812B still draws quiescent current, so the sleep-current figures in `firmware-architecture.md` do not yet account for 26 idle LEDs. Firmware cannot switch the rail off with the current hardware.

### SA818 H/L pin: open-drain only

Drive GPIO35 low for low power, and release it (input / high-impedance) for high power. Do not drive it high.

- Source: schematic review, 2026-11-14. This is from the SA818 module's usual application guidance and should be confirmed against the datasheet for the module actually bought.

### Do not drive radio pins while the radio is unpowered

GPIO34 (SA818_PD) also enables the load switch that powers the SA818. While that switch is off, PTT, TX and H/L must not be driven high, or the module is back-powered through its I/O pins. Set them low or high-impedance before turning the switch off, and only drive them after it is on.

### Fuel gauge: use I2C temperature mode

The LC709203F shares the charger's thermistor and its own thermistor-drive pin (TSW) is not connected, so its thermistor mode will not read correctly. Supply temperature over I2C instead, or leave it at the default.

- The gauge is powered from VSYS, which sits near 5 V with USB connected, so its voltage reading is wrong while charging. Whether to fix that in hardware is still open; until then, firmware should not trust the gauge while USB is present.

### Band ID reads as VHF when the radio is off

The UHF carrier's ID pull-up goes to the switched radio supply. Read GPIO39 only after the load switch is on. The scheme cannot yet tell "no carrier" from "VHF"; a hardware change is under discussion.

---

## Already designed — do not re-decide

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
