#pragma once

#include <string>
#include <vector>
#include <memory>

#ifdef _WIN32
#include <Windows.h>

enum class SerialMode : unsigned long {
    Read = GENERIC_READ,
    Write = GENERIC_WRITE,
    ReadWrite = Read | Write
};

#endif

const size_t MAX_DATA_SIZE = 128;
struct SerialReadData {
    char data[MAX_DATA_SIZE];
    size_t actualSize = 0;
};

class SerialPort
{
public:
    SerialPort(const std::string &portName, SerialMode mode = SerialMode::ReadWrite, uint32_t baudrate = 9600u);

    void open();
    void close();
    SerialReadData read();
    void write(const char *data, uint32_t count);
    static std::vector<std::string> queryAvailable();

private:
    std::string name;
    uint32_t baudrate;
    SerialMode mode;
    HANDLE m_hCom;
    DCB m_serialParams;
};