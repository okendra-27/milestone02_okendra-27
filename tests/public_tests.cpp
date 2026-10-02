#include "aiws/chunking_strategy.hpp"
#include "aiws/context_strategy.hpp"
#include "aiws/processing_core.hpp"
#include "aiws/retrieval_strategy.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
int failures = 0;
void check(bool ok, const char* name) {
    if (!ok) { std::cerr << "FAIL: " << name << '\n'; ++failures; }
}

class OneChunk final : public aiws::ChunkingStrategy {
public:
    std::vector<aiws::Chunk> chunk(const aiws::Document& d, std::size_t order) const override {
        return {{d.id()+"#custom", d.id(), order, 0, "custom chunk", 2, 0, d.text().size()}};
    }
};
class FirstOnly final : public aiws::RetrievalStrategy {
public:
    std::vector<aiws::SearchResult> search(const std::string&, int k,
        const std::vector<aiws::Chunk>& chunks, const aiws::CorpusIndex&) const override {
        if (k <= 0 || chunks.empty()) return {};
        const auto& c = chunks.front();
        return {{c.id, c.document_id, c.sequence, c.text, 99.0, 1}};
    }
};
class PrefixContext final : public aiws::ContextStrategy {
public:
    std::vector<aiws::ContextItem> build(const std::vector<aiws::SearchResult>& ranked,
                                          std::size_t budget) const override {
        if (ranked.empty() || budget == 0) return {};
        const auto& r = ranked.front();
        return {{r.chunk_id, r.document_id, r.chunk_sequence, "custom", 1, r.score, true}};
    }
};
}

int main() {
    using namespace aiws;

    // M1 compatibility smoke.
    Workspace ws;
    ws.add_document(Document{"a", "", "Alpha beta beta."});
    ws.add_document(Document{"b", "", "Gamma alpha."});
    ProcessingCore normal;
    normal.rebuild(ws);
    check(ProcessingCore::normalize("Search... SEARCH!! 42-times") == "search search 42 times",
          "M1 normalization preserved");
    check(normal.chunk_count() == 2, "default chunking preserved");
    auto ranked = normal.search("beta", 2);
    check(ranked.size() == 1 && ranked[0].document_id == "a", "default retrieval preserved");

    // M2 runtime composition smoke.
    try {
        ProcessingCore custom(std::make_unique<OneChunk>(),
                              std::make_unique<FirstOnly>(),
                              std::make_unique<PrefixContext>());
        custom.rebuild(ws);
        check(custom.chunk_count() == 2, "custom chunker invoked");
        check(custom.chunks()[0].id == "a#custom", "custom chunk output retained");
        auto r = custom.search("anything", 1);
        check(r.size() == 1 && r[0].score == 99.0, "custom retrieval invoked");
        auto c = custom.build_context("anything", 1, 5);
        check(c.size() == 1 && c[0].text == "custom", "custom context invoked");
    } catch (const std::exception& e) {
        std::cerr << "FAIL: strategy injection threw: " << e.what() << '\n';
        ++failures;
    }

    bool threw = false;
    try {
        ProcessingCore bad(nullptr, std::make_unique<FirstOnly>(), std::make_unique<PrefixContext>());
    } catch (const std::invalid_argument&) { threw = true; }
    catch (...) {}
    check(threw, "null strategy rejected with invalid_argument");

    if (failures) return 1;
    std::cout << "M2 public tests passed\n";
    return 0;
}
