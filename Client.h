//
// Created by thoma on 17-May-17.
//

#ifndef VISIBLE_LIGHT_COMMUNICATION_CLIENT_H
#define VISIBLE_LIGHT_COMMUNICATION_CLIENT_H

#include "mbed.h"
#include "MySoftSerial.h"
#include "SDFileSystem.h"
#include "wave_player.h"

class Client {
public:
    Client(PinName rx_luart, int baudrate_luart);
    void run();

private:

    SPI spi_amp;
    MySoftSerial softSerial;
    SDFileSystem fileSystem;
    AnalogOut aout;
    DigitalOut dout;
    wave_player player;
    FILE *wave_file;

    static const int SEND_FILE = 0b00001111;
    static const int SET_VOLUME = 0b00110011;
    static const int ACK = 0b11111111;
    static const int PLAY_LAST = 0b10001011;
    char buffer[512];


    void set_volume();
    void play_file();

    void handle_type();

    void send_file();
};


#endif //VISIBLE_LIGHT_COMMUNICATION_CLIENT_H
