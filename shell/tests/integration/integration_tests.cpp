#include "include/gtest/gtest.h"
#include "shell/src/shell.h"

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
    auto streams = std::make_shared<shell::StreamsBuffered>(parameter.input);

    shell::Shell shell_object(streams);
    shell_object.start();

    EXPECT_EQ(streams->get_err_buffer(), parameter.error_stream_expected);
    EXPECT_EQ(streams->get_out_buffer(), parameter.out_stream_expected);
}


std::vector<TestParameter> test_cases = {
    TestParameter{"echo test program",
          "test program\n",
          ""},
    TestParameter{"echo arg1\necho arg1",
                  "arg1\narg1\n",
                  ""},
};

INSTANTIATE_TEST_SUITE_P(
        ShellExecutorIntegrationTests,
        ShellExecutorIntegrationTestsFixture,
        ::testing::ValuesIn(test_cases));
