#include "CameraAdapter.hpp"

auto vidIO::CameraAdapter::frameData() const -> const FrameData & {
    return fdat;
}
