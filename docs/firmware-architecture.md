# DEFCON SSTV Badge Firmware Architecture

## 🎯 Overview

The DEFCON SSTV Badge firmware is designed as a real-time embedded system running on the Raspberry Pi RP2350 dual-core ARM Cortex-M33 microcontroller. The architecture leverages both cores to provide responsive user interaction while performing computationally intensive SSTV encoding and decoding operations.

## 🏗️ System Architecture

### Dual-Core Design Strategy
```
┌─────────────────────────┐    ┌─────────────────────────┐
│        CORE 0           │    │        CORE 1           │
│    (User Interface)     │    │   (Signal Processing)   │
├─────────────────────────┤    ├─────────────────────────┤
│ • Display Management    │    │ • SSTV Encoding         │
│ • Button Handling       │    │ • SSTV Decoding         │
│ • Menu Navigation       │    │ • Audio Processing      │
│ • File System          │    │ • Image Processing      │
│ • System Management     │    │ • Digital Filtering     │
│ • Battery Monitoring    │    │ • SA818 Control         │
│ • Status LEDs           │    │ • Camera Interface      │
└─────────────────────────┘    └─────────────────────────┘
           │                              │
           └──────────── IPC ─────────────┘
                (Inter-Process Communication)
```

### Core Responsibilities

#### Core 0: User Interface and System Management
- **Primary Role**: Responsive user experience
- **Priority**: Real-time interaction, never block on user input
- **Key Functions**:
  - Display rendering and backlight control
  - Button debouncing and menu navigation
  - File system operations (save/load images)
  - Battery monitoring and charging status
  - System configuration and settings
  - LED status indicators
  - Error handling and user feedback

#### Core 1: Signal Processing and Radio Control
- **Primary Role**: Computationally intensive operations
- **Priority**: Signal quality and timing accuracy
- **Key Functions**:
  - SSTV encoding algorithms (Martin, Scottie, Robot modes)
  - SSTV decoding and automatic mode detection
  - Audio DAC/ADC control and DSP operations
  - Camera image capture and processing
  - SA818 radio control and frequency management
  - Image format conversion and compression
  - Real-time audio filtering

---

## 📦 Software Component Hierarchy

### Layer 1: Hardware Abstraction Layer (HAL)
```
Hardware Abstraction Layer
├── GPIO Driver
│   ├── Button input handling
│   ├── LED output control
│   └── Switch position reading
├── SPI Driver  
│   ├── Display interface (ILI9341)
│   ├── Flash memory (W25Q128)
│   └── SD card interface
├── I2C Driver
│   ├── Battery fuel gauge (LC709203F)
│   ├── Camera configuration (OV2640 SCCB)
│   └── SAO expansion devices
├── UART Driver
│   └── SA818 radio control
├── I2S Driver
│   └── PCM5102A DAC data interface (TX audio)
├── ADC Driver
│   └── RP2350 internal ADC (RX audio)
├── DVP Driver
│   └── Camera data interface
└── PWM Driver
    ├── Display backlight control
    └── Audio signal generation
```

### Layer 2: Device Drivers
```
Device Drivers
├── Display Driver (ILI9341)
│   ├── Graphics primitives
│   ├── Font rendering
│   ├── Image display
│   └── Backlight control
├── Camera Driver (OV2640)
│   ├── Sensor configuration
│   ├── Image capture
│   ├── Format conversion
│   └── Auto exposure/white balance
├── Audio Driver (PCM5102A DAC + RP2350 ADC)
│   ├── ADC/DAC control
│   ├── Volume and gain settings
│   ├── Audio routing
│   └── Power management
├── Radio Driver (SA818)
│   ├── Frequency control
│   ├── Power/channel settings
│   ├── PTT control
│   └── Status monitoring
├── Storage Driver
│   ├── SD card file system
│   ├── Image file management
│   ├── Configuration storage
│   └── Log file handling
└── Power Management
    ├── Battery monitoring
    ├── Charging control
    ├── Power saving modes
    └── System reset/shutdown
```

### Layer 3: Application Services
```
Application Services
├── SSTV Engine
│   ├── Mode detection
│   ├── Encoding algorithms
│   ├── Decoding algorithms
│   └── Signal analysis
├── Image Processing
│   ├── JPEG decode/encode
│   ├── Format conversion
│   ├── Scaling and cropping
│   ├── Overlay text rendering
│   └── Quality optimization
├── Audio Processing
│   ├── Digital filtering
│   ├── AGC and limiting
│   ├── Tone generation
│   └── Level monitoring
├── User Interface
│   ├── Menu system
│   ├── Settings management
│   ├── Image gallery
│   └── Status displays
├── File Management
│   ├── Image archival
│   ├── Thumbnail generation
│   ├── Metadata handling
│   └── Storage optimization
└── Communication
    ├── Radio control
    ├── Transmission scheduling
    ├── Reception handling
    └── Error correction
```

