#ifndef SRC_SHELL_EXECUTOR_SHELL_EXECUTOR_H
#define SRC_SHELL_EXECUTOR_SHELL_EXECUTOR_H


#include <iostream>
#include <map>
#include <string>
#include <optional>
#include <vector>
#include <memory>
#include <sstream>
#include <iterator>


using token_t = std::string;
using args_t = std::vector<token_t>;


// ----------------
// State class
// ----------------

class IState {
public:
    virtual ~IState() = default;
};

class State : public IState{
public:
    class Env {
        void set_var(const token_t& var_name, const token_t& var_value) {
            vars_[var_name] = var_value;
        }

        token_t get_var(const token_t& var_name) const { 
            const auto it = vars_.find(var_name);
            
            if (it != vars_.end()) {
                return it->second;
            }

            return {};

        }

    private:
        std::map<token_t, token_t> vars_;
    };

};



// ----------------
// Streams classes
// ----------------

class IStreams {
public:
    virtual std::istream& get_in_stream() = 0;
    virtual std::ostream& get_out_stream() = 0;
    virtual std::ostream& get_err_stream() = 0;

    virtual ~IStreams() = default;
};

class StreamsGlobal : public IStreams {
public:
    std::istream& get_in_stream() override {return std::cin; }
    std::ostream& get_out_stream() override {return std::cout; }
    std::ostream& get_err_stream() override {return std::cerr; }
};

class StreamsBuffered : public IStreams {
public:
    explicit StreamsBuffered(std::string input = "") : is_{std::move(input)} {}

    std::istream& get_in_stream() override {return is_; }
    std::ostream& get_out_stream() override {return os_; }
    std::ostream& get_err_stream() override {return es_; }

    std::string get_out_buffer() { return os_.str(); }
    std::string get_err_buffer() { return es_.str(); }
    

private:
    std::istringstream is_;
    std::ostringstream os_;
    std::ostringstream es_;
};




// ----------------
// Cmd classes
// ----------------

class ArgsHolder {
public:
    explicit ArgsHolder(args_t args = {}) : args_{std::move(args)} {}

    const token_t& get_arg(size_t i) const {
         return args_.at(i); 
    }

    void add_arg(token_t arg) {
        args_.push_back(std::move(arg));
    }

    const args_t& get_all() const { 
        return args_; 
    }

private:
    args_t args_;
};




class Cmd {
public:

    Cmd(token_t cmd, args_t args = {})
        : cmd_{std::move(cmd)}, args_{std::move(args)} 
    {
        // do nothing
    }

    token_t& get_cmd() { return cmd_; }
    ArgsHolder& get_args() { return args_; }

private:
    token_t cmd_;
    ArgsHolder args_;
};



class CmdLine {
public:
    CmdLine(token_t cmd_line) : cmd_line_(std::move(cmd_line)) {}

    const token_t& get_cmd_line() const { return cmd_line_; }

    void preprocess(const IState& state) {
        substitute(state);
     }  

private:
    void substitute([[maybe_unused]] const IState& state) { }   

private:
    token_t cmd_line_;
};








// ----------------
// Exceptions
// ----------------

struct SubcommandParseException : public std::exception {
    char const* what() const noexcept override {
        return "Failed to parse command!";
    }
};

struct CmdNotFoundException : public std::exception {
    char const* what() const noexcept override {
        return "Command is not found!";
    }
};

struct CmdErrorException : public std::exception {
    char const* what() const noexcept override {
        return "Error was raised during command execution!";
    }
};

struct CmdExitException : public std::exception {
    char const* what() const noexcept override {
        return "Execution finished using exit command!";
    }
};



// ----------------
// Commands to execute
// ----------------

// REDO: singleton
class IExecutable {
public:
    virtual void execute(const ArgsHolder& args, IStreams& streams, IState& state) = 0;
    virtual ~IExecutable() = default;
};

// test
class TestCmd : public IExecutable {
public:
    void execute(const ArgsHolder& args, IStreams& streams, [[maybe_unused]] IState& state) {
        auto& cout_ = streams.get_out_stream();
        cout_ << "TestCmd running with arguments: ";
        for (const auto& arg : args.get_all()) {
           cout_ << arg << " "; // fix
        }
        cout_ << std::endl;
    }
};


// echo
class EchoCmd : public IExecutable {
public:
    void execute(const ArgsHolder& args, IStreams& streams, [[maybe_unused]] IState& state) {
        streams.get_out_stream() << args.get_arg(0) << std::endl;
    }
};


// exit
class ExitCmd : public IExecutable {
public:
    void execute([[maybe_unused]] const ArgsHolder& args, [[maybe_unused]] IStreams& streams, [[maybe_unused]] IState& state) {
        throw CmdExitException();
    }
};




// ----------------
// Program class
// ----------------

