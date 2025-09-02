# DEFCON SSTV Badge - Complete Schematic

## Overview
This schematic describes a SSTV (Slow Scan Television) badge for DEFCON with the following key features:
- SA818 VHF/UHF transceiver for radio communication
- RP2350 dual-core microcontroller for signal processing
- 2MP camera for image capture
- 2.4" color display for image viewing
- MicroSD storage for image archival
- USB-C charging with 2000mAh LiPo battery

---

## Power Management Section

### USB-C Charging Circuit
```
J1: USB-C Connector (GCT USB4105-GF-A or equivalent)
├── VBUS (pins 4,9) ──────────────── 5V input to charge controller
├── CC1 (pin A5) ─── 5.1kΩ ──── GND  │ USB-C current capability
├── CC2 (pin B5) ─── 5.1kΩ ──── GND  │ advertisement resistors
├── D+ (pin A6) ──────────────── NC   │ Data lines not used
├── D- (pin A7) ──────────────── NC   │ (power only design)
├── GND (pins A1,A12,B1,B12) ── GND  │
└── Shield ─────────────────── GND
```

### Li-Po Battery Charge Controller
```
U1: MCP73871-2CCI/ML (QFN-20 package)
┌─────────────────────────────────────┐
│                                     │
│  VIN (1)  ←─── USB VBUS             │
│  VSS (2)  ←─── GND                  │
│  STAT1 (3) ──→ LED1 (Orange)        │ Charging indicator
│  STAT2 (4) ──→ LED2 (Green)         │ Charge complete
│  VBAT (5) ←──→ Battery positive      │
│  THERM (6) ─── 10kΩ ──── GND        │ Temperature sensing
│  PROG (7) ──── 2kΩ ──── GND         │ 1A charge current
│  VDD (8) ←─── VIN                   │
│                                     │
└─────────────────────────────────────┘

Associated Components:
- C1: 10µF, 16V (VBUS decoupling)
- C2: 4.7µF, 16V (VDD decoupling) 
- C3: 4.7µF, 16V (VBAT decoupling)
- LED1: Orange LED + 1kΩ resistor (charge status)
- LED2: Green LED + 1kΩ resistor (charge complete)
```

### Battery and Fuel Gauge
```
BT1: 2000mAh Li-Po Battery (3.7V nominal)
├── Positive ──→ MCP73871 VBAT, LC709203F VDD
└── Negative ──→ GND

U2: LC709203F Battery Fuel Gauge (WDFN-6)
┌─────────────────────────────────────┐
│  VDD (1) ←─── Battery positive       │
│  SDA (2) ←──→ RP2350 GPIO2          │ I2C data
│  SCL (3) ←──→ RP2350 GPIO3          │ I2C clock
│  ALCC (4) ─── 1kΩ ─── VDD           │ Alarm output config
│  TSENSE1 (5) ─ 10kΩ NTC ─ GND       │ Temperature sensing
│  VSS (6) ←─── GND                   │
└─────────────────────────────────────┘
```

### 3.3V System Regulator
```
U3: AMS1117-3.3 (SOT-223 package)
┌─────────────────────────────────────┐
│  VIN ←─── Battery positive           │
│  GND ←─── System GND                │
│  VOUT ──→ 3.3V system rail          │
└─────────────────────────────────────┘

Decoupling:
- C4: 22µF, 16V (input side)
- C5: 22µF, 16V (output side)
```

---

## Raspberry Pi RP2350 Microcontroller

### Main MCU
```
U4: RP2350 (QFN-60 or QFP-64 package)

Power Connections:
├── IOVDD (multiple pins) ──→ 3.3V
├── DVDD (multiple pins) ───→ 3.3V  
├── ADC_AVDD ──────────────→ 3.3V (filtered)
├── USB_VDD ──────────────→ 3.3V
└── VSS/GND (multiple) ───→ GND

Each power pin has 100nF decoupling capacitor to GND
```

### Clock Generation
```
Y1: 12MHz Crystal (HC49S package)
├── Pin 1 ─── 22pF ─── GND
├── Pin 1 ──→ RP2350 XI (pin 7)
├── Pin 2 ─── 22pF ─── GND
└── Pin 2 ──→ RP2350 XO (pin 8)
```

