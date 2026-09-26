#include "AutocompleteEngine.h"
#include "TextUtils.h"

namespace autocomplete {

void AutocompleteEngine::addWord(const std::string& input, long long frequency) {
    const std::string word = normalizeWord(input);
    if (word.empty()) return;
    trie.insert(word);
    usageManager.addWord(word, frequency);
}

std::vector<std::string> AutocompleteEngine::autocomplete(const std::string& prefix, int k) const {
    return rankingEngine.getTopK(trie.getWordsWithPrefix(normalizeWord(prefix)), usageManager, k);
}

void AutocompleteEngine::recordUsage(const std::string& input) {
    const std::string word = normalizeWord(input);
    if (word.empty() || !trie.search(word)) return;
    usageManager.recordUsage(word);
}

bool AutocompleteEngine::loadData(const std::string& wordsFile, const std::string& usageFile) {
    if (!fileManager.loadWords(wordsFile, trie)) return false;

    // Give every dictionary entry explicit zero-valued statistics before
    // restoring only the entries that have persisted usage.
    for (const auto& word : trie.getWordsWithPrefix("")) usageManager.addWord(word);

    // A first launch has no usage file yet; that is a valid startup state.
    fileManager.loadUsage(usageFile, usageManager);
    return true;
}

bool AutocompleteEngine::saveData(const std::string& usageFile) const {
    return fileManager.saveUsage(usageFile, usageManager);
}
}  // namespace autocomplete
