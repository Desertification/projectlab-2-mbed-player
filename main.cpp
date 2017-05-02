
#include "mbed.h"
#include "rtos.h"

Serial out(p9, p10); //p9
Serial in(p13, p14); //p14


void receive_uart() {
    while (true){
        if(in.readable()){
            char s[32];
            printf(in.gets(s, 32));
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
        out.printf("hello, term\r\n");
//        wait(1);
    }

}