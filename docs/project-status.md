# Project Status — DEFCON SSTV Badge

**Last Updated:** 2026-02-10

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
| Load Switch (TPS22919) | ✓ | ✓ | | |
| Battery Charger (MCP73871) | ✓ | ✓ | | |
| Fuel Gauge (LC709203F) | ✓ | ✓ | | |
| Audio TX (PCM5102A) | ✓ | ✓ | | |
| Audio RX (ADC) | ✓ | ✓ | | |
| Display (ILI9341) | ✓ | ✓ | | |
| Camera (OV2640) | ✓ | ✓ | | |
| MCU (RP2350B) | ✓ | 🔨 | | |
| Debug (Tag-Connect) | ✓ | | | |
| User Controls | ✓ | ✓ | | |
| Blinky LEDs (WS2812B) | ✓ | ✓ | | |
| SAO Connectors | ✓ | | | |
| SA818 Carrier | ✓ | | | |
| PCB Outline | ✓ | | | |

---

## Detailed Status by Subsystem

### USB-C Interface
- [x] Circuit design complete (through-hole connector, ESD protection)
- [x] Component selection: USB4105-GF-A (USB 2.0 with D+/D-), USBLC6-2SC6, PESD5V0S1BL
- [x] CC resistors for sink identification (5.1kΩ)
- [x] **KiCad: USB-C schematic complete** (connector, CC resistors, ESD protection, D+/D- to MCU)
- [ ] Verify USB4105-GF-A footprint in KiCad
- [ ] Test USB data connection with RP2350 bootloader
- [ ] Test charging with various USB sources

### Power System
- [x] TPS63001 buck-boost circuit designed (3.3V output)
- [x] TPS22919 load switch circuit designed (SA818 power control)
- [x] Inductor selected: 1µH (Coilcraft XAL3030 or equivalent)
- [x] **KiCad: TPS63001 complete** (VSYS input, +3.3V output, inductor, caps)
- [x] **KiCad: TPS22919 load switch complete** (VSYS input, SA818_VCC output, SA818_PD enable, output caps)
- [ ] Verify TPS63001 QFN-10 footprint in KiCad
- [ ] Verify TPS22919 SOT-23-5 footprint in KiCad
- [ ] Prototype and validate power system across full battery range

### Battery Management
- [x] MCP73871 charger circuit designed (500mA charge current)
- [x] LC709203F fuel gauge circuit designed (I2C interface)
- [x] Thermistor shared between charger and fuel gauge
- [x] JST-PH battery connector specified
- [x] **KiCad: MCP73871 charger complete** (VBUS input, battery connector, PROG resistors, THERM)
- [x] **KiCad: LC709203F fuel gauge complete** (VSYS power, shared thermistor, I2C bus, decoupling cap)
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
- [x] Header pinouts defined (J1: power/audio, J2: control/data)
- [x] Band auto-detection via ID resistor
- [x] RF section on carrier only (main board has no RF)
- [ ] Verify SA818 2mm pitch footprint dimensions
- [ ] Source appropriate stubby antennas (VHF and UHF)
- [ ] Determine carrier PCB layers (2 or 4 layer for RF section)
- [ ] Design carrier board in KiCad
- [ ] Prototype carrier and validate RF performance independently

### Audio Interface
- [x] PCM5102A DAC circuit designed (TX path)
- [x] RP2350 ADC circuit designed (RX path with bias network)
- [x] TX attenuator/filter component values specified
- [x] RX anti-alias filter and DC blocking designed
- [x] Assign RP2350 GPIO pins for I2S (GPIO24, 25, 29: BCK, LRCK, DIN)
- [x] Assign RP2350 ADC GPIO pin for RX audio (GPIO26: ADC0)
- [x] **KiCad: PCM5102A DAC complete** (I2S, power, charge pump caps)
- [x] **KiCad: TX output filter complete** (R5/R6 attenuator, C24 LPF, C25 DC block → MIC+/MIC-)
- [x] **KiCad: RX input filter complete** (C26 DC block, R7/R8/C27 anti-alias, R9/R10 bias → RX_AUDIO)
- [ ] Verify PCM5102A TSSOP-20 footprint in KiCad
- [ ] Test TX audio quality with oscilloscope
- [ ] Test RX audio chain and verify bias levels
- [ ] Validate SSTV encode/decode with audio circuits

### Display Interface
- [x] ILI9341 2.4" module selected (320×240 with SD slot)
- [x] Backlight circuit designed (2N7002 MOSFET, PWM control)
- [x] Assign RP2350 SPI pins for display (GPIO2-4: SCK, MOSI, MISO)
- [x] Assign GPIO pins (GPIO5-7: CS/DC/RST, GPIO22: BL, GPIO23: SD_CS)
- [x] **KiCad: Display module complete** (14-pin connector, SPI bus, backlight MOSFET circuit)
- [ ] Source specific ILI9341 2.4" module (verify pinout, SD slot)
- [ ] Verify backlight circuit polarity for chosen module
- [ ] Test SPI communication at various clock speeds
- [ ] Test SD card on shared SPI bus
- [ ] Validate SSTV image display (all common modes)

### Camera Interface
- [x] OV2640 DVP interface designed (8-bit parallel + control)
- [x] Assign RP2350B GPIO pins for DVP (GPIO8-19: D0-D7, PCLK, VSYNC, HREF, XCLK)
- [x] CAM_RST and CAM_PWDN: tied with resistors (not GPIO) to free pins for SAO
- [x] XCLK generation: 20MHz via PWM
- [x] **KiCad: Camera module complete** (18-pin connector, DVP data/control, I2C, RST/PWDN resistors)
- [ ] Source OV2640 DVP module with 2.54mm pin header
- [ ] Verify module pinout matches design
- [ ] Implement PIO program for DVP capture
- [ ] Test SCCB (I2C) communication with OV2640
- [ ] Validate viewfinder frame rate (target 15-20 FPS)
- [ ] Test JPEG capture mode for storage

