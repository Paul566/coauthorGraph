#include <iostream>
#include "Analyser.h"
#include "SpectralPartitioner.h"
#include "Annealing.h"

int main() {
    //Analyser an = Analyser("../adjList", "../ErdosIndex");
    //std::cout << an.findMaxErdosNumber() << '\n';

    //Annealing ann = Annealing("../edges.json");
    //ann.anneal();
    //std::cout << ann.bestResult << ' ' << ann.bestResultSize << '\n';

    SpectralPartitioner sp("../specData", "../adjList", "../ErdosIndex");
    double conductance;
    int minSize;
    std::tie(conductance, minSize) = sp.getPartition(100);
    std::cout << conductance << ' ' << minSize << '\n';
    return 0;
}
