# DEFCON SSTV Badge Hardware Design

This directory contains all hardware design files for the DEFCON SSTV Badge, including KiCAD schematics, PCB layouts, and production files.

## 📁 Directory Structure

```
hardware/
├── kicad/                     # KiCAD project files
│   ├── defcon-sstv-badge.kicad_pro    # Main project file
│   ├── defcon-sstv-badge.kicad_sch    # Schematic
│   ├── defcon-sstv-badge.kicad_pcb    # PCB layout
│   ├── sym-lib-table                  # Symbol library table
│   ├── fp-lib-table                   # Footprint library table
│   ├── libraries/                     # Custom symbols and footprints
│   │   ├── defcon-sstv.kicad_sym     # Custom symbols
│   │   └── defcon-sstv.pretty/       # Custom footprints
│   └── plots/                         # Generated plots and images
├── gerbers/                   # Production files
│   ├── defcon-sstv-badge-gerbers.zip # Fabrication package
│   ├── defcon-sstv-badge-pos.csv     # Pick and place file
│   └── defcon-sstv-badge-bom.csv     # Assembly BOM
└── 3d-models/                 # Custom 3D models
    ├── SA818-module.wrl       # SA818 transceiver 3D model
    └── custom-connectors/     # Custom connector models
```

## 🎯 Design Overview

### Key Specifications
- **PCB Size**: 120mm x 80mm (TV-shaped outline)
- **Layer Count**: 4 layers (signal/ground/power/signal)
- **Thickness**: 1.6mm standard FR4
- **Copper Weight**: 1oz (35μm) on all layers
- **Via Size**: 0.2mm drill, 0.4mm annular ring
- **Minimum Trace**: 0.1mm (4 mil)
- **Minimum Spacing**: 0.1mm (4 mil)

### Design Features
- **Retro TV aesthetic** with rounded CRT-style outline
- **Dual SAO connectors** positioned as "TV antennas"
- **Component placement** optimized for neck-worn comfort
- **RF section isolation** from digital circuits
- **Battery placement** for balanced weight distribution
- **Status LEDs** positioned for visibility without blinding

## 🔧 KiCAD Project Setup

### Prerequisites
- **KiCAD 7.0 or later** (required for RP2350 symbols)
- **Git LFS** (for 3D models and large files)
- **KiCAD libraries** (standard libraries should be up to date)

### Opening the Project
1. Clone the repository with LFS support:
   ```bash
   git clone https://github.com/[username]/defcon-sstv-badge.git
   git lfs pull
   ```

2. Open KiCAD and load the project:
   - File → Open Project
   - Navigate to `hardware/kicad/`
   - Select `defcon-sstv-badge.kicad_pro`

3. Update library paths if needed:
   - Preferences → Manage Symbol Libraries
   - Preferences → Manage Footprint Libraries

### Custom Libraries
The project includes custom components not found in standard KiCAD libraries:

| Component | Library | Notes |
|-----------|---------|-------|
| RP2350 | defcon-sstv.kicad_sym | Latest pinout and specifications |
| SA818 Module | defcon-sstv.kicad_sym | Accurate pin mapping |
| Badge outline | defcon-sstv.pretty | TV-shaped PCB outline |
| SAO connectors | defcon-sstv.pretty | Badgelife standard SAO |

## 📐 PCB Layout Guidelines

### Layer Stack-Up
```
Layer 1: Component placement and high-speed signals
Layer 2: Ground plane (solid pour, minimal breaks)
Layer 3: Power plane (3.3V with analog/digital splits)  
Layer 4: Return paths and low-speed signals
```

### Design Rules
- **Impedance Control**: 50Ω single-ended, 100Ω differential
- **Via Stitching**: Every 5mm on power planes
- **Thermal Relief**: 0.2mm spoke width for hand soldering
- **Solder Mask**: Green, matte finish
- **Silkscreen**: White, legible component references

### Critical Routing
- **SA818 to SMA**: 50Ω controlled impedance, minimal vias
- **Crystal Oscillator**: Guard traces, ground pour isolation
- **Camera DVP**: Length-matched traces ±0.1mm
- **Power Distribution**: 0.3mm minimum trace width for 500mA
- **I2S Audio**: Keep away from switching circuits

## 📋 Bill of Materials (BOM)

The complete BOM is available in [../docs/bom.md](../docs/bom.md). Key sourcing notes:

### Primary Suppliers
- **JLCPCB/LCSC**: PCB fabrication and basic components
- **DigiKey**: Premium components, fast shipping
- **Mouser**: Alternative source for premium parts
- **AliExpress**: SA818 modules, displays (longer lead times)

### Critical Components
- **RP2350**: Latest stock - check availability at major distributors
- **SA818 Modules**: Order both VHF (-V) and UHF (-U) versions
- **2.4" Display**: Verify ST7789 controller compatibility
- **LiPo Battery**: Ensure proper connector and protection circuit

## 🏭 Manufacturing Files

