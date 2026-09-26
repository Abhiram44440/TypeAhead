#include "RankingEngine.h"
#include "Trie.h"
#include "UsageManager.h"

#include <chrono>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    const int count = argc > 1 ? std::stoi(argv[1]) : 100000;
    autocomplete::Trie trie;
    autocomplete::UsageManager usage;
    autocomplete::RankingEngine ranking;
    const auto constructionStart = std::chrono::steady_clock::now();
    for (int i = 0; i < count; ++i) {
        const std::string word = "word" + std::to_string(i);
        trie.insert(word);
        usage.addWord(word);
    }
    const auto constructionEnd = std::chrono::steady_clock::now();
    const auto lookupStart = std::chrono::steady_clock::now();
    const bool prefixExists = trie.startsWith("word");
    const auto lookupEnd = std::chrono::steady_clock::now();
    const auto collectionStart = std::chrono::steady_clock::now();
    const auto candidates = trie.getWordsWithPrefix("word");
    const auto collectionEnd = std::chrono::steady_clock::now();
    for (const int k : {5, 10, 20}) {
        const auto rankingStart = std::chrono::steady_clock::now();
        const auto results = ranking.getTopK(candidates, usage, k);
        const auto rankingEnd = std::chrono::steady_clock::now();
        std::cout << "K=" << k << ", results=" << results.size()
                  << ", ranking (us)=" << std::chrono::duration_cast<std::chrono::microseconds>(rankingEnd - rankingStart).count()
                  << ", total autocomplete (us)=" << std::chrono::duration_cast<std::chrono::microseconds>(rankingEnd - lookupStart).count() << '\n';
    }
    std::cout << "Trie construction time (ms)="
              << std::chrono::duration_cast<std::chrono::milliseconds>(constructionEnd - constructionStart).count() << '\n';
    std::cout << "Prefix lookup time (us)="
              << std::chrono::duration_cast<std::chrono::microseconds>(lookupEnd - lookupStart).count() << '\n';
    std::cout << "Candidate collection time (us)="
              << std::chrono::duration_cast<std::chrono::microseconds>(collectionEnd - collectionStart).count()
              << ", prefix found=" << std::boolalpha << prefixExists << '\n';
}
