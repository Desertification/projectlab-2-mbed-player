
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
    printf("%s\n", "hello, term");

    while (true){
        out.printf("hello, term\n");
        wait(1);
    }

}