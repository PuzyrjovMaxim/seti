#define _WINSOCK_DEPRECATED_NO_WARNINGS  
#include <iostream>
#include <winsock2.h>
#include <string>
#pragma comment(lib, "Ws2_32.lib")
using namespace std;
u_short MY_PORT = 666;
#define PRINTNUSERS if (nclients) cout << "user on-line" << nclients << endl; else cout << "NO User on-line\n";


struct Person {
	int weight;
	int height;
	char secondName[25];
}P;

double getBMI(int height, int weight) {
	return double(weight * 10000) / (height * height);
}

DWORD WINAPI ConToClient(LPVOID client_socket);

int nclients = 0;

int main() {
	char buff[1024];
	cout << "TCP ECHO-SERVER \n";
	if (WSAStartup(0x0202, (WSADATA*)&buff[0])) {
		cout << "Error WSAStartup \n" << WSAGetLastError();
		return 1;
	}

	SOCKET mysocket;
	if ((mysocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		cout << "Error socket \n" << WSAGetLastError();
		WSACleanup();
		return -1;
	}

	sockaddr_in local_addr;
	local_addr.sin_addr.s_addr = 0;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(MY_PORT);
	if (bind(mysocket, (sockaddr*)&local_addr, sizeof(local_addr))) {
		cout << "Error bind \n" << WSAGetLastError();
		closesocket(mysocket);
		WSACleanup();
		return -1;
	}

	if (listen(mysocket, 0x100)) {
		cout << "Error listen \n" << WSAGetLastError();
		closesocket(mysocket);
		WSACleanup();
		return -1;
	}

	cout << "Waiting connections\n";
	SOCKET client_socket;
	sockaddr_in client_addr;
	int client_addr_size = sizeof(client_addr);

	while ((client_socket = accept(mysocket, (sockaddr*)&client_addr, &client_addr_size))) {
		nclients++;
		HOSTENT* hst;
		hst = gethostbyaddr((char*)&client_addr.sin_addr.s_addr, 4, AF_INET);
		cout << "+new connect!\n";
		if (hst) cout << hst->h_name;
		else cout << "";
		cout << inet_ntoa(client_addr.sin_addr);
		PRINTNUSERS
			cout << endl;
		DWORD thID;
		CreateThread(NULL, NULL, ConToClient, &client_socket, NULL, &thID);
	}
	return 0;
}

DWORD WINAPI ConToClient(LPVOID client_socket) {
	SOCKET my_sock;
	int len;
	my_sock = ((SOCKET*)client_socket)[0];
	char buff[1024];

	while (SOCKET_ERROR != (len = recv(my_sock, (char*)&P, sizeof(P), 0))) {
		cout << "New Client was connected!" << endl;
		double BMI = getBMI(P.height, P.weight);
		cout << "Received Data" << endl;
		cout << "Second Name:" << P.secondName << "; Height:" << P.height << "; Weight:" << P.weight << endl;
		cout << BMI << endl;
		send(my_sock, (char*)&BMI, sizeof(BMI), 0);
	}
	nclients--;
	cout << "-disconnect\n";
	PRINTNUSERS
		closesocket(my_sock);
	return 0;
}