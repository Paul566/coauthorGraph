#include "SpectralPartitioner.h"

SpectralPartitioner::SpectralPartitioner(const std::string &adjList_filename, const std::string &ErdosIndex_filename)
        : Analyser(adjList_filename, ErdosIndex_filename) {
}

std::tuple<float, std::vector<float>> SpectralPartitioner::getSpecData(std::vector<int> ignoreList) {
    //returns the second eigenvalue and the second eigenvector of normalized Laplacian matrix
    std::sort(ignoreList.begin(), ignoreList.end());
    int ignoreSize = ignoreList.size();
    std::vector<bool> ignoreMask(size, false);
    for (int i: ignoreList)
        ignoreMask[i] = true;
    std::vector<std::vector<int>> tmpAdjList(size - ignoreSize);
    std::vector<int> newIndices(size);
    int tmpIndex = 0;
    for (int i = 0; i < size; i++) {
        if (ignoreMask[i])
            newIndices[i] = -1;
        else {
            newIndices[i] = tmpIndex;
            tmpIndex++;
        }
    }
    for (int i = 0; i < size; i++) {
        if (ignoreMask[i])
            continue;
        for (int to: adjList[i]) {
            if (!ignoreMask[to])
                tmpAdjList[newIndices[i]].push_back(newIndices[to]);
        }
    }

    float lambda2;
    std::vector<float> v2;
    std::tie(lambda2, v2) = getL2V2(tmpAdjList);
    for (int i = ignoreSize - 1; i >= 0; i--)
        v2.insert(v2.begin() + ignoreList[i], 0.);
    return std::tie(lambda2, v2);
}

std::tuple<float, std::vector<bool>>
SpectralPartitioner::getPartition(int threshold,
                                  const std::vector<float> &_v2) { // the sizes of components are greater than threshold
    // returns a tuple with the number of vertices in the least component and the number of edges between the components
    // reached with the spectral partition
    std::cout << "starting spectral partition...\n";
    std::vector<std::pair<float, int>> aux;
    for (int i = 0; i < size; i++)
        aux.emplace_back(_v2[i], i);
    std::sort(aux.begin(), aux.end(),
              [](const std::pair<float, int> &a, const std::pair<float, int> &b) { return a.first < b.first; });

    //std::unordered_set<std::pair<int, int>, MyHash> cutEdges;
    float minConductance = MAXFLOAT;
    int cut = 0;
    int partVolume = 0;
    int bestIndex = 0;
    int bestVolume = 0;
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
        partVolume += adjList[cur].size();
        if ((i >= threshold) && (minConductance > float(cut) / float(std::min(partVolume, volume - partVolume)))) {
            minConductance = float(cut) / float(std::min(partVolume, volume - partVolume));
            bestIndex = i + 1;
            bestVolume = partVolume;
        }
    }

    std::vector<bool> guess(size, false);
    if (bestVolume < volume / 2) {
        for (int i = 0; i < bestIndex; i++)
            guess[aux[i].second] = true;
    } else {
        guess = std::vector<bool>(size, true);
        for (int i = 0; i < bestIndex; i++)
            guess[aux[i].second] = false;
    }
    return std::tie(minConductance, guess);
}

void
SpectralPartitioner::saveSpecData(const std::string &specDataFilename, const std::vector<float> &_v2) {
    std::ofstream output;
    output.open(specDataFilename);
    for (float f: _v2)
        output << f << "\n";
    output.close();
}

std::tuple<float, std::vector<float>>
SpectralPartitioner::getL2V2(std::vector<std::vector<int>> &_adjList) {
    std::cout << "constructing the Laplacian matrix...\n";
    std::vector<float> _v2;
    int _size = _adjList.size();
    Eigen::SparseMatrix<float> L(_size, _size);
    L.reserve(Eigen::VectorXi::Constant(L.cols(), maxdeg));
    for (int i = 0; i < _size; i++) {
        //L.insert(i, i) = 1; for normalized Laplacian
        L.insert(i, i) = _adjList[i].size();
        for (int to: _adjList[i])
            L.insert(i, to) = -1.;
    }
    Spectra::SparseSymMatProd<float> op(L);
    Spectra::SymEigsSolver<float, Spectra::SMALLEST_ALGE, Spectra::SparseSymMatProd<float>> eigs(&op, 2, 100);
    eigs.init();
    std::cout << "computing the first two eigenvalues and eigenvectors...\n";
    eigs.compute(100, 1e-7);
    auto evalues = eigs.eigenvalues();
    auto evectors = eigs.eigenvectors();
    std::cout << evectors.rows() << " " << evectors.cols() << '\n';
    for (int i = 0; i < _size; i++)
        _v2.push_back(float(evectors(i, 0)));
    float _lambda2 = evalues(0);
    return std::tie(_lambda2, _v2);
}

std::vector<float> SpectralPartitioner::readV2(const std::string &filename) {
    std::vector<float> v2;
    std::ifstream input;
    input.open(filename);
    std::string line;
    while (getline(input, line))
        v2.push_back(stod(line));
    return v2;
}
