#include "RankingEngine.h"
#include "UsageManager.h"

#include <algorithm>
#include <cmath>
#include <queue>

namespace autocomplete {
namespace {
struct ScoredCandidate { std::string word; double score; long long frequency; long long lastUsed; };

// Returns true when left must appear before right in the final result.
bool isBetter(const ScoredCandidate& left, const ScoredCandidate& right) {
    constexpr double epsilon = 1e-12;
    if (std::abs(left.score - right.score) > epsilon) return left.score > right.score;
    if (left.frequency != right.frequency) return left.frequency > right.frequency;
    if (left.lastUsed != right.lastUsed) return left.lastUsed > right.lastUsed;
    return left.word < right.word;
}
struct BetterFirst { bool operator()(const ScoredCandidate& a, const ScoredCandidate& b) const { return isBetter(a, b); } };
}

RankingEngine::RankingEngine(double alphaValue, double betaValue, double lambdaValue)
    : alpha(alphaValue), beta(betaValue), lambda(lambdaValue) {}

double RankingEngine::calculateScore(const std::string& word, const UsageManager& usageManager,
                                     long long maxFrequency) const {
    const WordStats stats = usageManager.getStats(word);
    const double frequencyScore = maxFrequency > 0
        ? std::log1p(static_cast<double>(stats.frequency)) / std::log1p(static_cast<double>(maxFrequency)) : 0.0;
    const double recencyScore = stats.lastUsed > 0
        ? std::exp(-lambda * static_cast<double>(usageManager.getCurrentCounter() - stats.lastUsed)) : 0.0;
    return alpha * frequencyScore + beta * recencyScore;
}

std::vector<std::string> RankingEngine::getTopK(const std::vector<std::string>& candidates,
                                                  const UsageManager& usageManager, int k) const {
    if (k <= 0) return {};
    long long maxFrequency = 0;
    for (const auto& word : candidates) maxFrequency = std::max(maxFrequency, usageManager.getFrequency(word));
    std::priority_queue<ScoredCandidate, std::vector<ScoredCandidate>, BetterFirst> heap;
    for (const auto& word : candidates) {
        const WordStats stats = usageManager.getStats(word);
        ScoredCandidate candidate{word, calculateScore(word, usageManager, maxFrequency), stats.frequency, stats.lastUsed};
        if (static_cast<int>(heap.size()) < k) heap.push(candidate);
        else if (isBetter(candidate, heap.top())) { heap.pop(); heap.push(candidate); }
    }
    std::vector<ScoredCandidate> ordered;
    while (!heap.empty()) { ordered.push_back(heap.top()); heap.pop(); }
    std::sort(ordered.begin(), ordered.end(), isBetter);
    std::vector<std::string> result;
    for (const auto& candidate : ordered) result.push_back(candidate.word);
    return result;
}
}  // namespace autocomplete
