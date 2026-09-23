#define _CRT_SECURE_NO_WARNINGS

#include "functions.h"
#include <cstdio>
#include <iostream>
#include <sstream>
#include <clocale>
#include <cctype>
#include <cstdlib>

std::string sanitizeAndNormalizeUTF8(const std::string& rawText) {
    std::string resultText = "";
    resultText.reserve(rawText.size());

    size_t textIndex = 0;
    const size_t textLength = rawText.length();

    while (textIndex < textLength) {
        unsigned char currentByte = static_cast<unsigned char>(rawText[textIndex]);

        //Английские буквы и цифры
        if ((currentByte >= 'a' && currentByte <= 'z') || (currentByte >= '0' && currentByte <= '9')) {
            resultText += static_cast<char>(currentByte);
            textIndex++;
        }
        else if (currentByte >= 'A' && currentByte <= 'Z') {
            resultText += static_cast<char>(currentByte + 32);
            textIndex++;
        }
        //Дефис внутри слов
        else if (currentByte == '-') {
            if (textIndex > 0 && textIndex + 1 < textLength &&
                resultText.back() != ' ' &&
                (static_cast<unsigned char>(rawText[textIndex + 1]) >= 0x80 ||
                    (rawText[textIndex + 1] >= 'a' && rawText[textIndex + 1] <= 'z') ||
                    (rawText[textIndex + 1] >= 'A' && rawText[textIndex + 1] <= 'Z'))) {
                resultText += '-';
            }
            else {
                resultText += ' ';
            }
            textIndex++;
        }
        //Русские буквы в кодировке UTF-8
        else if (currentByte == 0xD0 && (textIndex + 1) < textLength) {
            unsigned char nextByte = static_cast<unsigned char>(rawText[textIndex + 1]);

            if (nextByte >= 0x90 && nextByte <= 0x9F) {
                //'А'..'П' -> 'а'..'п'
                resultText += static_cast<char>(0xD0);
                resultText += static_cast<char>(nextByte + 0x20);
            }
            else if (nextByte >= 0xA0 && nextByte <= 0xAF) {
                //'Р'..'Я' -> 'р'..'я'
                resultText += static_cast<char>(0xD1);
                resultText += static_cast<char>(nextByte - 0x20);
            }
            else if (nextByte >= 0xB0 && nextByte <= 0xBF) {
                //'а'..'п'
                resultText += static_cast<char>(0xD0);
                resultText += static_cast<char>(nextByte);
            }
            else if (nextByte == 0x81) {
                //'Ё' -> 'ё'
                resultText += static_cast<char>(0xD1);
                resultText += static_cast<char>(0x91);
            }
            else {
                resultText += ' ';
            }
            textIndex += 2;
        }
        else if (currentByte == 0xD1 && (textIndex + 1) < textLength) {
            unsigned char nextByte = static_cast<unsigned char>(rawText[textIndex + 1]);

            if (nextByte >= 0x80 && nextByte <= 0x8F) {
                //'р'..'я'
                resultText += static_cast<char>(0xD1);
                resultText += static_cast<char>(nextByte);
            }
            else if (nextByte == 0x91) {
                //'ё'
                resultText += static_cast<char>(0xD1);
                resultText += static_cast<char>(0x91);
            }
            else {
                resultText += ' ';
            }
            textIndex += 2;
        }
        //Все остальные Юникод-символы (тире, кавычки многоточие …) превращаются в пробел
        else {
            resultText += ' ';
            textIndex++;
        }
    }

    return resultText;
}

std::string readFileContent(const std::string& inputFileName) {
    FILE* filePointer = fopen(inputFileName.c_str(), "rb");
    if (!filePointer) {
        std::cout << "Не удалось открыть файл: " << inputFileName << std::endl;
        return "";
    }

    fseek(filePointer, 0, SEEK_END);
    long fileSize = ftell(filePointer);
    fseek(filePointer, 0, SEEK_SET);

    if (fileSize <= 0) {
        fclose(filePointer);
        return "";
    }

    std::string bufferContent;
    bufferContent.resize(fileSize);

    size_t bytesRead = fread(&bufferContent[0], 1, fileSize, filePointer);
    fclose(filePointer);

    return bufferContent;
}

void processWordCounting(const std::vector<std::string>& fileList, const std::string& outputFileName) {
    std::cout << "\nОбработка файлов..." << std::endl;

    std::map<std::string, int> wordFrequencyMap;

    for (size_t fileIndex = 0; fileIndex < fileList.size(); ++fileIndex) {
        std::string fileData = readFileContent(fileList[fileIndex]);
        if (fileData.empty()) {
            continue;
        }

        std::string cleanedText = sanitizeAndNormalizeUTF8(fileData);

        std::stringstream bufferStream(cleanedText);
        std::string currentWord;

        while (bufferStream >> currentWord) {
            wordFrequencyMap[currentWord]++;
        }
    }

    FILE* outputPointer = fopen(outputFileName.c_str(), "w");
    if (!outputPointer) {
        std::cout << "Ошибка при создании файла" << std::endl;
        return;
    }

    for (const auto& pairItem : wordFrequencyMap) {
        fprintf(outputPointer, "%s - %d\n", pairItem.first.c_str(), pairItem.second);
    }

    fclose(outputPointer);
    std::cout << "Результаты подсчёта сохранены в файл: " << outputFileName << std::endl;
}

