#pragma once

#include <vector>
#include <unordered_map>

class MathematicalFonction {
public:
    static double calculateEntropy(const std::vector<int>& data);

private:
    static std::unordered_map<size_t, double> entropyCache;
    static bool fonctionCheckPoint;
};