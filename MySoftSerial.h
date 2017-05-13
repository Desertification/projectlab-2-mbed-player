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
    Timer dc_offset_timer;
    virtual int max_time_between_transmission_us;
    virtual void rx_gpio_irq_handler(void);
    virtual int required_correction_transmissions;
    virtual void format(int bits, Parity parity, int stop_bits);

    virtual void tx_handler(void);
    virtual void rx_detect_start(void);
    int start_bit;
    int start_bits;

public:
    virtual ~MySoftSerial();

    int putc(int c);
    int getc();
    virtual void baud(int baudrate);
    virtual int readable();
    virtual int writeable();
    void correct_dc_offset();

    void calc_required_correction_symbols();

};


#endif //VISIBLE_LIGHT_COMMUNICATION_MYSOFTSERIAL_H
