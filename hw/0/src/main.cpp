#include <fmt/core.h>

#include <algorithm>
#include <cmath>
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <numbers>
#include <numeric>
#include <opencv2/opencv.hpp>
#include <vector>

int main() {
  fmt::print("Hello World!\n");

  Eigen::Matrix3d m;
  m << 1, 2, 3, 4, 5, 6, 7, 8, 9;

  std::cout << m << std::endl;

  return 0;
}