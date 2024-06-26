#include "HashData.hpp"

bool HashData::fonctionCheckPoint = false;

size_t HashData::hashData(const std::vector<int>& data) {
    if (fonctionCheckPoint) {
        std::cerr << "Fonction Checkpoint : hashData" << std::endl;
    }
    size_t hash = 0;
    for (int num : data) {
        hash = hash * 31 + num;
    }
    return hash;
}