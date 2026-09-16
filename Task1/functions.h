#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include <vector>
#include <map>

/// @brief Подготавливает текст: переводит буквы в нижний регистр и заменяет любую пунктуацию и тире на пробелы.
/// @param rawText Исходный текст из файла.
/// @return Очищенный текст в нижнем регистре.
std::string sanitizeAndNormalizeUTF8(const std::string& rawText);

/// @brief Считывает содержимое файла в строку с использованием C-style I/O без fstream.
/// @param inputFileName Имя файла для чтения.
/// @return Строка с полным содержимым файла.
std::string readFileContent(const std::string& inputFileName);

/// @brief Подсчитывает количество вхождений каждого слова во всех указанных файлах и сохраняет результат.
/// @param fileList Список имён файлов для обработки.
/// @param outputFileName Имя файла для сохранения результатов.
void processWordCounting(const std::vector<std::string>& fileList, const std::string& outputFileName);

/// @brief Выводит главное меню программы в консоль.
void printConsoleMenu();

#endif 