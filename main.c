#include <stdio.h>
#include <gpiod.h>
#include <unistd.h>
#include "rbpi-i2c.h"
#include "global.h"

struct gpiod_line *i2c_sda;
struct gpiod_line *i2c_scl;
 
int main() {

    unsigned char add_read[1]={0x81}; //read                                            

    
    int length=40;
    unsigned char data_to_send[5]={0x80,0xE0,0x00,0x00,0x00}; //device ID
    unsigned char data_to_send2[5]={0x80,0xC6,0x08,0x00,0x00}; //ISC_ENABLE Targetting internal flash
    unsigned char data_to_send3[5]={0x80,0xFB,0x00,0x00,0x00}; // Feabits read
    unsigned char data_to_read [4];
    unsigned char data_to_read2[2];
    
    
    
    i2c_init(&i2c_sda,&i2c_scl);
  
    i2c_start();
    i2c_write_byte(length,data_to_send); 
    i2c_start();
    i2c_write_byte(1,add_read); 
    i2c_read_bytes(32, data_to_read, 0);
    i2c_stop();
    
    printf("Device ID Read \n");
    for (int r=0;r<4;r++){
        printf("Read byte: 0x%02X\n", data_to_read[r]);
    }
    
 


    return 0;
}