### Layer 4: Application Layer
```
Main Application
├── Boot Sequence
├── System Initialization  
├── Main Event Loop
├── Inter-Core Communication
├── Error Recovery
└── System Shutdown
```

---

## 🔄 Inter-Core Communication (IPC)

### Communication Mechanisms
```c
// Core-to-core message passing
typedef struct {
    uint32_t msg_type;
    uint32_t data_len;
    uint8_t data[256];
} ipc_message_t;

// Message types
#define IPC_CAPTURE_IMAGE    0x01
#define IPC_ENCODE_SSTV      0x02  
#define IPC_DECODE_SSTV      0x03
#define IPC_TRANSMIT         0x04
#define IPC_STATUS_UPDATE    0x05
#define IPC_CONFIG_CHANGE    0x06
```

### Shared Memory Regions
```c
// Shared data structures
typedef struct {
    volatile uint32_t flags;
    volatile uint32_t image_buffer_addr;
    volatile uint32_t audio_buffer_addr;
    volatile system_status_t status;
    volatile system_config_t config;
} shared_memory_t;

#define SHARED_MEM_BASE 0x20040000
```

### Synchronization Primitives
- **Mutexes**: Protect shared resources
- **Semaphores**: Signal completion of operations  
- **Message Queues**: Asynchronous communication
- **Event Flags**: Status and control signaling

---

## 📸 Camera and Image Processing Pipeline

### Image Capture Flow
```
Camera Capture Pipeline:
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   Button    │───▶│   Core 0    │───▶│   Core 1    │
│   Press     │    │  UI Handler │    │Camera Driver│
└─────────────┘    └─────────────┘    └─────────────┘
                          │                   │
                          ▼                   ▼
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│  Display    │◀───│    Image    │◀───│  OV2640     │
│  Preview    │    │ Processing  │    │  Capture    │
└─────────────┘    └─────────────┘    └─────────────┘
                          │
                          ▼
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│    SSTV     │◀───│  Overlay    │───▶│   SD Card   │
│  Encoding   │    │  Callsign   │    │   Storage   │
└─────────────┘    └─────────────┘    └─────────────┘
```

### Image Processing Functions
```c
// Core image processing API
typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t format;  // JPEG, RGB565, etc.
    uint32_t size;
    uint8_t* data;
} image_t;

// Image processing functions
int camera_capture_image(image_t* img);
int image_add_callsign_overlay(image_t* img, const char* callsign);  
int image_convert_format(image_t* src, image_t* dst, uint8_t format);
int image_scale(image_t* src, image_t* dst, uint16_t width, uint16_t height);
int image_save_to_sd(image_t* img, const char* filename);
int image_load_from_sd(image_t* img, const char* filename);
```

---

## 📡 SSTV Encoding and Decoding

### SSTV Mode Support
```c
typedef enum {
    SSTV_MARTIN_1,
    SSTV_MARTIN_2, 
    SSTV_SCOTTIE_1,
    SSTV_SCOTTIE_2,
    SSTV_ROBOT_36,
    SSTV_PD_90,
    SSTV_PD_120,
    SSTV_PD_180
} sstv_mode_t;

typedef struct {
    sstv_mode_t mode;
    uint16_t width;
    uint16_t height;
    float line_time_ms;
    float sync_time_ms;
    bool color_mode;
} sstv_format_t;
```

### Encoding Pipeline
```
SSTV Encoding Process:
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│    RGB      │───▶│   YUV/RGB   │───▶│  Line-by-   │
│   Image     │    │ Conversion  │    │Line Encode  │
└─────────────┘    └─────────────┘    └─────────────┘
                          │                   │
                          ▼                   ▼
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   Audio     │◀───│   Timing    │◀───│  FM Tone    │
│ Output      │    │  Control    │    │ Generation  │
└─────────────┘    └─────────────┘    └─────────────┘
```

### Decoding Pipeline  
```
SSTV Decoding Process:
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   Audio     │───▶│    FFT      │───▶│    Mode     │
│   Input     │    │  Analysis   │    │ Detection   │
└─────────────┘    └─────────────┘    └─────────────┘
                          │                   │
                          ▼                   ▼
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   Image     │◀───│   Pixel     │◀───│  Line Sync  │
│ Reconstruction │  │ Extraction  │    │ Detection   │
└─────────────┘    └─────────────┘    └─────────────┘
```

