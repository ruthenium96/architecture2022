#include "gtest/gtest.h"
#include "src/shell_executor/shell_executor.h"

#include <iostream>
#include <string>


TEST(ShellExecutorUnitTests, TestCommand) {
    std::string executable_token = "test";
    std::vector<std::string> args = {"arg0", "1", "arg2"};

    std::shared_ptr<ICmd> cmd = std::make_shared<Cmd>(std::move(executable_token), 
                                                      std::move(args));

    ASSERT_EQ(cmd->get_cmd(), "test");
    ASSERT_EQ(cmd->get_args().get_all().size(), 3);
    ASSERT_EQ(cmd->get_args().get_arg(0), "arg0");
    ASSERT_EQ(cmd->get_args().get_arg(1), "1");
    ASSERT_EQ(cmd->get_args().get_arg(2), "arg2");
}

TEST(ShellExecutorUnitTests, SimpleSecondaryCmdParser) {
    std::shared_ptr<ICmd> cmd = std::make_shared<NoArgumentCmd>("test arg0 1 arg2");

    SimpleSecondaryCmdParser parser(cmd);
    auto subcommand = parser.next();

    ASSERT_EQ(subcommand->get_cmd(), "test");
    ASSERT_EQ(subcommand->get_args().get_all().size(), 3);
    ASSERT_EQ(subcommand->get_args().get_arg(0), "arg0");
    ASSERT_EQ(subcommand->get_args().get_arg(1), "1");
    ASSERT_EQ(subcommand->get_args().get_arg(2), "arg2");
}
