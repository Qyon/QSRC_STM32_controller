//
// Created by Admin on 2017-05-15.
//

#include <cstdlib>
#include "Encoder.h"

Encoder::Encoder(TIM_HandleTypeDef *htimer, GPIO_TypeDef *btn_gpio, uint16_t btn_pin, bool reverse) : htimer(htimer),
                                                                                                      btn_gpio(btn_gpio),
                                                                                                      btn_pin(btn_pin),
                                                                                                      reverse(reverse) {

}

int32_t Encoder::getPosition() {
    volatile uint16_t pulse_count = (uint16_t) htimer->Instance->CNT;
//    if (abs(last_pulse_count - pulse_count) < 4){
//        return position;
//    }
    htimer->Instance->CNT = RELOAD_VALUE << 2;
    if (reverse){
        position_delta = (int8_t) (RELOAD_VALUE -  (pulse_count >> 2));
    } else {
        position_delta = (int8_t) ((pulse_count >> 2) - RELOAD_VALUE);
    }
    position += position_delta;
    return position;
}

void Encoder::init() {
    HAL_TIM_Encoder_Start(htimer, TIM_CHANNEL_ALL);
    htimer->Instance->CNT = RELOAD_VALUE << 2;
}

int16_t Encoder::getDelta() {
    return position_delta;
}

float Encoder::getSpeedFactor() {
    if (abs(position_delta) > 6){
        return 1.0f;
    } else if (abs(position_delta) > 2){
        return 0.1f;
    }
    return 0.1f;
}

bool Encoder::getButton() {
    return HAL_GPIO_ReadPin(btn_gpio, btn_pin) == GPIO_PIN_RESET;
}
