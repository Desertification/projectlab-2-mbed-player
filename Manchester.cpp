#include <iostream>
#include "Manchester.h"


void ::Manchester::encode_manchester(const char *in, int size, char *out) {
    int m_char_index = 0; // index for expanded manchester char

    // iterate every char
    for (int char_index = 0; char_index < size; char_index++) {
        bool bits[16]; // manchester char
        int m_bit_index = 0;

        //split in bits and insert inverse
        for (int i = 0; i < 8; i++) { // iterate over all the bits in the char
            bool bit = (bool) ((in[char_index] >> 7 - i) & 1);
            bits[m_bit_index] = bit;
            m_bit_index++;
            bits[m_bit_index] = !bit;
            m_bit_index++;
        }

        //combine back to 2 chars
        int bit_index = 0;
        for (int i = 0; i < 2; ++i) {
            out[m_char_index] = 0;
            for (int j = 0; j < 8; j++) { //assemble first char
                out[m_char_index] |= (bits[bit_index] << (7 - j));
                bit_index++;
            }
            m_char_index++;
        }
    }
}

bool ::Manchester::decode_manchester(const char *in, int size, char *out) {
    for (int m_char_index = 0; m_char_index < size; ++m_char_index) {
        bool bits[8];
        bool bits_inverted[8];
        int bit_index = 0;

        // split bits and put in bit arrays
        for (int i = 0; i < 2; ++i) {
            for (int m_bit_index = 0; m_bit_index < 8; ++m_bit_index) {
                bool bit = (bool) ((in[m_char_index + i] >> 7 - m_bit_index) & 1);
                bits[bit_index] = bit;
                m_bit_index++;
                bool bit_inv = (bool) ((in[m_char_index + i] >> 7 - m_bit_index) & 1);
                bits_inverted[bit_index] = bit_inv;
                bit_index++;

                // test if correct manchester encoded
                if (bit == bit_inv) {
                    return false; //bit flip in transport
                }
            }
        }
        // assemble char
        int char_index = m_char_index >> 1;
        out[char_index] = 0;
        for (int i = 0; i < 8; ++i) {
            out[char_index] |= (bits[i] << (7 - i));
        }
        m_char_index++;
    }
    return true;
}

void ::Manchester::print_char_bits(const char *buffer, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < 8; j++) {
            int bit = ((buffer[i] >> 7 - j) & 1);
            printf("%i", bit);
        }
        printf(" ");
    }
}

void ::Manchester::debug() {
    char s[32] = "test 1 2 3 4, hello";
    const int size = sizeof(s);
    printf("size: %i\r\n", size);

    print_char_bits(s, size);
    printf("\n");

    char m[size * 2];
    encode_manchester(s, size, m);

    print_char_bits(m, size * 2);
    printf("\n");

    char decoded[size];
    decode_manchester(m, size * 2, decoded);

    printf("\n");
    print_char_bits(decoded, size);
    printf("\n");
    printf(decoded);
}


