#include "MatrixCopy.hpp"

#include "../CVmisc.hpp"

#include <iostream>

inline int clamp(int val, int min, int max)
{
    if (val < min) return min;
    if (val > max) return max;

    return val;
}

// Some strange buggy shit which I cannot track and resolve happens while using this function so I really need to think
// of deprecating or simply deleting it.
cv::Mat copySubMatrix(const cv::Mat &inp, int startRow, int startCol, int rowCount, int colCount)
{
    const cv::Mat rowRange = inp.rowRange(clamp(startRow, 0, inp.rows), clamp(startRow + rowCount, 0, inp.rows));

    return rowRange.colRange(clamp(startCol, 0, inp.cols), clamp(startCol + rowCount, 0, inp.cols));
}