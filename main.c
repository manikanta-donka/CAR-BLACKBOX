/*
 * File:   main.c
 * Author: Manikanta Donka
 * Description: CAR BLACK BOX
 * Created on 31 December, 2024, 2:06 PM
 */


#include <xc.h>
#include "main.h"
#include "clcd.h"
#include "adc.h"
#include "external_eeprom.h"
#include "i2c.h"
#include "ds1307.h"
#include "matrix_keypad.h"
#include "uart.h"

State_t state;


// Initializations
void init_config()
{
    state = e_dashboard;
    init_adc_config();
    init_clcd();
    init_matrix_keypad();
    init_i2c();
    init_ds1307();
    init_uart();
}

void main(void) {
    
    init_config();
   
    while(1)
    {
        key = read_switches(STATE_CHANGE);
        get_time();
            
        
        switch (state)
        {
            case e_dashboard:
                view_dashboard();
                if (key == MK_SW11) // Entering to main menu
                {
                    state = e_main_menu;
                    CLEAR_DISP_SCREEN;
                }
                break;
                
            case e_main_menu:
                // Display dashboard
                display_main_menu();
                
                if(key == MK_SW11)
                {
                    if (current_menu == 0 && star == 0)     // Entering to view log
                    {
                        state = e_view_log;
                        CLEAR_DISP_SCREEN;
                    } 
                    else if ((current_menu == 0 && star == 1) || (current_menu == 1 && star == 0))  // Entering to clear log
                    {
                        state = e_clear_log;
                        CLEAR_DISP_SCREEN;
                    }
                    else if ((current_menu == 1 && star == 1) || (current_menu == 2 && star == 0))  // Entering to download log
                    {
                        state = e_download_log;
                        CLEAR_DISP_SCREEN;
                    }
                    else if((current_menu == 2 && star == 1))   // Entering to set time
                    {
                        state = e_set_time;
                        CLEAR_DISP_SCREEN;
                    }
                }
                break;
            
            case e_view_log:
                view_log();
                break;
                 
            case e_download_log:
                download_log();
                break;
                
            case e_clear_log:
                clear_log();
                break;
                
                      
            case e_set_time:
                set_time();
                break;
        }
        
        
    }
}
