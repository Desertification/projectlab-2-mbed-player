
#include "mbed.h"
#include "Manchester.h"
//#include "SoftSerial.h"
#include "MySoftSerial.h"
#include "SDFileSystem.h"
#include "wave_player.h"


Serial usb(USBTX,USBRX);
SDFileSystem sd(p5, p6, p7, p8, "sd");


void test_player(){
    AnalogOut DACout(p18);
    wave_player waver(&DACout);
    waver.set_verbosity(0);
    sd.disk_initialize();
    FILE *wave_file;
    if(wave_file == NULL) {
        error("Could not open file for write\n");
    }
    printf("Hello, wave world!\n");
    wave_file=fopen("/sd/test.wav","r");
    waver.play(wave_file);
    fclose(wave_file);
}

void client(){
    MySoftSerial s(p18,p17);
    
    s.baud(20000);
    while (true) {
        while (!s.readable());
        printf("got %i\r\n",s.getc());
    }
}

void relay(){
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

#define RELAY 1

int main(){
    usb.baud(115200);
    
    //test_player();
    
    if (RELAY) {
        printf("relay\r\n");
        relay();
    } else {
        printf("client\r\n");
        client();
    }
}