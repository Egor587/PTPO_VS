#pragma once
#ifndef RESOURCES_H
#define RESOURCES_H

#include <string>
#include "Language.h"

/// @brief Статический класс для работы с переводами
class Resources {
private:
    static Language currentLanguage;
    static bool initialized;

public:
    /// @brief Загружает ресурсы из JSON-файла и выбирает язык по системной локали
    /// @param resourcesFileName Имя файла ресурсов
    static void loadFromFile(const std::string& resourcesFileName);

    /// @brief Возвращает перевод по ключу
    /// @param key Ключ перевода
    /// @return Строка перевода
    static std::string get(const std::string& key);

    /// @brief Возвращает текущий код языка
    /// @return Код языка
    static std::string getCurrentLanguageCode();

    /// @brief Определяет код языка системы через user32.dll (Windows.h)
    /// @return "ru" или "en"
    static std::string detectSystemLanguage();
};

#endif