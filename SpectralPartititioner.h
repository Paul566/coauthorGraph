#ifndef COLLECT_PY_SPECTRALPARTITITIONER_H
#define COLLECT_PY_SPECTRALPARTITITIONER_H


#include "Analyser.h"
#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <Spectra/SymEigsSolver.h>
#include <Spectra/MatOp/SparseSymMatProd.h>
#include <sys/stat.h>
#include <unistd.h>

class SpectralPartititioner : public Analyser {
private:
    class MyHash {
    public:
        std::size_t operator()(const std::pair<int, int> &p) const {
            return std::hash<int>()(p.first) ^ std::hash<int>()(p.second);
        }
    };

    std::string specDataFilename = "../specData";

    void getSpecData();

public:
    double lambda2; //the second smallest eigenvalue of the normalized Laplacian matrix
    double lambda1; //should be around zero
    std::vector<double> v2; //the eigenvector corresponding to lambda2
    SpectralPartititioner(const std::string &filename);

    std::tuple<double, int> getPartitition(int threshold = 0);
};


#endif //COLLECT_PY_SPECTRALPARTITITIONER_H
