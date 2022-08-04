/*---------------------------------------------------------------------
Author         : naszly
License        : BSD
Repository     : https://github.com/naszly/Arduino-StaticSerialCommands
-----------------------------------------------------------------------*/

#include <StaticSerialCommands.h>

#define LED_PIN 13

void cmd_led_on(SerialCommands& sender, Args& args) {
  digitalWrite(LED_PIN, HIGH);
  sender.getSerial().println(F("Led is on"));
}

void cmd_led_off(SerialCommands& sender, Args& args) {
  digitalWrite(LED_PIN, LOW);
  sender.getSerial().println(F("Led is off"));
}

Command commands[] {
  COMMAND(cmd_led_on, "on"),
  COMMAND(cmd_led_off, "off"),
};

SerialCommands serialCommands = SERIAL_COMMANDS(Serial, commands);

void setup() {
  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  serialCommands.readSerial();
}
