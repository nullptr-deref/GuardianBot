#pragma once

#include <string>
#include <vector>
#include <memory>

#ifdef _WIN32
#include <Windows.h>

namespace SerialMode
{
    inline unsigned long Read = GENERIC_READ;
    inline unsigned long Write = GENERIC_WRITE;
    inline unsigned long ReadWrite = Read | Write;
}

#endif

#ifdef __unix__

#include <fnctl.h>

namespace SerialMode
{
    int Read = O_RDONLY;
    int Write = O_WRONLY;
    int ReadWrite = O_RDWR;
}

#endif

class SerialPortImpl;

typedef unsigned int uint;

class SerialPort
{
public:
    SerialPort(const char *portName, int mode = SerialMode::ReadWrite, uint baudrate = 9600u);
    SerialPort(const char *portName, unsigned long mode = SerialMode::ReadWrite, uint baudrate = 9600u);

    void open(const char *portName, int mode);
    void close();
    const char *read();
    void write(const char *data, uint count);
    std::vector<std::string> queryAvailable();

private:
    std::unique_ptr<SerialPortImpl> pImpl = nullptr;
};