#include "FileManager.h"
#include "TextUtils.h"
#include "Trie.h"
#include "UsageManager.h"

#include <fstream>
#include <sstream>

namespace autocomplete {

bool FileManager::loadWords(const std::string& filename, Trie& trie) const {
    std::ifstream input(filename);
    if (!input) return false;
    std::string line;
    while (std::getline(input, line)) {
        const std::string word = normalizeWord(line);
        if (!word.empty()) trie.insert(word);
    }
    return true;
}

bool FileManager::loadUsage(const std::string& filename, UsageManager& usageManager) const {
    std::ifstream input(filename);
    if (!input) return false;
    std::string line;
    while (std::getline(input, line)) {
        std::istringstream parser(line);
        std::string word;
        long long frequency = 0;
        long long lastUsed = 0;
        std::string extra;
        if (!(parser >> word >> frequency >> lastUsed) || parser >> extra ||
            word.empty() || frequency < 0 || lastUsed < 0) continue;
        usageManager.setStats(word, WordStats{frequency, lastUsed});
    }
    return true;
}

bool FileManager::saveUsage(const std::string& filename, const UsageManager& usageManager) const {
    std::ofstream output(filename, std::ios::trunc);
    if (!output) return false;
    for (const auto& [word, wordStats] : usageManager.getAllStats())
        output << word << ' ' << wordStats.frequency << ' ' << wordStats.lastUsed << '\n';
    return static_cast<bool>(output);
}
}  // namespace autocomplete
