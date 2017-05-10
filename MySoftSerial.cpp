//
// Created by thoma on 10-May-17.
//

#include "MySoftSerial.h"
#include "Manchester.h"

MySoftSerial::MySoftSerial(PinName TX, PinName RX, const char *name) : SoftSerial(TX, RX, name) {
    format(16, SoftSerial::None, 1);
    timer.reset();
    timer.start();
}

MySoftSerial::~MySoftSerial() {

}

int MySoftSerial::putc(int c) {
    c = Manchester::encode(c);
    timer.stop();
    if(timer.read_us() > max_time_between_transmission_us){
        correct_dc_offset();
    }
    return _putc(c);
}

int MySoftSerial::getc() {
    int character = _getc();
    timer.reset();
    timer.start();
    return Manchester::decode(character);
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

void MySoftSerial::correct_dc_offset() {
    for (int i = 0; i < 1; ++i) {
        tx->write(0);
        wait_us(bit_period * 12); // send 12 zeros
        tx->write(1);
        wait_us(bit_period * 4); // send 4 ones
    }
}
