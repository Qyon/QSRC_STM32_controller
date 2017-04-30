//
// Created by Admin on 2017-04-30.
//

#ifndef QSRC_STM32_CONTROLLER_STARTUP_H
#define QSRC_STM32_CONTROLLER_STARTUP_H

#ifdef __cplusplus
extern "C" {
#endif

void startup();
void USB_CDC_ReceiveHandler(uint8_t *buffer, uint32_t len);
#ifdef __cplusplus
}
#endif


#endif //QSRC_STM32_CONTROLLER_STARTUP_H
