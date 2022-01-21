#include "SerialPort.hpp"

#include <stdexcept>

SerialPort::SerialPort(const std::string &portName, int mode, uint32_t baudrate)
: name(portName), mode(mode), baudrate(baudrate) {}

SerialPort::SerialPort(const std::string &portName, unsigned long mode, uint32_t baudrate)
: name(portName), mode(mode), baudrate(baudrate) {}

void SerialPort::open()
{
	m_hCom = CreateFileA(name.c_str(), mode, false, nullptr, CREATE_NEW, OPEN_EXISTING, nullptr);

	if (INVALID_HANDLE_VALUE == m_hCom) throw std::runtime_error("Cannot connect to serial port [" + name + ']');

	DCB m_serialParams { 0 };
	m_serialParams.DCBlength = sizeof(m_serialParams);

	const bool currComStatusRetrieved = GetCommState(m_hCom, &m_serialParams);

	if (!currComStatusRetrieved) throw std::runtime_error("Could not retrieve serial port status.");

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

void SerialPort::close()
{
	if(this != nullptr)
	{
		bool isClosed = CloseHandle(m_hCom);
		if (!isClosed) throw std::runtime_error("Could not close the serial port.");
	}
}

void SerialPort::write(const char *data, uint32_t count)
{
	DWORD bytesWritten = 0;
	const bool isWritten = WriteFile(m_hCom, data, count, &bytesWritten, nullptr);
	if (!isWritten) throw std::runtime_error("Could not write to serial port.");
}

const char* SerialPort::read()
{
	char buf[64];
	DWORD bytesRead = 0;
	const bool isRead = ReadFile(m_hCom, buf, 64, &bytesRead, nullptr);
	if (!isRead) throw std::runtime_error("Could not read from the serial port.");

	return buf;
}

using PortName = std::string;
std::vector<std::string> SerialPort::queryAvailable()
{
	std::vector<PortName> availablePorts;
	const size_t BUF_LENGTH = 1024u;
	char *buf = new char[BUF_LENGTH];
	for (size_t i = 0; i < 256; i++)
	{
		const PortName portName = "COM" + std::to_string(i);
		DWORD res = QueryDosDevice(portName.c_str(), buf, BUF_LENGTH);
		if (res > 0) availablePorts.push_back(portName);
	}

	return availablePorts;
}