### SSTV Algorithm Implementation
```c
// SSTV encoding functions
int sstv_encode_image(image_t* img, sstv_mode_t mode, audio_buffer_t* audio);
int sstv_generate_vis_code(sstv_mode_t mode, audio_buffer_t* audio);
int sstv_encode_line(uint8_t* line_data, sstv_format_t* format, audio_buffer_t* audio);

// SSTV decoding functions  
int sstv_detect_mode(audio_buffer_t* audio, sstv_mode_t* detected_mode);
int sstv_decode_image(audio_buffer_t* audio, sstv_mode_t mode, image_t* img);
int sstv_sync_detection(audio_buffer_t* audio, float* sync_time);
```

---

## 🎵 Audio Processing System

### Audio Signal Chain
```
TX Path (PCM5102A DAC):
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   SA818     │◀───│  PCM5102A   │◀───│   RP2350    │
│   MIC In    │    │  DAC (I2S)  │    │  I2S Out    │
└─────────────┘    └─────────────┘    └─────────────┘

RX Path (RP2350 Internal ADC):
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   SA818     │───▶│  Anti-Alias │───▶│   RP2350    │
│   SPK Out   │    │  Filter     │    │  ADC Input  │
└─────────────┘    └─────────────┘    └─────────────┘
```

### Audio Buffer Management
```c
#define AUDIO_SAMPLE_RATE   48000
#define AUDIO_BUFFER_SIZE   1024
#define NUM_AUDIO_BUFFERS   4

typedef struct {
    int16_t samples[AUDIO_BUFFER_SIZE];
    uint32_t sample_count;
    uint32_t timestamp;
    bool ready;
} audio_buffer_t;

// Audio processing functions
int audio_init(void);
int audio_start_capture(void);
int audio_start_playback(void);
int audio_get_buffer(audio_buffer_t** buffer);
int audio_process_sstv_rx(audio_buffer_t* buffer);
int audio_generate_sstv_tx(audio_buffer_t* buffer, sstv_line_t* line);
```

---

## 💾 File System and Storage

### Storage Architecture
```
Storage System:
┌─────────────────────────────────────────────────┐
│                MicroSD Card                     │
├─────────────────────────────────────────────────┤
│  /images/                                       │
│    ├── captured/     (user captured images)     │
│    ├── received/     (SSTV received images)     │
│    ├── thumbnails/   (preview images)           │
│    └── temp/         (processing workspace)     │
├─────────────────────────────────────────────────┤
│  /config/                                       │
│    ├── settings.ini  (user preferences)         │
│    ├── callsign.txt  (amateur radio callsign)   │
│    └── frequencies.ini (channel memory)         │
├─────────────────────────────────────────────────┤
│  /logs/                                         │
│    ├── system.log    (debug and error logs)     │
│    ├── contacts.log  (SSTV contacts made)       │
│    └── activity.log  (transmission log)         │
└─────────────────────────────────────────────────┘
```

### File Management API
```c
// File system functions
int fs_init(void);
int fs_save_image(image_t* img, const char* filename);
int fs_load_image(image_t* img, const char* filename);
int fs_delete_image(const char* filename);
int fs_list_images(char** filenames, int max_count);
int fs_get_free_space(uint64_t* bytes_free);

// Configuration management
int config_load_settings(system_config_t* config);
int config_save_settings(system_config_t* config);
int config_get_callsign(char* callsign, size_t max_len);
int config_set_frequency(uint32_t frequency_hz);
```

---

## 🎮 User Interface System

### Menu System Architecture
```
Menu Hierarchy:
┌─────────────────────────────────────────────────┐
│                Main Menu                        │
├─────────────────────────────────────────────────┤
│ • Capture Photo                                 │
│ • Browse Images                                 │
│ • Radio Settings                                │
│ • System Settings                               │
│ • Status/Info                                   │
└─────────────────────────────────────────────────┘
           │
           ├── Radio Settings ─┐
           │                  │
           ├── Browse Images ──┼── Image Gallery
           │                  │     ├── View Image
           ├── System Settings─┼     ├── Delete Image  
           │                  │     └── Transmit Image
           └── Status/Info ────┘
                               │
                System Settings─┼── Callsign Setup
                               ├── Display Settings
                               ├── Audio Settings  
                               ├── Power Management
                               └── Factory Reset
```

### UI Event Handling
```c
typedef enum {
    UI_EVENT_BUTTON_PRESS,
    UI_EVENT_BUTTON_RELEASE, 
    UI_EVENT_DPAD_UP,
    UI_EVENT_DPAD_DOWN,
    UI_EVENT_DPAD_LEFT,
    UI_EVENT_DPAD_RIGHT,
    UI_EVENT_DPAD_CENTER,
    UI_EVENT_PHOTO_CAPTURE,
    UI_EVENT_SYSTEM_STATUS,
    UI_EVENT_TIMEOUT
} ui_event_type_t;

typedef struct {
    ui_event_type_t type;
    uint32_t timestamp;
    uint32_t data;
} ui_event_t;

// UI functions
int ui_init(void);
int ui_handle_event(ui_event_t* event);
int ui_update_display(void);
int ui_show_image(image_t* img);
int ui_show_menu(menu_t* menu);
int ui_show_status(system_status_t* status);
```

