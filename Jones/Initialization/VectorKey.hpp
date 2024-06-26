#pragma once

#include <tuple>
#include <vector>
#include <functional>
#include <string>

using VectorKey = std::tuple<int, int, int, std::string, int>;
static bool fonctionCheckPoint;

struct VectorKeyHash {
    size_t operator()(const VectorKey& key) const;
};

struct VectorKeyEqual {
    bool operator()(const VectorKey& lhs, const VectorKey& rhs) const;
};