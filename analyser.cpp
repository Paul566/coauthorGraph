#include <iostream>
#include "json.hpp"
#include <fstream>
#include "string"
#include "vector"
#include "unordered_map"
#include "queue"
#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <Spectra/SymEigsSolver.h>
#include <Spectra/MatOp/SparseSymMatProd.h>

using json = nlohmann::json;

std::vector<std::pair<std::string, std::string>> readEdges() {
    json input;
    std::ifstream edgesFile("edges.json", std::ifstream::binary);
    edgesFile >> input;
    std::vector<std::pair<std::string, std::string>> edges = input.get<std::vector<std::pair<std::string, std::string>>>();
    return edges;
}

std::unordered_map<std::string, std::string> readNames() {
    json input;
    std::ifstream edgesFile("ids_to_names.json", std::ifstream::binary);
    edgesFile >> input;
    std::unordered_map<std::string, std::string> idsToNames = input.get<std::unordered_map<std::string, std::string>>();
    return idsToNames;
}

std::vector<std::string> getVertexVector(std::unordered_map<std::string, std::string> idsToNames) {
    std::vector<std::string> nodeIds;
    for (auto p : idsToNames)
        nodeIds.push_back(p.first);
    return nodeIds;
}

std::unordered_map<std::string, int> getIdsToIndices(std::vector<std::string> nodeIds) {
    std::unordered_map<std::string, int> idsToIndices;
    for (int i = 0; i < nodeIds.size(); i++)
        idsToIndices[nodeIds[i]] = i;
    return idsToIndices;
}

std::vector<std::vector<int>>
getAdjList(std::vector<std::pair<std::string, std::string>> edges, std::unordered_map<std::string, int> idsToIndices) {
    std::vector<std::vector<int>> a;
    for (int i = 0; i < idsToIndices.size(); i++) {
        a.push_back(std::vector<int>());
    }
    for (auto e : edges) {
        int from = idsToIndices[e.first];
        int to = idsToIndices[e.second];
        if (std::find(a[from].begin(), a[from].end(), to) == a[from].end()) {
            a[from].push_back(to);
            a[to].push_back(from);
        }
    }
    return a;
}

int findLargestErdosNumber(std::vector<std::vector<int>> adjList, int erdos) {
    std::queue<int> q;
    q.push(erdos);
    std::vector<int> dist(adjList.size(), INT32_MAX);
    dist[erdos] = 0;
    while (!q.empty()) {
        int cur = q.front();
        q.pop();
        for (auto v : adjList[cur]) {
            if (dist[v] != INT32_MAX)
                continue;
            dist[v] = dist[cur] + 1;
            q.push(v);
        }
    }
    int ans = 0;
    for (int i = 0; i < adjList.size(); i++) {
        if (ans < dist[i])
            ans = dist[i];
    }
    return ans;
}

Spectra::SymEigsSolver<float, Spectra::SMALLEST_ALGE, Spectra::SparseSymMatProd<float>>
spectrum(int n, std::vector<std::vector<int>> adjList) {
    int size = adjList.size();
    Eigen::SparseMatrix<float> M(size, size);
    M.reserve(Eigen::VectorXi::Constant(M.cols(), 10)); //"10" is almost surely twice greater than average degree of a node
    std::cout << "im fine\n";
    for (int i = 0; i < size; i++) {
        M.insert(i, i) = adjList[i].size();
        for (int to : adjList[i]) {
            M.insert(i, to) = -1;
        }
    }
    std::cout << "im fine\n";
    Spectra::SparseSymMatProd<float> op(M);
    std::cout << "im fine\n";
    Spectra::SymEigsSolver<float, Spectra::SMALLEST_ALGE, Spectra::SparseSymMatProd<float>> eigs(&op, n, 1000); //1000 >= n
    std::cout << "im fine\n";
    eigs.init();
    eigs.compute(10, 0.001);
    return eigs;
}

int main() {
    auto edges = readEdges();
    auto idsToNames = readNames();
    auto nodeIds = getVertexVector(idsToNames);
    auto idsToIndices = getIdsToIndices(nodeIds);
    auto adjList = getAdjList(edges, idsToIndices);
    std::cout << idsToNames.size() << " nodes\n";
    std::cout << edges.size() << " edges\n";
    std::string erdosId = "cVeVZ1YAAAAJ";
    std::cout << "max Erdos numer " << findLargestErdosNumber(adjList, idsToIndices[erdosId]) << '\n';

    auto eigs = spectrum(2, adjList);
    Eigen::VectorXf evalues = eigs.eigenvalues();
    std::cout << "Eigenvalues found:\n" << evalues << std::endl;
    std::cout << "num iterations: " << eigs.num_iterations() << '\n';
    std::cout << "num operations: " << eigs.num_operations() << '\n';


    return 0;
}
