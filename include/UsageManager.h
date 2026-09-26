#pragma once

#include <string>
#include <unordered_map>

namespace autocomplete {

struct WordStats {
    long long frequency = 0;
    long long lastUsed = 0;
};

class UsageManager {
public:
    void addWord(const std::string& word, long long initialFrequency = 0);
    void recordUsage(const std::string& word);
    WordStats getStats(const std::string& word) const;
    long long getFrequency(const std::string& word) const;
    long long getLastUsed(const std::string& word) const;
    long long getCurrentCounter() const;
    const std::unordered_map<std::string, WordStats>& getAllStats() const;

    // Used only by persistence to restore a previously saved deterministic clock.
    void setStats(const std::string& word, WordStats wordStats);

private:
    std::unordered_map<std::string, WordStats> stats;
    long long usageCounter = 0;
};

}  // namespace autocomplete
