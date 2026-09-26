# Autocomplete Engine: Project Flow

This document explains how the autocomplete engine loads data, finds suggestions, ranks them, and learns from selections.

## High-level architecture

```text
data/words.txt ──> FileManager ──> Trie
                                      │
CLI / Browser UI ──> AutocompleteEngine ──> RankingEngine ──> Top-K suggestions
                                      │              ▲
                                      │              │
                                      ▼              │
                                  UsageManager ──────┘
                                      │
                                      ▼
                                data/usage.txt
```

## 1. Startup flow

```text
Application starts
      │
      ▼
Load data/words.txt
      │
      ▼
Insert every word into the Trie
      │
      ▼
Create zero-valued usage statistics for every dictionary word
      │
      ▼
Load saved frequency and last-used values from data/usage.txt
      │
      ▼
Application is ready for autocomplete requests
```

`data/words.txt` contains one dictionary word per line. `data/usage.txt` stores one record per word in this format:

```text
word frequency lastUsed
```

For example:

```text
project 12 88
```

This means `project` has been selected 12 times, and its most recent selection was at usage-counter position 88.

## 2. Autocomplete query flow

When the user enters a prefix such as `pro`:

```text
User enters "pro"
      │
      ▼
AutocompleteEngine::autocomplete("pro", K)
      │
      ▼
Trie finds the node for "pro"
      │
      ▼
Trie collects every complete word below that node
      │
      ▼
Candidates: program, project, process, production, ...
      │
      ▼
RankingEngine scores each candidate
      │
      ▼
Bounded priority queue retains the best K candidates
      │
      ▼
Sorted ranked suggestions are returned to the CLI or browser
```

The Trie stores words character by character. A shared prefix uses shared Trie nodes, so words such as `program`, `project`, and `process` all share the path for `pro`.

## 3. Ranking flow

For each candidate, the ranking engine gets its frequency and recency from `UsageManager`.

```text
frequencyScore = log(1 + frequency) / log(1 + maxCandidateFrequency)

recencyScore = exp(-lambda * (currentUsageCounter - lastUsed))

finalScore = alpha * frequencyScore + beta * recencyScore
```

Default ranking parameters:

```text
alpha  = 0.6  (frequency weight)
beta   = 0.4  (recency weight)
lambda = 0.1  (recency decay rate)
```

Interpretation:

- Frequently selected words receive a higher frequency score.
- Recently selected words receive a higher recency score.
- A word that is both popular and recent generally ranks highest.
- A never-selected word has a recency score of zero.

### Deterministic tie-breaking

When two candidates have the same final score, the engine ranks them by:

1. Higher frequency
2. More recent `lastUsed` value
3. Lexicographically smaller word

This makes result order stable and predictable.

## 4. Selection and learning flow

```text
User selects "project"
      │
      ▼
AutocompleteEngine verifies that the word is in the Trie
      │
      ▼
UsageManager::recordUsage("project")
      │
      ├── frequency increases by 1
      ├── global usage counter increases by 1
      └── lastUsed becomes the new usage-counter value
      │
      ▼
FileManager saves updated usage records to data/usage.txt
      │
      ▼
Future searches can rank "project" higher
```

Autocomplete requests do not change statistics. Only a user selection changes frequency and recency data.

## 5. Interface flow

### CLI

```text
Enter prefix → show ranked suggestions → select a number → update usage → save data
```

The CLI also lets the user enter another prefix instead of selecting a displayed result.

### Browser interface

```text
User types a prefix
      │
      ▼
web/app.js calls GET /api/autocomplete?prefix=...
      │
      ▼
WebServer calls AutocompleteEngine
      │
      ▼
JSON suggestions return to the browser
      │
      ▼
User clicks a result or presses Enter
      │
      ▼
web/app.js calls POST /api/select?word=...
      │
      ▼
Usage is updated and saved immediately
```

The browser supports Up/Down navigation, Enter to select, and Escape to clear the current search.

## 6. Performance summary

| Operation | Complexity |
|---|---:|
| Insert word of length `L` | `O(L)` |
| Search word of length `L` | `O(L)` |
| Find prefix of length `P` | `O(P)` |
| Collect matching words | `O(P + S)` |
| Select Top-K from `N` candidates | `O(N log K)` |

`S` is the size of the matching Trie subtree, and `K` is the requested number of suggestions.

## Key takeaway

The Trie efficiently finds words that match a prefix. The ranking engine decides which matching words are most relevant. Usage statistics persist on disk, allowing selected words to become more prominent in future searches.
