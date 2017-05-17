//
// Created by thoma on 17-May-17.
//

#include "Client.h"

Client::Client(PinName rx_luart, int baudrate_luart) :
        aout(p18), fileSystem(p5, p6, p7, p8, "sd"), player(&aout), softSerial(NC, rx_luart),
        spi_amp(p11, p12, p13, p14) {
    softSerial.baud(baudrate_luart);
    spi_amp.format(8, 0);
    spi_amp.frequency(1000000);
    fileSystem.disk_initialize();
    wave_file = fopen("/sd/receive.wav", "w+");
    if (wave_file == NULL) {
        error("Could not open file for w+\n");
    }
}

void Client::play_file() {
    fflush(wave_file);
    rewind(wave_file);
    printf("playing_file\r\n");
    player.play(wave_file);
    printf("done\r\n");
    rewind(wave_file);
}

void Client::set_volume() {
    while (!softSerial.readable());
    int value = softSerial.getc();

    spi_amp.write(0);
    spi_amp.write(int((value / 100.0) * 255.0));
    printf("volume set to %i\r\n", value);
}

void Client::run() {
    while (true) {
        handle_type();
    }
}

void Client::handle_type() {
    while (!softSerial.readable());
    int packet_type = softSerial.getc();

    switch (packet_type) {
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

void Client::send_file() {
    printf("starting file write\r\n");
    // get file size
    int size = 0;
    for (int i = 0; i < 4; ++i) {
        while (!softSerial.readable());
        int b = softSerial.getc();
        size |= b << ((3 - i) << 3);
    }
    printf("must write %i bytes\r\n", size);


    // send data
    int j = 0;
    while (j < size) {

        // get max 64 bytes
        int k = 0;
        for (k; k < 64; ++k) {
            if (j == size) {
                break;
            }
            while (!softSerial.readable());
            int b = softSerial.getc();
            buffer[k] = (char) b;
            j++;
        }

        // store max 64 bytes
        fwrite(buffer, sizeof(char), (size_t) k, wave_file);
        //printf("wrote %i:%i bytes\r\n",k,j);
    }

    play_file();
}