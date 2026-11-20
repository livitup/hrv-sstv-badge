# Project Status — DEFCON SSTV Badge

**Last Updated:** 2026-11-14

This document tracks the design and implementation status for all badge subsystems.

---

## Status Legend

- [x] = Design complete (documented in electrical-design.md)
- [ ] = Future work (KiCad, sourcing, prototyping, firmware)
- 🔨 = In progress

---

## Design Status Summary

| Subsystem | Design | KiCad | Sourced | Tested |
|-----------|--------|-------|---------|--------|
| USB-C | ✓ | ✓ | | |
| Power (TPS63001) | ✓ | ✓ | | |
| LED supply rail (TPS63000, 4.0V) | ✓ | ✓ | | |
| Load Switch (TPS22919) | ✓ | ✓ | | |
| Battery Charger (MCP73871) | ✓ | ✓ | | |
| Fuel Gauge (LC709203F) | ✓ | ✓ | | |
| Audio TX (PCM5102A) | ✓ | ✓ | | |
| Audio RX (ADC) | ✓ | ✓ | | |
| Display (ILI9341) | ✓ | ✓ | | |
| Camera (OV2640) | ✓ | ✓ | | |
| MCU (RP2350B) | ✓ | ✓ | | |
| Debug (Tag-Connect) | ✓ | ✓ | | |
| User Controls | ✓ | ✓ | | |
| Blinky LEDs (WS2812B) | ✓ | ✓ | | |
| SAO Connectors | ✓ | ✓ | | |
| SA818 Carrier (VHF) | ✓ | ✓ | | |
| SA818 Carrier (UHF) | ✓ | ✓ | | |
| Carrier harmonic LPF (VHF + UHF) | ✓ | ✓ | | |
| SA818 Carrier PCBs | ✓ | ✓ | | |
| Main Badge PCB | ✓ | ✓ (routed) | | |

---

## Detailed Status by Subsystem

### USB-C Interface
- [x] Circuit design complete (through-hole connector, ESD protection)
- [x] Component selection: USB4105-GF-A (USB 2.0 with D+/D-), USBLC6-2SC6, PESD5V0S1BL
- [x] CC resistors for sink identification (5.1kΩ)
- [x] **KiCad: USB-C schematic complete** (connector, CC resistors, ESD protection, D+/D- to MCU)
- [ ] Verify USB4105-GF-A footprint in KiCad
- [ ] Test USB data connection with RP2350 bootloader, including while the radio is transmitting (D+/D− are not routed as a matched pair)
- [ ] Test charging with various USB sources

### Power System
- [x] TPS63001 buck-boost circuit designed (3.3V output)
- [x] TPS22919 load switch circuit designed (SA818 power control)
- [x] Inductor selected: 2.2µH (Coilcraft XAL3030 or equivalent; TPS63001 datasheet value)
- [x] **KiCad: TPS63001 complete** (VSYS input, +3.3V output, inductor, caps)
- [x] **KiCad: TPS22919 load switch complete** (VSYS input, SA818_VCC output, SA818_PD enable, output caps)
- [x] **LED supply rail added (2026-11-14)**: U11 TPS63000 buck-boost at 4.0V (VLED) for all 26 WS2812B LEDs, distributed on the inner power layer; see `engineers-notebook/led-supply-rail.md`
- [ ] Prototype check of the LED rail: output voltage, stability, voltage at the far ear tip, firmware brightness cap
- [ ] Verify TPS63001 QFN-10 footprint in KiCad
- [ ] Verify TPS22919 SOT-23-5 footprint in KiCad
- [ ] Prototype and validate power system across full battery range

### Battery Management
- [x] MCP73871 charger circuit designed (500mA charge current)
- [x] LC709203F fuel gauge circuit designed (I2C interface)
- [x] Thermistor (TH1) on the charger; fuel gauge takes temperature over I2C (they cannot share one — corrected 2026-11-17)
- [x] JST-PH battery connector specified — changed 2026-11-14 to SMD side-entry (S2B-PH-SM4-TB) so no pins come through under the display; see `engineers-notebook/main-badge-review-2026-11.md` section 7
- [x] Display free-end support socket J23 (1×3, no connection) placed from the MSP2401 dimension drawing (2026-11-18); fit check with a real module is in `prototype-checklist.md`
- [x] **KiCad: MCP73871 charger complete** (VBUS input, battery connector, PROG resistors, THERM)
- [x] **KiCad: LC709203F fuel gauge complete** (powered from the battery net VBAT, temperature over I2C, ALARMB grounded, 1µF decoupling — wiring corrected 2026-11-17)
- [ ] Verify MCP73871 QFN-20 footprint in KiCad
- [ ] Verify LC709203F WDFN-8 footprint in KiCad
- [ ] Source 10K NTC thermistor (B=3380)
- [ ] Source 2000mAh LiPo with JST-PH connector
- [ ] Test charge cycle and thermal regulation
- [ ] Calibrate LC709203F APA value for chosen battery
- [ ] Implement I2C driver for fuel gauge readings

