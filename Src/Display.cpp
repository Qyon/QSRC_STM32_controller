//
// Created by Admin on 2017-04-30.
//


#include "Display.h"
void lcd_error(LCD_RESULT x){

}
void Display::init() {
    lcd->errorCallback = lcd_error;
    lcd->pcf8574.PCF_I2C_ADDRESS = 7;
    lcd->pcf8574.PCF_I2C_TIMEOUT = 1000;
    lcd->pcf8574.i2c.Instance = I2C1;
    lcd->pcf8574.i2c.Init.ClockSpeed = 400000;
    lcd->NUMBER_OF_LINES = NUMBER_OF_LINES_2;
    lcd->type = TYPE0;

    LCD_Init(lcd);

    LCD_ClearDisplay(lcd);


    print(0, 0, (char *)  "QSRC ");
    print(1, 0, (char *) __DATE__);
    print(2, 0, (char *)  __TIME__);
    print(3, 0, (char *) "SQ5RWU");

    HAL_Delay(1000);
    LCD_ClearDisplay(lcd);
}


Display::Display(LCD_PCF8574_HandleTypeDef *lcd) : lcd(lcd) {

}
/**
 * @param row
 * @param data
 * @param len
 */
void Display::print(uint8_t row, uint8_t col, char *data) {
    setRC(row, col);
    LCD_WriteString(lcd, data);
}

void Display::setRC(uint8_t row, uint8_t col) const {
    if (row == 3 || row == 2){
        col += 20;
        row -= 2;
    }
    LCD_SetLocation(lcd, col, row);
}

void Display::print(uint8_t row, uint8_t col, float value, uint8_t digits) {
    setRC(row, col);
    LCD_WriteFloat(lcd, value, digits);
}

void Display::print(uint8_t row, uint8_t col, uint32_t value, uint8_t base) {
    setRC(row, col);
    LCD_WriteNumber(lcd, value, base);
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
        print(2, (uint8_t) (3 + (el_current < 10 ? 2 : (el_current < 100 ? 1 : 0))), el_current, 2);
    }
    if (az_desired_refresh){
        az_desired_refresh = 0;
        print(1, 0, (char *) "          ");
        print(1, (uint8_t) (3 + (az_desired < 10 ? 2 : (az_desired < 100 ? 1 : 0))), az_desired, 2);
    }
    if (el_desired_refresh){
        el_desired_refresh = 0;
        print(3, 0, (char *) "          ");
        print(3, (uint8_t) (3 + (el_desired < 10 ? 2 : (el_desired < 100 ? 1 : 0))), el_desired, 2);
    }
    if (comm_tx_err_refresh){
        comm_tx_err_refresh = 0;
        print(2, 10, (char *) "          ");
        print(2, 10, comm_tx_err);
    }
    if (comm_rx_err_refresh){
        comm_rx_err_refresh = 0;
        print(3, 10, (char *) "          ");
        print(3, 10, comm_rx_err);
    }
}

void Display::setAz(float az_current) {
    Display::az_current = az_current;
    az_current_refresh = 1;
}

void Display::setAzTarget(float az_desired) {
    Display::az_desired = az_desired;
    az_desired_refresh = 1;
}

void Display::setEl(float el_current) {
    Display::el_current = el_current;
    el_current_refresh = 1;
}

void Display::setElTarget(float el_desired) {
    Display::el_desired = el_desired;
    el_desired_refresh = 1;
}

void Display::clear() {
    LCD_ClearDisplay(lcd);
}

void Display::setComm_tx_err(uint16_t comm_tx_err) {
    Display::comm_tx_err = comm_tx_err;
    comm_tx_err_refresh = 1;
}

void Display::setComm_rx_err(uint16_t comm_rx_err) {
    Display::comm_rx_err = comm_rx_err;
    comm_rx_err_refresh = 1;
}

