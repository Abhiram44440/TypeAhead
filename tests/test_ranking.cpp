#include "RankingEngine.h"
#include "UsageManager.h"

#include <cassert>
#include <vector>

int main() {
    autocomplete::UsageManager usage;
    usage.recordUsage("unknown");
    assert(usage.getFrequency("unknown") == 1);
    assert(usage.getLastUsed("unknown") == 1);
    assert(usage.getCurrentCounter() == 1);
    usage.addWord("old-popular", 100);
    usage.addWord("recent", 1);
    usage.addWord("both", 100);
    usage.addWord("unused");
    usage.recordUsage("old-popular");
    for (int i = 0; i < 20; ++i) usage.recordUsage("recent");
    usage.recordUsage("both");
    autocomplete::RankingEngine ranking;
    const std::vector<std::string> candidates{"old-popular", "recent", "both", "unused"};
    const auto result = ranking.getTopK(candidates, usage, 4);
    assert(result.size() == 4);
    assert(result.front() == "both"); // high frequency and most recent
    assert(result.back() == "unused");
    assert(ranking.getTopK(candidates, usage, 0).empty());
}
