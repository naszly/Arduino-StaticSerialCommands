/*---------------------------------------------------------------------
Author         : naszly
License        : BSD
Repository     : https://github.com/naszly/Arduino-StaticSerialCommands
-----------------------------------------------------------------------*/

#include <StaticSerialCommands.h>

void cmd_help(SerialCommands& sender, Args& args);
void cmd_calc(SerialCommands& sender, Args& args);
void cmd_calc_add(SerialCommands& sender, Args& args);
void cmd_calc_mul(SerialCommands& sender, Args& args);

/*
COMMAND macro is used to create Command object.
It takes the following arguments:
    COMMAND(function, command)
    COMMAND(function, command, subcommands)
    COMMAND(function, command, subcommands, description)
    COMMAND(function, command, arguments..., subcommands, description)
*/

Command subCommands[] {
        COMMAND(cmd_calc_add, "+", ArgType::Int, nullptr, "add numbers"),
        COMMAND(cmd_calc_mul, "*", ArgType::Int, nullptr, "multiply numbers"),
};

Command commands[] {
        COMMAND(cmd_help, "help", nullptr, "list commands"),
        COMMAND(cmd_calc, "calc", ArgType::Int, subCommands, "calculator"),
};

SerialCommands serialCommands(Serial, commands, sizeof(commands) / sizeof(Command));

// if default buffer size (64) is too small pass a buffer through constructor
// char buffer[128];
// SerialCommands serialCommands(Serial, commands, sizeof(commands) / sizeof(Command), buffer, sizeof(buffer));

void setup() {
    Serial.begin(9600);

    // customize delimiter, termination and quotation characters
    // serialCommands.setDelimiterChars<' ', '\t'>();
    // serialCommands.setTerminationChars<'\n', '\r', ';'>();
    // serialCommands.setQuotationChars<'"', '\''>();

    serialCommands.listAllCommands();
}

void loop() {
    serialCommands.readSerial();
}

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