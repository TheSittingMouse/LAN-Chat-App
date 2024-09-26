#include <iostream>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <thread>
#include <mutex>

int clientAccepting(SOCKET& serverSocket, std::vector<SOCKET>& clients, bool& run);
int serverRecv(SOCKET& currentClient, std::vector<SOCKET>& clients, bool& run);

int maxClients = 5;
const int serverBufLen = 512;

int server(char** argv){
    int iResult;
    WSADATA wsaData;

    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0){
        std::cout << "Error at WSAStartup: " << iResult << "\n";
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
    SOCKET serverSocket = INVALID_SOCKET;
    serverSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (serverSocket == INVALID_SOCKET){
        std::cout << "Invalid socket: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // making the serverSocket non-blocking??
    u_long mode = 1;
    ioctlsocket(serverSocket, FIONBIO, &mode);

    // Binding the socket
    iResult = bind(serverSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR){
        std::cout << "Binding failed: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    else{
        std::cout << "Server is bound to the address: " << argv[2] << " at port: " << argv[3] << std::endl;
        freeaddrinfo(result);
    }

    // Listening on the opened port
    iResult = listen(serverSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR){
        std::cout << "Listen failed: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    else{
        std::cout << "Socket is listening on the the address: " << argv[2] << " at port: " << argv[3] << std::endl;
    }

    //client accepting condition
    bool run = true;

    // setting up a vector for clients
    std::vector<SOCKET> clients(maxClients, INVALID_SOCKET);

    std::thread acceptThead(clientAccepting, std::ref(serverSocket), std::ref(clients), std::ref(run));
    //clientAccepting(serverSocket, clients, run);

    const int serverCommandBufLen = 20;
    char serverCommandBuffer[serverCommandBufLen];
    ZeroMemory(serverCommandBuffer, serverCommandBufLen);

    char serverBuf[serverBufLen];
    ZeroMemory(serverBuf, serverBufLen);

    // creating a vector of threads for recieving from clients
    std::vector<std::thread> threadVector;
    for (int i = 0; i < maxClients; ++i){
        std::thread th(serverRecv, std::ref(clients[i]), std::ref(clients), std::ref(run));
        threadVector.push_back(std::move(th));
    }

    std::cin.getline(serverCommandBuffer, serverCommandBufLen);

    if (strcmp(serverCommandBuffer, "end") == 0){
        run = false;
    }
    
    acceptThead.join();
    for (std::thread &x: threadVector){
        x.join();
    }

    std::cout << "Closing server...\n";
    closesocket(serverSocket);
    for (SOCKET &s: clients){
        if (s != INVALID_SOCKET){
            closesocket(s);
        }
    }

    WSACleanup();
    std::cin.get();
    return 0;
}
