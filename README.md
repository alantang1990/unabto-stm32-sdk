# uNabto STM32 SDK

## Description

This demo shows how to create a simple [AppMyProduct](http://www.appmyproduct.com/) application for a STM32 MCU using the STM32Cube embedded software libraries including FreeRTOS and the LwIP TCP/IP stack. The device can be controlled by the [Heat Control Ionic](https://github.com/nabto/ionic-starter-nabto) starter app. Also check out the article about this demo on [blog.nabto.com](https://blog.nabto.com/2016/04/24/nabto-freertos-running-on-cortex-m7-using-the-st-stm32f746g-disco-board/).

## Directory contents

```
nabto-stm32-demo/
├── Inc/
│  ├── app_ethernet.h               Header of Src/app_ethernet.c file
│  ├── demo_application.h           Heat-pump demo header
│  ├── ethernetif.h                 Header of Src/ethernetif.c file
│  ├── fp_acl_flash.h               Header of Src/fp_acld_flash.c
│  ├── FreeRTOSConfig.h             FreeRTOS configuration
│  ├── lcd_log_conf.h               LCD Log configuration
│  ├── lwipopts.h                   LwIP stack configuration
│  ├── main.h                       Header of Src/main.c file
│  ├── stm32f7xx_hal_conf.h         HAL configuration
│  ├── stm32f7xx_it.h               STM32 interrupt handlers header
│  ├── unabto_config.h              uNabto configuration
│  ├── unabto_main.h                Header of Src/unabto_main.c file
│  ├── unabto_platform.h            Defines platform specific dependencies
│  └── unabto_platform_types.h      Defines platform specific types needed by uNabto
│
├── Src/
│  ├── app_ethernet.c               DHCP thread
│  ├── dns_adapter.c                uNabto DNS adapter implementation
│  ├── ethernetif.c                 Interfacing LwIP to ETH driver
│  ├── fp_acl_flash.c               Pairing config flash database
│  ├── log_adapter.c                uNabto log adapter implementation
│  ├── main.c                       Main program
│  ├── network_adapter.c            uNabto network adapter implementation
│  ├── random_adapter.c             uNabto random adapter implementation
│  ├── stm32f7xx_hal_timebase_tim.c HAL time
│  ├── stm32f7xx_it.c               STM32 interrupt handlers
│  ├── system_stm32f7xx.c           STM32F7xx system clock configuration file
│  ├── time_adapter.c               uNabto time adapter implementation
│  ├── unabto_application.c         Heat-pump demo application
│  └── unabto_main.c                uNabto main thread
│
├── STM32CubeF7/                    Folder for STM32CubeF7
│  ├── Documentation
│  ├── Drivers
│  ├── [...]
│  └── README.md
│
├── SW4STM32/                       System Workbench for STM32 (SW4STM32) project
└── unabto/                         uNabto server
```
  
## Hardware and Software environment

- This demo runs on STM32F746xx devices.
- The demo has been compiled with the [System Workbench for STM32 _(SW4STM32)_ (v1.13.1)][sw4stm32] and tested on a STM32746G&#x2011;DISCO board

## How to use?

1. Clone this repository with `git clone --recursive https://github.com/nabto/unabto-stm32-sdk.git`
2. Download the [STM32CubeF7 (v1.6.0)][stm32cubef7] package and place it in the [`/STM32CubeF7`](/STM32CubeF7) folder.
3. Import the project from [`/SW4STM32/NABTO_STM32`](/SW4STM32/NABTO_STM32) into your [System Workbench for STM32 _(SW4STM32)_][sw4stm32] workspace.
4. Insert your Device ID and the preshared key from [portal.appmyproduct.com](http://portal.appmyproduct.com/) in [`/Src/unabto_main.c`](/Src/unabto_main.c). 
5. Build project and transfer the image to the device memory.
6. Connect the device to the ethernet.
7. Run the demo and connect from your [Heat Control Ionic](https://github.com/nabto/ionic-starter-nabto) starter app. You should be able to turn the LCD display on and off using the _Heatpump active_ switch and control the heat-pump parameters shown at the bottom of the display.
8. If you want to reset the device config you can perform a factory reset by pressing the _User_ button at startup until you see _FACTORY RESET_ printed to the display.

[stm32cubef7]: http://www2.st.com/content/st_com/en/products/embedded-software/mcus-embedded-software/stm32-embedded-software/stm32cube-embedded-software/stm32cubef7.html
[sw4stm32]: http://www.st.com/sw4stm32
