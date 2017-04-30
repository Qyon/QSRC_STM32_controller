//
// Created by Admin on 2017-04-30.
//

#ifndef QSRC_STM32_CONTROLLER_DISPLAY_H
#define QSRC_STM32_CONTROLLER_DISPLAY_H
#ifdef __cplusplus
extern "C" {
#endif
#include <hd44780.h>
#ifdef __cplusplus
}
#endif

class Display {
private:
    LCD_PCF8574_HandleTypeDef *lcd;

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
public:
    Display(LCD_PCF8574_HandleTypeDef *lcd);

    void init();
    void refresh();
    void print(uint8_t row, uint8_t col, char *data);
    void print(uint8_t row, uint8_t col, float value, uint8_t digits);
    void setRC(uint8_t row, uint8_t col) const;
};


#endif //QSRC_STM32_CONTROLLER_DISPLAY_H
