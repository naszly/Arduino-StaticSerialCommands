/*---------------------------------------------------------------------
Author         : naszly
License        : BSD
Repository     : https://github.com/naszly/Arduino-StaticSerialCommands
-----------------------------------------------------------------------*/

#include <StaticSerialCommands.h>

#define START_PIN 2
#define END_PIN 13

void cmd_help(SerialCommands& sender, Args& args) {
  sender.listCommands();
}

void cmd_hello(SerialCommands& sender, Args& args) {
  sender.getSerial().print(F("Hello "));
  sender.getSerial().println(args[0].getString());
}

void cmd_multiply(SerialCommands& sender, Args& args) {
  auto number1 = args[0].getFloat();
  auto number2 = args[1].getFloat();
  sender.getSerial().println(number1 * number2);
}

void cmd_led_on(SerialCommands& sender, Args& args) {
  int pin = args[0].getInt();
  digitalWrite(pin, HIGH);
  sender.getSerial().print(F("Led "));
  sender.getSerial().print(pin);
  sender.getSerial().println(F(" is on "));
}

void cmd_led_off(SerialCommands& sender, Args& args) {
  int pin = args[0].getInt();
  digitalWrite(pin, LOW);
  sender.getSerial().print(F("Led "));
  sender.getSerial().print(pin);
  sender.getSerial().println(F(" is off "));
}

/*
COMMAND macro is used to create Command object.
It takes the following arguments:
    COMMAND(function, command)
    COMMAND(function, command, subcommands)
    COMMAND(function, command, subcommands, description)
    COMMAND(function, command, arguments..., subcommands, description)

ARG macro is used to specify argument type, range (if type is numeric) and name.
It takes the following arguments:
    ARG(type)
    ARG(type, name)
    ARG(type, min, max)
    ARG(type, min, max, name)
*/

Command commands[] {
  COMMAND(cmd_help, "help", NULL, "list commands"),

  // if string argument contains space it should be inside quotation marks
  // for example: name "Firstname Lastname"
  COMMAND(cmd_hello, "name", ArgType::String, NULL, ""),
  COMMAND(cmd_multiply, "mul", ArgType::Float, ArgType::Float, NULL, "multiply two numbers"),

  // argument should be an integer between 2 and 13
  // otherwise error message will be printed and command won't be executed
  COMMAND(cmd_led_on, "on", ARG(ArgType::Int, START_PIN, END_PIN, "pin"), NULL, "turn on the led on the given pin"),
  COMMAND(cmd_led_off, "off", ARG(ArgType::Int, START_PIN, END_PIN, "pin"), NULL, "turn off the led on the given pin"),
};

SerialCommands serialCommands(Serial, commands, sizeof(commands) / sizeof(Command));

// if default buffer size (64) is too small pass a buffer through constructor
// char buffer[128];
// SerialCommands serialCommands(Serial, commands, sizeof(commands) / sizeof(Command), buffer, sizeof(buffer));

void setup() {
  Serial.begin(9600);

  for (int i = START_PIN; i <= END_PIN; i++)
    pinMode(i, OUTPUT);

  // customize delimiter, termination and quotation characters
  // serialCommands.setDelimiterChars<' ', '\t'>();
  // serialCommands.setTerminationChars<'\n', '\r', ';'>();
  // serialCommands.setQuotationChars<'"', '\''>();

  serialCommands.listCommands();
}

void loop() {
  serialCommands.readSerial();
}