### Boot Flash Memory
```
U5: W25Q128JVSIQ (16MB SPI Flash, SOIC-8)
┌─────────────────────────────────────┐
│  CS (1) ←──── RP2350 GPIO1          │
│  DO (2) ────→ RP2350 GPIO4 (MISO)   │
│  WP (3) ────── 3.3V (disabled)      │
│  GND (4) ───── GND                  │
│  DI (5) ←──── RP2350 GPIO7 (MOSI)   │
│  CLK (6) ←─── RP2350 GPIO6 (SCLK)   │
│  HOLD (7) ──── 3.3V (disabled)      │
│  VCC (8) ───── 3.3V                 │
└─────────────────────────────────────┘
```

### GPIO Assignments
```
RP2350 GPIO Mapping:
┌─────┬────────────────────────────────────┐
│ Pin │ Function                           │
├─────┼────────────────────────────────────┤
│  0  │ Boot select (10kΩ pullup to 3.3V) │
│  1  │ SPI Flash CS                       │
│  2  │ I2C SDA (4.7kΩ pullup)            │
│  3  │ I2C SCL (4.7kΩ pullup)            │
│  4  │ SPI Flash MISO / SA818 UART RX    │
│  5  │ SA818 UART TX                      │
│  6  │ SPI Flash SCLK                     │
│  7  │ SPI Flash MOSI                     │
│  8  │ Camera PCLK                        │
│  9  │ Camera VSYNC                       │
│ 10  │ Camera HREF                        │
│ 11  │ Camera Data D7 (MSB)               │
│ 12  │ Camera XCLK (PWM output)           │
│ 13  │ Camera RESET                       │
│ 14  │ Camera PWDN                        │
│ 15  │ D-pad Center button                │
│ 16  │ Display CS                         │
│ 17  │ Display SCLK                       │
│ 18  │ Display MOSI                       │
│ 19  │ Display DC (Data/Command)          │
│ 20  │ Display RESET                      │
│ 21  │ Display Backlight (PWM)            │
│ 22  │ D-pad Up button                    │
│ 23  │ D-pad Down button                  │
│ 24  │ D-pad Left button                  │
│ 25  │ D-pad Right button                 │
│ 26  │ Photo Snap button                  │
│ 27  │ SA818 PTT (Push-to-Talk)           │
│ 28  │ SA818 PD (Power Down)              │
│ 29  │ MicroSD CS                         │
│ 30  │ MicroSD SCLK                       │
│ 31  │ MicroSD MOSI                       │
│ 32  │ MicroSD Card Detect                │
│ 33  │ Band Select Switch (VHF/UHF)       │
│ 34  │ I2S BCLK (to audio codec)          │
│ 35  │ I2S LRCLK (to audio codec)         │
│ 36  │ I2S DIN (from audio codec)         │
│ 37  │ I2S DOUT (to audio codec)          │
│ 38  │ TX Status LED                      │
│ 39  │ RX Status LED                      │
│ 40  │ SD Card Status LED                 │
│ 41  │ SAO GPIO1                          │
│ 42  │ SAO GPIO2                          │
└─────┴────────────────────────────────────┘

Note: GPIO0/MISO shared between boot flash and SA818 via multiplexing
```

---

## Display Section

### 2.4" TFT LCD Display
```
U6: 2.4" TFT LCD with ST7789 Controller
┌─────────────────────────────────────┐
│  VCC ←─── 3.3V                      │
│  GND ←─── GND                       │
│  CS ←──── RP2350 GPIO16             │
│  RESET ← RP2350 GPIO20              │
│  DC ←──── RP2350 GPIO19             │
│  MOSI ←── RP2350 GPIO18             │
│  SCK ←─── RP2350 GPIO17             │
│  LED ←─── Backlight control         │
└─────────────────────────────────────┘

Backlight Control Circuit:
Q1: 2N7002K (N-channel MOSFET, SOT-23)
├── Gate ←─── RP2350 GPIO21 (PWM)
├── Source ── GND  
└── Drain ──→ Display LED cathode

Display LED anode ─── 100Ω ─── 3.3V
```

---

## Camera Section

