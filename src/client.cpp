#include <iostream>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

int clientSend(SOCKET& clientSocket, char* allias);
int clientRecv(SOCKET& clientSocket);

int client(char** argv, char* allias){

    int iResult;
    WSADATA wsaData;

    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0){
        std::cout << "Error initializing WSA: " << iResult;
        return 1;
    }

    // Declaring the addrinfo structures
    struct addrinfo *result = NULL;
    struct addrinfo *ptr = NULL;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    // Cleaning the garbage is important!!

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;


    iResult = getaddrinfo(argv[2], argv[3], &hints, &result);
    if (iResult != 0){
        std::cout << "getaddrinfo failed: " << iResult << std::endl;
        WSACleanup();
        return 1;
    }

    // Creating the SOCKET object for the server
    SOCKET clientSocket = INVALID_SOCKET;
    clientSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (clientSocket == INVALID_SOCKET){
        std::cout << "Invalid socket: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    
    // Binding the socket
    iResult = connect(clientSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR){
        std::cout << "Connect failed: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    else{
        std::cout << "Connected to the address: " << argv[2] << " at port: " << argv[3] << std::endl;
        freeaddrinfo(result);
    }

    // opening threads to recieve and send
    std::thread recvThread(clientRecv, std::ref(clientSocket));
    std::thread sendThread(clientSend, std::ref(clientSocket),  allias);

    recvThread.join();
    sendThread.join();

    // terminate the existing sockets
    closesocket(clientSocket);
    WSACleanup();

    std::cin.get();
    return 0;

}