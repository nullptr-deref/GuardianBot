#include "imgcmp.hpp"

float imgcmp::compareImagesByPixel(const Image &im1, const Image &im2)
{
    float similarity = 1.0f;
    uint32_t differentPixels = 0;

    const float similarityFactor = 1.0f / (im1.size[0] * im1.size[1]);

    for (size_t y = 0; y < im1.size[0]; ++y)
    {
        for (size_t x = 0; x < im1.size[1]; ++x)
        {
            if (im1.at<unsigned char>(static_cast<int>(y), static_cast<int>(x)) != im2.at<unsigned char>(static_cast<int>(y), static_cast<int>(x)))
            {
                similarity -= similarityFactor;
                differentPixels++;
            }
        }
    }

    return similarity;
}

float imgcmp::compareImagesByRegions(const Image &im1, const Image &im2, size_t regStep, uint32_t treshold)
{
    const size_t rowsResidue = im1.rows % regStep != 0 ? 1 : 0;
    const size_t colsResidue = im1.cols % regStep != 0 ? 1 : 0;
    cv::Mat im1Weights = cv::Mat::zeros({ static_cast<int>(im1.rows / regStep + rowsResidue), static_cast<int>(im1.cols / regStep + colsResidue) }, CV_32S);
    cv::Mat im2Weights = cv::Mat::zeros({ static_cast<int>(im2.rows / regStep + rowsResidue), static_cast<int>(im2.cols / regStep + colsResidue) }, CV_32S);

    for (size_t rx = 0; rx < im1Weights.cols; ++rx)
    {
        for (size_t ry = 0; ry < im1Weights.rows; ++ry)
        {
            for (size_t x = 0; x < regStep; ++x)
            {
                for (size_t y = 0; y < regStep; ++y)
                {
                    const size_t xLoc = rx * regStep + x;
                    const size_t yLoc = ry * regStep + y;
                    const unsigned char val1 = im1.at<unsigned char>(static_cast<int>(yLoc), static_cast<int>(xLoc));
                    const unsigned char val2 = im2.at<unsigned char>(static_cast<int>(yLoc), static_cast<int>(xLoc));

                    im1Weights.at<int>(static_cast<int>(ry), static_cast<int>(rx)) += val1;
                    im2Weights.at<int>(static_cast<int>(ry), static_cast<int>(rx)) += val2;
                }
            }
        }
    }

    float similarity = 1.0f;
    const float similarityDecr = 1.0f / (im1Weights.rows * im1Weights.cols);
    for (size_t y = 0; y < im1Weights.rows; ++y)
    {
        for (size_t x = 0; x < im1Weights.cols; ++x)
        {
            const int weight1 = im1Weights.at<int>(static_cast<int>(y), static_cast<int>(x));
            const int weight2 = im2Weights.at<int>(static_cast<int>(y), static_cast<int>(x));

            if (std::abs(weight1 - weight2) > static_cast<int>(treshold))
                similarity -= similarityDecr;
        }
    }

    return similarity;
}

int main() { return 0; }