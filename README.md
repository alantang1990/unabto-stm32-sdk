# uNabto STM32 SDK

## Description

This demo shows the usage of the [Nabto](http://www.nabto.com/) platform in combination with the STM32Cube embedded software including FreeRTOS and the LwIP TCP/IP stack. Also check out the article about this demo on [blog.nabto.com](https://blog.nabto.com/2016/04/24/nabto-freertos-running-on-cortex-m7-using-the-st-stm32f746g-disco-board/).

## Directory contents

```
nabto-stm32-demo/
├──Inc/
│  ├── app_ethernet.h             header of app_ethernet.c file
│  ├── ethernetif.h               header for ethernetif.c file
│  ├── FreeRTOSConfig.h           FreeRTOS configuration options
│  ├── lcd_log_conf.h             LCD Log configuration file
│  ├── lwipopts.h                 LwIP stack configuration options
│  ├── main.h                     Main program header file
│  ├── nabto.h                    header of nabto.c file
│  ├── stm32f7xx_hal_conf.h       HAL configuration file
│  ├── stm32f7xx_it.h             STM32 interrupt handlers header file
│  ├── unabto_config.h            uNabto configuration
│  ├── unabto_platform.h          Defines platofrm specific dependencies
│  └── unabto_platform_types.h    Defines platofrm specific types needed by uNabto
│
├──Src/
│  ├──app_ethernet.c              Ethernet specific module
│  ├──dns_adapter.c               Nabto DNS adapter implementation
│  ├──ethernetif.c                Interfacing LwIP to ETH driver
│  ├──log_adapter.c               Nabto log adapter implementation
│  ├──main.c                      Main program
│  ├──nabto.c                     Nabto main thread
│  ├──network_adapter.c           Nabto network adapter implementation
│  ├──random_adapter.c            Nabto random adapter implementation
│  ├──stm32f7xx_it.c              STM32 interrupt handlers
│  ├──system_stm32f7xx.c          STM32F7xx system clock configuration file
│  └──time_adapter.c              Nabto time adapter implementation
│
├── STM32CubeF7/                  Folder for STM32CubeF7
├── SW4STM32/                     System Workbench for STM32 (SW4STM32) project
└── unabto/                       uNabto server
```
  
## Hardware and Software environment

- This demo runs on STM32F746xx devices.
- The demo has been compiled with the [System Workbench for STM32 (SW4STM32)][sw4stm32] and tested on a STM32746G&#x2011;DISCO board

## How to use?

1. Download the [STM32CubeF7 (v1.4.0)][stm32cubef7] package and place it in the [`/STM32CubeF7`](/STM32CubeF7) folder.
2. Import the project from [`/SW4STM32/NABTO_STM32`](/SW4STM32/NABTO_STM32) into your [System Workbench for STM32 (SW4STM32)][sw4stm32] workspace.
3. Insert your Nabto ID and the preshared key from [developer.nabto.com](http://developer.nabto.com/) in [`/Src/nabto.c`](/Src/nabto.c). 
4. Define `USE_DHCP` or specify static IP address in [`/Inc/main.h`](/Inc/main.h).
5. Build project and transfer the image to the device memory.
6. Connect the device to the ethernet.
7. Run the demo and connect from your Nabto client. You should be able to switch the device's LCD display on and off.

[stm32cubef7]: http://www2.st.com/content/st_com/en/products/embedded-software/mcus-embedded-software/stm32-embedded-software/stm32cube-embedded-software/stm32cubef7.html
[sw4stm32]: http://www.st.com/sw4stm32
