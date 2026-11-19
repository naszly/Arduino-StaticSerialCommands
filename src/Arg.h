/*---------------------------------------------------------------------
Author         : naszly
License        : BSD
Repository     : https://github.com/naszly/Arduino-StaticSerialCommands
-----------------------------------------------------------------------*/

#ifndef STATIC_SERIAL_COMMANDS_ARG_H
#define STATIC_SERIAL_COMMANDS_ARG_H
#include <Arduino.h>

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
  impl::ArgConstraint(type, impl::ArgConstraint::getNamePgm<GET_STR(name)>)

#define ARG_3(type, min, max) \
  impl::ArgConstraint(type, min, max)

#define ARG_4(type, min, max, name) \
  impl::ArgConstraint(type, min, max, impl::ArgConstraint::getNamePgm<GET_STR(name)>)

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

struct IntRange {
  constexpr IntRange(int32_t minimum, int32_t maximum)
    : minimum(minimum), maximum(maximum) {}

  int32_t minimum;
  int32_t maximum;
};

struct FloatRange {
  constexpr FloatRange(float minimum, float maximum)
    : minimum(minimum), maximum(maximum) {}

  float minimum;
  float maximum;
};

union Range {
  constexpr Range() : intRange(INT32_MIN, INT32_MAX) {}
  constexpr Range(const IntRange& range) : intRange(range) {}
  constexpr Range(const FloatRange& range) : floatRange(range) {}

  IntRange intRange;
  FloatRange floatRange;
};

struct ArgConstraint {
  ArgConstraint()
    : type(ArgType::Null),
      range(makeRange(ArgType::Int, INT32_MIN, INT32_MAX)),
      hasRange(false),
      getNamePgmFn(nullptr) {}

  constexpr ArgConstraint(const ArgType type)
    : type(type),
      range(makeRange(type, INT32_MIN, INT32_MAX)),
      hasRange(false),
      getNamePgmFn(nullptr) {}

  constexpr ArgConstraint(const ArgType type, double minimum, double maximum)
    : type(type),
      range(makeRange(type, minimum, maximum)),
      hasRange(true),
      getNamePgmFn(nullptr) {}

  constexpr ArgConstraint(const ArgType type, PGM_P (*getNamePgmFn)())
    : type(type),
      range(makeRange(type, INT32_MIN, INT32_MAX)),
      hasRange(false),
      getNamePgmFn(getNamePgmFn) {}

  constexpr ArgConstraint(const ArgType type, double minimum, double maximum, PGM_P (*getNamePgmFn)())
    : type(type),
      range(makeRange(type, minimum, maximum)),
      hasRange(true),
      getNamePgmFn(getNamePgmFn) {}

  ArgType type;
  Range range;
  bool hasRange;
  PGM_P (*getNamePgmFn)();

  PGM_P getNamePgm() const {
    if (getNamePgmFn) {
      return (*getNamePgmFn)();
    } else {
      return getTypeNamePgm(type);
    }
  }

  bool isInRange(Arg arg) {
    if (!hasRange) {
      return true;
    }

    switch (type) {
      case ArgType::Float:
        return arg.getFloat() >= range.floatRange.minimum && arg.getFloat() <= range.floatRange.maximum;
      case ArgType::Int:
        return arg.getInt() >= range.intRange.minimum && arg.getInt() <= range.intRange.maximum;
      default:
        return true;
    }
  }

  Range getRange() {
    if (hasRange) {
      return range;
    }

    return makeRange(type, INT32_MIN, INT32_MAX);
  }

  static PGM_P getTypeNamePgm(ArgType type) {
    static const char _null[] PROGMEM = "null";
    static const char _int[] PROGMEM = "int";
    static const char _float[] PROGMEM = "float";
    static const char _string[] PROGMEM = "string";
    static const char* const types[] PROGMEM{ _null, _int, _float, _string };
    return reinterpret_cast<PGM_P>(pgm_read_ptr(&(types[(int) type])));
  }

  template<char... chars>
  static PGM_P getNamePgm() {
    static const char _name[] PROGMEM = { chars... };
    return _name;
  }

  private:
  static constexpr Range makeRange(ArgType type, int32_t minimum, int32_t maximum) {
    return type == ArgType::Float
      ? Range(FloatRange((float)minimum, (float)maximum))
      : Range(IntRange(minimum, maximum));
  }

  static constexpr Range makeRange(ArgType type, double minimum, double maximum) {
    return type == ArgType::Float
      ? Range(FloatRange((float)minimum, (float)maximum))
      : Range(IntRange((int32_t)minimum, (int32_t)maximum));
  }
};

}

#endif // STATIC_SERIAL_COMMANDS_ARG_H
