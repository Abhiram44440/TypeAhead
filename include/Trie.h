#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace autocomplete {

struct TrieNode {
    std::unordered_map<char, std::unique_ptr<TrieNode>> children;
    bool isEnd = false;
    std::string word;
};

class Trie {
public:
    Trie();
    void insert(const std::string& word);
    bool search(const std::string& word) const;
    bool startsWith(const std::string& prefix) const;
    std::vector<std::string> getWordsWithPrefix(const std::string& prefix) const;

private:
    std::unique_ptr<TrieNode> root;
    const TrieNode* findNode(const std::string& value) const;
    void collectWords(const TrieNode* node, std::vector<std::string>& result) const;
};

}  // namespace autocomplete
