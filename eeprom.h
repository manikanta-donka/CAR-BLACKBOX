/* 
 * File:   eeprom.h
 * Author: sekha
 *
 * Created on 3 January, 2025, 2:49 PM
 */

#ifndef EEPROM_H
#define	EEPROM_H

void init_eeprom_config();

unsigned char read_internal_eeprom(unsigned char);

void write_internal_eeprom(unsigned char, unsigned char);


#endif	/* EEPROM_H */

