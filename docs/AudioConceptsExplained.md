## Audio Concepts Explained

*This section explains the filtering and biasing used in the audio circuits.*

### What is an Audio Signal?

Audio is a voltage that wiggles back and forth over time:

```
    Voltage
       │
   +1V ┤      ╱╲      ╱╲      ╱╲
       │     ╱  ╲    ╱  ╲    ╱  ╲
    0V ┤────╱────╲──╱────╲──╱────╲────  ← Audio: wiggling around zero
       │  ╱      ╲╱      ╲╱      ╲
   -1V ┤ ╱                        ╲
       └──────────────────────────────→ Time
```

- **Fast wiggles** = high pitch (treble)
- **Slow wiggles** = low pitch (bass)
- **Frequency** = wiggles per second (Hz)

SSTV uses tones from ~1200 Hz to ~2300 Hz.

### What is Filtering?

A **filter** decides which frequencies pass through and which get blocked.

**Low-pass filter:** Lets slow wiggles through, blocks fast wiggles.

```
    IN:  ~~~~ (1000 Hz)     →  FILTER  →  ~~~~ (passes!)
    IN:  ∿∿∿∿∿∿ (50000 Hz)  →  FILTER  →  ─── (blocked)
```

**Why filter?** The world is full of electrical noise at frequencies we don't care about. Filtering keeps the audio we want and blocks the garbage.

### How an RC Filter Works

The simplest low-pass filter is a resistor and capacitor:

```
        R (resistor)
    IN ───[████]───┬─── OUT
                   │
              C  ══╪══  (capacitor)
                   │
                  GND
```

A capacitor is like a tiny rechargeable battery. Fast wiggles try to charge/discharge it quickly — it "fights back" and shorts them to ground. Slow wiggles give it time to follow along and pass through.

The **cutoff frequency** (where blocking starts) is: `fc = 1 / (2π × R × C)`

### What is DC Blocking?

A **DC blocking capacitor** removes any constant voltage and only passes the wiggly AC signal:

```
    IN: ──────────── (constant 2V DC)     →  CAP  →  ─── (blocked)
    IN: ~~~~~ (AC wiggle)                 →  CAP  →  ~~~~~ (passes)
```

Capacitors pass changing signals but block steady ones.

### What is Bias?

The RP2350's ADC can only measure **positive voltages** (0V to 3.3V). But audio wiggles **above AND below zero**:

```
    Raw audio:                      With 1.65V bias added:

       +0.3V ┤    ╱╲                   2.0V ┤    ╱╲
             │   ╱  ╲                       │   ╱  ╲
        0V ──┼──╱────╲──              1.65V ┼──╱────╲── ← new center
             │ ╱      ╲                     │ ╱      ╲
       -0.3V ┤╱        ╲               1.3V ┤╱        ╲
             │                              │
        Negative half                  All positive now!
        is LOST (clipped)              ADC sees everything
```

**Bias** = adding a constant DC voltage (1.65V) to shift the signal into the ADC's visible range.

We create bias with a voltage divider (two equal resistors splitting 3.3V in half):

```
           VCC (3.3V)
              │
         R5 [47kΩ]
              │
              ├───→ 1.65V bias output
              │
         R6 [47kΩ]
              │
             GND
```

### What is Anti-Alias Filtering?

When the ADC samples audio (takes snapshots), frequencies faster than half the sample rate can "disguise" themselves as lower frequencies — creating ghost signals called **aliasing**.

The anti-alias filter removes these too-fast frequencies before sampling.

### Summary

| Concept | What it does | Analogy |
|---------|--------------|---------|
| **Low-pass filter** | Blocks high frequencies | Bouncer that only lets slow people in |
| **DC blocking cap** | Removes constant voltage, passes wiggles | Removes the floor, keeps only the bounce |
| **Voltage divider** | Shrinks voltage by a ratio | Water pressure reducer |
| **Bias** | Shifts AC signal into positive range | Raising a seesaw so both ends stay above ground |
| **Anti-alias filter** | Removes frequencies above sample rate/2 | Preventing ghosts in the recording |

---