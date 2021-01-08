#pragma once

#include <iostream>

static void cvLogCall(const char *file, int line)
{
    std::cerr << "CV method is called: " << file << ":" << line << '\n';
}

#define CVCALL cvLogCall(__FILE__, __LINE__)