/*---------------------------------------------------------------------
Author         : naszly
License        : BSD
Repository     : https://github.com/naszly/Arduino-StaticSerialCommands
-----------------------------------------------------------------------*/

#ifndef STATIC_SERIAL_COMMANDS_COMMAND_H
#define STATIC_SERIAL_COMMANDS_COMMAND_H

#include <Arduino.h>
#include "CommandBuilder.h"
#include "Arg.h"

class SerialCommands;

namespace impl {
  
template<uint8_t commandLength = 0, uint16_t descriptionLength = 0, uint8_t argCount = 0>
struct Command {
  void (*getCommand)(const void*, char**);
  void (*getDescription)(const void*, char**);
  void (*getArgs)(const void*, ArgConstraint**, uint8_t*);
  void (*function)(SerialCommands&, Args&);
  const void* subcommands;
  const uint16_t subcommandsCount;
  const char command[commandLength];
  const char description[descriptionLength];
  const ArgConstraint args[argCount];

  static void _getCommand(const void* cmdPtr, char** buffer) {
    memcpy_P(
      *buffer,
      ((const Command<commandLength, descriptionLength, argCount>*)cmdPtr)->command,
      commandLength);
  }

  static void _getDescription(const void* cmdPtr, char** buffer) {
    memcpy_P(
      *buffer,
      ((const Command<commandLength, descriptionLength, argCount>*)cmdPtr)->description,
      descriptionLength);
  }

  static void _getArgs(const void* cmdPtr, ArgConstraint** args, uint8_t* count) {
    memcpy_P(
      *args,
      ((const Command<commandLength, descriptionLength, argCount>*)cmdPtr)->args,
      argCount * sizeof(ArgConstraint));
    *count = argCount;
  }
};
}

class Command {
  public:
    constexpr Command(const void* command)
      : _command((const impl::Command<>*)command), parent(nullptr) {}

    constexpr Command(const void* command, const void* parent)
      : _command((const impl::Command<>*)command), parent((const impl::Command<>*)parent) {}

    void getCommand(char** buffer) const {
      void (*_getCommand)(const void*, char**) = pgm_read_word(&(_command->getCommand));
      (*_getCommand)(_command, buffer);
    }

    void getDescription(char** buffer) const {
      void (*_getDescription)(const void*, char**) = pgm_read_word(&(_command->getDescription));
      (*_getDescription)(_command, buffer);
    }

    void getArgs(impl::ArgConstraint** args, uint8_t* count) const {
      void (*_getArgs)(const void*, impl::ArgConstraint**, uint8_t*) = pgm_read_word(&(_command->getArgs));
      (*_getArgs)(_command, args, count);
    }

    void runCommand(SerialCommands &sender, Args &args) const {
      void (*function)(SerialCommands&, Args&) = pgm_read_word(&(_command->function));
      if (function) {
        (*function)(sender, args);
      }
    }

    void getSubCommands(const Command** commands, uint16_t* count) const {
      *commands = pgm_read_word(&(_command->subcommands));
      *count = pgm_read_word(&(_command->subcommandsCount));
    }

    const Command getParent() const {
      return Command(parent);
    }

    bool hasParent() const {
      return parent != nullptr;
    }

    const impl::Command<>* get() {
      return _command;
    }

  private:
    const impl::Command<>* _command;
    const impl::Command<>* parent;
};

#endif // STATIC_SERIAL_COMMANDS_COMMAND_H