### RF / Carrier Board
- [x] Carrier board architecture designed (full-width overlay, front-mounted)
- [x] 4-point mounting defined (2× 1x8 signal + 2× 1x3 mechanical)
- [x] Header pinouts defined (J14/J18: Power/Audio, J15/J20: Control/Data) and matched to the locked main badge J7/J8
- [x] Band auto-detection via ID resistor (R18: 0Ω to GND = VHF, 10kΩ pull-up to SA818 VCC = UHF); "no carrier" told apart in firmware by reading the pin with pull-down then pull-up (decided 2026-11-20)
- [x] RF section on carrier only (main board has no RF)
- [x] Mechanical headers (J16/J17, J21/J22) connected to GND for RF shielding
- [x] **KiCad: VHF carrier schematic complete** (U9 SA818V, J13-J17, C54-C55, R18, LPF C58-C63 + L3-L5)
- [x] **KiCad: UHF carrier schematic complete** (U10 SA818U, J18-J22, C56-C57, R18 10kΩ, LPF C64-C69 + L6-L8)
- [x] **Harmonic LPF designed** (7-pole Chebyshev, discrete; see `engineers-notebook/harmonic-filter-design.md`)
- [x] **KiCad: VHF carrier PCB layout complete** (LPF laid out, headers on back; 0 unconnected, 0 parity issues)
- [x] **KiCad: UHF carrier PCB layout complete** (same layout as VHF; mirrored-header bug fixed; 0 unconnected, 0 parity issues)
- [ ] Visual review of the UHF carrier PCB in KiCad (it was generated from the VHF layout)
- [ ] Confirm the SA818 underside has no exposed copper where RXD/TXD/MIC run beneath it
- [ ] Prototype tuning of both LPFs (NanoVNA), then lock BOM values
- [ ] Verify SA818 2mm pitch footprint dimensions
- [ ] Source appropriate stubby antennas (VHF and UHF)
- [ ] Prototype carrier and validate RF performance independently

### Audio Interface
- [x] PCM5102A DAC circuit designed (TX path)
- [x] RP2350 ADC circuit designed (RX path with bias network)
- [x] TX attenuator/filter component values specified
- [x] RX anti-alias filter and DC blocking designed
- [x] Assign RP2350 GPIO pins for I2S (GPIO3: BCK, GPIO1: LRCK, GPIO2: DIN)
- [x] Assign RP2350 ADC GPIO pin for RX audio (GPIO40: ADC0)
- [x] **KiCad: PCM5102A DAC complete** (I2S, power, charge pump caps)
- [x] **KiCad: TX output filter complete** (R5/R6 100:1 divider, C24 LPF, C25 DC block → MIC+; R6 wiring corrected 2026-11-14)
- [x] **KiCad: RX input filter complete** (C26 DC block, R7/R8/C27 anti-alias, R9/R10 bias → RX_AUDIO)
- [ ] Verify PCM5102A TSSOP-20 footprint in KiCad
- [ ] Test TX audio quality with oscilloscope
- [ ] Test RX audio chain and verify bias levels
- [ ] Validate SSTV encode/decode with audio circuits

### Display Interface
- [x] ILI9341 2.4" module selected (320×240), standalone SD socket (J10)
- [x] Backlight circuit designed (AO3401A P-channel high-side switch, PWM control, GPIO low = on)
- [x] Assign RP2350 SPI pins for display (GPIO4,6,7: MISO, SCK, MOSI)
- [x] Assign GPIO pins (GPIO5: CS, GPIO19: DC, GPIO20: RST, GPIO29: BL, GPIO26: SD_CS)
- [x] **KiCad: Display module complete** (14-pin connector, SPI bus, backlight MOSFET circuit)
- [x] Display module specified: QDtech / LCDwiki MSP2401 (pin order and SD header position read from its manual and drawing, 2026-11-18)
- [x] Module's LED pin is active-high per the MSP2401/MSP2402 manual; still to be confirmed on a real part (`prototype-checklist.md`)
- [ ] Test SPI communication at various clock speeds
- [ ] Test SD card on shared SPI bus
- [ ] Validate SSTV image display (all common modes)

