#include "Language.h"

Language::Language() : languageCode("en") {
}

Language::Language(const std::string& code) : languageCode(code) {
}

Language::Language(const Language& other) {
    languageCode = other.languageCode;
    translations = other.translations;
}

std::string Language::get(const std::string& key) const {
    std::map<std::string, std::string>::const_iterator it = translations.find(key);
    if (it != translations.end()) {
        return it->second;
    }
    return key;
}

std::string Language::getCode() const {
    return languageCode;
}

void Language::set(const std::string& key, const std::string& value) {
    translations[key] = value;
}