#ifndef COLLECT_PY_FREESTYLEPARTITIONER_H
#define COLLECT_PY_FREESTYLEPARTITIONER_H


#include "Analyser.h"

class FreeStylePartitioner : public Analyser {
private:
    struct shrunkGraph {
        std::vector<std::vector<std::pair<int, int>>> adjList; // adjList[i][j].second is the weight of the edge
        std::vector<int> weights; // the weights of the vertices
        std::vector<std::vector<int>> getBack; // getBack[i] is a list of vertices of the parent graph inside the shrunk graph
        int size;
    };

    void dfsForBridges(int time, int u, std::vector<bool> &visited, std::vector<int> &discover, std::vector<int> &low,
                       std::vector<int> &parent, std::vector<std::pair<int, int>> &bridges);
    void dfsForCutVertices(int time, int u, std::vector<bool> &visited, std::vector<int> &discover, std::vector<int> &low,
                       int parent, std::vector<int> &cutVertices);

public:
    std::vector<std::shared_ptr<shrunkGraph>> tower;

    FreeStylePartitioner(const std::string &adjList_filename, const std::string &ErdosIndex_filename);

    void hideLeaves();

    int getNumberOfLeaves();

    std::vector<std::pair<int, int>> findBridges();
    std::vector<int> findCutVertices();
};


#endif //COLLECT_PY_FREESTYLEPARTITIONER_H
