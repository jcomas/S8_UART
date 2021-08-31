# S8_UART
S8 Library for serial communication (UART)

This library is for Senseair S8 CO2 sensor to use with Arduino framework using serial communication (UART).

Senseair S8 LP (004-0-0053) is recommended for CO2 measurement in indoors for air quality application.



## Installation

### PlatformIO

Download S8_UART library and copy extracted folder into PlatformIO\Projects folder. Open Visual Studio Code and select the directory of the project. Modify platformio.ini file according your board and requirements. Build and upload the example selected of platformio.ini file.

### Arduino IDE

Go to Documents\Arduino\libraries and create the **S8_UART** directory. Download S8_UART library and copy the files of **src** folder into the new created folder.

To test the examples, copy the folder of the example into Documents\Arduino\Projects folder, rename the **.cpp** file of the example to **.ino**. Open the .ino file and modify the parameters of **configuration section**. In the menu of IDE select your board and compile and upload the program.