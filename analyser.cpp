#include <iostream>
#include "json.hpp"
#include <fstream>
#include "string"
#include "vector"
#include "unordered_map"
#include "unordered_set"
#include "queue"
#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <Spectra/SymEigsSolver.h>
#include <Spectra/MatOp/SparseSymMatProd.h>
#include <iomanip>

using json = nlohmann::json;

class MyHash {
public:
    std::size_t operator()(const std::pair<int, int> &p) const {
        return std::hash<int>()(p.first) ^ std::hash<int>()(p.second);
    }
};

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

void writeV2L2(std::vector<std::vector<int>> adjList) {
    //saves the second eigenvector and the second eigenvalue of normalized Laplacian matrix to files
    int n = 2;
    int size = adjList.size();
    Eigen::SparseMatrix<double> L(size, size);
    L.reserve(
            Eigen::VectorXi::Constant(L.cols(), 100)); //100 is almost surely twice greater than degree of a given node
    for (int i = 0; i < size; i++) {
        L.insert(i, i) = 1;
        for (int to : adjList[i]) {
            L.insert(i, to) = -1.0 / sqrt(adjList[i].size() * adjList[to].size());
        }
    }
    Spectra::SparseSymMatProd<double> op(L);
    int ncv = 200;
    int maxit = 10;
    double tol = 1e-12;
    Spectra::SymEigsSolver<double, Spectra::SMALLEST_ALGE, Spectra::SparseSymMatProd<double>> eigs(&op, n, ncv);
    std::cout << "ncv: " << ncv << ", maxit: " << maxit << ", tol: " << tol << "\n";
    eigs.init();
    eigs.compute(maxit, tol);
    auto evalues = eigs.eigenvalues();
    auto evectors = eigs.eigenvectors();
    std::ofstream outputv2;
    outputv2.open("v2");
    for (int i = 0; i < size; i++) {
        outputv2 << std::setprecision(17) << evectors(i, 0) << '\n';
    }
    outputv2.close();
    std::cout << "lambda1: " << evalues(1) << '\n';
    std::cout << "lambda2: " << evalues(0) << '\n';
    std::cout << "num iterations: " << eigs.num_iterations() << '\n';
    std::cout << "num operations: " << eigs.num_operations() << '\n';
    std::ofstream outputl2;
    outputl2.open("lambda2");
    outputl2 << std::setprecision(17) << evalues(0);
    outputl2.close();
}

std::vector<double> readV2() {
    std::vector<double> ans;
    std::ifstream inputfile;
    inputfile.open("v2");
    std::string line;
    while (getline(inputfile, line))
        ans.push_back(stod(line));
    return ans;
}

std::pair<double, int> spectralDecomposition(std::vector<std::vector<int>> adjList, std::vector<double> v2) {
    //returns a pair of the least conductance reached with the spectral decomposition and the number of vertices in the cut
    int size = adjList.size();
    std::vector<std::pair<double, int>> kostyl;
    for (int i = 0; i < size; i++)
        kostyl.push_back(std::pair<double, int>(v2[i], i));
    std::sort(kostyl.begin(), kostyl.end(),
              [](const std::pair<double, int> &a, const std::pair<double, int> &b) {
                  return a.first < b.first;
              });

    std::unordered_set<std::pair<int, int>, MyHash> d;
    double minConductance = 100; //surely greater than actual conductances
    int bestIndex = 0;
    for (int i = 0; i < size - 1; i++) {
        int cur = kostyl[i].second;
        for (int v : adjList[cur]) {
            if (d.find(std::pair<int, int>(v, cur)) == d.end())
                d.insert(std::pair<int, int>(cur, v));
            else
                d.erase(std::pair<int, int>(v, cur));
        }
        if (minConductance > double(d.size()) / double(std::min(i + 1, size - i - 1))) {
            minConductance = double(d.size()) / double(std::min(i + 1, size - i - 1));
            bestIndex = i + 1;
        }
    }
    return std::pair<double, int>(minConductance, std::min(bestIndex, size - bestIndex));
}

std::vector<bool> getSubsetV2(std::vector<std::vector<int>> adjList, std::vector<double> v2, int k) {
    //returns a subset of k vertices that have the least (v2, e_i) value
    int size = adjList.size();
    std::vector<double> copyV2(v2);
    std::sort(copyV2.begin(), copyV2.end());
    double critVal = copyV2[k - 1];
    std::vector<bool> ans(size, 0);
    for (int i = 0; i < size; i++) {
        if (v2[i] <= critVal)
            ans[i] = true;
    }
    return ans;
}

double getEOverV(std::vector<std::vector<int>> adjList, std::vector<bool> subset) {
    //returns E(S, V\S) / |S| for given subset of vertices S
    int e = 0;
    int v = 0;
    int size = adjList.size();
    for (int i = 0; i < size; i++) {
        if (subset[i]) {
            v++;
            for (int v : adjList[i]) {
                if (!subset[v])
                    e++;
            }
        }
    }
    if (v == 0) {
        std::cout << "The subset is empty\n";
        return 0;
    }
    return double(e) / double(v);
}

int main() {
    std::cout.precision(12);
    auto edges = readEdges();
    auto idsToNames = readNames();
    auto nodeIds = getVertexVector(idsToNames);
    auto idsToIndices = getIdsToIndices(nodeIds);
    auto adjList = getAdjList(edges, idsToIndices);
    std::cout << idsToNames.size() << " nodes\n";
    std::cout << edges.size() << " edges\n";
    std::string erdosId = "cVeVZ1YAAAAJ";
    std::cout << "Max Erdos numer " << findLargestErdosNumber(adjList, idsToIndices[erdosId]) << '\n';
    writeV2L2(adjList);
    auto v2 = readV2();
    auto sd = spectralDecomposition(adjList, v2);
    std::cout << "Cheeger constant is less or equals to " << sd.first << " with " << sd.second
              << " vertices in the subset\n";
    return 0;
}