### OV2640 Camera Module
```
U7: OV2640 Camera Module (24-pin package)
┌─────────────────────────────────────┐
│ Power Rails:                        │
│  AVDD ←─── 3.3V (via ferrite bead)  │
│  DOVDD ←── 3.3V                     │
│  DVDD ←─── 3.3V                     │
│  AGND ←─── GND                      │
│  DGND ←─── GND                      │
│                                     │
│ Control Interface:                  │
│  SDA ←──→ RP2350 GPIO2 (I2C)        │
│  SCL ←──→ RP2350 GPIO3 (I2C)        │
│  RESET ← RP2350 GPIO13              │
│  PWDN ─── 10kΩ ─── GND              │
│                                     │
│ Image Data Interface:               │
│  XCLK ←── RP2350 GPIO12 (12MHz)     │
│  PCLK ───→ RP2350 GPIO8             │
│  VSYNC ──→ RP2350 GPIO9             │
│  HREF ───→ RP2350 GPIO10            │
│  D7 ─────→ RP2350 GPIO11            │
│  D6-D0 ── NC (8-bit mode)           │
└─────────────────────────────────────┘

Power Supply Filtering:
- L1: 10µH ferrite bead (AVDD supply)
- C6: 10µF || 100nF (AVDD decoupling)
- C7: 100nF (DOVDD decoupling)  
- C8: 100nF (DVDD decoupling)
```

---

## MicroSD Card Storage

### MicroSD Card Socket
```
J2: MicroSD Card Socket (Push-Push type)
┌─────────────────────────────────────┐
│  VDD ←─── 3.3V                      │
│  VSS ←─── GND                       │
│  CS ←──── RP2350 GPIO29             │
│  MOSI ←── RP2350 GPIO31             │
│  MISO ──→ RP2350 GPIO0 (shared)     │
│  SCK ←─── RP2350 GPIO30             │
│  CD ─────→ RP2350 GPIO32            │ Card detect
│  WP ─────→ NC                       │ Write protect
└─────────────────────────────────────┘

Card Detect: 10kΩ pullup on GPIO32
```

---

## SA818 Transceiver Section

### SA818 Module (Socketed)
```
U8: SA818-V (VHF) or SA818-U (UHF) Module
┌─────────────────────────────────────┐
│ Power:                              │
│  VCC ←─── 3.3V (via airplane switch)│
│  GND ←─── GND                       │
│                                     │
│ Control Interface:                  │
│  UART_TX ─→ RP2350 GPIO4            │
│  UART_RX ←─ RP2350 GPIO5            │
│  PTT ←───── RP2350 GPIO27           │
│  PD ←────── RP2350 GPIO28           │
│                                     │
│ Audio Interface:                    │
│  MIC+ ←─── Audio codec LOUT1        │
│  MIC- ←─── Audio codec ROUT1        │
│  SPK+ ───→ Audio codec LINPUT1      │
│  SPK- ───→ Audio codec RINPUT1      │
│                                     │
│ RF Interface:                       │
│  ANT ────→ J3 (SMA connector)       │
└─────────────────────────────────────┘

Band Select Configuration:
SW1: 2-position DIP switch
├── Position 1 ──→ RP2350 GPIO33 (10kΩ pullup)
│   └── ON = UHF mode, OFF = VHF mode
└── Position 2 ──→ Reserved for future use

Airplane Mode Control:
SW9: SPDT Slide Switch
├── Common ──→ SA818 VCC pin
├── NC ──────→ 3.3V rail (normal operation)
└── NO ──────→ GND (shutdown mode)
```

---

## Audio Processing Section

### Audio Codec
```
U9: WM8960 Audio Codec (QFN-32 package)
┌─────────────────────────────────────┐
│ Power:                              │
│  AVDD ←─── 3.3V (via ferrite bead)  │
│  DVDD ←─── 3.3V                     │
│  AGND ←─── GND                      │
│  DGND ←─── GND                      │
│                                     │
│ Control Interface (I2C):            │
│  SDA ←──→ RP2350 GPIO2              │
│  SCL ←──→ RP2350 GPIO3              │
│                                     │
│ Digital Audio Interface (I2S):      │
│  BCLK ←── RP2350 GPIO34             │
│  WCLK ←── RP2350 GPIO35             │
│  ADCDAT ─→ RP2350 GPIO36            │
│  DACDAT ←─ RP2350 GPIO37            │
│                                     │
│ Analog Audio Interface:             │
│  MICBIAS ─ 2.2µF ─ Microphone       │
│  LINPUT1 ←─ SA818 SPK+ (RX audio)   │
│  RINPUT1 ←─ SA818 SPK-              │
│  LOUT1 ──→ SA818 MIC+ (TX audio)    │
│  ROUT1 ──→ SA818 MIC-               │
│  SPK_LN ─→ Speaker negative         │
│  SPK_LP ─→ Speaker positive         │
└─────────────────────────────────────┘

Audio Supply Filtering:
- L2: 600Ω@100MHz ferrite bead (AVDD)
- C9: 10µF || 100nF (AVDD decoupling)
- C10: 100nF (DVDD decoupling)

Optional Speaker:
SP1: 8Ω, 0.5W Speaker
├── Positive ── WM8960 SPK_LP
└── Negative ── WM8960 SPK_LN

Audio Mute Control:
SW2: SPDT Slide Switch (on PCB edge)
├── Common ── WM8960 SPK_LP
├── NC ────── Speaker positive (muted)
└── NO ────── Speaker positive (enabled)
```

