# GPIO Rewiring Checklist

Step-by-step checklist for reassigning U1 (RP2350B) GPIO connections in the KiCad schematic. All rewiring happens on the **MCU sheet** (mcu.kicad_sch). Other sheets use global labels that don't change names.

Reference: `docs/engineers-notebook/gpio-optimization.md` for rationale.

## Before You Start

- Save a backup of all .kicad_sch files
- Have `docs/u1-pinout.md` open for reference
- Work through one group at a time, checking off as you go

## Signals That DO NOT Move (no action needed)

These stay on the same GPIO — skip them:

| Signal | GPIO | Status |
|--------|------|--------|
| SPI_MISO | GPIO4 | No change |
| DISP_CS | GPIO5 | No change |
| CAM_D0 | GPIO8 | No change |
| CAM_D1 | GPIO9 | No change |
| CAM_D2 | GPIO10 | No change |
| CAM_D3 | GPIO11 | No change |
| CAM_D4 | GPIO12 | No change |
| CAM_D5 | GPIO13 | No change |
| CAM_D6 | GPIO14 | No change |
| CAM_D7 | GPIO15 | No change |
| CAM_PCLK | GPIO16 | No change |
| CAM_VSYNC | GPIO17 | No change |
| CAM_HREF | GPIO18 | No change |
| RX_AUDIO | GPIO40 | No change |

Also unchanged: USB_DM, USB_DP, SWCLK, SWDIO, RUN, all QSPI, crystal, VREG, power pins.

---

## Rewiring Checklist

Work through these groups in order. For each signal: disconnect the global label wire from the old GPIO pin on U1, and reconnect it to the new GPIO pin.

### Group 1: I2C Bus (move from top to right side)

- [ ] **I2C_SDA**: disconnect from GPIO0 → reconnect to **GPIO44**
- [ ] **I2C_SCL**: disconnect from GPIO1 → reconnect to **GPIO45**

### Group 2: I2S Audio (move to where I2C was)

- [ ] **I2S_LRCK**: disconnect from GPIO25 → reconnect to **GPIO1**
- [ ] **I2S_DIN**: disconnect from GPIO29 → reconnect to **GPIO2**
- [ ] **I2S_BCK**: disconnect from GPIO24 → reconnect to **GPIO3**

### Group 3: SPI Bus (shift from GPIO2-3 to GPIO6-7)

- [ ] **SPI_SCK**: disconnect from GPIO2 → reconnect to **GPIO6**
- [ ] **SPI_MOSI**: disconnect from GPIO3 → reconnect to **GPIO7**

### Group 4: Display Control (scatter to new homes)

- [ ] **DISP_DC**: disconnect from GPIO6 → reconnect to **GPIO19** (was no_connect, remove the NC marker first)
- [ ] **DISP_RST**: disconnect from GPIO7 → reconnect to **GPIO43**
- [ ] **DISP_BL**: disconnect from GPIO22 → reconnect to **GPIO29**
- [ ] **SD_CS**: disconnect from GPIO23 → reconnect to **GPIO46**

### Group 5: LED Chain (move to top side)

- [ ] **LED_DATA**: disconnect from GPIO47 → reconnect to **GPIO0**
- [ ] Mark **GPIO47** as no_connect (add NC marker)

### Group 6: Camera XCLK (new signal)

- [ ] Remove no_connect marker from **GPIO19** (if not already done in Group 4)
- [ ] **CAM_XCLK**: create new global label, connect to **GPIO20** (was SAO1_GPIO1)

### Group 7: SAO Connectors (rearrange)

- [ ] **SAO1_GPIO1**: disconnect from GPIO20 → reconnect to **GPIO27**
- [ ] **SAO1_GPIO2**: disconnect from GPIO21 → reconnect to **GPIO28**
- [ ] **SAO2_GPIO1**: disconnect from GPIO27 → reconnect to **GPIO41**
- [ ] **SAO2_GPIO2**: disconnect from GPIO28 → reconnect to **GPIO42** (was no_connect, remove NC marker first)

### Group 8: D-pad Switches (consolidate to bottom-left)

