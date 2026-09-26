#include "AutocompleteEngine.h"

#include <cassert>

int main() {
    autocomplete::AutocompleteEngine engine;
    engine.addWord("program");
    engine.addWord("project");
    engine.addWord("process");
    const auto initial = engine.autocomplete("PRO", 10);
    assert(initial.size() == 3);
    engine.recordUsage("project");
    const auto ranked = engine.autocomplete("pro", 2);
    assert(ranked.size() == 2);
    assert(ranked.front() == "project");
    assert(engine.autocomplete("missing", 5).empty());
    assert(engine.autocomplete("pro", -1).empty());
}
