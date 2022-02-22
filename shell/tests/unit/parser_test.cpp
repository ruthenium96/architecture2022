#include "include/gtest/gtest.h"

#include "shell/src/parser.h"
#include "shell/src/commands/echo_cmd.h"

#include <memory>


TEST(ParserTests, GetWord) {
   {
       shell::parser::Parser parser("cmd test test");
       auto token = parser.get_next_token();
       ASSERT_TRUE(std::dynamic_pointer_cast<shell::parser::StringToken>(token));
       ASSERT_EQ(token->GetValue(), "cmd");

       token = parser.get_next_token();
       ASSERT_TRUE(std::dynamic_pointer_cast<shell::parser::StringToken>(token));
       ASSERT_EQ(token->GetValue(), "test");

       token = parser.get_next_token();
       ASSERT_TRUE(std::dynamic_pointer_cast<shell::parser::StringToken>(token));
       ASSERT_EQ(token->GetValue(), "test");
   }
   {
       shell::parser::Parser parser("cmd 'few words argument' lol");
       auto token = parser.get_next_token();

       ASSERT_TRUE(std::dynamic_pointer_cast<shell::parser::StringToken>(token));
       ASSERT_EQ(token->GetValue(), "cmd");

       token = parser.get_next_token();
       ASSERT_TRUE(std::dynamic_pointer_cast<shell::parser::RawStringToken>(token));
       ASSERT_EQ(token->GetValue(), "few words argument");

       token = parser.get_next_token();
       ASSERT_TRUE(std::dynamic_pointer_cast<shell::parser::StringToken>(token));
       ASSERT_EQ(token->GetValue(), "lol");
   }
   {
       shell::parser::Parser parser("wc \"text multi word\"");
       auto token = parser.get_next_token();

       ASSERT_TRUE(std::dynamic_pointer_cast<shell::parser::StringToken>(token));
       ASSERT_EQ(token->GetValue(), "wc");

       token = parser.get_next_token();
       ASSERT_TRUE(std::dynamic_pointer_cast<shell::parser::StringToken>(token));
       ASSERT_EQ(token->GetValue(), "text multi word");
   }
}

TEST(ParserTests, parse_command) {
    shell::parser::Parser parser("echo 'some text'");
    auto descriptor = parser.parse_command();
    ASSERT_NE(descriptor, std::nullopt);
    ASSERT_TRUE(std::dynamic_pointer_cast<shell::EchoCommand>(descriptor.value().command) != nullptr);

    ASSERT_EQ(descriptor.value().arguments.size(), 1);
    ASSERT_EQ(descriptor.value().arguments[0], "some text");
}