### Camera Interface
- [x] OV2640 DVP interface designed (8-bit parallel + control)
- [x] Assign RP2350B GPIO pins for DVP (GPIO8-18: D0-D7, PCLK, VSYNC, HREF; no XCLK net on the board — J4 has no XCLK pin)
- [x] CAM_RST and CAM_PWDN: tied with resistors (not GPIO) to free pins for SAO
- [ ] **REQUIRED: source an OV2640 module with its own on-board oscillator** — the main badge does not drive XCLK and cannot be changed; test one sample (PCLK toggling with no clock fed in) before ordering in quantity
- [x] **KiCad: Camera module complete** (2x9 connector matching OV2640 module, DVP data/control, I2C, RST/PWDN resistors)
- [ ] Source OV2640 DVP module with 2.54mm pin header
- [ ] Verify module pinout matches design
- [ ] Implement PIO program for DVP capture
- [ ] Test SCCB (I2C) communication with OV2640
- [ ] Validate viewfinder frame rate (target 15-20 FPS)
- [ ] Test JPEG capture mode for storage

### User Controls
- [x] D-pad circuit designed (5× tactile switches, active-low with pullups; center doubles as photo capture)
- [x] Airplane mode switch circuit designed (slide SPDT)
- [x] PWR LED circuit designed (RGB common-anode with current-limit resistors; part changed 2026-11-14 to the flat-top Kingbright AAA3528LSEEZGKQBKS, moved next to SAO connector J6, and given a second job as TX/RX indicator)
- [x] Assign GPIO pins for D-pad (GPIO21-25) and airplane switch (GPIO43)
- [x] Assign GPIO pins for PWR LED (GPIO30-32: R, G, B)
- [x] Assign GPIO pin for LED_DATA (GPIO0)
- [x] **KiCad: User Controls complete** (6× switches — D-pad center doubles as capture, PWR LED with resistors, all global labels)
- [ ] Source tactile switches (6×6mm for D-pad)
- [ ] Source slide switch for airplane mode
- [ ] Source RGB LED (Kingbright AAA3528LSEEZGKQBKS common-anode)
- [ ] Implement PWM control for PWR LED
- [ ] Implement battery status color mapping from LC709203 readings
- [ ] Implement software debounce in firmware
- [ ] Test all controls and LEDs

### MCU
- [x] Select MCU variant: **RP2350B (QFN-80)** — 48 GPIOs required
- [x] Complete GPIO pin assignment for all subsystems (46/48 allocated; GPIO46, GPIO47 spare)
- [x] Crystal oscillator: 12MHz, 3215 package, 15pF load caps
- [x] External flash: W25Q128 16MB QSPI
- [x] Decoupling capacitors specified (6× 100nF + 2× 10µF)
- [x] BOOTSEL button designed (on QSPI_SS, for UF2 bootloader)
- [x] I2C pullups specified (4.7kΩ on SDA/SCL for shared bus)
- [x] Internal voltage regulator: L2 (3.3µH, Abracon AOTA-B201610S3R3-101-T), 4.7µF input/output caps, 33Ω + 4.7µF on VREG_AVDD (Raspberry Pi reference design)
- [x] **KiCad: MCU power section entered** (VDD pins, decoupling caps, internal regulator)
- [x] KiCad: Crystal oscillator circuit
- [x] KiCad: QSPI flash circuit
- [x] KiCad: BOOTSEL button
- [x] KiCad: I2C pullups (with global labels)
- [x] **KiCad: MCU sheet complete** (all 48 GPIOs labeled, USB, debug, flash, crystal)
- [ ] Source RP2350B and verify QFN-80 footprint
- [ ] Verify crystal footprint (3215) and load cap values
- [ ] Verify flash footprint (SOIC-8)
- [ ] Verify peripheral pin mappings (UART1, SPI0, I2C0, PIO)

### Debug Interface
- [x] Interface selected: Tag-Connect TC2030-CTX-NL (pogo pads, no header)
- [x] Pinout defined (ARM SWD: VCC, SWDIO, RESETn, SWCLK, GND, SWO)
- [x] **KiCad: Tag-Connect complete** (TC2030-NL symbol with SWD signals and RUN)
- [ ] Add TC2030-CTX-NL footprint to KiCad
- [ ] Verify placement on back of PCB

### Blinky / LED Matrix
- [x] Architecture established: WS2812B chain, single GPIO
- [x] LED layout designed: 26 LEDs (10 ears + 16 border)
- [x] Chain order defined for easy animation programming
- [x] Power budget calculated (~70-120mA typical)
- [x] Animation concepts finalized:
  - TX: "Capacitor Discharge" (charge → ZAP! → flash)
  - RX: "Filling Tank" (pulse per row)
  - Idle: Rainbow border + breathing ear tips
