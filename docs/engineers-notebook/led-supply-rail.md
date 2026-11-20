# LED Supply Rail: Why the WS2812Bs Have Their Own 4.0 V Regulator

**Date:** 2026-11-14
**Decision:** add a TPS63000 buck-boost converter (U11) making a 4.0 V rail, VLED, for all 26 WS2812B LEDs.
**What was built:** see "LED Supply Rail" in `docs/electrical-design.md`.

---

## The problem

The LEDs were on the 3.3 V rail. The schematic-versus-datasheet review found that this is outside both parts' supply range:

| Part | Where | Supply range in datasheet |
|---|---|---|
| WS2812B-2020 | 16 display border LEDs | 3.7–5.3 V |
| WS2812B (5050) | 10 ear LEDs | 3.5–5.3 V |

Plenty of badges run WS2812Bs from 3.3 V and get away with it. The usual symptom is weak or off-colour blue, because blue has the highest forward voltage. It is outside the datasheet, though, and it can vary from batch to batch — not something to find out after a production run.

## The constraint that shapes the answer: the data line

The obvious fix is "give them 5 V". That creates a second problem.

The LED data line comes from an RP2350 GPIO at 3.3 V logic. The 5050 datasheet specifies the data-high threshold as **0.7 × VDD**:

| LED supply | Data-high needed | 3.3 V GPIO OK? |
|---|---|---|
| 3.3 V | 2.3 V | Yes, but supply is out of spec |
| 4.0 V | 2.8 V | Yes, 0.5 V margin |
| 4.5 V | 3.15 V | Marginal |
| 5.0 V | 3.5 V | No |

The 2020 datasheet is more forgiving (2.7 V at a 5 V supply), but the **first LED in the chain is a 5050 ear LED**, so the stricter number applies.

So the target is not "as high as possible". It is a window: at or above 3.7 V for the supply spec, and low enough that 0.7 × VDD stays under 3.3 V with margin. **4.0 V** sits in the middle of that window.

## Options considered

### A. Feed the LEDs straight from VSYS

VSYS is the charger's output: battery voltage (3.0–4.2 V) on battery, about 5 V with USB connected.

- No added parts.
- Below 3.7 V for the lower part of the battery's discharge, which is exactly when the badge has been in use for hours.
- VSYS sags during transmit (the radio pulls its current through the same node), so the LEDs would flicker with the radio.
- On USB the rail is about 5 V and the data level is out of spec.

Rejected: it fixes the problem only some of the time and adds two new ones.

### B. Boost converter to about 4.0 V

- In spec on battery.
- A boost converter cannot regulate below its input: with USB connected VSYS is about 5 V and that passes straight through, so the data-level problem returns. It would need a level shifter on the data line to be correct in all conditions.
- Slightly cheaper than C.

Rejected: correct only with a level shifter, at which point it is no simpler than C.

### C. Buck-boost at 4.0 V — chosen

- Holds 4.0 V whether VSYS is 3.0 V or 5 V, so supply and data level are both in spec on battery and on USB, with no level shifter.
- The TPS63000 is the adjustable sibling of the TPS63001 already on the board: same package, same pinout, same inductor, same datasheet. The layout of the existing regulator could be reused, and no new part family enters the BOM.
- Takes the LED load off the 3.3 V rail that feeds the MCU and the audio DAC.
- Cost: about $2 in parts, eight components, and a re-route of the LED supply.

### D. Leave at 3.3 V and test on prototypes

- Free, and it might be fine.
- If it is not fine, the fix is a board respin, because the LED supply was a plane, not a net that can be patched.

Rejected in favour of C once the user decided the risk was not worth a respin.

## Detail decisions inside option C

**Output voltage, 4.0 V.** R23 = 1 MΩ, R24 = 143 kΩ: 0.5 V × (1 + 1000/143) = 4.00 V. Both are standard 1 % values. A 2.2 pF feed-forward capacitor across R23 follows the datasheet (2.2 µs / R23).

**Power-save mode on (PS/SYNC to ground).** The 3.3 V regulator runs with power save off to keep ripple out of the SSTV audio path. The LED rail has no such concern and spends most of its life at tens of milliamps, where power-save mode is markedly more efficient.

**Always enabled (EN to VSYS).** Putting EN on a spare GPIO would let firmware cut the LEDs' quiescent current in sleep. It was left out because only two GPIOs are spare and the camera-clock question may want them.

*Revisited and closed 2026-11-20.* The camera-clock question had been closed, so both spare GPIOs were free and the enable line was looked at properly:

