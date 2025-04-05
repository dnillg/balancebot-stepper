#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <Arduino.h>

int indexOfCharInFirstN(const String &str, char ch, unsigned int lookupLen) {
    int len = min(lookupLen, str.length());  // Limit search to first 'n' characters
    for (int i = 0; i < len; i++) {
        if (str[i] == ch) return i;
    }
    return -1;
}

#endif // UTILS_HPP_