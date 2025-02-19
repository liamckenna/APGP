#pragma once
#include <unordered_map>

struct VertexKey {
    int positionIndex;
    int texcoordIndex;
    int normalIndex;

    bool operator==(const VertexKey& other) const {
        return positionIndex == other.positionIndex &&
            texcoordIndex == other.texcoordIndex &&
            normalIndex == other.normalIndex;
    }
};

namespace std {
    template<>
    struct hash<VertexKey> {
        size_t operator()(const VertexKey& key) const {
            return ((std::hash<int>()(key.positionIndex) ^
                (std::hash<int>()(key.texcoordIndex) << 1)) >> 1) ^
                (std::hash<int>()(key.normalIndex) << 1);
        }
    };
}
