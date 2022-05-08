//
// Created by Admin on 2017-04-30.
//


#include <cstring>
#include "Display.h"

void Display::init() {
    LCD_Init(i2c);

    LCD_Cls();
    LCD_Cls();

    print(0, 0, (char *)  "QSRC ");
    print(1, 0, (char *) __DATE__);
    print(2, 0, (char *)  __TIME__);
    print(3, 0, (char *) "SQ5RWU");

    HAL_Delay(1000);
    LCD_Cls();
}


Display::Display(I2C_HandleTypeDef *i2c) : i2c(i2c) {

}
/**
 * @param row
 * @param data
 * @param len
 */
void Display::print(uint8_t row, uint8_t col, char *data) {
    setRC(row, col);
    LCD_String(data);
}

void Display::setRC(uint8_t row, uint8_t col) const {
    if (row == 3 || row == 2){
        col += 20;
        row -= 2;
    }
    LCD_Locate(col, row);
}

void Display::print(uint8_t row, uint8_t col, float value, uint8_t digits) {
    setRC(row, col);
    char fs[LCD_X+1];
    sprintf(fs, "%%0.%df", digits);
    char buf[LCD_X+1];
    sprintf(buf, fs, value);
    LCD_String(buf);
}

void Display::print(uint8_t row, uint8_t col, int32_t value) {
    setRC(row, col);
    LCD_Int(value);
}

void Display::refresh() {
    if (az_current_refresh) {
        az_current_refresh = 0;
        print(0, 0, (char *) "AZ        ");
        print(0, (uint8_t) (3 + (az_current < 10 ? 2 : (az_current < 100 ? 1 : 0))), az_current, 2);
    }
    if (el_current_refresh) {
        el_current_refresh = 0;
        print(2, 0, (char *) "EL        ");
        print(2, (uint8_t) (3 + (el_current < 10 ? 2 : (el_current < 100 ? 1 : 0)) + (el_current < 0 ? -1 : 0)), el_current, 2);
    }
    if (az_desired_refresh){
        az_desired_refresh = 0;
        print(1, 0, (char *) "          ");
        print(1, (uint8_t) (3 + (az_desired < 10 ? 2 : (az_desired < 100 ? 1 : 0))), az_desired, 2);
    }
    if (el_desired_refresh){
        el_desired_refresh = 0;
        print(3, 0, (char *) "          ");
        print(3, (uint8_t) (3 + (el_desired < 10 ? 2 : (el_desired < 100 ? 1 : 0))  + (el_desired < 0 ? -1 : 0)), el_desired, 2);
    }
    if (comm_tx_err_refresh){
        comm_tx_err_refresh = 0;
        print(3, 10, (char *) "     ");
        print(3, 10, comm_tx_err);
    }
    if (comm_rx_err_refresh){
        comm_rx_err_refresh = 0;
        print(3, 15, (char *) "     ");
        print(3, 15, comm_rx_err);
    }
    if (mode_setting_value_refresh){
        mode_setting_value_refresh = 0;
        print(1, 10, (char *) "          ");
        print(1, 10, mode_setting_value, 2);
    }
    if (mode_setting_name_refresh){
        mode_setting_name_refresh = 0;
        print(0, 10, (char *) "          ");
        print(0, 10, mode_setting_name);
    }
}

void Display::setAz(float az_current) {
    if (Display::az_current != az_current){
        Display::az_current = az_current;
        az_current_refresh = 1;
    }
}

void Display::setAzTarget(float az_desired) {
    if (Display::az_desired != az_desired) {
        Display::az_desired = az_desired;
        az_desired_refresh = 1;
    }
}

void Display::setEl(float el_current) {
    if (Display::el_current != el_current) {
        Display::el_current = el_current;
        el_current_refresh = 1;
    }
}

void Display::setElTarget(float el_desired) {
    if (Display::el_desired != el_desired) {
        Display::el_desired = el_desired;
        el_desired_refresh = 1;
    }
}

void Display::clear() {
    LCD_Cls();
}

void Display::setComm_tx_err(uint16_t comm_tx_err) {
    Display::comm_tx_err = comm_tx_err;
    comm_tx_err_refresh = 1;
}

void Display::setComm_rx_err(uint16_t comm_rx_err) {
    if (Display::comm_rx_err != comm_rx_err) {
        Display::comm_rx_err = comm_rx_err;
        comm_rx_err_refresh = 1;
    }
}

void Display::setMode_setting_value(float mode_setting_value) {
    if (Display::mode_setting_value != mode_setting_value){
        Display::mode_setting_value = mode_setting_value;
        mode_setting_value_refresh = 1;
    }
}

void Display::setMode_setting_name(char * mode_setting_name) {
    if (strcmp(mode_setting_name, Display::mode_setting_name)){
        strcpy(Display::mode_setting_name, mode_setting_name);
        mode_setting_name_refresh = 1;
    }
}

