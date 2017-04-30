//
// Created by Admin on 2017-04-30.
//
#include <Controller.h>
#include "Display.h"
#include "startup.h"
#include "cstring"

LCD_PCF8574_HandleTypeDef lcd;
Display display(&lcd);
Controller controller(&display);

void startup() {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
    controller.init();
    float x = 0.0f;
    while (1){
        display.setAz(x);
        x += 1.123456f;
        controller.loop();
        HAL_Delay(500);
    }
#pragma GCC diagnostic pop
}


void USB_CDC_ReceiveHandler(uint8_t *buffer, uint32_t len){
//  CDC_Transmit_FS(Buf, *Len);
    controller.handleUartRx(buffer, len);
    memset(buffer, 0, len);
}