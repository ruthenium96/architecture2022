#pragma once
#include "shell/src/state.h"
#include "shell/src/parser.h"

#include <memory>

namespace shell::utils {

bool is_valid_identifier_symbol(char c) {
    return std::isalnum(c) || c == '_';
}

std::string substitute_single_token(const std::string& token_value, const State& state) {
    std::string_view token_view{token_value};
    std::string result;
    size_t index = 0;
    while (index < token_value.size()) {
        if (token_value[index] != '$') {
            result += token_view[index++];
            continue;
        }
        const size_t index_identifier_start = ++index;
        while (is_valid_identifier_symbol(token_view[index]) && index < token_view.size()) {
            ++index;
        }
        auto identifier = token_view.substr(index_identifier_start, index - index_identifier_start);

        result += state.get_env().get_var(std::string{identifier});
    }
    return result;
}

void substitute_tokens(std::vector<std::shared_ptr<parser::IToken>>& tokens, const State& state) {
    for (auto& token : tokens) {
        if (std::dynamic_pointer_cast<parser::RawStringToken>(token)) {
            continue;
        }

        std::string token_value = token->GetValue();
        if (token_value.empty()) {
            throw ParsingError();
        }
        std::string substituted_value = substitute_single_token(token_value, state);
        token->SetValue(substituted_value);
    }
}

CommandBinding construct_command_binding(std::vector<std::shared_ptr<parser::IToken>>& tokens,
                                         State& state,
                                         IStreams& streams) {
    substitute_tokens(tokens, state);
    auto& cmd_manager = CommandManager::get_instance();

    std::string cmd_name = tokens[0]->GetValue();
    auto command = cmd_manager.get_command(cmd_name);

    Arguments args;
    for (size_t index = 1; index < tokens.size(); ++index) {
        args.push_back(tokens[index]->GetValue());
    }

    return CommandBinding{command, args, state, streams};
}

} // namespace shell::utils
