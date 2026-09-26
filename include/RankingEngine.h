#pragma once

#include <string>
#include <vector>

namespace autocomplete {

class UsageManager;

struct RankedWord {
    std::string word;
    double score = 0.0;
};

class RankingEngine {
public:
    RankingEngine(double alpha = 0.6, double beta = 0.4, double lambda = 0.1);
    std::vector<std::string> getTopK(const std::vector<std::string>& candidates,
                                     const UsageManager& usageManager, int k) const;

private:
    double alpha;
    double beta;
    double lambda;
    double calculateScore(const std::string& word, const UsageManager& usageManager,
                          long long maxFrequency) const;
};

}  // namespace autocomplete
