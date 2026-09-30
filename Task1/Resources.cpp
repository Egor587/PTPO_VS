#define _CRT_SECURE_NO_WARNINGS

#include "Resources.h"
#include "json.hpp"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <windows.h>

using json = nlohmann::json;

Language Resources::currentLanguage;
bool Resources::initialized = false;

std::string Resources::detectSystemLanguage() {
    //Текущая раскладка клавиатуры
    HKL layout = GetKeyboardLayout(0);

    // Достаём код языка из текущей раскладки
    LANGID langId = LOWORD(reinterpret_cast<DWORD_PTR>(layout));
    WORD primaryLang = PRIMARYLANGID(langId);

    // 0x19 это код русского языка в Windows
    if (primaryLang == LANG_RUSSIAN) {
        return "ru";
    }
    return "en";
}

void Resources::loadFromFile(const std::string& resourcesFileName) {
    FILE* filePointer = fopen(resourcesFileName.c_str(), "rb");
    if (!filePointer) {
        std::cout << "Warning: cannot open resources file: " << resourcesFileName << std::endl;
        std::cout << "Falling back to English." << std::endl;
        currentLanguage = Language("en");
        initialized = true;
        return;
    }

    // Чтение файла целиком
    fseek(filePointer, 0, SEEK_END);
    long fileSize = ftell(filePointer);
    fseek(filePointer, 0, SEEK_SET);

    std::string buffer;
    if (fileSize > 0) {
        buffer.resize(fileSize);
        fread(&buffer[0], 1, fileSize, filePointer);
    }
    fclose(filePointer);

    json parsed;
    try {
        parsed = json::parse(buffer);
    }
    catch (...) {
        std::cout << "Warning: resources file is not valid JSON. Falling back to English." << std::endl;
        currentLanguage = Language("en");
        initialized = true;
        return;
    }

    std::string systemLang = detectSystemLanguage();

    // если в JSON есть этот язык,то берём его, иначе "en".
    if (parsed.contains(systemLang)) {
        currentLanguage = Language(systemLang);
        for (json::iterator it = parsed[systemLang].begin(); it != parsed[systemLang].end(); ++it) {
            currentLanguage.set(it.key(), it.value().get<std::string>());
        }
    }
    else if (parsed.contains("en")) {
        currentLanguage = Language("en");
        for (json::iterator it = parsed["en"].begin(); it != parsed["en"].end(); ++it) {
            currentLanguage.set(it.key(), it.value().get<std::string>());
        }
    }
    else {
        currentLanguage = Language("en");
    }

    initialized = true;
}

std::string Resources::get(const std::string& key) {
    if (!initialized) {
        return key;
    }
    return currentLanguage.get(key);
}

std::string Resources::getCurrentLanguageCode() {
    return currentLanguage.getCode();
}