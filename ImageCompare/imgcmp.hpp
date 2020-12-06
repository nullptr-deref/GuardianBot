#pragma once

#include <opencv2/imgproc.hpp>

using Image = cv::Mat;

namespace imgcmp
{
    float compareImagesByPixel(const Image &im1, const Image &im2);
    float compareImagesByRegions(const Image &im1, const Image &im2, size_t regStep, uint32_t treshold = 10000u);
}