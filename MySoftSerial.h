//
// Created by thoma on 10-May-17.
//

#ifndef VISIBLE_LIGHT_COMMUNICATION_MYSOFTSERIAL_H
#define VISIBLE_LIGHT_COMMUNICATION_MYSOFTSERIAL_H

#include "mbed.h"
#include "SoftSerial_Ticker.h"

class MySoftSerial {
public:
    MySoftSerial(PinName TX, PinName RX, const char *name = NULL);

    enum Parity {
        None = 0,
        Odd,
        Even,
        Forced1,
        Forced0
    };

    enum IrqType {
        RxIrq = 0,
        TxIrq
    };

protected:

    DigitalOut *tx;
    InterruptIn *rx;

    bool tx_en, rx_en;
    int bit_period;
    int _bits, _stop_bits, _total_bits;
    Parity _parity;

    FunctionPointer fpointer[2];

    int read_buffer, rx_bit;
    volatile int out_buffer;
    volatile bool out_valid;
    bool rx_error;
    FlexTicker rxticker;
    FlexTicker txticker;
    int _char;
    volatile int tx_bit;

    Timer dc_offset_timer;
    int max_time_between_transmission_us;
    virtual void rx_gpio_irq_handler(void);
    void rx_handler(void);
    int required_correction_transmissions;
    virtual void format(int bits, Parity parity, int stop_bits);
    virtual int _putc(int c);
    virtual int _getc( void );

    virtual void tx_handler(void);
    virtual void prepare_tx(int c);
    virtual void rx_detect_start(void);
    int start_bit;
    int start_bits;
    uint32_t overhead_us;

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
