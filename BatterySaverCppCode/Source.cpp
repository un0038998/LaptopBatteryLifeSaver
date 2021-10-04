#include <iostream>
#include <string>
#include "Windows.h"
#include <iostream>
#include <stdio.h>
#include "SerialPort.h"

using namespace std;

SerialPort arduinoPort;

int main() 
{
	std::string  inputPortName = "";
	std::cout << "Enter Com port name:";
	getline(cin, inputPortName);
	std::string  finalInputPortName = "\\\\.\\" + inputPortName;

	while (1)
	{
		std::string batteryStatus = "";

		SYSTEM_POWER_STATUS status; // note not LPSYSTEM_POWER_STATUS
		GetSystemPowerStatus(&status);

		if (status.ACLineStatus == 1) 
		{
			batteryStatus = "Charging";
		}
		else if (status.ACLineStatus == 0)
		{
			batteryStatus = "Not charging";
		}

		cout << "\nStatus: " << batteryStatus << ".  Battery left:" << int(status.BatteryLifePercent) << "%" << endl; //    hours left : " << status.BatteryLifeTime / 3600.00 << endl;
		while (!arduinoPort.isConnected())
		{
			Sleep(1000);
			std::cout << "Connecting to port " << finalInputPortName << std::endl << std::flush;
			arduinoPort.connect(finalInputPortName.c_str());
		}
		std::string batteryPercentageStr = std::to_string(int(status.BatteryLifePercent));
		batteryPercentageStr = batteryPercentageStr + '\n';
		cout << "Sent size:" << arduinoPort.sendArray(batteryPercentageStr.c_str(), batteryPercentageStr.length()) << std::endl;
		/*
		cout << "Read size:" << arduinoPort.getArray(readBuffer, 5) << std::endl;
		std::cout << readBuffer << std::endl;
		*/
		Sleep(5000);
	}
}

