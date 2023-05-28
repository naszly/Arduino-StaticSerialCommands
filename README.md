# StaticSerialCommands
An Arduino library that allows you to parse commands received over a serial port. It is optimized for low dynamic memory usage by storing commands in program memory.
* Typed arguments with strict input validation. Valid argument types: Int, Float, String.
* Friendly error messages for invalid input.
* Customizable delimiter character(s), default is `SPACE`.
* Customizable termination character(s), default is `NL` & `CR`.
* Quotation marks can be used to escape delimiter character. Customizable, default is `"` (double quote).
* Commands can have subcommands.
* Methods to list commands with syntax and description.

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
COMMAND macro is used to create Command object with data stored in program memory.
```cpp
COMMAND(function, command)
COMMAND(function, command, subcommands)
COMMAND(function, command, subcommands, description)
COMMAND(function, command, arguments..., subcommands, description)
```
### Simple arguments
Valid argument types: Int, Float, String \
Maximum number of arguments is limited to 16.
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
  COMMAND(cmd_multiply, "mul", ArgType::Int, ArgType::Int, nullptr, "multiply two numbers"),
};
```
### Custom arguments
ARG macro is used to specify command argument type, range (if type is numeric) and name.
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
If you want to declare argument constraint into a variable, mark it as `constexpr`:
```cpp 
constexpr auto argConstraint = ARG(ArgType::Int, 2, 13, "pin");

Command commands[] {
  COMMAND(cmd_led_on, "on", argConstraint, nullptr, "turn on the led on the given pin"),
  COMMAND(cmd_led_off, "off", argConstraint, nullptr, "turn off the led on the given pin"),
};
```
### Subcommands
An array of subcommands can only be passed to one command. \
The sum of command arguments and subcommand arguments must be less than or equal to 16. \
If the parent of subcommands has N arguments, the first N arguments of the subcommands will be the parent's arguments.
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
  auto number1 = args[0].getInt();
  auto number2 = args[1].getInt();
  sender.getSerial().println(number1 + number2);
}

void cmd_calc_mul(SerialCommands& sender, Args& args) {
  auto number1 = args[0].getInt();
  auto number2 = args[1].getInt();
  sender.getSerial().println(number1 * number2);
}
```
Result:
```
help - list commands
calc <int> - calculator
calc <int> + <int> - add numbers
calc <int> * <int> - multiply numbers
```
## SerialCommands methods
Public methods of SerialCommands class:

Read serial port and parse command when new line is received \
If parsing is successful, command function will be called \
If parsing is unsuccessful, error message will be printed
```cpp
void readSerial();
```
\
Get Serial object
```cpp
Stream& getSerial();
```
\
Print the command syntax
```cpp
void printCommand(const Command& command);
```
\
Print command description
```cpp
void printCommandDescription(const Command& command);
```
\
List all commands but not their subcommands
```cpp
void listCommands();
void listCommands(const Command* commands, uint16_t commandsCount);
```
\
List all commands and their subcommands
```cpp
void listAllCommands();
void listAllCommands(const Command* commands, uint16_t commandsCount);
```
\
Set delimiter characters
```cpp
template<char... chars>
void setDelimiterChars();
```
Usage:
```cpp
setDelimiterChars<' '>(); // default delimiter is space
setDelimiterChars<' ', '\t', '\r', '\n'>(); // space, tab, carriage return and new line
```
\
Set termination characters
```cpp
template<char... chars>
void setTerminationChars();
```
Usage:
```cpp
setTerminationChars<'\r', '\n'>(); // default termination characters are carriage return and new line
setTerminationChars<'\r', '\n', ';'>(); // carriage return, new line and semicolon
```
\
Set quotation characters
```cpp
template<char... chars>
void setQuotationChars();
```
Usage:
```cpp
setQuotationChars<'"'>(); // default quotation character is double quote
setQuotationChars<'"', '\''>(); // double quote and single quote
```
## Custom buffer size
Default buffer size is 64 bytes. \
The buffer should be large enough to receive the longest command
```cpp
char buffer[128];
SerialCommands serialCommands(
  Serial,
  commands, sizeof(commands) / sizeof(Command),
  buffer, sizeof(buffer)
);
```

## Timeout

If the command is not received within the specified time, the buffer will be cleared and the command will be ignored. \
By default, timeout is disabled. \
To enable timeout, pass timeout value in milliseconds to the constructor.
```cpp
char buffer[64];
SerialCommands serialCommands(
  Serial,
  commands, sizeof(commands) / sizeof(Command),
  buffer, sizeof(buffer),
  1000 // 1 second
);
```