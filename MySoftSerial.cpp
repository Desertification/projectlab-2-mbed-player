//
// Created by thoma on 10-May-17.
//

#include "MySoftSerial.h"
#include "Manchester.h"


MySoftSerial::MySoftSerial(PinName TX, PinName RX, const char *name) {
//MySoftSerial::MySoftSerial(PinName TX, PinName RX, const char *name) : SoftSerial(TX, RX, name) {
    tx_en = rx_en = false;
    read_buffer = 0;
    if (TX != NC) {
        tx = new DigitalOut(TX);
        tx_en = true;
        tx->write(1);
        tx_bit = -1;
        txticker.attach(this, &MySoftSerial::tx_handler);
    }
    if (RX != NC) {
        rx = new InterruptIn(RX);
        rx_en = true;
        out_valid = false;
        rxticker.attach(this, &MySoftSerial::rx_handler);
        rx->fall(this, &MySoftSerial::rx_gpio_irq_handler);
    }

    baud(9600);

    max_time_between_transmission_us = 10000;
    calc_required_correction_symbols();
    format(16, MySoftSerial::None, 1);
    start_bits = 4;
    dc_offset_timer.reset();
    dc_offset_timer.start();
    overhead_us = 200 * 1000000 / SystemCoreClock;

}

void MySoftSerial::calc_required_correction_symbols() {
    required_correction_transmissions = (int) ceil(30000.0 / (16 * bit_period));
}

MySoftSerial::~MySoftSerial() {
    if (tx_en)
        delete(tx);
    if (rx_en)
        delete(rx);
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

int MySoftSerial::_putc(int c)
{
    while(!writeable());
    prepare_tx(c);
    tx_bit = 0;
    txticker.prime();
    tx_handler();
    return 0;
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
    bit_period = 1000000 / baudrate;
    calc_required_correction_symbols();
}

int MySoftSerial::readable() {
    return out_valid;
}

int MySoftSerial::writeable() {
    if (!tx_en)
        return false;
    if (tx_bit == -1)
        return true;
    return false;
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

int MySoftSerial::_getc( void ) {
    while(!readable());
    out_valid = false;
    return out_buffer;
}

void MySoftSerial::rx_gpio_irq_handler(void) {
    rxticker.prime();
    rxticker.setNext(bit_period + (bit_period >> 1) - overhead_us); // jump 1.5 bit periods
    rx->fall(NULL);
    rxticker.attach(this, &MySoftSerial::rx_detect_start);
    start_bit = 1; // start from second bit
    rx_bit = 0;
    rx_error = false;
    printf("q\n");
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
            printf("a\n");
            break;
        case 2:
            ok = val==0;
            printf("b\n");
            break;
        case 3:
            printf("c\n");
            ok = val==1;
            rxticker.attach(this, &MySoftSerial::rx_handler);
            break;
        default:
            printf("d\n");
            ok = false;
    }

    if(ok){
        start_bit++;
        rxticker.setNext(bit_period);
    } else {
        // start pattern was not correct, this is not a data packet
        rxticker.detach();
        rx->fall(this, &MySoftSerial::rx_gpio_irq_handler);
    }
}

void MySoftSerial::rx_handler(void) {
    //Receive data
    printf("%i\n",rx_bit);
    int val = rx->read();
 
    rxticker.setNext(bit_period);
    rx_bit++;
    
    
    if (rx_bit <= _bits) {
        read_buffer |= val << (rx_bit - 1);
        return;
    }
    
    //Receive parity
    bool parity_count;
    if (rx_bit == _bits + 1) {
        switch (_parity) {
            case Forced1:
                if (val == 0)
                    rx_error = true;
                return;
            case Forced0:
                if (val == 1)
                    rx_error = true;
                return;
            case Even:
            case Odd:
                parity_count = val;
                for (int i = 0; i<_bits; i++) {
                    if (((read_buffer >> i) & 0x01) == 1)
                        parity_count = !parity_count;
                }
                if ((parity_count) && (_parity == Even))
                    rx_error = true;
                if ((!parity_count) && (_parity == Odd))
                    rx_error = true;
                return;
        }
    }
    
    //Receive stop
    if (rx_bit < _bits + (bool)_parity + _stop_bits) {
        if (!val)
            rx_error = true;
        return;
    }    
    
    //The last stop bit
    if (!val)
        rx_error = true;
    
    if (!rx_error) {
        out_valid = true;
        out_buffer = read_buffer;
        fpointer[RxIrq].call();
    }
    read_buffer = 0;
    rxticker.detach(); 
    rx->fall(this, &MySoftSerial::rx_gpio_irq_handler);
}

void MySoftSerial::format(int bits, MySoftSerial::Parity parity, int stop_bits) {
    _bits = bits;
    _parity = parity;
    _stop_bits = stop_bits;
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
