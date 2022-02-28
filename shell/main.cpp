#include "src/shell.h"

int main() {
//    std::cout << "Small C++ team's CLI is started..." << std::endl;
    shell::CommandManager::get_instance().initialize_builtin_commands();
    shell::StreamsGlobal global;
    shell::State state;
    shell::Shell shell("CLI");
    shell.execute({}, state, global);
    return 0;
}