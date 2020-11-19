#ifndef GUID_DEVINTERFACE_USB_DEVICE
#include <initguid.h>
#include <usbiodef.h>
#endif

#include <iostream>

#include <librealsense2/rs.hpp>
#include <opencv2/highgui.hpp>

int main()
{
    rs2::context ctx;
    cv::namedWindow("Main window", cv::WindowFlags::WINDOW_NORMAL);
    auto list = ctx.query_devices();
    if (list.size() == 0) {
        std::cerr << "No devices connected.\n";
        return 0;
    }
    rs2::device dev = list.front();
    return 0;
}