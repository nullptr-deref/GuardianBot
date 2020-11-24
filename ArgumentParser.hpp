#pragma once

#include <string>

class ArgumentParser
{
public:
    ArgumentParser(int argc, char **argv)
        : m_argc(argc) {}
private:
    int m_argc;
    
};