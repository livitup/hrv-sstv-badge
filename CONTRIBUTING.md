# Contributing to DEFCON SSTV Badge

🎉 Thank you for your interest in contributing to the DEFCON SSTV Badge project! This guide will help you get started with contributing to our open-source hardware and software project.

## 🎯 Project Vision

We're building an open-source SSTV badge that brings together:
- **Amateur radio operators** interested in digital modes
- **Badgelife enthusiasts** who love electronic art and social interaction  
- **Makers and hackers** passionate about RF, embedded systems, and photography
- **DEFCON attendees** who want unique, interactive badges

## 🤝 Ways to Contribute

### 🔧 Hardware Contributions
- **PCB Layout and Routing** - Help optimize the KiCAD design
- **Component Selection** - Suggest alternatives for cost/availability
- **Mechanical Design** - 3D printed cases, mounting solutions
- **Signal Integrity Analysis** - RF simulations and measurements
- **Power Optimization** - Battery life improvements
- **EMC/EMI Testing** - Regulatory compliance verification

### 💾 Firmware Contributions  
- **SSTV Encoding/Decoding** - Implement additional modes (Martin, Scottie, Robot)
- **Camera Interface** - OV2640 driver optimization
- **Display Graphics** - UI improvements and animations
- **Audio Processing** - Digital signal processing for clean SSTV
- **Power Management** - Sleep modes and battery monitoring
- **Menu System** - User interface enhancements
- **File System** - microSD card management
- **Radio Control** - SA818 frequency management

### 📝 Documentation Contributions
- **Assembly Guides** - Step-by-step build instructions with photos
- **User Manuals** - Operating procedures and troubleshooting
- **Developer Documentation** - API references and code examples
- **Video Tutorials** - YouTube guides for building and using
- **Translation** - Internationalization for global community
- **Test Procedures** - Quality assurance and validation steps

### 🎨 Design Contributions
- **PCB Artwork** - Silkscreen graphics and badge aesthetics
- **Case Design** - 3D printable enclosures and protection
- **User Experience** - Interface design and usability improvements
- **Logo and Branding** - Visual identity for the project
- **Website** - Project homepage and documentation site

## 🚀 Getting Started

### 1. Set Up Your Development Environment

#### Hardware Design (KiCAD)
```bash
# Install KiCAD 7.0 or later
# Ubuntu/Debian:
sudo apt install kicad

# macOS:
brew install kicad

# Windows: Download from kicad.org
```

#### Firmware Development (RP2350)
```bash
# Install Raspberry Pi Pico SDK
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk
git submodule update --init

# Set environment variable
export PICO_SDK_PATH=/path/to/pico-sdk

# Install build tools
sudo apt install cmake gcc-arm-none-eabi build-essential
```

#### Documentation (Markdown/Git)
```bash
# Clone the repository
git clone https://github.com/[username]/defcon-sstv-badge.git
cd defcon-sstv-badge

# Create a new branch for your work
git checkout -b feature/your-contribution-name
```

### 2. Choose Your Contribution Area

| Area | Difficulty | Skills Needed | Time Investment |
|------|------------|---------------|-----------------|
| **Documentation** | 🟢 Easy | Writing, Markdown | 1-5 hours |
| **Hardware Testing** | 🟢 Easy | Basic electronics | 2-8 hours |
| **Firmware Features** | 🟡 Medium | C/C++, Embedded | 5-20 hours |
| **PCB Layout** | 🟡 Medium | KiCAD, RF design | 10-40 hours |
| **SSTV Algorithms** | 🔴 Hard | DSP, Math | 20-80 hours |
| **RF Optimization** | 🔴 Hard | RF engineering | 40+ hours |

### 3. Find Issues to Work On