- [ ] **DPAD_UP**: disconnect from GPIO37 → reconnect to **GPIO21**
- [ ] **DPAD_DOWN**: disconnect from GPIO38 → reconnect to **GPIO22**
- [ ] **DPAD_LEFT**: disconnect from GPIO39 → reconnect to **GPIO23**
- [ ] **DPAD_RIGHT**: disconnect from GPIO26 → reconnect to **GPIO24**
- [ ] **DPAD_CENTER**: disconnect from GPIO41 → reconnect to **GPIO25**

### Group 9: Airplane Switch

- [ ] **AIRPLANE**: disconnect from GPIO43 → reconnect to **GPIO26**

### Group 10: SA818 Radio (consolidate to right side)

- [ ] **SA818_PTT**: disconnect from GPIO32 → reconnect to **GPIO33**
- [ ] **SA818_PD**: disconnect from GPIO33 → reconnect to **GPIO34**
- [ ] **SA818_HL**: disconnect from GPIO34 → reconnect to **GPIO35**
- [ ] **SA818_TX**: disconnect from GPIO31 → reconnect to **GPIO36**
- [ ] **SA818_RX**: disconnect from GPIO30 → reconnect to **GPIO37**
- [ ] **SA818_SQ**: disconnect from GPIO36 → reconnect to **GPIO38**
- [ ] **SA818_ID**: disconnect from GPIO35 → reconnect to **GPIO39**

**Warning:** SA818 signals are currently tangled (e.g., GPIO35 currently has SA818_ID but GPIO35's new signal is SA818_HL). Disconnect ALL seven SA818 labels first, then reconnect them all. Don't do them one at a time or you'll create conflicts.

### Group 11: PWR LED (move from upper-right to bottom)

- [ ] **PWR_R**: disconnect from GPIO44 → reconnect to **GPIO30**
- [ ] **PWR_G**: disconnect from GPIO45 → reconnect to **GPIO31**
- [ ] **PWR_B**: disconnect from GPIO46 → reconnect to **GPIO32**

---

## Post-Rewiring Verification

- [ ] Run **ERC** — should be clean (no unconnected pins, no missing connections)
- [ ] Verify no GPIO has two signals (check for duplicate connections)
- [ ] Verify GPIO19 has DISP_DC (not NC)
- [ ] Verify GPIO20 has CAM_XCLK (new label)
- [ ] Verify GPIO42 has SAO2_GPIO2 (was NC)
- [ ] Verify GPIO47 is NC (was LED_DATA)
- [ ] Count: 47 GPIOs assigned + 1 spare (GPIO47) = 48 total

## Quick Reference: Final GPIO Assignment

| GPIO | Signal | GPIO | Signal |
|------|--------|------|--------|
| 0 | LED_DATA | 24 | DPAD_RIGHT |
| 1 | I2S_LRCK | 25 | DPAD_CENTER |
| 2 | I2S_DIN | 26 | AIRPLANE |
| 3 | I2S_BCK | 27 | SAO1_GPIO1 |
| 4 | SPI_MISO | 28 | SAO1_GPIO2 |
| 5 | DISP_CS | 29 | DISP_BL |
| 6 | SPI_SCK | 30 | PWR_R |
| 7 | SPI_MOSI | 31 | PWR_G |
| 8-15 | CAM_D0-D7 | 32 | PWR_B |
| 16 | CAM_PCLK | 33 | SA818_PTT |
| 17 | CAM_VSYNC | 34 | SA818_PD |
| 18 | CAM_HREF | 35 | SA818_HL |
| 19 | DISP_DC | 36 | SA818_TX |
| 20 | CAM_XCLK | 37 | SA818_RX |
| 21 | DPAD_UP | 38 | SA818_SQ |
| 22 | DPAD_DOWN | 39 | SA818_ID |
| 23 | DPAD_LEFT | 40 | RX_AUDIO |
| | | 41 | SAO2_GPIO1 |
| | | 42 | SAO2_GPIO2 |
| | | 43 | DISP_RST |
| | | 44 | I2C_SDA |
| | | 45 | I2C_SCL |
| | | 46 | SD_CS |
| | | 47 | spare (NC) |