---

## ⚡ Power Management

### Power States
```c
typedef enum {
    POWER_STATE_ACTIVE,     // Full operation
    POWER_STATE_IDLE,       // Display off, radio standby
    POWER_STATE_SLEEP,      // CPU sleep, wake on interrupt
    POWER_STATE_DEEP_SLEEP, // Minimal power, wake on button
    POWER_STATE_SHUTDOWN    // Controlled shutdown
} power_state_t;

typedef struct {
    power_state_t current_state;
    uint32_t battery_voltage_mv;
    uint32_t battery_percentage;
    bool is_charging;
    bool is_usb_connected;
    uint32_t estimated_runtime_minutes;
} power_status_t;
```

### Power Management Functions
```c
int power_init(void);
int power_set_state(power_state_t state);
int power_get_status(power_status_t* status);
int power_enable_radio(bool enable);
int power_set_display_brightness(uint8_t brightness);
int power_shutdown_sequence(void);
```

---

## 🔧 System Configuration

### Configuration Structure
```c
typedef struct {
    // Radio configuration
    char callsign[12];
    uint32_t frequency_hz;
    sstv_mode_t default_sstv_mode;
    uint8_t radio_power_level;
    bool airplane_mode;
    
    // Display configuration  
    uint8_t display_brightness;
    uint32_t display_timeout_ms;
    bool auto_brightness;
    
    // Audio configuration
    uint8_t audio_volume;
    bool audio_mute;
    uint8_t mic_gain;
    
    // System configuration
    bool auto_save_images;
    uint32_t image_quality;
    bool debug_logging;
    
    // Version tracking
    uint32_t config_version;
    uint32_t firmware_version;
} system_config_t;
```

---

## 🚨 Error Handling and Recovery

### Error Classification
```c
typedef enum {
    ERROR_NONE = 0,
    ERROR_HARDWARE_FAULT,
    ERROR_COMMUNICATION_TIMEOUT,
    ERROR_MEMORY_ALLOCATION,
    ERROR_FILE_SYSTEM,
    ERROR_INVALID_PARAMETER,
    ERROR_OPERATION_TIMEOUT,
    ERROR_RESOURCE_BUSY,
    ERROR_CRITICAL_FAILURE
} error_code_t;

typedef struct {
    error_code_t code;
    const char* description;
    uint32_t timestamp;
    uint32_t line_number;
    const char* file_name;
} error_info_t;
```

### Recovery Strategies
- **Soft Reset**: Restart affected subsystem
- **Hard Reset**: Full system restart  
- **Safe Mode**: Limited functionality mode
- **Factory Reset**: Restore default configuration
- **Error Logging**: Persistent error history for debugging

---

## 🧪 Testing and Debugging

### Debug Interfaces
```c
// Debug output functions
#define DEBUG_PRINT(fmt, ...) debug_printf(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define ERROR_PRINT(fmt, ...) error_printf(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

// System health monitoring
typedef struct {
    uint32_t uptime_seconds;
    uint32_t free_heap_bytes;
    uint32_t max_stack_usage_bytes;
    uint32_t cpu_usage_percent[2];  // Per core
    uint32_t error_count;
    uint32_t watchdog_resets;
} system_health_t;

int debug_get_system_health(system_health_t* health);
```

### Test Modes
- **Hardware Test**: Verify all peripherals
- **RF Test**: Transmit test patterns
- **Audio Test**: Generate/receive test tones  
- **Display Test**: Show test patterns
- **Storage Test**: Read/write verification
- **Battery Test**: Discharge/charge cycles

---

## 📊 Performance Specifications

### Real-Time Requirements
| Function | Maximum Latency | Notes |
|----------|----------------|-------|
| Button Response | 50ms | UI responsiveness |
| Display Update | 16ms | 60 FPS target |
| Audio Processing | 1ms | Real-time DSP |
| SSTV Line Timing | ±1ms | Signal quality |
| Camera Capture | 500ms | Image acquisition |
| File Operations | 2s | SD card dependent |

### Memory Usage
| Component | RAM Usage | Flash Usage |
|-----------|-----------|-------------|
| Core 0 UI | 64KB | 128KB |
| Core 1 DSP | 128KB | 256KB |
| Image Buffers | 150KB | - |
| Audio Buffers | 32KB | - |
| File System | 16KB | 64KB |
| **Total** | **390KB** | **448KB** |

### Power Consumption
| Mode | Current Draw | Battery Life |
|------|-------------|--------------|
| Active (no TX) | 200mA | 10 hours |
| Idle | 100mA | 20 hours |
| Sleep | 10mA | 200 hours |
| Transmitting | 1.7A | 1.2 hours continuous |

