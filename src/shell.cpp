#include "src/shell_executor/shell_executor.h"
#include <iostream>

// main: run shell
int main() {
    std::cout << "Small C++ team's CLI is started! Write commands:" << std::endl;
    
    StreamsGlobal streams;
    State state;

    Shell shell(streams, state);
    shell.execute();

    // return state.get_errno();
    return 0;

}
