#pragma once

#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <sstream>
#include <string_view>
#include <memory>
#include <optional>

#include "shell/src/exception.h"
#include "shell/src/commands/command.h"

namespace shell::parser {

// Token is "atomic" part of input.
class IToken {
public:
    explicit IToken(std::string value) : value_(std::move(value)) {}
    const std::string& GetValue() const {
        return value_;
    }
    void SetValue(const std::string& value) {
        value_ = value;
    }
    virtual ~IToken() {}
private:
    std::string value_;
};


class StringToken : public IToken {
public:
    StringToken(std::string value) : IToken(std::move(value)) {}
    ~StringToken() override = default;
};

using Argument = std::string;

struct CommandDescriptor {
    std::shared_ptr<Command> command;
    std::vector<Argument> arguments;
};

// Converts input line into IToken vectors.
// At first, it lazy splits line with pipes.
// At second, it splits line with spaces.
class Parser {
public:
    explicit Parser(std::string line) : line_(std::move(line)), line_view_(line_) {}

    // Returns next ITokens until new pipe or line end.
    std::optional<std::vector<std::shared_ptr<IToken>>> parse_tokens();

    // It is public only for tests. ¯\_(ツ)_/¯
    // But it splits line onto Itokens.
    std::optional<std::shared_ptr<IToken>> get_next_token();
private:
    bool is_pipe(const std::string& token) const;
    std::string line_;
    std::string_view line_view_;
    size_t current_position_ = 0;
};


} // namespace shell::parser
