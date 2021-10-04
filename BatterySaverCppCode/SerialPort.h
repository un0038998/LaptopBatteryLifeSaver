#pragma once

#include <Windows.h>
#include <iostream>

class SerialPort {
private:
	HANDLE serialPortHandle;

public:
	SerialPort();
	~SerialPort();

	int connect(const char* device);
	void disconnect(void);

	int sendArray(const char* buffer, int len);
	int getArray(char* buffer, int len);

	void clear();
	bool isConnected();
};

