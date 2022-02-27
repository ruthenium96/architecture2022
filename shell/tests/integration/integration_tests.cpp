#include "include/gtest/gtest.h"
#include "shell/src/shell.h"
#include "shell/src/state.h"

#include <string>

struct TestParameter {
    std::string input;
    std::string out_stream_expected;
    std::string error_stream_expected;
};

class ShellExecutorIntegrationTestsFixture : public ::testing::TestWithParam<TestParameter> {};


TEST_P(ShellExecutorIntegrationTestsFixture, CommandExecution) {
    TestParameter parameter = GetParam();
    auto streams = shell::StreamsBuffered(parameter.input);

    shell::CommandManager::get_instance().initialize_builtin_commands();

    shell::Shell shell_object("shell");
    shell::State shell_state;
    shell_object.execute({}, shell_state, streams);

    EXPECT_EQ(streams.get_err_buffer(), parameter.error_stream_expected);
    EXPECT_EQ(streams.get_out_buffer(), parameter.out_stream_expected);
}


std::vector<TestParameter> test_cases = {
    TestParameter{"echo test program",
                  "test program\n",
                  ""},
    TestParameter{"echo \"many words argument\"\necho arg1",
                  "many words argument\narg1\n",
                  ""},
};

INSTANTIATE_TEST_SUITE_P(
        ShellExecutorIntegrationTests,
        ShellExecutorIntegrationTestsFixture,
        ::testing::ValuesIn(test_cases));
