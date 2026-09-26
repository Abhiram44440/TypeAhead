#include "Trie.h"

#include <algorithm>
#include <cassert>
#include <vector>

int main() {
    autocomplete::Trie trie;
    trie.insert("Apple");
    trie.insert("application");
    trie.insert("apply");
    trie.insert("apple");
    assert(trie.search("APPLE"));
    assert(!trie.search("app"));
    assert(trie.startsWith("App"));
    assert(!trie.startsWith("banana"));
    const auto all = trie.getWordsWithPrefix("");
    assert(all.size() == 3);
    auto matches = trie.getWordsWithPrefix("app");
    std::sort(matches.begin(), matches.end());
    assert((matches == std::vector<std::string>{"apple", "application", "apply"}));
}
