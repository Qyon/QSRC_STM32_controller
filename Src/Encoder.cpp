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
    volatile auto pulse_count = (uint16_t) htimer->Instance->CNT;
    auto diff = (int16_t)(uint16_t)(last_pulse_count - pulse_count);
    last_pulse_count = pulse_count;
    if (reverse) {
        position_delta = (int16_t)-diff;
    }
    else {
        position_delta = diff;
    }
    return 0;
}

void Encoder::init() {
    HAL_TIM_Encoder_Start(htimer, TIM_CHANNEL_ALL);
    last_pulse_count =  htimer->Instance->CNT;
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