- [x] Animation colors specified (amber, green, light blue, white, soft blue)
- [x] PCB outline designed: 120×120mm body with diagonal rabbit ear antennas and semicircular ball tips
- [x] LED placement defined: 5 per ear (WS2812B 5050), 16 border (WS2812B-2020 in 3x5x3x5 layout)
- [x] **KiCad: WS2812B chain complete** (26 LEDs with 100nF bypass caps, LED_DATA input, organized by section; border LEDs moved to the correct WS2812B-2020 symbol 2026-11-14)
- [x] **KiCad: PCB outline created** (edge cuts with rabbit ear antennas and ball tips)
- [x] **KiCad: All 26 LEDs placed** (10 ear LEDs on antennas, 16 border LEDs around display)
- [ ] Implement PIO driver for 26-LED chain
- [ ] Implement HSV rainbow animation for border
- [ ] Implement ear lightning animations (TX discharge, RX materialize)
- [ ] Implement border waterfall/fill animations
- [ ] Implement ear tip breathing (idle)
- [ ] Implement brightness control and power saving modes
- [ ] Test power consumption at various animation states

### SAO Connectors
- [x] 2× SAO connectors specified (I2C + 2 GPIO each)
- [x] GPIO assignments: SAO1 (GPIO27-28, left edge), SAO2 (GPIO41-42, right edge)
- [x] Shared I2C bus (GPIO44-45)
- [x] **KiCad: SAO connectors complete** (2× 2x3 headers with I2C and GPIO labels)
- [ ] Verify SAO 2×3 header footprint in KiCad

### Main Board
- [x] PCB outline designed: 120×120mm body with diagonal rabbit ear antennas and semicircular ball tips
- [x] Component placement zones defined
- [x] Lanyard slot specified (centered oval cutout)
- [x] **KiCad schematic complete** (all sheets: Power, Audio, Display/Camera, User Controls, MCU, Connectors)
- [x] **KiCad: Board outline created** (edge cuts with rabbit ear antennas and ball tips)
- [x] **KiCad: Component placement nearly complete** (MCU, power, audio, display, camera sections placed; power ICs on back side)
- [x] Trace routing (main board) — complete, design locked
- [x] Ground pour
- [x] DRC clean-up — 0 unconnected, 0 schematic-parity issues; only the excluded silkscreen items remain
- [x] **Design review fixes (2026-11-13)** — see "Main Badge Review" below
- [ ] Open questions from the reviews: radio supply drop and TPS22919 rating vs TX current (crystal series resistor R29 added 2026-11-18; band ID settled in firmware 2026-11-20), SD card pull-ups, VBUS capacitance — see `engineers-notebook/main-badge-review-2026-11.md`
- [x] **Smaller review items worked through (2026-11-14)**: VBUS capacitance under 10µF, SD card pull-ups, six power test pads, BOOTSEL resistor moved closer, 1.1V links widened — see `engineers-notebook/main-badge-review-2026-11.md` section 8
- [x] **SD card socket turned to face the board edge (2026-11-14)** — the card now sits flush with the right-hand edge; all four SD pull-ups fitted
- [ ] Not done: third DVDD cap at U1 pin 32 (no room)
- [x] USB D+/D− length mismatch (about 215 mm vs 170 mm) reviewed and **accepted** 2026-11-14 — not significant at Full Speed; see `engineers-notebook/main-badge-review-2026-11.md` section 6
- [ ] Evaluate if main board can be 2-layer with RF removed
- [ ] Integration test: carrier + main board

### Main Badge Review (2026-11-13)

A full review of the routed board found two schematic errors that would have stopped it powering up, plus layout weaknesses. All of the following are fixed in schematic and PCB:

