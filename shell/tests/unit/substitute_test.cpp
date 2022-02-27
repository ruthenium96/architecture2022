#include "include/gtest/gtest.h"

#include "shell/src/utils/substitute.h"
#include "shell/src/commands/echo_command.h"

#include <memory>


TEST(SubstituteTest, SubstituteSingleToken) {
    {
        shell::State state;
        state.get_env().set_var("x", "ec");
        state.get_env().set_var("y", "ho");

        const std::string pattern = "$x$y";
        const std::string substitude_result = shell::utils::substitute_single_token(pattern, state);

        const std::string expected = "echo";
        ASSERT_EQ(substitude_result, expected);
    }
    {
        shell::State state;
        state.get_env().set_var("command_name", "echo");
        state.get_env().set_var("command_arg", "Hello world!");

        const std::string pattern = "$command_name $command_arg";
        const std::string substitute_result = shell::utils::substitute_single_token(pattern, state);

        const std::string expected = "echo Hello world!";
        ASSERT_EQ(substitute_result, expected);
    }
}

TEST(SubstituteTest, SubstituteTokens) {
    std::vector<std::shared_ptr<shell::parser::IToken>> tokens = {
            std::make_shared<shell::parser::StringToken>("$x$y"),
            std::make_shared<shell::parser::StringToken>("$arg1"),
            std::make_shared<shell::parser::StringToken>("$arg2"),
            std::make_shared<shell::parser::StringToken>("$arg3"),
    };

    shell::State state;
    state.get_env().set_var("x", "ec");
    state.get_env().set_var("y", "ho");
    state.get_env().set_var("arg1", "argument1");
    state.get_env().set_var("arg2", "argument2");
    state.get_env().set_var("arg3", "argument3");

    shell::utils::substitute_tokens(tokens, state);

    ASSERT_EQ(tokens[0]->GetValue(), "echo");
    ASSERT_EQ(tokens[1]->GetValue(), "argument1");
    ASSERT_EQ(tokens[2]->GetValue(), "argument2");
    ASSERT_EQ(tokens[3]->GetValue(), "argument3");
}

TEST(SubstituteTest, ConstructCommand) {
    shell::CommandManager& manager = shell::CommandManager::get_instance();
    manager.register_command<shell::EchoCommand>("echo");

    std::vector<std::shared_ptr<shell::parser::IToken>> tokens = {
            std::make_shared<shell::parser::StringToken>("$a$b$c$d"),
            std::make_shared<shell::parser::StringToken>("Hello world")
    };

    shell::State state;
    shell::StreamsBuffered streams;
    state.get_env().set_var("a", "e");
    state.get_env().set_var("b", "c");
    state.get_env().set_var("c", "h");
    state.get_env().set_var("d", "o");

    auto binding = shell::utils::construct_command_binding(tokens, state, streams);
    ASSERT_EQ(binding.get_arguments(), shell::Arguments{"Hello world"});
    ASSERT_TRUE(std::dynamic_pointer_cast<shell::EchoCommand>(binding.get_command()) != nullptr);
}
