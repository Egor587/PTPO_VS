#include "Cli.h"
#include <iostream>
#include <string>

CliOptions::CliOptions()
    : hasMode(false), mode(0),
    hasFile(false), file(""),
    hasResult(false), result(""),
    hasHelp(false) {
}

CliOptions parseCommandLine(int argc, char* argv[]) {
    CliOptions options;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            options.hasHelp = true;
            continue;
        }

        // возможность ввода --mode=1 или --mode 1
        if (arg.find("--mode=") == 0) {
            options.mode = std::stoi(arg.substr(7));
            options.hasMode = true;
        }
        else if (arg == "--mode" && i + 1 < argc) {
            options.mode = std::stoi(argv[++i]);
            options.hasMode = true;
        }
        else if (arg.find("--file=") == 0) {
            options.file = arg.substr(7);
            options.hasFile = true;
        }
        else if (arg == "--file" && i + 1 < argc) {
            options.file = argv[++i];
            options.hasFile = true;
        }
        else if (arg.find("--result=") == 0) {
            options.result = arg.substr(9);
            options.hasResult = true;
        }
        else if (arg == "--result" && i + 1 < argc) {
            options.result = argv[++i];
            options.hasResult = true;
        }
    }

    return options;
}

void printCliHelp() {
    std::cout << "Usage: program.exe [OPTIONS]" << std::endl;
    std::cout << "  --mode=N       Run menu item N (1, 2, 3)" << std::endl;
    std::cout << "  --file=PATH    Input file path" << std::endl;
    std::cout << "  --result=PATH  Output file path" << std::endl;
    std::cout << "  --help         Show this help" << std::endl;
}