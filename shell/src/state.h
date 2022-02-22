#pragma once

#include <string>
#include <unordered_map>

namespace shell {

class IState {
public:
    virtual ~IState() = default;
};

class State : public IState {
public:
    class Env {
        void set_var(const std::string &var_name, const std::string &var_value) {
            vars_[var_name] = var_value;
        }

        std::string get_var(const std::string &var_name) const {
            const auto it = vars_.find(var_name);

            if (it != vars_.end()) {
                return it->second;
            }

            return {};
        }

    private:
        std::unordered_map<std::string, std::string> vars_;
    };

};
} // namespace shell
