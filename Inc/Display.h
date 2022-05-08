//
// Created by Admin on 2017-04-30.
//

#ifndef QSRC_STM32_CONTROLLER_DISPLAY_H
#define QSRC_STM32_CONTROLLER_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif
#include "LCD_HD44780_I2C.h"
#ifdef __cplusplus
}
#endif

class Display {
private:
    I2C_HandleTypeDef *i2c;

    float az_current = 0.0f;
public:
    void setAz(float az_current);
    void setAzTarget(float az_desired);
    void setEl(float el_current);
    void setElTarget(float el_desired);

private:
    uint8_t az_current_refresh = 1;
    float az_desired = 0.0f;
    uint8_t az_desired_refresh = 1;
    float el_current = 0.0f;
    uint8_t el_current_refresh = 1;
    float el_desired = 0.0f;
    uint8_t el_desired_refresh = 1;
    uint16_t comm_tx_err = 0;
    uint8_t mode_setting_name_refresh = 1;
    uint8_t mode_setting_value_refresh = 1;
    char mode_setting_name[11];
    bool mode_setting_active = false;
public:
    void setMode_setting_value(float mode_setting_value);
    void setMode_setting_active(bool active);
    void setMode_setting_name(char *mode_setting_name);

private:
    float mode_setting_value = 0.0f;
public:
    void setComm_tx_err(uint16_t comm_tx_err);

    void setComm_rx_err(uint16_t comm_rx_err);

private:
    uint16_t comm_rx_err = 0;
    uint8_t comm_tx_err_refresh = 1;
    uint8_t comm_rx_err_refresh = 1;
public:
    Display(I2C_HandleTypeDef *i2c);

    void init();
    void refresh();
    void print(uint8_t row, uint8_t col, char *data);
    void print(uint8_t row, uint8_t col, int32_t value);
    void print(uint8_t row, uint8_t col, float value, uint8_t digits);
    void setRC(uint8_t row, uint8_t col) const;

    void clear();
};


#endif //QSRC_STM32_CONTROLLER_DISPLAY_H