### User Controls
- [x] D-pad circuit designed (5× tactile switches, active-low with pullups)
- [x] Photo button circuit designed (12mm tactile)
- [x] Airplane mode switch circuit designed (slide SPDT)
- [x] PWR LED circuit designed (RGB common-anode APFA3010LSEEZGKQBKC with current-limit resistors)
- [x] Assign GPIO pins for D-pad, photo, airplane (GPIO37-43)
- [x] Assign GPIO pins for PWR LED (GPIO44-46: R, G, B)
- [x] Assign GPIO pin for LED_DATA (GPIO47)
- [x] **KiCad: User Controls complete** (7× switches, PWR LED with resistors, all global labels)
- [ ] Source tactile switches (6×6mm for D-pad, 12mm for photo)
- [ ] Source slide switch for airplane mode
- [ ] Source RGB LED (Kingbright APFA3010LSEEZGKQBKC common-anode)
- [ ] Implement PWM control for PWR LED
- [ ] Implement battery status color mapping from LC709203 readings
- [ ] Implement software debounce in firmware
- [ ] Test all controls and LEDs

### MCU
- [x] Select MCU variant: **RP2350B (QFN-80)** — 48 GPIOs required
- [x] Complete GPIO pin assignment for all subsystems (48/48 allocated)
- [x] Crystal oscillator: 12MHz, 3215 package, 15pF load caps
- [x] External flash: W25Q128 16MB QSPI
- [x] Decoupling capacitors specified (6× 100nF + 2× 10µF)
- [x] BOOTSEL button designed (on QSPI_SS, for UF2 bootloader)
- [x] I2C pullups specified (4.7kΩ on SDA/SCL for shared bus)
- [x] Internal voltage regulator: L2 (3.3µH) inductor, C_DVDD (1µF), power pin assignments
- [x] **KiCad: MCU power section entered** (VDD pins, decoupling caps, internal regulator)
- [x] KiCad: Crystal oscillator circuit
- [x] KiCad: QSPI flash circuit
- [x] KiCad: BOOTSEL button
- [x] KiCad: I2C pullups (with global labels)
- [ ] Source RP2350B and verify QFN-80 footprint
- [ ] Verify crystal footprint (3215) and load cap values
- [ ] Verify flash footprint (SOIC-8)
- [ ] Verify peripheral pin mappings (UART1, SPI0, I2C0, PIO)

### Debug Interface
- [x] Interface selected: Tag-Connect TC2030-CTX-NL (pogo pads, no header)
- [x] Pinout defined (ARM SWD: VCC, SWDIO, RESETn, SWCLK, GND, SWO)
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
- [x] PCB outline designed: Simpsons TV shape, 120×95mm body + 50mm ears
- [x] LED placement defined: 5 per ear (10-12mm spacing), 16 border (12-15mm spacing)
- [x] **KiCad: WS2812B chain complete** (26 LEDs with 100nF bypass caps, LED_DATA input, organized by section)
- [ ] Create PCB outline in KiCad (edge cuts)
- [ ] Route LED chain on PCB (ears + border)
- [ ] Implement PIO driver for 26-LED chain
- [ ] Implement HSV rainbow animation for border
- [ ] Implement ear lightning animations (TX discharge, RX materialize)
- [ ] Implement border waterfall/fill animations
- [ ] Implement ear tip breathing (idle)
- [ ] Implement brightness control and power saving modes
- [ ] Test power consumption at various animation states

### SAO Connectors
- [x] 2× SAO connectors specified (I2C + 2 GPIO each)
- [x] GPIO assignments: SAO1 (GPIO20-21), SAO2 (GPIO27-28)
- [x] Shared I2C bus (GPIO0-1)
- [ ] Verify SAO 2×3 header footprint in KiCad

### Main Board
- [x] PCB outline designed: 120mm wide body, 50mm ears, ~140mm total height
- [x] Component placement zones defined
- [x] Lanyard slot specified (centered oval cutout)
- [ ] Create PCB outline in KiCad (edge cuts)
- [ ] Design main board schematic in KiCad
- [ ] Layout main board in KiCad
- [ ] Evaluate if main board can be 2-layer with RF removed
- [ ] Integration test: carrier + main board

---

## Next Steps (Priority Order)

1. **KiCad schematic entry** — Start with MCU and power system
2. **Source long-lead components** — SA818, display, camera modules
3. **PCB layout** — Main board first, then carrier
4. **Prototype ordering** — JLCPCB or similar
5. **Firmware development** — Can start in parallel with hardware

---

## Milestones

| Milestone | Target | Status |
|-----------|--------|--------|
| Electrical design complete | ✓ | Done |
| BOM finalized | ✓ | Done |
| PCB outline designed | ✓ | Done |
| KiCad schematic | | **In progress** (Power, Audio, Display/Camera, User Controls done; MCU in progress) |
| KiCad layout | | Not started |
| Gerbers generated | | Not started |
| Prototype ordered | | Not started |
| First power-on | | Not started |
| Firmware MVP | | Not started |
| DEFCON ready | | Not started |

---

## Related Documents

- [Electrical Design](electrical-design.md) — Circuit schematics and component details
- [BOM](bom.md) — Complete bill of materials with costs
- [PCB Design](../hardware/pcb-design.md) — Physical PCB layout and dimensions
- [Firmware Architecture](firmware-architecture.md) — Software design

---

*Let's build this thing!*
