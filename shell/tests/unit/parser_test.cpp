#include "include/gtest/gtest.h"

#include "shell/src/parser.h"

#include <memory>


TEST(ParserTests, GetWord) {
   {
       shell::parser::Parser parser("cmd test test");
       auto token = parser.get_next_token().value();
       ASSERT_EQ(token->GetValue(), "cmd");

       token = parser.get_next_token().value();
       ASSERT_EQ(token->GetValue(), "test");

       token = parser.get_next_token().value();
       ASSERT_EQ(token->GetValue(), "test");
   }
   {
       shell::parser::Parser parser("cmd 'few words argument' lol");
       auto token = parser.get_next_token().value();

       ASSERT_EQ(token->GetValue(), "cmd");

       token = parser.get_next_token().value();
       ASSERT_EQ(token->GetValue(), "'few words argument'");

       token = parser.get_next_token().value();
       ASSERT_EQ(token->GetValue(), "lol");
   }
   {
       shell::parser::Parser parser("wc \"text multi word\"");
       auto token = parser.get_next_token().value();

       ASSERT_TRUE(std::dynamic_pointer_cast<shell::parser::StringToken>(token));
       ASSERT_EQ(token->GetValue(), "wc");

       token = parser.get_next_token().value();
       ASSERT_TRUE(std::dynamic_pointer_cast<shell::parser::StringToken>(token));
       ASSERT_EQ(token->GetValue(), "\"text multi word\"");
   }
}

TEST(ParserTests, ParseCommand) {
    shell::parser::Parser parser("echo 'some text'");
    auto tokens = parser.parse_tokens().value();
    ASSERT_EQ(tokens.size(), 2);
    ASSERT_EQ(tokens[0]->GetValue(), "echo");

    ASSERT_EQ(tokens[1]->GetValue(), "'some text'");

}
