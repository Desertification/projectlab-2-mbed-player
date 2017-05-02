
#include "mbed.h"
#include "rtos.h"
#include "Manchester.h"

Serial out(p9, p10, 3200); //p9
Serial in(p13, p14, 3200); //p14


void receive_uart() {
    while (true){
        if(in.readable()){
            char s[28];
            in.gets(s, 28);

            char dec[14];
            Manchester::decode_manchester(s,28,dec);

            printf(dec);
        }else{
            Thread::yield();
        }
    }
}

int main() {
    printf("%s\r\n", "hello, term!");

    Thread thread;
    thread.start(receive_uart);

    while (true){
        while (!out.writeable());

        char tosend[28];
        Manchester::encode_manchester("hello, term\r\n",14,tosend);
        out.printf(tosend);
        wait(1);
    }

}