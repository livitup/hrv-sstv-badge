# DEFCON SSTV Badge — Raw BOM for Ordering

**Version:** 1.0
**Date:** 2026-02-15
**For:** One complete badge (main board + one carrier)

---

## ICs

| Part | Value/Type | Qty |
|------|------------|-----|
| RP2350B | QFN-80 MCU | 1 |
| W25Q128JVS | 16MB QSPI Flash | 1 |
| TPS63001DRCR | Buck-Boost Regulator | 1 |
| TPS22919DCKT | Load Switch | 1 |
| MCP73871 | Battery Charger | 1 |
| LC709203FQH-01TWG | Fuel Gauge | 1 |
| PCM5102A | I2S DAC | 1 |
| USBLC6-2SC6 | USB ESD Protection | 1 |
| SA818V *or* SA818U | VHF/UHF Transceiver | 1 |

## Discrete Semiconductors

| Part | Value/Type | Qty |
|------|------------|-----|
| 2N7002 | N-MOSFET SOT-23 | 1 |
| PESD5V0S1BL | TVS Diode | 1 |
| APFA3010 | RGB LED (common-anode) | 1 |
| WS2812B | Addressable LED | 26 |

## Capacitors

| Value | Qty | Notes |
|-------|-----|-------|
| 15pF | 2 | Crystal load caps |
| 4.7nF | 1 | RX anti-alias filter |
| 10nF | 1 | TX low-pass filter |
| 100nF | 38 | Decoupling (MCU, LEDs, ICs, carrier) |
| 1µF | 2 | DVDD, RX DC block |
| 2.2µF | 2 | DAC charge pump |
| 10µF | 14 | Bulk caps throughout |
| 100µF | 1 | Load switch output |

## Resistors

| Value | Qty | Notes |
|-------|-----|-------|
| 0 | 1 | Band ID (VHF carrier only) |
| 100 | 2 | PWR LED G/B |
| 220 | 1 | PWR LED R |
| 470 | 1 | TX attenuator |
| 1k | 1 | Backlight gate pulldown |
| 2k | 1 | Charge current (PROG1) |
| 4.7k | 4 | I2C pullups, RX filter |
| 5.1k | 2 | USB CC resistors |
| 10k | 3 | Pullups/pulldowns |
| 15k | 1 | Pre-charge/term (PROG3) |
| 47k | 3 | TX attenuator, RX bias |

## Thermistor

| Value | Qty | Notes |
|-------|-----|-------|
| NTC 10k (B=3380) | 1 | Battery temperature |

## Inductors

| Value | Qty | Notes |
|-------|-----|-------|
| 1µH | 1 | TPS63001 buck-boost |
| 3.3µH | 1 | RP2350 internal regulator |

## Crystal

| Value | Qty | Notes |
|-------|-----|-------|
| 12MHz | 1 | 3215 package, 15pF load |

## Connectors

| Type | Qty | Notes |
|------|-----|-------|
| USB4105-GF-A | 1 | USB-C connector |
| JST-PH 2-pin | 1 | Battery connector |
| 2x3 header (2.54mm) | 2 | SAO connectors |
| 1x8 socket (2.54mm) | 2 | Carrier signal (main board) |
| 1x3 socket (2.54mm) | 2 | Carrier mechanical (main board) |
| 1x8 pin header (2.54mm) | 2 | Carrier signal (carrier board) |
| 1x3 pin header (2.54mm) | 2 | Carrier mechanical (carrier board) |
| SMA right-angle female | 1 | Antenna connector (carrier) |
| Micro SD card socket | 1 | Storage |
| Tag-Connect TC2030-NL | — | Pads only, no component |

## Switches

| Type | Qty | Notes |
|------|-----|-------|
| Tactile 6x6mm | 5 | D-pad |
| Tactile 12mm | 1 | Photo button |
| Slide SPDT | 1 | Airplane mode |
| Tactile 3x4mm SMD | 1 | BOOTSEL |

## Modules

| Type | Qty | Notes |
|------|-----|-------|
| ILI9341 2.4" TFT | 1 | 320x240 SPI display |
| OV2640 DVP | 1 | Camera module |
| Stubby antenna | 1 | VHF or UHF matched |

## Battery

| Type | Qty | Notes |
|------|-----|-------|
| LiPo 2000mAh | 1 | 3.7V, JST-PH connector |

---

## Quick Counts

| Category | Total |
|----------|-------|
| ICs | 9 |
| Capacitors | 63 |
| Resistors | 19 |
| LEDs | 27 |
| Connectors | 13 |
| Switches | 8 |
| Modules | 3 |

---

*Note: For UHF carrier, omit the 0Ω resistor (R18 DNP).*
