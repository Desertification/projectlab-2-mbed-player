//
// Created by thoma on 10-May-17.
//

#ifndef VISIBLE_LIGHT_COMMUNICATION_MYSOFTSERIAL_H
#define VISIBLE_LIGHT_COMMUNICATION_MYSOFTSERIAL_H

#include "SoftSerial.h"

class MySoftSerial : protected SoftSerial {
public:
    MySoftSerial(PinName TX, PinName RX, const char *name = NULL);

protected:
    virtual void prepare_tx(int c);
    Timer timer;
    int max_time_between_transmission_us = 100000;

public:
    virtual ~MySoftSerial();

    int putc(int c);
    int getc();
    virtual void baud(int baudrate);
    virtual int readable();
    virtual int writeable();

    void correct_dc_offset();
};


#endif //VISIBLE_LIGHT_COMMUNICATION_MYSOFTSERIAL_H
