This is an I2C Master bit bang implementation tested on Raspberry Pi 5. 

This example uses lgpiod library so make sure to install it:
    sudo apt-get install gpiod libgpiod-dev
    
For lgpiod documentation, you can check here: https://libgpiod-dlang.dpldocs.info/gpiod.html

The main.c file can be modified to send commands to your target slave. 

This design is portable to older Raspberry Pi boards. Just change the chip number below with the appropriate chip number for your board:
![image](https://github.com/user-attachments/assets/f56f8621-0472-444d-adae-576b720c9b45)

This was ported and raspberry pi 3 by using "gpiochip0".

To run the design on your raspberry pi:
1. Run makefile to compile the executable files
2. Run the program using ./rhodz_i2c




This I2C master is tested on MachXO3LF Development Board. Sample Waveform transactions are below:

Device ID Read.
![image](https://github.com/user-attachments/assets/45788e82-9bba-499b-8286-dd786dbf65ba)

Feature Bits Read:
![image](https://github.com/user-attachments/assets/0c426385-ac5d-4f31-a5bd-7255751ce730)

Sample Readout using the terminal:
![I2C_screenshot](https://github.com/user-attachments/assets/b9b48c86-c262-4cce-a814-0239aac6855e)


  