---

## User Interface Section

### Button Layout
```
Photo Snap Button:
SW3: 12mm Tactile Switch (B3F-4055 or similar)
├── Pin 1 ──→ RP2350 GPIO26
├── Pin 2 ──→ GND
└── Pull-up: 10kΩ resistor (GPIO26 to 3.3V)

D-Pad Button Cluster:
SW4: Up    ── RP2350 GPIO22 ── 10kΩ pullup ── 3.3V
SW5: Down  ── RP2350 GPIO23 ── 10kΩ pullup ── 3.3V  
SW6: Left  ── RP2350 GPIO24 ── 10kΩ pullup ── 3.3V
SW7: Right ── RP2350 GPIO25 ── 10kΩ pullup ── 3.3V
SW8: Center── RP2350 GPIO15 ── 10kΩ pullup ── 3.3V

All buttons: Other side connected to GND
Button type: 6x6mm tactile switches (B3F-4050)
```

### Control Switches
```
Airplane Mode Switch:
SW9: SPDT Slide Switch (SS12SDP2)
├── Common ──→ SA818 VCC
├── Position 1 ──→ 3.3V (normal operation)
└── Position 2 ──→ GND (radio disabled)

Audio Mute Switch:  
SW2: SPDT Slide Switch (SS12SDP2)
├── Common ──→ WM8960 SPK_LP
├── Position 1 ──→ Speaker + (audio enabled)
└── Position 2 ──→ NC (audio muted)

Band Select Switch:
SW1: 2-position DIP Switch (219-2LPST)
├── Position 1 ──→ RP2350 GPIO33 (10kΩ pullup)
│   └── Software reads: HIGH = UHF, LOW = VHF
└── Position 2 ──→ Reserved
```

---

## Status LED Section

### LED Indicators
```
Power LED (always on when powered):
LED3: Green LED (LTST-C170GKT)
├── Anode ── 1kΩ ── 3.3V
└── Cathode ──── GND

TX Status LED:
LED4: Red LED (LTST-C170CKT) 
├── Anode ── 1kΩ ── 3.3V
└── Cathode ── Q2 drain ── RP2350 GPIO38
Q2: 2N7002K MOSFET (source to GND)

RX Status LED:
LED5: Blue LED (LTST-C170TBKT)
├── Anode ── 1kΩ ── 3.3V  
└── Cathode ── Q3 drain ── RP2350 GPIO39
Q3: 2N7002K MOSFET (source to GND)

SD Card Status LED:
LED6: Yellow LED (LTST-C170YKT)
├── Anode ── 1kΩ ── 3.3V
└── Cathode ── Q4 drain ── RP2350 GPIO40  
Q4: 2N7002K MOSFET (source to GND)

Battery Charge LEDs:
LED1: Orange LED ── 1kΩ ── 3.3V ── MCP73871 STAT1
LED2: Green LED ── 1kΩ ── 3.3V ── MCP73871 STAT2
```

---

## SAO (Shitty Add-On) Connectors

### SAO Port Layout
```
J4, J5: 2x3 Pin Header (PEC03SAAN)

Standard SAO Pinout:
┌─────┬─────┬──────────────────────────┐
│ Pin │ Net │ Connection               │
├─────┼─────┼──────────────────────────┤
│  1  │ GND │ System ground            │
│  2  │ VCC │ 3.3V power              │
│  3  │ SDA │ RP2350 GPIO2 (I2C data) │
│  4  │ SCL │ RP2350 GPIO3 (I2C clock)│
│  5  │ GP1 │ RP2350 GPIO41           │
│  6  │ GP2 │ RP2350 GPIO42           │
└─────┴─────┴──────────────────────────┘

Physical placement: Positioned to look like "TV antennas"
on the badge's TV-shaped PCB outline
```

---

## RF Section

### Antenna Connection
```
J3: SMA Female Connector (142-0701-801)
├── Center pin ──→ SA818 ANT
├── Shield ─────→ GND
└── Mounting ───→ GND (4 places)

Antenna Requirements:
- VHF (SA818-V): 2m band (144-148 MHz) 
- UHF (SA818-U): 70cm band (420-450 MHz)
- SWR < 2:1 recommended for best performance
- Suggest rubber duck or telescoping whip
```

