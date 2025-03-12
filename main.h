/* 
 * File:   main.h
 * Author: sekha
 *
 * Created on 31 December, 2024, 2:35 PM
 */

typedef enum
{
    e_dashboard, e_main_menu, e_view_log, e_set_time, e_download_log, e_clear_log
}State_t;

State_t state;
unsigned char key;
unsigned int address = 0, total_events = 0,extra_count = 0, current_event = 0,current_menu = 0,star = 0, temp_address = 0,event_address;
unsigned int field = 0, hr = 0, min = 0, sec = 0, set_flag = 0, delay = 0;
unsigned char gear[10][3] = {"ON", "GN", "G1", "G2", "G3", "G4", "GR", "C  "};
unsigned char menu[4][17] = {"View log         ", "Clear log      ", "Download log   ", "Set log        "};
unsigned char clock_reg[3];
unsigned char time[9],time1[9], data[10];

//Dashboard function declaration
void view_dashboard(void);

//Storing events function declaration
void event_store(void);

//Password function declaration
void password(void);

//main menu function declaration
void display_main_menu(void);

//View log function declaration
void view_log(void);

//Reading events function declaration
void event_reader(void);

//Change password function declaration
void change_password(void);

//Set time function declaration
void set_time(void);

//Download log function decleration
void download_log(void);

//Clear log function declaration
void clear_log(void);


    

