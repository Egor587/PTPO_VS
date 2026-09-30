#include "Sync.h"
#include "Resources.h"
#include <iostream>

Sync::Sync(const std::string& name) : mutexName(name), mutexHandle(NULL) {
    mutexHandle = CreateMutexA(NULL, FALSE, mutexName.c_str());
    if (mutexHandle == NULL) {
        std::cout << "Warning: cannot create mutex." << std::endl;
    }
}

Sync::~Sync() {
    if (mutexHandle != NULL) {
        CloseHandle(mutexHandle);
        mutexHandle = NULL;
    }
}

void Sync::lock() {
    if (mutexHandle == NULL) {
        return;
    }
    DWORD waitResult = WaitForSingleObject(mutexHandle, INFINITE);
    if (waitResult == WAIT_OBJECT_0) {
    }
    else {
        std::cout << "Warning: wait for mutex failed." << std::endl;
    }
}

void Sync::unlock() {
    if (mutexHandle != NULL) {
        ReleaseMutex(mutexHandle);
    }
}