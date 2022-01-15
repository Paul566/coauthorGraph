//
// Created by paul on 1/8/22.
//

#include "Annealing.h"

Annealing::Annealing(const std::string &adjList_filename, const std::string &ErdosIndex_filename, float _initTemp,
                     long _numSteps, long _updateTempEveryThisManyTimesteps) : Analyser(adjList_filename,
                                                                                        ErdosIndex_filename) {
    std::random_device rd;
    gen = std::mt19937(rd());
    stepChoiceDistribution = std::uniform_real_distribution<>(0., 1.);
    nodeIndexDistribution = std::uniform_int_distribution<>(0, size - 1);
    curTemp = _initTemp;
    initTemp = _initTemp;
    curStep = 0;
    numSteps = _numSteps;
    updateTempEveryThisManyTimesteps = _updateTempEveryThisManyTimesteps;
    guess = std::vector<bool>(size, false);
    curCutEdges = 0;
    for (int i = 0; i < size / 2; i++) {
        int vertex = nodeIndexDistribution(gen);
        guess[vertex] = true;
    }
    trueSize = size / 2;
    curResult = computeConductanceManually(guess);
    curCutEdges = int(curResult * trueSize);
    bestResultSize = trueSize;
    bestResult = curResult;
}

void Annealing::anneal() {
    std::cout << "starting annealing...\n";
    while (curStep < numSteps) {
        if (curStep % (10000 * updateTempEveryThisManyTimesteps) == 0)
            std::cout << "step\t" << curStep << "\ttemperature\t" << curTemp << "\tminsize\t" << bestResultSize
                      << "\tconductance\t" << bestResult << '\n';
        for (int i = 0; i < updateTempEveryThisManyTimesteps; i++)
            makeStep();
        updateTemp();
    }
}

void Annealing::updateTemp() {
    curTemp = initTemp - initTemp * curStep / numSteps;
}

void Annealing::makeStep() {
    int vertex = nodeIndexDistribution(gen);
    int deltaEdges = 0;
    for (int to: adjList[vertex]) {
        if (guess[vertex] != guess[to])
            deltaEdges--;
        else
            deltaEdges++;
    }
    int newTrueSize;
    if (guess[vertex])
        newTrueSize = trueSize - 1;
    else
        newTrueSize = trueSize + 1;
    if ((trueSize == 0) || (trueSize == size))
        return;
    float newResult = float(curCutEdges + deltaEdges) / std::min(newTrueSize, size - newTrueSize);
    if (doWeAccept(newResult)) {
        guess[vertex] = not guess[vertex];
        trueSize = newTrueSize;
        curResult = newResult;
        curCutEdges += deltaEdges;
        if (curResult < bestResult) {
            bestResult = curResult;
            bestResultSize = std::min(newTrueSize, size - newTrueSize);
            //std::cout << "step\t" << curStep << "\ttemperature\t" << curTemp << "\tminsize\t" << bestResultSize
            //          << "\tconductance\t" << bestResult << '\n';
        }
    }
    curStep++;
}

bool Annealing::doWeAccept(float newResult) {
    return expf((curResult - newResult) / curTemp) > stepChoiceDistribution(gen);
}