---

## 🔄 Boot Sequence and Initialization

### Boot Process Flow
```
System Boot Sequence:
┌─────────────────┐
│  Hardware Reset │
└─────┬───────────┘
      │
┌─────▼───────────┐
│ BootROM (RP2350)│
│ • Load from SPI │
│ • Jump to Flash │
└─────┬───────────┘
      │
┌─────▼───────────┐
│ Stage 1 Boot    │
│ • Core 0 Only   │
│ • Basic HAL     │
│ • Memory Init   │
└─────┬───────────┘
      │
┌─────▼───────────┐
│ Stage 2 Boot    │
│ • Start Core 1  │
│ • Device Drivers│
│ • File System   │
└─────┬───────────┘
      │
┌─────▼───────────┐
│ Application     │
│ • UI System     │
│ • SSTV Engine   │
│ • Main Loop     │
└─────────────────┘
```

### Initialization Sequence
```c
// Main initialization function
int system_init(void) {
    // Stage 1: Critical hardware
    clock_init();
    gpio_init();
    uart_init(UART_DEBUG);
    
    // Stage 2: Memory and storage
    heap_init();
    flash_init();
    sd_card_init();
    
    // Stage 3: Communication interfaces
    i2c_init();
    spi_init();
    i2s_init();
    
    // Stage 4: Device drivers
    display_init();
    camera_init();
    audio_init();  // PCM5102A DAC + RP2350 ADC
    radio_init();
    
    // Stage 5: Application services
    ui_init();
    sstv_engine_init();
    file_manager_init();
    power_manager_init();
    
    // Stage 6: Start second core
    multicore_launch_core1(core1_main);
    
    // Stage 7: Load configuration
    config_load();
    
    return 0;
}
```

---

## 🔄 Main Event Loop

### Core 0 Main Loop (UI)
```c
void core0_main(void) {
    ui_event_t event;
    system_status_t status;
    uint32_t last_display_update = 0;
    
    while (true) {
        // Handle user input events
        if (ui_get_event(&event, 10)) {
            ui_handle_event(&event);
        }
        
        // Update display periodically
        if (time_us_32() - last_display_update > DISPLAY_UPDATE_INTERVAL) {
            ui_update_display();
            last_display_update = time_us_32();
        }
        
        // Handle inter-core messages
        ipc_message_t msg;
        if (ipc_receive_message(&msg, 0)) {
            handle_core1_message(&msg);
        }
        
        // System health monitoring
        if (should_check_system_health()) {
            system_health_check();
        }
        
        // Power management
        power_manager_update();
        
        // Yield CPU if idle
        if (system_is_idle()) {
            __WFI();  // Wait for interrupt
        }
    }
}
```

### Core 1 Main Loop (Signal Processing)
```c
void core1_main(void) {
    audio_buffer_t* audio_buf;
    image_t* image_buf;
    sstv_context_t sstv_ctx;
    
    // Initialize signal processing subsystems
    sstv_engine_init(&sstv_ctx);
    audio_processing_init();
    camera_interface_init();
    
    while (true) {
        // Handle inter-core messages from UI
        ipc_message_t msg;
        if (ipc_receive_message(&msg, 1)) {
            handle_core0_message(&msg);
        }
        
        // Process incoming audio for SSTV reception
        if (audio_get_rx_buffer(&audio_buf)) {
            sstv_process_audio(&sstv_ctx, audio_buf);
            audio_release_buffer(audio_buf);
        }
        
        // Handle transmission requests
        if (sstv_ctx.tx_pending) {
            sstv_transmit_image(&sstv_ctx);
        }
        
        // Camera operations
        if (camera_frame_ready()) {
            camera_process_frame();
        }
        
        // Background processing
        sstv_background_tasks(&sstv_ctx);
        
        // Yield if no work pending
        if (core1_is_idle()) {
            __WFI();
        }
    }
}
```

---

## 📡 SA818 Radio Control Protocol

### SA818 Command Interface
```c
// SA818 command structure
typedef struct {
    char cmd[16];        // Command string
    uint32_t timeout_ms; // Response timeout
    bool expects_response;
} sa818_cmd_t;

// Common SA818 commands
#define SA818_CMD_DMOSETGROUP   "AT+DMOSETGROUP"
#define SA818_CMD_DMOSETVOLUME  "AT+DMOSETVOLUME"  
#define SA818_CMD_SETFILTER     "AT+SETFILTER"
#define SA818_CMD_VERSION       "AT+VERSION"

// SA818 control functions
int sa818_send_command(const char* cmd, char* response, size_t max_len);
int sa818_set_frequency(uint32_t tx_freq_hz, uint32_t rx_freq_hz);
int sa818_set_volume(uint8_t volume);
int sa818_enable_tx(bool enable);
int sa818_get_status(sa818_status_t* status);
```

