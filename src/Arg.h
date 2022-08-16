/*---------------------------------------------------------------------
Author         : naszly
License        : BSD
Repository     : https://github.com/naszly/Arduino-StaticSerialCommands
-----------------------------------------------------------------------*/

#ifndef STATIC_SERIAL_COMMANDS_ARG_H
#define STATIC_SERIAL_COMMANDS_ARG_H

#include "Arduino.h"

#define MAX_ARGS 16

#define GET_1_CHAR(str, i) (sizeof(str) > (i) ? str[(i)] : 0)

#define GET_4_CHAR(str, i) \
  GET_1_CHAR(str, i + 0), GET_1_CHAR(str, i + 1), \
  GET_1_CHAR(str, i + 2), GET_1_CHAR(str, i + 3)

#define GET_16_CHAR(str, i) \
  GET_4_CHAR(str, i + 0), GET_4_CHAR(str, i + 4), \
  GET_4_CHAR(str, i + 8), GET_4_CHAR(str, i + 12)

#define GET_STR(str) GET_16_CHAR(str, 0), 0 // guard for longer strings

#define ARG_1(type) \
  impl::ArgConstraint(type)

#define ARG_2(type, name) \
  impl::ArgConstraint(type, impl::ArgConstraint::getName<GET_STR(name)>)

#define ARG_3(type, min, max) \
  impl::ArgConstraint(type, impl::ArgConstraint::getRange<min,max>)

#define ARG_4(type, min, max, name) \
  impl::ArgConstraint(type, impl::ArgConstraint::getRange<min,max>, impl::ArgConstraint::getName<GET_STR(name)>)

#define ARG_X(a0, a1, a2, a3, a4, FUNC, ...)  FUNC

#define ARG(...) \
  ARG_X(, ##__VA_ARGS__, \
        ARG_4(__VA_ARGS__), \
        ARG_3(__VA_ARGS__), \
        ARG_2(__VA_ARGS__), \
        ARG_1(__VA_ARGS__) \
       )

enum class ArgType : uint8_t {
  Null,
  Int,
  Float,
  String
};

union ArgData {
  int32_t num;
  float num_f;
  const char* string;
};

class Arg {
  public:
    constexpr Arg() : value{ 0 }, type(ArgType::Null) {}
    explicit Arg(int32_t value)
      : value{ .num = value }, type(ArgType::Int) {}
    explicit Arg(float value)
      : value{ .num_f = value }, type(ArgType::Float) {}
    explicit Arg(const char* value)
      : value{ .string = value }, type(ArgType::String) {}

    int32_t getInt() {
      return value.num;
    }

    float getFloat() {
      return value.num_f;
    }

    const char* getString() {
      return value.string;
    }

    ArgType getType() {
      return type;
    }

  private:
    ArgData value;
    ArgType type;
};

class Args {
  public:
    Arg& operator[](uint8_t idx) {
      return args[idx];
    }

    const Arg& operator[](uint8_t idx) const {
      return args[idx];
    }

  private:
    Arg args[MAX_ARGS];
};

namespace impl {

struct Range {
  constexpr Range(int32_t minimum, int32_t maximum) : minimum(minimum), maximum(maximum) {}
  int32_t minimum;
  int32_t maximum;
};

struct ArgConstraint {
  ArgConstraint()
    : type(ArgType::Null), getRangeFn(nullptr), getNameFn(nullptr) {}

  constexpr ArgConstraint(ArgType type)
    : type(type), getRangeFn(nullptr), getNameFn(nullptr) {}

  constexpr ArgConstraint(ArgType type,  Range (*getRangeFn)())
    : type(type), getRangeFn(getRangeFn), getNameFn(nullptr) {}

  constexpr ArgConstraint(ArgType type, void (*getNameFn)(char**))
    : type(type), getRangeFn(nullptr), getNameFn(getNameFn) {}

  constexpr ArgConstraint(ArgType type,  Range (*getRangeFn)(), void (*getNameFn)(char**))
    : type(type), getRangeFn(getRangeFn), getNameFn(getNameFn) {}

  const ArgType type;
  Range (*getRangeFn)();
  void (*getNameFn)(char**);

  void getName(char** buffer) const {
    if (getNameFn) {
      (*getNameFn)(buffer);
    } else {
      getTypeName(type, buffer);
    }
  }

  bool isInRange(Arg arg) {
    if (getRangeFn) {
      Range range = (*getRangeFn)();
      switch (arg.getType()) {
        case ArgType::Int:
          return arg.getInt() >= range.minimum && arg.getInt() <= range.maximum;
        case ArgType::Float:
          return arg.getFloat() >= range.minimum && arg.getFloat() <= range.maximum;
        default:
          return true;
      }
    }
    return true;
  }

  Range getRange() {
    if (getRangeFn) {
      return (*getRangeFn)();
    }
    return Range(INT32_MIN, INT32_MAX);
  }

  static void getTypeName(ArgType type, char** buffer) {
    static const char _null[] PROGMEM = "null";
    static const char _int[] PROGMEM = "int";
    static const char _float[] PROGMEM = "float";
    static const char _string[] PROGMEM = "string";
    static const char* const types[] PROGMEM{ _null, _int, _float, _string };
    strcpy_P(*buffer, (char*)pgm_read_word(&(types[(int)type])));
  }

  template<char... chars>
  static void getName(char** buffer) {
    static const char str[] PROGMEM = { chars... };
    strcpy_P(*buffer, str);
  }

  template<int32_t min, int32_t max>
  static Range getRange() {
    return Range(min, max);
  }

};

}

#endif // STATIC_SERIAL_COMMANDS_ARG_H
