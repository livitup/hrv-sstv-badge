# 3D Models

Shapes used only by KiCad's 3D viewer and renders. They have no effect on Gerbers, drill files or assembly data.

Most parts on the three boards use models that ship with KiCad (`${KICAD9_3DMODEL_DIR}`). This folder holds the ones KiCad does not have. Footprints reference them as `${KIPRJMOD}/../3dmodels/<file>`.

## Third-party models (openly licensed)

These are real models of the parts, copied here under their licences. Keep this table and the licence file with them.

| File | Part | Used by | Source | Licence |
|---|---|---|---|---|
| `RP2350B_QFN-80.step` | Raspberry Pi RP2350B, QFN-80 | Main badge U1 | Embedded in Raspberry Pi's "RP2350B Minimal" KiCad design (`RPI-RP2350B-MINIMAL_R4-S1`), file `rpi_rp2350b.step` | MIT, © Raspberry Pi Ltd — see `LICENSE-RaspberryPi-MIT.txt` |
| `L_Abracon_AOTA-B201610.step` | Abracon AOTA-B201610S3R3-101-T inductor | Main badge L2 | Same Raspberry Pi design, file `abracon_aota-b201610s3r3-101-t.step` | MIT, © Raspberry Pi Ltd — see `LICENSE-RaspberryPi-MIT.txt` |
| `LED_WS2812B-2020.step` | WS2812B-2020 addressable LED | Main badge D12–D27 | [marbastlib](https://github.com/ebastler/marbastlib), `3dmodels/LED_WS2812_2020.step` | [CERN-OHL-P-2.0](https://ohwr.org/cern_ohl_p_v2.txt) |
| `JST_PH_S2B-PH-SM4-TB.step` | JST PH 2-pin side-entry SMD header | Main badge J1 | [SparkFun KiCad Libraries](https://github.com/sparkfun/SparkFun-KiCad-Libraries), `3dmodels/Connector.3dshapes/JST_SMD_2.0mm-2-Black.step` | [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/) |

Notes:

- The Raspberry Pi licence file says the design "may incorporate individual elements that have been provided by third parties with permission to redistribute", with no additional terms.
- SparkFun asks, as attribution, for an email with a link when a product using their library goes on sale.
- The JST model is placed with a rotation and offset in the footprint (−90° about X, −4.8 mm in Y) so that its leads land on the pads. The model's solder tabs do not line up exactly with the KiCad footprint's tab pads.
- The files are unmodified copies, renamed.

## Placeholder shape made for this project

A plain box drawn to the footprint's outline. It is **not** a manufacturer model, and its height is approximate. Do not use it for mechanical clearance checks.

| File | Part | Used by | Notes |
|---|---|---|---|
| `SA818_module.wrl` | SA818 radio module | Both carriers, U9 / U10 | Board outline from the footprint; shield can and height approximate |

It is a VRML (`.wrl`) file, which the 3D viewer and `kicad-cli pcb render` use. A STEP export of the board will not include it.

**Why there is no real SA818 model here.** A DRA818V STEP file exists on Flux.ai, on a user-uploaded part page whose licence field says CC BY 4.0. That page's datasheet link points at SnapEDA, which suggests the part was imported from there, and SnapEDA's own files are CC BY-SA 4.0 with extra site terms on redistribution. The two licences disagree and the file's origin could not be confirmed, so it was not copied. A model downloaded directly from SnapEDA could be added under CC BY-SA 4.0 with attribution.

## Stand-ins from KiCad's own library

Where neither KiCad nor an open source has the exact part, the footprint points at the closest KiCad model:

| Part | Stand-in model | How close |
|---|---|---|
| J10, Molex 104031-0811 microSD socket | Hirose DM3D-SF microSD socket, turned 180° | Body within about 0.1 mm in length and width, 1.55 mm high against 1.42 mm; contact positions match |
| D1, Kingbright APFA3010 right-angle RGB LED | XINGLIGHT 1204 side-view LED, stretched to the footprint's depth | Shows a side-emitting LED in the right place; not the true shape |
| L1, L9, 3×3 mm power inductor | APV ANR3015 | Same size class |
| L3–L8 on the carriers, 0603 wirewound | Generic 0603 inductor | Same size |

Exact-package KiCad models are used for U5/U11 (VSON-10), U6 (SC-70-6), C1/C77 (0402) and J2 (GCT USB4105).

## Deliberately without a model

J9 (Tag-Connect pads), the fiducials and the test pads TP1–TP6 are bare copper with no component.

## Regenerating a render

```bash
kicad-cli pcb render --side top    -o front.png hardware/main_badge/defcon-sstv-badge.kicad_pcb
kicad-cli pcb render --side bottom -o back.png  hardware/main_badge/defcon-sstv-badge.kicad_pcb
```
