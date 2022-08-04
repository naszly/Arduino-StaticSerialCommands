/*---------------------------------------------------------------------
Author         : naszly
License        : BSD
Repository     : https://github.com/naszly/Arduino-StaticSerialCommands
-----------------------------------------------------------------------*/

#ifndef COMMAND_BUILDER_H
#define COMMAND_BUILDER_H

#define _COMMAND(function, command, subcommands, description, argsCount, ...) \
  impl::Command<sizeof(command), sizeof(description), argsCount> { \
    impl::Command<sizeof(command), sizeof(description), argsCount>::_getCommand, \
    impl::Command<sizeof(command), sizeof(description), argsCount>::_getDescription, \
    impl::Command<sizeof(command), sizeof(description), argsCount>::_getArgs, \
    function, \
    subcommands, \
    sizeof(subcommands) / sizeof(Command), \
    command, \
    description, \
    { __VA_ARGS__ } \
  }

#define _LINK_SUB_CMDS(cmd, subcommands) \
  if (subcommands != nullptr) { \
    for (uint16_t i = 0; i < sizeof(subcommands) / sizeof(Command); ++i) { \
      Command* ptr = (Command*)subcommands + i; \
      *ptr = Command(ptr->get(), &cmd); \
    } \
  }

#define COMMAND_2(function, command) []() { \
    const static PROGMEM auto cmd = _COMMAND(function, command, nullptr, "", 0); \
    return Command(&cmd); }()

#define COMMAND_3(function, command, subcommands) []() { \
    const static PROGMEM auto cmd = _COMMAND(function, command, subcommands, "", 0); \
    _LINK_SUB_CMDS(cmd, subcommands); \
    return Command(&cmd); }()

#define COMMAND_4(function, command, subcommands, description) []() { \
    const static PROGMEM auto cmd = _COMMAND(function, command, subcommands, description, 0); \
    _LINK_SUB_CMDS(cmd, subcommands); \
    return Command(&cmd); }()

#define COMMAND_5(function, command, arg, subcommands, description) []() { \
    const static PROGMEM auto cmd = _COMMAND(function, command, subcommands, description, 1, arg); \
    _LINK_SUB_CMDS(cmd, subcommands); \
    return Command(&cmd); }()

#define COMMAND_6(function, command, arg1, arg2, subcommands, description) []() { \
    const static PROGMEM auto cmd = _COMMAND(function, command, subcommands, description, 2, arg1, arg2); \
    _LINK_SUB_CMDS(cmd, subcommands); \
    return Command(&cmd); }()

#define COMMAND_7(function, command, arg1, arg2, arg3, subcommands, description) []() { \
    const static PROGMEM auto cmd = _COMMAND(function, command, subcommands, description, 3, arg1, arg2, arg3); \
    _LINK_SUB_CMDS(cmd, subcommands); \
    return Command(&cmd); }()

#define COMMAND_8(function, command, arg1, arg2, arg3, arg4, subcommands, description) []() { \
    const static PROGMEM auto cmd = _COMMAND(function, command, subcommands, description, 4, arg1, arg2, arg3, arg4); \
    _LINK_SUB_CMDS(cmd, subcommands); \
    return Command(&cmd); }()

#define COMMAND_9(function, command, arg1, arg2, arg3, arg4, arg5, subcommands, description) []() { \
    const static PROGMEM auto cmd = _COMMAND(function, command, subcommands, description, 5, arg1, arg2, arg3, arg4, arg5); \
    _LINK_SUB_CMDS(cmd, subcommands); \
    return Command(&cmd); }()

#define COMMAND_10(function, command, arg1, arg2, arg3, arg4, arg5, arg6, subcommands, description) []() { \
    const static PROGMEM auto cmd = _COMMAND(function, command, subcommands, description, 6, arg1, arg2, arg3, arg4, arg5, arg6); \
    _LINK_SUB_CMDS(cmd, subcommands); \
    return Command(&cmd); }()

