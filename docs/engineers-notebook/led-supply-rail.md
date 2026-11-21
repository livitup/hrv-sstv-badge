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
- The backlight FET's 3.3 V via ended up on the wrong side of the edge trunk; it was first patched with a short back-layer hop to a second via, then fixed properly on 2026-11-21 by moving Q1 so that one via east of the trunk serves it (see `main-badge-review-2026-11.md`, section 20).

**Regulator placement.** On the back between the display socket and the charger: the only free area next to VSYS, and close to the left-edge trunk.

**Input feed.** A 0.5 mm front-layer trace runs about 14 mm from the VSYS trunk beside the load switch (U6) to U11's input via at (12.1, 96.5). It joins the 0.8 mm trunk directly on the front layer at the via at (20.1, 85.55). As first built it tapped VSYS at U6's pin 1 pad on the back through an extra via, so the LED regulator's current passed through the load switch's pad; that was tidied on 2026-11-20 (the extra via and back-layer stub removed) after the owner asked what the trace was for.

## Consequences to remember

- **Firmware must cap LED brightness.** The regulator can supply roughly 0.8 A when boosting from a low battery; all 26 LEDs at full white would ask for about 0.94 A (26 × 36 mA; corrected 2026-11-20 from 1.5 A after reading the datasheets for both packages, which give 12 mA per colour).
- **Battery life.** WS2812Bs are constant-current devices, so raising the rail from 3.3 V to 4.0 V costs about 20 % more LED power — around 15 mA more battery current at idle.
- **Sleep current** includes the quiescent draw of 26 LEDs, since the rail is always on.
- **Not verified on hardware.** Regulator stability with this divider, real LED brightness balance between the two packages, and the voltage at the far ear tip should be checked on the first prototype.

---

*Written 2026-11-14.*

---

## Addendum (2026-11-21): top row and right column connections rebuilt

During the owner's visual pass the LED supply connections along the top of the display (D12–D16 and C38–C42) were found untidy: the front-layer link past D13 was an auto-routed staircase, capacitors had their own vias with stub tracks on the inner layer, and widths were mixed. They were rebuilt to one pattern.

**The pattern.** The inner-layer LED track runs under the row. At each LED there is one via on that track, between the LED and its capacitor. On the front, a short trace runs from the LED's supply pad to the via, and another from the via to the capacitor's supply pad. One via serves both parts; the capacitors have no via or inner-layer stub of their own. D14, D15 and D16 are identical. D12 is the same shape with the via 0.3 mm higher, because the inner track is higher there.

**D13 is the exception.** The 2.0 mm radio supply track crosses the inner layer exactly under D13, so nothing on the LED net can exist on the inner layer between about x = 30.5 and 33.5 mm, and no via can go there. The inner track stops at a via west of it (29.33, 62.265) and starts again at a via east of it (34.228, 62.575). Between them the supply runs on the front: into D13's pad, on along y = 62.9 below D13's data-out pad, with a drop to C42. This front-layer link carries the current for D13 to D16, about 0.15 A at most.

**Right column.** C44 and C45 were to be made alike. They cannot be identical, because USB D+ runs on the back directly under where C45's vias would go. As built: both have a straight trace down from the supply pad to a via (0.85 mm long at C44, 1.4 mm at C45, the nearest point clear of USB D+); C44's ground via is straight below its pad, C45's is beside its pad.

Vias on the LED net in this area are all 0.6 mm; front traces 0.4 mm; inner track 0.5 mm.

**Bottom row, done the same day.** D20, D21, D22 and D24 with C46, C47, C48 and C50 now use the same one-via pattern, mirrored (the via sits on the inner track at y = 109.49, between the LED above and its capacitor). Before this, C46–C48 hung off one 25 mm front-layer trace with a single via, and C50 had its own via and inner-layer stub. The inner track along the bottom was re-laid as one clean line; it keeps its dip to y = 108.99 between x = 33 and 39 mm, which clears a ground via and the USB data via there, and it stays at y = 109.49 (not a rounder number) because that is exactly the clearance limit from a ground via at (23.7, 108.7).

**D23 is the bottom row's exception.** Its ground pad could not get a via of its own (the USB connector's traces are underneath), so its ground trace runs up through the middle of the LED to the capacitor's ground. That trace occupies the spot where the shared via would go. D23 and C49 therefore each have their own via on the inner track, one either side.

**Side columns: a different pattern, by necessity.** The columns (D17–D19 on the right, D25–D27 on the left) cannot use the one-via pattern. In the rows the capacitor sits on the same side of the LED as the LED's supply pad, so one via between them serves both. In the columns the LEDs keep the same rotation but the capacitor is below the LED, which puts it on the opposite side from the supply pad, and the gap through the middle of each LED is already used by the data line going to the next LED. So each column pair keeps two vias: one beside the LED's supply pad on the inner track, and one at the capacitor with a short inner-layer link to that track. The three pairs in each column already matched each other. On 2026-11-21 their front traces were only tidied (odd angles replaced with 45° bends, ends put on pad and via centres). Making the columns single-via would mean rotating the LEDs or capacitors and re-laying the data chain past the camera and USB traces on the right side; that was judged not worth it.

**Inner-layer track in the columns re-laid (same day).** The LED track on the inner layer in both columns had been built up from overlapping pieces, duplicates of different widths and segments a tenth of a millimetre long. It was replaced with single clean lines, all horizontal, vertical or 45°:

- *Left:* 0.8 mm from the top down to D27's via at (4.1, 74.0), then 0.6 mm: a jog west to x = 3.06 (to pass C53's ground via), back to x = 4.284 past D26 and D25, and on to x = 5.384, where it meets the feed from the LED regulator. The regulator's feed now joins the diagonal to the bottom row at (7.605, 103.603) instead of running beside it.
- *Right:* 0.5 mm from D17's via straight down x = 87.55 and across to D19's via. D18's via sits 0.1 mm west of that line because it cannot move: 0.1 mm further east it is too close to a camera data trace on the front.
- *Capacitor links:* each capacitor's via has one straight 0.4 mm link at 45° to the main track.

**Ears, same day.** The inner track up each ear was a chain of separate pieces that stopped about 0.1 mm short of each via and relied on overlapping the via's pad; the left ear also had a 12 mm duplicate run lying on top of the main one. Each ear is now a single straight 45° line that passes through every via (within 0.03 mm of each centre), with a short vertical lead-in at the base. D3's via in the left ear sits about 1 mm off that line and keeps a short straight link to it.

