//
// Created by thoma on 10-May-17.
//

#include "MySoftSerial.h"
#include "Manchester.h"

MySoftSerial::MySoftSerial(PinName TX, PinName RX, const char *name) : SoftSerial(TX, RX, name) {
    format(16, SoftSerial::None, 1);
}

MySoftSerial::~MySoftSerial() {

}

int MySoftSerial::putc(int c) {
    c = Manchester::encode(c);
    return _putc(c);
}

int MySoftSerial::getc() {
    return Manchester::decode(_getc());
}

void MySoftSerial::prepare_tx(int c) {

    SoftSerial::prepare_tx(c);
}

void MySoftSerial::baud(int baudrate) {
    SoftSerial::baud(baudrate);
}

int MySoftSerial::readable() {
    return SoftSerial::readable();
}

int MySoftSerial::writeable() {
    return SoftSerial::writeable();
}