// pimpl
class Program {
public:
    Program(Cmd& cmd) : cmd_(cmd) {
        if (cmd.get_cmd() == "test") {
            executable_ = std::make_unique<TestCmd>();
        }
        else if (cmd.get_cmd() == "exit") {
            executable_ =  std::make_unique<ExitCmd>();
        }
        else if (cmd.get_cmd() == "echo") {
            executable_ =  std::make_unique<EchoCmd>();
        }
        else {
            throw CmdNotFoundException();
        }
    }


    void run(IStreams& streams, IState& state) {
        try {
            executable_->execute(cmd_.get_args(), streams, state);
        } 
        catch (const CmdExitException& e) {
            throw;
        }
        catch (...) {
            throw CmdErrorException();
        }
    }
private:
    std::unique_ptr<IExecutable> executable_{};
    Cmd cmd_;
};






// ----------------
// Parser classes
// ----------------

// old interface
// class ICmdParser {
// public:
//     virtual bool empty() const = 0;
//     virtual CmdRaw get_next_cmd() = 0;

//     virtual ~ICmdParser() = default;
// };





class IInitialCmdParser {
public:
    virtual ~IInitialCmdParser() = default;
};

// one-line command without pipes and subshells
class SimpleInitialCmdParser : public IInitialCmdParser {
public:
    explicit SimpleInitialCmdParser([[maybe_unused]] const token_t& cmd_line) 
        : cmd_line_{cmd_line}
    {
        // here will be splitting into pipe commands
    }

    bool empty() const {
        return empty_;
    }

    CmdLine next() {
        empty_ = true;
        return {std::move(cmd_line_)};
    }

private:
    bool empty_{false};
    token_t cmd_line_;
};




class ISecondaryCmdParser {
public:
    virtual ~ISecondaryCmdParser() = default;
};

// single subcommand: one-word cmd and args
class SimpleSecondaryCmdParser : public ISecondaryCmdParser {
public:
    SimpleSecondaryCmdParser(const CmdLine& cmd_line) 
        : cmd_line_{cmd_line}
    {

    }

    bool empty() const {
        return empty_;
    }

    Cmd next() {
        token_t executable_token;
        args_t args;
        
        std::istringstream iss(cmd_line_.get_cmd_line());

        iss >> executable_token;
        std::copy(std::istream_iterator<token_t>(iss),
                  std::istream_iterator<token_t>(),
                  std::back_inserter(args));
        
        empty_ = true;
        return Cmd(std::move(executable_token), std::move(args));
    }

private:
    bool empty_{false};
    CmdLine cmd_line_;
};



// ----------------
// Our CLI shell class
// ----------------

class Shell {
public:
    Shell() = delete;
    Shell(const Shell&) = delete;
    Shell(Shell&&) = delete;
    Shell& operator=(const Shell&) = delete;
    Shell& operator=(Shell&&) = delete;

    Shell(IStreams& streams, IState& state) : streams_{streams}, state_{state} {}



    void execute() {
        status_ = ShellStatus::RUNNING;

        while ((status_ == ShellStatus::RUNNING) && streams_.get_in_stream()) {
            token_t cmd_line;


            // CmdParserStub cmd_parser(cmd_line);
            // comment two lines below to use test stub
            std::getline(streams_.get_in_stream(), cmd_line);
            SimpleInitialCmdParser cmd_line_parser(cmd_line);

            StreamsBuffered streams_local;

            while (!cmd_line_parser.empty()) {
            
                try {
                    CmdLine subcommand_line = cmd_line_parser.next(); 
                    subcommand_line.preprocess(state_);

                    SimpleSecondaryCmdParser subcommand_line_parser(subcommand_line);
                    Cmd subcommand = subcommand_line_parser.next();
                
                    Program program = Program(subcommand);

                    program.run(streams_local, state_);
                } 
                catch (const SubcommandParseException& e) {
                    streams_local.get_err_stream() << e.what() << std::endl;
                } 
                catch (const CmdNotFoundException& e) {
                    streams_local.get_err_stream() << e.what() << std::endl;
                } 
                catch (const CmdErrorException& e) {
                    streams_local.get_err_stream() << e.what() << std::endl;
                    break;
                } 
                catch (const CmdExitException& e) {
                    streams_local.get_err_stream() << e.what() << std::endl;
                    status_ = ShellStatus::FINISHED;
                    break;
                }                
            }

            streams_.get_out_stream() << streams_local.get_out_buffer();
            streams_.get_err_stream() << streams_local.get_err_buffer();
        }
    }



private:
    IStreams& streams_;
    IState& state_;

    enum class ShellStatus {
        IDLE, RUNNING, FINISHED
    };
    ShellStatus status_{ShellStatus::IDLE};
};





#endif // SRC_SHELL_EXECUTOR_SHELL_EXECUTOR_H