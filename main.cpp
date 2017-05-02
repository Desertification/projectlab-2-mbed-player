
#include "mbed.h"

int main() {
    printf("%s", "hello, term");
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