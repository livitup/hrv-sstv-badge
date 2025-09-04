# DEFCON SSTV Badge Assembly Guide

## ⚠️ Important Safety Information

**READ THIS FIRST BEFORE ASSEMBLY**

### Safety Requirements
- 🔥 **Soldering Safety**: Use proper ventilation, temperature-controlled iron
- ⚡ **ESD Protection**: Use anti-static wrist strap and ESD-safe work surface  
- 🔋 **Battery Safety**: Li-Po batteries require careful handling - no puncturing, overheating, or short circuits
- 📡 **RF Safety**: Completed badge transmits RF energy - follow amateur radio safety guidelines
- 🧪 **Chemical Safety**: Flux and cleaning solvents require ventilation

### Legal Requirements
- 📜 **Amateur License Required**: You must hold a valid amateur radio license to operate this device
- 🌍 **Local Regulations**: Ensure compliance with your local amateur radio regulations
- 📢 **Station Identification**: Configure your callsign before first use

---

## 🛠️ Tools and Equipment Required

### Essential Tools
| Tool | Purpose | Notes |
|------|---------|-------|
| **Soldering Iron** | Component assembly | Temperature controlled, 300-350°C |
| **Solder** | 0.6-0.8mm diameter | 63/37 or lead-free SAC305 |
| **Flux** | Clean solder joints | No-clean or rosin flux |
| **Tweezers** | SMD component placement | Fine-tip, ESD-safe |
| **Magnifying Glass** | Visual inspection | 2-5x magnification |
| **Multimeter** | Testing and troubleshooting | Basic continuity/voltage |

### Recommended Tools  
| Tool | Purpose | Notes |
|------|---------|-------|
| **Hot Air Station** | SMD rework | Temperature and airflow control |
| **Solder Paste** | SMD assembly | Type 3 or 4, SAC305 recommended |
| **Stencil** | Paste application | 3D printed or laser-cut |
| **Pick and Place Tweezers** | Precise component placement | Vacuum pickup preferred |
| **Oscilloscope** | Signal verification | Basic 2-channel scope adequate |
| **SMA Torque Wrench** | Antenna connector | Prevent over-tightening |

### Consumables Needed
- **Isopropyl Alcohol** (99%) - PCB cleaning
- **Cotton Swabs** - Flux removal
- **Kapton Tape** - Heat protection during rework
- **Solder Wick** - Mistake correction
- **ESD Bags** - Component storage

---

## 📦 Kit Contents Verification

Before starting assembly, verify you have all components from the [BOM](bom.md):

### Main Components Checklist
- [ ] PCB (1x) - Check for damage, correct revision
- [ ] RP2350 MCU (1x) - Verify package type (QFN-60)
- [ ] SA818 Module Socket (1x) - 16-pin DIP socket
- [ ] 2.4" TFT Display (1x) - ST7789 controller  
- [ ] OV2640 Camera Module (1x) - With ribbon cable
- [ ] MicroSD Card Slot (1x) - Push-push type
- [ ] USB-C Connector (1x) - 16-pin SMD type
- [ ] SMA Connector (1x) - Female, PCB mount
- [ ] 2000mAh Li-Po Battery (1x) - With JST connector
- [ ] SAO Headers (2x) - 2x3 pin headers

### IC Components Checklist  
- [ ] MCP73871 Battery Charger (1x)
- [ ] LC709203F Fuel Gauge (1x)
- [ ] AMS1117-3.3 Regulator (1x)
- [ ] WM8960 Audio Codec (1x)
- [ ] W25Q128 SPI Flash (1x)

### Passive Components
- [ ] Resistors - Various values (see BOM)
- [ ] Capacitors - Ceramic and tantalum (see BOM)
- [ ] Inductors/Ferrite beads (2x)
- [ ] 12MHz Crystal (1x)
- [ ] LEDs - Various colors (6x)

### Mechanical Components
- [ ] Tactile Switches (6x) - Various sizes
- [ ] Slide Switches (3x) - SPDT type
- [ ] DIP Switch (1x) - 2-position

---

## 🔧 Assembly Sequence

### Phase 1: PCB Inspection and Preparation

1. **Visual Inspection**
   ```
   Check PCB for:
   ├── Physical damage (cracks, chips)
   ├── Solder mask defects
   ├── Drill hole quality
   ├── Silkscreen legibility
   └── Copper trace continuity (if visible)
   ```

2. **Electrical Testing**
   ```
   Verify with multimeter:
   ├── No shorts between power rails
   ├── Ground plane continuity
   ├── Test point accessibility
   └── Via connectivity (spot check)
   ```

3. **PCB Cleaning**
   - Clean with isopropyl alcohol and lint-free cloth
   - Remove any manufacturing residue
   - Ensure completely dry before proceeding

### Phase 2: SMD Component Assembly

**🎯 Assembly Order: Smallest to Largest Components**

