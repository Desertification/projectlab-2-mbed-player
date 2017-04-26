#define TX_MODE 0 //E
//#define TX_MODE 1 //F

#include "mbed.h"
#include "C12832.h"
#include "Buffer.h"
#include "SoftSerial.h"
#include "BufferedSoftSerial.h"

C12832 lcd(p5, p7, p6, p8, p11);
//BufferedSoftSerial buf(p9, p10);
Serial uart(p9, p10);

void to_manchester(char* in, int size, char* out){
    int m_char_index = 0;
    for(int char_index = 0; char_index < size; char_index++){
        bool bits[16]; // manchester char
        int m_bit_index = 0;
        //split in bits and insert inverse
        for(int j = 0; j < 8; j++){ // iterate over all the bits in the char
            bool bit = (bool) ((in[char_index] >> 7-j) & 1);
            bits[m_bit_index] = bit;
            m_bit_index++;
            bits[m_bit_index] = !bit;
            m_bit_index++;
        }
        //combine back to 2 chars //todo dry
        int bit_index = 0;
        for(int j = 0; j < 8; j++){ //assemble first char
            out[m_char_index] = out[m_char_index] | (bits[bit_index] << (7-j));
            bit_index++;
        }
        m_char_index++;
        for(int j = 0; j < 8; j++){ // assemble second char
            out[m_char_index] = out[m_char_index] | (bits[bit_index] << (7-j));
            bit_index++;
        }
        m_char_index++;
    }
}

void print_bits(char* buffer, int size){
    for(int i = 0; i < size; i++){
        for(int j = 0; j < 8; j++){
            int bit = ((buffer[i] >> 7-j) & 1);
            lcd.printf("%i",bit);
        }
        lcd.printf(" ");
    }
}

int main() {
    lcd.cls();
    lcd.locate(0,0);
    char s[2];
    s[0] = '\0';
    s[1] = 15;
    print_bits(s,2);
    lcd.printf("\r\n");
    char m[4];
    to_manchester(s,2,m);
    print_bits(m,4);
    /**
    uart.baud(9600);
    lcd.cls();
    lcd.printf("mbed application board!");
    while(true){
        if (TX_MODE){
            uart.printf("Hello World - buffered \r\n");
            wait(0.1f);
        } else {
            char r[64];
            //buf.gets(r, 64);
            //lcd.printf("%s",r);
            lcd.putc(uart.getc());
        }
    }**/
}




/** A serial port (UART) for communication with other serial devices
 *
 * Can be used for Full Duplex communication, or Simplex by specifying
 * one pin as NC (Not Connected)
 *
 * This uses software serial emulation, regular serial pins are alot better,
 * however if you don't have spare ones, you can use this. It is advicable
 * to put the serial connection with highest speed to hardware serial.
 *
 * If you lack RAM memory you can also use SoftSerial without this buffer around it.
 * In that case it is fully blocking.
 *
 * Example:
 * @code
 * #include "mbed.h"
 * #include "BufferedSoftSerial.h"
 * 
 * SoftSerial block(USBTX, USBRX);
* BufferedSoftSerial buf(USBTX, USBRX);
 * 
 * int main()
 * {
 *     while(1) {
 *         Timer s;
 * 
 *         s.start();
 *         buf.printf("Hello World - buffered\r\n");
 *         int buffered_time = s.read_us();
 *         wait(0.1f); // give time for the buffer to empty
 * 
 *         s.reset();
 *         block.printf("Hello World - blocking\r\n");
 *         int polled_time = s.read_us();
 *         s.stop();
 *         wait(0.1f); // give time for the buffer to empty
 * 
 *         buf.printf("printf buffered took %d us\r\n", buffered_time);
 *         buf.printf("printf blocking took %d us\r\n", polled_time);
 *         wait(5);
 *     }
 * }
 **/