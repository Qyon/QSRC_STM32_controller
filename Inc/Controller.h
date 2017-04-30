//
// Created by Admin on 2017-04-30.
//

#ifndef QSRC_STM32_CONTROLLER_CONTROLLER_H
#define QSRC_STM32_CONTROLLER_CONTROLLER_H

#include "Display.h"
#include "Rot2Prog.h"
#include "protocol.h"
extern "C" {
#include "crc.h"
};

class Controller {
private:
    Display * display;
    UART_HandleTypeDef *comm_uart;
    float az_current = 0.0f;
    float az_desired = 0.0f;
    float el_current = 0.0f;
    float el_desired = 0.0f;
    RTC_TimeTypeDef last_time;

    CommandPacket cmd_buffer;
    CommandPacket cmd_to_process;

    uint32_t serial_sync;
    uint8_t serial_sync_tmp;



    void setAz_current(float az_current);

    void setAz_desired(float az_desired);

    void setEl_current(float el_current);

    void setEl_desired(float el_desired);
public:
    Controller(Display *display, UART_HandleTypeDef *comm_uart);

    void handleUartRx(uint8_t *data, uint32_t len);

    void init();

    void loop();

    void process_set_command(Rot2ProgCmd *pCmd);

    float readRot2ProgAngle(uint8_t *angle_data, uint8_t resolution);

    void send_respose_status();

    void getRot2ProgAngle(float angle, uint8_t *angle_response);

    void onUSARTTxComplete(UART_HandleTypeDef *huart);

    void onUSARTRxComplete(UART_HandleTypeDef *huart);

    bool validateCommandPacket(CommandPacket *pPacket);

    uint16_t getPacketCRC(const CommandPacket *pPacket) const;

    void handleCommand(CommandPacket *pPacket, CommandPacket *pResponse);
};


#endif //QSRC_STM32_CONTROLLER_CONTROLLER_H
