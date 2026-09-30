#define _CRT_SECURE_NO_WARNINGS

#include <chrono>
#include "Resources.h"
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

std::string readFileInChunks(const std::string& inputFileName, size_t chunkSize) {
    FILE* filePointer = fopen(inputFileName.c_str(), "rb");
    if (!filePointer) {
        std::cout << Resources::get("CantOpenFile") << inputFileName << std::endl;
        return "";
    }

    std::string totalContent;
    totalContent.reserve(chunkSize);

    std::vector<char> buffer(chunkSize);

    while (true) {
        size_t bytesRead = fread(buffer.data(), 1, chunkSize, filePointer);
        if (bytesRead == 0) {
            break;
        }
        totalContent.append(buffer.data(), bytesRead);
    }

    fclose(filePointer);
    return totalContent;
}

void processWordCountingWithTiming(const std::vector<std::string>& fileList, const std::string& outputFileName) {
    std::cout << Resources::get("Processing") << std::endl;

    std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();

    std::map<std::string, int> wordFrequencyMap;

    for (size_t fileIndex = 0; fileIndex < fileList.size(); ++fileIndex) {
        std::string fileData = readFileInChunks(fileList[fileIndex], 4 * 1024 * 1024);
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
        std::cout << "Error: cannot create output file" << std::endl;
        return;
    }

    for (std::map<std::string, int>::iterator it = wordFrequencyMap.begin(); it != wordFrequencyMap.end(); ++it) {
        fprintf(outputPointer, "%s - %d\n", it->first.c_str(), it->second);
    }

    fclose(outputPointer);

    std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
    long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    std::cout << Resources::get("SavedTo") << outputFileName << std::endl;
    std::cout << Resources::get("TimeElapsed") << ms << std::endl;
}

void printConsoleMenu() {
    std::cout << Resources::get("MenuTitle") << std::endl;
    std::cout << Resources::get("Punkt1") << std::endl;
    std::cout << Resources::get("Punkt2") << std::endl;
    std::cout << Resources::get("Punkt3") << std::endl;
    std::cout << Resources::get("Exit") << std::endl;
    std::cout << Resources::get("Choose");
}

void processWordIndexingWithTiming(const std::vector<std::string>& fileList, const std::string& outputFileName) {
    std::cout << Resources::get("Indexing") << std::endl;

    std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();

    std::map<std::string, std::vector<int>> wordPositionsMap;

    for (size_t fileIndex = 0; fileIndex < fileList.size(); ++fileIndex) {
        std::string fileData = readFileInChunks(fileList[fileIndex], 4 * 1024 * 1024);
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
        std::cout << "Error: cannot create output file" << std::endl;
        return;
    }

    for (std::map<std::string, std::vector<int>>::iterator it = wordPositionsMap.begin(); it != wordPositionsMap.end(); ++it) {
        fprintf(outputPointer, "%s - ", it->first.c_str());
        for (size_t positionIndex = 0; positionIndex < it->second.size(); ++positionIndex) {
            if (positionIndex > 0) {
                fprintf(outputPointer, ", ");
            }
            fprintf(outputPointer, "%d", it->second[positionIndex]);
        }
        fprintf(outputPointer, "\n");
    }

    fclose(outputPointer);

    std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
    long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    std::cout << Resources::get("SavedTo") << outputFileName << std::endl;
    std::cout << Resources::get("TimeElapsed") << ms << std::endl;
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
    //Задание 3а
    std::transform(numbers.begin(), numbers.end(), numbers.begin(),
        [](int value) {
            if (isPrime(value)) {
                return value * value;
            }
            return value;
        });
}

void sortOddAscEvenDesc(std::vector<int>& numbers) {
    //Задание 3б
    std::sort(numbers.begin(), numbers.end(),
        [](int leftValue, int rightValue) {
            bool leftIsOdd = (leftValue % 2 != 0);
            bool rightIsOdd = (rightValue % 2 != 0);

            // Нечётные всегда идут раньше чётных
            if (leftIsOdd != rightIsOdd) {
                return leftIsOdd;
            }
            // Оба нечётные — по возрастанию
            if (leftIsOdd) {
                return leftValue < rightValue;
            }
            // Оба чётные — по убыванию
            return leftValue > rightValue;
        });
}

