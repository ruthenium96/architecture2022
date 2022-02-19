#include "src/shell_executor/shell_executor.h"
#include <iostream>
#include <cassert>
#include <string>

class ShellTestRunner {
public:
    ShellTestRunner(const std::string& test_in, 
                    const std::string& test_out, 
                    const std::string& test_err = "")
        : test_in_{test_in},
          test_out_{test_out},
          test_err_{test_err}
    {
    }

    void run() {
        std::cout << "Test " << test_count_++ << ": ";

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

        if (asserts_enabled_) {
            assert(test_result_);
        }
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
        ShellTestRunner("test arg1 2 arg3\nexit",
                        "TestCmd running with arguments: arg1 2 arg3 \n",
                        "Execution finished using exit command!\n"
                        ).run();
    }

    {
        ShellTestRunner("exit",
                        "",
                        "Execution finished using exit command!\n"
                       ).run();
    }

    {
        ShellTestRunner("exit",
                        "",
                        "Execution finished using exit command!\n"
                       ).run();
    }



    // return state.get_errno();
    return 0;




}
