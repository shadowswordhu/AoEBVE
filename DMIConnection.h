#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_BUFLEN 512
#define DMI_ADDRESS "192.168.178.61"
#define DEFAULT_PORT "4444"


class DMIConnection
{
private:
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
    const char* sendbuf = "this is a test";
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;


public:

    // Constructor
    // Initialize Winsock
    DMIConnection() {
        

        iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            printf("WSAStartup failed with error: %d\n", iResult);
            exit(-1);
        }

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;


        // Resolve the server address and port
        iResult = getaddrinfo(DMI_ADDRESS, DEFAULT_PORT, &hints, &result);
        if (iResult != 0) {
            printf("getaddrinfo failed with error: %d\n", iResult);
            WSACleanup();
            exit(-1);
        }
        
    }

    void Connect() {
        // Attempt to connect to an address until one succeeds
        for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

            // Create a SOCKET for connecting to server
            ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
                ptr->ai_protocol);
            if (ConnectSocket == INVALID_SOCKET) {
                printf("socket failed with error: %ld\n", WSAGetLastError());
                OutputDebugStringA("Connect(): Socket failed!\n");
                WSACleanup();
                exit(-1);
            }

            // Connect to server.
            iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
            if (iResult == SOCKET_ERROR) {
                closesocket(ConnectSocket);
                ConnectSocket = INVALID_SOCKET;
                continue;
            }
            break;
        }

        freeaddrinfo(result);

        if (ConnectSocket == INVALID_SOCKET) {
            printf("Unable to connect to server!\n");
            OutputDebugStringA("Connect(): Unable to connect to server!\n");
            WSACleanup();
            exit(-1);
        }

    }

    ~DMIConnection() {
        iResult = shutdown(ConnectSocket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            printf("shutdown failed with error: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            exit(-1);
        }

        // Receive until the peer closes the connection
        /*
        do {

            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            if (iResult > 0)
                printf("Bytes received: %d\n", iResult);
            else if (iResult == 0)
                printf("Connection closed\n");
            else
                printf("recv failed with error: %d\n", WSAGetLastError());

        } while (iResult > 0);
        */

        // cleanup
        closesocket(ConnectSocket);
        WSACleanup();

    }


    void updateSpeed(float Speed) {
        short message = (short)roundf(Speed * 10);


        iResult = send(ConnectSocket, (char*)&message, (int)sizeof(message), 0);
        OutputDebugStringA("updateSpeed(): invoke\n");
        if (iResult == SOCKET_ERROR) {
            printf("send failed with error: %d\n", WSAGetLastError());
            OutputDebugStringA("updateSpeed(): Socket error!\n");
            closesocket(ConnectSocket);
            WSACleanup();
            exit(167);
        }

    }


};

