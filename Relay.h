//
// Created by thoma on 16-May-17.
//

#ifndef VISIBLE_LIGHT_COMMUNICATION_RELAY_H
#define VISIBLE_LIGHT_COMMUNICATION_RELAY_H


#include "mbed.h"
#include "MySoftSerial.h"

class Relay {
public:
    Relay(PinName tx_luart, int baudrate_luart, PinName tx_usb, PinName rx_usb, int baudrate_usb);
    void run();

private:
    MySoftSerial softSerial;
    Serial usb;

    static const int SEND_FILE = 0b00001111;
    static const int SET_VOLUME = 0b00110011;
    static const int ACK = 0b11111111;
    int buffer[64];

    void handle_type();

    void set_volume();

    void send_file();
};


#endif //VISIBLE_LIGHT_COMMUNICATION_RELAY_H
