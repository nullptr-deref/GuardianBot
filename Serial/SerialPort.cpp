#include "SerialPort.hpp"

#include "SerialPortImpl.cpp"

SerialPort::SerialPort(const char* portName, int mode, uint baudrate)
{
	pImpl = std::make_unique<SerialPortImpl>();
	pImpl->open(portName, mode, baudrate);
}

SerialPort::SerialPort(const char* portName, unsigned long mode, uint baudrate)
{
	pImpl = std::make_unique<SerialPortImpl>();
	pImpl->open(portName, mode, baudrate);
}

void SerialPort::open(const char *portName, int mode)
{
	pImpl->open(portName, mode);
}

void SerialPort::close()
{
	pImpl->close();
}

void SerialPort::write(const char *data, uint count)
{
	pImpl->write(data, count);
}

const char* SerialPort::read()
{
	return pImpl->read();
}