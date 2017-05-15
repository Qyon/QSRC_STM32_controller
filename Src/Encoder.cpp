//
// Created by Admin on 2017-05-15.
//

#include <cstdlib>
#include "Encoder.h"

Encoder::Encoder(TIM_HandleTypeDef *htimer, GPIO_TypeDef *btn_gpio, uint16_t btn_pin) : htimer(htimer),
                                                                                        btn_gpio(btn_gpio),
                                                                                        btn_pin(btn_pin) {

}

int32_t Encoder::getPosition() {
    volatile uint16_t pulse_count = (uint16_t) htimer->Instance->CNT;
    htimer->Instance->CNT = RELOAD_VALUE << 2;
    position_delta = (int8_t) ((pulse_count >> 2) - RELOAD_VALUE);
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