std::vector<int> filterUniqueInRange(const std::vector<int>& numbers, int low, int high) {
    //Задание 3в

    //Отбираем элементы, попадающие в диапазон
    std::vector<int> filtered;
    std::copy_if(numbers.begin(), numbers.end(), std::back_inserter(filtered),
        [low, high](int value) {
            return value >= low && value <= high;
        });

    //Сортировка
    std::sort(filtered.begin(), filtered.end());

    //Удаление подряд идущих дубликатов
    std::vector<int>::iterator lastUnique = std::unique(filtered.begin(), filtered.end());

    filtered.erase(lastUnique, filtered.end());

    return filtered;
}

void runTaskThree() {
    std::cout << "\n" << Resources::get("Task3Title") << "\n" << std::endl;

    std::vector<int> baseArray;

    int sourceChoice = -1;
    while (sourceChoice != 1 && sourceChoice != 2) {
        std::cout << Resources::get("SourceChoice") << std::endl;
        std::cout << Resources::get("YourChoice");

        if (!(std::cin >> sourceChoice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            sourceChoice = -1;
            continue;
        }

        if (sourceChoice != 1 && sourceChoice != 2) {
            std::cout << Resources::get("InvalidInput") << std::endl;
        }
    }

    if (sourceChoice == 1) {
        baseArray = { 12, 7, 5, 18, 3, 20, 11, 9, 4, 13, 6, 2, 15, 8, 17 };
    }
    else {
        int count = 0;
        int minValue = 0;
        int maxValue = 0;

        std::cout << Resources::get("Count");
        std::cin >> count;
        std::cout << Resources::get("MinValue");
        std::cin >> minValue;
        std::cout << Resources::get("MaxValue");
        std::cin >> maxValue;

        baseArray = generateRandomVector(count, minValue, maxValue);
    }

    printIntVector(baseArray, Resources::get("SourceArray"));

    std::vector<int> arrayForTaskA = baseArray;
    squarePrimeNumbers(arrayForTaskA);
    std::cout << "\n" << Resources::get("Task3A") << std::endl;
    printIntVector(arrayForTaskA, Resources::get("Result"));

    std::vector<int> arrayForTaskB = baseArray;
    sortOddAscEvenDesc(arrayForTaskB);
    std::cout << "\n" << Resources::get("Task3B") << std::endl;
    printIntVector(arrayForTaskB, Resources::get("Result"));

    int low, high;
    std::cout << "\n" << Resources::get("Task3C") << std::endl;
    std::cout << Resources::get("LowBound");
    std::cin >> low;
    std::cout << Resources::get("HighBound");
    std::cin >> high;

    std::vector<int> arrayForTaskC = filterUniqueInRange(baseArray, low, high);
    printIntVector(arrayForTaskC, Resources::get("Result"));

    const std::string outputFileName = "result_task3.txt";
    FILE* outputPointer = fopen(outputFileName.c_str(), "w");
    if (!outputPointer) {
        std::cout << "Error: cannot create file" << std::endl;
        return;
    }

    fprintf(outputPointer, "Task 3\n\n");
    fprintf(outputPointer, "Source array: ");
    for (size_t i = 0; i < baseArray.size(); i++) {
        if (i > 0) fprintf(outputPointer, ", ");
        fprintf(outputPointer, "%d", baseArray[i]);
    }
    fprintf(outputPointer, "\n\n");

    fprintf(outputPointer, "Task 3a (primes squared): ");
    for (size_t i = 0; i < arrayForTaskA.size(); i++) {
        if (i > 0) fprintf(outputPointer, ", ");
        fprintf(outputPointer, "%d", arrayForTaskA[i]);
    }
    fprintf(outputPointer, "\n\n");

    fprintf(outputPointer, "Task 3b (odd asc, even desc): ");
    for (size_t i = 0; i < arrayForTaskB.size(); i++) {
        if (i > 0) fprintf(outputPointer, ", ");
        fprintf(outputPointer, "%d", arrayForTaskB[i]);
    }
    fprintf(outputPointer, "\n\n");

    fprintf(outputPointer, "Task 3c (range [%d; %d]): ", low, high);
    for (size_t i = 0; i < arrayForTaskC.size(); i++) {
        if (i > 0) fprintf(outputPointer, ", ");
        fprintf(outputPointer, "%d", arrayForTaskC[i]);
    }
    fprintf(outputPointer, "\n");

    fclose(outputPointer);
    std::cout << "\n" << Resources::get("SavedTo") << outputFileName << std::endl;
}