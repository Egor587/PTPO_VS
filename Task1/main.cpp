#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include "functions.h"

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    std::vector<std::string> volumeFiles = {
        "Война и мир. Том 1.txt",
        "Война и мир. Том 2.txt",
        "Война и мир. Том 3.txt",
        "Война и мир. Том 4.txt"
    };

    const std::string resultFileName = "result_task1.txt";
    int userChoice = -1;

    while (userChoice != 0) {
        printConsoleMenu();
        if (!(std::cin >> userChoice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        if (userChoice == 1) {
            processWordCounting(volumeFiles, resultFileName);
        }
        else if (userChoice == 0) {
            std::cout << "Завершение работы программы" << std::endl;
        }
        else {
            std::cout << "Неверный ввод, попробуйте снова" << std::endl;
        }
    }

    return 0;
}