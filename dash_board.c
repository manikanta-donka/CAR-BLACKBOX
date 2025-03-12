/*
 * File:   dash_board.c
 * Author: Manikanta Donka
 * Description: CAR BLACK BOX
 * Created on 31 December, 2024, 2:06 PM
 */


#include <xc.h>
#include "clcd.h"
#include "matrix_keypad.h"
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "ds1307.h"
#include "external_eeprom.h"

unsigned int count = 0, current_gear = 0, speed, crash = 0;


void view_dashboard() {
    current_menu = 0; // Reset menu state
    star = 0;         // Reset star indicator

    // Display the current time on the CLCD
    display_time();

    // Display static labels for the dashboard
    clcd_print("TIME", LINE1(2));
    clcd_print("EV", LINE1(10));
    clcd_print("SP", LINE1(14));

    // Display current gear
    clcd_print(gear[current_gear], LINE2(10));

    // If crash occurred, display crash gear indicator
    if (crash) {
        clcd_print(gear[7], LINE2(10));
    }

    // Read ADC value to determine speed
    unsigned short adc_value = read_adc();
    speed = (int) adc_value / (float) 10.32; // Convert ADC value to speed upto 99

    // Display speed on the CLCD
    clcd_putch('0' + (speed / 10), LINE2(14)); // Tens place
    clcd_putch('0' + (speed % 10), LINE2(15)); // Units place

    // Handle user inputs via matrix keypad
    if (key == MK_SW1) {
        crash = 1;         // Set crash flag
        current_gear = 1;  // Reset to first gear
        event_store();     // Log the event
    }
    if (key == MK_SW2) {
        if (crash) {
            crash = 0;      // Clear crash flag
            current_gear = 1; // Reset to first gear
            event_store();  // Log the event
        } else if (current_gear < 6) {
            current_gear++; // Increment gear
            event_store();  // Log the event
        }
    }
    if (key == MK_SW3) {
        if (crash) {
            crash = 0;      // Clear crash flag
            current_gear = 1; // Reset to first gear
            event_store();  // Log the event
        } else if (current_gear > 1) {
            current_gear--; // Decrement gear
            event_store();  // Log the event
        }
    }
}


void event_store(void) {
    // Wrap address around if it exceeds the EEPROM memory space
    if (address > 99) {
        address = 0;
    }

    // Write time (HH:MM:SS) to EEPROM, skipping ':' characters
    for (int i = 0; i < 8; i++) {
        if (i == 2 || i == 5) // Skip ':' separators
            continue;
        write_external_eeprom(address++, time[i]);
    }

    // Write gear and speed information to EEPROM
    write_external_eeprom(address++, gear[current_gear][0]); // Gear tens
    write_external_eeprom(address++, gear[current_gear][1]); // Gear units
    write_external_eeprom(address++, (speed / 10) + 48); // Speed tens (ASCII)
    write_external_eeprom(address++, (speed % 10) + 48); // Speed units (ASCII)
    
    // Update event counters
    if (total_events < 10) {
        total_events++; // Increment total event count
    } else {
        extra_count++;  // Increment overflow count
    }
}


void display_time(void) {
    clcd_print(time, LINE2(0)); // Print time string at LINE2
}


void get_time(void) {
    // Read hour, minute, and second from the DS1307
    clock_reg[0] = read_ds1307(HOUR_ADDR);
    clock_reg[1] = read_ds1307(MIN_ADDR);
    clock_reg[2] = read_ds1307(SEC_ADDR);

    // Format the hour (12-hour or 24-hour format)
    if (clock_reg[0] & 0x40) { // 12-hour format
        time[0] = '0' + ((clock_reg[0] >> 4) & 0x01);
        time[1] = '0' + (clock_reg[0] & 0x0F);
    } else { // 24-hour format
        time[0] = '0' + ((clock_reg[0] >> 4) & 0x03);
        time[1] = '0' + (clock_reg[0] & 0x0F);
    }
    time[2] = ':'; // Hour-minute separator
    time[3] = '0' + ((clock_reg[1] >> 4) & 0x0F); // Minute tens
    time[4] = '0' + (clock_reg[1] & 0x0F);       // Minute units
    time[5] = ':'; // Minute-second separator
    time[6] = '0' + ((clock_reg[2] >> 4) & 0x0F); // Second tens
    time[7] = '0' + (clock_reg[2] & 0x0F);       // Second units
    time[8] = '\0'; // Null-terminate the string
}
