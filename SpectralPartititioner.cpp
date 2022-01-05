#include "SpectralPartititioner.h"

SpectralPartititioner::SpectralPartititioner(const std::string &filename) : Analyser(filename) {
    std::ifstream input;
    input.open(specDataFilename);
    if (input) {
        std::string line;
        getline(input, line);
        lambda1 = stod(line);
        getline(input, line);
        lambda2 = stod(line);
        while (getline(input, line))
            v2.push_back(stod(line));
    } else
        getSpecData();
}

void SpectralPartititioner::getSpecData() {
    //saves the second eigenvector and the second eigenvalue of normalized Laplacian matrix
    std::cout << "creating the normalized Laplacian matrix...\n";
    Eigen::SparseMatrix<double> L(size, size);
    L.reserve(
            Eigen::VectorXi::Constant(L.cols(), maxdeg));
    for (int i = 0; i < size; i++) {
        L.insert(i, i) = 1;
        for (int to: adjList[i]) {
            L.insert(i, to) = -1.0 / sqrt(adjList[i].size() * adjList[to].size());
        }
    }
    Spectra::SparseSymMatProd<double> op(L);
    Spectra::SymEigsSolver<double, Spectra::SMALLEST_ALGE, Spectra::SparseSymMatProd<double>> eigs(&op, 2, 30);
    eigs.init();
    std::cout << "computing the first two eigenvalues and eigenvectors...\n";
    eigs.compute(100, 1e-12);
    auto evalues = eigs.eigenvalues();
    auto evectors = eigs.eigenvectors();
    std::ofstream output;
    output.open(specDataFilename);
    output << std::setprecision(12) << evalues(1) << "\n";
    output << std::setprecision(12) << evalues(0) << "\n";
    for (int i = 0; i < size; i++)
        output << std::setprecision(12) << evectors(i, 0) << "\n";
    output.close();
    lambda1 = evalues(1);
    lambda2 = evalues(0);
    std::cout << "lambda1: " << lambda1 << '\n';
    std::cout << "lambda2: " << lambda2 << '\n';
}

std::tuple<double, int>
SpectralPartititioner::getPartitition(int threshold) { // the sizes of components are greater than threshold
    // returns a tuple with the number of vertices in the least component and the number of edges between the components
    // reached with the spectral decomposition
    std::cout << "starting spectral partitition...\n";
    std::vector<std::pair<double, int>> aux;
    for (int i = 0; i < size; i++)
        aux.push_back(std::pair<double, int>(v2[i], i));
    std::sort(aux.begin(), aux.end(),
              [](const std::pair<double, int> &a, const std::pair<double, int> &b) { return a.first < b.first; });

    //std::unordered_set<std::pair<int, int>, MyHash> cutEdges;
    double minConductance = MAXFLOAT;
    int cut = 0;
    int bestIndex = 0;
    std::vector<bool> component(size, false);
    for (int i = 0; i < size - 1 - threshold; i++) {
        int cur = aux[i].second;
        component[cur] = true;
        for (int v: adjList[cur]) {
            if (component[v])
                cut--;
            else
                cut++;
        }
        if ((i >= threshold) && (minConductance > double(cut) / double(std::min(i + 1, size - i - 1)))) {
            minConductance = double(cut) / double(std::min(i + 1, size - i - 1));
            bestIndex = i + 1;
        }
    }
    return std::tuple<double, int>(minConductance, std::min(bestIndex, size - bestIndex));
}
