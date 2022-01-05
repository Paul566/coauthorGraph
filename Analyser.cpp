#include "Analyser.h"

using json = nlohmann::json;

Analyser::Analyser(const std::string &filename) {
    std::cout << "creating the adjacency list...\n";
    json input;
    std::ifstream file(filename, std::ifstream::binary);
    file >> input;
    std::vector<std::pair<std::string, std::string>> edges = input.get<std::vector<std::pair<std::string, std::string>>>();
    std::unordered_map<std::string, int> idsToIndices;
    for (const auto& edge: edges) {
        int index1, index2;
        if (idsToIndices.find(edge.first) == idsToIndices.end()) {
            index1 = adjList.size();
            idsToIndices[edge.first] = index1;
            if (edge.first == ErdosId)
                ErdosIndex = index1;
            adjList.push_back(std::vector<int>(0));
        } else
            index1 = idsToIndices[edge.first];
        if (idsToIndices.find(edge.second) == idsToIndices.end()) {
            index2 = adjList.size();
            idsToIndices[edge.second] = index2;
            if (edge.second == ErdosId)
                ErdosIndex = index2;
            adjList.push_back(std::vector<int>(0));
        } else
            index2 = idsToIndices[edge.second];
        if (find(adjList[index1].begin(), adjList[index1].end(), index2) == adjList[index1].end()) //to avoid duplicate edges
            adjList[index1].push_back(index2);
        if (find(adjList[index2].begin(), adjList[index2].end(), index1) == adjList[index2].end())
            adjList[index2].push_back(index1);
    }
}

int Analyser::findLargestErdosNumber() {
    std::cout << "finding tha largest Erdos number...\n";
    std::queue<int> q;
    q.push(ErdosIndex);
    std::vector<int> dist(adjList.size(), INT32_MAX);
    dist[ErdosIndex] = 0;
    while (!q.empty()) {
        int cur = q.front();
        q.pop();
        for (auto v: adjList[cur]) {
            if (dist[v] != INT32_MAX)
                continue;
            dist[v] = dist[cur] + 1;
            q.push(v);
        }
    }
    return *std::max_element(dist.begin(), dist.end());
}
