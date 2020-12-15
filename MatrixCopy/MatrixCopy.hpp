#pragma once

#include <opencv2/imgproc.hpp>

cv::Mat copySubMatrix(const cv::Mat &inp, int startRow, int startCol, int rowCount, int colCount);