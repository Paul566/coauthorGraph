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

    void getSpecData(const std::string &specDataFilename);

public:
    double lambda2; //the second smallest eigenvalue of the normalized Laplacian matrix
    double lambda1; //should be around zero
    std::vector<float> v2; //the eigenvector corresponding to lambda2
    SpectralPartitioner(const std::string &specDataFilename, const std::string &adjList_filename,
                        const std::string &ErdosIndex_filename);

    std::tuple<float, int> getPartition(int threshold = 0);
};


#endif //COLLECT_PY_SPECTRALPARTITIONER_H
