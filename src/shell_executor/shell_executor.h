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
// ArgsHolder class
// ----------------

class ArgsHolder {
public:
    explicit ArgsHolder(args_t args = {}) : args_{std::move(args)} {}

    const token_t& get_arg(size_t i = 0) const {
         return args_.at(i); 
    }

    void add_arg(token_t arg) {
        args_.push_back(std::move(arg));
    }

    const args_t& get_all() const { 
        return args_; 
    }

    bool empty() const { return args_.empty(); }
private:
    args_t args_;
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
// Cmd classes
// ----------------

class ICmd {
public:
    virtual const token_t& get_cmd() const = 0;

    virtual bool has_args() const = 0;
    virtual const ArgsHolder& get_args() const = 0;
    
    virtual ~ICmd() = default;
};



class Cmd : public ICmd {
public:
    Cmd(token_t cmd, args_t args)
        : cmd_{std::move(cmd)}, args_{std::move(args)}
    {
    }

    const token_t& get_cmd() const override { return cmd_; }

    bool has_args() const override {return get_args().empty(); }
    const ArgsHolder& get_args() const override { return args_; }

private:
    token_t cmd_;
    ArgsHolder args_;
};



class NoArgumentCmd : public Cmd {
public:    
    explicit NoArgumentCmd(token_t cmd) : Cmd{cmd, {}} {}

    bool has_args() const override { return false; }
};




// ----------------
// Program class
// ----------------

// pimpl
class Program {
public:
    Program(std::shared_ptr<ICmd> cmd) : cmd_{cmd} {
        token_t cmd_line = cmd->get_cmd();

        if (cmd_line == "test") {
            executable_ = std::make_unique<TestCmd>();
        }
        else if (cmd_line == "exit") {
            executable_ =  std::make_unique<ExitCmd>();
        }
        else if (cmd_line == "echo") {
            executable_ =  std::make_unique<EchoCmd>();
        }
        else {
            throw CmdNotFoundException();
        }
    }


    void run(IStreams& streams, IState& state) {
        try {
            executable_->execute(cmd_->get_args(), streams, state);
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
    std::shared_ptr<ICmd> cmd_;
};






// ----------------
// Parser classes
// ----------------

class ICmdParser {
public:
    virtual bool empty() const = 0;
    virtual std::shared_ptr<ICmd> next() = 0;

    virtual ~ICmdParser() = default;
};


// one-line command without pipes and subshells
class SimpleInitialCmdParser : public ICmdParser {
public:
    explicit SimpleInitialCmdParser(token_t cmd_line) 
        : cmd_line_{std::move(cmd_line)}
    {
        // here will be splitting into pipe commands
    }

    bool empty() const override {return empty_; }

    std::shared_ptr<ICmd> next() override {
        empty_ = true;
        std::shared_ptr<ICmd> cmd = std::make_shared<NoArgumentCmd>(cmd_line_);
        return cmd;
    }

private:
    bool empty_{false};
    token_t cmd_line_;
};


// single subcommand: one-word cmd and args
class SimpleSecondaryCmdParser : public ICmdParser {
public:
    SimpleSecondaryCmdParser(std::shared_ptr<ICmd> cmd) 
        : cmd_{cmd}
    {
    }

    bool empty() const override {return empty_; }

    std::shared_ptr<ICmd> next() override {
        token_t executable_token;
        args_t args;
        
        std::istringstream iss(cmd_->get_cmd());

        iss >> executable_token;

        std::copy(std::istream_iterator<token_t>(iss),
                  std::istream_iterator<token_t>(),
                  std::back_inserter(args));
        
        
        std::shared_ptr<ICmd> new_cmd = std::make_shared<Cmd>(std::move(executable_token), 
                                                              std::move(args));
        empty_ = true;

        return new_cmd;
    }

private:
    bool empty_{false};
    std::shared_ptr<ICmd> cmd_;
};



// ----------------
// CommandProcessor class
// ----------------

// pattern strategy looks nicely here
class CommandProcessor {
public:
    CommandProcessor() = default;

    void process(std::shared_ptr<ICmd> cmd, IState& state) {
        substitute(cmd, state);
     }  

private:
    void substitute([[maybe_unused]] std::shared_ptr<ICmd> cmd, 
                    [[maybe_unused]] const IState& state) 
    {
    }   
};



// ----------------
// Our CLI shell class
// ----------------

class Shell {
public:
    Shell(IStreams& streams, IState& state) : streams_{streams}, state_{state} {}

    void execute() {
        this->start();

        while (is_running() && streams_.get_in_stream()) {
            token_t cmd_line;

            std::getline(streams_.get_in_stream(), cmd_line);
            SimpleInitialCmdParser cmd_parser(cmd_line);

            StreamsBuffered streams_local;

            while (!cmd_parser.empty()) {
                try {
                    std::shared_ptr<ICmd> subcommand = cmd_parser.next(); 

                    CommandProcessor processor;
                    processor.process(subcommand, state_);

                    SimpleSecondaryCmdParser subcommand_parser(subcommand);
                    std::shared_ptr<ICmd> parsed_subcommand = subcommand_parser.next();
                
                    Program program = Program(parsed_subcommand);

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
                    this->stop();
                    break;
                }                
            }

            streams_.get_out_stream() << streams_local.get_out_buffer();
            streams_.get_err_stream() << streams_local.get_err_buffer();
        }
    }

private:
    enum class ShellStatus {
        IDLE, RUNNING, FINISHED
    };
    ShellStatus status_{ShellStatus::IDLE};

    void start() { status_ = ShellStatus::RUNNING; }
    void stop() { status_ = ShellStatus::FINISHED; }
    ShellStatus status() const { return status_; }
    bool is_running() const {return status_ == ShellStatus::RUNNING; }

private:  
    IStreams& streams_;
    IState& state_;
};





#endif // SRC_SHELL_EXECUTOR_SHELL_EXECUTOR_H