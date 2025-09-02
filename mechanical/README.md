# DEFCON SSTV Badge Mechanical Design

This directory contains mechanical design files and accessories for the DEFCON SSTV Badge.

## 🎯 Badge Design Philosophy

As a DEFCON badge, this device is designed to be worn **without a traditional enclosure**. The PCB itself is the final product, optimized for:
- **Comfortable neck wearing** - Balanced weight distribution and rounded edges
- **Visual appeal** - Retro TV aesthetic with visible components as art
- **Durability** - Robust PCB design to withstand conference handling
- **Accessibility** - All user controls and connectors easily accessible

## 📐 Physical Specifications

### Dimensions and Weight
| Parameter | Value | Notes |
|-----------|-------|-------|
| **PCB Size** | 120mm × 80mm | TV-shaped outline with rounded corners |
| **Thickness** | ~15mm max | Including tallest components |
| **Weight** | ~150g | Including 2000mAh battery |
| **Balance Point** | Center-bottom | Optimized for comfortable wearing |

### Wearability Features
- **Rounded PCB edges** - No sharp corners that could snag or poke
- **Component placement** - Heavy components (battery, SA818) positioned for balance
- **Smooth back side** - Minimal components on reverse for comfort against clothing
- **Lanyard mounting** - 4x M3 mounting holes for secure attachment

## 📁 Directory Structure

```
mechanical/
└── assembly/              # Assembly-related mechanical files
    ├── alignment-jigs/    # 3D printable assembly aids
    ├── test-fixtures/     # Programming and testing jigs  
    ├── protective-accessories/ # Optional protective items
    └── mounting-solutions/     # Lanyard and wearing accessories
```

## 🛠️ Assembly Considerations

### PCB as Final Product
- **No case required** - The PCB is designed to be worn directly
- **Component protection** - Critical components positioned away from edges
- **Connector accessibility** - USB-C, SMA, and microSD easily accessible
- **Button placement** - Optimized for thumb operation when worn

### Weight Distribution
The badge is designed with weight balance in mind:
- **Battery placement** - Center-bottom for stability when hanging
- **Display position** - Upper area for easy viewing when worn
- **Heavy components** - SA818 and camera positioned to minimize swing

## 🔧 Assembly Aids and Jigs

### Recommended Assembly Tools
While not required, these mechanical aids can improve assembly quality:

#### Soldering Alignment Jig
- **Purpose**: Hold PCB steady during component placement
- **Material**: 3D printed PLA or PETG
- **Features**: Cutouts for component access, stable base

#### Programming Test Fixture  
- **Purpose**: Secure connection for firmware updates
- **Features**: Pogo pin contacts, consistent positioning
- **Benefit**: Reduces wear on debug connectors

#### Component Placement Guide
- **Purpose**: Ensure correct component orientation
- **Material**: Laser-cut acrylic or 3D printed
- **Features**: Component outlines, polarity indicators

*Note: 3D files for these jigs will be added as the design matures and assembly experience is gained.*

## 🎒 Wearing and Transportation

### Lanyard Attachment
- **Mounting holes**: 4x M3 holes in PCB corners
- **Hardware**: M3 screws and washers (included in BOM)
- **Lanyard types**: Standard conference lanyards compatible
- **Load rating**: PCB designed for >2kg static load

### Protective Considerations
While no enclosure is used, consider these protective measures:

#### Display Protection
- **Screen protector**: Standard phone screen protector can be cut to fit
- **Material**: Tempered glass or PET film
- **Application**: Clean installation prevents air bubbles

#### Component Guards (Optional)
- **Button protection**: 3D printed caps prevent accidental activation
- **Antenna guard**: Protective cover for SMA connector during transport
- **Camera lens**: Clear protective cover to prevent scratches

#### Storage and Transport
- **Anti-static bag**: Prevents ESD damage during storage
- **Foam padding**: Custom-cut foam for shipping protection
- **Component covers**: Protect delicate parts during assembly

## 📏 Mechanical Testing

### Durability Requirements
The badge must withstand typical conference conditions:

#### Drop Testing
- **Height**: 1 meter drop onto concrete
- **Orientations**: Face-down, back-down, edge impact
- **Criteria**: No component damage, continued operation

#### Wear Testing  
- **Duration**: 12+ hours continuous wearing
- **Comfort**: No pressure points or sharp edges
- **Balance**: Minimal swinging or rotation when walking

#### Environmental
- **Temperature**: 0°C to 40°C operation
- **Humidity**: Up to 80% relative humidity  
- **Vibration**: Walking, dancing, general conference activity

## 🔄 Design Evolution

### Feedback Integration
As badges are built and worn, mechanical improvements may include:
- **Comfort optimization** - Edge radius adjustments
- **Weight reduction** - Component selection and placement
- **Durability improvements** - Reinforcement of stress points
- **Accessory development** - Community-requested add-ons

### Version Tracking
- **Rev A**: Initial design (current)
- **Rev B+**: Incorporate user feedback and lessons learned

## 🤝 Community Contributions

### Accessory Development
The community is encouraged to develop and share:
- **Custom lanyards** - Themed designs for different cons
- **Mounting solutions** - Belt clips, magnetic mounts, stands
- **Protective gear** - Cases for specific use scenarios
- **Display stands** - For showcasing badges when not worn

### Sharing Guidelines
- **File formats**: STL for 3D printing, DXF for laser cutting
- **Documentation**: Include print settings, material recommendations
- **Testing**: Verify fit and function before sharing
- **Licensing**: Use compatible open-source licenses

## 📚 Reference Documents

For detailed assembly information, see:
- **[Assembly Guide](../docs/assembly-notes.md)** - Complete build instructions
- **[Hardware Design](../hardware/README.md)** - PCB specifications and design files
- **[BOM](../docs/bom.md)** - Complete parts list including mechanical hardware

## ⚠️ Safety Considerations

### Electrical Safety
- **Battery protection** - LiPo safety circuit prevents overcharge
- **RF exposure** - Amateur radio compliance with FCC Part 97
- **Component protection** - ESD precautions during handling

### Mechanical Safety  
- **Sharp edges** - All PCB edges rounded and smooth
- **Component security** - All parts firmly attached, no loose items
- **Lanyard strength** - Attachment points rated for safety loads
- **Material safety** - Lead-free solder, RoHS compliant components

---

**The badge IS the product - no assembly required beyond electronics!** 

*Ready to wear your radio on your sleeve (or around your neck)? 📡*

*73 de [Your Callsign]*