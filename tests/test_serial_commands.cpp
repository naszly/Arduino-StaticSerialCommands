#include <cstdlib>
#include <iostream>
#include <string>

#include "StaticSerialCommands.h"

uint64_t __test_millis = 0;

class FakeStream : public Stream {
  public:
    void feed(const std::string& data) {
      input += data;
    }

    std::string takeOutput() {
      std::string out = output;
      output.clear();
      return out;
    }

    int available() override {
      return static_cast<int>(input.size() - readPos);
    }

    int read() override {
      if (readPos >= input.size()) {
        return -1;
      }
      return static_cast<unsigned char>(input[readPos++]);
    }

    std::size_t write(uint8_t c) override {
      output.push_back(static_cast<char>(c));
      return 1;
    }

  private:
    std::string input;
    std::size_t readPos = 0;
    std::string output;
};

static bool contains(const std::string& haystack, const std::string& needle) {
  return haystack.find(needle) != std::string::npos;
}

static int failures = 0;

#define ASSERT_TRUE(cond) do { \
  if (!(cond)) { \
    std::cerr << "Assertion failed at line " << __LINE__ << ": " #cond << std::endl; \
    ++failures; \
    return; \
  } \
} while (0)

#define ASSERT_EQ(actual, expected) do { \
  if (!((actual) == (expected))) { \
    std::cerr << "Assertion failed at line " << __LINE__ << ": " #actual " == " #expected << std::endl; \
    std::cerr << "Actual: " << (actual) << "\nExpected: " << (expected) << std::endl; \
    ++failures; \
    return; \
  } \
} while (0)

static int g_value = 0;
static std::string g_text;
static int g_sum = 0;

void cmd_ping(SerialCommands&, Args&) {
  g_value = 1;
}

void cmd_set_int(SerialCommands&, Args& args) {
  g_value = args[0].getInt();
}

void cmd_set_name(SerialCommands&, Args& args) {
  g_text = args[0].getString();
}

void cmd_calc(SerialCommands&, Args&) {}

void cmd_calc_add(SerialCommands&, Args& args) {
  g_sum = args[0].getInt() + args[1].getInt();
}

Command g_calc_subcommands[] = {
  COMMAND(cmd_calc_add, "+", ArgType::Int, nullptr, ""),
};

Command g_calc_commands[] = {
  COMMAND(cmd_calc, "calc", ArgType::Int, g_calc_subcommands, ""),
};

void test_runs_command_without_args() {
  g_value = 0;
  FakeStream stream;
  Command commands[] = {
    COMMAND(cmd_ping, "ping"),
  };
  SerialCommands serial(stream, commands, sizeof(commands) / sizeof(Command));

  stream.feed("ping\n");
  serial.readSerial();

  ASSERT_EQ(g_value, 1);
  ASSERT_EQ(stream.takeOutput(), "");
}

void test_parses_integer_argument() {
  g_value = 0;
  FakeStream stream;
  Command commands[] = {
    COMMAND(cmd_set_int, "set", ArgType::Int, nullptr, ""),
  };
  SerialCommands serial(stream, commands, sizeof(commands) / sizeof(Command));

  stream.feed("set 42\n");
  serial.readSerial();

  ASSERT_EQ(g_value, 42);
}

void test_reports_unknown_command() {
  FakeStream stream;
  Command commands[] = {
    COMMAND(cmd_ping, "ping"),
  };
  SerialCommands serial(stream, commands, sizeof(commands) / sizeof(Command));

  stream.feed("pong\n");
  serial.readSerial();

  const std::string out = stream.takeOutput();
  ASSERT_TRUE(contains(out, "ERROR: Command does not exist \"pong\""));
}

void test_reports_not_enough_args() {
  FakeStream stream;
  Command commands[] = {
    COMMAND(cmd_set_int, "set", ArgType::Int, nullptr, ""),
  };
  SerialCommands serial(stream, commands, sizeof(commands) / sizeof(Command));

  stream.feed("set\n");
  serial.readSerial();

  const std::string out = stream.takeOutput();
  ASSERT_TRUE(contains(out, "ERROR: Not enough arguments"));
  ASSERT_TRUE(contains(out, "set <int>"));
}

void test_reports_range_error() {
  FakeStream stream;
  Command commands[] = {
    COMMAND(cmd_set_int, "set", ARG(ArgType::Int, 2, 5, "pin"), nullptr, ""),
  };
  SerialCommands serial(stream, commands, sizeof(commands) / sizeof(Command));

  stream.feed("set 8\n");
  serial.readSerial();

  const std::string out = stream.takeOutput();
  ASSERT_TRUE(contains(out, "ERROR: Argument out of range 1 (2 - 5)"));
  ASSERT_TRUE(contains(out, "set <pin>"));
}

void test_supports_quoted_string_arg() {
  g_text.clear();
  FakeStream stream;
  Command commands[] = {
    COMMAND(cmd_set_name, "name", ArgType::String, nullptr, ""),
  };
  SerialCommands serial(stream, commands, sizeof(commands) / sizeof(Command));

  stream.feed("name \"John Doe\"\n");
  serial.readSerial();

  ASSERT_EQ(g_text, "John Doe");
}

void test_runs_subcommand_with_parent_args() {
  g_sum = 0;
  FakeStream stream;
  SerialCommands serial(stream, g_calc_commands, sizeof(g_calc_commands) / sizeof(Command));

  stream.feed("calc 4 + 5\n");
  serial.readSerial();

  ASSERT_EQ(g_sum, 9);
}

void test_reports_too_many_args() {
  FakeStream stream;
  Command commands[] = {
    COMMAND(cmd_set_int, "set", ArgType::Int, nullptr, ""),
  };
  SerialCommands serial(stream, commands, sizeof(commands) / sizeof(Command));

  stream.feed("set 7 9\n");
  serial.readSerial();

  const std::string out = stream.takeOutput();
  ASSERT_TRUE(contains(out, "ERROR: Too many arguments"));
  ASSERT_TRUE(contains(out, "set <int>"));
}

void test_handles_small_buffer_boundary_safely() {
  g_value = 0;
  FakeStream stream;
  char buffer[5];
  Command commands[] = {
    COMMAND(cmd_ping, "ping"),
  };
  SerialCommands serial(stream, commands, sizeof(commands) / sizeof(Command), buffer, sizeof(buffer));

  stream.feed("ping\n");
  serial.readSerial();
  ASSERT_EQ(g_value, 1);
  ASSERT_EQ(stream.takeOutput(), "");

  g_value = 0;
  stream.feed("pingx\n");
  serial.readSerial();
  const std::string out = stream.takeOutput();
  ASSERT_TRUE(contains(out, "ERROR: Buffer overflow"));
  ASSERT_EQ(g_value, 0);
}

int main() {
  test_runs_command_without_args();
  test_parses_integer_argument();
  test_reports_unknown_command();
  test_reports_not_enough_args();
  test_reports_range_error();
  test_supports_quoted_string_arg();
  test_runs_subcommand_with_parent_args();
  test_reports_too_many_args();
  test_handles_small_buffer_boundary_safely();

  if (failures > 0) {
    std::cerr << failures << " test(s) failed." << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "All tests passed." << std::endl;
  return EXIT_SUCCESS;
}
