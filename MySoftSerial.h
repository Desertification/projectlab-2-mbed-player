//
// Created by thoma on 10-May-17.
//

#ifndef VISIBLE_LIGHT_COMMUNICATION_MYSOFTSERIAL_H
#define VISIBLE_LIGHT_COMMUNICATION_MYSOFTSERIAL_H

#include "SoftSerial.h"

class MySoftSerial : public SoftSerial {
public:
    MySoftSerial(PinName TX, PinName RX, const char *name = NULL);

    virtual ~MySoftSerial();

    int putc(int c);
    int getc();


};


#endif //VISIBLE_LIGHT_COMMUNICATION_MYSOFTSERIAL_H
