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
private:
    //number of vertices
    std::string ErdosId = "cVeVZ1YAAAAJ"; //Google Scholar ID of Paul Erdos
    int ErdosIndex;
protected:
    std::vector<std::vector<int>> adjList;
public:
    int size;
    int maxdeg;

    explicit Analyser(const std::string &edges_filename);
    Analyser(const std::string &adjList_filename, const std::string &ErdosIndex_filename);

    int findMaxErdosNumber();   //just BFS starting from Erdos
    float computeConductanceManually(std::vector<bool> partitition);
    void saveAdjList(const std::string& filename);
    void saveErdosIndex(const std::string& filename) const;
};


#endif //COAUTHORGRAPH_ANALYSER_H
