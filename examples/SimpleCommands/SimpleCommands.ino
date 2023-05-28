/*---------------------------------------------------------------------
Author         : naszly
License        : BSD
Repository     : https://github.com/naszly/Arduino-StaticSerialCommands
-----------------------------------------------------------------------*/

#include <StaticSerialCommands.h>

#define LED_PIN 13

void cmd_help(SerialCommands& sender, Args& args) {
    sender.listCommands();
}

void cmd_led_on(SerialCommands& sender, Args& args) {
  digitalWrite(LED_PIN, HIGH);
  sender.getSerial().println(F("Led is on"));
}

void cmd_led_off(SerialCommands& sender, Args& args) {
  digitalWrite(LED_PIN, LOW);
  sender.getSerial().println(F("Led is off"));
}

Command commands[] {
  COMMAND(cmd_help, "help"),
  COMMAND(cmd_led_on, "on"),
  COMMAND(cmd_led_off, "off"),
};

SerialCommands serialCommands(Serial, commands, sizeof(commands) / sizeof(Command));

// if default buffer size (64) is too small pass a buffer through constructor
// char buffer[128];
// SerialCommands serialCommands(Serial, commands, sizeof(commands) / sizeof(Command), buffer, sizeof(buffer));

void setup() {
  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);

  // customize delimiter, termination and quotation characters
  // serialCommands.setDelimiterChars<' ', '\t'>();
  // serialCommands.setTerminationChars<'\n', '\r', ';'>();
  // serialCommands.setQuotationChars<'"', '\''>();

  serialCommands.listCommands();
}

void loop() {
  serialCommands.readSerial();
}
