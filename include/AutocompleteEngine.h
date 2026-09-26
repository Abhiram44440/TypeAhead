#pragma once

#include "FileManager.h"
#include "RankingEngine.h"
#include "Trie.h"
#include "UsageManager.h"

#include <string>
#include <vector>

namespace autocomplete {
class AutocompleteEngine {
public:
    void addWord(const std::string& word, long long frequency = 0);
    std::vector<std::string> autocomplete(const std::string& prefix, int k) const;
    void recordUsage(const std::string& word);
    bool loadData(const std::string& wordsFile, const std::string& usageFile);
    bool saveData(const std::string& usageFile) const;

private:
    Trie trie;
    UsageManager usageManager;
    RankingEngine rankingEngine;
    FileManager fileManager;
};
}  // namespace autocomplete