---

## Power Distribution Summary

### System Power Rails
```
Battery (3.7V nominal, 2000mAh)
    │
    ├── MCP73871 (charging control)
    │   └── USB-C (5V input)
    │
    └── AMS1117-3.3 (system regulator)
        └── 3.3V Rail Distribution:
            ├── RP2350 MCU (~30mA)
            ├── Display + backlight (~40mA)
            ├── Camera (when active, ~50mA)  
            ├── SA818 (30mA RX, 1.5A TX)
            ├── Audio codec (~15mA)
            ├── Flash memory (~5mA)
            ├── LEDs (~20mA total)
            └── Misc circuits (~15mA)

Total Current Budget:
- Idle: ~155mA (10+ hour operation)
- Active (no TX): ~205mA (8+ hour operation)
- Transmitting: ~1.7A (brief periods only)
```

### Decoupling Requirements
```
Power Supply Decoupling:
- Main power rails: 22µF tantalum + 100nF ceramic
- IC power pins: 100nF ceramic (0805 package)
- Analog supplies: Additional 10µF + ferrite bead
- High-frequency: 100nF close to switching circuits

Ground Plane Strategy:
- Dedicated ground plane (Layer 2)
- Power plane for 3.3V (Layer 3)  
- Analog/digital ground separation at audio codec
- Single-point ground connection to battery
```

---

## PCB Layout Guidelines

### Layer Stack (4-layer PCB)
```
Layer 1: Component placement and signals
Layer 2: Ground plane (solid pour)
Layer 3: Power plane (3.3V, split for analog)  
Layer 4: Additional signals and returns

Critical Routing:
- Crystal: < 10mm traces, ground guard
- SA818 to SMA: 50Ω controlled impedance
- Camera DVP: Length-matched parallel traces
- I2S audio: Keep away from switching circuits
- Power traces: Minimum 10 mil width for currents
```

### Component Placement Strategy
```
RF Section (top-left):
├── SA818 socket near SMA connector
├── Band select switch adjacent
└── TX/RX LEDs visible but not blinding

User Interface (center):  
├── Display prominent and protected
├── Photo button easily accessible
├── D-pad comfortable for thumb operation
└── Status LEDs clearly visible

Power Section (bottom):
├── USB-C connector on PCB edge
├── Battery connector internal
├── Charge LEDs near USB connector
└── Power switch accessible but protected

Add-ons (top-right):
├── SAO connectors as "TV antennas"
├── MicroSD slot accessible
└── Programming header (if included)
```

---

## Test Points and Debug Access

### Recommended Test Points
```
Power Rails:
- TP1: Battery positive
- TP2: 3.3V main rail  
- TP3: Ground reference
- TP4: USB VBUS

Critical Signals:
- TP5: RP2350 SWDIO (debug)
- TP6: RP2350 SWCLK (debug)
- TP7: SA818 UART TX
- TP8: SA818 UART RX
- TP9: I2C SDA
- TP10: I2C SCL

RF Monitoring:
- TP11: SA818 ANT (RF output)
- TP12: Audio codec output

All test points: 0.1" diameter, via-in-pad acceptable
```

### Programming Interface
```
Optional: Tag-Connect TC2030-IDC footprint
├── 1: 3.3V
├── 2: RP2350 SWDIO  
├── 3: GND
├── 4: RP2350 SWCLK
├── 5: GND  
└── 6: RP2350 RUN (reset)

Allows cable-less programming with TC2030 cable
```

---

## Design Notes and Considerations

### Signal Integrity
- Keep high-speed digital signals short and properly terminated
- Use ground guards around crystal oscillator  
- Separate analog and digital ground domains at audio codec
- Route I2S signals away from switching power circuits

### Thermal Management
- SA818 generates heat during transmit (~2W dissipation)
- Consider thermal pad under SA818 socket
- Adequate copper pour for heat spreading
- Keep temperature-sensitive components away from SA818

### EMC/EMI Considerations  
- Ferrite beads on power supplies to sensitive circuits
- Proper shielding between RF and digital sections
- Adequate ground plane coverage (>90%)
- Control impedance for high-frequency signals

### Mechanical Design
- PCB thickness: 1.6mm standard
- TV-shaped outline with rounded corners
- Mounting holes for badge attachment
- Component height considerations for comfortable wearing

---

*This completes the detailed schematic documentation for the DEFCON SSTV Badge. All connections, component values, and design considerations are specified for successful implementation.*