#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")

int main(void) {
    WSADATA sockData;
    SOCKET clientSocket;
    struct sockaddr_in srvaddr;
    char serverIP[15] = "127.0.0.1";
    int serverPort = 1337;
    char userInput[1025];
    char srvResponse[1025] = { '\0' };
    int bytesRead;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &sockData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    // Create socket
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Failed to create socket\n");
        WSACleanup();
        return 1;
    }

    // Set up server address
    memset(&srvaddr, 0, sizeof(srvaddr));
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverIP, &srvaddr.sin_addr) <= 0) {
        printf("Invalid address/Address not supported\n");
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Connect to server
    if (connect(clientSocket, (struct sockaddr*)&srvaddr, sizeof(srvaddr)) < 0) {
        printf("Connection failed\n");
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Main loop for sending messages
    while (1) {
        printf("Enter a message to send across the network (type 'Done' to exit): ");
        fgets(userInput, 1024, stdin);
        //scanf("%1025s", userInput);

        // Remove newline character
        userInput[strcspn(userInput, "\n")] = '\0';

        // Check for exit condition
        if (strcmp(userInput, "Done") == 0)
            break;

        // Send message to server
        if (send(clientSocket, userInput, strlen(userInput), 0) == SOCKET_ERROR) {
            printf("Send failed\n");
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }

        // Receive response from server
        bytesRead = recv(clientSocket, srvResponse, 1024, 0);
        if (bytesRead == SOCKET_ERROR) {
            printf("Receive failed\n");
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }

        // Null-terminate the received data
        //srvResponse[bytesRead] = '\0';
        // Check if the received response is an MD5 hash
        if (strncmp(userInput, "md5", 3) == 0) {
            // Print the hash
            printf("Server Response: ");
            for (int i = 0; i < 16; i++) {
                printf("%02X", srvResponse[i]);
            }
            printf("\n");
        } else if (strncmp(userInput, "sha1", 4) == 0) {
            // Print the hash
            printf("Server Response: ");
            for (int i = 0; i < 20; i++) {
                printf("%02X", srvResponse[i]);
            }
            printf("\n");
        } else if (strncmp(userInput, "sha256", 6) == 0) {
            // Print the hash
            printf("Server Response: ");
            for (int i = 0; i < 32; i++) {
                printf("%02X", srvResponse[i]);
            }
            printf("\n");
        }
        else {
            // Handle other responses differently
            printf("Server response: %s\n", srvResponse);
        }
        memset(srvResponse, '\0', 1025);
    }

    // Close socket and cleanup
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}