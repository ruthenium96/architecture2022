#include "gtest/gtest.h"
#include "src/shell_executor/shell_executor.h"

#include <iostream>
#include <string>

struct TestParameter {
    std::string input;
    std::string out_stream_expected;
    std::string error_stream_expected;
};

class ShellExecutorIntegrationTestsFixture : public ::testing::TestWithParam<TestParameter> {};


TEST_P(ShellExecutorIntegrationTestsFixture, CommandExecution) {
    TestParameter parameter = GetParam();

    StreamsBuffered streams(parameter.input);
    State state;

    Shell().execute(streams, state);

    EXPECT_EQ(streams.get_err_buffer(), parameter.error_stream_expected);
    EXPECT_EQ(streams.get_out_buffer(), parameter.out_stream_expected);
}


std::vector<TestParameter> test_cases = {
    TestParameter{"exit",
                  "",
                  "Execution finished using exit command!\n"},

    TestParameter{"test arg1\n",
                  "TestCmd running with arguments: arg1 \n",
                  ""},

    TestParameter{"test arg1 2 arg3\nexit",
                  "TestCmd running with arguments: arg1 2 arg3 \n",
                  "Execution finished using exit command!\n"},

    TestParameter{"echo arg1\nexit",
                  "arg1\n",
                  "Execution finished using exit command!\n"},

    TestParameter{"echo arg1\necho arg1\nexit",
                  "arg1\narg1\n",
                  "Execution finished using exit command!\n"},
};

INSTANTIATE_TEST_SUITE_P(
        ShellExecutorIntegrationTests,
        ShellExecutorIntegrationTestsFixture,
        ::testing::ValuesIn(test_cases));
