//
// Created by Admin on 2017-04-30.
//
#include <Controller.h>
#include <usart.h>
#include "Display.h"
#include "startup.h"
#include "cstring"

LCD_PCF8574_HandleTypeDef lcd;
Display display(&lcd);
Controller controller(&display, &huart1);

void startup() {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-noreturn"

    controller.init();
    while (1){
        controller.loop();
    }
#pragma GCC diagnostic pop
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    controller.onUSARTRxComplete(huart);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    controller.onUSARTTxComplete(huart);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    controller.onUSARTRxComplete(huart);
}

void USB_CDC_ReceiveHandler(uint8_t *buffer, uint32_t len){
//  CDC_Transmit_FS(Buf, *Len);
    controller.handleUartRx(buffer, len);
    memset(buffer, 0, len);
}