#### Step 1: Passive Components (Resistors, Capacitors)
```
Order of assembly:
1. 0805 Resistors (20x)
2. 0805 Ceramic Capacitors (20x)  
3. 1206 Tantalum Capacitors (5x)
4. Ferrite Beads (2x)
```

**Assembly Tips:**
- Apply solder paste with stencil or syringe
- Use tweezers to place components accurately
- Check component orientation (tantalum caps are polarized!)
- Reflow with hot air station or carefully with iron

#### Step 2: Crystal and Small ICs
```
Assembly sequence:
1. 12MHz Crystal (Y1) - Check orientation
2. MOSFETs (4x) - Note pin 1 orientation  
3. LC709203F Fuel Gauge - WDFN-6 package
```

#### Step 3: Major ICs
```
Assembly sequence:
1. W25Q128 SPI Flash (SOIC-8)
2. AMS1117-3.3 Regulator (SOT-223)
3. MCP73871 Battery Charger (QFN-20)
4. WM8960 Audio Codec (QFN-32) 
5. RP2350 MCU (QFN-60) - MOST CRITICAL!
```

**⚠️ RP2350 Assembly Critical Points:**
- Use plenty of flux
- Ensure all pads are clean and tinned
- Check for bridges under microscope
- Test power rails before proceeding

#### Step 4: LEDs and Indicators
```
Install in order:
1. Status LEDs (6x) - Check polarity!
2. Power LED (always on type)
```

### Phase 3: Through-Hole Components

#### Step 1: Connectors
```
Install order:
1. USB-C Connector (J1) - Secure mechanical tabs first
2. MicroSD Socket (J2) - Check orientation
3. SMA Connector (J3) - Use torque wrench
4. SAO Headers (J4, J5) - Ensure straight alignment
```

#### Step 2: User Interface
```
Install order:
1. SA818 Socket (U8) - 16-pin DIP socket
2. Tactile Switches (SW3-SW8) - Check actuator height
3. Slide Switches (SW1, SW2, SW9) - Test operation
```

#### Step 3: Display and Camera
```
Final assembly:
1. Camera Module (U7) - Handle ribbon cable carefully
2. TFT Display (U6) - May require standoffs
3. Speaker (SP1) - Optional component
```

---

## ⚡ Power-Up and Initial Testing

### Pre-Power Checks
```
Before applying power, verify:
├── No component bridges or shorts
├── Correct component orientation
├── All solder joints complete
├── No loose components or debris
└── Battery disconnected initially
```

### Power Rail Testing
1. **Connect USB-C** (no battery)
2. **Check voltages:**
   - USB VBUS: 5V ±0.25V
   - 3.3V Rail: 3.3V ±0.1V
   - RP2350 power pins: 3.3V
3. **Current consumption:** <50mA without SA818

### Battery Connection
1. **Connect Li-Po battery** (with power switch OFF if present)
2. **Verify charging circuit:**
   - Orange LED = charging
   - Green LED = charge complete
3. **Check battery voltage:** 3.7-4.2V typical

### Basic Functionality Test
```
Test sequence:
1. Power LED illuminates
2. RP2350 boots (activity on test points)
3. Display shows backlight/initialization
4. Button presses register
5. SD card detect works
```

---

## 🔧 Advanced Assembly Tips

### SMD Soldering Techniques

#### Hot Air Reflow
```
Recommended settings:
├── Temperature: 250-300°C
├── Air flow: Low-medium
├── Nozzle: 3-5mm diameter
└── Time: 30-60 seconds per component
```

#### Hand Soldering SMD
1. **Tin one pad** with solder
2. **Place component** with tweezers  
3. **Tack down** one side
4. **Check alignment** and adjust if needed
5. **Solder remaining** pins/pads

#### QFN Package Soldering (RP2350, Audio Codec)
```
Special techniques required:
├── Use plenty of flux
├── Drag soldering for fine pitch pins  
├── Check center pad thermal connection
├── Use microscope for inspection
└── Test continuity on all pins
```

### Common Assembly Issues

| Problem | Symptoms | Solution |
|---------|----------|----------|
| **Cold Solder Joint** | Intermittent connection | Reheat with fresh flux |
| **Solder Bridge** | Short circuit | Remove with wick/flux |
| **Component Tombstone** | One end lifted | Reheat both ends simultaneously |
| **Missing Solder** | No connection | Add flux and solder |
| **Wrong Orientation** | Circuit doesn't work | Check polarity markings |

### QC Inspection Points
```
Critical checkpoints:
├── Power rail voltages correct
├── No shorts between adjacent pins
├── All components seated properly  
├── Correct component orientations
├── Clean solder joints (no flux residue)
├── Mechanical connections secure
└── Test points accessible
```

---

## 🧪 Functional Testing Procedures

### Basic System Test
1. **Power-on Self Test**
   - All LEDs flash in sequence
   - Display shows boot screen
   - Buttons respond to presses

2. **Communication Tests**
   - I2C devices respond (fuel gauge, audio codec)
   - SPI devices respond (flash, display, SD card)
   - UART communication with SA818

