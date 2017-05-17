//
// Created by thoma on 16-May-17.
//

#include "Relay.h"


Relay::Relay(PinName tx_luart, int baudrate_luart, PinName tx_usb, PinName rx_usb, int baudrate_usb) :
        softSerial(tx_luart,NC), usb(tx_usb,rx_usb,baudrate_usb) {
    softSerial.baud(baudrate_luart);
}

void Relay::run() {
    while(true){
        handle_type();
    }
}

void Relay::play_last(){
    
}

void Relay::set_volume(){
    while (!usb.readable());
    int volume = usb.getc();

    while(!softSerial.writeable());
    softSerial.putc(volume);

    while (!usb.writeable());
    usb.putc(ACK);
}

void Relay::send_file(){
    // get file size
    int size = 0;
    for (int i = 0; i < 4; ++i) {
        while (!usb.readable());
        int b = usb.getc();
        while(!softSerial.writeable());
        softSerial.putc(b);
        while (!usb.writeable());
        usb.putc(ACK);
        size |=  b << ((3-i) << 3);
    }

    // send data
    int j = 0;
    while(j < size){

        // get max 64 bytes
        int k = 0;
        for (k; k < 512; ++k) {
            if(j == size){
                break;
            }
            while (!usb.readable());
            int b = usb.getc();
            buffer[k] = b;
            j++;
        }

        // send max 64 bytes
        int m = 0;
        for (m; m < k; ++m) {
            while (!softSerial.writeable());
            softSerial.putc(buffer[m]);
        }

        // request new bytes from rpi
        while (!usb.writeable());
        usb.putc(ACK);
    }
}

void Relay::handle_type() {
    while (!usb.readable());
    int packet_type = usb.getc();

    while(!softSerial.writeable());
    softSerial.putc(packet_type);

    while (!usb.writeable());
    usb.putc(ACK);

    switch (packet_type){
        case SET_VOLUME:
            set_volume();
            break;
        case SEND_FILE:
            send_file();
            break;
        default:
            break;
    }
}
