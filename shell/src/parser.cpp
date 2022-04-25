#include "shell/src/parser.h"

#include <vector>

namespace shell::parser {

std::optional<std::shared_ptr<IToken>> Parser::get_next_token() {
    bool isSingleQuoteOpen = false;
    bool isDoubleQuoteOpen = false;
    bool isNonSpaceMet = false;

    size_t start_position = current_position_;
    while (true) {
        if (current_position_ >= line_.size()) {
            // End of line:
            if (isSingleQuoteOpen || isDoubleQuoteOpen) {
                // if some quote was not closed:
                return std::nullopt;
            } else {
                // finish token
                break;
            }
        } else if (!isNonSpaceMet) {
            // Truncate leading spaces
            if (line_[current_position_] == ' ') {
                // move right if it still spaces
                ++current_position_;
                continue;
            } else {
                // start token
                start_position = current_position_;
                isNonSpaceMet = true;
            }
        }
        if (line_[current_position_] == ' ') {
            if (isSingleQuoteOpen || isDoubleQuoteOpen) {
                ++current_position_;
            } else {
                // finish token
                break;
            }
        } else if (line_[current_position_] == '"') {
            // close or open double quotes
            isDoubleQuoteOpen = !isDoubleQuoteOpen;
            ++current_position_;
        } else if (line_[current_position_] == '\'') {
            // close or open single quotes
            isSingleQuoteOpen = !isSingleQuoteOpen;
            ++current_position_;
        } else {
            ++current_position_;
        }
    }
    size_t token_length = current_position_ - start_position;
    std::string answer_string = "";
    if (start_position < line_.size()) {
        answer_string = std::string(line_.substr(start_position, token_length));
    }
    ++current_position_;
    return std::make_shared<StringToken>(answer_string);
}

std::optional<std::vector<std::shared_ptr<IToken>>> Parser::parse_tokens() {
    std::vector<std::shared_ptr<IToken>> tokens;

    auto token = get_next_token();
    if (!token.has_value()) {
        return std::nullopt;
    }
    while (!token.value()->GetValue().empty() && !is_pipe(token.value()->GetValue())) {
        tokens.push_back(token.value());
        token = get_next_token();
        if (!token.has_value()) {
            return std::nullopt;
        }
    };

    return tokens;
}

bool Parser::is_pipe(const std::string& token) const {
    return token == "|";
}

} // namespace shell::parser
