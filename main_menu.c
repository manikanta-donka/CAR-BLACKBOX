/*
 * File:   main_menu.c
 * Author: Manikanta Donka
 * Description: CAR BLACK BOX
 * Created on 7 January, 2025, 2:14 PM
 */


#include <xc.h>
#include "main.h"
#include "matrix_keypad.h"
#include "clcd.h"
#include "external_eeprom.h"
#include "ds1307.h"
#include "uart.h"

//Function to display main menu
void display_main_menu(void) {
    
    // Display star indication based on menu selection
    if (star == 0)
    {
        clcd_putch('*', LINE1(0));
        clcd_putch(' ', LINE2(0));
    }
    else if (star == 1)
    {
        clcd_putch(' ', LINE1(0));
        clcd_putch('*', LINE2(0));
    }
    
    // To display menu options
    clcd_print(menu[current_menu], LINE1(2));
    clcd_print(menu[current_menu + 1], LINE2(2));


    if (key == MK_SW1) {    // Switch - 1 for scrolling up
        if(star == 1)
        {
            star = 0;
        }
        else if (current_menu > 0) {
            current_menu--;
        }
    } else if (key == MK_SW2) { // switch - 2 for scrolling down 
        if(star == 0)
        {
            star = 1;
        }
        else if (current_menu < 2) {
            current_menu++;
        }
    } else if (key == MK_SW12) {    // switch - 12 for return back to dashboard
        
        state = e_dashboard;
        CLEAR_DISP_SCREEN;
    }
    
    

}

// Function for to display the storing events in EEPROM
void view_log(void)
{
    static int flag = 0;
    if (flag == 0)
    {
        flag =1;
        temp_address = extra_count * 10;    // condition for displaying old events first 
    }
    // Read the log data from EEPROM
    for(int i = 0; i < 10; i++)
    {
        data[i] = read_external_eeprom(temp_address+i);
    } 
    
    if (total_events == 0)  // Condition for no logs
    {
        for (unsigned int wait = 0; wait < 2000; wait++)
        {
            clcd_print("NO LOGS TO", LINE1(0));
            clcd_print("DISPLAY :)", LINE2(3));
        }
        CLEAR_DISP_SCREEN;
        state = e_main_menu;
        return;
    }
    else
    {
        // To display log entires
        clcd_putch('#', LINE1(0));
        clcd_print("VIEW LOG", LINE1(2));
        clcd_putch('0' + current_event, LINE2(0));  // To display log count
        
        // To display logs
        clcd_putch(data[0], LINE2(2));
        clcd_putch(data[1], LINE2(3));
        clcd_putch(':', LINE2(4));
        clcd_putch(data[2], LINE2(5));
        clcd_putch(data[3], LINE2(6));
        clcd_putch(':', LINE2(7));
        clcd_putch(data[4], LINE2(8));
        clcd_putch(data[5], LINE2(9));
        clcd_putch(data[6], LINE2(11));
        clcd_putch(data[7], LINE2(12));
        clcd_putch(data[8], LINE2(14));
        clcd_putch(data[9], LINE2(15));
    }
    
    if (key == MK_SW1 && current_event > 0)     // Scrolling up
    {
        current_event--;
        temp_address = temp_address - 10;
    } 
    else if (key == MK_SW2 && current_event < (total_events - 1))   // Scrolling down
    {
        current_event++;
        temp_address = temp_address + 10;
        if (temp_address > 99)  
            temp_address = 0x00;    // Wrap around if address exceeds 10 events
    } 
    else if (key == MK_SW12) 
    {
        state = e_main_menu; // Return to the main menu
        CLEAR_DISP_SCREEN;
        current_event = 0;
        temp_address = 0x00;
        flag = 0;
    }
}

// Clearing all Log data 
void clear_log(void)
{
    total_events = 0;   // Resetting event counter
    address = 0;    // Resetting EEPROM data
    
    for(unsigned int wait = 0; wait < 2000; wait++)
    {
        clcd_print("Clearing Logs...", LINE1(0));
        clcd_print("Just a minute", LINE2(2));
    }
    
    CLEAR_DISP_SCREEN;
    state = e_main_menu;    // Return to main menu
    return;
}

// Function for Download all logs through UART
void download_log(void)
{
    if (total_events == 0)  // Condition for if no logs are present
    {
        for (unsigned int wait = 0; wait < 2000; wait++)
        {
            clcd_print("NO LOGS TO", LINE1(0));
            clcd_print("DOWNLOAD :)", LINE2(3));
        }
        CLEAR_DISP_SCREEN;
        puts("NO LOGS TO DOWNLOAD");    // display in UART
        puts("\n\r");
        state = e_main_menu;    // return to main menu
        return;
    }
    
    else
    {
       for (unsigned int wait = 0; wait < 2000; wait++) {
            clcd_print("Downloading Logs", LINE1(0));
            clcd_print("Through UART", LINE2(0));
        }
        CLEAR_DISP_SCREEN;
        state = e_main_menu;    // Return to main menu

        puts("NO. TIME   EV SP");
        puts("\n\r");


        event_address = extra_count * 10;
        for (unsigned int event_index = 0; event_index < total_events ; event_index++) {
    

            for (int i = 0; i < 10; i++) {
                data[i] = read_external_eeprom(event_address++);
            }

            if (event_address > 99)
                event_address=0x00; // wrap around if address exceeds
            
            putch(event_index + '0');   // To print event count
            putch(' ');

            // Send log details through UART
            putch(data[0]);
            putch(data[1]);
            puts(":");
            putch(data[2]);
            putch(data[3]);
            puts(":");
            putch(data[4]);
            putch(data[5]);
            puts(" ");
            putch(data[6]);
            putch(data[7]);
            puts(" ");
            putch(data[8]);
            putch(data[9]);
            puts("\n\r");

        } 
    }
    
    
}

