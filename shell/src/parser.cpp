#include "shell/src/parser.h"

namespace shell::parser {

std::shared_ptr<IToken> Parser::get_next_token() {
    const size_t single_quote_pos = line_view_.find('\'');
    const size_t double_quote_pos = line_view_.find('"');

    const size_t min_quote_pos = std::min(double_quote_pos, single_quote_pos);
    const char min_quote_value = double_quote_pos > single_quote_pos ? '\'' : '"';

    auto space_pos = line_view_.find(' ');
    if (min_quote_pos < space_pos && min_quote_pos > 0) {  // pattern like # dsa"text " # isn't valid
        throw ParsingError();
    } else if (min_quote_pos == std::string::npos && space_pos == std::string::npos) {
        auto command_value = std::string{line_view_};
        line_view_ = line_view_.substr(line_view_.size());
        return std::make_shared<StringToken>(command_value);
    } else if (min_quote_pos >= space_pos) {
        auto command_value = std::string{line_view_.substr(0, space_pos)};

        size_t next_token_start = space_pos == std::string::npos ? line_view_.size() : space_pos + 1;
        line_view_ = line_view_.substr(next_token_start);
        return std::make_shared<StringToken>(command_value);
    }

    size_t next_quote_pos = line_view_.find(min_quote_value, min_quote_pos + 1);
    if (next_quote_pos == std::string::npos) {
        throw ParsingError();
    }
    size_t next_space = line_view_.find(' ', next_quote_pos);

    if (next_space != std::string::npos && next_quote_pos != std::string::npos && next_space != next_quote_pos + 1) {
        throw ParsingError();
    }

    // NB: quotes are trimmed here
    auto argument_value = std::string{line_view_.substr(min_quote_pos + 1, next_quote_pos - min_quote_pos - 1)};
    std::shared_ptr<IToken> token;
    if (min_quote_value == '"') {
        token = std::make_shared<StringToken>(argument_value);
    } else {
        token = std::make_shared<RawStringToken>(argument_value);
    }

    size_t next_token_start = next_space == std::string::npos ? line_view_.size() : next_space + 1;
    line_view_ = line_view_.substr(next_token_start);
    return token;
}

std::optional<CommandDescriptor> Parser::parse_command() {
    auto token = get_next_token();
    // TODO: command name should be preprocessed as well as arguments
    std::string cmd_name = token->GetValue();
    if (cmd_name == "|" || cmd_name == "") {
        return std::nullopt;
    }
    std::vector<Argument> arguments;
//    if (!cmd_manager_.command_exist(cmd_name)) {
//        throw; // TODO: Should find executable
//    }

    auto command = cmd_manager_.get_command(cmd_name);

    while (true) {
        token = get_next_token();

        if (token->GetValue().empty() || token->GetValue() == "|") {
            break;
        }

        arguments.push_back(token->GetValue());
    };

    return CommandDescriptor{command, arguments};
}

} // namespace shell::parser
