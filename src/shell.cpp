#include "src/shell_executor/shell_executor.h"

// main: run shell
int main() {
    StreamsGlobal streams;
    State state;

    Shell shell(streams, state);
    shell.execute();

    // return state.get_errno();
    return 0;

}
