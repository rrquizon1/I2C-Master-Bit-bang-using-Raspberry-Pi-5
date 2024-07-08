#include <stdio.h>
#include <gpiod.h>
#include <unistd.h>
#include "rbpi-i2c.h"
#include "global.h"
#include <stdio.h>


/*Written by Rhodz */


char a='A';// Dummy character needed for lbgpiod functions
int pull_up=1; //to enable pull up. It is always enabled by default

/* configure_input() sets the pin to Hi-Z emulating open-drain behavior. During this mode the pin will rely on the pull up to go high
 * This is to emulate behavior of an i2c open-drain pin
 * 
 * */

int configure_input(struct gpiod_line *line, const char *consumer, int pull_up)
{
    struct gpiod_line_request_config config = {
        .consumer = consumer,
        .request_type = GPIOD_LINE_REQUEST_DIRECTION_INPUT,
        .flags = pull_up ? GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP : GPIOD_LINE_REQUEST_FLAG_BIAS_DISABLE
    };

    if (gpiod_line_request(line, &config,1) < 0)
    {
        perror("Request line as input failed");
        return -1;
    }

    return 0;
}
/* configure_output() sets the pin to output low emulating behavior of open drain pin when pulled low.
 * 
 * */
int configure_output(struct gpiod_line *line, const char *consumer, int value)
{
    if (gpiod_line_request_output(line, consumer, value) < 0)
    {
        perror("Request line as output failed");
        return -1;
    }

    return 0;
}

/* i2c_init() initializes the i2c bus and preparation for the start condition
 * 
 * 
 * */
int i2c_init(struct gpiod_line **i2c_sda,struct gpiod_line **i2c_scl){
    struct gpiod_chip *chip;

    
    
    int offset_sda = 20; // Replace with your GPIO pin number for SDA
    int offset_scl = 16; // Replace with your GPIO pin number for SCL
    
    chip = gpiod_chip_open("/dev/gpiochip4"); // Replace 4 with the appropriate chip number

    if (!chip) {
        perror("Open chip failed");
        return 1;
    }

    *i2c_sda= gpiod_chip_get_line(chip, offset_sda);
    *i2c_scl= gpiod_chip_get_line(chip, offset_scl);

    if (!*i2c_sda) {
        perror("Get line failed");
        gpiod_chip_close(chip);
        return 1;
    }
    
    if (!*i2c_scl) {
        perror("Get line failed");
        gpiod_chip_close(chip);
        return 1;
    }
    configure_input(*i2c_sda,&a,pull_up);
    configure_input(*i2c_scl,&a,pull_up);
    usleep(10);
    gpiod_line_release(*i2c_sda);
    usleep(10);
    gpiod_line_release(*i2c_scl);
    usleep(10);
    return 0;
    
}
/* i2c_start() sets the i2c start condition*/

void i2c_start() {

    configure_input(i2c_sda, &a, pull_up);
    configure_input(i2c_scl, &a, pull_up);
    usleep(10);
    gpiod_line_release(i2c_sda);
    gpiod_line_release(i2c_scl);
    usleep(10);
    configure_output(i2c_sda, &a, 0);
    usleep(10);
    configure_output(i2c_scl, &a, 0);
    usleep(10);
    gpiod_line_release(i2c_sda);
    gpiod_line_release(i2c_scl);
    
}


/* i2c_write_byte() sends the data and checks slave acknowledgement*/

