//
// Created by Admin on 2017-04-30.
//

#include "Controller.h"

Controller::Controller(Display *display) : display(display) {

}

void Controller::handleUartRx(uint8_t *data, uint32_t len) {

}

void Controller::init() {
    display->init();
}

void Controller::loop() {
    display->refresh();
}
