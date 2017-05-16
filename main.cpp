
#include "mbed.h"
#include "Manchester.h"
//#include "SoftSerial.h"
#include "MySoftSerial.h"
#include "SDFileSystem.h"
#include "wave_player.h"
#include "Relay.h"
#include "Client.h"


void test_player(){
    SDFileSystem sd(p5, p6, p7, p8, "sd");
    Serial usb(USBTX,USBRX);
    usb.baud(115200);
    AnalogOut DACout(p18);
    wave_player waver(&DACout);
    waver.set_verbosity(0);
    sd.disk_initialize();
    FILE *wave_file;
    printf("Hello, wave world!\n");
    wave_file=fopen("/sd/test.wav","r");
    if(wave_file == NULL) {
        error("Could not open file for write\n");
    }
    waver.play(wave_file);
    fclose(wave_file);
}

void test_client(){
    Serial usb(USBTX,USBRX);
    usb.baud(115200);

    MySoftSerial s(p18,p17);
    
    s.baud(20000);
    while (true) {
        while (!s.readable());
        printf("got %i\r\n",s.getc());
    }
}

void test_relay(){
    Serial usb(USBTX,USBRX);
    usb.baud(115200);

    MySoftSerial s(p17,p18);
    //MySoftSerial s(LED1,p18);
    s.baud(20000);
    int i = 0;
    while (true) {
        //wait(0.1);
        for (int j = 0; j < 1; ++j) {
            while(!s.writeable());
            printf("put %i\r\n", i);
            s.putc(i);
            i++;
        }
    }
}

#define RELAY 0

int main(){
    
    //test_player();
    
    if (RELAY) {
        Relay relay(p17,20000,USBTX,USBRX,115200);
        relay.run();
        //test_relay();
    } else {
        Client client(p17,20000);
        client.run();
        //test_client();
    }
}