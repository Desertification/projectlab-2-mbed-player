//
// Created by thoma on 02-May-17.
//

#ifndef MANCHESTER_MANCHESTER_H
#define MANCHESTER_MANCHESTER_H

namespace Manchester {
    void encode_manchester(char *in, int size, char *out);
    bool decode_manchester(char *in, int size, char *out);
    void print_char_bits(char *buffer, int size);
    void debug();
}


#endif //MANCHESTER_MANCHESTER_H
