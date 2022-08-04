/*---------------------------------------------------------------------
Author         : naszly
License        : BSD
Repository     : https://github.com/naszly/Arduino-StaticSerialCommands
-----------------------------------------------------------------------*/

#ifndef PARSE_H
#define PARSE_H

#include <Arduino.h>

namespace parse {

bool strtou(const char* str, uint32_t* out) {
  uint32_t value = 0;
  for (int i = 0; str[i] != '\0'; ++i) {
    uint8_t d = str[i] - '0';
    if (d > 9) return false;
    uint32_t tmp = value;
    value = value * 10 + d;
    if (tmp > value) return false;  // overflow
  }
  *out = value;
  return true;
}

bool strtoi(const char* str, int32_t* out) {
  int32_t value = 0;
  int i = 0;
  int32_t sign = 1;
  if (str[i] == '-') {
    sign = -1;
    ++i;
  } else if (str[i] == '+') {
    ++i;
  }

  for (; str[i] != '\0'; ++i) {
    uint8_t d = str[i] - '0';
    if (d > 9) return false;
    int32_t tmp = value;
    value = value * 10 + d;
    if (tmp > value) return false;  // overflow
  }
  *out = value * sign;
  return true;
}

bool strtof(const char* str, float* out) {
  char* str_end = nullptr;
  *out = (float)strtod(str, &str_end);
  if (*out == 0.0f && str_end == str)
    return false;
  return true;
}

}

#endif
