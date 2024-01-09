<!-- Please do not change this logo with link -->

[![MCHP](images/microchip.png)](https://www.microchip.com)

# Getting Started with the 8-Bit MDFU Client for PIC16F18446 (MPLAB X)

This repository provides the MPLAB® X projects needed to configure some basic MDFU bootloader solutions for the PIC16F18446 Curiosity Nano Evaluation Board using code generated from the MPLAB® Code Configurator (MCC).

The MDFU Ecosystem is a device firmware update ecosystem that is enabled through a File-Based Transfer Protocol (FTP) and a custom image format. This repository provides the basic starting point to configure and customize the 8-Bit MDFU Client library on the PIC16F18446 Curiosity Nano and also provides instructions for running the examples.

## Related Documentation

- [PIC16F18446 Family Product Page](https://www.microchip.com/en-us/products/microcontrollers-and-microprocessors/8-bit-mcus/pic-mcus/pic16f18446)
- [8-Bit MDFU Client Getting Started](https://onlinedocs.microchip.com/oxy/GUID-67539092-2179-43C1-8600-118A85E49693-en-US-2/index.html)
- [8-Bit MDFU Client v1.0.0-beta.1 Release Note](https://onlinedocs.microchip.com/oxy/GUID-9CE46BA1-9CDF-4B9C-9C73-4793B27ED320-en-US-2/index.html)
- [8-Bit MDFU Client v1.0.0-beta.1 API Documentation](https://onlinedocs.microchip.com/oxy/GUID-01EC9542-92FF-436D-91FB-7CBEEE66D701-en-US-2/index.html)

## Software Used

- MPLAB® X IDE 6.15.0 or newer [(MPLAB® X IDE 6.15)](https://www.microchip.com/en-us/development-tools-tools-and-software/mplab-x-ide?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_MMTCha_MPAE_Examples&utm_content=pic16f18446-cnano-8bit-mdfu-client-mplab-mcc-github)
- MPLAB® XC8 2.45.0 or newer compiler [(MPLAB® XC8 2.45)](https://www.microchip.com/en-us/development-tools-tools-and-software/mplab-xc-compilers?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_MMTCha_MPAE_Examples&utm_content=pic16f18446-cnano-8bit-mdfu-client-mplab-mcc-github)
- MPLAB® Code Configurator (MCC) 5.4.1 or newer [(microchip.com/mplab/mplab-code-configurator)](https://www.microchip.com/mplab/mplab-code-configurator)
- MPLAB® Code Configurator (MCC) Device Libraries PIC10 / PIC12 / PIC16 / PIC18 MCUs [(microchip.com/mplab/mplab-code-configurator)](https://www.microchip.com/mplab/mplab-code-configurator)
- Python 3.8 or later [(Python)](https://www.python.org/downloads/)
- pyfwimagebuilder v1.0.1.14 or newer [(pyfwimagebuilder)](https://pypi.org/project/pyfwimagebuilder/)
- pymdfu v1.0.1.5 or newer[(pymdfu)](https://pypi.org/project/pymdfu/)

## Hardware Used

- PIC16F18446 Curiosity Nano [(DM164144)](https://www.microchip.com/en-us/development-tool/DM164144)

[![PIC16F18446_CNano](images\190702-MCU8-PHOTO-DM164144-Front-Transparent_crop.png)](images\190702-MCU8-PHOTO-DM164144-Front-Transparent_crop.png)

## Setup
The following project setup steps have been followed for each example project pair. If something goes wrong in running these examples, validate that the settings in your project are consistent with the options seen in this section.

### Client Setup
[![mdfu-builder](images\ProjectConfigurationOverview.PNG)](images\ProjectConfigurationOverview.PNG)

**Configuration Bits**
- External Oscillator Selection bits: **Oscillator not enabled**
- Reset Oscillator Selection bits: **HFINTOSC (1MHz)**

[![CFG](images\ConfigBitsSetup.PNG)](images\ConfigBitsSetup.PNG)

**Clock Control**
- Clock Source: **HFINTOSC**
- HF Internal Clock: **4_MHz**
- Clock Divider: **1**

[![CLK](images\ClockSetup.PNG)](images\ClockSetup.PNG)

**NVM**
- Generate Flash APIs: **Enabled**
- Generate Device ID APIs: **Enabled**

[![NVM](images\NVMSetup.PNG)](images\NVMSetup.PNG)

**UART**
- Custom Name: **UART1**
- Requested Baudrate: **9600**
- Calculated Baudrate: **9615**
- Baud Rate Error (%): **0.16**
- Parity: **None**
- Data Size: **8**
- Stop Bits: **1**
- Flow Control Mode: **None**
- Redirect Printf to UART: **Disabled**
- Interrupt Driven: **Disabled**

[![UART](images\UARTDriverSetup.PNG)](images\UARTDriverSetup.PNG)

**EUSART PLIB**
- Actual Baud Rate: **9615.385**
- Baud Rate Error (%): **0.16**
- Receive Enable: **Enabled**
- Clock/Transmit Polarity: **Non-Inverted**
- Serial Port Enable: **Enabled**
- Transmit Enable: **Enabled**

[![UART_PLIB](images\UARTPLIBSetup.PNG)](images\UARTPLIBSetup.PNG)

**UART Pins**
- EUSART TX: **RB4**
- EUSART RX: **RB6**

[![UART_Pins](images\UARTPortsSetup.PNG)](images\UARTPortsSetup.PNG)

**8-Bit MDFU Client**
- Communication Protocol: **UART**
- Application Start Address: **Different for each project based on the verification selected**
- Device ID: **0x30D4**
- I/O Pin Indicator: **Enabled**
- I/O Pin Entry: **Enabled**
- Memory Verification: **Assigned Based on Example Project Naming Convention**

*Example for Checksum Verification*

[![MDFU](images\MDFUClientSetup.PNG)](images\MDFUClientSetup.PNG)

**8-Bit MDFU Client I/O**
- BOOT INDICATE: **RA2**
- BOOT ENTRY: **RC2**

[![IO-Pins](images\IOPortSetup.PNG)](images\IOPortSetup.PNG)
- BOOT INDICATE: **Start High**
- BOOT ENTRY: **Weak Pullup**

[![IO-Settings](images\IOPinsSetup.PNG)](images\IOPinsSetup.PNG)

**8-Bit MDFU Client Project Properties**
- ROM Ranges: **This option is configured based on the start address of the application.**
- For example, if the application starts at 0xC00 then this value will reflect as `00-7FF,800-BFF`.

[![IO-Settings](images\ProjectProperties.PNG)](images\ProjectProperties.PNG)

### Application Setup
[![app-builder](images\AppConfigurationOverview.PNG)](images\AppConfigurationOverview.PNG)

**IO Pins**
- GPIO Output: **RA2**

[![app_io](images\AppPortSetup.PNG)](images\AppPortSetup.PNG)
- Custom Name: **LED**

[![app_io_settings](images\LEDSetup.PNG)](images\LEDSetup.PNG)

**Project Properties**

Linker Additional Options
- Codeoffset: **0x<APP_START> = 0xC00**
- Checksum: **Dependant on the verification scheme**
*Check the table below to understand how the Checksum option must be configured in the application projects*

[![app_offset](images\AppOffset.PNG)](images\AppOffset.PNG)

|Verification Scheme | Checksum Setting |
|---- |---- |
|Reset Vector | N/A |
|Status Byte | N/A |
|Checksum | C00-3FFD@3FFE,width=-2,algorithm=2,code=3F |
|CRC-16 | C00-3FFD@3FFE,width=-2,algorithm=5,offset=FFFF,polynomial=1021,code=3F |
|CRC-32 | C00-3FFB@3FFC,width=-4,algorithm=-5,offset=FFFFFFFF,polynomial=04C11DB7,code=3F |

Fill Flash Memory
- Which area to fill: **Provide Range to fill**
- How to fill it: **Constant or incremental value**
- Constant: **0x3FFF**
- Increment/Decrement: **No Incrementing**
- Memory address range: **0x<APP_START>:0x<FLASH_END> = 0xC00:0x3FFF**

[![app_fill](images\AppFill.PNG)](images\AppFill.PNG)

## Operation
In this section, we will walkthrough how to run the examples that are housed in this repository. For this example walkthrough we will execute the Checksum verification example and update the device flash memory with the checksum application image to demonstrate a successful device firmware update (DFU).

**8-Bit MDFU Client Operation**

1. Open the MDFU Client Project.

[![OpenMDFU](images\openBtnMDFU.png)](images\openBtnMDFU.png)

[![OpenMDFUProject](images\openProjectMDFU.png)](images\openProjectMDFU.png)

2. Set MDFU Client Project as Main Project.

[![OpenMDFUProject](images\setAsMainProject.png)](images\setAsMainProject.png)

3. Right click, then select Clean and Build.

[![CleanBuild](images\CleanAndBuildMDFU.png)](images\CleanAndBuildMDFU.png)

4. Program the MDFU Client Project

[![ProgramMDFU](images\ProgramMDFU.png)](images\ProgramMDFU.png)

**Bootloader Operation after initial programming**

After initial programming, the LED should be held on.
[![MDFU_BootMode](images\PIC16F18446_BootMode.png)](images\PIC16F18446_BootMode.png)

**Application Operation**
1. Open the Application Project that is configured for your selected verification scheme.

[![OpenApp](images\openBtnApp.png)](images\openBtnApp.png)

[![OpenAppProject](images\openProjectApp.png)](images\openProjectApp.png)

2. Set the application project as the Main Project.

[![MainAppProject](images\setAppAsMainProject.png)](images\setAppAsMainProject.png)

3. Build the required Application project.

Right click, then select Clean and Build

[![CleanBuild_App](images\CleanAndBuildApp.png)](images\CleanAndBuildApp.png)

4. Build the Application Image File using **pyfwimagebuilder**

*Hint: The configuration TOML file is generated by the MDFU Client project*

**Example Command:**

`pyfwimagebuilder build -i "application_hex_file.hex"  -c "mdfu_config_file.toml" -o output.img`

[![build_img](images\BuildTheImage.PNG)](images\BuildTheImage.PNG)

5. Use the **pymdfu** host tool to transfer the application image file to the bootloader.

*Hint: You can find the COM port of the MCU using the MPLAB Data Visualizer.*

**Example Command:**

`pymdfu update serial ./output.img --baudrate 9600 --port COM##`

[![transfer_img](images\SendTheImage.PNG)](images\SendTheImage.PNG)

**Application Has Been Updated Successfully**

[![MDFU_AppMode](images\PIC16F18446_AppMode.gif)](images\PIC16F18446_AppMode.gif)

## Summary

This repository demonstrates how to configure the 8-Bit MDFU Client library in MCC to enable device firmware updates over UART on a PIC16F18446 Curiosity Nano.
