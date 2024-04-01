#include <stdio.h>
#include <string.h>
#include <process.h>
#include <WinSock2.h> //Sockets
#include <wincrypt.h> //Hashing
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Crypt32.lib")

int repeat(int num, SOCKET clientConn, char message[]) { //private function for repeat command
	for (int i = 0; i < num; i++) {
		send(clientConn, message, strlen(message), 0);
	}
	return 0;
}

char hashBuffer[1024]; //for hashing functions

int __stdcall HandleClient(SOCKET* clientSocket) {
	SOCKET clientConn = *clientSocket;
	char response[1024] = { 0 };
	char socketData[1024] = { 0 }; //1k of data
	while (TRUE) {
		if (recv(clientConn, socketData, sizeof(socketData), 0)) {
			printf("Client sent: %s\n", socketData); //testing
			char* command = (char*)malloc(10 * sizeof(char));
			char* message = (char*)malloc(1024 * sizeof(char));
			sscanf_s(socketData, "%9s %1023[^\n]", command, 10, message, 1024);

			printf("Command: %s\n", command);
			printf("Message: %s\n", message);
			//different commands
			if (strcmp(command, "echo") == 0) { //echo command
				printf("Echo Command Found\n");
				send(clientConn, message, sizeof(message), 0);
			}
			else if (strcmp(command, "repeat2") == 0) { //repeat 2-9 commands
				printf("Repeat2 Command Found\n");
				repeat(2, clientConn, message);
			}
			else if (strcmp(command, "repeat3") == 0) {
				printf("Repeat3 Command Found\n");
				repeat(3, clientConn, message);
			}
			else if (strcmp(command, "repeat4") == 0) {
				printf("Repeat4 Command Found\n");
				repeat(4, clientConn, message);
			}
			else if (strcmp(command, "repeat5") == 0) {
				printf("Repeat5 Command Found\n");
				repeat(5, clientConn, message);
			}
			else if (strcmp(command, "repeat6") == 0) {
				printf("Repeat6 Command Found\n");
				repeat(6, clientConn, message);
			}
			else if (strcmp(command, "repeat7") == 0) {
				printf("Repeat7 Command Found\n");
				repeat(7, clientConn, message);
			}
			else if (strcmp(command, "repeat8") == 0) {
				printf("Repeat8 Command Found\n");
				repeat(8, clientConn, message);
			}
			else if (strcmp(command, "repeat9") == 0) {
				printf("Repeat9 Command Found\n");
				repeat(9, clientConn, message);
			}
			else if (strcmp(command, "length") == 0) { //length command
				printf("Length Command Found\n");
				char lengthStr[1024];
				sprintf_s(lengthStr, "Length: %d", (int)strlen(message));
				send(clientConn, lengthStr, strlen(lengthStr), 0);
			}
			else if (strcmp(command, "md5") == 0) {
				printf("MD5 Command Found\n");
				HCRYPTPROV hProv;
				HCRYPTHASH hHash;
				CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, 0);
				CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash);
				CryptHashData(hHash, (BYTE*)message, strlen(message), 0);
				DWORD dwHashLen = 16; // default MD5 hash length
				CryptGetHashParam(hHash, HP_HASHVAL, (BYTE*)hashBuffer, &dwHashLen, 0);
				CryptDestroyHash(hHash);
				CryptReleaseContext(hProv, 0);
				printf("Hash: %s\n", hashBuffer);
				send(clientConn, hashBuffer, strlen(hashBuffer), 0);
			}
			else if (strcmp(command, "sha1") == 0) {
				printf("SHA1 Command Found\n");
				HCRYPTPROV hProv;
				HCRYPTHASH hHash;
				CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, 0);
				CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash);
				CryptHashData(hHash, (BYTE*)message, strlen(message), 0);
				DWORD dwHashLen = 16; // default MD5 hash length
				CryptGetHashParam(hHash, HP_HASHVAL, (BYTE*)hashBuffer, &dwHashLen, 0);
				CryptDestroyHash(hHash);
				CryptReleaseContext(hProv, 0);
				printf("Hash: %s\n", hashBuffer);
				send(clientConn, hashBuffer, strlen(hashBuffer), 0);
			}
			else if (strcmp(command, "sha256") == 0) {
				printf("SHA256 Command Found\n");
				HCRYPTPROV hProv;
				HCRYPTHASH hHash;
				CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, 0);
				CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash);
				CryptHashData(hHash, (BYTE*)message, strlen(message), 0);
				DWORD dwHashLen = 16; // default MD5 hash length
				CryptGetHashParam(hHash, HP_HASHVAL, (BYTE*)hashBuffer, &dwHashLen, 0);
				CryptDestroyHash(hHash);
				CryptReleaseContext(hProv, 0);
				printf("Hash: %s\n", hashBuffer);
				send(clientConn, hashBuffer, strlen(hashBuffer), 0);
			}
			else { //if command doesn't exist
				char errorMsg[1024];
				sprintf_s(errorMsg, "Invalid Command: %s", command);
				send(clientConn, errorMsg, strlen(errorMsg), 0);
			}
			free(command);
			free(message);
		}
	}
	closesocket(clientConn);
	_endthreadex(0);
	return 0;
}

int main(void) {
	//initialization
	WSADATA winSockData;
	SOCKET serverSocket, clientSocket;
	struct sockaddr_in serverAddress, clientAddress;
	int addrLen = sizeof(clientAddress);
	HANDLE thread;
	int result;
	int port = 1337;

	//winSock2
	result = WSAStartup(MAKEWORD(2, 2), &winSockData);
	if (result != 0) {
		printf("Winsock failed to initialize\n");
		return 1;
	}

	//sockets
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET) {
		printf("Error creating server socket\n");
		WSACleanup();
		return 1;
	}

	//addressing
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(port);
	bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	listen(serverSocket, 3);

	//infinite loop listening for clients
	while (TRUE) {
		clientSocket = accept(serverSocket, &clientAddress, &addrLen);
		if (clientSocket == INVALID_SOCKET) {
			printf("Accept failed with error: %d\n", WSAGetLastError());
			closesocket(serverSocket);
			WSACleanup();
			return 1;
		}

		thread = (HANDLE)_beginthreadex(NULL, 0, &HandleClient, &clientSocket, 0, NULL);
		if (thread == NULL) {
			printf("Failed to create thread\n");
			closesocket(clientSocket);
		}
	}

	//finish
	closesocket(serverSocket);
	WSACleanup();
	return 0;
}