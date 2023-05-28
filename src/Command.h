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
  PGM_P (*getCommandPgmFn)(PGM_VOID_P);
  PGM_P (*getDescriptionPgmFn)(PGM_VOID_P);
  const impl::ArgConstraint* (*getArgsPgmFn)(PGM_VOID_P, uint8_t*);
  void (*function)(SerialCommands&, Args&);
  const void* subcommands;
  const uint16_t subcommandsCount;
  const char command[commandLength];
  const char description[descriptionLength];
  const ArgConstraint args[argCount];

  static PGM_P _getCommandPgm(PGM_VOID_P cmdPtr) {
    return ((const Command<commandLength, descriptionLength, argCount>*)cmdPtr)->command;
  }

  static PGM_P _getDescriptionPgm(PGM_VOID_P cmdPtr) {
    return ((const Command<commandLength, descriptionLength, argCount>*)cmdPtr)->description;
  }

  static const impl::ArgConstraint* _getArgsPgm(PGM_VOID_P cmdPtr, uint8_t* count) {
    *count = argCount;
    return ((const Command<commandLength, descriptionLength, argCount>*)cmdPtr)->args;
  }
};
}

class Command {
  public:
    constexpr Command(const void* command)
      : _command((const impl::Command<>*)command), parent(nullptr) {}

    constexpr Command(const void* command, const void* parent)
      : _command((const impl::Command<>*)command), parent((const impl::Command<>*)parent) {}

    PGM_P getCommandPgm() const {
      PGM_P (*_getCommandPgmFn)(PGM_VOID_P) = pgm_read_word(&(_command->getCommandPgmFn));
      return (*_getCommandPgmFn)(_command);
    }

    PGM_P getDescriptionPgm() const {
      PGM_P (*_getDescriptionPgmFn)(PGM_VOID_P) = pgm_read_word(&(_command->getDescriptionPgmFn));
      return (*_getDescriptionPgmFn)(_command);
    }

    const impl::ArgConstraint* getArgsPgm(uint8_t* count) const {
      impl::ArgConstraint* (*_getArgsPgmFn)(PGM_VOID_P, uint8_t*) = pgm_read_word(&(_command->getArgsPgmFn));
      return (*_getArgsPgmFn)(_command, count);
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
