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
  float number1 = args[0].getFloat();
  float number2 = args[1].getFloat();
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

Command commands[] {
  COMMAND(cmd_help, "help", NULL, "list commands"),

  // if string argument contains space it should be inside quotation marks
  // for example: myname "Firstname Lastname"
  COMMAND(cmd_hello, "myname", ArgType::String, NULL, ""),
  COMMAND(cmd_multiply, "mul", ArgType::Float, ArgType::Float, NULL, "multiplies two number"),

  // set min and max value of argument
  // if the argument is out of range command does not run
  COMMAND(cmd_led_on, "on", ARG(ArgType::Int, START_PIN, END_PIN, "pin"), NULL, "turn on the led on the given pin"),
  COMMAND(cmd_led_off, "off", ARG(ArgType::Int, START_PIN, END_PIN, "pin"), NULL, "turn off the led on the given pin"),
};

SerialCommands serialCommands = SERIAL_COMMANDS(Serial, commands);

void setup() {
  Serial.begin(9600);

  for (int i = START_PIN; i <= END_PIN; i++)
    pinMode(i, OUTPUT);

  serialCommands.listCommands();
}

void loop() {
  serialCommands.readSerial();
}
