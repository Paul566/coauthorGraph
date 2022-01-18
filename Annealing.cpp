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
    trueVolume = 0;
    for (int i = 0; i < size / 2; i++) {
        int vertex = nodeIndexDistribution(gen);
        if (guess[vertex])
            continue;
        guess[vertex] = true;
        trueVolume += adjList[vertex].size();
    }
    bestGuess = guess;
    curConductance = computeConductanceManually(guess);
    curCutEdges = int(curConductance * std::min(trueVolume, volume - trueVolume));
    bestConductance = curConductance;
}

void Annealing::anneal() {
    std::cout << "starting annealing...\n";
    while (curStep < numSteps) {
        if (curStep % (10000 * updateTempEveryThisManyTimesteps) == 0) {
            int minSize = 0;
            for (auto b: guess)
                minSize += b;
            printf("step\t%ld\ttemperature\t%.9f\tbest conductance\t%.6f\tcurrent conductance\t%.6f\tcurrent sizes of the parts:\t%d\t%d\n",
                   curStep, curTemp, bestConductance, curConductance, minSize, size - minSize);
        }
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
    int newTrueVolume;
    if (guess[vertex])
        newTrueVolume = trueVolume - adjList[vertex].size();
    else
        newTrueVolume = trueVolume + adjList[vertex].size();
    if ((newTrueVolume == 0) || (newTrueVolume == volume))
        return;
    float newResult = float(curCutEdges + deltaEdges) / std::min(newTrueVolume, volume - newTrueVolume);
    if (doWeAccept(newResult)) {
        guess[vertex] = not guess[vertex];
        trueVolume = newTrueVolume;
        curConductance = newResult;
        curCutEdges += deltaEdges;
        if (curConductance < bestConductance) {
            bestConductance = curConductance;
            if (curTemp < initTemp / 2) //in order not to spend too much time in the beginning
                bestGuess = guess;
        }
    }
    curStep++;
}

bool Annealing::doWeAccept(float newResult) {
    return expf((curConductance - newResult) / curTemp) > stepChoiceDistribution(gen);
}
