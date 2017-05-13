//
// Created by thoma on 10-May-17.
//

#include "MySoftSerial.h"
#include "Manchester.h"

uint32_t overhead_us = 200 * 1000000 / SystemCoreClock;

MySoftSerial::MySoftSerial(PinName TX, PinName RX, const char *name) : SoftSerial(TX, RX, name) {
    max_time_between_transmission_us = 100000;
    calc_required_correction_symbols();
    format(16, SoftSerial::None, 1);
    start_bits = 4;
    dc_offset_timer.reset();
    dc_offset_timer.start();
}

void MySoftSerial::calc_required_correction_symbols() {
    required_correction_transmissions = (int) ceil(30000.0 / (16 * bit_period));
}

MySoftSerial::~MySoftSerial() {

}

int MySoftSerial::putc(int c) {
    c = Manchester::encode(c);
    // check if last transmission was not too long ago
    dc_offset_timer.stop();
    if (dc_offset_timer.read_us() > max_time_between_transmission_us) {
        // transmission was too long ago, must correct the dc offset
        correct_dc_offset();
    }
    return _putc(c);
}

int MySoftSerial::getc() {
    int character = _getc();
    return Manchester::decode(character);
}

void MySoftSerial::prepare_tx(int c) {
    // _char is transmitted lsb first
    _char = c << start_bits; // make room for start bits
    _char |= 0b1010; //add start bits (lsb first)
    _char |= 0xFFFF << (start_bits + _bits); // pad with stop bits
    _char &= ~(1<<_total_bits); // zero after last stop bit   ~(1<<_total_bits) = ...111101111...
}

void MySoftSerial::baud(int baudrate) {
    SoftSerial::baud(baudrate);
    calc_required_correction_symbols();
}

int MySoftSerial::readable() {
    return SoftSerial::readable();
}

int MySoftSerial::writeable() {
    return SoftSerial::writeable();
}

void MySoftSerial::correct_dc_offset() {
    for (int i = 0; i < required_correction_transmissions; ++i) {
        //16 bit_periods
        tx->write(0);
        wait_us(bit_period * 12); // send 12 zeros
        tx->write(1);
        wait_us(bit_period * 4); // send 4 ones
    }
}

void MySoftSerial::rx_gpio_irq_handler(void) {
    rxticker.prime();
    rxticker.setNext(bit_period + (bit_period >> 1) - overhead_us); // jump 1.5 bit periods
    rx->fall(NULL);
    rxticker.attach(this, &MySoftSerial::rx_detect_start);
    start_bit = 1; // start from second bit
    rx_bit = 0;
    rx_error = false;
}

void MySoftSerial::rx_detect_start(void){
    int val = rx->read();

    // check start pattern
    // todo store pattern in array or int
    bool ok;
    switch(start_bit){
        case 0:
            ok = val==0;
            break;
        case 1:
            ok = val==1;
            break;
        case 2:
            ok = val==0;
            break;
        case 3:
            ok = val==1;
            rxticker.attach(this, &MySoftSerial::tx_handler);
            break;
        default:
            ok = false;
    }

    if(ok){
        rx_bit++;
        rxticker.setNext(bit_period);
    } else {
        // start pattern was not correct, this is not a data packet
        rxticker.detach();
        rx->fall(this, &SoftSerial::rx_gpio_irq_handler);
    }
}

void MySoftSerial::format(int bits, SoftSerial::Parity parity, int stop_bits) {
    SoftSerial::format(bits, parity, stop_bits);
    _total_bits = 4 + _bits + _stop_bits;
}

void MySoftSerial::tx_handler(void) {
    if (tx_bit == _total_bits) {
        tx_bit = -1;
        // transmission done, start measuring time until next transmission
        dc_offset_timer.reset();
        dc_offset_timer.start();

        fpointer[TxIrq].call();
        return;
    }

    //Flip output
    int cur_out = tx->read();
    tx->write(!cur_out);

    //Calculate when to do it again
    int count = bit_period;
    tx_bit++;
    while(((_char >> tx_bit) & 0x01) == !cur_out) {
        count+=bit_period;
        tx_bit++;
    }

    txticker.setNext(count);
}
