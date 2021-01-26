typedef unsigned int uint;

#include <stdexcept>

#ifdef _WIN32

#include <Windows.h>

class SerialPortImpl
{
public:
    SerialPortImpl() = default;
    ~SerialPortImpl() = default;

    void open(const char *portName, int mode, uint baudrate = 9600)
    {
        m_hCom = CreateFileA(portName, mode, false, nullptr, CREATE_NEW, OPEN_EXISTING, nullptr);

        if (INVALID_HANDLE_VALUE == m_hCom)
        {
            throw std::runtime_error("Cannot connect to serial port [" + std::string(portName) + ']');
        }

        DCB m_serialParams { 0 };
        m_serialParams.DCBlength = sizeof(m_serialParams);

        const bool currComStatusRetrieved = GetCommState(m_hCom, &m_serialParams);

        if (false == currComStatusRetrieved)
        {
            throw std::runtime_error("Could not retrieve serial port status.");
        }

        m_serialParams.BaudRate = baudrate;
        m_serialParams.fBinary = true;
        m_serialParams.fParity = true;
        m_serialParams.StopBits = ONESTOPBIT;
        m_serialParams.Parity = PARITY_NONE;

        SetCommState(m_hCom, &m_serialParams);

        COMMTIMEOUTS timeouts = { 0 };
        timeouts.ReadIntervalTimeout = 50;
        timeouts.ReadTotalTimeoutConstant = 50;
        timeouts.ReadTotalTimeoutMultiplier = 50;
        timeouts.WriteTotalTimeoutConstant = 50;
        timeouts.WriteTotalTimeoutMultiplier = 10;

        SetCommTimeouts(m_hCom, &timeouts);
    }

    void write(const char *data, uint count)
    {
        DWORD bytesWritten = 0;
        const bool isWritten = WriteFile(m_hCom, data, count, &bytesWritten, nullptr);

        if (!isWritten)
        {
            throw std::runtime_error("Could not write to serial port.");
        }
    }

    const char *read()
    {
        char buf[64];
        DWORD bytesRead = 0;
        const bool isRead = ReadFile(m_hCom, buf, 64, &bytesRead, nullptr);

        if (!isRead)
        {
            throw std::runtime_error("Could not read from the serial port.");
        }

        return buf;
    }

    void close()
    {
        bool isClosed = CloseHandle(m_hCom);

        if (!isClosed)
        {
            throw std::runtime_error("Could not close the serial port.");
        }
    }

private:
    HANDLE m_hCom;
    DCB m_serialParams;
};

#endif

#ifdef __unix__

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

class SerialPortImpl
{
public:
    void open(const char *portName, int mode, int baudrate = 9600)
    {
        m_fDescriptor = open(portName, mode);

        if (-1 == fDescriptor)
        {
            throw std::runtime_error("Could not open the serial port [" + portName + ']');
        }

        fnctl(m_fDescriptor, F_SETFL, O_RDWR);

        tcgetattr(m_fDescriptor, &m_opts);

        cfsetispeed(&m_opts, baudrate);
        cfsetospeed(&m_opts, baudrate);

        m_opts.c_cflag |= (CLOCAL | CREAD);

        tcsetattr(m_fDescriptor, TCSANOW, &m_opts);
    }

    const char *read()
    {
        char buf[64];
        const int bytesRead = read(m_fDescriptor, buf, 64);

        if (-1 == bytesRead)
        {
            throw std::runtime_error("Could not read from serial port.");
        }

        return buf;
    }

    void write(const char* buf, uint count)
    {
        const int bytesSent = write(m_fDescriptor, buf, count);

        if (-1 == bytesSent)
        {
            throw std::runtime_error("Could not write to serial port.");
        }
    }

    void close()
    {
        close(m_fDescriptor);
    }

private:
    int m_fDescriptor;
    struct termios m_opts;
};
#endif