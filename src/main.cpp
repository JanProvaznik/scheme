#include <iostream>
#include <string>
#include "util.h"
#include "reader.h"
#include "printer.h"
#include "datatypes/environment.h"
#include "evaluator.h"


void rep(const EnvironmentPtr &env) {
    try {
        print(*eval(read_stdin(), env));
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }


}

void execute_file(const std::string &path) {
    BaseEnvironment e;
    EnvironmentPtr eptr = std::make_shared<BaseEnvironment>(e);
    print(*eval(read_file(path), eptr));
}


[[noreturn]] void repl() {
    BaseEnvironment environment;
    EnvironmentPtr environment_ptr = std::make_shared<BaseEnvironment>(environment);
    while (true) {
        std::cout << ">> ";
        rep(environment_ptr);
    }

}

int main(int argc, char const *argv[]) {
    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        execute_file((std::string &) argv[1]);
    } else {
        std::cout << "Usage: " << argv[0] << " [file]" << std::endl;
        std::cout << "If no file is specified, the repl will be started" << std::endl;
        return 1;
    }

    return 0;
}