int i2c_write_byte(int length, unsigned char *data_bytes) {
    int bytes = (length + 7) / 8;
    
    for (int j=0;j<bytes;j++){
        unsigned char data=data_bytes[j];
    
    for (int i = 0; i < 8; i++) {
        if (data & 0x80) {
            gpiod_line_release(i2c_sda);
            configure_input(i2c_sda, &a, pull_up); // Release SDA (high)
        } else {
            gpiod_line_release(i2c_sda);
            configure_output(i2c_sda, &a, 0); // Drive SDA low
        }
        //delay(0.5);
        gpiod_line_release(i2c_scl);
        configure_input(i2c_scl, &a, pull_up); // Clock high
        //delay(0.5);
        gpiod_line_release(i2c_scl);
        configure_output(i2c_scl, &a, 0); // Clock low
        //delay(0.5);

        data <<= 1; // Shift data left by 1 bit
    }

    // Check for ACK/NACK
    gpiod_line_release(i2c_sda);
    configure_input(i2c_sda, &a, pull_up);  // Release SDA for ACK/NACK
    usleep(10);

    gpiod_line_release(i2c_scl);
    configure_input(i2c_scl, &a, pull_up); // Clock high
    usleep(10);

    int ack = !gpiod_line_get_value(i2c_sda); // Check if SDA is low (ACK)
    usleep(10);
    
    if(ack!=1){
        return -1;
    }
    gpiod_line_release(i2c_scl);
    configure_output(i2c_scl, &a, 0); // Clock low
    usleep(10);

    gpiod_line_release(i2c_scl);
    gpiod_line_release(i2c_sda);
    
    }

    return 0;
}

/*i2c_stop() sends stop condition*/

void i2c_stop()
{
    // Ensure SDA is low
    gpiod_line_release(i2c_sda);
    configure_output(i2c_sda, &a, 0);
    usleep(1);

    // Clock SCL high
    gpiod_line_release(i2c_scl);
    configure_input(i2c_scl, &a, pull_up);
    usleep(1);

    // Release SDA to high
    gpiod_line_release(i2c_sda);
    configure_input(i2c_sda, &a, pull_up);
    usleep(1);

    // Release SCL
    
    gpiod_line_release(i2c_scl);
    gpiod_line_release(i2c_sda);
}

/*i2c_read_bit() used for reading input during read out*/

int i2c_read_bit() {
    int bit;
    configure_input(i2c_sda, &a, pull_up);

    configure_input(i2c_scl, &a, pull_up);

    bit = gpiod_line_get_value(i2c_sda);

    gpiod_line_release(i2c_scl);
    configure_output(i2c_scl, &a, 0);

    gpiod_line_release(i2c_scl);
    gpiod_line_release(i2c_sda);
    return bit;
}




/* send acknowledge bit*/
void i2c_send_ack() {
    // Ensure SDA is low (ACK)
    configure_output(i2c_sda, &a, 0);
   // usleep(1);

    // Clock SCL high
    configure_input(i2c_scl, &a, pull_up);
   // usleep(1);

    // Clock SCL low
    gpiod_line_release(i2c_scl);
    configure_output(i2c_scl, &a, 0);
    //usleep(1);

    // Release SDA
    gpiod_line_release(i2c_sda);
    gpiod_line_release(i2c_scl);
}

/* send nack */

void i2c_send_nack() {
    gpiod_line_release(i2c_scl);
    gpiod_line_release(i2c_sda);
   // usleep(1);
    configure_input(i2c_sda, &a, pull_up); // Set SDA high to send NACK
    //usleep(1);
    configure_input(i2c_scl, &a, pull_up); // Set SCL high
    //usleep(1);
    gpiod_line_release(i2c_scl);
    configure_output(i2c_scl, &a, 0); // Set SCL low
    //usleep(1);
    gpiod_line_release(i2c_sda); // Release SDA
    gpiod_line_release(i2c_scl); // Release SCL
}



/* i2c_read_bytes() reads multiple bytes and send ack or nack depending on the command */

int i2c_read_bytes(int length, unsigned char *a_ByteRead, int send_ack) {
    int bytes = (length + 7) / 8; // Calculate the number of bytes to read

    for (int i = 0; i < bytes; i++) {
        unsigned char byte = 0;

        // Read each bit of the byte
        for (int j = 0; j < 8; j++) {
        byte = (byte << 1) | i2c_read_bit();
        
            }
		usleep(10);

        a_ByteRead[i] = byte;

        usleep(10); // Small delay for timing

        // Send ACK for all but the last byte or send NACK if it's the last byte and send_ack is 0
       if (i < bytes - 1 || send_ack) {
            i2c_send_ack();
        } else {
           i2c_send_nack();
        }
    }

    // Release SDA and SCL lines after reading all bytes
    gpiod_line_release(i2c_sda);
    gpiod_line_release(i2c_scl);

    return 0;
}
