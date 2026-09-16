#define _CRT_SECURE_NO_WARNINGS

#include "functions.h"
#include <cstdio>
#include <iostream>
#include <sstream>
#include <clocale>
#include <cctype>

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
    std::cout << "Успешно! Результаты подсчёта сохранены в файл: " << outputFileName << std::endl;
}

void printConsoleMenu() {
    std::cout << "Лабораторная работа 1" << std::endl;
    std::cout << "1 - Посчитать все слова во всех томах" << std::endl;
    std::cout << "2 - Индексация позиций слов во всех томах" << std::endl;
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
    std::cout << "Успешно! Индексация позиций сохранена в файл: " << outputFileName << std::endl;
}