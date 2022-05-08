//
// Created by Admin on 2017-05-15.
//

#ifndef QSRC_STM32_CONTROLLER_ENCODER_H
#define QSRC_STM32_CONTROLLER_ENCODER_H

static const int RELOAD_VALUE = 50;
#ifdef __cplusplus
extern "C" {
#endif
#include <tim.h>
#ifdef __cplusplus
}
#endif

class Encoder {
private:
    TIM_HandleTypeDef * htimer;
    GPIO_TypeDef* btn_gpio;
    uint16_t btn_pin;
    bool reverse;
    int32_t position;
    uint16_t last_pulse_count;
    int16_t position_delta;
public:
    void init();
    int32_t getPosition();
    int16_t getDelta();


public:
    Encoder(TIM_HandleTypeDef *htimer, GPIO_TypeDef *btn_gpio, uint16_t btn_pin, bool reverse);

    float getSpeedFactor();

    bool getButton();
};

#endif //QSRC_STM32_CONTROLLER_ENCODER_H
