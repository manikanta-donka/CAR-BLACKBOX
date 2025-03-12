/* 
 * File:   i2c.h
 * Author: sekha
 *
 * Created on 8 January, 2025, 8:13 PM
 */

void init_i2c(void);
void i2c_start(void);
void i2c_rep_start(void);
void i2c_stop(void);
void i2c_write(unsigned char data);
unsigned char i2c_read(void);
void get_time(void);
void display_time(void);