void printConsoleMenu() {
    std::cout << "Лабораторная работа 1" << std::endl;
    std::cout << "1 - Посчитать все слова во всех томах" << std::endl;
    std::cout << "2 - Индексация позиций слов во всех томах" << std::endl;
    std::cout << "3 - Задание 3 (простые числа, сортировка, диапазон)" << std::endl;
    std::cout << "0 - Выход" << std::endl;
    std::cout << "Выберите действие: ";
}

void processWordIndexing(const std::vector<std::string>& fileList, const std::string& outputFileName) {
    std::cout << "\nИндексация позиций слов..." << std::endl;

    std::map<std::string, std::vector<int>> wordPositionsMap;

    for (size_t fileIndex = 0; fileIndex < fileList.size(); ++fileIndex) {
        std::string fileData = readFileContent(fileList[fileIndex]);
        if (fileData.empty()) {
            continue;
        }

        std::string cleanedText = sanitizeAndNormalizeUTF8(fileData);
        std::stringstream bufferStream(cleanedText);
        std::string currentWord;
        int currentPosition = 0;

        while (bufferStream >> currentWord) {
            wordPositionsMap[currentWord].push_back(currentPosition);
            currentPosition++;
        }
    }

    FILE* outputPointer = fopen(outputFileName.c_str(), "w");
    if (!outputPointer) {
        std::cout << "Ошибка при создании файла" << std::endl;
        return;
    }

    for (const auto& pairItem : wordPositionsMap) {
        fprintf(outputPointer, "%s - ", pairItem.first.c_str());
        for (size_t positionIndex = 0; positionIndex < pairItem.second.size(); ++positionIndex) {
            if (positionIndex > 0) {
                fprintf(outputPointer, ", ");
            }
            fprintf(outputPointer, "%d", pairItem.second[positionIndex]);
        }
        fprintf(outputPointer, "\n");
    }

    fclose(outputPointer);
    std::cout << "Индексация позиций сохранена в файл: " << outputFileName << std::endl;
}

//Задание 3

bool isPrime(int number) {
    if (number < 2) {
        return false;
    }
    for (int divisor = 2; divisor * divisor <= number; divisor++) {
        if (number % divisor == 0) {
            return false;
        }
    }
    return true;
}

std::vector<int> generateRandomVector(int count, int minValue, int maxValue) {
    std::vector<int> result;
    for (int i = 0; i < count; i++) {
        int randomValue = minValue + rand() % (maxValue - minValue + 1);
        result.push_back(randomValue);
    }
    return result;
}

void printIntVector(const std::vector<int>& numbers, const std::string& title) {
    std::cout << title << ": ";
    for (size_t i = 0; i < numbers.size(); i++) {
        if (i > 0) {
            std::cout << ", ";
        }
        std::cout << numbers[i];
    }
    std::cout << std::endl;
}

void squarePrimeNumbers(std::vector<int>& numbers) {
    // Проходим по всем элементам, если число простое — возводим в квадрат
    for (size_t i = 0; i < numbers.size(); i++) {
        if (isPrime(numbers[i])) {
            numbers[i] = numbers[i] * numbers[i];
        }
    }
}

void sortOddAscEvenDesc(std::vector<int>& numbers) {
    // Сначала нечётные по возрастанию
    for (size_t i = 0; i < numbers.size(); i++) {
        for (size_t j = i + 1; j < numbers.size(); j++) {
            bool iOdd = (numbers[i] % 2 != 0);
            bool jOdd = (numbers[j] % 2 != 0);

            // Если слева чётное, а справа нечётное — меняем местами
            if (!iOdd && jOdd) {
                int temp = numbers[i];
                numbers[i] = numbers[j];
                numbers[j] = temp;
            }
            // Если оба нечётные и левое больше правого — меняем (возрастание)
            else if (iOdd && jOdd && numbers[i] > numbers[j]) {
                int temp = numbers[i];
                numbers[i] = numbers[j];
                numbers[j] = temp;
            }
            // Если оба чётные и левое меньше правого — меняем (убывание)
            else if (!iOdd && !jOdd && numbers[i] < numbers[j]) {
                int temp = numbers[i];
                numbers[i] = numbers[j];
                numbers[j] = temp;
            }
        }
    }
}

std::vector<int> filterUniqueInRange(const std::vector<int>& numbers, int low, int high) {
    std::vector<int> result;

    for (size_t i = 0; i < numbers.size(); i++) {
        int current = numbers[i];

        // Проверяем, что число в диапазоне
        if (current < low || current > high) {
            continue;
        }

        // Проверяем, что такого числа ещё нет в результате
        bool alreadyExists = false;
        for (size_t j = 0; j < result.size(); j++) {
            if (result[j] == current) {
                alreadyExists = true;
                break;
            }
        }

        if (!alreadyExists) {
            result.push_back(current);
        }
    }

    return result;
}

