#pragma once

#include "aiws/document.hpp"
#include "aiws/processing_types.hpp"

#include <cstddef>
#include <vector>

namespace aiws {

// M2 PUBLIC-INTERFACE DESIGN TASK
// Complete this class as a safe abstract polymorphic interface.
// Keep the class name, operation name, parameter types, return type,
// const qualification, and namespace unchanged.
class ChunkingStrategy {
public:
    // TODO: make destruction safe through a base-class pointer.
    virtual ~ChunkingStrategy() = default;

    // TODO: make this a required polymorphic operation.
    virtual std::vector<Chunk> chunk(const Document& document,
                                     std::size_t document_order) const = 0;
};

}  // namespace aiws
