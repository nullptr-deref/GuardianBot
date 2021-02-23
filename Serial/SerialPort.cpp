#include "SerialPort.hpp"

#include "SerialPortImpl.cpp"

SerialPort::SerialPort(const std::string &portName, int mode, uint baudrate)
{
	pImpl = std::make_unique<SerialPortImpl>(portName, mode, baudrate);
}

SerialPort::SerialPort(const std::string &portName, unsigned long mode, uint baudrate)
{
	pImpl = std::make_unique<SerialPortImpl>(portName, mode, baudrate);
}

void SerialPort::open()
{
	pImpl->open();
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

std::vector<std::string> SerialPort::queryAvailable()
{
	return SerialPortImpl::queryAvailable();
}