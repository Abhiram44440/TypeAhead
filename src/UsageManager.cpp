#include "UsageManager.h"
#include "TextUtils.h"

#include <algorithm>

namespace autocomplete {

void UsageManager::addWord(const std::string& input, long long initialFrequency) {
    const std::string word = normalizeWord(input);
    if (word.empty()) return;
    auto [entry, inserted] = stats.emplace(word, WordStats{std::max(0LL, initialFrequency), 0});
    if (!inserted && initialFrequency > entry->second.frequency)
        entry->second.frequency = initialFrequency;
}

void UsageManager::recordUsage(const std::string& input) {
    const std::string word = normalizeWord(input);
    if (word.empty()) return;
    WordStats& wordStats = stats[word];
    ++usageCounter;
    ++wordStats.frequency;
    wordStats.lastUsed = usageCounter;
}

WordStats UsageManager::getStats(const std::string& input) const {
    const auto found = stats.find(normalizeWord(input));
    return found == stats.end() ? WordStats{} : found->second;
}
long long UsageManager::getFrequency(const std::string& word) const { return getStats(word).frequency; }
long long UsageManager::getLastUsed(const std::string& word) const { return getStats(word).lastUsed; }
long long UsageManager::getCurrentCounter() const { return usageCounter; }
const std::unordered_map<std::string, WordStats>& UsageManager::getAllStats() const { return stats; }

void UsageManager::setStats(const std::string& input, WordStats wordStats) {
    const std::string word = normalizeWord(input);
    if (word.empty() || wordStats.frequency < 0 || wordStats.lastUsed < 0) return;
    stats[word] = wordStats;
    usageCounter = std::max(usageCounter, wordStats.lastUsed);
}

}  // namespace autocomplete
