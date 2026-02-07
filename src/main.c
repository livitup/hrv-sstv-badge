/**
 * DEFCON SSTV Badge - Main Firmware Entry Point
 * 
 * This is your introduction to embedded C! Let's build this step by step.
 * 
 * Key embedded C concepts we'll learn:
 * 1. Memory-mapped I/O and registers
 * 2. Real-time constraints and timing
 * 3. Hardware abstraction layers (HAL)
 * 4. Resource sharing between dual cores
 * 5. Interrupt-driven programming
 */

#include "pico/stdlib.h"        // Core Pico SDK functions
#include "hardware/gpio.h"      // GPIO control functions
#include "hardware/spi.h"       // SPI peripheral control
#include "hardware/i2c.h"       // I2C peripheral control
#include "hardware/uart.h"      // UART peripheral control
#include "pico/multicore.h"     // Dual-core functions
#include <stdio.h>              // Standard I/O for printf

/**
 * Hardware Pin Definitions
 * 
 * In embedded C, we use #define for constants instead of variables.
 * This is because the preprocessor replaces these at compile time,
 * making the code more efficient - no RAM used for storage!
 */

// Status LED - we'll use the built-in LED first
#define STATUS_LED_PIN      25

// SA818 Radio Module Control (UART0)
#define SA818_UART_TX       4
#define SA818_UART_RX       5
#define SA818_UART_ID       uart0
#define SA818_BAUD_RATE     9600

// Display Interface (SPI1) - ST7789 TFT Controller
#define DISPLAY_CS          16      // Chip Select
#define DISPLAY_DC          19      // Data/Command
#define DISPLAY_RST         18      // Reset
#define DISPLAY_BL          20      // Backlight
#define DISPLAY_SPI_ID      spi1

// Camera Interface (I2C0) - OV2640 Camera
#define CAMERA_I2C_SDA      2
#define CAMERA_I2C_SCL      3
#define CAMERA_I2C_ID       i2c0
#define CAMERA_I2C_FREQ     100000  // 100kHz

// Audio Codec Interface (I2C1) - WM8960
#define CODEC_I2C_SDA       6
#define CODEC_I2C_SCL       7
#define CODEC_I2C_ID        i2c1
#define CODEC_I2C_FREQ      400000  // 400kHz

/**
 * Function Declarations
 * 
 * In C, we need to declare functions before using them.
 * This is different from Python where functions can be called
 * after they're defined in the same file.
 */
bool init_gpio(void);
bool init_uart(void);
bool init_spi(void);
bool init_i2c(void);
bool init_hardware_subsystems(void);
void core1_main(void);
void blink_status_led(int count, int delay_ms);

/**
 * Hardware Initialization Functions
 * 
 * In embedded systems, we break initialization into logical groups.
 * Each function returns bool - true for success, false for failure.
 * This is a common embedded C pattern for error handling.
 */

bool init_gpio(void) {
    printf("Initializing GPIO...\n");
    
    // Status LED
    gpio_init(STATUS_LED_PIN);
    gpio_set_dir(STATUS_LED_PIN, GPIO_OUT);
    gpio_put(STATUS_LED_PIN, 0);  // Start with LED off
    
    // Display control pins
    gpio_init(DISPLAY_CS);
    gpio_set_dir(DISPLAY_CS, GPIO_OUT);
    gpio_put(DISPLAY_CS, 1);      // CS high = not selected
    
    gpio_init(DISPLAY_DC);
    gpio_set_dir(DISPLAY_DC, GPIO_OUT);
    
    gpio_init(DISPLAY_RST);
    gpio_set_dir(DISPLAY_RST, GPIO_OUT);
    gpio_put(DISPLAY_RST, 1);     // Not in reset
    
    gpio_init(DISPLAY_BL);
    gpio_set_dir(DISPLAY_BL, GPIO_OUT);
    gpio_put(DISPLAY_BL, 0);      // Backlight off initially
    
    printf("  ✓ GPIO initialized\n");
    return true;
}

bool init_uart(void) {
    printf("Initializing UART for SA818 radio...\n");
    
    // Initialize UART with our baud rate
    uart_init(SA818_UART_ID, SA818_BAUD_RATE);
    
    // Set the GPIO pin functions for UART
    gpio_set_function(SA818_UART_TX, GPIO_FUNC_UART);
    gpio_set_function(SA818_UART_RX, GPIO_FUNC_UART);
    
    // Optional: Configure UART parameters (8N1 is default)
    uart_set_hw_flow(SA818_UART_ID, false, false);  // No hardware flow control
    uart_set_format(SA818_UART_ID, 8, 1, UART_PARITY_NONE);
    
    printf("  ✓ UART0 initialized at %d baud\n", SA818_BAUD_RATE);
    return true;
}

