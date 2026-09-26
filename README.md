# C++ Autocomplete Engine

A C++17, trie-backed autocomplete engine with deterministic frequency-and-recency ranking, bounded Top-K selection, persistent usage statistics, an interactive CLI, tests, and a reproducible benchmark program.

## Features

- Case-normalized dictionary storage and prefix lookup using a trie.
- Separate `UsageManager` for word frequency and a monotonic `lastUsed` counter.
- Combined score: `alpha * normalizedFrequency + beta * recencyScore`.
- Configurable defaults: `alpha=0.6`, `beta=0.4`, `lambda=0.1`.
- `std::priority_queue` min-heap selection in `O(N log K)`, with deterministic ties.
- Safe handling for empty/unknown prefixes, invalid K values, duplicate words, malformed usage records, and missing files.

## Architecture

```text
User -> Prefix Query -> Trie -> Matching candidates -> RankingEngine -> min-heap -> Top-K
                                                                  ^
                                                                  |
                  Select suggestion -> UsageManager (frequency++, lastUsed++)
                                                                  |
                                                            FileManager
```

The trie only stores words. Usage data is deliberately separate, so dictionary lookup remains focused and ranking/persistence can evolve independently.

## Ranking

For each matching candidate, the engine calculates:

```text
normalizedFrequency = log(1 + frequency) / log(1 + maximumFrequency)
recencyScore        = exp(-lambda * (currentCounter - lastUsed))
finalScore          = alpha * normalizedFrequency + beta * recencyScore
```

Never-used words have a recency score of zero. Equal scores break by higher frequency, then more recent use, then lexical order. Autocomplete never updates statistics; only a selected suggestion does.

## Complexity

`insert`, `search`, and `startsWith` are `O(L)`, where `L` is the word or prefix length. Collecting matching candidates is `O(M)`, and Top-K ranking is `O(N log K)` for `N` candidates. The returned results are finally sorted only across at most `K` entries.

## Build and run

```bash
mkdir build
cd build
cmake ..
cmake --build .
./autocomplete
```

On Windows with CMake's default generator, run `autocomplete.exe` from the project root so the relative `data/` paths resolve, or copy `data/` into the executable working directory.

At startup the CLI loads `data/words.txt`, inserts every word into the Trie, initializes its statistics to zero, and then restores saved values from `data/usage.txt` when available. The user only enters prefixes and selects suggestions; a selection is saved automatically, and statistics are also saved on exit. `words.txt` contains one dictionary word per line, while `usage.txt` is application-managed and contains `word frequency lastUsed` records.

## Browser interface

Build the project, then run the local web server from the project root:

```powershell
.\build\autocomplete_web.exe
```

Open [http://127.0.0.1:8080](http://127.0.0.1:8080) in a browser. The server listens only on your computer. Type a prefix, use the mouse or arrow keys to select a suggestion, and press Enter to save the selection immediately. Press `Ctrl+C` in the server terminal to stop it.

## Tests

After building:

```bash
ctest --output-on-failure
```

The tests cover trie lookup/prefix collection/duplicates, usage counter updates including unknown words, combined ranking behavior, and an autocomplete integration flow.

## Benchmarking

The project includes a measurement program rather than fabricated results:

```bash
./autocomplete_benchmark 10000
./autocomplete_benchmark 100000
./autocomplete_benchmark 1000000
```

It reports trie construction, prefix lookup, candidate collection, ranking, and total autocomplete time for `K=5`, `10`, and `20`, making measurements directly reproducible on the target machine.

## Project structure

```text
include/     Public components (Trie, UsageManager, RankingEngine, FileManager, coordinator)
src/         Implementations
data/        Sample dictionary and usage data
tests/       Lightweight unit and integration tests
benchmarks/  Reproducible timing program
```

## Future improvements

Potential additions include fuzzy matching, cache layers, database persistence, networking, a GUI, and concurrency. None are included in this version.
