#include "AutocompleteEngine.h"

#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace {
bool parseSelection(const std::string& input, int& selection) {
    try {
        std::size_t consumed = 0;
        const int parsed = std::stoi(input, &consumed);
        if (consumed != input.size()) return false;
        selection = parsed;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}
}

int main() {
    using autocomplete::AutocompleteEngine;
    constexpr const char* wordsFile = "data/words.txt";
    constexpr const char* usageFile = "data/usage.txt";
    constexpr int topK = 5;
    AutocompleteEngine engine;
    if (!engine.loadData(wordsFile, usageFile)) {
        std::cerr << "Unable to load dictionary: " << wordsFile << '\n';
        return 1;
    }

    std::cout << "========================================\n"
              << "        AUTOCOMPLETE ENGINE\n"
              << "========================================\n"
              << "Enter a prefix; choose a result to record usage. Type 'exit' to quit.\n";

    std::string pendingInput;
    while (true) {
        std::cout << "\nEnter prefix (or 'exit' to quit):\n> ";
        if (!pendingInput.empty()) {
            std::cout << pendingInput << '\n';
        } else if (!std::getline(std::cin, pendingInput)) {
            engine.saveData(usageFile);
            return 0;
        }
        if (pendingInput == "exit") break;
        const std::vector<std::string> results = engine.autocomplete(pendingInput, topK);
        pendingInput.clear();
        if (results.empty()) {
            std::cout << "No suggestions found.\n";
            continue;
        }
        std::cout << "\nSuggestions:\n";
        for (std::size_t i = 0; i < results.size(); ++i)
            std::cout << i + 1 << ". " << results[i] << '\n';

        std::cout << "Select [1-" << results.size() << "] or enter another prefix:\n> ";
        std::string selectionInput;
        if (!std::getline(std::cin, selectionInput)) break;
        int selection = 0;
        if (parseSelection(selectionInput, selection)) {
            if (selection < 1 || selection > static_cast<int>(results.size())) {
                std::cout << "Invalid selection.\n";
                continue;
            }
            const std::string& selectedWord = results[static_cast<std::size_t>(selection - 1)];
            engine.recordUsage(selectedWord);
            if (!engine.saveData(usageFile)) std::cerr << "Warning: usage could not be saved.\n";
            std::cout << "Selected: " << selectedWord << "\nUsage updated automatically.\n";
        } else {
            pendingInput = selectionInput;
        }
    }

    if (!engine.saveData(usageFile)) std::cerr << "Warning: usage could not be saved.\n";
    std::cout << "Goodbye.\n";
    return 0;
}
