#pragma once
#ifndef CLI_H
#define CLI_H

#include <string>

/// @brief Структура параметров командной строки
struct CliOptions {
    bool hasMode;           ///< Был ли передан --mode
    int mode;               ///< Номер пункта меню
    bool hasFile;           ///< Был ли --file
    std::string file;       ///< Путь к файлу
    bool hasResult;         ///< Был ли --result
    std::string result;     ///< Путь к выходному файлу
    bool hasHelp;           ///< Был ли --help

    CliOptions();
};

/// @brief Разбирает аргументы командной строки
/// @param argc Количество аргументов
/// @param argv Массив аргументов
/// @return Структура с параметрами
CliOptions parseCommandLine(int argc, char* argv[]);

/// @brief Печатает справку по использованию
void printCliHelp();

#endif