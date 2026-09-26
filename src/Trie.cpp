#include "Trie.h"
#include "TextUtils.h"

namespace autocomplete {

Trie::Trie() : root(std::make_unique<TrieNode>()) {}

void Trie::insert(const std::string& input) {
    const std::string word = normalizeWord(input);
    if (word.empty()) return;
    TrieNode* current = root.get();
    for (char character : word) {
        auto& child = current->children[character];
        if (!child) child = std::make_unique<TrieNode>();
        current = child.get();
    }
    current->isEnd = true;
    current->word = word;
}

const TrieNode* Trie::findNode(const std::string& input) const {
    const TrieNode* current = root.get();
    for (char character : normalizeWord(input)) {
        const auto found = current->children.find(character);
        if (found == current->children.end()) return nullptr;
        current = found->second.get();
    }
    return current;
}

bool Trie::search(const std::string& word) const {
    const TrieNode* node = findNode(word);
    return node != nullptr && node->isEnd;
}

bool Trie::startsWith(const std::string& prefix) const { return findNode(prefix) != nullptr; }

void Trie::collectWords(const TrieNode* node, std::vector<std::string>& result) const {
    if (node->isEnd) result.push_back(node->word);
    for (const auto& [character, child] : node->children) collectWords(child.get(), result);
}

std::vector<std::string> Trie::getWordsWithPrefix(const std::string& prefix) const {
    std::vector<std::string> result;
    const TrieNode* node = findNode(prefix);
    if (node) collectWords(node, result);
    return result;
}

}  // namespace autocomplete