- *What it would save depends on the LED revision.* Worldsemi's V5 datasheet gives 0.6 mA quiescent per LED; the V6 datasheet gives 1 µA or less. For 26 LEDs that is about 16 mA against about 26 µA. The regulator itself idles at about 50 µA in power-save mode.
- *What it would cost.* Both spare pins (GPIO46, GPIO47) are boxed in at the chip: they sit in a 0.3 mm gap between the I2C clock trace and a 3.3 V via, too narrow for a trace or a via. Escaping means moving that via and trace or going under the chip, followed by about 90 mm of routing across the board to U11. A dry run found a route from U11 to mid-board but not out of the chip; a complete route was not found.
- *Decision.* Leave EN tied to VSYS and specify the V6 revision of both LED types in the BOM. That gets nearly all the saving with no board change. V6 has the same footprint and pinout as V5.

Side findings from reading the datasheets:

- Both V5 and V6 need a reset gap of more than 280 µs (now in the firmware notes).
- V6's data-high threshold is 0.55 × VDD — 2.2 V at the 4.0 V rail — which gives 3.3 V logic more margin than the 0.7 × VDD used when the rail voltage was chosen.
- **V6 is rated for a 3.3–5.3 V supply in both packages** ("3.3 V power supply is supported"). The datasheets this rail was designed against gave 3.7 V as the minimum. So with V6 parts the LEDs would be in spec on the 3.3 V rail, and this regulator is no longer strictly required. It was not removed: it is built and routed, it keeps the board correct if an older revision is ever fitted, and a higher supply leaves the blue LEDs more voltage to work with. A future cost-down could revisit it.
- Both packages are driven at 12 mA per colour, 36 mA at full white, so the whole chain is about 0.94 A at full white rather than the 1.5 A assumed when the rail was sized.

If a later board revision wants a hardware switch anyway, the place to start is the 3.3 V via beside U1 pins 57–58.

Sources: [WS2812B-V6 datasheet](https://atta.szlcsc.com/upload/public/pdf/source/20251201/45CD695EA06F90F53A3AC4760AC4DC7D.pdf), [WS2812B V5 datasheet](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/8880/WS2812B.pdf), [WS2812B-2020-V6 datasheet](https://files.waveshare.com/wiki/common/WS2812_0807.pdf).

**Distribution on the inner power layer as tracks.** The LED supply pads already had vias into the power plane, so re-netting them and joining the vias with inner-layer tracks disturbed the outer layers least. The cost is that every such track is a slot in the 3.3 V plane.

- To keep slots out of the middle of the board, the trunk follows the left and top edges and reaches both ears from there. A slot at the board edge isolates only a sliver of plane.
- The 3.3 V regulator sits *inside* the ring of border LEDs, so a closed ring of VLED track would have fenced it in. The ring is left open at the top-right corner (between D16 and D17), which is the direct path from the regulator to the MCU.
- The existing 2.0 mm radio supply track on the same layer blocks the top row between D13 and D14. That link hops over it on the front layer.
- The backlight FET's 3.3 V via ended up on the wrong side of the edge trunk; it was moved to the other side with a short back-layer hop.

**Regulator placement.** On the back between the display socket and the charger: the only free area next to VSYS, and close to the left-edge trunk.

**Input feed.** A 0.5 mm front-layer trace runs about 14 mm from the VSYS trunk beside the load switch (U6) to U11's input via at (12.1, 96.5). It joins the 0.8 mm trunk directly on the front layer at the via at (20.1, 85.55). As first built it tapped VSYS at U6's pin 1 pad on the back through an extra via, so the LED regulator's current passed through the load switch's pad; that was tidied on 2026-11-20 (the extra via and back-layer stub removed) after the owner asked what the trace was for.

## Consequences to remember

- **Firmware must cap LED brightness.** The regulator can supply roughly 0.8 A when boosting from a low battery; all 26 LEDs at full white would ask for about 0.94 A (26 × 36 mA; corrected 2026-11-20 from 1.5 A after reading the datasheets for both packages, which give 12 mA per colour).
- **Battery life.** WS2812Bs are constant-current devices, so raising the rail from 3.3 V to 4.0 V costs about 20 % more LED power — around 15 mA more battery current at idle.
- **Sleep current** includes the quiescent draw of 26 LEDs, since the rail is always on.
- **Not verified on hardware.** Regulator stability with this divider, real LED brightness balance between the two packages, and the voltage at the far ear tip should be checked on the first prototype.

---

*Written 2026-11-14.*
