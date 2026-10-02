#pragma once

#include "aiws/corpus_index.hpp"
#include "aiws/processing_types.hpp"

#include <string>
#include <vector>

namespace aiws {

// M2 PUBLIC-INTERFACE DESIGN TASK
// Complete this class as a safe abstract polymorphic interface.
// Keep the class name, operation name, parameter types, return type,
// const qualification, and namespace unchanged.
class RetrievalStrategy {
public:
    // TODO: make destruction safe through a base-class pointer.
    virtual ~RetrievalStrategy() = default;

    // TODO: make this a required polymorphic operation.
    virtual std::vector<SearchResult> search(const std::string& query,
                                             int k,
                                             const std::vector<Chunk>& chunks,
                                             const CorpusIndex& index) const = 0;
};

}  // namespace aiws
