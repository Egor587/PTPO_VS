#pragma once
#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <string>
#include <map>

/// @brief Класс одного языка: хранит пары "ключ - перевод"
class Language {
private:
    std::string languageCode;
    std::map<std::string, std::string> translations;

public:
    /// @brief Конструктор по умолчанию
    Language();

    /// @brief Конструктор с кодом языка
    /// @param code Код языка ("ru", "en")
    Language(const std::string& code);

    /// @brief Конструктор копирования (демонстрация требования "ознакомиться с конструктором копирования")
    /// @param other Копируемый объект
    Language(const Language& other);

    /// @brief Возвращает перевод по ключу или сам ключ, если перевода нет
    /// @param key Ключ
    /// @return Перевод
    std::string get(const std::string& key) const;

    /// @brief Возвращает код языка
    std::string getCode() const;

    /// @brief Добавляет перевод
    /// @param key Ключ
    /// @param value Значение
    void set(const std::string& key, const std::string& value);
};

#endif