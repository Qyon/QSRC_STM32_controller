//
// Created by Admin on 2017-04-30.
//

#include <usbd_cdc_if.h>
#include <protocol.h>
#include <rtc.h>
#include <stm32f1xx_hal_rtc.h>
#include <Encoder.h>
#include <math.h>
#include "Controller.h"

Controller::Controller(Display *display, UART_HandleTypeDef *comm_uart, Encoder *encoder_s, Encoder *encoder_az,
                       Encoder *encoder_el) : display(display), comm_uart(comm_uart), encoder_s(encoder_s),
                                              encoder_az(encoder_az), encoder_el(encoder_el) {

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
    encoder_s->init();
    encoder_az->init();
    encoder_el->init();
}

void Controller::loop() {
    static uint32_t tickstart = HAL_GetTick();
    if (HAL_GetTick() - tickstart > 300){
        CommandPacket commandPacket;
        commandPacket.command = cmdReadAzEl;
        this->queueCommand(&commandPacket);
        commandPacket.command = cmdReadEncoders;
        this->queueCommand(&commandPacket);
        tickstart = HAL_GetTick();
    }

    checkCommandsQueue();

    encoder_az->getPosition();
    int8_t delta = (int8_t) encoder_az->getDelta();
    if (delta){
        if (encoder_az->getButton()){
            sendAzEl(roundf(az_desired) + delta * 10, el_desired);
        } else {
            sendAzEl((az_desired + (delta * encoder_az->getSpeedFactor())), el_desired);
        }
    }

    encoder_el->getPosition();
    delta = (int8_t) encoder_el->getDelta();
    if (delta){
        if (encoder_el->getButton()){
            sendAzEl(az_desired, roundf(el_desired) + delta * 10);
        } else {
            sendAzEl(az_desired, (el_desired + (delta * encoder_el->getSpeedFactor())));
        }
    }

    encoder_s->getPosition();
    delta = (int8_t) encoder_s->getDelta();
    static uint32_t mode_active_start = HAL_GetTick();
    if (!mode_setting_active){
        if (delta != 0){
            if (!mode_setting_active && HAL_GetTick() - mode_active_start > 500){
                if (delta > 0){
                    current_mode_setting = (ModeSetting) ((int)current_mode_setting + 1);
                    if (current_mode_setting >= msLast){
                        current_mode_setting = msNone;
                    }
                    mode_active_start = HAL_GetTick();
                }
                if (delta < 0){
                    if (current_mode_setting == msNone){
                        current_mode_setting = msSetEl;
                    } else {
                        current_mode_setting = (ModeSetting) ((int)current_mode_setting - 1);
                    }
                    mode_active_start = HAL_GetTick();
                }
            }
        }
        display->setMode_setting_name(getModeSettingName(current_mode_setting));
        if (encoder_s->getButton()){
            mode_active_start = HAL_GetTick();
            mode_setting_active = 1;
            switch (current_mode_setting){
                case msSetAz:
                    set_az_el_value = az_current;
                    display->setMode_setting_value(set_az_el_value);
                    break;
                case msSetEl:
                    set_az_el_value = el_current;
                    display->setMode_setting_value(set_az_el_value);
                    break;
                case msNone:
                case msLast:
                default:
                    break;
            }
        }
    } else {
        if (delta){
            switch (current_mode_setting){
                case msSetAz:
                    set_az_el_value += delta * encoder_s->getSpeedFactor();
                    display->setMode_setting_value(set_az_el_value);
                    break;
                case msSetEl:
                    set_az_el_value += delta * encoder_s->getSpeedFactor();
                    display->setMode_setting_value(set_az_el_value);
                    break;
                case msNone:
                case msLast:
                default:
                    mode_setting_active = 0;
                    break;
            }
        }
        if (encoder_s->getButton() && HAL_GetTick() - mode_active_start > 500){
            CommandPacket commandPacket;

            switch (current_mode_setting){
                case msSetAz:
                    commandPacket.command = cmdSetAzEl;
                    commandPacket.payload.setAzEl.az = set_az_el_value;
                    commandPacket.payload.setAzEl.el = el_current;

                    if (queueCommand(&commandPacket)){
                        mode_setting_active = 0;
                        current_mode_setting = msNone;
                        setAz_desired(set_az_el_value);
                    }
                    break;
                case msSetEl:
                    commandPacket.command = cmdSetAzEl;
                    commandPacket.payload.setAzEl.el = set_az_el_value;
                    commandPacket.payload.setAzEl.az = az_current;

                    if (queueCommand(&commandPacket)){
                        mode_setting_active = 0;
                        current_mode_setting = msNone;
                        setEl_desired(set_az_el_value);
                    }

                    break;
                case msNone:
                case msLast:
                default:
                    break;
            }
        }
    }


    display->refresh();
}

