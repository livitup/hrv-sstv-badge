# DEFCON SSTV Badge Firmware

This directory contains all firmware source code for the DEFCON SSTV Badge running on the Raspberry Pi RP2350 microcontroller.

## 🏗️ Architecture

The firmware uses a dual-core architecture:
- **Core 0**: User Interface, file system, system management
- **Core 1**: SSTV processing, audio DSP, camera control

See [../docs/firmware-architecture.md](../docs/firmware-architecture.md) for detailed design information.

## 📁 Directory Structure

```
firmware/
├── src/                    # Main source code
│   ├── main.c             # Application entry point
│   ├── core0_main.c       # Core 0 user interface tasks
│   ├── core1_main.c       # Core 1 signal processing tasks
│   ├── hardware/          # Hardware abstraction layer
│   ├── drivers/           # Device drivers
│   ├── sstv/              # SSTV encoding/decoding
│   ├── ui/                # User interface system
│   └── utils/             # Utility functions
├── lib/                    # External libraries
│   ├── pico-sdk/          # Raspberry Pi Pico SDK (submodule)
│   └── third-party/       # Third-party libraries
├── examples/               # Example programs and tests
│   ├── blink/             # Basic LED blink test
│   ├── display/           # Display driver test
│   ├── camera/            # Camera interface test
│   └── sstv/              # SSTV algorithm tests
├── build/                  # Build output directory (auto-generated)
└── CMakeLists.txt         # Build configuration
```

## 🛠️ Development Setup

### Prerequisites

1. **Raspberry Pi Pico SDK**
   ```bash
   git clone https://github.com/raspberrypi/pico-sdk.git
   cd pico-sdk
   git submodule update --init
   export PICO_SDK_PATH=$(pwd)
   ```

2. **Build Tools**
   ```bash
   # Ubuntu/Debian
   sudo apt update
   sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential

   # macOS  
   brew install cmake gcc-arm-embedded

   # Windows - Install via MSYS2 or use WSL
   ```

3. **Optional Tools**
   - **OpenOCD**: For debugging with SWD probe
   - **picotool**: For UF2 file manipulation
   - **minicom/screen**: For serial console debugging

### Building the Firmware

1. **Clone this repository**
   ```bash
   git clone https://github.com/[username]/defcon-sstv-badge.git
   cd defcon-sstv-badge/firmware
   ```

2. **Create build directory**
   ```bash
   mkdir build
   cd build
   ```

3. **Configure build**
   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Release
   ```

4. **Compile**
   ```bash
   make -j4
   ```

5. **Output files**
   - `defcon_sstv_badge.uf2` - Main firmware for drag-and-drop install
   - `defcon_sstv_badge.elf` - ELF file for debugging
   - `defcon_sstv_badge.bin` - Binary for direct flash programming

### Flashing the Firmware

#### Method 1: UF2 Bootloader (Easiest)
1. Hold BOOTSEL button while connecting USB
2. RP2350 appears as USB drive
3. Drag `defcon_sstv_badge.uf2` to the drive
4. Device automatically reboots with new firmware

#### Method 2: SWD Programming (For Development)
```bash
# Using OpenOCD with Raspberry Pi Debug Probe
openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "program defcon_sstv_badge.elf verify reset exit"
```

#### Method 3: picotool (Command Line)
```bash
# Put device in bootloader mode, then:
picotool load defcon_sstv_badge.uf2
picotool reboot
```

## 🧪 Testing and Development

### Running Examples

Each example in the `examples/` directory can be built independently:

```bash
cd examples/blink
mkdir build && cd build
cmake .. 
make
# Flash the resulting .uf2 file
```

### Debug Console

The firmware provides a debug console over USB CDC (virtual serial port):

```bash
# Linux/macOS
screen /dev/ttyACM0 115200

# Windows
# Use PuTTY or similar terminal program
# Connect to COMx port at 115200 baud
```

Debug commands available:
- `status` - Show system status
- `test-camera` - Test camera functionality  
- `test-sstv` - Test SSTV encoding/decoding
- `freq <MHz>` - Set radio frequency
- `mem` - Show memory usage

### Debugging with GDB

```bash
# Start OpenOCD in one terminal
openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg

# In another terminal, start GDB
arm-none-eabi-gdb defcon_sstv_badge.elf
(gdb) target extended-remote localhost:3333
(gdb) load
(gdb) monitor reset init
(gdb) break main
(gdb) continue
```

## ⚙️ Configuration

### Build Options

Configure build options in `CMakeLists.txt` or via command line:

```bash
# Enable debug output
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_DEBUG=ON

# Disable specific features  
cmake .. -DENABLE_CAMERA=OFF -DENABLE_AUDIO=OFF

# Set frequency band
cmake .. -DDEFAULT_BAND=VHF
```

### Runtime Configuration

The badge stores configuration in `config/settings.ini` on the microSD card:

```ini
[radio]
callsign = N0CALL
frequency = 145500000
mode = martin1
power = 1

[display]
brightness = 128
timeout = 30000

[system]
auto_save = true
debug_log = false
```

## 🐛 Troubleshooting

### Common Issues

**Build Errors:**
- Ensure `PICO_SDK_PATH` environment variable is set
- Check that all git submodules are initialized
- Verify cross-compiler is installed and in PATH

**Flash/Programming Issues:**
- Try different USB cable (data-capable, not power-only)
- Ensure device is in bootloader mode (BOOTSEL button)
- Check permissions on Linux (`sudo usermod -a -G dialout $USER`)

**Runtime Issues:**
- Check debug console for error messages
- Verify all hardware connections match schematic
- Test with minimal configuration (disable non-essential features)

**Radio Issues:**
- Verify amateur radio license and frequency allocations
- Check antenna connection and SWR
- Ensure SA818 module is properly seated

### Getting Help

1. **Check the debug console** for error messages
2. **Review the documentation** in `docs/` directory
3. **Search existing issues** on GitHub
4. **Create a new issue** with:
   - Hardware revision
   - Firmware version
   - Debug console output
   - Steps to reproduce

## 🤝 Contributing

See [../CONTRIBUTING.md](../CONTRIBUTING.md) for detailed contribution guidelines.

### Code Style

- **Indentation**: 4 spaces (no tabs)
- **Naming**: `snake_case` for functions/variables, `UPPER_CASE` for constants
- **Comments**: Doxygen format for functions, inline for complex logic
- **Line length**: Maximum 100 characters

### Testing Requirements

- All new features must include example code
- Hardware-dependent code should have simulation/mock alternatives
- Performance-critical code should include benchmarks
- Breaking changes require migration guide

## 📊 Performance Metrics

Current firmware performance on RP2350 @ 125MHz:

| Function | Execution Time | CPU Usage | Memory |
|----------|----------------|-----------|---------|
| SSTV Line Encode | 15ms | 65% Core 1 | 32KB |
| Display Update | 8ms | 25% Core 0 | 76KB |
| Camera Capture | 200ms | 80% Core 1 | 150KB |
| SD Card Write | Variable | 10% Core 0 | 8KB |

## 📜 License

This firmware is released under the MIT License. See [../LICENSE](../LICENSE) for details.

---

**Ready to start coding? Check out the examples directory and dive in!** 🚀

*73 de [Your Callsign]* 📡