### Frequency Management
```c
typedef struct {
    uint32_t frequency;
    char name[16];
    sstv_mode_t preferred_mode;
    uint8_t power_level;
} frequency_memory_t;

// Frequency control
int freq_validate_amateur_band(uint32_t freq_hz);
int freq_set_active(uint32_t freq_hz);
int freq_store_memory(uint8_t memory_slot, frequency_memory_t* freq);
int freq_recall_memory(uint8_t memory_slot, frequency_memory_t* freq);
```

---

## 🎨 Display Graphics System

### Graphics Primitives
```c
// Graphics context
typedef struct {
    uint16_t width;
    uint16_t height;
    uint16_t* framebuffer;
    font_t* current_font;
    uint16_t fg_color;
    uint16_t bg_color;
} graphics_ctx_t;

// Drawing functions
int gfx_clear_screen(graphics_ctx_t* ctx, uint16_t color);
int gfx_draw_pixel(graphics_ctx_t* ctx, int x, int y, uint16_t color);
int gfx_draw_line(graphics_ctx_t* ctx, int x1, int y1, int x2, int y2, uint16_t color);
int gfx_draw_rectangle(graphics_ctx_t* ctx, int x, int y, int w, int h, uint16_t color);
int gfx_draw_circle(graphics_ctx_t* ctx, int x, int y, int radius, uint16_t color);
int gfx_draw_text(graphics_ctx_t* ctx, int x, int y, const char* text);
int gfx_draw_image(graphics_ctx_t* ctx, int x, int y, image_t* img);
```

### UI Widget System
```c
// Widget types
typedef enum {
    WIDGET_BUTTON,
    WIDGET_LABEL,
    WIDGET_IMAGE,
    WIDGET_PROGRESSBAR,
    WIDGET_LISTBOX,
    WIDGET_MENU
} widget_type_t;

// Widget base structure
typedef struct widget {
    widget_type_t type;
    int x, y, width, height;
    bool visible;
    bool enabled;
    void (*draw)(struct widget* w, graphics_ctx_t* ctx);
    bool (*handle_event)(struct widget* w, ui_event_t* event);
} widget_t;

// Menu system
typedef struct {
    char title[32];
    widget_t* widgets[MAX_WIDGETS];
    int widget_count;
    int selected_widget;
} menu_screen_t;
```

---

## 🔊 Digital Signal Processing

### SSTV Signal Processing
```c
// SSTV decoder state machine
typedef enum {
    SSTV_STATE_IDLE,
    SSTV_STATE_LISTENING,
    SSTV_STATE_VIS_DETECTED,
    SSTV_STATE_SYNC_DETECTED,
    SSTV_STATE_RECEIVING_IMAGE,
    SSTV_STATE_IMAGE_COMPLETE
} sstv_state_t;

// SSTV processing context
typedef struct {
    sstv_state_t state;
    sstv_mode_t mode;
    float sample_rate;
    uint32_t samples_processed;
    uint32_t current_line;
    uint32_t current_pixel;
    image_t* current_image;
    
    // DSP buffers and filters
    float* fft_buffer;
    float* filter_buffer;
    complex_t* frequency_domain;
    
    // Timing and synchronization
    uint32_t sync_timer;
    float frequency_offset;
    float phase_accumulator;
} sstv_context_t;
```

### Audio Filtering
```c
// Digital filter types
typedef enum {
    FILTER_LOWPASS,
    FILTER_HIGHPASS,
    FILTER_BANDPASS,
    FILTER_NOTCH
} filter_type_t;

typedef struct {
    filter_type_t type;
    float cutoff_freq;
    float bandwidth;
    float sample_rate;
    float coeffs_a[5];
    float coeffs_b[5];
    float delay_line[5];
} digital_filter_t;

// DSP functions
int dsp_init_filter(digital_filter_t* filter, filter_type_t type, float cutoff, float fs);
float dsp_apply_filter(digital_filter_t* filter, float sample);
int dsp_fft(float* input, complex_t* output, int n);
float dsp_detect_frequency(complex_t* spectrum, int n, float sample_rate);
int dsp_generate_tone(float frequency, float amplitude, float* output, int n_samples);
```

---

## 🔐 Security and Validation

### Input Validation
```c
// Parameter validation macros
#define VALIDATE_PTR(ptr) \
    do { if (!(ptr)) return ERROR_INVALID_PARAMETER; } while(0)

#define VALIDATE_RANGE(val, min, max) \
    do { if ((val) < (min) || (val) > (max)) return ERROR_INVALID_PARAMETER; } while(0)

// Validation functions
int validate_callsign(const char* callsign);
int validate_frequency(uint32_t freq_hz);
int validate_sstv_mode(sstv_mode_t mode);
int validate_image_format(image_t* img);
```

