cmake_minimum_required(VERSION 3.16)
project(AutocompleteEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(autocomplete_core
    src/Trie.cpp
    src/UsageManager.cpp
    src/RankingEngine.cpp
    src/FileManager.cpp
    src/AutocompleteEngine.cpp)
target_include_directories(autocomplete_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(autocomplete main.cpp)
target_link_libraries(autocomplete PRIVATE autocomplete_core)

add_executable(autocomplete_web src/WebServer.cpp)
target_link_libraries(autocomplete_web PRIVATE autocomplete_core ws2_32)

enable_testing()
foreach(test_name IN ITEMS test_trie test_ranking test_autocomplete)
    add_executable(${test_name} tests/${test_name}.cpp)
    target_link_libraries(${test_name} PRIVATE autocomplete_core)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

add_executable(autocomplete_benchmark benchmarks/benchmark.cpp)
target_link_libraries(autocomplete_benchmark PRIVATE autocomplete_core)
