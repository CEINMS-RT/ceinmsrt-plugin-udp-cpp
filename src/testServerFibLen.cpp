#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <iostream>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "8945"

int main(void)
{
	SOCKET socketS;

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	struct sockaddr_in local;
	struct sockaddr_in from;
	int fromlen = sizeof(from);
	local.sin_family = AF_INET;
	local.sin_port = htons(1237);
	local.sin_addr.s_addr = INADDR_ANY;

	socketS = socket(AF_INET, SOCK_DGRAM, 0);
	bind(socketS, (sockaddr*)&local, sizeof(local));

	getsockname(socketS, (SOCKADDR *)&local, (int *)sizeof(local));
	printf("Server: Receiving IP(s) used: %s\n", inet_ntoa(local.sin_addr));
	printf("Server: Receiving port used: %d\n", htons(local.sin_port));
	printf("Server: I\'m ready to receive a datagram...\n");

	std::vector<double> data;
	data.push_back(3);
	std::vector<char> dataSend;
	//dataSend.resize(2);
	dataSend.resize(sizeof(double)*data.size());
	memcpy(dataSend.data(), data.data(), sizeof(double)*data.size());

	std::cout << dataSend.size() << std::endl;

	char ReceiveBuf[1024];
	ZeroMemory(ReceiveBuf, sizeof(ReceiveBuf));


	std::vector<double> tempData;
	tempData.resize(1);

	while (1)
	{
		recvfrom(socketS, ReceiveBuf, sizeof(double), 0, (sockaddr*)&from, &fromlen);
		memcpy(tempData.data(), ReceiveBuf, sizeof(double));
		std::cout << tempData[0] << std::endl;
		Sleep(100);
	}
	closesocket(socketS);
	WSACleanup();

	return 0;
}



