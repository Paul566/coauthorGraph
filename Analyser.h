#ifndef COAUTHORGRAPH_ANALYSER_H
#define COAUTHORGRAPH_ANALYSER_H


#include <iostream>
#include "json.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <iomanip>
#include <functional>
#include <utility>

using json = nlohmann::json;

class Analyser {

    std::vector<std::vector<int>> adjList;
    std::string ErdosId = "cVeVZ1YAAAAJ"; //Google Scholar ID of Paul Erdos
    int ErdosIndex;

public:

    explicit Analyser(const std::string &filename);
    int findLargestErdosNumber();   //just BFS starting from Erdos

};


#endif //COAUTHORGRAPH_ANALYSER_H
