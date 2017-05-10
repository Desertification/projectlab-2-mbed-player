
#include "mbed.h"
#include "rtos.h"
#include "Manchester.h"
#include "SoftSerial.h"
#include "MySoftSerial.h"

//Serial out(p9, p10, 3200); //p9
//Serial in(p13, p14, 3200); //p14
//Serial usb(USBTX,USBRX);
//
//char *read_2_bytes(Serial *serial, char out[2]) {
//    int i = 0;
//    while (true) {
//        while (!serial->readable());
//        char c = (char) serial->getc();
//        if (c == 0b11100011) //no data flag
//            continue;
//        else {
//            out[i] = c;
//            i++;
//        }
//
//        if (i == 2) {
//            break;
//        }
//    }
//    return out;
//}
//
//
//void receive_uart() {
//    while (true) {
//        if (in.readable()) {
//            char s[2];
//            read_2_bytes(&in, s);
//            char dec[1];
//            bool success = Manchester::decode_manchester(s, 2, dec);
//            printf(dec);
//            if (!success) {
//                printf("#");
//            }
//        } else {
//            Thread::yield();
//        }
//    }
//}
//
//void send_uart(Serial *serial, const char *s, int size) {
//    int size_m = size * 2;
//    char encoded[size_m];
//    Manchester::encode_manchester(s, size, encoded);
//
//    for (int i = 0; i < size_m; ++i) {
//        while (!serial->writeable());
//        serial->putc(encoded[i]);
//    }
//}
//
//void hardware_uart_test(){
//
//
//    printf("%s\r\n", "hello, term!");
//
//    Thread thread;
//    thread.start(receive_uart);
//
//    while (true) {
//        char tosend[28];
//        Manchester::encode_manchester("hello, term\r\n", 14, tosend);
//        send_uart(&out, tosend, 28);
//        //wait(1);
//    }
//}

void client(){
    MySoftSerial s(p18,p17);
    s.baud(1);
    s.format(16, SoftSerial::None, 1);
    while (true) {
        while (!s.readable());
        printf("got %i\r\n",s.getc());
    }
}

void relay(){
    MySoftSerial s(LED1,p18);
    s.baud(1);
    s.format(16, SoftSerial::None, 1);
    int i = 0;
    while (true){
        wait(0.1);
        while(!s.writeable());
        printf("put %i\r\n", 0b0101010100110011);
        s.putc(0b0101010100110011);
        i++;
    }
}

#define RELAY 1

Serial usb(USBTX,USBRX);

int main() {
    usb.baud(115200);
    if (RELAY){
        printf("relay\r\n");
        relay();
    } else {
        printf("client\r\n");
        client();
    }
}