#pragma once

#include <string>

namespace autocomplete {
class Trie;
class UsageManager;

class FileManager {
public:
    bool loadWords(const std::string& filename, Trie& trie) const;
    bool loadUsage(const std::string& filename, UsageManager& usageManager) const;
    bool saveUsage(const std::string& filename, const UsageManager& usageManager) const;
};
}  // namespace autocomplete
