#ifndef COLLECT_PY_SPECTRALPARTITIONER_H
#define COLLECT_PY_SPECTRALPARTITIONER_H


#include "Analyser.h"
#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <Spectra/SymEigsSolver.h>
#include <Spectra/MatOp/SparseSymMatProd.h>
#include <sys/stat.h>
#include <unistd.h>

class SpectralPartitioner : public Analyser {
private:
    class MyHash {
    public:
        std::size_t operator()(const std::pair<int, int> &p) const {
            return std::hash<int>()(p.first) ^ std::hash<int>()(p.second);
        }
    };

public:
    SpectralPartitioner(const std::string &adjList_filename, const std::string &ErdosIndex_filename);

    std::tuple<float, std::vector<bool>> getPartition(int threshold, const std::vector<float> &_v2);

    std::tuple<float, std::vector<float>> getSpecData(std::vector<int> ignoreList);

    std::tuple<float, std::vector<float>> getL2V2(std::vector<std::vector<int>> &_adjList);

    void saveSpecData(const std::string &specDataFilename, const std::vector<float> &_v2);

    std::vector<float> readV2(const std::string &filename);
};


#endif //COLLECT_PY_SPECTRALPARTITIONER_H
