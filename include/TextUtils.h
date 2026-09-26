#pragma once

#include <algorithm>
#include <cctype>
#include <string>

namespace autocomplete {

inline std::string normalizeWord(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char character) {
        return static_cast<char>(std::tolower(character));
    });
    return value;
}

}  // namespace autocomplete
