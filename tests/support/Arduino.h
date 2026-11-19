#ifndef TEST_SUPPORT_ARDUINO_H
#define TEST_SUPPORT_ARDUINO_H

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iomanip>

using uint8_t = std::uint8_t;
using uint16_t = std::uint16_t;
using uint32_t = std::uint32_t;
using int32_t = std::int32_t;

struct __FlashStringHelper;
using PGM_P = const char*;
using PGM_VOID_P = const void*;

#define PROGMEM
#define F(str_literal) reinterpret_cast<const __FlashStringHelper*>(str_literal)
#define memcpy_P std::memcpy
#define memcmp_P std::memcmp

template <typename T>
inline T pgm_read_word(const T* ptr) {
  return *ptr;
}

template <typename T>
inline T pgm_read_ptr(const T* ptr) {
  return *ptr;
}

extern uint64_t __test_millis;
inline uint64_t millis() {
  return __test_millis;
}

class Stream {
  public:
    virtual ~Stream() = default;
    virtual int available() = 0;
    virtual int read() = 0;
    virtual std::size_t write(uint8_t c) = 0;

    std::size_t print(char c) {
      return write(static_cast<uint8_t>(c));
    }

    std::size_t print(const char* str) {
      if (!str) {
        return 0;
      }
      std::size_t written = 0;
      while (*str != '\0') {
        written += write(static_cast<uint8_t>(*str++));
      }
      return written;
    }

    std::size_t print(const __FlashStringHelper* str) {
      return print(reinterpret_cast<const char*>(str));
    }

    std::size_t print(int32_t value) {
      return print(std::to_string(value).c_str());
    }

    std::size_t print(uint32_t value) {
      return print(std::to_string(value).c_str());
    }

    std::size_t print(float value) {
      std::ostringstream oss;
      oss << std::setprecision(6) << value;
      return print(oss.str().c_str());
    }

    std::size_t println() {
      return print('\n');
    }

    std::size_t println(char c) {
      return print(c) + println();
    }

    std::size_t println(const char* str) {
      return print(str) + println();
    }

    std::size_t println(const __FlashStringHelper* str) {
      return print(str) + println();
    }

    std::size_t println(int32_t value) {
      return print(value) + println();
    }

    std::size_t println(uint32_t value) {
      return print(value) + println();
    }

    std::size_t println(float value) {
      return print(value) + println();
    }
};

#endif
