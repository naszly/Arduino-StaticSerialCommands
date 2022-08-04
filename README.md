# StaticSerialCommands

An Arduino library for parsing commands received over a serial port. Optimized for low dynamic memory usage, commands are stored in program memory. Typed arguments with strict input validation and friendly error messages. Commands can have subcommands.

## Quickstart

```cpp
#include "StaticSerialCommands.h"

void cmd_help(SerialCommands& sender, Args& args) {
  sender.listCommands();
}

Command commands[] {
    COMMAND(cmd_help, "help"),
};

SerialCommands serialCommands(Serial, commands, sizeof(commands) / sizeof(Command));

void setup() {
  Serial.begin(9600);
}

void loop() {
  serialCommands.readSerial();
}
```

## Commands

COMMAND macro syntax:
```cpp
COMMAND(function, command)
COMMAND(function, command, subcommands)
COMMAND(function, command, subcommands, description)
COMMAND(function, command, arguments..., subcommands, description)
```
### Simple arguments
Valid argument types: Int, Float, String
```cpp
void cmd_hello(SerialCommands& sender, Args& args) {
  sender.getSerial().print(F("Hello "));
  sender.getSerial().println(args[0].getString());
}

void cmd_multiply(SerialCommands& sender, Args& args) {
  auto number1 = args[0].getInt();
  auto number2 = args[1].getInt();
  sender.getSerial().println(number1 * number2);
}

Command commands[] {
  // if string argument contains space it should be inside quotation marks
  // for example: name "Firstname Lastname"
  COMMAND(cmd_hello, "name", ArgType::String, nullptr, ""),
  COMMAND(cmd_multiply, "mul", ArgType::Int, ArgType::Int, nullptr, "multiplies two number"),
};
```

### Custom arguments
ARG macro syntax:
```cpp
ARG(type)
ARG(type, name)
ARG(type, min, max)
ARG(type, min, max, name)
```

```cpp
void cmd_led_on(SerialCommands& sender, Args& args) {
  auto pin = args[0].getInt();
  digitalWrite(pin, HIGH);
  sender.getSerial().print(F("Led "));
  sender.getSerial().print(pin);
  sender.getSerial().println(F(" is on "));
}

void cmd_led_off(SerialCommands& sender, Args& args) {
  auto pin = args[0].getInt();
  digitalWrite(pin, LOW);
  sender.getSerial().print(F("Led "));
  sender.getSerial().print(pin);
  sender.getSerial().println(F(" is off "));
}

Command commands[] {
  // argument should be an integer between 2 and 13
  // otherwise error message will be printed and command won't be executed
  COMMAND(cmd_led_on, "on", ARG(ArgType::Int, 2, 13, "pin"), nullptr, "turn on the led on the given pin"),
  COMMAND(cmd_led_off, "off", ARG(ArgType::Int, 2, 13, "pin"), nullptr, "turn off the led on the given pin"),
};
```

### Subcommands
```
help - list commands
calc <int> - calculator
calc <int> + <int> - add numbers
calc <int> * <int> - multiply numbers
```

```cpp
void cmd_help(SerialCommands& sender, Args& args);
void cmd_calc(SerialCommands& sender, Args& args);
void cmd_calc_add(SerialCommands& sender, Args& args);
void cmd_calc_mul(SerialCommands& sender, Args& args);

Command subCommands[] {
  COMMAND(cmd_calc_add, "+", ArgType::Int, nullptr, "add numbers"),
  COMMAND(cmd_calc_mul, "*", ArgType::Int, nullptr, "multiply numbers"),
};

Command commands[] {
  COMMAND(cmd_help, "help", nullptr, "list commands"),
  COMMAND(cmd_calc, "calc", ArgType::Int, subCommands, "calculator"),
};

void cmd_help(SerialCommands& sender, Args& args) {
  sender.listAllCommands();
}

void cmd_calc(SerialCommands& sender, Args& args) {
  sender.listAllCommands(subCommands, sizeof(subCommands) / sizeof(Command));
}

void cmd_calc_add(SerialCommands& sender, Args& args) {
  float number1 = args[0].getInt();
  float number2 = args[1].getInt();
  sender.getSerial().println(number1 + number2);
}

void cmd_calc_mul(SerialCommands& sender, Args& args) {
  float number1 = args[0].getInt();
  float number2 = args[1].getInt();
  sender.getSerial().println(number1 * number2);
}
```
