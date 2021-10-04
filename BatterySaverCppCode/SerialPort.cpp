#include "SerialPort.h"
#include <system_error>

std::string GetLastErrorAsString() 
{
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0) 
    {
        return std::string(); //No error message has been recorded
    }
    else 
    {
        return std::system_category().message(errorMessageID);
    }
}

SerialPort::SerialPort() 
{
    serialPortHandle = INVALID_HANDLE_VALUE;
}

SerialPort::~SerialPort() 
{
    if (serialPortHandle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(serialPortHandle);
    }
    serialPortHandle = INVALID_HANDLE_VALUE;
}


int SerialPort::connect(const char* device) 
{
    int error = 0;

    DCB dcb;
    memset(&dcb, 0, sizeof(dcb));
    dcb.DCBlength = sizeof(dcb);
    dcb.BaudRate = 9600;
    dcb.Parity = NOPARITY;
    dcb.fParity = 0;
    dcb.StopBits = ONESTOPBIT;
    dcb.ByteSize = 8;

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    serialPortHandle = CreateFileA(static_cast<LPCSTR>(device), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, NULL, NULL);

    //std::cout << "Error is " << GetLastErrorAsString() << std::endl;
    if (serialPortHandle != INVALID_HANDLE_VALUE) 
    {
        if (!SetCommState(serialPortHandle, &dcb))
        {
            error = 2;
        }
        else
        {
            if (!SetCommTimeouts(serialPortHandle, &timeouts))
            {
                error = 2;
            }
        }
    }
    else 
    {
        error = 1;
    }

    if (error != 0) 
    {
        disconnect();
    }
    else 
    {
        clear();
    }

    return error;
}

void SerialPort::disconnect(void) 
{
    if (serialPortHandle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(serialPortHandle);
        serialPortHandle = INVALID_HANDLE_VALUE;
    }
}

int SerialPort::sendArray(const char* buffer, int len)
{
    unsigned long result = 0;

    if (serialPortHandle != INVALID_HANDLE_VALUE)
    {
        WriteFile(serialPortHandle, buffer, len, &result, NULL);
        if (result == 0)
        {
            disconnect();
            std::cout << "Disconnecting .." << std::endl;
        }
    }

    return result;
}

int SerialPort::getArray(char* buffer, int len) 
{
    unsigned long read_nbr;

    read_nbr = 0;
    if (serialPortHandle != INVALID_HANDLE_VALUE)
    {
        ReadFile(serialPortHandle, buffer, len, &read_nbr, NULL);
    }

    return((int)read_nbr);
}

void SerialPort::clear() 
{
    PurgeComm(serialPortHandle, PURGE_RXCLEAR | PURGE_TXCLEAR);
}

// Checking if serial port is connected
bool SerialPort::isConnected()
{
    if (serialPortHandle != INVALID_HANDLE_VALUE)
    {
        COMSTAT status;
        DWORD errors;

        int statusValue = ClearCommError(serialPortHandle, &errors, &status);
        if (!statusValue)
        {
            disconnect();
            serialPortHandle = INVALID_HANDLE_VALUE;
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        return false;
    }
}