- **Charger output now feeds VSYS.** U4 (MCP73871) OUT was connected only to C_OUT1; the 3.3V regulator, load switch and fuel gauge had no supply. The power flag that hid this from ERC was removed.
- **Charger input now on VBUS.** U4 IN pins 18/19 were tied to GND.
- **Radio supply path widened.** Battery, VSYS and SA818_VCC traces widened to 0.8mm where clearances allow; SA818_VCC also has a 2.0mm path on the PWR layer in parallel, with three vias at the load switch end.
- **3.3V regulator** switch-node and output traces widened.
- **RP2350**: 100nF DVDD caps under the chip at pins 10 and 51; six ground vias in the exposed pad (it previously reached the ground plane through two thin traces); 27Ω USB series resistors; 1kΩ in series with BOOTSEL.
- **Decoupling** added at the load switch input, the charger input and the DAC's DVDD/CPVDD pins; local ground vias added at 24 capacitors and the DAC.
- **RP2350 regulator rebuilt to the Raspberry Pi reference layout (2026-11-14)**: Abracon polarity-marked inductor, 4.7µF input and output capacitors, 33Ω + 4.7µF filter on VREG_AVDD, switch-node trace run between the capacitor pads. The five back-layer signal tracks that crossed under the regulator (USB D+, SAO1_GPIO2, I2C SCL, I2C SDA, SD_CS) were rerouted around its north side, so the back layer under it now carries only the 1.1 V trunk, as in the reference: two vias at the output capacitor feed a 0.6 mm back-layer trace under the chip, and the capacitor grounds have two local vias. Remaining differences: the copper is drawn as tracks rather than small filled polygons, and this is a 4-layer board (ground plane directly under the regulator) where the reference is 2-layer. `defcon-sstv-badge.kicad_dru` allows the reference's 0.12 mm gaps for the three regulator nets only.
- **Board-wide tidy (2026-11-14)**: every signal net one width, power stubs normalised, redundant ground links and via stubs removed, bent traces straightened, five long detours rerouted.
- **Housekeeping**: stale zone fill refilled (U5 clearance errors), U5 thermal-via pads renumbered to match the symbol, stray net-less via removed, real 1.6mm stackup entered, eight fiducials added.

### Schematic Review Against Datasheets (2026-11-14)

Every sheet was checked against the part datasheets. Fixed in schematic and PCB:

- **Border LEDs D12–D27 were miswired.** They used the 5050 WS2812B symbol on the 2020 footprint; the two packages number their pins differently. Now on the WS2812B-2020 symbol, and all 16 re-routed.
- **TX attenuator** — R6 (470Ω) was in series instead of to ground; it is now the 100:1 divider the design calls for.
- **Backlight** — Q1 is now an AO3401A P-channel high-side switch, because the 14-pin ILI9341 module's LED pin must be driven high. PWM is inverted (GPIO low = on).
- **L1** — 1µH → 2.2µH (TPS63001 datasheet value).

Not verifiable on paper: camera module pinout against J4, battery connector polarity. Check these with parts in hand. (The power LED's pad numbering was an open item for the old APFA3010 part; the replacement AAA3528 part's pinout was read from its datasheet drawings.)

**Follow-up the same day:** the review also found the WS2812B LEDs out of their supply range at 3.3V. They now run from a dedicated 4.0V rail (U11, TPS63000).

---

## Next Steps (Priority Order)

1. **Pre-fab review** — look over the reworked main badge power section and the area under U1 in KiCad; settle the open design questions from the review
2. **Source long-lead components** — SA818, display, camera modules, LPF tuning values (Murata GJM15 caps, Coilcraft 0603HP inductors)
3. **Generate Gerbers** — main badge and both carriers
4. **Prototype ordering** — JLCPCB or similar (carriers can go first); tune the LPFs on the prototypes
5. **Breadboard prototype** — Validate circuits before main board PCB fab
6. **Firmware development** — Can start in parallel with hardware

---

## Milestones

| Milestone | Target | Status |
|-----------|--------|--------|
| Electrical design complete | ✓ | Done |
| BOM finalized | ✓ | Done |
| PCB outline designed | ✓ | Done |
| KiCad schematic | ✓ | **Complete** — ERC clean (Main board + VHF/UHF carriers) |
| Carrier PCB layout | ✓ | **Complete** — Both carriers routed with harmonic LPF; 0 unconnected, 0 parity issues |
| Main badge PCB layout | ✓ | **Complete** — Routed; 2026-11 review fixes and LED supply rail applied, DRC clean |
| Gerbers generated | | Not started |
| Prototype ordered | | Not started |
| First power-on | | Not started |
| Firmware MVP | | Not started |
| DEFCON ready | | Not started |

---

## Related Documents

- [Electrical Design](electrical-design.md) — Circuit schematics and component details
- [BOM](bom.md) — Complete bill of materials with costs
- [Pre-Fab, Parts-in-Hand and Prototype Checklist](prototype-checklist.md) — Desk checks and fab outputs before ordering, then checks that need the physical parts or an assembled prototype
- [PCB Design](../hardware/pcb-design.md) — Physical PCB layout and dimensions
- [Firmware Architecture](firmware-architecture.md) — Software design

---

*Let's build this thing!*
