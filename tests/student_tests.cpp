#include "aiws/chunking_strategy.hpp"
#include "aiws/context_strategy.hpp"
#include "aiws/processing_core.hpp"
#include "aiws/retrieval_strategy.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace {
int failures = 0;

void check(bool condition, const char* description) {
    if (!condition) {
        std::cerr << "FAIL: " << description << '\n';
        ++failures;
    }
}

class MarkerChunker final : public aiws::ChunkingStrategy {
public:
    std::vector<aiws::Chunk> chunk(const aiws::Document& document,
                                   std::size_t order) const override {
        return {{document.id() + "#marker", document.id(), order, 0,
                 "marker token", 2, 0, document.text().size()}};
    }
};

class FixedRetriever final : public aiws::RetrievalStrategy {
public:
    std::vector<aiws::SearchResult> search(
        const std::string&, int k, const std::vector<aiws::Chunk>& chunks,
        const aiws::CorpusIndex&) const override {
        if (k <= 0 || chunks.empty()) return {};
        const auto& chunk = chunks.front();
        return {{chunk.id, chunk.document_id, chunk.sequence, chunk.text, 42.0, 1}};
    }
};

class FixedContext final : public aiws::ContextStrategy {
public:
    std::vector<aiws::ContextItem> build(
        const std::vector<aiws::SearchResult>& ranked,
        std::size_t budget) const override {
        if (ranked.empty() || budget == 0) return {};
        const auto& result = ranked.front();
        return {{result.chunk_id, result.document_id, result.chunk_sequence,
                 "injected context", 1, result.score, false}};
    }
};
}

int main() {
    using namespace aiws;
    static_assert(!std::is_copy_constructible<ProcessingCore>::value,
                  "ProcessingCore must not copy unique ownership");
    static_assert(std::is_move_constructible<ProcessingCore>::value,
                  "ProcessingCore must be movable");

    Workspace workspace;
    workspace.add_document(Document{"alpha", "", "Alpha beta beta."});
    workspace.add_document(Document{"gamma", "", "Gamma alpha."});

    ProcessingCore defaults;
    defaults.rebuild(workspace);
    check(ProcessingCore::normalize("Search... SEARCH!! 42-times") ==
              "search search 42 times", "default normalization preserved");
    check(defaults.chunk_count() == 2, "default chunking preserved");
    const auto default_results = defaults.search("beta", 2);
    check(default_results.size() == 1 && default_results[0].document_id == "alpha",
          "default retrieval preserved");
    const auto bounded = defaults.build_context("beta", 2, 1);
    check(bounded.size() == 1 && bounded[0].token_count == 1,
          "default context respects token budget");

    ProcessingCore custom(std::make_unique<MarkerChunker>(),
                          std::make_unique<FixedRetriever>(),
                          std::make_unique<FixedContext>());
    custom.rebuild(workspace);
    check(custom.chunk_count() == 2 && custom.chunks()[0].id == "alpha#marker",
          "injected chunker output used");
    const auto custom_results = custom.search("not in documents", 1);
    check(custom_results.size() == 1 && custom_results[0].score == 42.0,
          "injected retrieval used for arbitrary query");
    const auto custom_context = custom.build_context("anything", 1, 5);
    check(custom_context.size() == 1 &&
              custom_context[0].text == "injected context",
          "injected context builder receives retrieval output");

    bool rejected_null = false;
    try {
        ProcessingCore invalid(nullptr, std::make_unique<FixedRetriever>(),
                               std::make_unique<FixedContext>());
    } catch (const std::invalid_argument&) {
        rejected_null = true;
    }
    check(rejected_null, "null strategy rejected");

    ProcessingCore moved(std::move(custom));
    check(moved.search("anything", 1).front().score == 42.0,
          "move construction retains owned strategies and corpus");

    Workspace duplicate;
    duplicate.add_document(Document{"same", "", "first"});
    duplicate.add_document(Document{"same", "", "second"});
    bool rejected_duplicate = false;
    try {
        moved.rebuild(duplicate);
    } catch (const std::invalid_argument&) {
        rejected_duplicate = true;
    }
    check(rejected_duplicate, "duplicate IDs rejected");
    check(moved.chunk_count() == 2 &&
              moved.chunks()[0].id == "alpha#marker" &&
              moved.search("anything", 1).front().score == 42.0,
          "failed rebuild preserves prior corpus and strategy composition");

    if (failures != 0) return 1;
    std::cout << "Student M2 tests passed\n";
    return 0;
}
