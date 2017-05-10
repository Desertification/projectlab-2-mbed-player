//
// Created by thoma on 10-May-17.
//

#include "MySoftSerial.h"

MySoftSerial::MySoftSerial(PinName TX, PinName RX, const char *name) : SoftSerial(TX, RX, name) {

}

MySoftSerial::~MySoftSerial() {

}

int MySoftSerial::putc(int c) {
    return _putc(c);
}

int MySoftSerial::getc() {
    return _getc();
}
