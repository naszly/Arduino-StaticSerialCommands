/*---------------------------------------------------------------------
Author         : naszly
License        : BSD
Repository     : https://github.com/naszly/Arduino-StaticSerialCommands
-----------------------------------------------------------------------*/

#ifndef STATIC_SERIAL_COMMANDS_H
#define STATIC_SERIAL_COMMANDS_H

#include <Arduino.h>
#include "Command.h"

#define SERIAL_COMMANDS(serial, commands) SerialCommands(serial, commands, sizeof(commands) / sizeof(Command))

#define CMD_DELIM ' '
#define CMD_QUOTATION '"'
#define CMD_TERM_1 '\n'
#define CMD_TERM_2 '\r'

class SerialCommands {
  public:
    typedef bool (*CharPredicate)(char);

    SerialCommands(Stream& serial, const Command* commands, uint16_t commandsCount, char* buffer, uint16_t bufferSize, uint16_t timeout = 0)
      : serial(serial), buffer(buffer), bufferSize(bufferSize),
        commands(commands), commandsCount(commandsCount),
        timeout(timeout) {}

    SerialCommands(Stream& serial, const Command* commands, uint16_t commandsCount)
      : serial(serial),
        commands(commands), commandsCount(commandsCount), timeout(0) {
      static char buffer[64];
      this->buffer = buffer;
      bufferSize = sizeof(buffer);
    }

    void printCommand(const Command& command);
    void printCommandDescription(const Command& command);

    void listCommands(const Command* commands, uint16_t commandsCount);

    void listAllCommands(const Command* commands, uint16_t commandsCount);

    void listCommands() {
      listCommands(commands, commandsCount);
    }

    void listAllCommands() {
      listAllCommands(commands, commandsCount);
    }

    void readSerial();

    Stream& getSerial() {
      return serial;
    }

    void setDelimiterPredicate(CharPredicate predicate) {
      isDelim = predicate;
    }

    void setQuotationPredicate(CharPredicate predicate) {
      isQuotation = predicate;
    }

    void setTerminationPredicate(CharPredicate predicate) {
      isTerm = predicate;
    }

    template<char... chars>
    void setDelimiterChars() {
      isDelim = anyChar<chars...>;
    }

    template<char... chars>
    void setQuotationChars() {
      isQuotation = anyChar<chars...>;
    }

    template<char... chars>
    void setTerminationChars() {
      isTerm = anyChar<chars...>;
    }

  private:
    Stream& serial;
    char* buffer;
    uint16_t bufferSize;
    const Command* commands;
    const uint16_t commandsCount;
    const uint16_t timeout;

    CharPredicate isDelim = [](char c) { return c == CMD_DELIM; };
    CharPredicate isQuotation = [](char c) { return c == CMD_QUOTATION; };
    CharPredicate isTerm = [](char c) { return c == CMD_TERM_1 || c == CMD_TERM_2; };

    const Command* findCommand(const char* const string, const Command* commands, uint16_t commandsCount);
    void parseCommand(char* string);

    char* getToken(char** stringp);

    bool getArg(Arg& out, const char* string, const impl::ArgConstraint& arg);

    void printFromPgm(PGM_P str);

    template<char... chars>
    static bool anyChar(char c) {
      //return ((c == chars) || ...);
      char arr[] = { chars... };
      for (auto ch : arr) {
        if (c == ch) return true;
      }
      return false;
    }
};

#endif // STATIC_SERIAL_COMMANDS_H
