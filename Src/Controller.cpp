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
    RTC_TimeTypeDef time;
    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    if (time.Seconds != last_time.Seconds){
        last_time = time;
        display->print(1, 10, (uint32_t)time.Seconds, 2);
        CommandPacket commandPacket;
        commandPacket.command = cmdReadAzEl;
        this->display->print(0, 10, (char *) "      ");
        this->display->print(0, 10, commands_queue_counter);
        this->queueCommand(&commandPacket);
    }
    if (commands_queue_counter){
        if (this->sendCommand(&commands_queue[commands_queue_counter-1])){
            commands_queue_counter--;
        }
    }
    display->refresh();
}

bool Controller::sendCommand(CommandPacket *pPacket) {
    HAL_StatusTypeDef s;
    HAL_GPIO_WritePin(green_led_GPIO_Port, green_led_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
    s = HAL_UART_Transmit(this->comm_uart, (uint8_t *)pPacket, sizeof(CommandPacket), 1001);
    HAL_Delay(10);
    HAL_GPIO_WritePin(green_led_GPIO_Port, green_led_Pin, GPIO_PIN_RESET);

    if (s == HAL_OK){
        __HAL_UART_CLEAR_FLAG(this->comm_uart, UART_FLAG_RXNE);
        __HAL_UART_CLEAR_FLAG(this->comm_uart, UART_FLAG_ORE);
        memset((void *)&this->cmd_buffer, 0, sizeof(this->cmd_buffer));
        s = HAL_UART_Receive(this->comm_uart, (uint8_t *) &this->cmd_buffer, sizeof(this->cmd_buffer), 200);
        if (s == HAL_OK){
            this->onUARTData();
            if (!this->validateCommandPacket((CommandPacket *) &this->cmd_to_process)){
                memset((void *)&this->cmd_to_process, 0, sizeof(this->cmd_to_process));
            } else {
                this->handleCommand((CommandPacket *) &this->cmd_to_process, nullptr);
                this->cmd_to_process.header = 0;
            }
            return true;
        } else {
            this->onRxError();
        }

    } else {
        this->onTxError();
    }
    return false;
}

bool Controller::validateCommandPacket(CommandPacket *pPacket) {
    if (pPacket->header != packetHeader){
        return false;
    }

    return getPacketCRC(pPacket) == pPacket->crc;
}

uint16_t Controller::getPacketCRC(const CommandPacket *pPacket) const { return crc16((uint8_t *) pPacket, sizeof(CommandPacket) - sizeof(uint16_t)); }


void Controller::process_set_command(Rot2ProgCmd *pCmd) {
    CommandPacket commandPacket;
    commandPacket.command = cmdGoToAzEl;
    commandPacket.payload.goToAzEl.az = readRot2ProgAngle(pCmd->azimuth, pCmd->azimuth_resolution);
    commandPacket.payload.goToAzEl.el = readRot2ProgAngle(pCmd->elevation, pCmd->elevation_resolution);
    setAz_desired(commandPacket.payload.goToAzEl.az);
    setEl_desired(commandPacket.payload.goToAzEl.el);
    queueCommand(&commandPacket);
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
}

void Controller::onUARTData() {
    memcpy((void *)&(cmd_to_process), (const void *) &cmd_buffer[1], sizeof(cmd_to_process));
}

void Controller::handleCommand(CommandPacket *pPacket, CommandPacket *pResponse) {
    if (nullptr != pResponse){
        memset(pResponse, 0, sizeof(CommandPacket));
        pResponse->header = packetHeader;
        pResponse->command = cmdOkResponse;
    }

    switch (pPacket->command){
        case cmdReadAzElResponse:
            setAz_current(pPacket->payload.readAzElResponse.az);
            setEl_current(pPacket->payload.readAzElResponse.el);
            break;
        default:
            break;
    }
    if (nullptr != pResponse){
        pResponse->crc = getPacketCRC(pResponse);
    }

}

void Controller::onTxError() {
    comm_tx_err++;
    display->setComm_tx_err(comm_tx_err);
}

void Controller::onRxError() {
    comm_rx_err++;
    display->setComm_rx_err(comm_rx_err);
}

void Controller::queueCommand(const CommandPacket *const pPacket) {
    if (commands_queue_counter >= MAX_COMMANDS_IN_QUEUE){
        // TODO: error handling
    } else {
        memcpy(&commands_queue[commands_queue_counter], pPacket, sizeof(CommandPacket));
        commands_queue[commands_queue_counter].header = packetHeader;
        commands_queue[commands_queue_counter].crc = getPacketCRC(&commands_queue[commands_queue_counter]);

        commands_queue_counter++;
    }
}

