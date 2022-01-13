#ifndef COLLECT_PY_ANNEALING_H
#define COLLECT_PY_ANNEALING_H

#include <random>
#include "Analyser.h"

class Annealing : public Analyser {
public:
    double bestResult;
    int bestResultSize;

    Annealing(const std::string &filename, float _initTemp = 1e-6, long _numSteps = 10000000000,
              long _updateTempEveryThisManyTimesteps = 1000);

    void anneal();

    void saveState(const std::string& filename);


private:
    std::mt19937 gen;
    std::uniform_int_distribution<> nodeIndexDistribution;
    std::uniform_real_distribution<> stepChoiceDistribution;
    float curTemp;
    float initTemp;
    long curStep;
    long numSteps;
    long updateTempEveryThisManyTimesteps;
    float curResult;
    int trueSize; //size of the part that marked "true" in guess
    int curCutEdges;
    std::vector<bool> guess;

    void makeStep();

    void updateTemp();

    bool doWeAccept(float newResult);
};


#endif //COLLECT_PY_ANNEALING_H
