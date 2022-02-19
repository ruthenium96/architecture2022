#include "src/shell_executor/shell_executor.h"
#include <iostream>
#include <cassert>
#include <string>

inline bool tests_success_global{true};

class ShellTestExecutor {
public:
    ShellTestExecutor(const std::string& test_in, 
                      const std::string& test_out, 
                      const std::string& test_err = "")
        : test_in_{test_in},
          test_out_{test_out},
          test_err_{test_err}
    {
    }

    void run() {
        std::cout << "Test ShellExecutor " << test_count_++ << ": ";

        StreamsBuffered streams(test_in_);
        State state;

        Shell shell(streams, state);
        shell.execute();

        test_result_ = (streams.get_out_buffer() == test_out_) &&
                       (streams.get_err_buffer() == test_err_);

        std::cout << (test_result_ ? "passed" : "failed") << std::endl;

        if (!test_result_) {
            std::cout << "Expected out: \"" << test_out_ << "\"" << std::endl;
            std::cout << "Received out: \"" << streams.get_out_buffer() << "\"" << std::endl;
            
            std::cout << "Expected err: \"" << test_err_ << "\"" << std::endl;
            std::cout << "Received err: \"" << streams.get_err_buffer() << "\"" << std::endl;
        }


        tests_success_global &= test_result_;
    }
    
private:
    std::string test_in_;
    std::string test_out_;
    std::string test_err_;

    bool test_result_;

    inline static size_t test_count_{1};
    inline static bool asserts_enabled_{false};
};



// main: run shell
int main() {
    std::cout << "Small C++ team's CLI is started..." << std::endl;
    // {
    //     StreamsGlobal streams;
    //     State state;

    //     Shell shell(streams, state);
    //     shell.execute();
    // }

    {
        ShellTestExecutor("test arg1 2 arg3\nexit",
                          "TestCmd running with arguments: arg1 2 arg3 \n",
                          "Execution finished using exit command!\n"
                          ).run();
    }

    {
        ShellTestExecutor("exit",
                          "",
                          "Execution finished using exit command!\n"
                         ).run();
    }

    {
        ShellTestExecutor("exit",
                          "",
                          "Execution finished using exit command!\n"
                         ).run();
    }


    {   
        std::cout << "Test Cmd: ";
        bool test_result{true};
        {
            std::string executable_token = "test";
            std::vector<std::string> args = {"arg0", "1", "arg2"};

            std::shared_ptr<ICmd> cmd = std::make_shared<Cmd>(std::move(executable_token), 
                                                              std::move(args));
            
            test_result &= cmd->get_cmd() == "test";
            test_result &= cmd->get_args().get_all().size() == 3;
            test_result &= cmd->get_args().get_arg(0) == "arg0";
            test_result &= cmd->get_args().get_arg(1) == "1";
            test_result &= cmd->get_args().get_arg(2) == "arg2";

        }        
        tests_success_global &= test_result;
        std::cout << (test_result ? "passed" : "failed") << std::endl;
    }

    {   
        std::cout << "Test SimpleSecondaryCmdParser: ";
        bool test_result{true};
        {
            std::shared_ptr<ICmd> cmd = std::make_shared<NoArgumentCmd>("test arg0 1 arg2");
            
            SimpleSecondaryCmdParser parser(cmd);
            auto subcommand = parser.next();

            test_result &= subcommand->get_cmd() == "test";
            test_result &= subcommand->get_args().get_all().size() == 3;
            test_result &= subcommand->get_args().get_arg(0) == "arg0";
            test_result &= subcommand->get_args().get_arg(1) == "1";
            test_result &= subcommand->get_args().get_arg(2) == "arg2";

        }        
        tests_success_global &= test_result;
        std::cout << (test_result ? "passed" : "failed") << std::endl;
    }


    if (!tests_success_global) {
        return 1;
    }
    
    // return state.get_errno();
    return 0;




}
