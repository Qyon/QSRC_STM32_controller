//
// Created by Admin on 2017-04-30.
//

#include <usbd_cdc_if.h>
#include <protocol.h>
#include <rtc.h>
#include <stm32f1xx_hal_rtc.h>
#include "Controller.h"

Controller::Controller(Display *display, UART_HandleTypeDef *comm_uart) : display(display), comm_uart(comm_uart) {

}

void Controller::handleUartRx(uint8_t *data, uint32_t len) {
    Rot2ProgCmd * cmd = (Rot2ProgCmd *) data;
    switch (cmd->command){
        case Rot2Prog_COMMAND_STATUS:
            this->send_respose_status();
            break;
        case Rot2Prog_COMMAND_SET:
            this->process_set_command(cmd);
            break;
        case Rot2Prog_COMMAND_STOP:
            break;
        default:
            break;
    }
}

Rot2ProgResponse response = {.start_byte = 'W', .azimuth={0,0,0,0}, .azimuth_resolution=10, .elevation={0,0,0,0}, .elevation_resolution=10, .end_byte=' '};

void Controller::send_respose_status() {
    getRot2ProgAngle(az_current, response.azimuth);
    getRot2ProgAngle(el_current, response.elevation);

    CDC_Transmit_FS((uint8_t *) &response, sizeof(Rot2ProgResponse));
}

void Controller::getRot2ProgAngle(float angle, uint8_t * angle_response) {
    uint16_t tmp = (uint16_t) ((angle + 360.0f) * 10);
    tmp = (uint16_t) (tmp % 10000);
    angle_response[0] = (uint8_t) (tmp / 1000);
    tmp = (uint16_t) (tmp % 1000);
    angle_response[1] = (uint8_t) (tmp / 100);
    tmp = (uint16_t) (tmp % 100);
    angle_response[2] = (uint8_t) (tmp / 10);
    tmp = (uint16_t) (tmp % 10);
    angle_response[3] = (uint8_t) (tmp % 10);
}

void Controller::init() {
    HAL_GPIO_WritePin(green_led_GPIO_Port, green_led_Pin, GPIO_PIN_RESET);
    display->init();
}

void Controller::loop() {
    static CommandPacket response;
    RTC_TimeTypeDef time;
    if (uart_has_data){

    } else {
        HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
        if (time.Seconds != last_time.Seconds){
            //display->print(0, 10, (char *) "NO DATA");
            last_time = time;
            display->print(1, 10, (uint32_t)time.Seconds, 2);
            response.command = cmdPing;
            response.header = packetHeader;
            response.crc = getPacketCRC(&response);

            HAL_GPIO_WritePin(green_led_GPIO_Port, green_led_Pin, GPIO_PIN_SET);
            //HAL_UART_Transmit_IT(this->comm_uart, (uint8_t *) &response, sizeof(response));
            HAL_UART_Transmit(this->comm_uart, (uint8_t *) &response, sizeof(response), 1001);
            HAL_GPIO_WritePin(green_led_GPIO_Port, green_led_Pin, GPIO_PIN_RESET);
            memset((void *)&this->cmd_buffer, 0, sizeof(this->cmd_buffer));
            HAL_StatusTypeDef s = (HAL_UART_Receive(comm_uart, (uint8_t *) (&(cmd_buffer)), sizeof(cmd_buffer)+1, 1000));
            onUARTData();
            if (!validateCommandPacket((CommandPacket *) &this->cmd_to_process)){
                display->print(0, 10, (uint32_t)cmd_to_process.crc, 16);
                memset((void *)&this->cmd_to_process, 0, sizeof(this->cmd_to_process));
            } else {
                display->print(0, 10, (char *) "OK  CMD!");
                handleCommand((CommandPacket *) &this->cmd_to_process, nullptr);
                this->cmd_to_process.header = 0;
            }
        }

    }
    display->refresh();
}

bool Controller::validateCommandPacket(CommandPacket *pPacket) {
    if (pPacket->header != packetHeader){
        return false;
    }

    return getPacketCRC(pPacket) == pPacket->crc;
}

uint16_t Controller::getPacketCRC(const CommandPacket *pPacket) const { return crc16((uint8_t *) pPacket, sizeof(CommandPacket) - sizeof(uint16_t)); }


void Controller::process_set_command(Rot2ProgCmd *pCmd) {
    setAz_desired(readRot2ProgAngle(pCmd->azimuth, pCmd->azimuth_resolution));
    setEl_desired(readRot2ProgAngle(pCmd->elevation, pCmd->elevation_resolution));
}

void Controller::setAz_current(float az_current) {
    Controller::az_current = az_current;
    display->setAz(az_current);
}

void Controller::setAz_desired(float az_desired) {
    Controller::az_desired = az_desired;
    display->setAzTarget(az_desired);
}

void Controller::setEl_current(float el_current) {
    Controller::el_current = el_current;
    display->setEl(el_current);
}

void Controller::setEl_desired(float el_desired) {
    Controller::el_desired = el_desired;
    display->setElTarget(el_desired);
}

uint8_t ascii_to_val(uint8_t ascii){
    return (uint8_t) (ascii >= '0' && ascii <= '9' ? ascii - '0' : 0);
}

float Controller::readRot2ProgAngle(uint8_t *angle_data, uint8_t resolution) {
    uint16_t tmp = 0;
    tmp += ascii_to_val(angle_data[0]) * 1000;
    tmp += ascii_to_val(angle_data[1]) * 100;
    tmp += ascii_to_val(angle_data[2]) * 10;
    tmp += ascii_to_val(angle_data[3]);
    return (((float)tmp) / resolution) - 360.0f;
}

void Controller::onUSARTTxComplete(UART_HandleTypeDef *huart) {
    if (huart->Instance != this->comm_uart->Instance){
        return;
    }
    //HAL_GPIO_WritePin(green_led_GPIO_Port, green_led_Pin, GPIO_PIN_RESET);
}


void Controller::onUSARTRxComplete(UART_HandleTypeDef *huart) {
    if (huart->Instance != this->comm_uart->Instance){
        return;
    }
    uart_has_data = 1;
}

void Controller::onUARTData() {
    memcpy((void *)&(cmd_to_process), (void *)&(cmd_buffer), sizeof(cmd_buffer));
    cmd_ready = true;
}

void Controller::handleCommand(CommandPacket *pPacket, CommandPacket *pResponse) {
    if (nullptr != pResponse){
        memset(pResponse, 0, sizeof(CommandPacket));
        pResponse->header = packetHeader;
        pResponse->command = cmdOkResponse;
    }


    switch (pPacket->command){
        default:break;
    }
    if (nullptr != pResponse){
        pResponse->crc = getPacketCRC(pResponse);
    }

}