void Controller::checkCommandsQueue() {
    if (commands_queue_counter){
        if (sendCommand(&commands_queue[0])){
            if (commands_queue_counter > 1){
                for (int i = 0; i < commands_queue_counter; ++i) {
                    commands_queue[i] = commands_queue[i + 1];
                }
            }
            commands_queue_counter--;
        }
    }
}

bool Controller::sendCommand(CommandPacket *pPacket) {
    HAL_StatusTypeDef s;
    HAL_GPIO_WritePin(green_led_GPIO_Port, green_led_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
    s = HAL_UART_Transmit(this->comm_uart, (uint8_t *)pPacket, sizeof(CommandPacket), 100);
    HAL_GPIO_WritePin(green_led_GPIO_Port, green_led_Pin, GPIO_PIN_RESET);
    if (s == HAL_OK){
        __HAL_UART_CLEAR_FLAG(this->comm_uart, UART_FLAG_RXNE);
        __HAL_UART_CLEAR_FLAG(this->comm_uart, UART_FLAG_ORE);
        memset((void *)&this->cmd_buffer, 0, sizeof(this->cmd_buffer));
        s = HAL_UART_Receive(this->comm_uart, (uint8_t *) &this->cmd_buffer, sizeof(this->cmd_buffer), 200);
        HAL_Delay(20);
        if (s == HAL_OK){
            this->onUARTData();
            if (!this->validateCommandPacket((CommandPacket *) &this->cmd_to_process)){
                memset((void *)&this->cmd_to_process, 0, sizeof(this->cmd_to_process));
                this->onRxError();
            } else {
                this->handleCommand((CommandPacket *) &this->cmd_to_process, nullptr);
                this->cmd_to_process.header = 0;
                return true;
            }
        } else {
            if (s == HAL_TIMEOUT){
                this->onTxError();
            }
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
    float az = readRot2ProgAngle(pCmd->azimuth, pCmd->azimuth_resolution);
    float el = readRot2ProgAngle(pCmd->elevation, pCmd->elevation_resolution);
    sendAzEl(az, el);
}

void Controller::sendAzEl(float az, float el) {
    CommandPacket commandPacket;
    commandPacket.command = cmdGoToAzEl;
    commandPacket.payload.goToAzEl.az = az;
    commandPacket.payload.goToAzEl.el = el;

    if (queueCommand(&commandPacket)){
        setAz_desired(commandPacket.payload.goToAzEl.az);
        setEl_desired(commandPacket.payload.goToAzEl.el);
    }
}

void Controller::setAz_current(float az_current) {
    Controller::az_current = az_current;
    display->setAz(az_current);
}

void Controller::setAz_desired(float az_desired) {
    if (az_desired > 360){
        az_desired = 360;
    }
    if (az_desired < 0){
        az_desired = 0;
    }
    Controller::az_desired = az_desired;
    display->setAzTarget(az_desired);
}

void Controller::setEl_current(float el_current) {
    Controller::el_current = el_current;
    display->setEl(el_current);
}

void Controller::setEl_desired(float el_desired) {
    if (el_desired > 90){
        el_desired = 90;
    }
    if (el_desired < 0){
        el_desired = 0;
    }
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
        case cmdReadEncodersResponse:
            raw_enc_az = pPacket->payload.readEncodersResponse.az;
            raw_enc_el = pPacket->payload.readEncodersResponse.el;
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

bool Controller::queueCommand(const CommandPacket *const pPacket) {
    if (commands_queue_counter >= MAX_COMMANDS_IN_QUEUE){
        // TODO: error handling
        return false;
    } else {
        memcpy(&commands_queue[commands_queue_counter], pPacket, sizeof(CommandPacket));
        commands_queue[commands_queue_counter].header = packetHeader;
        commands_queue[commands_queue_counter].crc = getPacketCRC(&commands_queue[commands_queue_counter]);

        commands_queue_counter++;
    }
    return true;
}

char *Controller::getModeSettingName(ModeSetting setting) {
    switch (setting){
        case msSetAz:
            strcpy(current_mode_setting_name, "Ustaw AZ");
            break;
        case msSetEl:
            strcpy(current_mode_setting_name, "Ustaw EL");
            break;
        case msNone:
        case msLast:
        default:
            strcpy(current_mode_setting_name, "Normal");
            break;
    }
    return current_mode_setting_name;
}

