#include "src/shell_executor/shell_executor.h"
#include <iostream>
#include <cassert>
#include <string>

void run_test(std::string&& test_input) {
    StreamsBuffered streams(std::move(test_input));
    State state;

    Shell shell(streams, state);
    shell.execute();

    assert(streams.get_out_buffer() == "TestCmd running with arguments: arg1 2 arg3 \n");
}

class ShellTestRunner {
public:
    ShellTestRunner(const std::string& test_in, const std::string& test_out)
        : test_in_{test_in}, test_out_{test_out}
    {
    }

    void run() {
        std::cout << "Test " << test_count_++ << ": ";

        StreamsBuffered streams(test_in_);
        State state;

        Shell shell(streams, state);
        shell.execute();

        if (streams.get_out_buffer() == test_out_) {
            std::cout << "passed" << std::endl; 
        } else {
            std::cout << "failed" << std::endl; 
        }
        
        if (abort_if_assert_failed) {
            assert(streams.get_out_buffer() == test_out_);
        }

    }

private:
    inline static size_t test_count_{1};
    inline static bool abort_if_assert_failed{false};

    std::string test_in_;
    std::string test_out_;
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
                        "TestCmd running with arguments: arg1 2 arg3 \n").run();
    }

    {
        ShellTestRunner("exit",
                        "").run();
    }

    {
        ShellTestRunner("echo azaza",
                        "azaza\n").run();
    }



    // return state.get_errno();
    return 0;




}