// Function for saving the time changes
void set_time(void)
{
    if(set_flag == 0)
    {
        set_flag = 1;
        
        // Extract current time values
        hr = ((time[0] - '0') * 10) + (time[1] - '0');
        min = ((time[3] - '0') * 10) + (time[4] - '0');
        sec = ((time[6] - '0') * 10) + (time[7] - '0');
    }
    
    clcd_print("HH:MM:SS", LINE1(0));
    
    // Handle Blinking for the selected field
    if(field == 0)  // Hour Field
    {
        if((delay++) < 500)
        {
            clcd_putch(' ', LINE2(0));
            clcd_putch(' ', LINE2(1));
        }
        else if((delay++) < 1000)
        {
            clcd_putch((hr / 10) + '0', LINE2(0));
            clcd_putch((hr % 10) + '0', LINE2(1));
            clcd_putch(':', LINE2(2));
            clcd_putch((min / 10) + '0', LINE2(3));
            clcd_putch((min % 10) + '0', LINE2(4));
            clcd_putch(':', LINE2(5));
            clcd_putch((sec / 10) + '0', LINE2(6));
            clcd_putch((sec % 10) + '0', LINE2(7));
        }
        else
        {
            delay = 0;
        }
    }
    
    else if (field == 1)    // Minute Field
    {
        if((delay++) < 500)
        {
            clcd_putch(' ', LINE2(3));
            clcd_putch(' ', LINE2(4));
        }
        else if((delay++) < 1000)
        {
            clcd_putch((hr / 10) + '0', LINE2(0));
            clcd_putch((hr % 10) + '0', LINE2(1));
            clcd_putch(':', LINE2(2));
            clcd_putch((min / 10) + '0', LINE2(3));
            clcd_putch((min % 10) + '0', LINE2(4));
            clcd_putch(':', LINE2(5));
            clcd_putch((sec / 10) + '0', LINE2(6));
            clcd_putch((sec % 10) + '0', LINE2(7));
        }
        else
        {
            delay = 0;
        }
    }
    
    else if (field == 2)    // Seconds Field
    {
        if((delay++) < 500) // Delay for Blinking number
        {
            clcd_putch(' ', LINE2(6));
            clcd_putch(' ', LINE2(7));
        }
        else if((delay++) < 1000)
        {
            clcd_putch((hr / 10) + '0', LINE2(0));
            clcd_putch((hr % 10) + '0', LINE2(1));
            clcd_putch(':', LINE2(2));
            clcd_putch((min / 10) + '0', LINE2(3));
            clcd_putch((min % 10) + '0', LINE2(4));
            clcd_putch(':', LINE2(5));
            clcd_putch((sec / 10) + '0', LINE2(6));
            clcd_putch((sec % 10) + '0', LINE2(7));
        }
        else
        {
            delay = 0;
        }
    }
    
    if (key == MK_SW1)  // Increment 
    {
        if(field == 0)  // Hours Increment
        {
            hr++;
            if(hr > 23)
            {
                hr = 0;
            }
        }
        
        else if(field == 1) // Minutes Increment
        {
            min++;
            if(min > 59)
            {
                min = 0;
            }
        }
        
        else if(field == 2) // Seconds Increment
        {
            sec++;
            if(sec > 59)
            {
                sec = 0;
            }
        }
    }
    
    else if (key == MK_SW2) // Change Field
    {
        field++;
        if(field > 2)
        {
            field = 0;
        }
    }
    
    else if(key == MK_SW11) // Save and Exit
    {
        
        clock_reg[0] = ((hr / 10) << 4) | (hr % 10);
        clock_reg[1] = ((min / 10) << 4) | (min % 10);
        clock_reg[2] = ((sec / 10) << 4) | (sec % 10);
        //used for write time into dashboard
        write_ds1307(HOUR_ADDR, clock_reg[0]);
        write_ds1307(MIN_ADDR, clock_reg[1]);
        write_ds1307(SEC_ADDR, clock_reg[2]);
        
        set_flag = 0;
        field = 0;
        state = e_dashboard;
        CLEAR_DISP_SCREEN;
    }
    
    else if(key == MK_SW12) // exit & without saving
    {
        set_flag = 0;
        field = 0;
        state = e_main_menu; // Return to the main menu
        CLEAR_DISP_SCREEN;
    }
}