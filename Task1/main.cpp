#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include "functions.h"
#include "Resources.h"
#include "Sync.h"
#include "Cli.h"

int main(int argc, char* argv[]) {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    srand(static_cast<unsigned int>(time(nullptr)));

    //Загрука ресурсов (JSON + определение языка системы)
    Resources::loadFromFile("resources.txt");

    // Читаем параметры запуска
    CliOptions options = parseCommandLine(argc, argv);

    if (options.hasHelp) {
        printCliHelp();
        return 0;
    }

    // если программа запущена дважды, второй запуск ждёт первого
    Sync fileLock("Global\\PTPO_VS_Lab1_Mutex");

    std::vector<std::string> volumeFiles = {
        "War_and_Peace_Vol1.txt",
        "War_and_Peace_Vol2.txt",
        "War_and_Peace_Vol3.txt",
        "War_and_Peace_Vol4.txt"
    };

    const std::string resultTask1FileName = "result_task1.txt";
    const std::string resultTask2FileName = "result_task2.txt";

    //Работа в режиме командной строки
    if (options.hasMode) {
        fileLock.lock();

        std::chrono::high_resolution_clock::time_point totalStart = std::chrono::high_resolution_clock::now();

        if (options.mode == 1) {
            std::vector<std::string> files;
            if (options.hasFile) {
                files.push_back(options.file);
            }
            else {
                files = volumeFiles;
            }
            std::string output = options.hasResult ? options.result : resultTask1FileName;
            processWordCountingWithTiming(files, output);
        }
        else if (options.mode == 2) {
            std::vector<std::string> files;
            if (options.hasFile) {
                files.push_back(options.file);
            }
            else {
                files = volumeFiles;
            }
            std::string output = options.hasResult ? options.result : resultTask2FileName;
            processWordIndexingWithTiming(files, output);
        }
        else if (options.mode == 3) {
            runTaskThree();
        }
        else {
            std::cout << "Unknown mode: " << options.mode << std::endl;
        }

        std::chrono::high_resolution_clock::time_point totalEnd = std::chrono::high_resolution_clock::now();
        long long totalMs = std::chrono::duration_cast<std::chrono::milliseconds>(totalEnd - totalStart).count();
        std::cout << Resources::get("TotalTime") << totalMs << std::endl;

        fileLock.unlock();
        return 0;
    }

    // 5. Интерактивный режим (меню)
    int userChoice = -1;

    while (userChoice != 0) {
        printConsoleMenu();

        if (!(std::cin >> userChoice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        fileLock.lock();

        std::chrono::high_resolution_clock::time_point totalStart = std::chrono::high_resolution_clock::now();

        if (userChoice == 1) {
            processWordCountingWithTiming(volumeFiles, resultTask1FileName);
        }
        else if (userChoice == 2) {
            processWordIndexingWithTiming(volumeFiles, resultTask2FileName);
        }
        else if (userChoice == 3) {
            runTaskThree();
        }

        std::chrono::high_resolution_clock::time_point totalEnd = std::chrono::high_resolution_clock::now();
        long long totalMs = std::chrono::duration_cast<std::chrono::milliseconds>(totalEnd - totalStart).count();
        if (userChoice == 1 || userChoice == 2 || userChoice == 3) {
            std::cout << Resources::get("TotalTime") << totalMs << std::endl;
        }

        fileLock.unlock();
    }

    return 0;
}