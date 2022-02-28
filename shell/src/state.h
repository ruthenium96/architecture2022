#pragma once

#include <string>
#include <unordered_map>

namespace shell {

class Env {
public:
    void set_var(const std::string &var_name, const std::string &var_value) {
        vars_[var_name] = var_value;
    }

    bool have_var(const std::string& var_name) const {
        return vars_.find(var_name) != vars_.end();
    }

    std::string get_var(const std::string &var_name) const {
        const auto it = vars_.find(var_name);

        if (it != vars_.end()) {
            return it->second;
        }

        return {};
    }

    std::unordered_map<std::string, std::string>::const_iterator begin() const {
        return vars_.begin();
    };
    std::unordered_map<std::string, std::string>::const_iterator end() const {
        return vars_.end();
    };

private:
    std::unordered_map<std::string, std::string> vars_;
};


class State {
public:
    State() = default;

    Env& get_env() {
        return env_;
    }

    const Env& get_env() const {
        return env_;
    }
private:
    Env env_;
};


} // namespace shell
