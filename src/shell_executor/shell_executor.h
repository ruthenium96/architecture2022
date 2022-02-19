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



// ----------------
// State class
// ----------------

class State {
public:
    class Env {
        void set_var(const std::string& var_name, const std::string& var_value) {
            vars_[var_name] = var_value;
        }

        std::string get_var(const std::string& var_name) const { 
            const auto it = vars_.find(var_name);
            
            if (it != vars_.end()) {
                return it->second;
            }

            return {};

        }

    private:
        std::map<std::string, std::string> vars_;
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
    std::istream& get_in_stream() override {return is_; }
    std::ostream& get_out_stream() override {return os_; }
    std::ostream& get_err_stream() override {return es_; }

    explicit StreamsBuffered(std::string input = "") : is_{std::move(input)} {}

    std::string get_out_buffer() {
        return os_.str();
    }

private:
    std::istringstream is_;
    std::ostringstream os_;
    std::ostringstream es_;
};




// ----------------
// Cmd classes
// ----------------

class Args {
public:
    using ArgType = std::string;

    Args() = default;
    explicit Args(ArgType arg) : args_{std::move(arg)} {}
    explicit Args(std::vector<ArgType> args) : args_{std::move(args)} {}


    const ArgType& get_arg(size_t i) const {
         return args_.at(i); 
    }

    void add_arg(ArgType arg) {
        args_.push_back(std::move(arg));
    }

    const std::vector<ArgType>& get_all() const { 
        return args_; 
    }

private:
    std::vector<ArgType> args_;
};


class Cmd {
public:

    Cmd(std::string cmd, std::vector<std::string> args = {})
        : cmd_{std::move(cmd)}, args_{std::move(args)} 
    {
        // do nothing
    }

    std::string& get_cmd() { return cmd_; }
    Args& get_args() { return args_; }

private:
    std::string cmd_;
    Args args_;
};


class CmdRaw : public Cmd {
public:
    CmdRaw(std::string cmd, std::vector<std::string> args = {})
        : Cmd(std::move(cmd), std::move(args)) 
    {

    }

    // substitution will be here
    void preprocess([[maybe_unused]] State& state) {}
};




// ----------------
// CmdParser classes
// ----------------

class ICmdParser {
public:
    virtual bool empty() const = 0;
    virtual void stop() = 0;
    virtual CmdRaw get_next_cmd() = 0;

    virtual ~ICmdParser() = default;
};


// single subcommand: one-word cmd and args
class CmdParserSimple : public ICmdParser {
public:
    explicit CmdParserSimple(std::string& cmd_line) {
        std::istringstream iss(cmd_line);
        iss >> cmd_;

        
        auto in_it = std::istream_iterator<std::string>(iss);
        
        std::copy(in_it,
                  std::istream_iterator<std::string>(),
                  std::back_inserter(args_));
    }

    bool empty() const override {
        return empty_;
    }

    void stop() override {
        empty_ = true;
    }

    CmdRaw get_next_cmd() override {
        empty_ = true;
        return {cmd_, args_};
    }

private:
    bool empty_{false};

    std::string cmd_;
    std::vector<std::string> args_;

};



// stub test class
class CmdParserStub : public ICmdParser {
private:
    constexpr static size_t kMaxQueryCount{3};
    size_t query_count{0}; 

public:
    explicit CmdParserStub([[maybe_unused]] std::string& cmd_line) {}

    bool empty() const override {
        return empty_;
    }


    void stop() override { empty_ = true; }


    CmdRaw get_next_cmd() override {
        if (query_count >= kMaxQueryCount) {
            return {{"exit"}};
        }

        ++query_count;
        return {{"test"}, {"arg1", "arg2_" + std::to_string(query_count)}};
    }

private:
    bool empty_{false};
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
    virtual void execute(Args& args, IStreams& streams, State& state) = 0;
    virtual ~IExecutable() = default;
};

// test
class TestCmd : public IExecutable {
public:
    void execute(Args& args, IStreams& streams, [[maybe_unused]] State& state) {
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
    void execute(Args& args, IStreams& streams, [[maybe_unused]] State& state) {
        streams.get_out_stream() << args.get_arg(0) << std::endl;
    }
};


// exit
class ExitCmd : public IExecutable {
public:
    void execute([[maybe_unused]] Args& args, [[maybe_unused]] IStreams& streams, [[maybe_unused]] State& state) {
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
            binary_ = std::make_unique<TestCmd>();
        }
        else if (cmd.get_cmd() == "exit") {
            binary_ =  std::make_unique<ExitCmd>();
        }
        else if (cmd.get_cmd() == "echo") {
            binary_ =  std::make_unique<EchoCmd>();
        }
        else {
            throw CmdNotFoundException();
        }
    }


    void run(IStreams& streams, State& state) {
        try {
            binary_->execute(cmd_.get_args(), streams, state);
        } 
        catch (const CmdExitException& e) {
            throw;
        }
        catch (...) {
            throw CmdErrorException();
        }
    }
private:
    std::unique_ptr<IExecutable> binary_{};
    Cmd cmd_;
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

    Shell(IStreams& streams, State& state) : streams_{streams}, state_{state} {}



    void execute() {
        status_ = ShellStatus::RUNNING;

        while ((status_ == ShellStatus::RUNNING) && streams_.get_in_stream()) {
            std::string cmd_line;


            // CmdParserStub cmd_parser(cmd_line);
            // comment two lines below to use test stub
            std::getline(streams_.get_in_stream(), cmd_line);
            CmdParserSimple cmd_parser(cmd_line);


            StreamsBuffered streams_local;

            while (!cmd_parser.empty()) {
            
                try {
                    CmdRaw current_subcommand = cmd_parser.get_next_cmd(); 
                    current_subcommand.preprocess(state_);
                
                    Program current_program = Program(current_subcommand);

                    current_program.run(streams_local, state_);
                } 
                catch (const SubcommandParseException& e) {
                    streams_local.get_err_stream() << e.what() << std::endl;
                } 
                catch (const CmdNotFoundException& e) {
                    streams_local.get_err_stream() << e.what() << std::endl;
                } 
                catch (const CmdErrorException& e) {
                    streams_local.get_err_stream() << e.what() << std::endl;
                    // cmd_parser.stop();
                    break;
                } 
                catch (const CmdExitException& e) {
                    streams_local.get_err_stream() << e.what() << std::endl;
                    // cmd_parser.stop();
                    status_ = ShellStatus::FINISHED;
                    break;
                }                
            }

            streams_.get_out_stream() << streams_local.get_out_buffer();

        }
    }


private:
    IStreams& streams_;
    State& state_;

    enum class ShellStatus {
        IDLE, RUNNING, FINISHED
    };
    ShellStatus status_{ShellStatus::IDLE};
};





#endif // SRC_SHELL_EXECUTOR_SHELL_EXECUTOR_H