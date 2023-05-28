# StaticSerialCommands
An Arduino library that allows you to parse commands received over a serial port. It is optimized for low dynamic memory usage by storing commands in program memory.
* Typed arguments with strict input validation. Valid argument types: Int, Float, String.
* Friendly error messages for invalid input.
* Customizable delimiter character, default is `SPACE`.
* Customizable terminator character, default is `NL` & `CR`.
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
public methods of SerialCommands class:
```cpp
// read serial port and parse command when new line is received
// if parsing is successful, command function will be called
// if parsing is unsuccessful, error message will be printed
void readSerial();

// get Serial object
Stream& getSerial();

// print the command syntax
void printCommand(const Command& command);

// print command description
void printCommandDescription(const Command& command);

// list all commands but not their subcommands
void listCommands();
void listCommands(const Command* commands, uint16_t commandsCount);

// list all commands and their subcommands
void listAllCommands();
void listAllCommands(const Command* commands, uint16_t commandsCount);

// set delimiter characters
// usage: setDelimiterChars<' ', '\t'>();
template<char... chars>
void setDelimiterChars();

// set termination characters
// usage: setTerminationChars<'\r', '\n', ';'>();
template<char... chars>
void setTerminationChars();

// set quotation characters
// usage: setQuotationChars<'"'>();
template<char... chars>
void setQuotationChars();

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