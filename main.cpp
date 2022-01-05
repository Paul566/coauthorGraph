#include <iostream>
#include "Analyser.h"
#include "SpectralPartititioner.h"

int main() {
    SpectralPartititioner sp("../edges.json");
    double conductance;
    int minSize;
    std::tie(conductance, minSize) = sp.getPartitition(100);
    std::cout << conductance << ' ' << minSize;
    return 0;
}
