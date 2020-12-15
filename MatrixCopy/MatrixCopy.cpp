#include "MatrixCopy.hpp"

cv::Mat copySubMatrix(const cv::Mat &inp, int startRow, int startCol, int rowCount, int colCount)
{
    const cv::Mat rowRange = inp.rowRange(startRow, startRow + rowCount);

    return rowRange.colRange(startCol, startCol + rowCount);
}