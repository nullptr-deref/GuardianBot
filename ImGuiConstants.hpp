#pragma once

namespace imguic
{
    const unsigned int BUFFER_SIZE = 128u;
    const float DEFAULT_WIDTH = 512;
    const float DEFAULT_HEIGHT = 256;
    // watcher definitions
    namespace watcher
    {
        const float x = 0;
        const float y = 0;
        const float w = DEFAULT_WIDTH;
        const float h = 64;
    }

    // controller definitions
    namespace controller
    {
        const float x = 0;
        const float y = watcher::h + 2;
        const float w = DEFAULT_WIDTH;
        const float h = 80;
        const float btnW = 35;
        const float btnH = 20;
    }
}