### Safe Memory Management
```c
// Safe memory allocation
#define SAFE_MALLOC(ptr, size) \
    do { \
        (ptr) = malloc(size); \
        if (!(ptr)) { \
            ERROR_PRINT("Memory allocation failed: %d bytes", size); \
            return ERROR_MEMORY_ALLOCATION; \
        } \
        memset((ptr), 0, (size)); \
    } while(0)

#define SAFE_FREE(ptr) \
    do { \
        if (ptr) { \
            free(ptr); \
            (ptr) = NULL; \
        } \
    } while(0)
```

---

## 📈 Performance Optimization

### CPU Optimization Strategies
```c
// Fast math approximations for DSP
static inline float fast_sin(float x) {
    // Taylor series approximation for speed
    const float x2 = x * x;
    return x * (1.0f - x2 * (0.16666667f - x2 * 0.00833333f));
}

static inline float fast_sqrt(float x) {
    // Newton-Raphson approximation
    float xhalf = 0.5f * x;
    int i = *(int*)&x;
    i = 0x5f3759df - (i >> 1);
    x = *(float*)&i;
    x = x * (1.5f - xhalf * x * x);
    return x;
}

// DMA for high-speed data transfers
int setup_dma_transfer(void* src, void* dst, size_t len);
```

### Memory Optimization
```c
// Memory pool for audio buffers
typedef struct {
    audio_buffer_t buffers[NUM_AUDIO_BUFFERS];
    bool buffer_in_use[NUM_AUDIO_BUFFERS];
    mutex_t pool_mutex;
} audio_buffer_pool_t;

audio_buffer_t* audio_buffer_alloc(void);
void audio_buffer_free(audio_buffer_t* buffer);
```

---

## 🧪 Testing Framework

### Unit Test Infrastructure
```c
// Test framework macros
#define TEST_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            test_fail(__FILE__, __LINE__, #condition); \
            return; \
        } \
    } while(0)

#define TEST_CASE(name) \
    void test_##name(void)

#define RUN_TEST(name) \
    do { \
        printf("Running test: %s\n", #name); \
        test_##name(); \
        printf("PASSED\n"); \
    } while(0)

// Example test cases
TEST_CASE(sstv_encoding) {
    image_t test_image = {320, 240, IMAGE_FORMAT_RGB565, NULL};
    audio_buffer_t audio_out;
    
    int result = sstv_encode_image(&test_image, SSTV_MARTIN_1, &audio_out);
    TEST_ASSERT(result == 0);
    TEST_ASSERT(audio_out.sample_count > 0);
}

TEST_CASE(frequency_validation) {
    TEST_ASSERT(freq_validate_amateur_band(145500000) == 0);  // Valid 2m
    TEST_ASSERT(freq_validate_amateur_band(100000000) != 0);  // Invalid
}
```

### Integration Testing
```c
// Hardware-in-the-loop testing
int test_complete_sstv_chain(void) {
    // 1. Capture test image
    image_t captured;
    if (camera_capture_image(&captured) != 0) return -1;
    
    // 2. Encode to SSTV
    audio_buffer_t encoded_audio;
    if (sstv_encode_image(&captured, SSTV_MARTIN_1, &encoded_audio) != 0) return -1;
    
    // 3. Decode back to image  
    image_t decoded;
    if (sstv_decode_audio(&encoded_audio, &decoded) != 0) return -1;
    
    // 4. Compare images (allowing for compression artifacts)
    float similarity = image_compare(&captured, &decoded);
    if (similarity < 0.8f) return -1;
    
    return 0;
}
```

---

## 🔧 Development Tools and Debugging

### Debug Console Commands
```c
// Debug command structure
typedef struct {
    const char* name;
    const char* description;
    int (*handler)(int argc, char* argv[]);
} debug_command_t;

// Example debug commands
int cmd_status(int argc, char* argv[]);
int cmd_test_camera(int argc, char* argv[]);
int cmd_test_sstv(int argc, char* argv[]);
int cmd_set_frequency(int argc, char* argv[]);
int cmd_memory_info(int argc, char* argv[]);

static const debug_command_t debug_commands[] = {
    {"status", "Show system status", cmd_status},
    {"test-camera", "Test camera functionality", cmd_test_camera},
    {"test-sstv", "Test SSTV encoding/decoding", cmd_test_sstv},
    {"freq", "Set radio frequency", cmd_set_frequency},
    {"mem", "Show memory usage", cmd_memory_info},
    {NULL, NULL, NULL}
};
```

