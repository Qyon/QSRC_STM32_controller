//
// Created by Admin on 2017-04-30.
//

#ifndef QSRC_STM32_CONTROLLER_CONTROLLER_H
#define QSRC_STM32_CONTROLLER_CONTROLLER_H

#include "Display.h"
#include "Rot2Prog.h"

class Controller {
private:
    Display * display;
public:
    Controller(Display *display);

    void handleUartRx(uint8_t *data, uint32_t len);

    void init();

    void loop();
};


#endif //QSRC_STM32_CONTROLLER_CONTROLLER_H
