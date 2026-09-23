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

/// @brief Индексирует позиции каждого слова во всех указанных файлах и сохраняет результат.
/// @param fileList Список имён файлов для обработки.
/// @param outputFileName Имя файла для сохранения результатов.
void processWordIndexing(const std::vector<std::string>& fileList, const std::string& outputFileName);

/// @brief Проверяет, является ли число простым.
/// @param number Проверяемое число.
/// @return true если число простое, иначе false.
bool isPrime(int number);

/// @brief Генерирует вектор случайных чисел в заданном диапазоне.
/// @param count Количество чисел.
/// @param minValue Минимальное значение.
/// @param maxValue Максимальное значение.
/// @return Вектор со случайными числами.
std::vector<int> generateRandomVector(int count, int minValue, int maxValue);

/// @brief Выводит вектор целых чисел в консоль.
/// @param numbers Вектор для вывода.
/// @param title Заголовок перед выводом.
void printIntVector(const std::vector<int>& numbers, const std::string& title);

/// @brief Задание 3а: все простые числа в векторе возводит в квадрат.
/// @param numbers Вектор чисел (изменяется).
void squarePrimeNumbers(std::vector<int>& numbers);

/// @brief Задание 3б: сортирует вектор: сначала нечётные по возрастанию, затем чётные по убыванию.
/// @param numbers Вектор чисел (изменяется).
void sortOddAscEvenDesc(std::vector<int>& numbers);

/// @brief Задание 3в: возвращает уникальные числа из вектора, попадающие в диапазон [low, high].
/// @param numbers Исходный вектор.
/// @param low Нижняя граница диапазона.
/// @param high Верхняя граница диапазона.
/// @return Вектор уникальных чисел из диапазона.
std::vector<int> filterUniqueInRange(const std::vector<int>& numbers, int low, int high);

/// @brief Запускает третье задание (а, б, в) и сохраняет результаты в result_task3.txt.
void runTaskThree();

#endif