void runTaskThree() {
    std::cout << "\nЗадание 3\n" << std::endl;

    std::vector<int> baseArray;

    // Спрашиваем пользователя: сгенерировать массив или использовать готовый
    int sourceChoice = -1;
    while (sourceChoice != 1 && sourceChoice != 2) {
        std::cout << "1 - Использовать заранее заготовленный массив" << std::endl;
        std::cout << "2 - Сгенерировать случайный массив" << std::endl;
        std::cout << "Ваш выбор: ";

        if (!(std::cin >> sourceChoice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            sourceChoice = -1;
            continue;
        }

        if (sourceChoice != 1 && sourceChoice != 2) {
            std::cout << "Неверный ввод, попробуйте снова" << std::endl;
        }
    }

    if (sourceChoice == 1) {
        // Заранее заготовленный массив
        baseArray = { 12, 7, 5, 18, 3, 20, 11, 9, 4, 13, 6, 2, 15, 8, 17 };
        std::cout << "\nИспользуется заготовленный массив." << std::endl;
    }
    else {
        // Генерация случайного массива
        int count = 0;
        int minValue = 0;
        int maxValue = 0;

        std::cout << "Введите количество чисел: ";
        std::cin >> count;
        std::cout << "Введите минимальное значение: ";
        std::cin >> minValue;
        std::cout << "Введите максимальное значение: ";
        std::cin >> maxValue;

        baseArray = generateRandomVector(count, minValue, maxValue);
        std::cout << "\nСгенерирован случайный массив." << std::endl;
    }

    printIntVector(baseArray, "Исходный массив");

    // Задание 3а: возведение простых чисел в квадрат
    std::vector<int> arrayForTaskA = baseArray;
    squarePrimeNumbers(arrayForTaskA);
    std::cout << "\nЗадание 3а: простые числа в квадрате" << std::endl;
    printIntVector(arrayForTaskA, "Результат");

    // Задание 3б: сортировка: нечётные по возрастанию, чётные по убыванию
    std::vector<int> arrayForTaskB = baseArray;
    sortOddAscEvenDesc(arrayForTaskB);
    std::cout << "\nЗадание 3б: нечётные, чётные" << std::endl;
    printIntVector(arrayForTaskB, "Результат");

    // Задание 3в: уникальные числа в диапазоне
    int low, high;
    std::cout << "\nЗадание 3в: уникальные числа в диапазоне" << std::endl;
    std::cout << "Введите нижнюю границу диапазона: ";
    std::cin >> low;
    std::cout << "Введите верхнюю границу диапазона: ";
    std::cin >> high;

    std::vector<int> arrayForTaskC = filterUniqueInRange(baseArray, low, high);
    printIntVector(arrayForTaskC, "Уникальные числа в диапазоне");

    // === Сохранение результатов в файл result_task3.txt ===
    const std::string outputFileName = "result_task3.txt";
    FILE* outputPointer = fopen(outputFileName.c_str(), "w");
    if (!outputPointer) {
        std::cout << "Ошибка при создании файла " << outputFileName << std::endl;
        return;
    }

    fprintf(outputPointer, "Задание 3\n\n");

    // Исходный массив
    fprintf(outputPointer, "Исходный массив: ");
    for (size_t i = 0; i < baseArray.size(); i++) {
        if (i > 0) {
            fprintf(outputPointer, ", ");
        }
        fprintf(outputPointer, "%d", baseArray[i]);
    }
    fprintf(outputPointer, "\n\n");

    // Задание 3а
    fprintf(outputPointer, "Задание 3а: простые числа в квадрате\n");
    fprintf(outputPointer, "Результат: ");
    for (size_t i = 0; i < arrayForTaskA.size(); i++) {
        if (i > 0) {
            fprintf(outputPointer, ", ");
        }
        fprintf(outputPointer, "%d", arrayForTaskA[i]);
    }
    fprintf(outputPointer, "\n\n");

    // Задание 3б
    fprintf(outputPointer, "Задание 3б: нечётные по возрастанию, чётные по убыванию\n");
    fprintf(outputPointer, "Результат: ");
    for (size_t i = 0; i < arrayForTaskB.size(); i++) {
        if (i > 0) {
            fprintf(outputPointer, ", ");
        }
        fprintf(outputPointer, "%d", arrayForTaskB[i]);
    }
    fprintf(outputPointer, "\n\n");

    // Задание 3в
    fprintf(outputPointer, "Задание 3в: уникальные числа в диапазоне [%d; %d]\n", low, high);
    fprintf(outputPointer, "Результат: ");
    for (size_t i = 0; i < arrayForTaskC.size(); i++) {
        if (i > 0) {
            fprintf(outputPointer, ", ");
        }
        fprintf(outputPointer, "%d", arrayForTaskC[i]);
    }
    fprintf(outputPointer, "\n");

    fclose(outputPointer);
    std::cout << "\nРезультаты задания 3 сохранены в файл: " << outputFileName << std::endl;
}