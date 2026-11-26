# Plug-in Rabbit Ears

**Date:** 2026-11-25
**Status:** Built in schematic and PCB. Main badge reopened for this change (it had been locked on 2026-11-21) and **locked again, with the new ear board, on 2026-11-25**. Carriers untouched.

## Why

Two problems with ears that are part of the main board:

1. **They would break.** Each ear was an arm about 75 mm long on a neck 10.8 mm wide, sticking out of a badge worn in a crowd. A reviewer pointed out that they were very likely to snap, and the owner agreed.
2. **They waste panel.** With ears, the main badge's bounding box was 185.8 × 199.0 mm. Without them it is 127.8 × 138.5 mm (body plus feet), about 52% less area for the fab to bill, and a far easier shape to panelise.

The ears stay — they are the look of the badge — but as separate boards that plug in. A knocked ear unplugs, a broken ear is replaced, and each kit ships with a spare.

## What was decided

| Question | Decision | By |
|----------|----------|----|
| How do ears attach? | 2×3 header and socket, the same connector as the SAO ports | Owner |
| Which side? | Back of the badge. The front is taken: the carrier covers the top 40 mm at full width, 5–8 mm off the surface | Owner and review agreed |
| LED chain order | Border first, then left ear, then right ear, so an unplugged ear cannot darken the border | Owner |
| Pinout | SAO-compatible, so a mix-up between ear sockets and SAO ports is harmless | Owner |
| Lost ears | Accept it; ship a spare ear per kit. No tether hole | Owner |
| Ear length | Longer than the old ear by the part hidden behind the badge, so the overall height does not change | Owner |

## As built

### Ear board (`hardware/ear`)

- One design, used for both ears. 15.0 × 95.5 mm, 2 layers, 1.6 mm.
- Shape: a straight strip 10.83 mm wide (the old neck width) with a 15 mm ball at the tip. The bottom 13 mm sits behind the badge.
- Parts: D2–D6 (WS2812B 5050; D2 at the tip), C28–C32 (100 nF 0402), J26 (2×3 pin header). These reuse the references freed on the main badge.
- **J26 is on the LED side.** The ear sits behind the badge with its LEDs facing forward, so the header points forward into the socket on the badge's back.
- Routing: supply up the left side on the front (0.5 mm); data up the right side on the back to the tip LED, then LED to LED down the front; the last LED's output returns down the right side to the header. Ground pour on both layers, two ground vias per LED.
- LED positions were taken from the old left ear on the main badge. With an ear plugged in, each LED is within 0.04 mm of where it used to be and the tip is 0.02 mm from the old outline.

### Main badge

- Ear outline removed; the top edge is a straight line. Ear LEDs D2–D11, their capacitors C28–C37 and all ear copper removed.
- **J24** (left) and **J25** (right): 2×3 through-hole sockets on the back, centred at (32.96, 5.30) and (87.04, 5.30) mm, turned 225° and 135°. The socket carries the 45° angle, which is what lets one straight ear serve both sides.
- **R31**, 1 kΩ 0402, front, at (35.9, 9.3): joins J24 pin 5 to pin 6.
- Footprint: the same 2×3 pattern as J5/J6. Female sockets are fitted by the user.

| Socket pin | Net | Ear header J26 pin |
|------------|-----|--------------------|
| 1 | VLED (4.0 V) | 2 |
| 2 | GND | 1 |
| 3 | not connected | 4 |
| 4 | not connected | 3 |
| 5 | data in (`EAR_L_DIN` on J24, `EAR_R_DIN` on J25) | 6 |
| 6 | data out (`EAR_R_DIN` on J24, not connected on J25) | 5 |

The ear header's numbers are the mirror of the socket's because the two face each other, as on any SAO. The mating was checked numerically from the two board files, pin by pin, for both sockets.

### Chain

```
GPIO0 → D12 … D27 (border, #0–15) → J24 → left ear tip … base (#16–20) → J25 → right ear tip … base (#21–25)
```

- `LED_DATA` now goes to D12. It reuses the back-layer run that used to bring the right ear's output to D12: one new via at (54.025, 20.975) joins the two.
- `EAR_L_DIN`: D27's output, front layer, up the left edge at x = 1.4 mm, then one 45° line to R31 and J24 pin 5. No vias.
- `EAR_R_DIN`: J24 pin 6 to J25 pin 5 on the back layer along the top edge (y = 1.35 mm). No vias; both ends are through-hole pins.
- VLED trunk on the inner power layer: unchanged up the left edge, then along the top at y = 2.61 mm through J24 pin 1, and round the south side of J25 to reach its pin 1 from the east (J25's pin 2, ground, is the one nearest the edge on that side).

### Why R31

WS2812Bs regenerate the data, so a missing ear in the middle of the chain would cut off everything after it. With the border first, only the right ear is after the left ear. R31 carries the data across the left socket when the left ear is missing. When the ear is present, its last LED drives the line and wins easily against 1 kΩ (about 4 mA of contention at worst). With the ear missing, the right ear's input sees 1 kΩ and a few tens of picofarads — a time constant of tens of nanoseconds against bit features of 220 ns and up.

Firmware cannot tell that an ear is missing. With the left ear out, the right ear's LEDs take indices 16–20.

## Things to know

- **An SAO in an ear socket** gets 4.0 V on its supply pin, not 3.3 V, and LED data on its GPIO pins. Nothing on the badge is harmed. Most SAOs will tolerate 4.0 V; it is still out of spec for them.
- **An ear in an SAO port** gets 3.3 V, which V6 LEDs accept, and whatever the badge drives on the SAO GPIO pins. It would also face backwards, because the SAO ports are on the front.
- **An ear plugged in upside down or one row off** puts 4.0 V and ground on LED data pins. It only looks right one way round (pointing up and outward), but nothing physically prevents it. Not protected against; noted in the assembly instructions.
- **Retention is friction only.** That is the point — the ear lets go before it breaks — but ears will be lost. One spare per kit.
- The ears stand about 11 mm behind the badge's back surface (socket plus header spacer). The battery pouch on the back is about the same thickness.

## Lanyard holes (same day)

With the ears off the outline, the question of how the badge hangs came up. The centred slot in the original outline plan was never on the board and does not fit (flash chip, carrier). Two 4 mm holes were added at the top edge instead, H1 at (20.0, 5.5) and H2 at (100.0, 5.5) mm, and the inner-layer LED supply track along the top-left moved from y = 6.5 to y = 9.8 mm to clear H1. Each ear's hidden section passes about 3.5 mm from its hole. Details: `hardware/pcb-design.md`, "Lanyard Holes".

## Checks

- Main badge: DRC 0 unconnected, 0 schematic-parity issues, the same 2 excluded silkscreen items as before. ERC unchanged (5 library-mismatch warnings).
- Ear: DRC 0 violations, 0 unconnected, 0 parity. ERC clean.
- Backup of the main badge board before this change: `defcon-sstv-badge.kicad_pcb.pre-plugears.bak` (not in git).

## Still to do

- Prototype checks are in `docs/prototype-checklist.md` (ear fit, retention, missing-ear behaviour, far-tip voltage through the connector).
- Fab files for the ear board, with the others.