bool init_spi(void) {
    printf("Initializing SPI for display...\n");
    
    // Initialize SPI at 10MHz (safe speed for display)
    spi_init(DISPLAY_SPI_ID, 10 * 1000 * 1000);
    
    // Set GPIO functions for SPI pins
    // Note: CS pin is handled manually as GPIO
    gpio_set_function(16, GPIO_FUNC_SPI);  // MISO (not used for display)
    gpio_set_function(17, GPIO_FUNC_SPI);  // CS (but we use manual CS)
    gpio_set_function(18, GPIO_FUNC_SPI);  // SCK
    gpio_set_function(19, GPIO_FUNC_SPI);  // MOSI
    
    printf("  ✓ SPI1 initialized at 10 MHz\n");
    return true;
}

bool init_i2c(void) {
    printf("Initializing I2C buses...\n");
    
    // I2C0 for camera
    i2c_init(CAMERA_I2C_ID, CAMERA_I2C_FREQ);
    gpio_set_function(CAMERA_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(CAMERA_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(CAMERA_I2C_SDA);  // Enable pull-ups for I2C
    gpio_pull_up(CAMERA_I2C_SCL);
    
    // I2C1 for audio codec
    i2c_init(CODEC_I2C_ID, CODEC_I2C_FREQ);
    gpio_set_function(CODEC_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(CODEC_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(CODEC_I2C_SDA);
    gpio_pull_up(CODEC_I2C_SCL);
    
    printf("  ✓ I2C0 initialized at %d Hz (camera)\n", CAMERA_I2C_FREQ);
    printf("  ✓ I2C1 initialized at %d Hz (codec)\n", CODEC_I2C_FREQ);
    return true;
}

bool init_hardware_subsystems(void) {
    printf("\n=== DEFCON SSTV Badge Hardware Initialization ===\n");
    
    // Initialize each subsystem - if any fails, we stop
    if (!init_gpio()) {
        printf("❌ GPIO initialization failed!\n");
        return false;
    }
    
    if (!init_uart()) {
        printf("❌ UART initialization failed!\n");
        return false;
    }
    
    if (!init_spi()) {
        printf("❌ SPI initialization failed!\n");
        return false;
    }
    
    if (!init_i2c()) {
        printf("❌ I2C initialization failed!\n");
        return false;
    }
    
    // Success pattern - blink LED 3 times
    blink_status_led(3, 200);
    printf("✅ All hardware subsystems initialized successfully!\n\n");
    return true;
}

/**
 * Utility function for visual feedback
 * Shows how embedded C handles timing and hardware control
 */
void blink_status_led(int count, int delay_ms) {
    for (int i = 0; i < count; i++) {
        gpio_put(STATUS_LED_PIN, 1);
        sleep_ms(delay_ms);
        gpio_put(STATUS_LED_PIN, 0);
        sleep_ms(delay_ms);
    }
}

/**
 * Core 1 Entry Point
 * 
 * The RP2350 has dual ARM Cortex-M33 cores. We'll use:
 * - Core 0: UI, file system, system management
 * - Core 1: SSTV processing, audio, camera, radio
 */
void core1_main(void) {
    printf("Core 1 started - will handle SSTV processing\n");
    
    // For now, just indicate core 1 is alive
    while (true) {
        sleep_ms(5000);  // Sleep for 5 seconds
        printf("Core 1 heartbeat - SSTV processing ready\n");
    }
}

/**
 * Main Function - Core 0 Entry Point
 * 
 * This is where execution begins when the RP2350 starts up.
 * The boot process goes: Boot ROM → Boot2 → main()
 */
int main() {
    // Initialize USB serial for debugging
    // This is crucial for embedded development - always set up debug output first!
    stdio_init_all();
    
    // Give USB time to enumerate
    sleep_ms(2000);
    
    printf("\n🚀 DEFCON SSTV Badge Starting Up...\n");
    printf("ARM GCC Version: %s\n", __VERSION__);
    printf("Pico SDK Version: %s\n", PICO_SDK_VERSION_STRING);
    printf("Target: RP2350 (ARM Cortex-M33 dual core)\n");
    
    // Initialize all hardware subsystems
    if (!init_hardware_subsystems()) {
        printf("💥 Hardware initialization failed - system halted\n");
        // In embedded systems, if hardware init fails, we usually halt
        while (true) {
            blink_status_led(1, 100);  // Fast blink = error
            sleep_ms(1000);
        }
    }
    
    // Launch Core 1 for signal processing
    printf("🚀 Launching Core 1 for SSTV processing...\n");
    multicore_launch_core1(core1_main);
    
    // Core 0 main loop - handles UI and system management
    printf("📱 Core 0 main loop - handling UI and system management\n");
    
    int loop_counter = 0;
    while (true) {
        // Heartbeat LED - slow pulse
        gpio_put(STATUS_LED_PIN, 1);
        sleep_ms(100);
        gpio_put(STATUS_LED_PIN, 0);
        sleep_ms(1900);
        
        // Periodic status message
        loop_counter++;
        if (loop_counter % 10 == 0) {
            printf("Core 0 heartbeat - system running normally (loop %d)\n", loop_counter);
        }
    }
    
    return 0;  // Never reached in embedded systems
}