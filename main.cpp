#include <iostream>
#include "Analyser.h"
#include "SpectralPartitioner.h"
#include "Annealing.h"

int main() {
    //Analyser an = Analyser("../files/adjList", "../files/ErdosIndex");
    //std::cout << an.findMaxErdosNumber() << '\n';


    /*
    //-------------------------------annealing

    Annealing ann = Annealing("../files/adjList", "../files/ErdosIndex");
    ann.anneal();
    ann.saveGuess("../files/annealingPartition", ann.bestGuess);
    int minSize = 0;
    for (auto b : ann.bestGuess)
        minSize += b;
    std::cout << "conductance:\t" << ann.bestConductance << "\tsizes of the parts:\t" << minSize << ' ' << ann.size - minSize << '\n';
    */



    //-----------------------------spectral partition------------------------------------

    SpectralPartitioner sp("../files/adjList", "../files/ErdosIndex");
    double conductance;
    std::vector<float> v2;
    std::vector<bool> guess;

    v2 = sp.readV2("../files/v2");
    std::tie(conductance, guess) = sp.getPartition(0, v2);
    sp.saveGuess("../files/spectralPartition", guess);
    int minSize = 0;
    for (auto b : guess)
        minSize += b;
    std::cout << "conductance:\t" << conductance << "\tsizes of the parts:\t" << minSize << ' ' << sp.size - minSize << '\n';

    std::tie(conductance,  guess) = sp.getPartition(10000, v2);
    sp.saveGuess("../files/spectralPartition>10000", guess);
    minSize = 0;
    for (auto b : guess)
        minSize += b;
    std::cout << "conductance:\t" << conductance << "\tsizes of the parts:\t" << minSize << ' ' << sp.size - minSize << '\n';

    std::vector<int> ignoreList;
    for (int i = 0; i < sp.size; i++) {
        if (guess[i])
            ignoreList.push_back(i);
    }

    v2 = sp.readV2("../files/v2-37");
    std::tie(conductance,  guess) = sp.getPartition(0, v2);
    sp.saveGuess("../files/spectralPartition-37", guess);
    minSize = 0;
    for (auto b : guess)
        minSize += b;
    std::cout << "conductance:\t" << conductance << "\tsizes of the parts:\t" << minSize << ' ' << sp.size - minSize << '\n';

    std::tie(conductance,  guess) = sp.getPartition(10000, v2);
    sp.saveGuess("../files/spectralPartition-37>10000", guess);
    minSize = 0;
    for (auto b : guess)
        minSize += b;
    std::cout << "conductance:\t" << conductance << "\tsizes of the parts:\t" << minSize << ' ' << sp.size - minSize << '\n';
    return 0;


}
