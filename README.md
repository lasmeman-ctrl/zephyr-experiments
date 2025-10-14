# Zephyr RTOS Experiments

Testing Zephyr fundamentals using the **Adafruit HUZZAH32 (ESP32)**.

##  Experiments
- **UART Shell LED Control:** Toggle onboard LED via UART command  
- **Thread Scheduling:** Run periodic tasks every 500ms  
- **Logging:** Compare `printk()` vs `LOG_INF()`  
- **Button Input:** Detect GPIO interrupts  

##  Stack
- Zephyr RTOS  
- west, CMake  
- VS Code  
- Serial Monitor for logging

## Notes
This repo serves as a lab notebook for understanding Zephyr’s threading, GPIO, and logging systems.
=======
# Zephyr RTOS Experiments

A collection of small projects exploring core concepts in **Zephyr RTOS**, running on the **Adafruit HUZZAH32 (ESP32)** and **NXP FRDM-MCXN947** boards.  
Each project focuses on one subsystem or kernel feature: GPIO, ADC/DAC, threads, queues, UART, CAN bus, and more.

---

## Project Index

| Project | Description | Key Concepts |
|----------|--------------|---------------|
| **adc_demo** | Reads analog input from a potentiometer and prints scaled voltage values. | ADC interface, analog-to-digital conversion, device binding |
| **button_demo** | Detects button presses and toggles an LED using interrupts. | GPIO input interrupts, ISR handling, debounce logic |
| **CAN_BUS_Demo** | Basic CAN frame transmission and reception between nodes. | CAN bus driver API, message framing, peripheral initialization |
| **CAN_internal_temp** | Reads on-chip temperature sensor data and transmits via CAN. | Sensor API, CAN messaging, periodic threads |
| **cmake_demo** | Minimal Zephyr app showing how CMakeLists ties with prj.conf. | Zephyr build system, CMake integration |
| **device_driver** | Skeleton for creating a custom Zephyr driver module. | Device Tree binding, driver initialization routines |
| **display_driver** | Interfaces with an ST7735 0.96" SPI display to render text and data. | SPI, display buffers, frame updates |
| **kconfig_demo** | Demonstrates how custom configuration options are defined in Kconfig. | Kconfig syntax, build-time configuration |
| **led_shell** | Toggle the built-in LED through UART shell commands. | Zephyr shell subsystem, command registration, GPIO control |
| **led_toggler** | Simple periodic LED blink task. | Threads, kernel sleep, GPIO output |
| **multithreading_mutex** | Runs multiple threads accessing shared data safely. | Kernel threads, mutex synchronization |
| **multithreading_test** | Compares thread priorities and scheduling behavior. | Thread scheduling, priority inversion |
| **sensor_queue** | Reads sensor data in one thread and sends to another via message queue. | Message queues, inter-thread communication |
| **temp_sensor** | Interfaces with MCP9808 temperature sensor using I2C. | I2C communication, sensor API, data conversion |

---

## Learning Goals
- Understand **Zephyr’s kernel model** — threads, priorities, and synchronization primitives.  
- Learn **driver development** and how the **Device Tree** and **Kconfig** system integrate hardware and software.  
- Explore **peripheral interfaces** (GPIO, I2C, SPI, ADC/DAC, CAN).  
- Practice clean build organization using `CMakeLists.txt` and `prj.conf`.  

---

## Tools & Environment
- **Boards:** Adafruit HUZZAH32 (ESP32), NXP FRDM-MCXN947  
- **Toolchain:** Zephyr SDK, west, OpenOCD, J-Link, VS Code  
- **Languages:** C, CMake, Kconfig, DeviceTree  
- **OS:** Linux (Ubuntu)  

---

## How to Build & Flash

```bash
# Example: build and flash LED shell demo
west build -b huzzah32 Projects/led_shell
west flash
minicom -D /dev/ttyUSB0 -b 115200
>>>>>>> 51baae0 (init(samples): add projects and README)
