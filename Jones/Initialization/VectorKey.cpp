#include "VectorKey.hpp"

size_t VectorKeyHash::operator()(const VectorKey& key) const {
    size_t h1 = std::hash<int>()(std::get<0>(key));
    size_t h2 = std::hash<int>()(std::get<1>(key));
    size_t h3 = std::hash<int>()(std::get<2>(key));
    size_t h4 = std::hash<std::string>()(std::get<3>(key));
    size_t h5 = std::hash<int>()(std::get<4>(key));
    return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4);
}

bool VectorKeyEqual::operator()(const VectorKey& lhs, const VectorKey& rhs) const {
    return lhs == rhs;
}
