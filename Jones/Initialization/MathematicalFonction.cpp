#include "MathematicalFonction.hpp"
#include "HashData.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

std::unordered_map<size_t, double> MathematicalFonction::entropyCache;

double MathematicalFonction::calculateEntropy(const std::vector<int>& data) {
    size_t dataHash = HashData::hashData(data);

    if (entropyCache.find(dataHash) != entropyCache.end()) {
        return entropyCache[dataHash];
    }

    int countOne = std::count(data.begin(), data.end(), 1);
    int total = data.size();
    double p1 = static_cast<double>(countOne) / total;
    double p0 = 1.0 - p1;

    if (p0 == 0.0 || p1 == 0.0) {
        entropyCache[dataHash] = 0.0;
        return 0.0;
    }

    double entropy = -p1 * log2(p1) - p0 * log2(p0);
    entropyCache[dataHash] = entropy;

    return entropy;
}