3. **Peripheral Tests**
   - Camera captures images
   - Display shows images correctly
   - Audio codec produces/receives signals
   - SD card reads/writes data

### Radio Frequency Testing
**⚠️ Requires Amateur Radio License and Test Equipment**

1. **Power Output Test**
   - Connect RF power meter to SMA
   - Measure output power (should be ~1W)
   - Check for spurious emissions

2. **Frequency Accuracy**
   - Verify transmit frequency within ±2.5kHz
   - Check frequency stability over temperature

3. **SSTV Signal Quality**
   - Generate test patterns
   - Verify proper timing and levels
   - Test with standard SSTV software

---

## 🔧 Troubleshooting Guide

### Power Issues
| Symptom | Possible Cause | Check/Fix |
|---------|----------------|-----------|
| No power | Battery dead, USB issue | Check voltages, connections |
| Won't charge | Charger IC problem | Verify MCP73871 connections |
| High current | Short circuit | Check power rails with meter |
| Unstable power | Regulator oscillation | Check decoupling capacitors |

### Communication Issues
| Symptom | Possible Cause | Check/Fix |
|---------|----------------|-----------|
| Display blank | SPI connection | Check wiring, try different mode |
| Camera not detected | I2C issue | Verify pullups, check address |
| SD card not working | SPI timing | Check clock speed, connections |
| SA818 not responding | UART config | Verify baud rate, levels |

### RF Issues
| Symptom | Possible Cause | Check/Fix |
|---------|----------------|-----------|
| No RF output | SA818 not powered | Check power, PTT control |
| Low power | Antenna mismatch | Check SWR, antenna connection |
| Distorted audio | Audio levels | Adjust codec gain settings |
| Won't receive | Frequency error | Check SA818 programming |

### Assembly Issues
| Symptom | Possible Cause | Check/Fix |
|---------|----------------|-----------|
| Component won't stay | Poor solder joint | Reflow with fresh flux |
| Intermittent operation | Cold joint | Reheat suspected connections |
| Wrong values | Component mix-up | Verify part numbers |
| Physical damage | Handling error | Replace damaged parts |

---

## 📋 Final Quality Control

### Pre-Delivery Checklist
```
Complete system verification:
├── ✓ All components installed correctly
├── ✓ No solder bridges or cold joints  
├── ✓ Power consumption within spec
├── ✓ All user interfaces functional
├── ✓ Display clarity and alignment
├── ✓ Camera focus and image quality
├── ✓ Audio levels appropriate
├── ✓ RF output power and frequency
├── ✓ Battery charging working
├── ✓ SD card read/write functional
├── ✓ Firmware loaded and configured
└── ✓ Callsign programmed
```

### Documentation Package
```
Include with completed badge:
├── Assembly documentation (this guide)
├── User manual and operating instructions
├── Schematic and component layout
├── Firmware source code and binaries  
├── Test results and calibration data
├── Warranty and support information
└── Amateur radio compliance statement
```

### Packaging for Delivery
- Clean PCB with isopropyl alcohol
- Protect display with removable film
- Include spare components (buttons, LEDs)
- Provide microSD card with sample images
- Package in anti-static bag
- Include programming cable if needed

---

## 🎓 Learning Resources

### Assembly Skills Development
- **IPC Standards** - Professional assembly guidelines
- **SMD Soldering Videos** - YouTube tutorials for techniques
- **ESD Training** - Proper handling of sensitive components
- **RF Safety** - FCC OET Bulletin 65 guidelines

### Amateur Radio Resources
- **SSTV Information** - ARRL SSTV guide and mode specifications
- **Digital Modes** - Understanding digital amateur radio
- **Frequency Coordination** - Local repeater and simplex frequencies  
- **License Study** - Resources for upgrading amateur license

### Project Development
- **KiCAD Tutorials** - PCB design and layout
- **Embedded Programming** - RP2350 and Pico SDK
- **Git and GitHub** - Version control for collaborative projects
- **Open Source Hardware** - Community and licensing

---

## ⚠️ Important Notes

### Safety Reminders
- Always wear safety glasses when soldering
- Use adequate ventilation for fumes
- Keep fire extinguisher nearby when working with batteries
- Ground yourself and work surface to prevent ESD damage
- Never leave soldering iron unattended

### Legal Reminders  
- Amateur radio license required for operation
- Respect frequency allocations and band plans
- Proper station identification required
- Follow local RF exposure guidelines
- FCC rules apply to all amateur radio operations

### Project Support
- Check GitHub issues for known problems
- Join community discussions for help
- Document any improvements or modifications
- Share your build experience with others
- Consider contributing back to the project

---

**🎉 Congratulations on completing your DEFCON SSTV Badge assembly!**

*Now you're ready to capture, encode, and share images through the magic of amateur radio. See you on the airwaves at DEFCON!*

**73s!** 📡

---

*Assembly guide version 1.0 - Please report errors or suggestions via GitHub issues*