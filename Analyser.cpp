#include "Analyser.h"

Analyser::Analyser(const std::string &edges_filename) {
    std::cout << "creating the adjacency list...\n";
    json input;
    std::ifstream file(edges_filename, std::ifstream::binary);
    file >> input;
    std::vector<std::pair<std::string, std::string>> edges = input.get<std::vector<std::pair<std::string, std::string>>>();
    std::unordered_map<std::string, int> idsToIndices;
    for (const auto &edge: edges) {
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
        if (find(adjList[index1].begin(), adjList[index1].end(), index2) ==
            adjList[index1].end()) //to avoid duplicate edges
            adjList[index1].push_back(index2);
        if (find(adjList[index2].begin(), adjList[index2].end(), index1) == adjList[index2].end())
            adjList[index2].push_back(index1);
    }
    size = adjList.size();

    maxdeg = 0;
    volume = 0;
    for (const auto &l: adjList) {
        if (l.size() > maxdeg)
            maxdeg = l.size();
        volume += l.size();
    }
}

int Analyser::findMaxErdosNumber() {
    std::cout << "finding tha largest Erdos number...\n";
    int visited = 0;
    std::queue<int> q;
    q.push(ErdosIndex);
    std::vector<int> dist(size, INT32_MAX);
    dist[ErdosIndex] = 0;
    while (!q.empty()) {
        visited++;
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

float Analyser::computeConductanceManually(std::vector<bool> partitition) {
    int cutEdges = 0;
    int partVolume = 0;
    for (int i = 0; i < size; i++) {
        if (partitition[i])
            partVolume += adjList[i].size();
        for (int v: adjList[i]) {
            if (partitition[v] != partitition[i])
                cutEdges++;
        }
    }
    return float(cutEdges) / 2 / std::min(partVolume, volume - partVolume);
}

void Analyser::saveAdjList(const std::string &filename) {
    std::ofstream output;
    output.open(filename);
    for (int i = 0; i < size; i++) {
        for (int to: adjList[i])
            output << to << " ";
        output << '\n';
    }
    output.close();
}

void Analyser::saveErdosIndex(const std::string &filename) const {
    std::ofstream output;
    output.open(filename);
    output << ErdosIndex;
    output.close();
}

Analyser::Analyser(const std::string &adjList_filename, const std::string &ErdosIndex_filename) {
    std::ifstream input1;
    input1.open(ErdosIndex_filename);
    input1 >> ErdosIndex;
    maxdeg = 0;
    volume = 0;

    std::ifstream input2;
    input2.open(adjList_filename);
    std::string line;
    while (getline(input2, line)) {
        adjList.push_back(std::vector<int>(0));
        std::stringstream iss(line);
        int to;
        while (iss >> to)
            adjList.back().push_back(to);
        if (adjList.back().size() > maxdeg)
            maxdeg = adjList.back().size();
        volume += adjList.back().size();
    }
    size = adjList.size();
}

void Analyser::saveGuess(const std::string &filename, std::vector<bool> _guess) {
    std::ofstream output;
    output.open(filename);
    for (int i = 0; i < size; i++)
        output << _guess[i] << "\n";
    output.close();
}