### Runtime Diagnostics
```c
// System monitoring
typedef struct {
    uint32_t heap_free;
    uint32_t heap_used;
    uint32_t stack_high_water[2];  // Per core
    uint32_t cpu_usage[2];         // Per core
    uint32_t interrupts_per_sec;
    uint32_t audio_buffer_overruns;
    uint32_t display_frame_drops;
} system_diagnostics_t;

int diagnostics_update(system_diagnostics_t* diag);
int diagnostics_log_event(const char* event, uint32_t data);
```

---

## 🚀 Future Enhancements and Extensibility

### Plugin Architecture
```c
// Plugin interface for future expansion
typedef struct {
    const char* name;
    const char* version;
    int (*init)(void);
    int (*process)(void* data);
    int (*cleanup)(void);
} plugin_interface_t;

// SAO expansion support
typedef struct {
    uint8_t i2c_address;
    const char* device_name;
    int (*init_device)(uint8_t address);
    int (*read_data)(uint8_t address, uint8_t* data, size_t len);
    int (*write_data)(uint8_t address, const uint8_t* data, size_t len);
} sao_device_t;
```

### Over-the-Air Updates
```c
// Firmware update framework
typedef struct {
    uint32_t version;
    uint32_t crc32;
    uint32_t size_bytes;
    uint8_t signature[64];
} firmware_header_t;

int firmware_check_for_updates(void);
int firmware_download_update(const char* url);
int firmware_verify_signature(firmware_header_t* header);
int firmware_apply_update(void);
```

### Advanced SSTV Features
```c
// Future SSTV enhancements
int sstv_add_custom_mode(sstv_format_t* format);
int sstv_enable_error_correction(bool enable);
int sstv_set_image_quality(float quality);
int sstv_enable_progressive_transmission(bool enable);
int sstv_add_metadata(image_t* img, const char* key, const char* value);
```

---

## 📚 API Reference Summary

### Core System Functions
| Function | Purpose | Core |
|----------|---------|------|
| `system_init()` | System initialization | 0 |
| `power_manager_update()` | Power state management | 0 |
| `ui_handle_event()` | User interface events | 0 |
| `sstv_process_audio()` | SSTV signal processing | 1 |
| `camera_capture_image()` | Image acquisition | 1 |
| `audio_process_buffer()` | Audio DSP | 1 |

### Hardware Abstraction Layer
| Module | Key Functions | Description |
|--------|---------------|-------------|
| GPIO | `gpio_init()`, `gpio_set_dir()`, `gpio_put()` | Pin control |
| SPI | `spi_init()`, `spi_write_read_blocking()` | Serial peripheral |
| I2C | `i2c_init()`, `i2c_write_blocking()` | Two-wire interface |
| UART | `uart_init()`, `uart_puts()` | Serial communication |
| I2S | `i2s_init()`, `i2s_write()` | Audio data interface |

### Application Services
| Service | Key Functions | Description |
|---------|---------------|-------------|
| SSTV Engine | `sstv_encode_image()`, `sstv_decode_audio()` | Digital mode processing |
| File Manager | `fs_save_image()`, `fs_load_config()` | Storage operations |
| UI System | `ui_show_menu()`, `ui_update_display()` | User interface |
| Radio Control | `sa818_set_frequency()`, `sa818_enable_tx()` | RF management |

---

## 📖 Documentation Standards

All firmware code should follow these documentation standards:

### Function Documentation
```c
/**
 * @brief Encode image data into SSTV audio format
 * 
 * Converts a RGB image into SSTV audio samples according to the specified
 * mode timing and format. The resulting audio can be transmitted via the
 * SA818 radio module.
 * 
 * @param[in] image Pointer to source image structure
 * @param[in] mode SSTV mode (Martin1, Scottie1, etc.)
 * @param[out] audio_out Pointer to audio buffer for output
 * @return 0 on success, negative error code on failure
 * 
 * @note This function allocates memory for audio_out->samples
 * @warning Caller must free audio buffer when done
 * 
 * @example
 * ```c
 * image_t my_image = {320, 240, IMAGE_FORMAT_RGB565, image_data};
 * audio_buffer_t audio;
 * if (sstv_encode_image(&my_image, SSTV_MARTIN_1, &audio) == 0) {
 *     // Transmit audio buffer
 *     radio_transmit_audio(&audio);
 *     free(audio.samples);
 * }
 * ```
 */
int sstv_encode_image(image_t* image, sstv_mode_t mode, audio_buffer_t* audio_out);
```

### Code Style Guidelines
- **Indentation**: 4 spaces (no tabs)
- **Naming**: Snake_case for functions and variables
- **Constants**: UPPER_CASE with underscores
- **Line Length**: Maximum 100 characters
- **Comments**: Doxygen-style for functions, inline for complex logic

---

**🎯 This completes the comprehensive firmware architecture documentation for the DEFCON SSTV Badge. The design provides a robust foundation for real-time SSTV processing while maintaining responsive user interaction and reliable system operation.**

*73s!* 📡