#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <Eigen/Dense>
#include <vector>
#include <string>

// Only a class and not namespace in case we want to add state later
/**
 * @brief Represents extrinsic calibration file for set of Azure Kinects,
 *        consists of 4x4 relative transforms in plain text format
 */
class Calibration {
public:
    static void writeFile(const std::string& path, const std::vector<Eigen::MatrixXd>&);
    static std::vector<Eigen::MatrixXd> readFile(const std::string&);
private:
    static void writeMatrix(std::ostream& out, const Eigen::MatrixXd& matrix);
    static Eigen::MatrixXd readMatrix(std::istream& input, int rows, int cols);
};

#endif