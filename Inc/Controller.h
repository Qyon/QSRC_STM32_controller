//
// Created by Admin on 2017-04-30.
//

#ifndef QSRC_STM32_CONTROLLER_CONTROLLER_H
#define QSRC_STM32_CONTROLLER_CONTROLLER_H

static const int MAX_COMMANDS_IN_QUEUE = 14;

#include "Display.h"
#include "Rot2Prog.h"
#include "protocol.h"
#include "Encoder.h"

extern "C" {
#include "crc.h"
};

enum ModeSetting{
    msNone = 0,
    msSetAz = 1,
    msSetEl = 2,
    msLast = 3,
};

class Controller {
private:
    Display * display;
    UART_HandleTypeDef *comm_uart;
    Encoder *encoder_s;
    Encoder *encoder_az;
    Encoder *encoder_el;
    float az_current = 0.0f;
    float az_desired = 0.0f;
    float el_current = 0.0f;
    float el_desired = 0.0f;
    uint16_t comm_tx_err = 0;
    uint16_t comm_rx_err = 0;
    RTC_TimeTypeDef last_time;
    uint16_t raw_enc_az;
    uint16_t raw_enc_el;
    bool first_read_done_az = false;
    bool first_read_done_el = false;

    ModeSetting current_mode_setting;
    bool mode_setting_active;
    char current_mode_setting_name[11];
    float set_az_el_value = 0.0f;

    volatile uint8_t cmd_in;
    volatile uint8_t cmd_buffer[sizeof(CommandPacket) + 2];
    volatile uint8_t cmd_buffer_index = 0;
    volatile CommandPacket cmd_to_process;

    CommandPacket commands_queue[MAX_COMMANDS_IN_QUEUE];
    uint8_t commands_queue_counter = 0;

    void setAz_current(float az_current);
    void setAz_desired(float az_desired);
    void setEl_current(float el_current);
    void setEl_desired(float el_desired);
    uint32_t last_command_send_tick;
    uint32_t last_response_received_tick;
public:
    Controller(Display *display, UART_HandleTypeDef *comm_uart, Encoder *encoder_s, Encoder *encoder_az,
                   Encoder *encoder_el);

    void handleUartRx(uint8_t *data, uint32_t len);

    void init();

    void loop();

    void process_set_command(Rot2ProgCmd *pCmd);

    float readRot2ProgAngle(uint8_t *angle_data, uint8_t resolution);

    void send_respose_status();

    void getRot2ProgAngle(float angle, uint8_t *angle_response);

    void onUSARTTxComplete(UART_HandleTypeDef *huart);
    void onUSARTError(UART_HandleTypeDef *huart);

    void onUSARTRxComplete(UART_HandleTypeDef *huart);

    bool validateCommandPacket(CommandPacket *pPacket);

    uint16_t getPacketCRC(const CommandPacket *pPacket) const;

    bool handleCommand(CommandPacket *pPacket, CommandPacket *pResponse);

    void onUARTData();

    bool sendCommand(CommandPacket *pPacket);

    void onTxError(uint16_t err_code);

    void onRxError(uint16_t err_code);

    bool queueCommand(const CommandPacket *const pPacket);

    void sendAzEl(float az, float el);

    bool checkCommandsQueue();

    char *getModeSettingName(ModeSetting setting);


};


#endif //QSRC_STM32_CONTROLLER_CONTROLLER_H
