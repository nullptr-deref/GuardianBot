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


class SerialPort
{
public:
    SerialPort(const std::string &portName, int mode = SerialMode::ReadWrite, uint32_t baudrate = 9600u);
    SerialPort(const std::string &portName, unsigned long mode = SerialMode::ReadWrite, uint32_t baudrate = 9600u);

    void open();
    void close();
    const char *read();
    void write(const char *data, uint32_t count);
    static std::vector<std::string> queryAvailable();

private:
    // std::unique_ptr<SerialPortImpl> pImpl = nullptr;
    std::string name;
    uint32_t baudrate;
    unsigned long mode;
    HANDLE m_hCom;
    DCB m_serialParams;
};