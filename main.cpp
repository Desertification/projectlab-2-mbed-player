
#include "mbed.h"
#include "rtos.h"
#include "Manchester.h"

Serial out(p9, p10, 3200); //p9
Serial in(p13, p14, 3200); //p14

char *read_2_bytes(Serial *serial, char out[2]) {
    int i = 0;
    while (true) {
        while (!serial->readable());
        char c = (char) serial->getc();
        if (c == 0b11100011) //no data flag
            continue;
        else {
            out[i] = c;
            i++;
        }

        if (i == 2) {
            break;
        }
    }
    return out;
}


void receive_uart() {
    while (true) {
        if (in.readable()) {
            char s[2];
            read_2_bytes(&in, s);
            char dec[1];
            bool error = Manchester::decode_manchester(s, 2, dec);
            printf(dec);
            if(error){
                printf("#");
            }
        } else {
            Thread::yield();
        }
    }
}

void send_uart(Serial *serial, const char *s, int size) {
    int size_m = size * 2;
    char encoded[size_m];
    Manchester::encode_manchester(s, size, encoded);
    
    for (int i = 0; i < size_m; ++i) {
        while (!serial->writeable());
            serial->putc(encoded[i]);
    }
}

int main() {
    printf("%s\r\n", "hello, term!");

    Thread thread;
    thread.start(receive_uart);

    while (true) {
        char tosend[28];
        Manchester::encode_manchester("hello, term\r\n", 14, tosend);
        send_uart(&out, tosend, 28);
        //wait(1);
    }

}