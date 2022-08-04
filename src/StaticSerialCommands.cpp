/*---------------------------------------------------------------------
Author         : naszly
License        : BSD
Repository     : https://github.com/naszly/Arduino-StaticSerialCommands
-----------------------------------------------------------------------*/

#include "Parse.h"
#include "StaticSerialCommands.h"

void SerialCommands::printCommand(const Command& command) {
  if (command.hasParent()) {
    printCommand(command.getParent());
    serial.print(' ');
  }

  command.getCommand(&buffer);
  serial.print(buffer);

  impl::ArgConstraint argcs[MAX_ARGS];
  impl::ArgConstraint* ptr = argcs;
  uint8_t count = 0;
  command.getArgs(&ptr, &count);
  for (uint16_t j = 0; j < count; ++j) {
    argcs[j].getName(&buffer);
    serial.print(' ');
    serial.print('<');
    serial.print(buffer);
    serial.print('>');
  }
}

void SerialCommands::printCommandDescription(const Command& command) {
  command.getDescription(&buffer);
  serial.print(buffer);
}

void SerialCommands::listCommands(const Command* commands, uint16_t commandsCount) {
  for (uint16_t i = 0; i < commandsCount; ++i) {
    printCommand(commands[i]);
    serial.print(F(" - "));
    printCommandDescription(commands[i]);
    serial.println();
  }
}

void SerialCommands::listAllCommands(const Command* commands, uint16_t commandsCount) {
  const Command* subcmds;
  uint16_t subcmdCount;
  for (uint16_t i = 0; i < commandsCount; ++i) {
    printCommand(commands[i]);
    serial.print(F(" - "));
    printCommandDescription(commands[i]);
    serial.println();

    subcmds = nullptr;
    commands[i].getSubCommands(&subcmds, &subcmdCount);
    if (subcmds != nullptr) {
      commands[i].getCommand(&buffer);

      listAllCommands(subcmds, subcmdCount);
    }
  }
}

void SerialCommands::readSerial() {
  static uint16_t index = 0;

  while (serial.available() > 0) {
    int ch = serial.read();
    if (ch == term1 || ch == term2) {
      if (index > 0) {
        buffer[index] = '\0';
        char* string = strdup(buffer);
        if (string != nullptr) {
          parseCommand(string);
          free(string);
        } else {
          serial.println(F("ERROR: Out of memory"));
        }
        index = 0;
      }
    } else if (index < bufferSize) {
      buffer[index] = ch;
      index++;
    } else {
      serial.println(F("ERROR: Buffer overflow"));
      index = 0;
    }
  }
}

const Command* SerialCommands::findCommand(const char* const string, const Command* commands, uint16_t commandsCount) {
  uint16_t len = strlen(string);
  uint16_t index;
  uint16_t count = 0;
  for (uint16_t i = 0; i < commandsCount; ++i) {
    commands[i].getCommand(&buffer);
    if (memcmp(buffer, string, len + 1) == 0)
      return &commands[i];
    if (memcmp(buffer, string, len) == 0) {
      ++count;
      index = i;
    }
  }
  if (count == 1)
    return &commands[index];
  return nullptr;
}

void SerialCommands::parseCommand(char* string) {
  char* token;
  uint16_t i;
  uint8_t argCount;
  uint16_t argIndex = 0;
  Args args{};
  const Command* cmd = nullptr;
  const Command* cmds = this->commands;
  uint16_t cmdsCount = commandsCount;

  while ((token = getToken(&string)) != nullptr) {
    cmd = findCommand(token, cmds, cmdsCount);
    if (cmd != nullptr) {
      impl::ArgConstraint argcs[MAX_ARGS];
      impl::ArgConstraint* ptr = argcs;
      cmd->getArgs(&ptr, &argCount);
      for (i = 0; i < argCount; ++i) {
        if ((token = getToken(&string)) != nullptr) {
          if (!getArg(args[argIndex], token, argcs[i])) {
            serial.print(F("ERROR: Can't parse argument "));
            serial.println(argIndex + 1);
            printCommand(*cmd);
            serial.println();
            return;
          }
          
          if (!argcs[i].isInRange(args[argIndex])) {
            serial.print(F("ERROR: Argument out of range "));
            serial.print(argIndex + 1);
            impl::Range range = argcs[i].getRange();
            serial.print(F(" ("));
            serial.print(range.minimum);
            serial.print(F(" - "));
            serial.print(range.maximum);
            serial.println(')');
            printCommand(*cmd);
            serial.println();
            return;
          }
          argIndex++;
        } else {
          serial.println(F("ERROR: Not enough arguments"));
          printCommand(*cmd);
          serial.println();
          return;
        }
      }
      cmd->getSubCommands(&cmds, &cmdsCount);

      if (cmds == nullptr) {
        if ((token = getToken(&string)) != nullptr) {
          serial.println(F("ERROR: Too many arguments"));
          printCommand(*cmd);
          serial.println();
          return;
        }
        cmd->runCommand(*this, args);
      }

    } else {
      serial.print(F("ERROR: Command does not exist \""));
      serial.print(token);
      serial.println('"');
      return;
    }
  }
}

char* SerialCommands::getToken(char** stringp) {
  char *begin, *end;
  begin = *stringp;
  if (begin == nullptr)
    return nullptr;

  while (*begin == delim) begin++;

  if (*begin == quotation) {
    begin++;
    end = begin;
    while (*end != quotation && *end != '\0') end++;

    if (*end == quotation) {
      *end = '\0';
      end++;
    }
  } else {
    end = begin;
    while (*end != delim && *end != '\0') end++;
  }

  while (*end == delim) {
    *end = '\0';
    end++;
  }

  if (*end != '\0')
    *stringp = end;
  else
    *stringp = nullptr;

  return begin;
}

bool SerialCommands::getArg(Arg& out, const char* string, const impl::ArgConstraint& arg) {
  switch (arg.type) {
    case ArgType::String:
      {
        out = Arg(string);
        return true;
      }
    case ArgType::Int:
      {
        int32_t value;
        if (parse::strtoi(string, &value)) {
          out = Arg(value);
          return true;
        }
      }
      break;
    case ArgType::Float:
      {
        float value_f;
        if (parse::strtof(string, &value_f)) {
          out = Arg(value_f);
          return true;
        }
      }
      break;
    default:
      break;
  }
  return false;
}
