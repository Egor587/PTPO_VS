#define _CRT_SECURE_NO_WARNINGS
#include <clocale>
#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include "functions.h"

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    srand(static_cast<unsigned int>(time(nullptr)));

    std::vector<std::string> volumeFiles = {
        "War_and_Peace_Vol1.txt",
        "War_and_Peace_Vol2.txt",
        "War_and_Peace_Vol3.txt",
        "War_and_Peace_Vol4.txt"
    };

    const std::string resultTask1FileName = "result_task1.txt";
    const std::string resultTask2FileName = "result_task2.txt";
    int userChoice = -1;

    while (userChoice != 0) {
        printConsoleMenu();
        if (!(std::cin >> userChoice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        if (userChoice == 1) {
            processWordCounting(volumeFiles, resultTask1FileName);
        }
        else if (userChoice == 2) {
            processWordIndexing(volumeFiles, resultTask2FileName);
        }
        else if (userChoice == 3) {
            runTaskThree();
        }
        else {
            std::cout << "Invalid input, try again" << std::endl;
        }
    }

    return 0;
}