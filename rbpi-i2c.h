#ifndef _RBPI_I2C_H_
#define _RBPI_I2C_H_

int i2c_init();
void i2c_start();
int i2c_write_byte(int length, unsigned char *data_bytes);
void i2c_stop();
void i2c_send_ack();
void i2c_send_nack();
int i2c_read_bytes(int length, unsigned char *a_ByteRead, int send_ack);



#endif