#define COMMAND_11(function, command, arg1, arg2, arg3, arg4, arg5, arg6, arg7, subcommands, description) []() { \
    const static PROGMEM auto cmd = _COMMAND(function, command, subcommands, description, 7, arg1, arg2, arg3, arg4, arg5, arg6, arg7); \
    _LINK_SUB_CMDS(cmd, subcommands); \
    return Command(&cmd); }()

#define COMMAND_12(function, command, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, subcommands, description) []() { \
    const static PROGMEM auto cmd = _COMMAND(function, command, subcommands, description, 8, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8); \
    _LINK_SUB_CMDS(cmd, subcommands); \
    return Command(&cmd); }()

#define COMMAND_13(function, command, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, subcommands, description) []() { \
    const static PROGMEM auto cmd = _COMMAND(function, command, subcommands, description, 9, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9); \
    _LINK_SUB_CMDS(cmd, subcommands); \
    return Command(&cmd); }()

#define COMMAND_14(function, command, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, subcommands, description) []() { \
    const static PROGMEM auto cmd = _COMMAND(function, command, subcommands, description, 10, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10); \
    _LINK_SUB_CMDS(cmd, subcommands); \
    return Command(&cmd); }()

#define COMMAND_15(function, command, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, subcommands, description) []() { \
    const static PROGMEM auto cmd = _COMMAND(function, command, subcommands, description, 11, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11); \
    _LINK_SUB_CMDS(cmd, subcommands); \
    return Command(&cmd); }()

#define COMMAND_16(function, command, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, subcommands, description) []() { \
    const static PROGMEM auto cmd = _COMMAND(function, command, subcommands, description, 12, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12); \
    _LINK_SUB_CMDS(cmd, subcommands); \
    return Command(&cmd); }()

#define COMMAND_17(function, command, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, subcommands, description) []() { \
    const static PROGMEM auto cmd = _COMMAND(function, command, subcommands, description, 13, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13); \
    _LINK_SUB_CMDS(cmd, subcommands); \
    return Command(&cmd); }()

#define COMMAND_18(function, command, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, subcommands, description) []() { \
    const static PROGMEM auto cmd = _COMMAND(function, command, subcommands, description, 14, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14); \
    _LINK_SUB_CMDS(cmd, subcommands); \
    return Command(&cmd); }()

#define COMMAND_19(function, command, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, subcommands, description) []() { \
    const static PROGMEM auto cmd = _COMMAND(function, command, subcommands, description, 15, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15); \
    _LINK_SUB_CMDS(cmd, subcommands); \
    return Command(&cmd); }()

#define COMMAND_20(function, command, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16, subcommands, description) []() { \
    const static PROGMEM auto cmd = _COMMAND(function, command, subcommands, description, 16, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16); \
    _LINK_SUB_CMDS(cmd, subcommands); \
    return Command(&cmd); }()

#define COMMAND_X(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, FUNC, ...)  FUNC

#define COMMAND(...) \
  COMMAND_X(, ##__VA_ARGS__, \
              COMMAND_20(__VA_ARGS__), \
              COMMAND_19(__VA_ARGS__), \
              COMMAND_18(__VA_ARGS__), \
              COMMAND_17(__VA_ARGS__), \
              COMMAND_16(__VA_ARGS__), \
              COMMAND_15(__VA_ARGS__), \
              COMMAND_14(__VA_ARGS__), \
              COMMAND_13(__VA_ARGS__), \
              COMMAND_12(__VA_ARGS__), \
              COMMAND_11(__VA_ARGS__), \
              COMMAND_10(__VA_ARGS__), \
              COMMAND_9(__VA_ARGS__), \
              COMMAND_8(__VA_ARGS__), \
              COMMAND_7(__VA_ARGS__), \
              COMMAND_6(__VA_ARGS__), \
              COMMAND_5(__VA_ARGS__), \
              COMMAND_4(__VA_ARGS__), \
              COMMAND_3(__VA_ARGS__), \
              COMMAND_2(__VA_ARGS__) \
             )
#endif