Check our [GitHub Issues](https://github.com/[username]/defcon-sstv-badge/issues) for:

- 🟢 **good first issue** - Perfect for newcomers
- 🆘 **help wanted** - We need assistance on these
- 🐛 **bug** - Something that needs fixing
- ✨ **enhancement** - New features or improvements
- 📝 **documentation** - Docs that need work

## 📋 Contribution Guidelines

### Code Standards

#### Hardware Design
- **KiCAD Version**: Use KiCAD 7.0 or later
- **Design Rules**: Follow IPC standards for PCB design
- **Component Libraries**: Use standard libraries when possible
- **Naming Convention**: Clear, descriptive net and component names
- **Documentation**: Include schematic notes and assembly drawings

#### Firmware Code Style
```c
// Use consistent indentation (4 spaces)
// Clear variable and function names
// Document complex algorithms
// Follow embedded C best practices

#define MAX_SSTV_MODES 8  // Constants in CAPS
static uint32_t system_clock_hz = 125000000;  // Snake case for variables

// Function documentation
/**
 * @brief Encode image data into SSTV format
 * @param image_data Pointer to image buffer
 * @param width Image width in pixels  
 * @param height Image height in pixels
 * @param mode SSTV mode (Martin1, Scottie1, etc.)
 * @return Status code (0 = success)
 */
int sstv_encode_image(uint8_t* image_data, uint16_t width, uint16_t height, sstv_mode_t mode);
```

#### Documentation Style
- **Markdown**: Use GitHub-flavored markdown
- **Headers**: Hierarchical structure with clear navigation
- **Code Blocks**: Always specify language for syntax highlighting
- **Images**: Include alt text for accessibility
- **Links**: Use relative paths for internal docs
- **Tone**: Friendly but technical, appropriate for makers/hams

### Pull Request Process

1. **Create a Branch**
   ```bash
   git checkout -b feature/descriptive-name
   ```

2. **Make Your Changes**
   - Follow coding standards
   - Add tests if applicable
   - Update documentation
   - Test thoroughly

3. **Commit Your Work**
   ```bash
   git add .
   git commit -m "feat: Add SSTV Robot 36 mode support
   
   - Implement Robot 36 encoding algorithm
   - Add mode selection to UI menu
   - Update documentation with timing specs
   - Closes #123"
   ```

4. **Push and Create Pull Request**
   ```bash
   git push origin feature/descriptive-name
   ```

5. **Pull Request Template**
   ```markdown
   ## Description
   Brief description of what this PR does
   
   ## Type of Change
   - [ ] Bug fix
   - [ ] New feature  
   - [ ] Documentation update
   - [ ] Hardware design change
   
   ## Testing
   - [ ] Code compiles without warnings
   - [ ] Hardware tested on prototype
   - [ ] Documentation reviewed
   - [ ] No breaking changes
   
   ## Checklist
   - [ ] Code follows style guidelines
   - [ ] Self-review completed
   - [ ] Documentation updated
   - [ ] Tests added/updated
   ```

### Commit Message Format

We use [Conventional Commits](https://www.conventionalcommits.org/):

```
<type>[optional scope]: <description>

[optional body]

[optional footer(s)]
```

**Types:**
- `feat:` - New features
- `fix:` - Bug fixes  
- `docs:` - Documentation changes
- `style:` - Code formatting (no logic changes)
- `refactor:` - Code restructuring
- `test:` - Adding/fixing tests
- `chore:` - Maintenance tasks

**Examples:**
```
feat(firmware): Add Martin 1 SSTV encoding

fix(hardware): Correct I2C pullup resistor values

docs(assembly): Add photos to soldering guide

style(firmware): Format code according to style guide
```

## 🧪 Testing Guidelines

### Hardware Testing
- **Electrical Verification**: Continuity, power rails, signal integrity
- **Functional Testing**: All features work as designed
- **RF Testing**: Transmit power, frequency accuracy, spurious emissions
- **Environmental Testing**: Temperature, vibration, drop tests
- **Regulatory Testing**: FCC/CE compliance measurements

### Firmware Testing
- **Unit Tests**: Individual function validation
- **Integration Tests**: Component interaction testing  
- **System Tests**: End-to-end functionality
- **Performance Tests**: Speed, memory usage, power consumption
- **Field Tests**: Real-world usage scenarios

### Documentation Testing
- **Accuracy**: All procedures work as documented
- **Completeness**: No missing steps or information
- **Clarity**: Easy to follow for target audience
- **Accessibility**: Works with screen readers, good contrast

## 🎨 Design Philosophy

### Hardware Design Principles
- **Open Source Friendly**: Use readily available components
- **Hackable**: Exposed test points, clear labeling
- **Reliable**: Conservative designs, proper derating
- **Manufacturable**: DFM considerations, reasonable tolerances
- **Cost Effective**: Balance features with affordability
- **Amateur Radio Compliant**: Follow regulations and band plans

### Software Design Principles
- **Modular**: Clean separation of concerns
- **Maintainable**: Well-documented, readable code
- **Efficient**: Optimized for embedded constraints
- **User Friendly**: Intuitive interface, good error handling
- **Extensible**: Easy to add new features
- **Safe**: Proper error checking, fail-safe behaviors

### Community Values
- **Inclusive**: Welcome contributors of all skill levels
- **Educational**: Help people learn new technologies
- **Collaborative**: Work together, share knowledge
- **Fun**: Remember this is about electronic art and play
- **Respectful**: Treat everyone with kindness and respect

## 🔒 Security Considerations

### Hardware Security
- No intentional backdoors or security vulnerabilities
- Secure boot processes where applicable
- Physical security considerations for badge use

### Software Security  
- Input validation for all user data
- Safe memory management practices
- No hardcoded secrets or credentials
- Regular security updates for dependencies

### Amateur Radio Security
- Proper station identification (callsign)
- Respect for frequency coordination
- No malicious interference or jamming

## 🏆 Recognition

Contributors will be recognized in:
- **README.md** - Contributors section
- **CONTRIBUTORS.md** - Detailed contributor list
- **Hardware** - Names on PCB silkscreen (if desired)
- **Firmware** - Credits in about screen
- **Documentation** - Author attribution
- **Conferences** - Acknowledgment in presentations

## ❓ Getting Help

### Community Support
- **GitHub Discussions** - General questions and community chat
- **Discord** - Real-time chat with other contributors
- **Weekly Meetings** - Video calls for major contributors (TBD)

### Technical Support
- **GitHub Issues** - Bug reports and feature requests
- **Documentation** - Check docs/ directory first
- **Code Review** - Get feedback on your contributions

### Mentorship
New to open source hardware or amateur radio? We can pair you with experienced mentors:

- **Hardware Mentors** - PCB design, RF engineering, component selection
- **Software Mentors** - Embedded C, digital signal processing, UI design  
- **Documentation Mentors** - Technical writing, tutorials, video creation
- **Amateur Radio Mentors** - Licensing, protocols, regulations

## 📊 Project Metrics

We track contribution health with:
- **Response Time**: How quickly we respond to issues/PRs
- **Merge Rate**: Percentage of PRs that get merged
- **Contributor Diversity**: First-time vs. repeat contributors
- **Documentation Coverage**: Percentage of features documented
- **Test Coverage**: Code coverage metrics

## 🎉 Thank You!

Every contribution, no matter how small, helps make this project better for the entire badgelife and amateur radio community. Whether you fix a typo, suggest a component change, or implement a major feature, you're helping create something amazing that will bring joy to people at DEFCON and beyond.

**Welcome to the team! 73 and happy hacking!** 📡✨

---

## 📞 Contact the Maintainers

- **Project Lead**: [Your name/callsign]
- **Hardware Team**: hardware@[project-domain]
- **Firmware Team**: firmware@[project-domain]  
- **Documentation Team**: docs@[project-domain]

*Last updated: [Current date]*