### Gerber Generation
The gerbers are generated with these specifications:
- **Format**: RS-274X extended gerber format
- **Units**: Millimeters
- **Precision**: 4.6 format (μm precision)
- **Drill**: Excellon format with plated/non-plated distinction

### Assembly Files
- **Pick and Place**: CSV format compatible with JLCPCB/PCBWAY
- **Component Rotation**: Verified for automated placement
- **BOM Format**: Includes manufacturer part numbers and alternates

### Quality Standards
- **IPC-A-610**: Class 2 (standard electronic products)
- **IPC-2221**: Generic standard for PCB design
- **IPC-6012**: Qualification and performance specification

## 🧪 Design Validation

### Electrical Rules Check (ERC)
All schematic violations must be resolved or justified:
- No unconnected pins (except unused GPIO)
- Power supply sequencing verified
- Pin types compatible (output to input, etc.)

### Design Rules Check (DRC)  
Zero DRC violations allowed for production:
- Minimum trace/space rules
- Via size and placement
- Courtyard overlap detection
- Silkscreen clarity

### 3D Visualization
Use KiCAD 3D viewer to verify:
- Component clearances and fit
- Connector accessibility  
- Mechanical interference checks
- Visual appearance validation

## 📊 Simulation and Analysis

### Signal Integrity
Critical nets analyzed with external tools:
- **RF Path**: S-parameters and return loss
- **Crystal Circuit**: Oscillation startup analysis  
- **Power Distribution**: DC drop and AC impedance
- **High-Speed Digital**: Eye diagrams and timing

### Thermal Analysis
- **SA818**: ~2W dissipation during TX
- **RP2350**: ~0.4W at full load
- **Display Backlight**: ~0.5W at full brightness
- **Thermal Relief**: Adequate copper pour for heat spreading

## 🔄 Version Control and Collaboration

### Git Workflow
- **Main branch**: Stable, tested designs only
- **Feature branches**: Individual design changes
- **Release tags**: Mark production-ready versions
- **Commit messages**: Clear description of changes

### File Management
- **Large files**: Use Git LFS for 3D models and plots  
- **Generated files**: Exclude build outputs from git
- **Backups**: KiCAD creates .bak files automatically

### Collaboration Guidelines
- **Lock files**: Coordinate when multiple people editing
- **Library changes**: Document and test thoroughly
- **Design reviews**: Required for major changes
- **Documentation**: Update README for significant changes

## 📈 Design Metrics

### Electrical Performance
| Parameter | Target | Achieved | Notes |
|-----------|---------|----------|-------|
| Power Consumption | <200mA avg | TBD | Excluding TX |
| TX Power Output | 1W ±10% | TBD | At antenna connector |
| Frequency Accuracy | ±2.5kHz | TBD | Crystal dependent |
| Battery Life | 8-10 hours | TBD | Moderate use |

### Physical Specifications
| Parameter | Value | Notes |
|-----------|-------|-------|
| PCB Dimensions | 120 x 80mm | TV-shaped outline |
| Component Height | <15mm max | Clearance for wearing |
| Total Weight | ~150g | Including battery |
| Mounting Points | 4x M3 holes | For lanyard attachment |

## 🚨 Known Issues and Errata

### Revision A (Prototype)
- None yet - this is the initial design

### Design Considerations
- **SA818 Heat**: Module generates heat during TX - thermal pad recommended
- **Battery Placement**: Center of gravity optimized for comfortable wearing
- **Antenna Connector**: SMA chosen for amateur radio compatibility
- **Display Protection**: Consider protective film for harsh environments

## 🤝 Contributing to Hardware Design

### Making Changes
1. **Create feature branch** from main
2. **Make changes** in KiCAD
3. **Update documentation** if needed
4. **Generate new plots/gerbers** if required
5. **Test thoroughly** with DRC/ERC
6. **Submit pull request** with clear description

### Review Criteria
- ✅ Electrical correctness (no shorts, proper connections)
- ✅ Mechanical fit (component clearances, connector access)
- ✅ Manufacturability (DFM rules followed)
- ✅ Documentation updated
- ✅ No regressions in existing functionality

### Testing Requirements
- **Simulation**: Critical nets analyzed where applicable
- **Prototype**: Major changes require prototype validation
- **Documentation**: Changes must be clearly documented
- **Backward Compatibility**: Consider impact on existing builds

## 📞 Getting Help

### Hardware Design Questions
- **GitHub Issues**: Technical questions and bug reports
- **GitHub Discussions**: Design ideas and general discussion
- **KiCAD Forums**: General KiCAD usage questions
- **Amateur Radio Groups**: RF design and regulatory questions

### Design Review Process
Major hardware changes undergo community review:
1. **Proposal**: Describe change and rationale
2. **Discussion**: Community feedback and suggestions
3. **Prototype**: Build and test if needed
4. **Integration**: Merge after successful validation

---

**Ready to design the future of badgelife? Fire up KiCAD and let's build something amazing!** ⚡

*73 de [Your Callsign]* 📡