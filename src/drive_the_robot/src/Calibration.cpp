#include "Calibration.h"
#include <fstream>
#include <iostream>

/**
 * @brief Write vector of transforms to target path
 *
 * @param path Output file path
 * @param transforms Camera pose relative to camera 0 in order of camera id
 */
void Calibration::writeFile(const std::string& path, const std::vector<Eigen::MatrixXd>& transforms)
{
    int nCameras = transforms.size();
    std::cout << nCameras << std::endl;
    std::ofstream out(path);
    out << nCameras << "\n";
    for (int i = 0; i < nCameras; i++) {
        writeMatrix(out, transforms[i]);
        
    }
    if(!out.good()){
        std::cerr << "Writing to file " << path << " failed" << std::endl;
    }
    out.close();
}

/**
 * @brief Read in calibration file for Kinect camera poses
 *
 * @param path Input file path
 * @return std::vector<Eigen::MatrixXd> Camera pose relative to camera 0 in order of camera id
 */
std::vector<Eigen::MatrixXd> Calibration::readFile(const std::string& path)
{
    std::ifstream in(path);
    int nCameras;
    in >> nCameras;
    if (!in.good()) {
        std::cerr << "Failed to get number of cameras from file, is file path & format correct?" << std::endl;
        exit(1);
    }
    
    std::vector<Eigen::MatrixXd> res;
    for (int i = 0; i < nCameras; i++) {
        res.push_back(readMatrix(in, 4, 4));
    }
    return res;
}

void Calibration::writeMatrix(std::ostream& out, const Eigen::MatrixXd& matrix)
{
    for (int row = 0; row < matrix.rows(); row++) {
        for (int col = 0; col < matrix.cols(); col++) {
            if (col > 0) {
                out << " ";
            }
            out << matrix(row, col);
        }
        out << "\n";
    }
}

Eigen::MatrixXd Calibration::readMatrix(std::istream& input, int rows, int cols)
{
    Eigen::MatrixXd mat(rows, cols);
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            input >> mat(row, col);
        }
    }
    return mat;
}