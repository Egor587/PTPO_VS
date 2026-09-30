#pragma once
#ifndef SYNC_H
#define SYNC_H

#include <string>
#include <windows.h>

/// @brief Класс синхронизации между процессами через именованный мьютекс
class Sync {
private:
    HANDLE mutexHandle;
    std::string mutexName;

public:
    /// @brief Создаёт мьютекс с заданным именем
    /// @param name Имя мьютекса
    Sync(const std::string& name);

    /// @brief Деструктор: освобождает и закрывает мьютекс
    ~Sync();

    /// @brief Ожидает освобождения мьютекса (блокирующий вызов)
    void lock();

    /// @brief Освобождает мьютекс
    void unlock();
};

#endif